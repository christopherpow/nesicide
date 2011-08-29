#include "newprojectdialog.h"
#include "ui_newprojectdialog.h"

#include <QFileDialog>

NewProjectDialog::NewProjectDialog(QWidget* parent,QString windowTitle,QString defName,QString defPath) :
   QDialog(parent),
   ui(new Ui::NewProjectDialog)
{
   ui->setupUi(this);
   ui->name->setText(defName);
   ui->path->setText(defPath);
   on_path_textChanged(defPath);
   setWindowTitle(windowTitle);
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

void NewProjectDialog::on_pathBrowse_clicked()
{
   QString value = QFileDialog::getExistingDirectory(this,"Path",QDir::currentPath());

   if ( !value.isEmpty() )
   {
      ui->path->setText(value);
   }
}

void NewProjectDialog::on_path_textChanged(QString text)
{
   ui->buttonBox->setEnabled(checkValidity());
}

void NewProjectDialog::on_name_textChanged(QString text)
{
   ui->buttonBox->setEnabled(checkValidity());
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
