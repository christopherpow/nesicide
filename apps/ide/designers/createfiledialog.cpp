#include "createfiledialog.h"
#include "ui_createfiledialog.h"

CreateFileDialog::CreateFileDialog(QWidget *parent) :
   QDialog(parent),
   ui(new Ui::CreateFileDialog)
{
   ui->setupUi(this);
}

CreateFileDialog::~CreateFileDialog()
{
   delete ui;
}
