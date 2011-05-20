#include "environmentsettingsdialog.h"
#include "ui_environmentsettingsdialog.h"

#include "main.h"

#include <QSettings>

static const char* debuggerUpdateRateMsgs[] =
{
   "Update only on emulation pause.",
   "Update at emulation framerate (50 times a second for PAL, 60 times a second for NTSC).",
   "Update once a second during emulation.",
};

static const char* soundBufferDepthMsgs[] =
{
   "1024 samples",
   "2048 samples",
   "3072 samples",
   "4096 samples",
   "5120 samples",
   "6144 samples",
   "7168 samples",
   "8192 samples"
};

static const char* exampleText =
   ".proc reset\n"
   "\tsei\t\t; disable interrupts\n"
   "\tcld\t\t; safety - disable missing 'decimal' mode\n"
   "\tjmp start\t; skip this string\n"
   "\t.asciiz \"This is a string\"\n"
   "start:\n"
   "  lda $2002\t\t; read PPU status to check for NMI\n";

EnvironmentSettingsDialog::EnvironmentSettingsDialog(QWidget* parent) :
   QDialog(parent),
   ui(new Ui::EnvironmentSettingsDialog)
{
   QSettings settings;
   int style;

   ui->setupUi(this);

   m_scintilla = new QsciScintilla();

   m_scintilla->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

   m_scintilla->setMarginWidth(3,0);
   m_scintilla->setMarginMarkerMask(3,0);
   m_scintilla->setMarginWidth(4,0);
   m_scintilla->setMarginMarkerMask(4,0);

   m_scintilla->setMarginWidth(2,22);
   m_scintilla->setMarginMarkerMask(2,QsciScintilla::SymbolMargin);
   m_scintilla->setMarginSensitivity(2,true);
   m_scintilla->setMarginMarkerMask(2,0xFFFFFFF0);
   m_scintilla->setFolding(QsciScintilla::BoxedTreeFoldStyle,2);

   m_scintilla->setMarginWidth(Margin_Decorations,22);
   m_scintilla->setMarginMarkerMask(Margin_Decorations,0x0000000F);
   m_scintilla->setMarginType(Margin_Decorations,QsciScintilla::SymbolMargin);
   m_scintilla->setMarginSensitivity(Margin_Decorations,true);

   m_scintilla->setMarginLineNumbers(Margin_LineNumbers,true);
   m_scintilla->setMarginWidth(Margin_LineNumbers,10);
   m_scintilla->setMarginMarkerMask(Margin_LineNumbers,0);
   m_scintilla->setMarginType(Margin_LineNumbers,QsciScintilla::NumberMargin);
   m_scintilla->setMarginSensitivity(Margin_LineNumbers,true);

   m_scintilla->setSelectionBackgroundColor(QColor(215,215,215));
   m_scintilla->setSelectionToEol(true);

   m_scintilla->markerDefine(QPixmap(":/resources/22_execution_pointer.png"),Marker_Execution);
   m_scintilla->markerDefine(QPixmap(":/resources/22_breakpoint.png"),Marker_Breakpoint);
   m_scintilla->markerDefine(QPixmap(":/resources/22_breakpoint_disabled.png"),Marker_BreakpointDisabled);
   m_scintilla->markerDefine(QPixmap(":/resources/error-mark.svg"),Marker_Error);
   m_scintilla->setMarkerForegroundColor(QColor(255,255,0),Marker_Error);
   m_scintilla->setMarkerBackgroundColor(QColor(255,0,0),Marker_Error);
   m_scintilla->markerDefine(QsciScintilla::Background,Marker_Highlight);
   m_scintilla->setMarkerBackgroundColor(QColor(235,235,235),Marker_Highlight);

   m_lexer = new QsciLexerCA65(m_scintilla);
   m_scintilla->setLexer(m_lexer);

   m_lexer->readSettings(settings,"CodeEditor");

   m_scintilla->setText(exampleText);

   ui->example->addWidget(m_scintilla);

   style = 0;
   while ( !m_lexer->description(style).isEmpty() )
   {
      ui->styleName->addItem(m_lexer->description(style));
      style++;
   }
   on_styleName_currentIndexChanged(ui->styleName->currentIndex());

   ui->showWelcomeOnStart->setChecked(settings.value(ui->showWelcomeOnStart->objectName(),true).toBool());
   ui->saveAllOnCompile->setChecked(settings.value(ui->saveAllOnCompile->objectName(),true).toBool());
   ui->rememberWindowSettings->setChecked(settings.value(ui->rememberWindowSettings->objectName(),true).toBool());
   ui->trackRecentProjects->setChecked(settings.value(ui->trackRecentProjects->objectName(),true).toBool());

   ui->useInternalDB->setChecked(settings.value(ui->useInternalDB->objectName(),true).toBool());
   ui->GameDatabasePathEdit->setText(settings.value("GameDatabase",QVariant("")).toString());
   ui->GameDatabasePathButton->setEnabled(!ui->useInternalDB->isChecked());
   ui->GameDatabasePathEdit->setEnabled(!ui->useInternalDB->isChecked());

   ui->GameDatabase->setText(gameDatabase.getGameDBAuthor()+", "+gameDatabase.getGameDBTimestamp());

   ui->ROMPath->setText(settings.value("ROMPath","").toString());
   ui->runRom->setChecked(settings.value("runRom",QVariant(true)).toBool());
   ui->followExecution->setChecked(settings.value("followExecution",QVariant(true)).toBool());

   switch ( settings.value("debuggerUpdateRate",QVariant(0)).toInt() )
   {
      case 0:
         ui->debuggerUpdateRate->setValue(0);
      break;
      case -1:
         ui->debuggerUpdateRate->setValue(2);
      break;
      case 1:
         ui->debuggerUpdateRate->setValue(1);
      break;
      default:
         ui->debuggerUpdateRate->setValue(2);
      break;
   }
   ui->debuggerUpdateRateMsg->setText(debuggerUpdateRateMsgs[ui->debuggerUpdateRate->value()]);

   ui->soundBufferDepth->setValue(settings.value("soundBufferDepth",QVariant(1024)).toInt() );;
   ui->soundBufferDepthMsg->setText(soundBufferDepthMsgs[(ui->soundBufferDepth->value()/1024)-1]);
}

EnvironmentSettingsDialog::~EnvironmentSettingsDialog()
{
   delete m_scintilla;
   delete m_lexer;

   delete ui;
}

void EnvironmentSettingsDialog::on_buttonBox_accepted()
{
    QSettings settings;

    settings.setValue(ui->showWelcomeOnStart->objectName(),ui->showWelcomeOnStart->isChecked());
    settings.setValue(ui->saveAllOnCompile->objectName(),ui->saveAllOnCompile->isChecked());
    settings.setValue(ui->rememberWindowSettings->objectName(),ui->rememberWindowSettings->isChecked());
    settings.setValue(ui->trackRecentProjects->objectName(),ui->trackRecentProjects->isChecked());

    settings.setValue(ui->useInternalDB->objectName(),ui->useInternalDB->isChecked());
    settings.setValue("GameDatabase",ui->GameDatabasePathEdit->text());

    settings.setValue("ROMPath",ui->ROMPath->text());

    settings.setValue("runRom",ui->runRom->isChecked());
    settings.setValue("followExecution",ui->followExecution->isChecked());

    switch ( ui->debuggerUpdateRate->value() )
    {
       case 0:
          settings.setValue("debuggerUpdateRate",0);
       break;
       case 1:
          settings.setValue("debuggerUpdateRate",1);
       break;
       case 2:
          settings.setValue("debuggerUpdateRate",-1);
       break;
    }

    settings.setValue("soundBufferDepth",ui->soundBufferDepth->value());
}

void EnvironmentSettingsDialog::changeEvent(QEvent* e)
{
   QDialog::changeEvent(e);

   switch (e->type())
   {
      case QEvent::LanguageChange:
         ui->retranslateUi(this);
         break;
      default:
         break;
   }
}

#if 0
QColor EnvironmentSettingsDialog::getIdealTextColor(const QColor& rBackgroundColor) const
{
   const int THRESHOLD = 105;
   int BackgroundDelta = (rBackgroundColor.red() * 0.299) + (rBackgroundColor.green() * 0.587) + (rBackgroundColor.blue() * 0.114);
   return QColor((255- BackgroundDelta < THRESHOLD) ? Qt::black : Qt::white);
}

void EnvironmentSettingsDialog::on_CodeBackgroundButton_clicked()
{
   QColorDialog* dlg = new QColorDialog(this);

   if (dlg->exec() == QColorDialog::Accepted)
   {
      QColor chosenColor= dlg->selectedColor();
      QString COLOR_STYLE("QPushButton { background-color : %1; color : %2; }");
      QColor idealTextColor = getIdealTextColor(chosenColor);
      this->ui->CodeBackgroundButton->setStyleSheet(COLOR_STYLE.arg(chosenColor.name()).arg(idealTextColor.name()));
   }

   delete dlg;
}
#endif

void EnvironmentSettingsDialog::on_PluginPathButton_clicked()
{
   QString value = QFileDialog::getExistingDirectory(this, "Plugin Path", ui->PluginPathEdit->text());

   if (!value.isEmpty())
   {
      ui->PluginPathEdit->setText(value);
   }
}

void EnvironmentSettingsDialog::on_useInternalDB_toggled(bool checked)
{
   ui->GameDatabasePathButton->setEnabled(!checked);
   ui->GameDatabasePathEdit->setEnabled(!checked);

   if ( (!checked) && (!ui->GameDatabasePathEdit->text().isEmpty()) )
   {
      bool openedOk = gameDatabase.initialize(ui->GameDatabasePathEdit->text());

      ui->GameDatabasePathButton->setEnabled(openedOk);
      ui->GameDatabasePathEdit->setEnabled(openedOk);

      ui->GameDatabase->setText(gameDatabase.getGameDBAuthor()+", "+gameDatabase.getGameDBTimestamp());
   }
   else if ( checked )
   {
      bool openedOk = gameDatabase.initialize(":GameDatabase");

      ui->GameDatabasePathButton->setEnabled(openedOk);
      ui->GameDatabasePathEdit->setEnabled(openedOk);

      ui->GameDatabase->setText(gameDatabase.getGameDBAuthor()+", "+gameDatabase.getGameDBTimestamp());
   }
}

void EnvironmentSettingsDialog::on_GameDatabasePathButton_clicked()
{
   QString value = QFileDialog::getOpenFileName(this,"Game Database",QDir::currentPath(),"XML Files (*.xml)");

   if (!value.isEmpty())
   {
      ui->GameDatabasePathEdit->setText(value);

      bool openedOk = gameDatabase.initialize(ui->GameDatabasePathEdit->text());

      ui->useInternalDB->setChecked(!openedOk);
      ui->GameDatabasePathButton->setEnabled(openedOk);
      ui->GameDatabasePathEdit->setEnabled(openedOk);

      ui->GameDatabase->setText(gameDatabase.getGameDBAuthor()+", "+gameDatabase.getGameDBTimestamp());
   }
}

void EnvironmentSettingsDialog::on_ROMPathBrowse_clicked()
{
   QString value = QFileDialog::getExistingDirectory(this,"ROM Path",ui->ROMPath->text());

   if ( !value.isEmpty() )
   {
      ui->ROMPath->setText(value);
   }
}

void EnvironmentSettingsDialog::on_debuggerUpdateRate_sliderMoved(int position)
{
   ui->debuggerUpdateRateMsg->setText(debuggerUpdateRateMsgs[position]);
}

void EnvironmentSettingsDialog::on_debuggerUpdateRate_valueChanged(int value)
{
   ui->debuggerUpdateRateMsg->setText(debuggerUpdateRateMsgs[value]);
}

void EnvironmentSettingsDialog::on_soundBufferDepth_sliderMoved(int position)
{
   ui->soundBufferDepthMsg->setText(soundBufferDepthMsgs[(position/1024)-1]);
}

void EnvironmentSettingsDialog::on_soundBufferDepth_valueChanged(int value)
{
   ui->soundBufferDepthMsg->setText(soundBufferDepthMsgs[(value/1024)-1]);
}

void EnvironmentSettingsDialog::on_styleName_currentIndexChanged(int index)
{
   QFont font = m_lexer->font(index);
   ui->styleFont->setCurrentFont(font);
   ui->fontBold->setChecked(font.bold());
   ui->fontItalic->setChecked(font.italic());
   ui->fontUnderline->setChecked(font.underline());
}

void EnvironmentSettingsDialog::on_fontBold_toggled(bool checked)
{
   QSettings settings;
   int style = ui->styleName->currentIndex();
   QFont font = m_lexer->font(style);
   font.setBold(checked);
   m_lexer->setFont(font,style);
   m_lexer->writeSettings(settings,"CodeEditor");
}

void EnvironmentSettingsDialog::on_fontItalic_toggled(bool checked)
{
   QSettings settings;
   int style = ui->styleName->currentIndex();
   QFont font = m_lexer->font(style);
   font.setItalic(checked);
   m_lexer->setFont(font,style);
   m_lexer->writeSettings(settings,"CodeEditor");
}

void EnvironmentSettingsDialog::on_fontUnderline_toggled(bool checked)
{
   QSettings settings;
   int style = ui->styleName->currentIndex();
   QFont font = m_lexer->font(style);
   font.setUnderline(checked);
   m_lexer->setFont(font,style);
   m_lexer->writeSettings(settings,"CodeEditor");
}

void EnvironmentSettingsDialog::on_styleFont_currentIndexChanged(QString fontName)
{
   QSettings settings;
   int style = ui->styleName->currentIndex();
   QFont font = ui->styleFont->currentFont();
   font.setBold(ui->fontBold->isChecked());
   font.setItalic(ui->fontItalic->isChecked());
   font.setUnderline(ui->fontUnderline->isChecked());
   m_lexer->setFont(font,style);
   m_lexer->writeSettings(settings,"CodeEditor");
}
