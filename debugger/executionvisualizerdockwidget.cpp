#include "executionvisualizerdockwidget.h"
#include "ui_executionvisualizerdockwidget.h"

#include "cdockwidgetregistry.h"

#include "cmarker.h"

#include "dbg_cnes6502.h"

#include "emulator_core.h"

#include "main.h"

ExecutionVisualizerDockWidget::ExecutionVisualizerDockWidget(QWidget *parent) :
    CDebuggerBase(parent),
    ui(new Ui::ExecutionVisualizerDockWidget)
{
   int i;

   ui->setupUi(this);

   model = new CExecutionMarkerDisplayModel();

   ui->tableView->setModel(model);
   ui->tableView->resizeColumnsToContents();

   imgData = new char[512*512*4];

   // Clear image...
   for ( i = 0; i < 512*512*4; i+=4 )
   {
      imgData[i] = 0;
      imgData[i+1] = 0;
      imgData[i+2] = 0;
      imgData[i+3] = 0xFF;
   }
   C6502DBG::ExecutionVisualizerInspectorTV ( (int8_t*)imgData );

   renderer = new CExecutionVisualizerRenderer(ui->frame,imgData);
   ui->frame->layout()->addWidget(renderer);
   ui->frame->layout()->update();
   QList<int> sizes;
   sizes.append(400);
   sizes.append(200);
   ui->splitter->setSizes(sizes);

   pThread = new DebuggerUpdateThread(&C6502DBG::RENDEREXECUTIONVISUALIZER);

   QObject::connect(emulator,SIGNAL(cartridgeLoaded()),pThread,SLOT(updateDebuggers()));
   QObject::connect(emulator,SIGNAL(emulatorReset()),pThread,SLOT(updateDebuggers()));
   QObject::connect(emulator,SIGNAL(emulatorPaused(bool)),pThread,SLOT(updateDebuggers()));
   QObject::connect(breakpointWatcher,SIGNAL(breakpointHit()),pThread,SLOT(updateDebuggers()));
   QObject::connect(pThread,SIGNAL(updateComplete()),this,SLOT(renderData()));
}

ExecutionVisualizerDockWidget::~ExecutionVisualizerDockWidget()
{
   delete ui;
   delete imgData;
   delete model;
   delete pThread;
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

void ExecutionVisualizerDockWidget::showEvent(QShowEvent* event)
{
   QDockWidget* breakpointInspector = dynamic_cast<QDockWidget*>(CDockWidgetRegistry::getWidget("Breakpoints"));
   QDockWidget* codeBrowser = dynamic_cast<QDockWidget*>(CDockWidgetRegistry::getWidget("Assembly Browser"));

   QObject::connect(codeBrowser,SIGNAL(breakpointsChanged()),pThread,SLOT(updateDebuggers()));
   QObject::connect(breakpointInspector,SIGNAL(breakpointsChanged()),pThread,SLOT(updateDebuggers()));

   QObject::connect(emulator,SIGNAL(updateDebuggers()),pThread,SLOT(updateDebuggers()));

   pThread->updateDebuggers();
}

void ExecutionVisualizerDockWidget::hideEvent(QHideEvent* event)
{
   QObject::disconnect(emulator,SIGNAL(updateDebuggers()),pThread,SLOT(updateDebuggers()));
}

void ExecutionVisualizerDockWidget::keyPressEvent(QKeyEvent *event)
{
   if ( ui->tableView->currentIndex().isValid() )
   {
      CMarker* pMarkers = nesGetExecutionMarkerDatabase();

      pMarkers->RemoveMarker(ui->tableView->currentIndex().row());
      pThread->updateDebuggers();
   }
}

void ExecutionVisualizerDockWidget::mousePressEvent(QMouseEvent *event)
{
   if ( event->button() == Qt::LeftButton )
   {
      pressPos = event->pos();
   }
}

void ExecutionVisualizerDockWidget::mouseMoveEvent(QMouseEvent *event)
{
   int zf = ui->zoomSlider->value();
   zf = zf-(zf%100);
   zf /= 100;

   if ( event->buttons() == Qt::LeftButton )
   {
      ui->horizontalScrollBar->setValue(ui->horizontalScrollBar->value()-((event->pos().x()/zf)-(pressPos.x()/zf)));
      ui->verticalScrollBar->setValue(ui->verticalScrollBar->value()-((event->pos().y()/zf)-(pressPos.y()/zf)));
   }
   else if ( event->buttons() == Qt::RightButton )
   {
      if ( event->pos().y() < pressPos.y() )
      {
         ui->zoomSlider->setValue(ui->zoomSlider->value()+100);
      }
      else
      {
         ui->zoomSlider->setValue(ui->zoomSlider->value()-100);
      }
   }
   pressPos = event->pos();
}

void ExecutionVisualizerDockWidget::wheelEvent(QWheelEvent *event)
{
   if ( event->delta() > 0 )
   {
      ui->zoomSlider->setValue(ui->zoomSlider->value()+100);
   }
   else if ( event->delta() < 0 )
   {
      ui->zoomSlider->setValue(ui->zoomSlider->value()-100);
   }
}

void ExecutionVisualizerDockWidget::renderData()
{
   renderer->updateGL ();
   model->update();
}

void ExecutionVisualizerDockWidget::resizeEvent(QResizeEvent* event)
{
   QDockWidget::resizeEvent(event);
   updateScrollbars();
}

void ExecutionVisualizerDockWidget::on_zoomSlider_valueChanged(int value)
{
   value = value-(value%100);
   ui->zoomSlider->setValue(value);
   renderer->changeZoom(value);
   ui->zoomValueLabel->setText(QString::number(value).append("%"));
   updateScrollbars();
}

void ExecutionVisualizerDockWidget::updateScrollbars()
{
   int value = ui->zoomSlider->value();
   value = value-(value%100);
   int viewWidth = (float)341 * ((float)value / 100.0f);
   int viewHeight = (float)262 * ((float)value / 100.0f);
   ui->horizontalScrollBar->setMaximum(viewWidth - renderer->width() < 0 ? 0 : ((viewWidth - renderer->width()) / ((float)value / 100.0f)) + 1);
   ui->verticalScrollBar->setMaximum(viewHeight - renderer->height() < 0 ? 0 : ((viewHeight - renderer->height()) / ((float)value / 100.0f)) + 1);
   renderer->scrollX = ui->horizontalScrollBar->value();
   renderer->scrollY = ui->verticalScrollBar->value();
}

void ExecutionVisualizerDockWidget::on_horizontalScrollBar_valueChanged(int value)
{
   renderer->scrollX = ui->horizontalScrollBar->value();
   renderer->update();
}

void ExecutionVisualizerDockWidget::on_verticalScrollBar_valueChanged(int value)
{
   renderer->scrollY = ui->verticalScrollBar->value();
   renderer->update();
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

bool ExecutionVisualizerDockWidget::deserialize(QDomDocument& doc, QDomNode& node, QString& errors)
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

void ExecutionVisualizerDockWidget::on_tableView_pressed(QModelIndex index)
{
   // If the user clicked on the start or end address cells, take them there.
   if ( (index.column() == ExecutionVisualizerCol_StartAddr) ||
        (index.column() == ExecutionVisualizerCol_EndAddr) )
   {
      emit snapTo("Address,"+index.data(Qt::DisplayRole).toString());
   }
}

void ExecutionVisualizerDockWidget::on_tableView_entered(QModelIndex index)
{
   on_tableView_pressed(index);
}

void ExecutionVisualizerDockWidget::on_tableView_clicked(QModelIndex index)
{
   on_tableView_pressed(index);
}

void ExecutionVisualizerDockWidget::on_tableView_activated(QModelIndex index)
{
   on_tableView_pressed(index);
}
