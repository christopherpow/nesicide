#include "executionmarkerdockwidget.h"
#include "ui_executionmarkerdockwidget.h"

#include "cdockwidgetregistry.h"

#include "main.h"

ExecutionMarkerDockWidget::ExecutionMarkerDockWidget(QWidget *parent) :
   CDebuggerBase(parent),
   ui(new Ui::ExecutionMarkerDockWidget)
{
   ui->setupUi(this);

   model = new CExecutionMarkerDisplayModel();

   ui->tableView->setModel(model);
   ui->tableView->resizeColumnsToContents();
}

ExecutionMarkerDockWidget::~ExecutionMarkerDockWidget()
{
   delete ui;
   delete model;
}

void ExecutionMarkerDockWidget::showEvent(QShowEvent* e)
{
   QDockWidget* breakpointInspector = CDockWidgetRegistry::getWidget("Breakpoints");
   QDockWidget* codeBrowser = CDockWidgetRegistry::getWidget("Assembly Browser");

   QObject::connect(codeBrowser,SIGNAL(breakpointsChanged()),model, SLOT(update()) );
   QObject::connect ( emulator, SIGNAL(updateDebuggers()), model, SLOT(update()));
   QObject::connect ( breakpointInspector, SIGNAL(breakpointsChanged()), model, SLOT(update()) );
}
