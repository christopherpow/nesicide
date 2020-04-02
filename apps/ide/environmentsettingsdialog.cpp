#include "environmentsettingsdialog.h"
#include "ui_environmentsettingsdialog.h"

#include "cnesicideproject.h"

#include "Qsci/qsciscintilla.h"
#include "cgamedatabasehandler.h"

#include "codeeditorform.h"

#include <QSettings>

// Settings data structures.
//QModelIndex EnvironmentSettingsDialog::m_lastActiveTab;
bool EnvironmentSettingsDialog::m_useInternalGameDatabase;
QString EnvironmentSettingsDialog::m_gameDatabase;
bool EnvironmentSettingsDialog::m_showWelcomeOnStart;
bool EnvironmentSettingsDialog::m_saveAllOnCompile;
bool EnvironmentSettingsDialog::m_rememberWindowSettings;
bool EnvironmentSettingsDialog::m_trackRecentProjects;
QString EnvironmentSettingsDialog::m_romPath;
bool EnvironmentSettingsDialog::m_runRomOnLoad;
bool EnvironmentSettingsDialog::m_useTabBarInEditorArea;
bool EnvironmentSettingsDialog::m_followExecution;
int EnvironmentSettingsDialog::m_debuggerUpdateRate;
int EnvironmentSettingsDialog::m_soundBufferDepth;
QColor EnvironmentSettingsDialog::m_marginBackgroundColor;
QColor EnvironmentSettingsDialog::m_marginForegroundColor;
bool EnvironmentSettingsDialog::m_lineNumbersEnabled;
QColor EnvironmentSettingsDialog::m_highlightBarColor;
bool EnvironmentSettingsDialog::m_highlightBarEnabled;
bool EnvironmentSettingsDialog::m_showSymbolTips;
bool EnvironmentSettingsDialog::m_showOpcodeTips;
bool EnvironmentSettingsDialog::m_autoIndentEnabled;
bool EnvironmentSettingsDialog::m_tabReplacementEnabled;
int EnvironmentSettingsDialog::m_spacesForTabs;
bool EnvironmentSettingsDialog::m_annotateSource;
bool EnvironmentSettingsDialog::m_foldSource;
bool EnvironmentSettingsDialog::m_textEncodingIsUtf8;
QString EnvironmentSettingsDialog::m_cSourceExtensions;
QString EnvironmentSettingsDialog::m_asmSourceExtensions;
QString EnvironmentSettingsDialog::m_headerExtensions;
QString EnvironmentSettingsDialog::m_customExtensions;
QString EnvironmentSettingsDialog::m_highlightAsC;
QString EnvironmentSettingsDialog::m_highlightAsASM;
int EnvironmentSettingsDialog::m_eolMode;
bool EnvironmentSettingsDialog::m_eolForceConsistent;
QColor EnvironmentSettingsDialog::m_caretColor;

static const char* sourceExtensionListC = ".c .c65";
static const char* sourceExtensionListAsm = ".a .asm .a65 .s .s65";
static const char* headerExtensionList = ".h .inc";
static const char* customExtensionList = ".uc";

static const char* highlightAsCList = ".c .c65 .h";
static const char* highlightAsASMList = ".a .asm .a65 .s .s65 .inc";

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
   "  lda $2002\t\t; read PPU status to check for NMI";

EnvironmentSettingsDialog::EnvironmentSettingsDialog(QWidget* parent) :
   QDialog(parent),
   ui(new Ui::EnvironmentSettingsDialog)
{
   ui->setupUi(this);

   readSettings();

   QSettings settings(QSettings::IniFormat, QSettings::UserScope, "CSPSoftware", "NESICIDE");

   ui->settingsLocation->setText(QString("Currently reading settings from: ")+settings.fileName());

   QFileInfo toolchainInfo(qgetenv("CC65_HOME"));
   if ( toolchainInfo.exists() &&
        toolchainInfo.isDir() )
   {
     ui->toolchainLocation->setText(qgetenv("CC65_HOME"));
   }

   m_scintilla = new QsciScintilla();
   m_defaultLexer = new QsciLexerDefault();
   m_cc65Lexer = new QsciLexerCC65();
   m_ca65Lexer = new QsciLexerCA65();
   m_lexer = m_defaultLexer;

   setupCodeEditor(0);

   ui->example->addWidget(m_scintilla);
   adjustSize();

   ui->showWelcomeOnStart->setChecked(m_showWelcomeOnStart);
   ui->saveAllOnCompile->setChecked(m_saveAllOnCompile);
   ui->rememberWindowSettings->setChecked(m_rememberWindowSettings);
   ui->trackRecentProjects->setChecked(m_trackRecentProjects);

   ui->useInternalDB->setChecked(m_useInternalGameDatabase);
   ui->GameDatabasePathEdit->setText(m_gameDatabase);
   ui->GameDatabasePathButton->setEnabled(!m_useInternalGameDatabase);
   ui->GameDatabasePathEdit->setEnabled(!m_useInternalGameDatabase);

   ui->GameDatabase->setText(CGameDatabaseHandler::instance()->getGameDBAuthor()+", "+CGameDatabaseHandler::instance()->getGameDBTimestamp());

   ui->ROMPath->setText(m_romPath);
   ui->runRom->setChecked(m_runRomOnLoad);
   ui->followExecution->setChecked(m_followExecution);
   ui->useTabBarInEditorArea->setChecked(m_useTabBarInEditorArea);

   switch ( m_debuggerUpdateRate )
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

   ui->soundBufferDepth->setValue(m_soundBufferDepth);
   ui->soundBufferDepthMsg->setText(soundBufferDepthMsgs[(m_soundBufferDepth/1024)-1]);

   ui->showHighlightBar->setChecked(m_highlightBarEnabled);
   ui->showLineNumberMargin->setChecked(m_lineNumbersEnabled);

   ui->autoIndent->setChecked(m_autoIndentEnabled);
   ui->replaceTabs->setChecked(m_tabReplacementEnabled);
   ui->spacesForTab->setValue(m_spacesForTabs);

   ui->annotate->setChecked(m_annotateSource);

   ui->fold->setChecked(m_foldSource);
   ui->textEncoding->setCurrentIndex(m_textEncodingIsUtf8);

   ui->showSymbolTips->setChecked(m_showSymbolTips);
   ui->showOpcodeTips->setChecked(m_showOpcodeTips);

   ui->sourceExtensionsC->setText(m_cSourceExtensions);
   ui->sourceExtensionsAsm->setText(m_asmSourceExtensions);
   ui->headerExtensions->setText(m_headerExtensions);
   ui->customExtensions->setText(m_customExtensions);
   ui->highlightAsC->setText(m_highlightAsC);
   ui->highlightAsASM->setText(m_highlightAsASM);

   ui->eolMode->setCurrentIndex(m_eolMode);
   ui->eolConsistent->setChecked(m_eolForceConsistent);

   pageMap.insert("General",ui->general);
   pageMap.insert("Code Editor",ui->codeeditor);
   pageMap.insert("Compiler",ui->compiler);
   pageMap.insert("Nintendo Entertainment System",ui->nesemulator);
   pageMap.insert("NES Emulation",ui->nesemulator);
   pageMap.insert("Debugging",ui->nesdebugger);
   pageMap.insert("Commodore 64",ui->c64emulator);
   pageMap.insert("C=64 Emulation",ui->c64emulator);

   ui->treeWidget->setCurrentItem(ui->treeWidget->findItems("General",Qt::MatchExactly).at(0));
   if ( !CNesicideProject::instance()->getProjectTarget().compare("nes",Qt::CaseInsensitive) )
   {
      ui->treeWidget->findItems("Commodore 64",Qt::MatchExactly).at(0)->setHidden(true);
   }
   else if ( !CNesicideProject::instance()->getProjectTarget().compare("c64",Qt::CaseInsensitive) )
   {
      ui->treeWidget->findItems("Nintendo Entertainment System",Qt::MatchExactly).at(0)->setHidden(true);
   }
}

EnvironmentSettingsDialog::~EnvironmentSettingsDialog()
{
   m_lexer = NULL;
   delete m_scintilla;
   delete m_defaultLexer;
   delete m_cc65Lexer;
   delete m_ca65Lexer;

   delete ui;
}

void EnvironmentSettingsDialog::readSettings()
{
   QSettings settings(QSettings::IniFormat, QSettings::UserScope, "CSPSoftware", "NESICIDE");

   settings.beginGroup("Environment");
   m_useInternalGameDatabase = settings.value("useInternalGameDB",QVariant(true)).toBool();
   m_gameDatabase = settings.value("GameDatabase").toString();
   m_showWelcomeOnStart = settings.value("showWelcomeOnStart",QVariant(true)).toBool();
   m_saveAllOnCompile = settings.value("saveAllOnCompile",QVariant(true)).toBool();
   m_rememberWindowSettings = settings.value("rememberWindowSettings",QVariant(true)).toBool();
   m_trackRecentProjects = settings.value("trackRecentProjects",QVariant(true)).toBool();
   m_romPath = settings.value("romPath").toString();
   m_runRomOnLoad = settings.value("runRomOnLoad",QVariant(false)).toBool();
   m_useTabBarInEditorArea = settings.value("useTabBarInEditorArea",QVariant(false)).toBool();
   m_followExecution = settings.value("followExecution",QVariant(true)).toBool();
   m_debuggerUpdateRate = settings.value("debuggerUpdateRate",QVariant(0)).toInt();
   m_soundBufferDepth = settings.value("soundBufferDepth",QVariant(1024)).toInt();
   if ( settings.contains("marginBackgroundColor") )
   {
      m_marginBackgroundColor = settings.value("marginBackgroundColor").value<QColor>();
   }
   else
   {
      m_marginBackgroundColor = QColor(235,235,235);
   }
   if ( settings.contains("marginForegroundColor") )
   {
      m_marginForegroundColor = settings.value("marginForegroundColor").value<QColor>();
   }
   else
   {
      m_marginForegroundColor = QColor(0,0,0);
   }
   if ( settings.contains("highlightBarColor") )
   {
      m_highlightBarColor = settings.value("highlightBarColor").value<QColor>();
   }
   else
   {
      m_highlightBarColor = QColor(240,240,240);
   }
   m_highlightBarEnabled = settings.value("highlightBarEnabled",QVariant(true)).toBool();
   m_lineNumbersEnabled = settings.value("lineNumbersEnabled",QVariant(true)).toBool();
   m_showSymbolTips = settings.value("showSymbolTips",QVariant(true)).toBool();
   m_showOpcodeTips = settings.value("showOpcodeTips",QVariant(true)).toBool();
   m_autoIndentEnabled = settings.value("autoIndent",QVariant(true)).toBool();
   m_tabReplacementEnabled = settings.value("tabReplacement",QVariant(true)).toBool();
   m_spacesForTabs = settings.value("spacesPerTab",QVariant(3)).toInt();
   m_annotateSource = settings.value("annotateSource",QVariant(true)).toBool();
   m_foldSource = settings.value("foldSource",QVariant(true)).toBool();
   m_textEncodingIsUtf8 = settings.value("textEncodingIsUtf8",QVariant(false)).toBool();
   m_cSourceExtensions = settings.value("SourceExtensionsC",QVariant(sourceExtensionListC)).toString();
   m_asmSourceExtensions = settings.value("SourceExtensionsAsm",QVariant(sourceExtensionListAsm)).toString();
   m_headerExtensions = settings.value("HeaderExtensions",QVariant(headerExtensionList)).toString();
   m_customExtensions = settings.value("CustomExtensions",QVariant(customExtensionList)).toString();
   m_highlightAsC = settings.value("HighlightAsC",QVariant(highlightAsCList)).toString();
   m_highlightAsASM = settings.value("HighlightAsASM",QVariant(highlightAsASMList)).toString();

#if defined(Q_OS_MAC) || defined(Q_OS_MACX) || defined(Q_OS_MAC64) || defined(Q_OS_LINUX)
//#if defined(Q_OS_MAC) || defined(Q_OS_MACX) || defined(Q_OS_MAC64)
//   m_eolMode = settings.value("EOLMode",QVariant(QsciScintilla::EolMac)).toInt();
//#endif
//#ifdef Q_OS_LINUX
   m_eolMode = settings.value("EOLMode",QVariant(QsciScintilla::EolUnix)).toInt();
#endif
#ifdef Q_OS_WIN
   m_eolMode = settings.value("EOLMode",QVariant(QsciScintilla::EolWindows)).toInt();
#endif
   m_eolForceConsistent = settings.value("EOLForceConsistent",QVariant(true)).toBool();
   if ( settings.contains("CaretColor") )
   {
      m_caretColor = settings.value("CaretColor").value<QColor>();
   }
   else
   {
      m_caretColor = QColor(0,0,0);
   }

//   m_lastActiveTab = settings.value("LastActiveTab",QVariant(0)).toInt();
   settings.endGroup();
}

void EnvironmentSettingsDialog::writeSettings()
{
   QSettings settings(QSettings::IniFormat, QSettings::UserScope, "CSPSoftware", "NESICIDE");

   // First save to locals.
   m_useInternalGameDatabase = ui->useInternalDB->isChecked();
   m_gameDatabase = ui->GameDatabasePathEdit->text();
   m_showWelcomeOnStart = ui->showWelcomeOnStart->isChecked();
   m_saveAllOnCompile = ui->saveAllOnCompile->isChecked();
   m_rememberWindowSettings = ui->rememberWindowSettings->isChecked();
   m_trackRecentProjects = ui->trackRecentProjects->isChecked();
   m_romPath = ui->ROMPath->text();
   m_runRomOnLoad = ui->runRom->isChecked();
   m_useTabBarInEditorArea = ui->useTabBarInEditorArea->isChecked();
   m_followExecution = ui->followExecution->isChecked();
   switch ( ui->debuggerUpdateRate->value() )
   {
      case 0:
         m_debuggerUpdateRate = 0;
      break;
      case 1:
         m_debuggerUpdateRate = 1;
      break;
      case 2:
         m_debuggerUpdateRate = -1;
      break;
   }
   m_soundBufferDepth = ui->soundBufferDepth->value();
   m_highlightBarEnabled = ui->showHighlightBar->isChecked();
   m_lineNumbersEnabled = ui->showLineNumberMargin->isChecked();
   m_showSymbolTips = ui->showSymbolTips->isChecked();
   m_showOpcodeTips = ui->showOpcodeTips->isChecked();
   m_autoIndentEnabled = ui->autoIndent->isChecked();
   m_tabReplacementEnabled = ui->replaceTabs->isChecked();
   m_spacesForTabs = ui->spacesForTab->value();
   m_annotateSource = ui->annotate->isChecked();
   m_foldSource = ui->fold->isChecked();
   m_textEncodingIsUtf8 = ui->textEncoding->currentIndex();
   m_cSourceExtensions = ui->sourceExtensionsC->text();
   m_asmSourceExtensions = ui->sourceExtensionsAsm->text();
   m_headerExtensions = ui->headerExtensions->text();
   m_customExtensions = ui->customExtensions->text();
   m_highlightAsC = ui->highlightAsC->text();
   m_highlightAsASM = ui->highlightAsASM->text();
   m_eolMode = ui->eolMode->currentIndex();
   m_eolForceConsistent = ui->eolConsistent->isChecked();

//   m_lastActiveTab = ui->treeWidget->currentIndex();

   // Then save to QSettings;
   settings.beginGroup("Environment");
   settings.setValue("showWelcomeOnStart",m_showWelcomeOnStart);
   settings.setValue("saveAllOnCompile",m_saveAllOnCompile);
   settings.setValue("rememberWindowSettings",m_rememberWindowSettings);
   settings.setValue("trackRecentProjects",m_trackRecentProjects);

   settings.setValue("useInternalGameDB",m_useInternalGameDatabase);
   settings.setValue("GameDatabase",m_gameDatabase);

   settings.setValue("romPath",m_romPath);

   settings.setValue("runRomOnLoad",m_runRomOnLoad);
   settings.setValue("followExecution",m_followExecution);
   settings.setValue("useTabBarInEditorArea",m_useTabBarInEditorArea);

   settings.setValue("debuggerUpdateRate",m_debuggerUpdateRate);

   settings.setValue("soundBufferDepth",m_soundBufferDepth);

   settings.setValue("marginBackgroundColor",m_marginBackgroundColor);
   settings.setValue("marginForegroundColor",m_marginForegroundColor);
   settings.setValue("highlightBarColor",m_highlightBarColor);
   settings.setValue("highlightBarEnabled",m_highlightBarEnabled);
   settings.setValue("lineNumbersEnabled",m_lineNumbersEnabled);
   settings.setValue("showSymbolTips",m_showSymbolTips);
   settings.setValue("showOpcodeTips",m_showOpcodeTips);
   settings.setValue("autoIndent",m_autoIndentEnabled);
   settings.setValue("tabReplacement",m_tabReplacementEnabled);
   settings.setValue("spacesPerTab",m_spacesForTabs);
   settings.setValue("annotateSource",m_annotateSource);
   settings.setValue("foldSource",m_foldSource);
   settings.setValue("textEncodingIsUtf8",m_textEncodingIsUtf8);

   settings.setValue("SourceExtensionsC",m_cSourceExtensions);
   settings.setValue("SourceExtensionsAsm",m_asmSourceExtensions);
   settings.setValue("HeaderExtensions",m_headerExtensions);
   settings.setValue("CustomExtensions",m_customExtensions);
   settings.setValue("HighlightAsC",m_highlightAsC);
   settings.setValue("HighlightAsASM",m_highlightAsASM);
   settings.setValue("EOLMode",m_eolMode);
   settings.setValue("EOLForceConsistent",m_eolForceConsistent);
   settings.setValue("CaretColor",m_caretColor);

//   settings.setValue("LastActiveTab",m_lastActiveTab);
   settings.endGroup();

   m_defaultLexer->writeSettings(settings,"CodeEditor");
   m_cc65Lexer->writeSettings(settings,"CodeEditor");
   m_ca65Lexer->writeSettings(settings,"CodeEditor");
}

void EnvironmentSettingsDialog::setupCodeEditor(int index)
{
   QSettings settings(QSettings::IniFormat, QSettings::UserScope, "CSPSoftware", "NESICIDE");
   int style;

   ui->styleName->clear();

   m_defaultLexer->readSettings(settings,"CodeEditor");
   m_cc65Lexer->readSettings(settings,"CodeEditor");
   m_ca65Lexer->readSettings(settings,"CodeEditor");

   // Clear current lexer first!
   m_scintilla->setLexer(NULL);

   switch ( index )
   {
   case 0:
      m_lexer = m_defaultLexer;
      break;
   case 1:
      m_lexer = m_ca65Lexer;
      break;
   case 2:
      m_lexer = m_cc65Lexer;
      break;
   }
   m_scintilla->setLexer(m_lexer);

   for ( style = 0; style < (1<<m_lexer->styleBitsNeeded()); style++ )
   {
      if ( !m_lexer->description(style).isEmpty() )
      {
         ui->styleName->addItem(m_lexer->description(style),style);
      }
   }
   on_styleName_currentIndexChanged(ui->styleName->currentIndex());

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
   m_scintilla->setMarginMarkerMask(Margin_LineNumbers,0);
   m_scintilla->setMarginType(Margin_LineNumbers,QsciScintilla::NumberMargin);
   m_scintilla->setMarginSensitivity(Margin_LineNumbers,true);
   if ( m_lineNumbersEnabled )
   {
      m_scintilla->setMarginWidth(Margin_LineNumbers,10);
   }
   else
   {
      m_scintilla->setMarginWidth(Margin_LineNumbers,0);
   }
   m_scintilla->setUtf8(m_textEncodingIsUtf8);

   m_scintilla->setSelectionBackgroundColor(QColor(215,215,215));
   m_scintilla->setSelectionToEol(true);

   m_scintilla->markerDefine(QPixmap(":/resources/22_execution_pointer.png"),Marker_Execution);
   m_scintilla->markerDefine(QPixmap(":/resources/22_breakpoint.png"),Marker_Breakpoint);
   m_scintilla->markerDefine(QPixmap(":/resources/22_breakpoint_disabled.png"),Marker_BreakpointDisabled);
   m_scintilla->markerDefine(QPixmap(":/resources/error-mark.png"),Marker_Error);
   m_scintilla->setMarkerForegroundColor(QColor(255,255,0),Marker_Error);
   m_scintilla->setMarkerBackgroundColor(QColor(255,0,0),Marker_Error);

   m_scintilla->markerDefine(QsciScintilla::Background,Marker_Highlight);
   if ( m_highlightBarEnabled )
   {
      m_scintilla->setMarkerBackgroundColor(m_highlightBarColor,Marker_Highlight);
   }
   else
   {
      // Set the highlight bar color to background to hide it.
      m_scintilla->setMarkerBackgroundColor(m_lexer->defaultPaper(),Marker_Highlight);
   }

   m_scintilla->setText(exampleText);

   m_scintilla->setMarginsBackgroundColor(m_marginBackgroundColor);
   m_scintilla->setMarginsForegroundColor(m_marginForegroundColor);
   m_scintilla->setMarginsFont(m_lexer->font(0)); // Always pick default font.

   m_scintilla->markerAdd(1,Marker_Execution);
   m_scintilla->markerAdd(1,Marker_Highlight);

   m_scintilla->setCaretForegroundColor(m_caretColor);
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
      bool openedOk = CGameDatabaseHandler::instance()->initialize(ui->GameDatabasePathEdit->text());

      ui->GameDatabasePathButton->setEnabled(openedOk);
      ui->GameDatabasePathEdit->setEnabled(openedOk);

      ui->GameDatabase->setText(CGameDatabaseHandler::instance()->getGameDBAuthor()+", "+CGameDatabaseHandler::instance()->getGameDBTimestamp());
   }
   else if ( checked )
   {
      bool openedOk = CGameDatabaseHandler::instance()->initialize(":GameDatabase");

      ui->GameDatabasePathButton->setEnabled(openedOk);
      ui->GameDatabasePathEdit->setEnabled(openedOk);

      ui->GameDatabase->setText(CGameDatabaseHandler::instance()->getGameDBAuthor()+", "+CGameDatabaseHandler::instance()->getGameDBTimestamp());
   }
}

void EnvironmentSettingsDialog::on_GameDatabasePathButton_clicked()
{
   QString value = QFileDialog::getOpenFileName(this,"Game Database",QDir::currentPath(),"XML Files (*.xml)");

   if (!value.isEmpty())
   {
      ui->GameDatabasePathEdit->setText(value);

      bool openedOk = CGameDatabaseHandler::instance()->initialize(ui->GameDatabasePathEdit->text());

      ui->useInternalDB->setChecked(!openedOk);
      ui->GameDatabasePathButton->setEnabled(openedOk);
      ui->GameDatabasePathEdit->setEnabled(openedOk);

      ui->GameDatabase->setText(CGameDatabaseHandler::instance()->getGameDBAuthor()+", "+CGameDatabaseHandler::instance()->getGameDBTimestamp());
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
   int style = ui->styleName->itemData(index).toInt();
   QFont font = m_lexer->font(style);

   ui->styleFont->setCurrentFont(font);
   ui->fontBold->setChecked(font.bold());
   ui->fontItalic->setChecked(font.italic());
   ui->fontUnderline->setChecked(font.underline());
   ui->fontSize->setValue(font.pointSize());
}

void EnvironmentSettingsDialog::on_fontBold_toggled(bool checked)
{
   QSettings settings(QSettings::IniFormat, QSettings::UserScope, "CSPSoftware", "NESICIDE");
   int style;

   if ( ui->applyAll->isChecked() )
   {
      for ( style = 0; style < (1<<m_lexer->styleBitsNeeded()); style++ )
      {
         if ( !m_lexer->description(style).isEmpty() )
         {
            QFont font = m_lexer->font(style);
            font.setBold(checked);
            m_lexer->setFont(font,style);
            if ( style == QsciLexerCA65::CA65_Default )
            {
               m_scintilla->setMarginsFont(font);
            }
         }
      }
   }
   else
   {
      style = ui->styleName->itemData(ui->styleName->currentIndex()).toInt();
      QFont font = m_lexer->font(style);
      font.setBold(checked);
      m_lexer->setFont(font,style);
      if ( style == QsciLexerCA65::CA65_Default )
      {
         m_scintilla->setMarginsFont(font);
      }
   }
}

void EnvironmentSettingsDialog::on_fontItalic_toggled(bool checked)
{
   QSettings settings(QSettings::IniFormat, QSettings::UserScope, "CSPSoftware", "NESICIDE");
   int style = ui->styleName->itemData(ui->styleName->currentIndex()).toInt();

   if ( ui->applyAll->isChecked() )
   {
      for ( style = 0; style < (1<<m_lexer->styleBitsNeeded()); style++ )
      {
         if ( !m_lexer->description(style).isEmpty() )
         {
            QFont font = m_lexer->font(style);
            font.setItalic(checked);
            m_lexer->setFont(font,style);
            if ( style == QsciLexerCA65::CA65_Default )
            {
               m_scintilla->setMarginsFont(font);
            }
         }
      }
   }
   else
   {
      style = ui->styleName->itemData(ui->styleName->currentIndex()).toInt();
      QFont font = m_lexer->font(style);
      font.setItalic(checked);
      m_lexer->setFont(font,style);
      if ( style == QsciLexerCA65::CA65_Default )
      {
         m_scintilla->setMarginsFont(font);
      }
   }
}

void EnvironmentSettingsDialog::on_fontUnderline_toggled(bool checked)
{
   QSettings settings(QSettings::IniFormat, QSettings::UserScope, "CSPSoftware", "NESICIDE");
   int style;

   if ( ui->applyAll->isChecked() )
   {
      for ( style = 0; style < (1<<m_lexer->styleBitsNeeded()); style++ )
      {
         if ( !m_lexer->description(style).isEmpty() )
         {
            QFont font = m_lexer->font(style);
            font.setUnderline(checked);
            m_lexer->setFont(font,style);
            if ( style == QsciLexerCA65::CA65_Default )
            {
               m_scintilla->setMarginsFont(font);
            }
         }
      }
   }
   else
   {
      style = ui->styleName->itemData(ui->styleName->currentIndex()).toInt();
      QFont font = m_lexer->font(style);
      font.setUnderline(checked);
      m_lexer->setFont(font,style);
      if ( style == QsciLexerCA65::CA65_Default )
      {
         m_scintilla->setMarginsFont(font);
      }
   }
}

void EnvironmentSettingsDialog::on_styleFont_currentIndexChanged(QString /*fontName*/)
{
   QSettings settings(QSettings::IniFormat, QSettings::UserScope, "CSPSoftware", "NESICIDE");
   int style;

   if ( ui->applyAll->isChecked() )
   {
      for ( style = 0; style < (1<<m_lexer->styleBitsNeeded()); style++ )
      {
         if ( !m_lexer->description(style).isEmpty() )
         {
            QFont font = ui->styleFont->currentFont();
            font.setBold(ui->fontBold->isChecked());
            font.setItalic(ui->fontItalic->isChecked());
            font.setUnderline(ui->fontUnderline->isChecked());
            font.setPointSize(ui->fontSize->value());
            m_lexer->setFont(font,style);
            if ( style == QsciLexerCA65::CA65_Default )
            {
               m_scintilla->setMarginsFont(font);
            }
         }
      }
   }
   else
   {
      style = ui->styleName->itemData(ui->styleName->currentIndex()).toInt();
      QFont font = ui->styleFont->currentFont();
      font.setBold(ui->fontBold->isChecked());
      font.setItalic(ui->fontItalic->isChecked());
      font.setUnderline(ui->fontUnderline->isChecked());
      font.setPointSize(ui->fontSize->value());
      m_lexer->setFont(font,style);
      if ( style == QsciLexerCA65::CA65_Default )
      {
         m_scintilla->setMarginsFont(font);
      }
   }
}

void EnvironmentSettingsDialog::on_styleColor_clicked()
{
   QSettings settings(QSettings::IniFormat, QSettings::UserScope, "CSPSoftware", "NESICIDE");
   QColorDialog dlg;
   int style = ui->styleName->itemData(ui->styleName->currentIndex()).toInt();

   dlg.setCurrentColor(m_lexer->color(style));

   if (dlg.exec() == QColorDialog::Accepted)
   {
      QColor chosenColor = dlg.selectedColor();
      m_lexer->setColor(chosenColor,style);
   }
}

void EnvironmentSettingsDialog::on_backgroundColor_clicked()
{
   QSettings settings(QSettings::IniFormat, QSettings::UserScope, "CSPSoftware", "NESICIDE");
   QColorDialog dlg;

   dlg.setCurrentColor(m_lexer->defaultPaper());

   if (dlg.exec() == QColorDialog::Accepted)
   {
      QColor chosenColor = dlg.selectedColor();
      m_lexer->setDefaultPaper(chosenColor);
      m_lexer->setPaper(chosenColor,-1);
   }
}

void EnvironmentSettingsDialog::on_caretColor_clicked()
{
   QSettings settings(QSettings::IniFormat, QSettings::UserScope, "CSPSoftware", "NESICIDE");
   QColorDialog dlg;

   dlg.setCurrentColor(m_lexer->defaultPaper());

   if (dlg.exec() == QColorDialog::Accepted)
   {
      QColor chosenColor = dlg.selectedColor();
      m_scintilla->setCaretForegroundColor(chosenColor);
      m_caretColor = chosenColor;
   }
}

void EnvironmentSettingsDialog::on_marginBackgroundColor_clicked()
{
   QSettings settings(QSettings::IniFormat, QSettings::UserScope, "CSPSoftware", "NESICIDE");
   QColorDialog dlg;

   dlg.setCurrentColor(m_marginBackgroundColor);

   if (dlg.exec() == QColorDialog::Accepted)
   {
      m_marginBackgroundColor = dlg.selectedColor();
      m_scintilla->setMarginsBackgroundColor(m_marginBackgroundColor);
   }
}

void EnvironmentSettingsDialog::on_marginForegroundColor_clicked()
{
   QSettings settings(QSettings::IniFormat, QSettings::UserScope, "CSPSoftware", "NESICIDE");
   QColorDialog dlg;

   dlg.setCurrentColor(m_marginForegroundColor);

   if (dlg.exec() == QColorDialog::Accepted)
   {
      m_marginForegroundColor = dlg.selectedColor();
      m_scintilla->setMarginsForegroundColor(m_marginForegroundColor);
   }
}

void EnvironmentSettingsDialog::on_highlightBarColor_clicked()
{
   QSettings settings(QSettings::IniFormat, QSettings::UserScope, "CSPSoftware", "NESICIDE");
   QColorDialog dlg;

   dlg.setCurrentColor(m_highlightBarColor);

   if (dlg.exec() == QColorDialog::Accepted)
   {
      m_highlightBarColor = dlg.selectedColor();
      m_scintilla->setMarkerBackgroundColor(m_highlightBarColor,Marker_Highlight);
   }
}

void EnvironmentSettingsDialog::on_showHighlightBar_toggled(bool checked)
{
   if ( checked )
   {
      m_scintilla->setMarkerBackgroundColor(m_highlightBarColor,Marker_Highlight);
   }
   else
   {
      // Set marker color to background to hide it.
      m_scintilla->setMarkerBackgroundColor(m_lexer->defaultPaper(),Marker_Highlight);
   }
}

void EnvironmentSettingsDialog::on_showLineNumberMargin_toggled(bool checked)
{
   if ( checked )
   {
      m_scintilla->setMarginLineNumbers(Margin_LineNumbers,true);
      m_scintilla->setMarginWidth(Margin_LineNumbers,10);
      m_scintilla->setMarginMarkerMask(Margin_LineNumbers,0);
      m_scintilla->setMarginType(Margin_LineNumbers,QsciScintilla::NumberMargin);
      m_scintilla->setMarginSensitivity(Margin_LineNumbers,true);
   }
   else
   {
      m_scintilla->setMarginWidth(Margin_LineNumbers,0);
   }
}

void EnvironmentSettingsDialog::on_fontSize_valueChanged(int value)
{
   QSettings settings(QSettings::IniFormat, QSettings::UserScope, "CSPSoftware", "NESICIDE");
   int style;

   if ( ui->applyAll->isChecked() )
   {
      for ( style = 0; style < (1<<m_lexer->styleBitsNeeded()); style++ )
      {
         if ( !m_lexer->description(style).isEmpty() )
         {
            QFont font = m_lexer->font(style);
            font.setPointSize(value);
            m_lexer->setFont(font,style);
            if ( style == QsciLexerCA65::CA65_Default )
            {
               m_scintilla->setMarginsFont(font);
            }
         }
      }
   }
   else
   {
      style = ui->styleName->itemData(ui->styleName->currentIndex()).toInt();
      QFont font = m_lexer->font(style);
      font.setPointSize(value);
      m_lexer->setFont(font,style);
      if ( style == QsciLexerCA65::CA65_Default )
      {
         m_scintilla->setMarginsFont(font);
      }
   }
}

void EnvironmentSettingsDialog::on_language_currentIndexChanged(int index)
{
   setupCodeEditor(index);
}

void EnvironmentSettingsDialog::on_treeWidget_itemSelectionChanged()
{
   ui->treeWidget->expand(ui->treeWidget->currentIndex());
   if ( ui->treeWidget->currentItem()->childCount() )
   {
      ui->treeWidget->setCurrentItem(ui->treeWidget->itemBelow(ui->treeWidget->currentItem()));
   }
   ui->stackedWidget->setCurrentWidget(pageMap[ui->treeWidget->currentItem()->text(0)]);
}

void EnvironmentSettingsDialog::on_buttonBox_accepted()
{
   writeSettings();
}

void EnvironmentSettingsDialog::on_textEncoding_currentIndexChanged(int index)
{
    m_scintilla->setUtf8(index);
}

QString EnvironmentSettingsDialog::textEncodingString()
{
   if ( m_textEncodingIsUtf8 )
   {
      return "UTF-8";
   }
   else
   {
      return "ISO 8859";
   }
}
