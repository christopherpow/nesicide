#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    nesicideProject = new CNesicideProject();
    ui->setupUi(this);
    projectTreeviewModel = new CProjectTreeViewModel(ui->projectTreeWidget, nesicideProject);
    ui->projectTreeWidget->mdiTabWidget = ui->tabWidget;
    ui->projectTreeWidget->setModel(projectTreeviewModel);
    projectDataChangesEvent();

}

MainWindow::~MainWindow()
{
    delete nesicideProject;
    delete ui;
    delete projectTreeviewModel;
}

void MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void MainWindow::projectDataChangesEvent()
{
    QString str;
    str.append("NESICIDE2");
    if (nesicideProject->getIsInitialized())
    {
        str.append(" - ");
        str.append(nesicideProject->ProjectTitle);
    }

    this->setWindowTitle(str);
    projectTreeviewModel->layoutChangedEvent();

    // Enabled/Disable actions based on if we have a project loaded or not
    ui->actionProject_Properties->setEnabled(nesicideProject->getIsInitialized());
    ui->actionSave_Project->setEnabled(nesicideProject->getIsInitialized());
    ui->actionSave_Project_As->setEnabled(nesicideProject->getIsInitialized());
}

void MainWindow::on_actionSave_Project_triggered()
{
    QMessageBox msgBox;
    QDomDocument doc;
    QDomProcessingInstruction instr = doc.createProcessingInstruction("xml", "version='1.0' encoding='UTF-8'");
    doc.appendChild(instr);
    nesicideProject->serialize(doc, doc);
    msgBox.setText(doc.toString());
    msgBox.exec();
}

void MainWindow::on_actionSave_Project_As_triggered()
{

}

void MainWindow::on_actionProject_Properties_triggered()
{
    ProjectPropertiesDialog *dlg = new ProjectPropertiesDialog(this);
    dlg->setProjectName(nesicideProject->ProjectTitle);
    if (dlg->exec() == QDialog::Accepted)
    {
        nesicideProject->ProjectTitle = dlg->getProjectName();
        projectDataChangesEvent();
    }
    delete dlg;
}

void MainWindow::on_actionNew_Project_triggered()
{
    NewProjectDialog *dlg = new NewProjectDialog();
    if (dlg->exec() == QDialog::Accepted)
    {
        nesicideProject->ProjectTitle = dlg->getProjectTitle();
        nesicideProject->initializeProject();
        projectDataChangesEvent();
    }
    delete dlg;
}

void MainWindow::on_actionCreate_Project_from_ROM_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, 0, 0, "NES Rom (*.nes)");
    if (fileName.isEmpty())
        return;

    nesicideProject->createProjectFromRom(fileName);
    projectDataChangesEvent();
}
