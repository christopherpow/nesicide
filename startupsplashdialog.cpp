#include "startupsplashdialog.h"
#include "ui_startupsplashdialog.h"

StartupSplashDialog::StartupSplashDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::StartupSplashDialog)
{
    ui->setupUi(this);
}

StartupSplashDialog::~StartupSplashDialog()
{
    delete ui;
}
