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

    emulatorDlg = (NESEmulatorDialog *)NULL;
    emulatorDlgTabIdx = -1;
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
    if (nesicideProject->get_isInitialized())
    {
        str.append(" - ");
        str.append(nesicideProject->get_projectTitle());
    }

    this->setWindowTitle(str);
    projectTreeviewModel->layoutChangedEvent();

    // Enabled/Disable actions based on if we have a project loaded or not
    ui->actionProject_Properties->setEnabled(nesicideProject->get_isInitialized());
    ui->actionSave_Project->setEnabled(nesicideProject->get_isInitialized());
    ui->actionSave_Project_As->setEnabled(nesicideProject->get_isInitialized());
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
    // Allow the user to select a file name. Note that using the static function produces a native
    // file dialog, while creating an instance of QFileDialog results in a non-native file dialog..
    QString fileName = QFileDialog::getSaveFileName(this, QString("Save Project"), QString(""),
                                                    QString("NESECIDE2 Project (*.nesproject)"));
    if (!fileName.isEmpty())
    {
        QFile file(fileName);
        if( !file.open( QFile::WriteOnly) )
        {
            QMessageBox::critical(this, "Error", "An error occured while trying to open the project file for writing.");
            return;
        }

        QDomDocument doc;
        QDomProcessingInstruction instr = doc.createProcessingInstruction("xml", "version='1.0' encoding='UTF-8'");
        doc.appendChild(instr);

        if (!nesicideProject->serialize(doc, doc))
        {
            QMessageBox::critical(this, "Error", "An error occured while trying to serialize the project data.");
        }

        // Create a text stream so we can stream the XML data to the file easily.
        QTextStream ts( &file );

        // Use the standard C++ stream function for streaming the string representation of our XML to
        // our file stream.
        ts << doc.toString();

        // And finally close the file.
        file.close();
    }
}

void MainWindow::on_actionProject_Properties_triggered()
{
    ProjectPropertiesDialog *dlg = new ProjectPropertiesDialog(
            this, nesicideProject->get_pointerToListOfProjectPaletteEntries());
    dlg->setProjectName(nesicideProject->get_projectTitle());
    if (dlg->exec() == QDialog::Accepted)
    {
        nesicideProject->set_projectTitle(dlg->getProjectName());
        nesicideProject->get_pointerToListOfProjectPaletteEntries()->clear();
        for (int paletteItemIndex=0; paletteItemIndex<dlg->currentPalette.count(); paletteItemIndex++)
            nesicideProject->get_pointerToListOfProjectPaletteEntries()->append(dlg->currentPalette.at(paletteItemIndex));
        projectDataChangesEvent();
    }
    delete dlg;
}

void MainWindow::on_actionNew_Project_triggered()
{
    NewProjectDialog *dlg = new NewProjectDialog();
    if (dlg->exec() == QDialog::Accepted)
    {
        nesicideProject->set_projectTitle(dlg->getProjectTitle());
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

void MainWindow::on_tabWidget_tabCloseRequested(int index)
{
    QObject *projectItem = (QObject *)NULL;
    bool foundItem = false;
    for (int treeviewItemIndex = 0; treeviewItemIndex < projectTreeviewModel->children().count(); treeviewItemIndex++)
    {
        projectItem = projectTreeviewModel->children().at(treeviewItemIndex);
        if (!projectItem)
            continue;

        if (((IProjectTreeViewItem *)projectItem)->getTabIndex() == index)
        {
            foundItem = true;
            break;
        }
    }

    if (foundItem)
    {
        if (((IProjectTreeViewItem *)projectItem)->onCloseQuery())
            ui->tabWidget->removeTab(index);
    } else {
        ui->tabWidget->removeTab(index);
        if (index == emulatorDlgTabIdx)
        {
            emulatorDlg->stopEmulation();
            ui->actionEmulation_Window->setChecked(false);
            emulatorDlgTabIdx = -1;
        }
    }
}

void MainWindow::on_projectBrowserDockWidget_visibilityChanged(bool visible)
{
    if (!visible)
    {
        if (!ui->projectBrowserDockWidget->isVisibleTo(this))
        {
            ui->action_Project_Browser->setChecked(false);
        }
    } else
        ui->action_Project_Browser->setChecked(visible);
}

void MainWindow::on_action_Project_Browser_toggled(bool visible)
{
    ui->projectBrowserDockWidget->setVisible(visible);
}

void MainWindow::on_actionEmulation_Window_toggled(bool value)
{
    if (value)
    {
        if (!emulatorDlg)
        {
            emulatorDlg = new NESEmulatorDialog();
        }
        emulatorDlgTabIdx = ui->tabWidget->addTab(emulatorDlg, "Emulation Window");
        ui->tabWidget->setCurrentIndex(emulatorDlgTabIdx);
    }
    else
        ui->tabWidget->removeTab(emulatorDlgTabIdx);
}

void MainWindow::on_MainWindow_destroyed()
{
}

void MainWindow::closeEvent ( QCloseEvent * event )
{
    if (emulatorDlg)
        emulatorDlg->stopEmulation();

    QWidget::closeEvent(event);
}

void MainWindow::on_actionOpen_Project_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, 0, 0, "NESECIDE2 Project (*.nesproject)");
    if (QFile::exists(fileName))
    {
        QDomDocument doc;
        QFile file( fileName );
        if (!file.open(QFile::ReadOnly))
        {
            QMessageBox::critical(this, "Error", "Failed to open the project file.");
            return;
        }

        if(!doc.setContent(file.readAll()))
        {
            QMessageBox::critical(this, "Error", "Failed to parse the project xml data.");
            file.close();
            return;
        }
        file.close();

        nesicideProject->deserialize(doc, doc);
        projectDataChangesEvent();
    }
}
