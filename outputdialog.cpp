#include "outputdialog.h"
#include "ui_outputdialog.h"

#include "cbuildertextlogger.h"
#include "cpluginmanager.h"

OutputDialog::OutputDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::OutputDialog)
{
    ui->setupUi(this);
    ui->outputTabWidget->setCurrentIndex(0);
    generalTextLogger.setTextEditControl(ui->generalOutputTextEdit);
    generalTextLogger.write("<strong>NESICIDE2</strong> Alpha Release");
    generalTextLogger.write("<strong>Plugin Scripting Subsystem:</strong> " + pluginManager->getVersionInfo());
    buildTextLogger.setTextEditControl(ui->compilerOutputTextEdit);
    debugTextLogger.setTextEditControl(ui->debuggerOutputTextEdit);
}

OutputDialog::~OutputDialog()
{
    delete ui;
}
