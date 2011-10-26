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

   QObject::connect ( emulator, SIGNAL(cartridgeLoaded()), this, SLOT(renderData()) );
   QObject::connect ( emulator, SIGNAL(emulatorPaused(bool)), this, SLOT(renderData()) );
   QObject::connect ( emulator, SIGNAL(emulatorReset()), this, SLOT(renderData()) );
   QObject::connect ( breakpointWatcher, SIGNAL(breakpointHit()), this, SLOT(renderData()) );

   renderer = new CExecutionVisualizerRenderer(ui->frame,imgData);
   ui->frame->layout()->addWidget(renderer);
   ui->frame->layout()->update();
   QList<int> sizes;
   sizes.append(400);
   sizes.append(200);
   ui->splitter->setSizes(sizes);
}

ExecutionVisualizerDockWidget::~ExecutionVisualizerDockWidget()
{
   delete ui;
   delete imgData;
   delete model;
}

void ExecutionVisualizerDockWidget::changeEvent(QEvent* e)
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

void ExecutionVisualizerDockWidget::showEvent(QShowEvent* event)
{
   QDockWidget* breakpointInspector = dynamic_cast<QDockWidget*>(CDockWidgetRegistry::getWidget("Breakpoints"));
   QDockWidget* codeBrowser = dynamic_cast<QDockWidget*>(CDockWidgetRegistry::getWidget("Assembly Browser"));

   QObject::connect(codeBrowser,SIGNAL(breakpointsChanged()),model, SLOT(update()) );
   QObject::connect ( emulator, SIGNAL(updateDebuggers()), model, SLOT(update()));
   QObject::connect ( breakpointInspector, SIGNAL(breakpointsChanged()), model, SLOT(update()) );

   QObject::connect ( emulator, SIGNAL(updateDebuggers()), this, SLOT(renderData()) );
   renderData();
}

void ExecutionVisualizerDockWidget::hideEvent(QHideEvent* event)
{
   QObject::disconnect ( emulator, SIGNAL(updateDebuggers()), this, SLOT(renderData()) );
}

void ExecutionVisualizerDockWidget::renderData()
{
   C6502DBG::RENDEREXECUTIONVISUALIZER();
   renderer->updateGL ();
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
      QDomElement breakpointElement = addElement( doc, element, "marker" );
      MarkerSetInfo* pMarker = pMarkers->GetMarker(marker);
      breakpointElement.setAttribute("index",marker);
      breakpointElement.setAttribute("state",pMarker->state);
      if ( pMarker->state == eMarkerSet_Started )
      {
         breakpointElement.setAttribute("startaddr",pMarker->startAddr);
         breakpointElement.setAttribute("startabsaddr",pMarker->startAbsAddr);
         breakpointElement.setAttribute("endaddr",0);
         breakpointElement.setAttribute("endabsaddr",0);
      }
      else if ( pMarker->state == eMarkerSet_Complete )
      {
         breakpointElement.setAttribute("startaddr",pMarker->startAddr);
         breakpointElement.setAttribute("startabsaddr",pMarker->startAbsAddr);
         breakpointElement.setAttribute("endaddr",pMarker->endAddr);
         breakpointElement.setAttribute("endabsaddr",pMarker->endAbsAddr);
      }
      else
      {
         breakpointElement.setAttribute("startaddr",0);
         breakpointElement.setAttribute("startabsaddr",0);
         breakpointElement.setAttribute("endaddr",0);
         breakpointElement.setAttribute("endabsaddr",0);
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
