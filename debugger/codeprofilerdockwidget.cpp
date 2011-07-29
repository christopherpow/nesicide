#include "codeprofilerdockwidget.h"
#include "ui_codeprofilerdockwidget.h"

#include "main.h"

CodeProfilerDockWidget::CodeProfilerDockWidget(QWidget *parent) :
    CDebuggerBase(parent),
    ui(new Ui::CodeProfilerDockWidget)
{
   ui->setupUi(this);
   model = new CDebuggerCodeProfilerModel();

   ui->tableView->setModel(model);
   ui->tableView->resizeColumnsToContents();

   QObject::connect(emulator,SIGNAL(cartridgeLoaded()),model,SLOT(update()));
   QObject::connect(emulator,SIGNAL(emulatorReset()),model,SLOT(update()));
   QObject::connect(emulator,SIGNAL(emulatorPaused(bool)),model,SLOT(update()));
   QObject::connect(emulator,SIGNAL(updateDebuggers()),model,SLOT(update()));
   QObject::connect(breakpointWatcher,SIGNAL(breakpointHit()),model,SLOT(update()));
}

CodeProfilerDockWidget::~CodeProfilerDockWidget()
{
    delete ui;
}
