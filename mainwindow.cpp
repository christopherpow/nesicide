#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "chrmeminspector.h"

#include "main.h"

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
    projectDataChangesEvent();

    m_pCHRMEMInspector = new CHRMEMInspector ();
    m_pCHRMEMInspector->setFeatures(QDockWidget::DockWidgetClosable|QDockWidget::DockWidgetFloatable|QDockWidget::DockWidgetMovable);
    m_pCHRMEMInspector->setWindowTitle("CHR Memory Inspector");
    m_pCHRMEMInspector->setAllowedAreas(Qt::AllDockWidgetAreas);
    addDockWidget(Qt::BottomDockWidgetArea, m_pCHRMEMInspector );
    QRect ppuRc = m_pCHRMEMInspector->geometry();
    ppuRc.setSize(QSize(300, 300));
    m_pCHRMEMInspector->setGeometry(ppuRc);
    m_pCHRMEMInspector->hide();
    QObject::connect(m_pCHRMEMInspector, SIGNAL(visibilityChanged(bool)), this, SLOT(chrmemInspector_close(bool)));

    m_pOAMInspector = new OAMInspector ();
    m_pOAMInspector->setFeatures(QDockWidget::DockWidgetClosable|QDockWidget::DockWidgetFloatable|QDockWidget::DockWidgetMovable);
    m_pOAMInspector->setWindowTitle("OAM Inspector");
    m_pOAMInspector->setAllowedAreas(Qt::AllDockWidgetAreas);
    addDockWidget(Qt::BottomDockWidgetArea, m_pOAMInspector );
    QRect oamRc = m_pOAMInspector->geometry();
    oamRc.setSize(QSize(300, 300));
    m_pOAMInspector->setGeometry(oamRc);
    m_pOAMInspector->hide();
    QObject::connect(m_pOAMInspector, SIGNAL(visibilityChanged(bool)), this, SLOT(oamInspector_close(bool)));

    builderTextLogger.setTextEditControl(ui->compilerOutputTextEdit);
    builderTextLogger.write("<strong>NESICIDE2</strong> Alpha Release");
}

MainWindow::~MainWindow()
{
    ui->tabWidget->clear();

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

    if (ui->tabWidget->currentIndex() >= 0)
    {
        IProjectTreeViewItem *projectItem = matchTab(nesicideProject, ui->tabWidget->currentIndex());
        if (projectItem)
        {
            ui->actionSave_Active_Document->setEnabled(((IProjectTreeViewItem *)projectItem)->isDocumentSaveable());
        } else {
            ui->actionSave_Active_Document->setEnabled(false);
        }
    }
}

void MainWindow::on_actionSave_Project_triggered()
{
    if (projectFileName.isEmpty())
    {
        QString fileName = QFileDialog::getSaveFileName(this, QString("Save Project"), QString(""),
                                                        QString("NESECIDE2 Project (*.nesproject)"));
        if (!fileName.isEmpty())
            saveProject(fileName);
    }
    else
        saveProject(projectFileName);
}

void MainWindow::saveProject(QString fileName)
{
    QFile file(fileName);
    if( !file.open( QFile::WriteOnly) )
    {
        QMessageBox::critical(this, "Error", "An error occured while trying to open the project file for writing.");
        projectFileName.clear();
        return;
    }

    QDomDocument doc;
    QDomProcessingInstruction instr = doc.createProcessingInstruction("xml", "version='1.0' encoding='UTF-8'");
    doc.appendChild(instr);

    if (!nesicideProject->serialize(doc, doc))
    {
        QMessageBox::critical(this, "Error", "An error occured while trying to serialize the project data.");
        file.close();
        projectFileName.clear();
    }

    // Create a text stream so we can stream the XML data to the file easily.
    QTextStream ts( &file );

    // Use the standard C++ stream function for streaming the string representation of our XML to
    // our file stream.
    ts << doc.toString();

    // And finally close the file.
    file.close();

    projectFileName = fileName;
}

void MainWindow::on_actionSave_Project_As_triggered()
{
    // Allow the user to select a file name. Note that using the static function produces a native
    // file dialog, while creating an instance of QFileDialog results in a non-native file dialog..
    QString fileName = QFileDialog::getSaveFileName(this, QString("Save Project"), QString(""),
                                                    QString("NESECIDE2 Project (*.nesproject)"));
    if (!fileName.isEmpty())
        saveProject(fileName);
}

void MainWindow::on_actionProject_Properties_triggered()
{
    ProjectPropertiesDialog *dlg = new ProjectPropertiesDialog(
            this, nesicideProject->get_pointerToListOfProjectPaletteEntries());
    dlg->setProjectName(nesicideProject->get_projectTitle());
    dlg->initSourcesList();
    if (nesicideProject->getProject()->getMainSource())
        dlg->setMainSource(nesicideProject->getProject()->getMainSource()->get_sourceName());
    else
        dlg->setMainSource("");
    if (dlg->exec() == QDialog::Accepted)
    {
        nesicideProject->set_projectTitle(dlg->getProjectName());
        nesicideProject->get_pointerToListOfProjectPaletteEntries()->clear();
        for (int paletteItemIndex=0; paletteItemIndex<dlg->currentPalette.count(); paletteItemIndex++)
            nesicideProject->get_pointerToListOfProjectPaletteEntries()->append(dlg->currentPalette.at(paletteItemIndex));
        projectDataChangesEvent();

        for (int sourceIndex = 0; sourceIndex < nesicideProject->getProject()->getSources()->childCount(); sourceIndex++)
        {
            CSourceItem *sourceItem = (CSourceItem *)nesicideProject->getProject()->getSources()->child(sourceIndex);
            if (sourceItem->get_sourceName() == dlg->getMainSource())
            {
                nesicideProject->getProject()->setMainSource(sourceItem);
            }
        }

    }
    delete dlg;
}

void MainWindow::on_actionNew_Project_triggered()
{
    NewProjectDialog *dlg = new NewProjectDialog();
    if (dlg->exec() == QDialog::Accepted)
    {
        ui->projectTreeWidget->setModel(NULL);
        nesicideProject->set_projectTitle(dlg->getProjectTitle());
        nesicideProject->initializeProject();
        ui->projectTreeWidget->setModel(projectTreeviewModel);
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

IProjectTreeViewItem *MainWindow::matchTab(IProjectTreeViewItem *root, int tabIndex)
{
    for (int treeviewItemIndex = 0;
         treeviewItemIndex < ((IProjectTreeViewItem *)root)->childCount(); treeviewItemIndex++)
    {
        IProjectTreeViewItem *item = ((IProjectTreeViewItem *)root)->child(treeviewItemIndex);
        if (!item)
            continue;

        if (((IProjectTreeViewItem *)item)->getTabIndex() == tabIndex)
            return item;

        if (((IProjectTreeViewItem *)item)->childCount() > 0)
        {
            IProjectTreeViewItem *result = matchTab(item, tabIndex);
            if (result)
                return result;
        }
    }

    return (IProjectTreeViewItem *)NULL;
}

void MainWindow::on_tabWidget_tabCloseRequested(int index)
{
    IProjectTreeViewItem *projectItem = matchTab(nesicideProject, index);
    if (projectItem)
    {
        if (((IProjectTreeViewItem *)projectItem)->onCloseQuery())
        {
            ui->tabWidget->removeTab(index);
            ((IProjectTreeViewItem *)projectItem)->onClose();
        }
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
            emulator->setDialog ( emulatorDlg );
        }
        emulatorDlgTabIdx = ui->tabWidget->addTab(emulatorDlg, "Emulation Window");
        ui->tabWidget->setCurrentIndex(emulatorDlgTabIdx);
    }
    else
        ui->tabWidget->removeTab(emulatorDlgTabIdx);
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

        ui->projectTreeWidget->setModel(NULL);

        nesicideProject->deserialize(doc, doc);
        ui->projectTreeWidget->setModel(projectTreeviewModel);

        while (ui->tabWidget->currentIndex() >= 0)
            ui->tabWidget->removeTab(0);

        projectDataChangesEvent();
        projectFileName = fileName;
    }
}

void MainWindow::on_tabWidget_currentChanged(int index)
{
    IProjectTreeViewItem *projectItem = matchTab(nesicideProject, index);
    if (projectItem)
    {
        ui->actionSave_Active_Document->setEnabled(((IProjectTreeViewItem *)projectItem)->isDocumentSaveable());
    } else {
        ui->actionSave_Active_Document->setEnabled(false);
    }
}

void MainWindow::on_actionSave_Active_Document_triggered()
{
    IProjectTreeViewItem *projectItem = matchTab(nesicideProject, ui->tabWidget->currentIndex());
    if (projectItem)
        ((IProjectTreeViewItem *)projectItem)->onSaveDocument();

}

void MainWindow::on_compilerOutputDockWidget_visibilityChanged(bool visible)
{
    if (!visible)
    {
        if (!ui->compilerOutputTextEdit->isVisibleTo(this))
        {
            ui->actionCompiler_Output->setChecked(false);
        }
    } else
        ui->actionCompiler_Output->setChecked(visible);
}

void MainWindow::on_actionCompiler_Output_toggled(bool value)
{
    ui->compilerOutputDockWidget->setVisible(value);
}


void MainWindow::on_actionCompile_Project_triggered()
{
    CCartridgeBuilder cartridgeBuilder;
    cartridgeBuilder.build();
}

void MainWindow::on_actionCHRMEM_Inspector_toggled(bool value)
{
   m_pCHRMEMInspector->setVisible(value);
}

void MainWindow::chrmemInspector_close (bool toplevel)
{
   ui->actionCHRMEM_Inspector->setChecked(toplevel);
}

void MainWindow::on_actionOAM_Inspector_toggled(bool value)
{
   m_pOAMInspector->setVisible(value);
}

void MainWindow::oamInspector_close (bool toplevel)
{
   ui->actionOAM_Inspector->setChecked(toplevel);
}
