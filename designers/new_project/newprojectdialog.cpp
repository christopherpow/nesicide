#include "newprojectdialog.h"
#include "ui_newprojectdialog.h"

#include <QFileDialog>

NewProjectDialog::NewProjectDialog(QWidget* parent) :
   QDialog(parent),
   ui(new Ui::NewProjectDialog)
{
   ui->setupUi(this);
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

QString NewProjectDialog::getProjectTitle()
{
   return ui->projectTitleLineEdit->text();
}

QString NewProjectDialog::getProjectBasePath()
{
   return ui->projectBasePath->text();
}

void NewProjectDialog::on_projectBasePathBrowse_clicked()
{
   QString value = QFileDialog::getExistingDirectory(this,"Project Base Path");
   
   if ( !value.isEmpty() )
   {
      ui->projectBasePath->setText(value);
   }    
}
