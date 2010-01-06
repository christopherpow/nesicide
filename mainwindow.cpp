#include "mainwindow.h"
#include "ui_mainwindow.h"

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

    m_pGfxCHRMemoryInspector = new CHRMEMInspector ();
    m_pGfxCHRMemoryInspector->setFeatures(QDockWidget::DockWidgetClosable|QDockWidget::DockWidgetFloatable|QDockWidget::DockWidgetMovable);
    m_pGfxCHRMemoryInspector->setWindowTitle("Graphical CHR Memory Inspector");
    m_pGfxCHRMemoryInspector->setAllowedAreas(Qt::AllDockWidgetAreas);
    addDockWidget(Qt::BottomDockWidgetArea, m_pGfxCHRMemoryInspector );
    m_pGfxCHRMemoryInspector->hide();
    QObject::connect(m_pGfxCHRMemoryInspector, SIGNAL(visibilityChanged(bool)), this, SLOT(reflectedGfxCHRMemoryInspector_close(bool)));

    m_pGfxOAMMemoryInspector = new OAMInspector ();
    m_pGfxOAMMemoryInspector->setFeatures(QDockWidget::DockWidgetClosable|QDockWidget::DockWidgetFloatable|QDockWidget::DockWidgetMovable);
    m_pGfxOAMMemoryInspector->setWindowTitle("Graphical OAM Inspector");
    m_pGfxOAMMemoryInspector->setAllowedAreas(Qt::AllDockWidgetAreas);
    addDockWidget(Qt::BottomDockWidgetArea, m_pGfxOAMMemoryInspector );
    m_pGfxOAMMemoryInspector->hide();
    QObject::connect(m_pGfxOAMMemoryInspector, SIGNAL(visibilityChanged(bool)), this, SLOT(reflectedGfxOAMMemoryInspector_close(bool)));

    m_pGfxNameTableMemoryInspector = new NameTableInspector ();
    m_pGfxNameTableMemoryInspector->setFeatures(QDockWidget::DockWidgetClosable|QDockWidget::DockWidgetFloatable|QDockWidget::DockWidgetMovable);
    m_pGfxNameTableMemoryInspector->setWindowTitle("Graphical Name Table Inspector");
    m_pGfxNameTableMemoryInspector->setAllowedAreas(Qt::AllDockWidgetAreas);
    addDockWidget(Qt::RightDockWidgetArea, m_pGfxNameTableMemoryInspector );
    m_pGfxNameTableMemoryInspector->hide();
    QObject::connect(m_pGfxNameTableMemoryInspector, SIGNAL(visibilityChanged(bool)), this, SLOT(reflectedGfxNameTableMemoryInspector_close(bool)));

    m_pExecutionInspector = new ExecutionInspector();
    m_pExecutionInspector->setFeatures(QDockWidget::DockWidgetClosable|QDockWidget::DockWidgetFloatable|QDockWidget::DockWidgetMovable);
    m_pExecutionInspector->setWindowTitle("Execution Inspector");
    m_pExecutionInspector->setAllowedAreas(Qt::AllDockWidgetAreas);
    addDockWidget(Qt::BottomDockWidgetArea, m_pExecutionInspector );
    m_pExecutionInspector->hide();
    QObject::connect(m_pExecutionInspector, SIGNAL(visibilityChanged(bool)), this, SLOT(reflectedExecutionInspector_close(bool)));

    m_pBinCPURegisterInspector = new RegisterInspector(eMemory_CPUregs);
    m_pBinCPURegisterInspector->setFeatures(QDockWidget::DockWidgetClosable|QDockWidget::DockWidgetFloatable|QDockWidget::DockWidgetMovable);
    m_pBinCPURegisterInspector->setWindowTitle("CPU Register Inspector");
    m_pBinCPURegisterInspector->setAllowedAreas(Qt::AllDockWidgetAreas);
    addDockWidget(Qt::BottomDockWidgetArea, m_pBinCPURegisterInspector );
    m_pBinCPURegisterInspector->hide();
    QObject::connect(m_pBinCPURegisterInspector, SIGNAL(visibilityChanged(bool)), this, SLOT(reflectedBinCPURegisterInspector_close(bool)));

    m_pBinCPUMemoryInspector = new MemoryInspector(eMemory_CPU);
    m_pBinCPUMemoryInspector->setFeatures(QDockWidget::DockWidgetClosable|QDockWidget::DockWidgetFloatable|QDockWidget::DockWidgetMovable);
    m_pBinCPUMemoryInspector->setWindowTitle("CPU Memory Inspector");
    m_pBinCPUMemoryInspector->setAllowedAreas(Qt::AllDockWidgetAreas);
    addDockWidget(Qt::BottomDockWidgetArea, m_pBinCPUMemoryInspector );
    m_pBinCPUMemoryInspector->hide();
    QObject::connect(m_pBinCPUMemoryInspector, SIGNAL(visibilityChanged(bool)), this, SLOT(reflectedBinCPUMemoryInspector_close(bool)));

    m_pBinPPUMemoryInspector = new MemoryInspector(eMemory_PPU);
    m_pBinPPUMemoryInspector->setFeatures(QDockWidget::DockWidgetClosable|QDockWidget::DockWidgetFloatable|QDockWidget::DockWidgetMovable);
    m_pBinPPUMemoryInspector->setWindowTitle("PPU Memory Inspector");
    m_pBinPPUMemoryInspector->setAllowedAreas(Qt::AllDockWidgetAreas);
    addDockWidget(Qt::BottomDockWidgetArea, m_pBinPPUMemoryInspector );
    m_pBinPPUMemoryInspector->hide();
    QObject::connect(m_pBinPPUMemoryInspector, SIGNAL(visibilityChanged(bool)), this, SLOT(reflectedBinPPUMemoryInspector_close(bool)));

    m_pBinPPURegisterInspector = new RegisterInspector(eMemory_PPUregs);
    m_pBinPPURegisterInspector->setFeatures(QDockWidget::DockWidgetClosable|QDockWidget::DockWidgetFloatable|QDockWidget::DockWidgetMovable);
    m_pBinPPURegisterInspector->setWindowTitle("PPU Register Inspector");
    m_pBinPPURegisterInspector->setAllowedAreas(Qt::AllDockWidgetAreas);
    addDockWidget(Qt::BottomDockWidgetArea, m_pBinPPURegisterInspector );
    m_pBinPPURegisterInspector->hide();
    QObject::connect(m_pBinPPURegisterInspector, SIGNAL(visibilityChanged(bool)), this, SLOT(reflectedBinPPURegisterInspector_close(bool)));

    m_pBinAPURegisterInspector = new RegisterInspector(eMemory_IOregs);
    m_pBinAPURegisterInspector->setFeatures(QDockWidget::DockWidgetClosable|QDockWidget::DockWidgetFloatable|QDockWidget::DockWidgetMovable);
    m_pBinAPURegisterInspector->setWindowTitle("APU Register Inspector");
    m_pBinAPURegisterInspector->setAllowedAreas(Qt::AllDockWidgetAreas);
    addDockWidget(Qt::BottomDockWidgetArea, m_pBinAPURegisterInspector );
    m_pBinAPURegisterInspector->hide();
    QObject::connect(m_pBinAPURegisterInspector, SIGNAL(visibilityChanged(bool)), this, SLOT(reflectedBinAPURegisterInspector_close(bool)));

    m_pBinCHRMemoryInspector = new MemoryInspector(eMemory_cartCHRMEM);
    m_pBinCHRMemoryInspector->setFeatures(QDockWidget::DockWidgetClosable|QDockWidget::DockWidgetFloatable|QDockWidget::DockWidgetMovable);
    m_pBinCHRMemoryInspector->setWindowTitle("CHR Memory Inspector");
    m_pBinCHRMemoryInspector->setAllowedAreas(Qt::AllDockWidgetAreas);
    addDockWidget(Qt::BottomDockWidgetArea, m_pBinCHRMemoryInspector );
    m_pBinCHRMemoryInspector->hide();
    QObject::connect(m_pBinCHRMemoryInspector, SIGNAL(visibilityChanged(bool)), this, SLOT(reflectedBinCHRMemoryInspector_close(bool)));

    m_pBinOAMMemoryInspector = new RegisterInspector(eMemory_PPUoam);
    m_pBinOAMMemoryInspector->setFeatures(QDockWidget::DockWidgetClosable|QDockWidget::DockWidgetFloatable|QDockWidget::DockWidgetMovable);
    m_pBinOAMMemoryInspector->setWindowTitle("OAM Memory Inspector");
    m_pBinOAMMemoryInspector->setAllowedAreas(Qt::AllDockWidgetAreas);
    addDockWidget(Qt::BottomDockWidgetArea, m_pBinOAMMemoryInspector );
    m_pBinOAMMemoryInspector->hide();
    QObject::connect(m_pBinOAMMemoryInspector, SIGNAL(visibilityChanged(bool)), this, SLOT(reflectedBinOAMMemoryInspector_close(bool)));

    m_pBinPaletteMemoryInspector = new MemoryInspector(eMemory_PPUpalette);
    m_pBinPaletteMemoryInspector->setFeatures(QDockWidget::DockWidgetClosable|QDockWidget::DockWidgetFloatable|QDockWidget::DockWidgetMovable);
    m_pBinPaletteMemoryInspector->setWindowTitle("Palette Memory Inspector");
    m_pBinPaletteMemoryInspector->setAllowedAreas(Qt::AllDockWidgetAreas);
    addDockWidget(Qt::BottomDockWidgetArea, m_pBinPaletteMemoryInspector );
    m_pBinPaletteMemoryInspector->hide();
    QObject::connect(m_pBinPaletteMemoryInspector, SIGNAL(visibilityChanged(bool)), this, SLOT(reflectedBinPaletteMemoryInspector_close(bool)));

    m_pBinSRAMMemoryInspector = new MemoryInspector(eMemory_cartSRAM);
    m_pBinSRAMMemoryInspector->setFeatures(QDockWidget::DockWidgetClosable|QDockWidget::DockWidgetFloatable|QDockWidget::DockWidgetMovable);
    m_pBinSRAMMemoryInspector->setWindowTitle("Cartridge SRAM Memory Inspector");
    m_pBinSRAMMemoryInspector->setAllowedAreas(Qt::AllDockWidgetAreas);
    addDockWidget(Qt::BottomDockWidgetArea, m_pBinSRAMMemoryInspector );
    m_pBinSRAMMemoryInspector->hide();
    QObject::connect(m_pBinSRAMMemoryInspector, SIGNAL(visibilityChanged(bool)), this, SLOT(reflectedBinSRAMMemoryInspector_close(bool)));

    m_pBinEXRAMMemoryInspector = new MemoryInspector(eMemory_cartEXRAM);
    m_pBinEXRAMMemoryInspector->setFeatures(QDockWidget::DockWidgetClosable|QDockWidget::DockWidgetFloatable|QDockWidget::DockWidgetMovable);
    m_pBinEXRAMMemoryInspector->setWindowTitle("Cartridge EXRAM Memory Inspector");
    m_pBinEXRAMMemoryInspector->setAllowedAreas(Qt::AllDockWidgetAreas);
    addDockWidget(Qt::BottomDockWidgetArea, m_pBinEXRAMMemoryInspector );
    m_pBinEXRAMMemoryInspector->hide();
    QObject::connect(m_pBinEXRAMMemoryInspector, SIGNAL(visibilityChanged(bool)), this, SLOT(reflectedBinEXRAMMemoryInspector_close(bool)));

    m_pBinMapperMemoryInspector = new RegisterInspector(eMemory_cartMapper);
    m_pBinMapperMemoryInspector->setFeatures(QDockWidget::DockWidgetClosable|QDockWidget::DockWidgetFloatable|QDockWidget::DockWidgetMovable);
    m_pBinMapperMemoryInspector->setWindowTitle("Cartridge Mapper Inspector");
    m_pBinMapperMemoryInspector->setAllowedAreas(Qt::AllDockWidgetAreas);
    addDockWidget(Qt::BottomDockWidgetArea, m_pBinMapperMemoryInspector );
    m_pBinMapperMemoryInspector->hide();
    QObject::connect(m_pBinMapperMemoryInspector, SIGNAL(visibilityChanged(bool)), this, SLOT(reflectedBinMapperMemoryInspector_close(bool)));

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
    ui->actionEmulation_Window->setEnabled ( nesicideProject->get_isInitialized() );
    ui->actionExecution_Inspector->setEnabled ( nesicideProject->get_isInitialized() );
    ui->actionGfxCHRMemory_Inspector->setEnabled ( nesicideProject->get_isInitialized() );
    ui->actionGfxOAMMemory_Inspector->setEnabled ( nesicideProject->get_isInitialized() );
    ui->actionGfxNameTableMemory_Inspector->setEnabled ( nesicideProject->get_isInitialized() );
    ui->actionBinCPURegister_Inspector->setEnabled ( nesicideProject->get_isInitialized() );
    ui->actionBinCPUMemory_Inspector->setEnabled ( nesicideProject->get_isInitialized() );
    ui->actionBinPPUMemory_Inspector->setEnabled ( nesicideProject->get_isInitialized() );
    ui->actionBinCHRMemory_Inspector->setEnabled ( nesicideProject->get_isInitialized() );
    ui->actionBinOAMMemory_Inspector->setEnabled ( nesicideProject->get_isInitialized() );
    ui->actionBinPaletteMemory_Inspector->setEnabled ( nesicideProject->get_isInitialized() );
    ui->actionBinSRAMMemory_Inspector->setEnabled ( nesicideProject->get_isInitialized() );
    ui->actionBinEXRAMMemory_Inspector->setEnabled ( nesicideProject->get_isInitialized() );
    ui->actionBinPPURegister_Inspector->setEnabled ( nesicideProject->get_isInitialized() );
    ui->actionBinAPURegister_Inspector->setEnabled ( nesicideProject->get_isInitialized() );
    ui->actionBinMapperMemory_Inspector->setEnabled ( nesicideProject->get_isInitialized() );

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

void MainWindow::on_actionExecution_Inspector_toggled(bool value)
{
   m_pExecutionInspector->setVisible(value);
}

void MainWindow::reflectedExecutionInspector_close (bool toplevel)
{
   ui->actionExecution_Inspector->setChecked(toplevel);
}

void MainWindow::on_actionGfxCHRMemory_Inspector_toggled(bool value)
{
   m_pGfxCHRMemoryInspector->setVisible(value);
}

void MainWindow::reflectedGfxCHRMemoryInspector_close (bool toplevel)
{
   ui->actionGfxCHRMemory_Inspector->setChecked(toplevel);
}

void MainWindow::on_actionGfxOAMMemory_Inspector_toggled(bool value)
{
   m_pGfxOAMMemoryInspector->setVisible(value);
}

void MainWindow::reflectedGfxOAMMemoryInspector_close (bool toplevel)
{
   ui->actionGfxOAMMemory_Inspector->setChecked(toplevel);
}

void MainWindow::on_actionGfxNameTableMemory_Inspector_toggled(bool value)
{
   m_pGfxNameTableMemoryInspector->setVisible(value);
}

void MainWindow::reflectedGfxNameTableMemoryInspector_close (bool toplevel)
{
   ui->actionGfxNameTableMemory_Inspector->setChecked(toplevel);
}

void MainWindow::on_actionBinOAMMemory_Inspector_toggled(bool value)
{
   m_pBinOAMMemoryInspector->setVisible(value);
}

void MainWindow::reflectedBinOAMMemoryInspector_close (bool toplevel)
{
   ui->actionBinOAMMemory_Inspector->setChecked(toplevel);
}

void MainWindow::on_actionBinCPURegister_Inspector_toggled(bool value)
{
   m_pBinCPURegisterInspector->setVisible(value);
}

void MainWindow::reflectedBinCPURegisterInspector_close ( bool toplevel )
{
   ui->actionBinCPURegister_Inspector->setChecked(toplevel);
}

void MainWindow::on_actionBinCPUMemory_Inspector_toggled(bool value)
{
   m_pBinCPUMemoryInspector->setVisible(value);
}

void MainWindow::reflectedBinCPUMemoryInspector_close (bool toplevel)
{
   ui->actionBinCPUMemory_Inspector->setChecked(toplevel);
}

void MainWindow::on_actionBinPPUMemory_Inspector_toggled(bool value)
{
   m_pBinPPUMemoryInspector->setVisible(value);
}

void MainWindow::reflectedBinPPUMemoryInspector_close (bool toplevel)
{
   ui->actionBinPPUMemory_Inspector->setChecked(toplevel);
}

void MainWindow::on_actionBinPaletteMemory_Inspector_toggled(bool value)
{
   m_pBinPaletteMemoryInspector->setVisible(value);
}

void MainWindow::reflectedBinPaletteMemoryInspector_close (bool toplevel)
{
   ui->actionBinPaletteMemory_Inspector->setChecked(toplevel);
}

void MainWindow::on_actionBinPPURegister_Inspector_toggled(bool value)
{
   m_pBinPPURegisterInspector->setVisible(value);
}

void MainWindow::reflectedBinPPURegisterInspector_close ( bool toplevel )
{
   ui->actionBinPPURegister_Inspector->setChecked(toplevel);
}

void MainWindow::on_actionBinAPURegister_Inspector_toggled(bool value)
{
   m_pBinAPURegisterInspector->setVisible(value);
}

void MainWindow::reflectedBinAPURegisterInspector_close ( bool toplevel )
{
   ui->actionBinAPURegister_Inspector->setChecked(toplevel);
}

void MainWindow::on_actionBinCHRMemory_Inspector_toggled(bool value)
{
   m_pBinCHRMemoryInspector->setVisible(value);
}

void MainWindow::reflectedBinCHRMemoryInspector_close ( bool toplevel )
{
   ui->actionBinCHRMemory_Inspector->setChecked(toplevel);
}

void MainWindow::on_actionBinSRAMMemory_Inspector_toggled(bool value)
{
   m_pBinSRAMMemoryInspector->setVisible(value);
}

void MainWindow::reflectedBinSRAMMemoryInspector_close ( bool toplevel )
{
   ui->actionBinSRAMMemory_Inspector->setChecked(toplevel);
}

void MainWindow::on_actionBinEXRAMMemory_Inspector_toggled(bool value)
{
   m_pBinEXRAMMemoryInspector->setVisible(value);
}

void MainWindow::reflectedBinEXRAMMemoryInspector_close ( bool toplevel )
{
   ui->actionBinEXRAMMemory_Inspector->setChecked(toplevel);
}

void MainWindow::on_actionBinMapperMemory_Inspector_toggled(bool value)
{
   m_pBinMapperMemoryInspector->setVisible(value);
}

void MainWindow::reflectedBinMapperMemoryInspector_close ( bool toplevel )
{
   ui->actionBinMapperMemory_Inspector->setChecked(toplevel);
}

