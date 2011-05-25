#include "symbolwatchdockwidget.h"
#include "ui_symbolwatchdockwidget.h"

#include "main.h"

enum
{
   SymbolWatchCol_Name,
   SymbolWatchCol_Address,
   SymbolWatchCol_Value
};

SymbolWatchDockWidget::SymbolWatchDockWidget(QWidget *parent) :
   QDockWidget(parent),
   ui(new Ui::SymbolWatchDockWidget)
{
   ui->setupUi(this);
   model = new CSymbolWatchModel();
   symbolDelegate = new CDebuggerSymbolDelegate();
   valueDelegate = new CDebuggerNumericItemDelegate();

   ui->tableView->setModel(model);
   ui->tableView->setItemDelegateForColumn(SymbolWatchCol_Name,symbolDelegate);
   ui->tableView->setItemDelegateForColumn(SymbolWatchCol_Value,valueDelegate);

   QObject::connect(emulator,SIGNAL(cartridgeLoaded()),model,SLOT(update()));
   QObject::connect(emulator,SIGNAL(emulatorReset()),model,SLOT(update()));
   QObject::connect(emulator,SIGNAL(emulatorPaused(bool)),model,SLOT(update()));
   QObject::connect(emulator,SIGNAL(updateDebuggers()),model,SLOT(update()));
   QObject::connect(breakpointWatcher,SIGNAL(breakpointHit()),model,SLOT(update()));
}

SymbolWatchDockWidget::~SymbolWatchDockWidget()
{
   delete ui;
   delete model;
   delete valueDelegate;
   delete symbolDelegate;
}

void SymbolWatchDockWidget::keyPressEvent(QKeyEvent *e)
{
   if ( (e->key() == Qt::Key_Delete) &&
        (ui->tableView->currentIndex().row() >= 0) )
   {
      model->removeRow(ui->tableView->currentIndex().row(),QModelIndex());
   }
}
