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
   QString value = QFileDialog::getExistingDirectory(this,"Path");

   if ( !value.isEmpty() )
   {
      ui->path->setText(value);
   }
}
