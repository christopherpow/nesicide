#include "filepropertiesdialog.h"
#include "ui_filepropertiesdialog.h"

FilePropertiesDialog::FilePropertiesDialog(CProjectBase *base, QWidget *parent) :
   QDialog(parent),
   ui(new Ui::FilePropertiesDialog)
{
   ui->setupUi(this);

   _base = base;

   setWindowTitle(base->caption());
   ui->includeInBuild->setChecked(base->includeInBuild());
}

FilePropertiesDialog::~FilePropertiesDialog()
{
   delete ui;
}

void FilePropertiesDialog::on_buttonBox_accepted()
{
   _base->setIncludeInBuild(ui->includeInBuild->isChecked());
   accept();
}

void FilePropertiesDialog::on_buttonBox_rejected()
{
   reject();
}
