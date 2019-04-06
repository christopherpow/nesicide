#include "panzoomrenderer.h"
#include "ui_panzoomrenderer.h"

#include "nes_emulator_core.h"

PanZoomRenderer::PanZoomRenderer(int sizeX,int sizeY,int textureSizeXY,int maxZoom,int8_t* imageData,bool showPalette,QWidget *parent) :
      QWidget(parent),
      _sizeX(sizeX),
      _sizeY(sizeY),
      _paletteVisible(true),
      ui(new Ui::PanZoomRenderer)
{
   ui->setupUi(this);

   renderer = new CRendererBase(sizeX,sizeY,textureSizeXY,maxZoom,imageData);

   ui->zoomSlider->setMaximum(maxZoom);

   commonConstructor(showPalette);
}

PanZoomRenderer::PanZoomRenderer(int sizeX,int sizeY,int maxZoom,int8_t* imageData,bool showPalette,QWidget *parent) :
    QWidget(parent),
    _sizeX(sizeX),
    _sizeY(sizeY),
    _paletteVisible(true),
    ui(new Ui::PanZoomRenderer)
{
    ui->setupUi(this);

    renderer = new CRendererBase(sizeX,sizeY,maxZoom,imageData);

    ui->zoomSlider->setMaximum(maxZoom);

    commonConstructor(showPalette);
}

void PanZoomRenderer::commonConstructor(bool showPalette)
{
   renderer->setMouseTracking(true);

   renderer->setSizePolicy(QSizePolicy::Ignored,QSizePolicy::Ignored);
   renderer->resize(_sizeX,_sizeY);

   renderer->installEventFilter(this);

   ui->frame->layout()->addWidget(renderer);
   ui->frame->layout()->update();

   ui->col0PushButton->setVisible(showPalette);
   ui->col1PushButton->setVisible(showPalette);
   ui->col2PushButton->setVisible(showPalette);
   ui->col3PushButton->setVisible(showPalette);
   ui->paletteLabel->setVisible(showPalette);

   ui->col0PushButton->setCurrentColor(QColor(nesGetPaletteRedComponent(0x0D),nesGetPaletteGreenComponent(0x0D),nesGetPaletteBlueComponent(0x0D)));
   ui->col1PushButton->setCurrentColor(QColor(nesGetPaletteRedComponent(0x00),nesGetPaletteGreenComponent(0x00),nesGetPaletteBlueComponent(0x00)));
   ui->col2PushButton->setCurrentColor(QColor(nesGetPaletteRedComponent(0x10),nesGetPaletteGreenComponent(0x10),nesGetPaletteBlueComponent(0x10)));
   ui->col3PushButton->setCurrentColor(QColor(nesGetPaletteRedComponent(0x20),nesGetPaletteGreenComponent(0x20),nesGetPaletteBlueComponent(0x20)));

   QObject::connect(ui->col0PushButton, SIGNAL(colorChanged(QColor)), this, SIGNAL(repaintNeeded()));
   QObject::connect(ui->col1PushButton, SIGNAL(colorChanged(QColor)), this, SIGNAL(repaintNeeded()));
   QObject::connect(ui->col2PushButton, SIGNAL(colorChanged(QColor)), this, SIGNAL(repaintNeeded()));
   QObject::connect(ui->col3PushButton, SIGNAL(colorChanged(QColor)), this, SIGNAL(repaintNeeded()));
}

PanZoomRenderer::~PanZoomRenderer()
{
    delete ui;
    delete renderer;
}

QColor PanZoomRenderer::getColor(int idx)
{
   switch ( idx )
   {
   case 0:
      return ui->col0PushButton->currentColor();
      break;
   case 1:
      return ui->col1PushButton->currentColor();
      break;
   case 2:
      return ui->col2PushButton->currentColor();
      break;
   case 3:
      return ui->col3PushButton->currentColor();
      break;
   }
   return QColor(0,0,0);
}

bool PanZoomRenderer::eventFilter(QObject *object, QEvent *event)
{
   if ( object == renderer )
   {
      if ( event->type() == QEvent::MouseButtonPress )
      {
         QMouseEvent* mouseEvent = dynamic_cast<QMouseEvent*>(event);
         renderer_mousePressEvent(mouseEvent);
      }
      else if ( event->type() == QEvent::MouseMove )
      {
         QMouseEvent* mouseEvent = dynamic_cast<QMouseEvent*>(event);
         renderer_mouseMoveEvent(mouseEvent);
      }
      else if ( event->type() == QEvent::Wheel )
      {
         QWheelEvent* wheelEvent = dynamic_cast<QWheelEvent*>(event);
         renderer_wheelEvent(wheelEvent);
      }
   }
   return false;
}

void PanZoomRenderer::renderer_mousePressEvent(QMouseEvent *event)
{
   if ( event->button() == Qt::LeftButton )
   {
      pressPos = event->pos();
   }
}

void PanZoomRenderer::renderer_mouseMoveEvent(QMouseEvent *event)
{
   int zf;

   if ( event->buttons() == Qt::LeftButton )
   {
      zf = ui->zoomSlider->value()/100;
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

void PanZoomRenderer::renderer_wheelEvent(QWheelEvent *event)
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

void PanZoomRenderer::showEvent(QShowEvent */*event*/)
{
   updateScrollBars();
}

void PanZoomRenderer::resizeEvent(QResizeEvent */*event*/)
{
   updateScrollBars();
}

void PanZoomRenderer::on_zoomSlider_valueChanged(int value)
{
   value = value-(value%100);
   ui->zoomSlider->setValue(value);
   renderer->changeZoom(value);
   ui->zoomValueLabel->setText(QString::number(value).append("%"));
   updateScrollBars();
}

void PanZoomRenderer::updateScrollBars()
{
   int value = ui->zoomSlider->value();
   value = value-(value%100);
   int viewWidth = (float)_sizeX * ((float)value / 100.0f);
   int viewHeight = (float)_sizeY * ((float)value / 100.0f);
   ui->horizontalScrollBar->setMaximum(viewWidth - renderer->width() < 0 ? 0 : ((viewWidth - renderer->width()) / ((float)value / 100.0f)) + 1);
   ui->verticalScrollBar->setMaximum(viewHeight - renderer->height() < 0 ? 0 : ((viewHeight - renderer->height()) / ((float)value / 100.0f)) + 1);
}

void PanZoomRenderer::on_verticalScrollBar_valueChanged(int /*value*/)
{
   renderer->setScrollY(ui->verticalScrollBar->value());
   renderer->update();
}

void PanZoomRenderer::on_horizontalScrollBar_valueChanged(int /*value*/)
{
   renderer->setScrollX(ui->horizontalScrollBar->value());
   renderer->update();
}
