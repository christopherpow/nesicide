#include "environmentsettingsdialog.h"
#include "ui_environmentsettingsdialog.h"

#include <QSettings>

EnvironmentSettingsDialog::EnvironmentSettingsDialog(QWidget* parent) :
   QDialog(parent),
   ui(new Ui::EnvironmentSettingsDialog)
{
   QSettings settings;
   ui->setupUi(this);
   
   ui->showWelcomeOnStart->setChecked(settings.value(ui->showWelcomeOnStart->objectName(),true).toBool());
   ui->saveAllOnCompile->setChecked(settings.value(ui->saveAllOnCompile->objectName(),true).toBool());
}

EnvironmentSettingsDialog::~EnvironmentSettingsDialog()
{
   delete ui;
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

void EnvironmentSettingsDialog::on_buttonBox_accepted()
{
    QSettings settings;
    
    settings.setValue(ui->showWelcomeOnStart->objectName(),ui->showWelcomeOnStart->isChecked());
    settings.setValue(ui->saveAllOnCompile->objectName(),ui->saveAllOnCompile->isChecked());
}
