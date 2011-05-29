#include <QStandardItemModel>

#include "breakpointdockwidget.h"
#include "ui_breakpointdockwidget.h"

#include "dbg_cnes.h"
#include "dbg_cnesrom.h"
#include "dbg_cnes6502.h"
#include "dbg_cnesppu.h"
#include "dbg_cnesapu.h"

#include "breakpointdialog.h"

#include "cdockwidgetregistry.h"

#include "ccc65interface.h"

#include "main.h"

BreakpointDockWidget::BreakpointDockWidget(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::BreakpointDockWidget)
{
   ui->setupUi(this);

   model = new CBreakpointDisplayModel();
   ui->tableView->setModel ( model );

   QObject::connect(breakpointWatcher,SIGNAL(breakpointHit()),this,SLOT(updateData()) );

   QObject::connect(breakpointWatcher,SIGNAL(breakpointHit()),model,SLOT(update()));
   QObject::connect(emulator,SIGNAL(breakpointClear()),model,SLOT(update()));
   QObject::connect(emulator,SIGNAL(cartridgeLoaded()),model,SLOT(update()));
   QObject::connect(emulator,SIGNAL(emulatorReset()),model,SLOT(update()));
   QObject::connect(emulator,SIGNAL(emulatorPaused(bool)),model,SLOT(update()));
   QObject::connect(this,SIGNAL(breakpointsChanged()),model,SLOT(update()));
}

BreakpointDockWidget::~BreakpointDockWidget()
{
   delete ui;
   delete model;
}

void BreakpointDockWidget::changeEvent(QEvent* e)
{
   QDockWidget::changeEvent(e);

   switch (e->type())
   {
      case QEvent::LanguageChange:
         ui->retranslateUi(this);
         break;
      default:
         break;
   }
}

void BreakpointDockWidget::showEvent(QShowEvent*)
{
   QDockWidget* codeBrowser = CDockWidgetRegistry::getWidget("Code Browser");
   QDockWidget* symbolInspector = CDockWidgetRegistry::getWidget("Symbol Inspector");
   QDockWidget* memoryInspector;

   QObject::connect(codeBrowser,SIGNAL(breakpointsChanged()),model, SLOT(update()) );
   QObject::connect(symbolInspector,SIGNAL(breakpointsChanged()),model, SLOT(update()) );

   memoryInspector = CDockWidgetRegistry::getWidget("CPU RAM Inspector");
   QObject::connect(memoryInspector,SIGNAL(breakpointsChanged()),model, SLOT(update()) );
   memoryInspector = CDockWidgetRegistry::getWidget("Cartridge EXRAM Memory Inspector");
   QObject::connect(memoryInspector,SIGNAL(breakpointsChanged()),model, SLOT(update()) );
   memoryInspector = CDockWidgetRegistry::getWidget("Cartridge SRAM Memory Inspector");
   QObject::connect(memoryInspector,SIGNAL(breakpointsChanged()),model, SLOT(update()) );
   memoryInspector = CDockWidgetRegistry::getWidget("PRG-ROM Inspector");
   QObject::connect(memoryInspector,SIGNAL(breakpointsChanged()),model, SLOT(update()) );
   model->update();
   updateData();
}

void BreakpointDockWidget::contextMenuEvent(QContextMenuEvent *e)
{
   CBreakpointInfo* pBreakpoints = nesGetBreakpointDatabase();
   QMenu menu;

   if ( ui->tableView->currentIndex().row() >= 0 )
   {
      menu.addAction(ui->actionEdit_Breakpoint);

      BreakpointInfo* pBreakpoint = pBreakpoints->GetBreakpoint(ui->tableView->currentIndex().row());
      if ( pBreakpoint->enabled )
      {
         menu.addAction(ui->actionDisable_Breakpoint);
      }
      else
      {
         menu.addAction(ui->actionEnable_Breakpoint);
      }
      menu.addSeparator();
   }
   if ( pBreakpoints->GetNumBreakpoints() < NUM_BREAKPOINTS )
   {
      menu.addAction(ui->actionAdd_Breakpoint);
   }
   if ( pBreakpoints->GetNumBreakpoints() > 0 )
   {
      menu.addAction(ui->actionRemove_Breakpoint);
      menu.addAction(ui->actionEnable_All_Breakpoints);
      menu.addAction(ui->actionDisable_All_Breakpoints);
   }

   menu.exec(e->globalPos());
}

void BreakpointDockWidget::dragEnterEvent(QDragEnterEvent *event)
{
   if ( event->mimeData()->hasFormat("application/x-qabstractitemmodeldatalist") )
   {
      event->acceptProposedAction();
   }
   else if ( event->mimeData()->hasText() )
   {
      event->acceptProposedAction();
   }
}

void BreakpointDockWidget::dragMoveEvent(QDragMoveEvent *event)
{
   if ( event->mimeData()->hasFormat("application/x-qabstractitemmodeldatalist") )
   {
      event->acceptProposedAction();
   }
   else if ( event->mimeData()->hasText() )
   {
      event->acceptProposedAction();
   }
}

void BreakpointDockWidget::dropEvent(QDropEvent *event)
{
   QString text;
   int     addr;
   int     bpIdx;

   if ( event->mimeData()->hasFormat("application/x-qabstractitemmodeldatalist") )
   {
      QStandardItemModel model;
      model.dropMimeData(event->mimeData(), Qt::CopyAction, 0,0, QModelIndex());
      text = model.item(0,0)->data(Qt::DisplayRole).toString();

      event->acceptProposedAction();
   }
   else if ( event->mimeData()->hasText() )
   {
      text = event->mimeData()->text();

      event->acceptProposedAction();
   }

   if ( event->isAccepted() )
   {
      addr = CCC65Interface::getSymbolAddress(text);
      if ( addr != 0xFFFFFFFF )
      {
         CBreakpointInfo* pBreakpoints = nesGetBreakpointDatabase();

         bpIdx = pBreakpoints->AddBreakpoint ( eBreakOnCPUMemoryAccess,
                                               eBreakpointItemAddress,
                                               0,
                                               addr,
                                               addr,
                                               addr,
                                               eBreakpointConditionTest,
                                               eBreakIfAnything,
                                               eBreakpointDataPure,
                                               0,
                                               true );

         if ( bpIdx < 0 )
         {
            QString str;
            str.sprintf("Cannot add breakpoint, maximum of %d already used.", NUM_BREAKPOINTS);
            QMessageBox::information(0, "Error", str);
         }

         emit breakpointsChanged();
      }
   }
}

void BreakpointDockWidget::updateData()
{
   CBreakpointInfo* pBreakpoints = nesGetBreakpointDatabase();
   int idx;

   for ( idx = 0; idx < pBreakpoints->GetNumBreakpoints(); idx++ )
   {
      BreakpointInfo* pBreakpoint = pBreakpoints->GetBreakpoint(idx);

      if ( pBreakpoint->hit )
      {
         ui->tableView->setCurrentIndex(model->index(idx,0));
      }
   }
   ui->tableView->resizeColumnToContents(0);
}

void BreakpointDockWidget::on_tableView_pressed(QModelIndex index)
{
   CBreakpointInfo* pBreakpoints = nesGetBreakpointDatabase();

   // Check for left-click to "enable/disable"...
   if ( QApplication::mouseButtons()&Qt::LeftButton )
   {
      if ( (index.row() >= 0) && (index.column() == 0) )
      {
         pBreakpoints->ToggleEnabled(index.row());
         emit breakpointsChanged();
      }
   }
}

void BreakpointDockWidget::on_tableView_doubleClicked(QModelIndex index)
{
   CBreakpointInfo* pBreakpoints = nesGetBreakpointDatabase();
   int result;

   // Check for double-click to "edit"...
   if ( (index.row() >= 0) && (index.column() > 0) )
   {
      BreakpointDialog bd(index.row(),this);
      result = bd.exec();
      if ( result )
      {
         pBreakpoints->ModifyBreakpoint(index.row(), bd.getBreakpoint());
         emit breakpointsChanged();
      }
   }
   // Check for double-click to "enable/disable"...
   if ( (index.row() >= 0) && (index.column() == 0) )
   {
      pBreakpoints->ToggleEnabled(index.row());
      emit breakpointsChanged();
   }
}

void BreakpointDockWidget::on_actionAdd_Breakpoint_triggered()
{
   CBreakpointInfo* pBreakpoints = nesGetBreakpointDatabase();
   int result;

   BreakpointDialog bd(-1,this);
   result = bd.exec();
   if ( result )
   {
      pBreakpoints->AddBreakpoint(bd.getBreakpoint());
      emit breakpointsChanged();
   }
}

void BreakpointDockWidget::on_actionRemove_Breakpoint_triggered()
{
   CBreakpointInfo* pBreakpoints = nesGetBreakpointDatabase();

   if ( ui->tableView->currentIndex().row() >= 0 )
   {
      pBreakpoints->RemoveBreakpoint(ui->tableView->currentIndex().row());
      emit breakpointsChanged();
   }
}

void BreakpointDockWidget::on_actionEdit_Breakpoint_triggered()
{
   CBreakpointInfo* pBreakpoints = nesGetBreakpointDatabase();
   int result;

   if ( ui->tableView->currentIndex().row() >= 0 )
   {
      BreakpointDialog bd(ui->tableView->currentIndex().row(),this);
      result = bd.exec();
      if ( result )
      {
         pBreakpoints->ModifyBreakpoint(ui->tableView->currentIndex().row(), bd.getBreakpoint());
         emit breakpointsChanged();
      }
   }
}

void BreakpointDockWidget::on_actionEnable_Breakpoint_triggered()
{
   CBreakpointInfo* pBreakpoints = nesGetBreakpointDatabase();

   if ( ui->tableView->currentIndex().row() >= 0 )
   {
      pBreakpoints->SetEnabled(ui->tableView->currentIndex().row(), true);
      emit breakpointsChanged();
   }
}

void BreakpointDockWidget::on_actionDisable_Breakpoint_triggered()
{
   CBreakpointInfo* pBreakpoints = nesGetBreakpointDatabase();

   if ( ui->tableView->currentIndex().row() >= 0 )
   {
      pBreakpoints->SetEnabled(ui->tableView->currentIndex().row(), false);
      emit breakpointsChanged();
   }
}

void BreakpointDockWidget::on_actionEnable_All_Breakpoints_triggered()
{
   CBreakpointInfo* pBreakpoints = nesGetBreakpointDatabase();
   int bp;

   for ( bp = 0; bp < pBreakpoints->GetNumBreakpoints(); bp++ )
   {
      pBreakpoints->SetEnabled(bp,true);
   }
   emit breakpointsChanged();
}

void BreakpointDockWidget::on_actionDisable_All_Breakpoints_triggered()
{
   CBreakpointInfo* pBreakpoints = nesGetBreakpointDatabase();
   int bp;

   for ( bp = 0; bp < pBreakpoints->GetNumBreakpoints(); bp++ )
   {
      pBreakpoints->SetEnabled(bp,false);
   }
   emit breakpointsChanged();
}

bool BreakpointDockWidget::serialize(QDomDocument& doc, QDomNode& node)
{
   QDomElement element = addElement( doc, node, "breakpoints" );
   CBreakpointInfo* pBreakpoints = nesGetBreakpointDatabase();
   int bp;

   for ( bp = 0; bp < pBreakpoints->GetNumBreakpoints(); bp++ )
   {
      QDomElement breakpointElement = addElement( doc, element, "breakpoint" );
      BreakpointInfo* pBreakpoint = pBreakpoints->GetBreakpoint(bp);
      breakpointElement.setAttribute("type",pBreakpoint->type);
      breakpointElement.setAttribute("enabled",pBreakpoint->enabled);
      breakpointElement.setAttribute("target",pBreakpoint->target);
      breakpointElement.setAttribute("itemtype",pBreakpoint->itemType);
      breakpointElement.setAttribute("event",pBreakpoint->event);
      breakpointElement.setAttribute("item1",pBreakpoint->item1);
      breakpointElement.setAttribute("item1absolute",pBreakpoint->item1Absolute);
      breakpointElement.setAttribute("item2",pBreakpoint->item2);
      breakpointElement.setAttribute("conditiontype",pBreakpoint->conditionType);
      breakpointElement.setAttribute("condition",pBreakpoint->condition);
      breakpointElement.setAttribute("datatype",pBreakpoint->dataType);
      breakpointElement.setAttribute("data",pBreakpoint->data);
   }

   return true;
}

bool BreakpointDockWidget::deserialize(QDomDocument& doc, QDomNode& node, QString& errors)
{
   CBreakpointInfo* pBreakpoints = nesGetBreakpointDatabase();
   QDomNode childNode = node.firstChild();
   QDomNode breakpointNode;

   if (!childNode.isNull())
   {
      do
      {
         if (childNode.nodeName() == "breakpoints")
         {
            breakpointNode = childNode.firstChild();
            do
            {
               QDomElement element = breakpointNode.toElement();
               BreakpointInfo breakpoint;
               breakpoint.type = (eBreakpointType)element.attribute("type").toInt();
               breakpoint.enabled = element.attribute("enabled").toInt();
               breakpoint.target = (eBreakpointTarget)element.attribute("target").toInt();
               breakpoint.itemType = (eBreakpointItemType)element.attribute("itemtype").toInt();
               breakpoint.event = element.attribute("event").toInt();
               breakpoint.item1 = element.attribute("item1").toInt();
               breakpoint.item1Absolute = element.attribute("item1absolute").toInt();
               breakpoint.item2 = element.attribute("item2").toInt();
               breakpoint.conditionType = (eBreakpointConditionType)element.attribute("conditiontype").toInt();
               breakpoint.condition = element.attribute("condition").toInt();
               breakpoint.dataType = (eBreakpointDataType)element.attribute("datatype").toInt();
               breakpoint.data = element.attribute("data").toInt();
               pBreakpoints->AddBreakpoint(&breakpoint);
            } while (!(breakpointNode = breakpointNode.nextSibling()).isNull());

            model->update();
         }
      } while (!(childNode = childNode.nextSibling()).isNull());
   }

   return true;
}
