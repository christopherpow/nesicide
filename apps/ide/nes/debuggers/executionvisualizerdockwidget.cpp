#include "executionvisualizerdockwidget.h"
#include "ui_executionvisualizerdockwidget.h"

#include "cdockwidgetregistry.h"

#include "cmarker.h"

#include "dbg_cnes6502.h"

#include "nes_emulator_core.h"

#include "cobjectregistry.h"
#include "main.h"

ExecutionVisualizerDockWidget::ExecutionVisualizerDockWidget(QWidget *parent) :
    CDebuggerBase(parent),
    ui(new Ui::ExecutionVisualizerDockWidget)
{
   ui->setupUi(this);

   model = new CExecutionMarkerDisplayModel();

   ui->tableView->setModel(model);
   ui->tableView->resizeColumnsToContents();
   
   QObject::connect(ui->tableView->selectionModel(),SIGNAL(currentChanged(QModelIndex,QModelIndex)),this,SLOT(tableView_currentChanged(QModelIndex,QModelIndex)));

   RENDERCPUEXECUTIONVISUALIZER();

   renderer = new PanZoomRenderer(341,312,512,10000,CPUEXECUTIONVISUALIZERTV(),false,ui->frame);
   ui->frame->layout()->addWidget(renderer);
   ui->frame->layout()->update();

   QList<int> sizes;
   sizes.append(400);
   sizes.append(200);
   ui->splitter->setSizes(sizes);

#if defined(Q_OS_MAC) || defined(Q_OS_MACX) || defined(Q_OS_MAC64)
   ui->tableView->setFont(QFont("Monaco", 11));
#endif
#ifdef Q_OS_LINUX
   ui->tableView->setFont(QFont("Monospace", 10));
#endif
#ifdef Q_OS_WIN
   ui->tableView->setFont(QFont("Consolas", 11));
#endif

   pThread = new DebuggerUpdateThread(&RENDERCPUEXECUTIONVISUALIZER);
   QObject::connect(pThread,SIGNAL(updateComplete()),this,SLOT(renderData()));
}

ExecutionVisualizerDockWidget::~ExecutionVisualizerDockWidget()
{
   delete pThread;
   delete ui;
   delete renderer;
   delete model;
}

void ExecutionVisualizerDockWidget::updateTargetMachine(QString /*target*/)
{
   QObject* breakpointWatcher = CObjectRegistry::getObject("Breakpoint Watcher");
   QObject* emulator = CObjectRegistry::getObject("Emulator");

   QObject::connect(emulator,SIGNAL(machineReady()),pThread,SLOT(updateDebuggers()));
   QObject::connect(emulator,SIGNAL(emulatorReset()),pThread,SLOT(updateDebuggers()));
   QObject::connect(emulator,SIGNAL(emulatorPaused(bool)),pThread,SLOT(updateDebuggers()));
   QObject::connect(breakpointWatcher,SIGNAL(breakpointHit()),pThread,SLOT(updateDebuggers()));
}

void ExecutionVisualizerDockWidget::changeEvent(QEvent* event)
{
   CDebuggerBase::changeEvent(event);

   switch (event->type())
   {
      case QEvent::LanguageChange:
         ui->retranslateUi(this);
         break;
      default:
         break;
   }
}

void ExecutionVisualizerDockWidget::showEvent(QShowEvent* /*event*/)
{
   QObject* emulator = CObjectRegistry::getObject("Emulator");
   QDockWidget* breakpointInspector = dynamic_cast<QDockWidget*>(CDockWidgetRegistry::getWidget("Breakpoints"));
   QDockWidget* codeBrowser = dynamic_cast<QDockWidget*>(CDockWidgetRegistry::getWidget("Assembly Browser"));

   QObject::connect(codeBrowser,SIGNAL(breakpointsChanged()),pThread,SLOT(updateDebuggers()));
   QObject::connect(breakpointInspector,SIGNAL(breakpointsChanged()),pThread,SLOT(updateDebuggers()));

   QObject::connect(emulator,SIGNAL(updateDebuggers()),pThread,SLOT(updateDebuggers()));

   pThread->updateDebuggers();
}

void ExecutionVisualizerDockWidget::hideEvent(QHideEvent* /*event*/)
{
   QObject* emulator = CObjectRegistry::getObject("Emulator");

   QObject::disconnect(emulator,SIGNAL(updateDebuggers()),pThread,SLOT(updateDebuggers()));
}

void ExecutionVisualizerDockWidget::keyPressEvent(QKeyEvent */*event*/)
{
   CMarker* pMarkers = nesGetExecutionMarkerDatabase();

   if ( ui->tableView->currentIndex().isValid() )
   {
      pMarkers->RemoveMarker(ui->tableView->currentIndex().row());
      pThread->updateDebuggers();

      emit breakpointsChanged();
   }
}

void ExecutionVisualizerDockWidget::contextMenuEvent(QContextMenuEvent *event)
{
   QMenu menu;
   if ( ui->tableView->currentIndex().isValid() )
   {
      menu.addAction(ui->actionRemove_Marker);
      menu.addAction(ui->actionReset_Marker_Data);
   }

   menu.exec(event->globalPos());
}

void ExecutionVisualizerDockWidget::renderData()
{
   renderer->reloadData(CPUEXECUTIONVISUALIZERTV());
   model->update();
}

bool ExecutionVisualizerDockWidget::serialize(QDomDocument& doc, QDomNode& node)
{
   QDomElement element = addElement( doc, node, "markers" );
   CMarker* pMarkers = nesGetExecutionMarkerDatabase();
   int marker;

   for ( marker = 0; marker < pMarkers->GetNumMarkers(); marker++ )
   {
      QDomElement markerElement = addElement( doc, element, "marker" );
      MarkerSetInfo* pMarker = pMarkers->GetMarker(marker);
      markerElement.setAttribute("index",marker);
      markerElement.setAttribute("state",pMarker->state);
      if ( pMarker->state == eMarkerSet_Started )
      {
         markerElement.setAttribute("startaddr",pMarker->startAddr);
         markerElement.setAttribute("startabsaddr",pMarker->startAbsAddr);
         markerElement.setAttribute("endaddr",0);
         markerElement.setAttribute("endabsaddr",0);
      }
      else if ( pMarker->state == eMarkerSet_Complete )
      {
         markerElement.setAttribute("startaddr",pMarker->startAddr);
         markerElement.setAttribute("startabsaddr",pMarker->startAbsAddr);
         markerElement.setAttribute("endaddr",pMarker->endAddr);
         markerElement.setAttribute("endabsaddr",pMarker->endAbsAddr);
      }
      else
      {
         markerElement.setAttribute("startaddr",0);
         markerElement.setAttribute("startabsaddr",0);
         markerElement.setAttribute("endaddr",0);
         markerElement.setAttribute("endabsaddr",0);
      }
   }

   return true;
}

bool ExecutionVisualizerDockWidget::deserialize(QDomDocument& /*doc*/, QDomNode& node, QString& /*errors*/)
{
   CMarker* pMarkers = nesGetExecutionMarkerDatabase();
   int marker;
   eMarkerSet_State state;
   uint32_t startAddr;
   uint32_t startAbsAddr;
   uint32_t endAddr;
   uint32_t endAbsAddr;
   QDomNode childNode = node.firstChild();
   QDomNode markerNode;

   // Start with a clean slate.
   pMarkers->RemoveAllMarkers();

   if (!childNode.isNull())
   {
      do
      {
         if (childNode.nodeName() == "markers")
         {
            markerNode = childNode.firstChild();
            while ( !(markerNode.isNull()) )
            {
               QDomElement element = markerNode.toElement();
               marker = element.attribute("index").toInt();
               if ( marker < pMarkers->GetNumMarkers() )
               {
                  state = (eMarkerSet_State)element.attribute("state").toInt();
                  startAddr = element.attribute("startaddr").toInt();
                  startAbsAddr = element.attribute("startabsaddr").toInt();
                  endAddr = element.attribute("endaddr").toInt();
                  endAbsAddr = element.attribute("endabsaddr").toInt();
                  switch ( state )
                  {
                  case eMarkerSet_Started:
                     pMarkers->AddSpecificMarker(marker,startAddr,startAbsAddr);
                     break;
                  case eMarkerSet_Complete:
                     pMarkers->AddSpecificMarker(marker,startAddr,startAbsAddr);
                     pMarkers->CompleteMarker(marker,endAddr,endAbsAddr);
                     break;
                  default:
                     break;
                  }
               }
               markerNode = markerNode.nextSibling();
            }

            model->update();
         }
      } while (!(childNode = childNode.nextSibling()).isNull());
   }

   return true;
}

void ExecutionVisualizerDockWidget::tableView_currentChanged(QModelIndex index,QModelIndex)
{
   // If the user clicked on the start or end address cells, take them there.
   if ( (index.column() == ExecutionVisualizerCol_StartAddr) ||
        (index.column() == ExecutionVisualizerCol_EndAddr) )
   {
      emit snapTo("Address,"+index.data(Qt::DisplayRole).toString());
   }
}

void ExecutionVisualizerDockWidget::on_actionReset_Marker_Data_triggered()
{
   CMarker* pMarkers = nesGetExecutionMarkerDatabase();
   int marker = ui->tableView->currentIndex().row();

   if ( ui->tableView->currentIndex().isValid() )
   {
      pMarkers->ZeroMarker(marker);
      pThread->updateDebuggers();
   }
}

void ExecutionVisualizerDockWidget::on_actionRemove_Marker_triggered()
{
   CMarker* pMarkers = nesGetExecutionMarkerDatabase();
   int marker = ui->tableView->currentIndex().row();

   if ( ui->tableView->currentIndex().isValid() )
   {
      pMarkers->RemoveMarker(marker);
      pThread->updateDebuggers();

      emit breakpointsChanged();
      emit markProjectDirty(true);
   }
}
