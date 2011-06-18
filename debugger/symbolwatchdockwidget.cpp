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
   CDebuggerBase(parent),
   ui(new Ui::SymbolWatchDockWidget)
{
   ui->setupUi(this);
   model = new CSymbolWatchModel();
   symbolDelegate = new CDebuggerSymbolDelegate();
   valueDelegate = new CDebuggerNumericItemDelegate();

   ui->tableView->setModel(model);
   ui->tableView->setItemDelegateForColumn(SymbolWatchCol_Name,symbolDelegate);
   ui->tableView->setItemDelegateForColumn(SymbolWatchCol_Value,valueDelegate);
   ui->tableView->resizeColumnsToContents();

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

void SymbolWatchDockWidget::updateUi()
{
   emit markProjectDirty(true);
}

void SymbolWatchDockWidget::keyPressEvent(QKeyEvent *event)
{
   if ( (event->key() == Qt::Key_Delete) &&
        (ui->tableView->currentIndex().row() >= 0) )
   {
      model->removeRow(ui->tableView->currentIndex().row(),QModelIndex());

      emit markProjectDirty(true);
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

      model->insertRow(text,QModelIndex());

      emit markProjectDirty(true);

      event->acceptProposedAction();
   }
}

void SymbolWatchDockWidget::contextMenuEvent(QContextMenuEvent *event)
{
   QModelIndex index = ui->tableView->currentIndex();
   QMenu menu;

   if ( index.isValid() )
   {
      menu.addAction(ui->actionRemove_symbol);
      menu.addSeparator();
      menu.addAction(ui->actionBreak_on_CPU_access_here);
      menu.addAction(ui->actionBreak_on_CPU_read_here);
      menu.addAction(ui->actionBreak_on_CPU_write_here);

      menu.exec(event->globalPos());
   }
}

void SymbolWatchDockWidget::showEvent(QShowEvent*)
{
   model->update();
}

void SymbolWatchDockWidget::hideEvent(QHideEvent *event)
{
}

bool SymbolWatchDockWidget::serialize(QDomDocument& doc, QDomNode& node)
{
   QDomElement element = addElement( doc, node, "symbolinspector" );
   QStringList symbols = model->getSymbols();

   for (int i=0; i < symbols.count(); i++)
   {
      QDomElement symbolElement = addElement( doc, element, "symbol" );
      symbolElement.setAttribute("name",symbols.at(i));
   }

   return true;
}

bool SymbolWatchDockWidget::deserialize(QDomDocument& doc, QDomNode& node, QString& errors)
{
   QDomNode childNode = node.firstChild();
   QDomNode symbolNode;
   QStringList symbols;
   QString symbol;

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

               symbol = symbolElement.attribute("name");
               if ( !symbol.isEmpty() )
               {
                  symbols.append(symbol);
               }
               symbolNode = symbolNode.nextSibling();
            }

            model->setSymbols(symbols);
            model->update();
         }
      } while (!(childNode = childNode.nextSibling()).isNull());
   }

   return true;
}

void SymbolWatchDockWidget::on_actionBreak_on_CPU_write_here_triggered()
{
   CBreakpointInfo* pBreakpoints = nesGetBreakpointDatabase();
   int row = ui->tableView->currentIndex().row();
   QModelIndex index = model->index(row,SymbolWatchCol_Address);
   bool ok;
   int addr = index.data(Qt::DisplayRole).toString().toInt(&ok,16);
   int bpIdx;

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
   int row = ui->tableView->currentIndex().row();
   QModelIndex index = model->index(row,SymbolWatchCol_Address);
   bool ok;
   int addr = index.data(Qt::DisplayRole).toString().toInt(&ok,16);
   int bpIdx;

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
   int row = ui->tableView->currentIndex().row();
   QModelIndex index = model->index(row,SymbolWatchCol_Address);
   bool ok;
   int addr = index.data(Qt::DisplayRole).toString().toInt(&ok,16);
   int bpIdx;

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
   if ( ui->tableView->currentIndex().isValid() )
   {
      model->removeRow(ui->tableView->currentIndex().row(),QModelIndex());

      emit markProjectDirty(true);
   }
}
