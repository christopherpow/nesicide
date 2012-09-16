#include "newprojectdialog.h"
#include "ui_newprojectdialog.h"

#include <QFileDialog>
#include <QSettings>

NewProjectDialog::NewProjectDialog(QWidget* parent,QString windowTitle,QString defName,QString defPath, bool showTemplate) :
   QDialog(parent),
   ui(new Ui::NewProjectDialog)
{
   ui->setupUi(this);
   ui->name->setText(defName);
   ui->path->setText(defPath);
   on_path_textChanged(defPath);
   setWindowTitle(windowTitle);
   ui->templateGroup->setVisible(showTemplate);
   ui->name->setFocus();

   QDir templatesDir(":/templates");
   QStringList templates = templatesDir.entryList();

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
   QSettings settings;

   QString path = settings.value("LastProjectBasePath").toString();

   QString value = QFileDialog::getExistingDirectory(this,"Path",path);

   if ( !value.isEmpty() )
   {
      ui->path->setText(value);

      QFileInfo fileInfo(value);

      settings.setValue("LastProjectBasePath",fileInfo.path());
   }
}

void NewProjectDialog::on_path_textChanged(QString text)
{
   ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(checkValidity());
}

void NewProjectDialog::on_name_textChanged(QString text)
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

      if ( (!ui->path->text().isEmpty()) && check.exists() && (!ui->name->text().isEmpty()) )
      {
         return true;
      }
      else
      {
         return false;
      }
   }
}
