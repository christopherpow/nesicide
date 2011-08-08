#include "symbolwatchdockwidget.h"
#include "ui_symbolwatchdockwidget.h"

#include "main.h"

#include "ccc65interface.h"

enum
{
   SymbolWatchCol_Name,
   SymbolWatchCol_Address,
   SymbolWatchCol_Value,
   SymbolWatchCol_File
};

enum
{
   Symbol_Watch_Window,
   Symbol_RAM_Window
};

SymbolWatchDockWidget::SymbolWatchDockWidget(QWidget *parent) :
   CDebuggerBase(parent),
   ui(new Ui::SymbolWatchDockWidget)
{
   ui->setupUi(this);

   watchModel = new CSymbolWatchModel(true);
   watchSymbolDelegate = new CDebuggerSymbolDelegate();
   watchValueDelegate = new CDebuggerNumericItemDelegate();

   ramModel = new CSymbolWatchModel(false);
   ramValueDelegate = new CDebuggerNumericItemDelegate();

   ui->watch->setModel(watchModel);
   ui->watch->setItemDelegateForColumn(SymbolWatchCol_Name,watchSymbolDelegate);
   ui->watch->setItemDelegateForColumn(SymbolWatchCol_Value,watchValueDelegate);
   ui->watch->resizeColumnsToContents();

   ui->ram->setModel(ramModel);
   ui->ram->setItemDelegateForColumn(SymbolWatchCol_Value,ramValueDelegate);
   ui->ram->resizeColumnsToContents();

   QObject::connect(emulator,SIGNAL(cartridgeLoaded()),watchModel,SLOT(update()));
   QObject::connect(emulator,SIGNAL(emulatorReset()),watchModel,SLOT(update()));
   QObject::connect(emulator,SIGNAL(emulatorPaused(bool)),watchModel,SLOT(update()));
   QObject::connect(emulator,SIGNAL(updateDebuggers()),watchModel,SLOT(update()));
   QObject::connect(breakpointWatcher,SIGNAL(breakpointHit()),watchModel,SLOT(update()));
   QObject::connect(ui->watch->horizontalHeader(),SIGNAL(sortIndicatorChanged(int,Qt::SortOrder)),watchModel,SLOT(sort(int,Qt::SortOrder)));
   QObject::connect(watchModel,SIGNAL(rowsInserted(QModelIndex,int,int)),this,SLOT(updateUi()));

   QObject::connect(emulator,SIGNAL(cartridgeLoaded()),ramModel,SLOT(update()));
   QObject::connect(emulator,SIGNAL(emulatorReset()),ramModel,SLOT(update()));
   QObject::connect(emulator,SIGNAL(emulatorPaused(bool)),ramModel,SLOT(update()));
   QObject::connect(emulator,SIGNAL(updateDebuggers()),ramModel,SLOT(update()));
   QObject::connect(breakpointWatcher,SIGNAL(breakpointHit()),ramModel,SLOT(update()));
   QObject::connect(ui->ram->horizontalHeader(),SIGNAL(sortIndicatorChanged(int,Qt::SortOrder)),ramModel,SLOT(sort(int,Qt::SortOrder)));

   QObject::connect(emulator,SIGNAL(cartridgeLoaded()),this,SLOT(updateVariables()));
   QObject::connect(compiler,SIGNAL(compileDone(bool)),this,SLOT(updateVariables()));
}

SymbolWatchDockWidget::~SymbolWatchDockWidget()
{
   delete ui;
   delete watchModel;
   delete watchValueDelegate;
   delete watchSymbolDelegate;
   delete ramModel;
   delete ramValueDelegate;
}

void SymbolWatchDockWidget::updateUi()
{
   emit markProjectDirty(true);
}

void SymbolWatchDockWidget::updateVariables()
{
   QStringList symbols = CCC65Interface::getSymbolsForSourceFile("");
   int addr;

   ramModel->removeRows(0,ramModel->rowCount());

   foreach ( QString symbol,symbols )
   {
      addr = CCC65Interface::getSymbolAddress(symbol);
      if ( addr < MEM_2KB )
      {
         // Symbol is in RAM...
         ramModel->insertRow(symbol);
      }
   }

   ramModel->update();
}

void SymbolWatchDockWidget::keyPressEvent(QKeyEvent *event)
{
   if ( ui->tabWidget->currentIndex() == Symbol_Watch_Window )
   {
      if ( (event->key() == Qt::Key_Delete) &&
           (ui->watch->currentIndex().row() >= 0) )
      {
         watchModel->removeRow(ui->watch->currentIndex().row());

         emit markProjectDirty(true);
      }
   }
}

void SymbolWatchDockWidget::dragEnterEvent(QDragEnterEvent *event)
{
   QString text;

   if ( event->mimeData()->hasText() )
   {
      text = event->mimeData()->text();

      event->acceptProposedAction();
   }
}

void SymbolWatchDockWidget::dragMoveEvent(QDragMoveEvent *event)
{
   QString text;

   if ( event->mimeData()->hasText() )
   {
      text = event->mimeData()->text();

      event->acceptProposedAction();
   }
}

void SymbolWatchDockWidget::dropEvent(QDropEvent *event)
{
   QString text;

   if ( event->mimeData()->hasText() )
   {
      text = event->mimeData()->text();

      ui->tabWidget->setCurrentIndex(Symbol_Watch_Window);

      watchModel->insertRow(text);

      emit markProjectDirty(true);

      event->acceptProposedAction();
   }
}

void SymbolWatchDockWidget::contextMenuEvent(QContextMenuEvent *event)
{
   QModelIndex index = ui->watch->currentIndex();
   QMenu menu;

   if ( index.isValid() )
   {
      if ( ui->tabWidget->currentIndex() == Symbol_Watch_Window )
      {
         menu.addAction(ui->actionRemove_symbol);
         menu.addSeparator();
      }
      menu.addAction(ui->actionBreak_on_CPU_access_here);
      menu.addAction(ui->actionBreak_on_CPU_read_here);
      menu.addAction(ui->actionBreak_on_CPU_write_here);

      menu.exec(event->globalPos());
   }
}

void SymbolWatchDockWidget::showEvent(QShowEvent*)
{
   watchModel->update();
   ramModel->update();
}

void SymbolWatchDockWidget::hideEvent(QHideEvent *event)
{
}

bool SymbolWatchDockWidget::serialize(QDomDocument& doc, QDomNode& node)
{
   QDomElement element = addElement( doc, node, "symbolinspector" );
   QList<WatchedItem> items = watchModel->getItems();

   for (int i=0; i < items.count(); i++)
   {
      QDomElement symbolElement = addElement( doc, element, "symbol" );
      symbolElement.setAttribute("name",items.at(i).symbol);
      symbolElement.setAttribute("segment",items.at(i).segment);
   }

   return true;
}

bool SymbolWatchDockWidget::deserialize(QDomDocument& doc, QDomNode& node, QString& errors)
{
   QDomNode childNode = node.firstChild();
   QDomNode symbolNode;
   QList<WatchedItem> items;
   WatchedItem item;

   if (!childNode.isNull())
   {
      do
      {
         if (childNode.nodeName() == "symbolinspector")
         {
            symbolNode = childNode.firstChild();
            while ( !(symbolNode.isNull()) )
            {
               QDomElement symbolElement = symbolNode.toElement();

               item.symbol = symbolElement.attribute("name");
               item.segment = symbolElement.attribute("segment","0").toInt();

               if ( !item.symbol.isEmpty() )
               {
                  items.append(item);
               }
               symbolNode = symbolNode.nextSibling();
            }

            watchModel->setItems(items);
            watchModel->update();
         }
      } while (!(childNode = childNode.nextSibling()).isNull());
   }

   return true;
}

void SymbolWatchDockWidget::on_actionBreak_on_CPU_write_here_triggered()
{
   CBreakpointInfo* pBreakpoints = nesGetBreakpointDatabase();
   int row;
   QModelIndex index;
   int addr;
   int bpIdx;
   bool ok;

   switch ( ui->tabWidget->currentIndex() )
   {
   case Symbol_Watch_Window:
      row = ui->watch->currentIndex().row();
      index = watchModel->index(row,SymbolWatchCol_Address);
      addr = index.data(Qt::DisplayRole).toString().toInt(&ok,16);
      break;
   case Symbol_RAM_Window:
      row = ui->ram->currentIndex().row();
      index = ramModel->index(row,SymbolWatchCol_Address);
      addr = index.data(Qt::DisplayRole).toString().toInt(&ok,16);
      break;
   }

   if ( ok )
   {
      bpIdx = pBreakpoints->AddBreakpoint ( eBreakOnCPUMemoryWrite,
                                            eBreakpointItemAddress,
                                            0,
                                            addr,
                                            addr,
                                            addr,
                                            eBreakpointConditionTest,
                                            0,
                                            eBreakpointDataPure,
                                            0,
                                            true );

      if ( bpIdx < 0 )
      {
         QString str;
         str.sprintf("Cannot add breakpoint, maximum of %d already used.", NUM_BREAKPOINTS);
         QMessageBox::information(0, "Error", str);
      }
      else
      {
         emit breakpointsChanged();
         emit markProjectDirty(true);
      }
   }
}

void SymbolWatchDockWidget::on_actionBreak_on_CPU_read_here_triggered()
{
   CBreakpointInfo* pBreakpoints = nesGetBreakpointDatabase();
   int row;
   QModelIndex index;
   int addr;
   int bpIdx;
   bool ok;

   switch ( ui->tabWidget->currentIndex() )
   {
   case Symbol_Watch_Window:
      row = ui->watch->currentIndex().row();
      index = watchModel->index(row,SymbolWatchCol_Address);
      addr = index.data(Qt::DisplayRole).toString().toInt(&ok,16);
      break;
   case Symbol_RAM_Window:
      row = ui->ram->currentIndex().row();
      index = ramModel->index(row,SymbolWatchCol_Address);
      addr = index.data(Qt::DisplayRole).toString().toInt(&ok,16);
      break;
   }

   if ( ok )
   {
      bpIdx = pBreakpoints->AddBreakpoint ( eBreakOnCPUMemoryRead,
                                            eBreakpointItemAddress,
                                            0,
                                            addr,
                                            addr,
                                            addr,
                                            eBreakpointConditionTest,
                                            0,
                                            eBreakpointDataPure,
                                            0,
                                            true );

      if ( bpIdx < 0 )
      {
         QString str;
         str.sprintf("Cannot add breakpoint, maximum of %d already used.", NUM_BREAKPOINTS);
         QMessageBox::information(0, "Error", str);
      }
      else
      {
         emit breakpointsChanged();
         emit markProjectDirty(true);
      }
   }
}

void SymbolWatchDockWidget::on_actionBreak_on_CPU_access_here_triggered()
{
   CBreakpointInfo* pBreakpoints = nesGetBreakpointDatabase();
   int row;
   QModelIndex index;
   int addr;
   int bpIdx;
   bool ok;

   switch ( ui->tabWidget->currentIndex() )
   {
   case Symbol_Watch_Window:
      row = ui->watch->currentIndex().row();
      index = watchModel->index(row,SymbolWatchCol_Address);
      addr = index.data(Qt::DisplayRole).toString().toInt(&ok,16);
      break;
   case Symbol_RAM_Window:
      row = ui->ram->currentIndex().row();
      index = ramModel->index(row,SymbolWatchCol_Address);
      addr = index.data(Qt::DisplayRole).toString().toInt(&ok,16);
      break;
   }

   if ( ok )
   {
      bpIdx = pBreakpoints->AddBreakpoint ( eBreakOnCPUMemoryAccess,
                                            eBreakpointItemAddress,
                                            0,
                                            addr,
                                            addr,
                                            addr,
                                            eBreakpointConditionTest,
                                            0,
                                            eBreakpointDataPure,
                                            0,
                                            true );

      if ( bpIdx < 0 )
      {
         QString str;
         str.sprintf("Cannot add breakpoint, maximum of %d already used.", NUM_BREAKPOINTS);
         QMessageBox::information(0, "Error", str);
      }
      else
      {
         emit breakpointsChanged();
         emit markProjectDirty(true);
      }
   }
}

void SymbolWatchDockWidget::on_actionRemove_symbol_triggered()
{
   if ( ui->watch->currentIndex().isValid() )
   {
      watchModel->removeRow(ui->watch->currentIndex().row());

      emit markProjectDirty(true);
   }
}
