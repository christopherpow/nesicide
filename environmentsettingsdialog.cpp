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

EnvironmentSettingsDialog::EnvironmentSettingsDialog(QWidget* parent) :
   QDialog(parent),
   ui(new Ui::EnvironmentSettingsDialog)
{
   QSettings settings;
   ui->setupUi(this);

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

void EnvironmentSettingsDialog::on_CodeDefaultButton_clicked()
{
   QColorDialog* dlg = new QColorDialog(this);

   if (dlg->exec() == QColorDialog::Accepted)
   {
      QColor chosenColor= dlg->selectedColor();
      QString COLOR_STYLE("QPushButton { background-color : %1; color : %2; }");
      QColor idealTextColor = getIdealTextColor(chosenColor);
      this->ui->CodeDefaultButton->setStyleSheet(COLOR_STYLE.arg(chosenColor.name()).arg(idealTextColor.name()));
   }

   delete dlg;
}

void EnvironmentSettingsDialog::on_CodeInstructionsButton_clicked()
{
   QColorDialog* dlg = new QColorDialog(this);

   if (dlg->exec() == QColorDialog::Accepted)
   {
      QColor chosenColor= dlg->selectedColor();
      QString COLOR_STYLE("QPushButton { background-color : %1; color : %2; }");
      QColor idealTextColor = getIdealTextColor(chosenColor);
      this->ui->CodeInstructionsButton->setStyleSheet(COLOR_STYLE.arg(chosenColor.name()).arg(idealTextColor.name()));
   }

   delete dlg;
}

void EnvironmentSettingsDialog::on_CodeCommentsButton_clicked()
{
   QColorDialog* dlg = new QColorDialog(this);

   if (dlg->exec() == QColorDialog::Accepted)
   {
      QColor chosenColor= dlg->selectedColor();
      QString COLOR_STYLE("QPushButton { background-color : %1; color : %2; }");
      QColor idealTextColor = getIdealTextColor(chosenColor);
      this->ui->CodeCommentsButton->setStyleSheet(COLOR_STYLE.arg(chosenColor.name()).arg(idealTextColor.name()));
   }

   delete dlg;
}

void EnvironmentSettingsDialog::on_CodePreprocessorButton_clicked()
{
   QColorDialog* dlg = new QColorDialog(this);

   if (dlg->exec() == QColorDialog::Accepted)
   {
      QColor chosenColor= dlg->selectedColor();
      QString COLOR_STYLE("QPushButton { background-color : %1; color : %2; }");
      QColor idealTextColor = getIdealTextColor(chosenColor);
      this->ui->CodePreprocessorButton->setStyleSheet(COLOR_STYLE.arg(chosenColor.name()).arg(idealTextColor.name()));
   }

   delete dlg;
}

void EnvironmentSettingsDialog::on_CodeNumbersButton_clicked()
{
   QColorDialog* dlg = new QColorDialog(this);

   if (dlg->exec() == QColorDialog::Accepted)
   {
      QColor chosenColor= dlg->selectedColor();
      QString COLOR_STYLE("QPushButton { background-color : %1; color : %2; }");
      QColor idealTextColor = getIdealTextColor(chosenColor);
      this->ui->CodeNumbersButton->setStyleSheet(COLOR_STYLE.arg(chosenColor.name()).arg(idealTextColor.name()));
   }

   delete dlg;
}

void EnvironmentSettingsDialog::on_CodeStringsButton_clicked()
{
   QColorDialog* dlg = new QColorDialog(this);

   if (dlg->exec() == QColorDialog::Accepted)
   {
      QColor chosenColor= dlg->selectedColor();
      QString COLOR_STYLE("QPushButton { background-color : %1; color : %2; }");
      QColor idealTextColor = getIdealTextColor(chosenColor);
      this->ui->CodeStringsButton->setStyleSheet(COLOR_STYLE.arg(chosenColor.name()).arg(idealTextColor.name()));
   }

   delete dlg;
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
