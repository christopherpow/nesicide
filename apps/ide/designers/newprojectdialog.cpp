#include "newprojectdialog.h"
#include "ui_newprojectdialog.h"

#include "projectpropertiesdialog.h"
#include "main.h"

#include <QFileDialog>
#include <QSettings>

NewProjectDialog::NewProjectDialog(QString windowTitle,QString defName,QString defPath, QWidget* parent) :
   QDialog(parent),
   ui(new Ui::NewProjectDialog)
{
   ui->setupUi(this);
   ui->name->setText(defName);
   ui->path->setText(defPath);
   on_path_textChanged(defPath);
   setWindowTitle(windowTitle);
   ui->name->setFocus();

   QDir templatesDir(":/templates/NES");
   QStringList templates = templatesDir.entryList();
   templates.removeAll("header.s_in");

   ui->templateProject->addItem("Empty Project");
   ui->templateProject->addItems(templates);
}

NewProjectDialog::~NewProjectDialog()
{
   delete ui;
}

void NewProjectDialog::changeEvent(QEvent* e)
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

QString NewProjectDialog::getName()
{
   return ui->name->text();
}

QString NewProjectDialog::getPath()
{
   return ui->path->text();
}

QString NewProjectDialog::getTarget()
{
   return ui->target->currentText();
}

QString NewProjectDialog::getTemplate()
{
   return ui->templateProject->currentText();
}

int NewProjectDialog::getTemplateIndex()
{
   return ui->templateProject->currentIndex();
}

void NewProjectDialog::on_pathBrowse_clicked()
{
   QSettings settings(QSettings::IniFormat, QSettings::UserScope, "CSPSoftware", "NESICIDE");

   QString path = settings.value("LastProjectBasePath").toString();

   QString value = QFileDialog::getExistingDirectory(this,"Project Path",path,QFileDialog::ShowDirsOnly);

   if ( !value.isEmpty() )
   {
      ui->path->setText(value);

      QFileInfo fileInfo(value);

      settings.setValue("LastProjectBasePath",fileInfo.absoluteFilePath());
   }
}

void NewProjectDialog::on_path_textChanged(QString /*text*/)
{
   ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(checkValidity());
}

void NewProjectDialog::on_name_textChanged(QString /*text*/)
{
   ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(checkValidity());
}

bool NewProjectDialog::checkValidity()
{
   // Hackety hack (but then again so is using "new project dialog" for adding
   // source files)
   if( windowTitle() == "New Source" )
   {
      return !ui->name->text().isEmpty();
   }
   else
   {
      QDir check(ui->path->text());

      if ( !ui->path->text().isEmpty() && !ui->name->text().isEmpty() )
      {
         return true;
      }
      else
      {
         return false;
      }
   }
}

void NewProjectDialog::on_buttonBox_accepted()
{
   // If they hit Project Properties button then we're all set except for the project name may have changed.
   if ( !nesicideProject->isInitialized() )
   {
      setupProject();
   }
   else
   {
      nesicideProject->setProjectTitle(getName());
   }
}

void NewProjectDialog::setupProject()
{
   QDir check(ui->path->text());

   if ( !check.exists() )
   {
      check.mkpath(ui->path->text());
   }

   QDir::setCurrent(getPath());

   // Set project target before initializing project...
   if ( getTarget() == "Commodore 64" )
   {
      nesicideProject->setProjectTarget("c64");
   }
   else if ( getTarget() == "Nintendo Entertainment System" )
   {
      nesicideProject->setProjectTarget("nes");
   }
   nesicideProject->initializeProject();
   nesicideProject->setDirty(true);
   nesicideProject->setProjectTitle(getName());
}

void NewProjectDialog::on_target_currentIndexChanged(QString target)
{
   QStringList templates;
   ui->templateProject->clear();
   ui->templateProject->addItem("Empty Project");
   if ( target == "Nintendo Entertainment System" )
   {
      QDir templatesDir(":/templates/NES");
      templates = templatesDir.entryList();
      templates.removeAll("header.s_in");
   }
   else if ( target == "Commodore 64" )
   {
      QDir templatesDir(":/templates/C64");
      templates = templatesDir.entryList();
      templates.removeAll("header.s_in");
   }
   ui->templateProject->addItems(templates);
}

void NewProjectDialog::on_projectProperties_clicked()
{
   ProjectPropertiesDialog* dlg;

   setupProject();

   dlg = new ProjectPropertiesDialog();

   if (dlg->exec() == QDialog::Accepted)
   {
      nesicideProject->setDirty(true);
      // They might have changed the project name...
      ui->name->setText(nesicideProject->getProjectTitle());
   }

   delete dlg;
}
