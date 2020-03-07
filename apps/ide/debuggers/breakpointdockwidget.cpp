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
#include "cobjectregistry.h"

#include "nes_emulator_core.h"
#include "c64_emulator_core.h"

BreakpointDockWidget::BreakpointDockWidget(CBreakpointInfo* pBreakpoints,QWidget *parent) :
    CDebuggerBase(parent),
    ui(new Ui::BreakpointDockWidget)
{
   ui->setupUi(this);

   m_pBreakpoints = pBreakpoints;

   model = new CBreakpointDisplayModel(pBreakpoints);
   ui->tableView->setModel ( model );
   ui->tableView->installEventFilter(this);
   ui->tableView->viewport()->installEventFilter(this);

   QObject::connect(this,SIGNAL(breakpointsChanged()),model,SLOT(update()));
}

BreakpointDockWidget::~BreakpointDockWidget()
{
   delete ui;
   delete model;
}

void BreakpointDockWidget::updateTargetMachine(QString /*target*/)
{
   QObject* breakpointWatcher = CObjectRegistry::instance()->getObject("Breakpoint Watcher");
   QObject* emulator = CObjectRegistry::instance()->getObject("Emulator");

   QObject::connect(breakpointWatcher,SIGNAL(breakpointHit()),this,SLOT(updateData()) );
   QObject::connect(breakpointWatcher,SIGNAL(breakpointHit()),model,SLOT(update()));
   if ( emulator )
   {
      QObject::connect(emulator,SIGNAL(machineReady()),model,SLOT(update()));
      QObject::connect(emulator,SIGNAL(emulatorReset()),model,SLOT(update()));
      QObject::connect(emulator,SIGNAL(emulatorPaused(bool)),model,SLOT(update()));
      QObject::connect(emulator,SIGNAL(emulatorStarted()),model,SLOT(update()));
      QObject::connect(this,SIGNAL(breakpointsChanged()),emulator,SLOT(breakpointsChanged()));
   }
}

bool BreakpointDockWidget::eventFilter(QObject *obj, QEvent *event)
{
   if ( obj == ui->tableView || obj == ui->tableView->viewport() )
   {
      if ( event->type() == QEvent::DragEnter )
      {
         QDragEnterEvent* newEvent = static_cast<QDragEnterEvent*>(event);
         dragEnterEvent(newEvent);
         return true;
      }
      if ( event->type() == QEvent::DragMove )
      {
         QDragMoveEvent* newEvent = static_cast<QDragMoveEvent*>(event);
         dragMoveEvent(newEvent);
         return true;
      }
      if ( event->type() == QEvent::Drop )
      {
         QDropEvent* newEvent = static_cast<QDropEvent*>(event);
         dropEvent(newEvent);
         return true;
      }
   }
   return false;
}

void BreakpointDockWidget::changeEvent(QEvent* e)
{
   CDebuggerBase::changeEvent(e);

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
   QDockWidget* codeBrowser = dynamic_cast<QDockWidget*>(CDockWidgetRegistry::instance()->getWidget("Assembly Browser"));
   QDockWidget* symbolInspector = dynamic_cast<QDockWidget*>(CDockWidgetRegistry::instance()->getWidget("Symbol Inspector"));
   QDockWidget* memoryInspector;
   QObject*     emulator = CObjectRegistry::instance()->getObject("Emulator");

   QObject::connect(codeBrowser,SIGNAL(breakpointsChanged()),model, SLOT(update()) );
   QObject::connect(symbolInspector,SIGNAL(breakpointsChanged()),model, SLOT(update()) );

   memoryInspector = dynamic_cast<QDockWidget*>(CDockWidgetRegistry::instance()->getWidget("CPU RAM Inspector"));
   QObject::connect(memoryInspector,SIGNAL(breakpointsChanged()),model, SLOT(update()) );
   if ( !nesicideProject->getProjectTarget().compare("nes",Qt::CaseInsensitive) )
   {
      memoryInspector = dynamic_cast<QDockWidget*>(CDockWidgetRegistry::instance()->getWidget("Cartridge EXRAM Memory Inspector"));
      QObject::connect(memoryInspector,SIGNAL(breakpointsChanged()),model, SLOT(update()) );
      memoryInspector = dynamic_cast<QDockWidget*>(CDockWidgetRegistry::instance()->getWidget("Cartridge SRAM Memory Inspector"));
      QObject::connect(memoryInspector,SIGNAL(breakpointsChanged()),model, SLOT(update()) );
      memoryInspector = dynamic_cast<QDockWidget*>(CDockWidgetRegistry::instance()->getWidget("PRG-ROM Inspector"));
      QObject::connect(memoryInspector,SIGNAL(breakpointsChanged()),model, SLOT(update()) );
   }

   if ( emulator )
   {
      QObject::connect(codeBrowser,SIGNAL(breakpointsChanged()),emulator, SLOT(breakpointsChanged()) );
      QObject::connect(symbolInspector,SIGNAL(breakpointsChanged()),model, SLOT(update()) );

      memoryInspector = dynamic_cast<QDockWidget*>(CDockWidgetRegistry::instance()->getWidget("CPU RAM Inspector"));
      QObject::connect(memoryInspector,SIGNAL(breakpointsChanged()),emulator, SLOT(breakpointsChanged()) );
      if ( !nesicideProject->getProjectTarget().compare("nes",Qt::CaseInsensitive) )
      {
         memoryInspector = dynamic_cast<QDockWidget*>(CDockWidgetRegistry::instance()->getWidget("Cartridge EXRAM Memory Inspector"));
         QObject::connect(memoryInspector,SIGNAL(breakpointsChanged()),emulator, SLOT(breakpointsChanged()) );
         memoryInspector = dynamic_cast<QDockWidget*>(CDockWidgetRegistry::instance()->getWidget("Cartridge SRAM Memory Inspector"));
         QObject::connect(memoryInspector,SIGNAL(breakpointsChanged()),emulator, SLOT(breakpointsChanged()) );
         memoryInspector = dynamic_cast<QDockWidget*>(CDockWidgetRegistry::instance()->getWidget("PRG-ROM Inspector"));
         QObject::connect(memoryInspector,SIGNAL(breakpointsChanged()),emulator, SLOT(breakpointsChanged()) );
      }
   }

   model->update();
   updateData();
}

void BreakpointDockWidget::contextMenuEvent(QContextMenuEvent *e)
{
   QMenu menu;

   if ( ui->tableView->currentIndex().row() < m_pBreakpoints->GetNumBreakpoints() )
   {
      menu.addAction(ui->actionEdit_Breakpoint);

      BreakpointInfo* pBreakpoint = m_pBreakpoints->GetBreakpoint(ui->tableView->currentIndex().row());
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
   if ( m_pBreakpoints->GetNumBreakpoints() < NUM_BREAKPOINTS )
   {
      menu.addAction(ui->actionAdd_Breakpoint);
   }
   if ( m_pBreakpoints->GetNumBreakpoints() > 0 )
   {
      menu.addAction(ui->actionRemove_Breakpoint);
      menu.addAction(ui->actionRemove_All_Breakpoints);
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
      addr = CCC65Interface::instance()->getSymbolAddress(text);
      if ( addr != 0xFFFFFFFF )
      {
         bpIdx = m_pBreakpoints->AddBreakpoint ( eBreakOnCPUMemoryAccess,
                                               eBreakpointItemAddress,
                                               0,
                                               addr,
                                               addr,
                                               addr,
                                               0xFFFF,
                                               false,
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
         emit markProjectDirty(true);
      }
   }
}

void BreakpointDockWidget::keyPressEvent(QKeyEvent *event)
{
   if ( event->key() == Qt::Key_Delete )
   {
      if ( ui->tableView->currentIndex().row() < m_pBreakpoints->GetNumBreakpoints() )
      {
         on_actionRemove_Breakpoint_triggered();
      }
   }
}

void BreakpointDockWidget::updateData()
{
   int idx;

   for ( idx = 0; idx < m_pBreakpoints->GetNumBreakpoints(); idx++ )
   {
      BreakpointInfo* pBreakpoint = m_pBreakpoints->GetBreakpoint(idx);

      if ( pBreakpoint->hit )
      {
         ui->tableView->setCurrentIndex(model->index(idx,0));
      }
   }
   ui->tableView->resizeColumnToContents(0);
}

void BreakpointDockWidget::on_tableView_pressed(QModelIndex index)
{
   char buffer[32];

   // Check for left-click to "enable/disable"...
   if ( QApplication::mouseButtons()&Qt::LeftButton )
   {
      // Emit snapTo if possible...
      if ( (index.row() < m_pBreakpoints->GetNumBreakpoints()) &&
           (m_pBreakpoints->GetBreakpoint(index.row())->type == eBreakOnCPUExecution) )
      {
         nesGetPrintablePhysicalAddress(buffer,m_pBreakpoints->GetBreakpoint(index.row())->item1,m_pBreakpoints->GetBreakpoint(index.row())->item1Physical);
         emit snapTo(QString("Address,")+QString(buffer));
      }

      if ( (index.row() < m_pBreakpoints->GetNumBreakpoints()) && (index.column() == 0) )
      {
         m_pBreakpoints->ToggleEnabled(index.row());
         emit breakpointsChanged();
         emit markProjectDirty(true);
      }
   }
}

void BreakpointDockWidget::on_tableView_doubleClicked(QModelIndex index)
{
   int result;

   // Check for double-click to "edit"...
   if ( (index.row() < m_pBreakpoints->GetNumBreakpoints()) && (index.column() > 0) )
   {
      BreakpointDialog bd(m_pBreakpoints,index.row(),this);
      result = bd.exec();
      if ( result )
      {
         m_pBreakpoints->ModifyBreakpoint(index.row(), bd.getBreakpoint());
         emit breakpointsChanged();
         emit markProjectDirty(true);
      }
   }
   // Check for double-click to "add"...
   if ( index.row() == m_pBreakpoints->GetNumBreakpoints() )
   {
      BreakpointDialog bd(m_pBreakpoints,-1,this);
      result = bd.exec();
      if ( result )
      {
         m_pBreakpoints->AddBreakpoint(bd.getBreakpoint());
         emit breakpointsChanged();
         emit markProjectDirty(true);
      }
   }
   // Check for double-click to "enable/disable"...
   if ( (index.row() < m_pBreakpoints->GetNumBreakpoints()) && (index.column() == 0) )
   {
      m_pBreakpoints->ToggleEnabled(index.row());
      emit breakpointsChanged();
      emit markProjectDirty(true);
   }
}

void BreakpointDockWidget::on_actionAdd_Breakpoint_triggered()
{
   int result;

   BreakpointDialog bd(m_pBreakpoints,-1,this);
   result = bd.exec();
   if ( result )
   {
      m_pBreakpoints->AddBreakpoint(bd.getBreakpoint());
      emit breakpointsChanged();
      emit markProjectDirty(true);
   }
}

void BreakpointDockWidget::on_actionRemove_Breakpoint_triggered()
{
   if ( ui->tableView->currentIndex().row() < m_pBreakpoints->GetNumBreakpoints() )
   {
      m_pBreakpoints->RemoveBreakpoint(ui->tableView->currentIndex().row());
      emit breakpointsChanged();
      emit markProjectDirty(true);
   }
}

void BreakpointDockWidget::on_actionRemove_All_Breakpoints_triggered()
{
   int idx = m_pBreakpoints->GetNumBreakpoints()-1;
   for ( ; idx >= 0; idx-- )
   {
      m_pBreakpoints->RemoveBreakpoint(idx);
   }
   emit breakpointsChanged();
   emit markProjectDirty(true);
}

void BreakpointDockWidget::on_actionEdit_Breakpoint_triggered()
{
   int result;

   if ( ui->tableView->currentIndex().row() < m_pBreakpoints->GetNumBreakpoints() )
   {
      BreakpointDialog bd(m_pBreakpoints,ui->tableView->currentIndex().row(),this);
      result = bd.exec();
      if ( result )
      {
         m_pBreakpoints->ModifyBreakpoint(ui->tableView->currentIndex().row(), bd.getBreakpoint());
         emit breakpointsChanged();
         emit markProjectDirty(true);
      }
   }
}

void BreakpointDockWidget::on_actionEnable_Breakpoint_triggered()
{
   if ( ui->tableView->currentIndex().row() < m_pBreakpoints->GetNumBreakpoints() )
   {
      m_pBreakpoints->SetEnabled(ui->tableView->currentIndex().row(), true);
      emit breakpointsChanged();
      emit markProjectDirty(true);
   }
}

void BreakpointDockWidget::on_actionDisable_Breakpoint_triggered()
{
   if ( ui->tableView->currentIndex().row() < m_pBreakpoints->GetNumBreakpoints() )
   {
      m_pBreakpoints->SetEnabled(ui->tableView->currentIndex().row(), false);
      emit breakpointsChanged();
      emit markProjectDirty(true);
   }
}

void BreakpointDockWidget::on_actionEnable_All_Breakpoints_triggered()
{
   int bp;

   for ( bp = 0; bp < m_pBreakpoints->GetNumBreakpoints(); bp++ )
   {
      m_pBreakpoints->SetEnabled(bp,true);
   }
   emit breakpointsChanged();
   emit markProjectDirty(true);
}

void BreakpointDockWidget::on_actionDisable_All_Breakpoints_triggered()
{
   int bp;

   for ( bp = 0; bp < m_pBreakpoints->GetNumBreakpoints(); bp++ )
   {
      m_pBreakpoints->SetEnabled(bp,false);
   }
   emit breakpointsChanged();
   emit markProjectDirty(true);
}

bool BreakpointDockWidget::serialize(QDomDocument& doc, QDomNode& node)
{
   QDomElement element = addElement( doc, node, "breakpoints" );
   int bp;

   for ( bp = 0; bp < m_pBreakpoints->GetNumBreakpoints(); bp++ )
   {
      QDomElement breakpointElement = addElement( doc, element, "breakpoint" );
      BreakpointInfo* pBreakpoint = m_pBreakpoints->GetBreakpoint(bp);
      breakpointElement.setAttribute("type",pBreakpoint->type);
      breakpointElement.setAttribute("enabled",pBreakpoint->enabled);
      breakpointElement.setAttribute("target",pBreakpoint->target);
      breakpointElement.setAttribute("itemtype",pBreakpoint->itemType);
      breakpointElement.setAttribute("event",pBreakpoint->event);
      breakpointElement.setAttribute("item1",pBreakpoint->item1);
      breakpointElement.setAttribute("item1absolute",pBreakpoint->item1Physical);
      breakpointElement.setAttribute("item2",pBreakpoint->item2);
      breakpointElement.setAttribute("itemMask",pBreakpoint->itemMask);
      breakpointElement.setAttribute("itemMaskExclusive",pBreakpoint->itemMaskExclusive);
      breakpointElement.setAttribute("conditiontype",pBreakpoint->conditionType);
      breakpointElement.setAttribute("condition",pBreakpoint->condition);
      breakpointElement.setAttribute("datatype",pBreakpoint->dataType);
      breakpointElement.setAttribute("data",pBreakpoint->data);
   }

   return true;
}

bool BreakpointDockWidget::deserialize(QDomDocument& /*doc*/, QDomNode& node, QString& /*errors*/)
{
   QDomNode childNode = node.firstChild();
   QDomNode breakpointNode;
   int bp;

   // Clear out existing breakpoints...
   if ( m_pBreakpoints->GetNumBreakpoints() )
   {
      for ( bp = m_pBreakpoints->GetNumBreakpoints()-1; bp >= 0; bp-- )
      {
         m_pBreakpoints->RemoveBreakpoint(bp);
      }
   }

   // Load new breakpoints...
   if (!childNode.isNull())
   {
      do
      {
         if (childNode.nodeName() == "breakpoints")
         {
            breakpointNode = childNode.firstChild();
            while ( !(breakpointNode.isNull()) )
            {
               QDomElement element = breakpointNode.toElement();
               BreakpointInfo breakpoint;
               breakpoint.type = (eBreakpointType)element.attribute("type").toInt();
               breakpoint.enabled = element.attribute("enabled").toInt();
               breakpoint.target = (eBreakpointTarget)element.attribute("target").toInt();
               breakpoint.itemType = (eBreakpointItemType)element.attribute("itemtype").toInt();
               breakpoint.event = element.attribute("event").toInt();
               breakpoint.item1 = element.attribute("item1").toInt();
               breakpoint.item1Physical = element.attribute("item1absolute").toInt();
               breakpoint.item2 = element.attribute("item2").toInt();
               breakpoint.itemMask = element.attribute("itemMask").toInt();
               breakpoint.itemMaskExclusive = element.attribute("itemMaskExclusive").toInt();
               breakpoint.conditionType = (eBreakpointConditionType)element.attribute("conditiontype").toInt();
               breakpoint.condition = element.attribute("condition").toInt();
               breakpoint.dataType = (eBreakpointDataType)element.attribute("datatype").toInt();
               breakpoint.data = element.attribute("data").toInt();
               m_pBreakpoints->AddBreakpoint(&breakpoint);
               breakpointNode = breakpointNode.nextSibling();
            }

            model->update();
         }
      } while (!(childNode = childNode.nextSibling()).isNull());
   }

   return true;
}
