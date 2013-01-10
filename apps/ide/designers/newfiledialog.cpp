#include "newfiledialog.h"
#include "ui_newfiledialog.h"

#include <QFileDialog>
#include <QSettings>

NewFileDialog::NewFileDialog(QString windowTitle,QString defName,QString defPath, QWidget* parent) :
   QDialog(parent),
   ui(new Ui::NewFileDialog)
{
   ui->setupUi(this);
   ui->name->setText(defName);
   ui->path->setText(defPath);
   on_path_textChanged(defPath);
   setWindowTitle(windowTitle);
   ui->name->setFocus();
}

NewFileDialog::~NewFileDialog()
{
   delete ui;
}

void NewFileDialog::changeEvent(QEvent* e)
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

QString NewFileDialog::getName()
{
   return ui->name->text();
}

QString NewFileDialog::getPath()
{
   return ui->path->text();
}

void NewFileDialog::on_pathBrowse_clicked()
{
   QSettings settings(QSettings::IniFormat, QSettings::UserScope, "CSPSoftware", "NESICIDE");

   QString path = settings.value("LastFileBasePath").toString();

   QString value = QFileDialog::getSaveFileName(this,"Path",path,"",NULL,QFileDialog::ShowDirsOnly);

   if ( !value.isEmpty() )
   {
      ui->path->setText(value);

      QFileInfo fileInfo(value);

      settings.setValue("LastFileBasePath",fileInfo.path());
   }
}

void NewFileDialog::on_path_textChanged(QString /*text*/)
{
   ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(checkValidity());
}

void NewFileDialog::on_name_textChanged(QString /*text*/)
{
   ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(checkValidity());
}

bool NewFileDialog::checkValidity()
{
   return !ui->name->text().isEmpty();
}

void NewFileDialog::on_buttonBox_accepted()
{
   QDir check(ui->path->text());

   if ( !check.exists() )
   {
      check.mkpath(ui->path->text());
   }
}
