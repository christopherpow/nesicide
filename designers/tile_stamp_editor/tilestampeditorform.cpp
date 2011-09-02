#include "tilestampeditorform.h"
#include "ui_tilestampeditorform.h"

#include "cnessystempalette.h"

TileStampEditorForm::TileStampEditorForm(QByteArray data,int xSize,int ySize,IProjectTreeViewItem* link,QWidget *parent) :
    CDesignerEditorBase(link,parent),
    ui(new Ui::TileStampEditorForm)
{
   int i;

   ui->setupUi(this);

   imgData = new char[256*256*4];

   m_xSize = xSize;
   m_ySize = ySize;

   // Clear image...
   for ( i = 0; i < 256*256*4; i+=4 )
   {
      imgData[i] = rand()%256;
      imgData[i+1] = rand()%256;
      imgData[i+2] = rand()%256;
      imgData[i+3] = 0xFF;
   }

   renderer = new CTileStampRenderer(ui->frame,imgData);
   renderer->setSize(m_xSize,m_ySize);
   ui->frame->layout()->addWidget(renderer);
   ui->frame->layout()->update();

   ui->col0PushButton->setCurrentColor(QColor(nesGetPaletteRedComponent(0x0D),nesGetPaletteGreenComponent(0x0D),nesGetPaletteBlueComponent(0x0D)));
   ui->col1PushButton->setCurrentColor(QColor(nesGetPaletteRedComponent(0x00),nesGetPaletteGreenComponent(0x00),nesGetPaletteBlueComponent(0x00)));
   ui->col2PushButton->setCurrentColor(QColor(nesGetPaletteRedComponent(0x10),nesGetPaletteGreenComponent(0x10),nesGetPaletteBlueComponent(0x10)));
   ui->col3PushButton->setCurrentColor(QColor(nesGetPaletteRedComponent(0x20),nesGetPaletteGreenComponent(0x20),nesGetPaletteBlueComponent(0x20)));

   ui->col0PushButton->setColorPopupEnabled(false);
   ui->col1PushButton->setColorPopupEnabled(false);
   ui->col2PushButton->setColorPopupEnabled(false);
   ui->col3PushButton->setColorPopupEnabled(false);

   QObject::connect(ui->col0PushButton,SIGNAL(pressed()),this,SLOT(colorPicked()));
   QObject::connect(ui->col1PushButton,SIGNAL(pressed()),this,SLOT(colorPicked()));
   QObject::connect(ui->col2PushButton,SIGNAL(pressed()),this,SLOT(colorPicked()));
   QObject::connect(ui->col3PushButton,SIGNAL(pressed()),this,SLOT(colorPicked()));
   QObject::connect(ui->col0PushButton,SIGNAL(released()),this,SLOT(colorPicked()));
   QObject::connect(ui->col1PushButton,SIGNAL(released()),this,SLOT(colorPicked()));
   QObject::connect(ui->col2PushButton,SIGNAL(released()),this,SLOT(colorPicked()));
   QObject::connect(ui->col3PushButton,SIGNAL(released()),this,SLOT(colorPicked()));

   //CPTODO: how to set the active color initially?

   // Combo-box index-to-size relations.
   ui->xSize->setItemData(0,8);
   ui->xSize->setItemData(1,16);
   ui->xSize->setItemData(2,32);
   ui->xSize->setItemData(3,48);
   ui->xSize->setItemData(4,64);
   ui->xSize->setItemData(5,80);
   ui->xSize->setItemData(6,96);
   ui->xSize->setItemData(7,112);
   ui->xSize->setItemData(8,128);
   ui->ySize->setItemData(0,8);
   ui->ySize->setItemData(1,16);
   ui->ySize->setItemData(2,32);
   ui->ySize->setItemData(3,48);
   ui->ySize->setItemData(4,64);
   ui->ySize->setItemData(5,80);
   ui->ySize->setItemData(6,96);
   ui->ySize->setItemData(7,112);
   ui->ySize->setItemData(8,128);

   switch ( m_xSize )
   {
   case 8:
      ui->xSize->setCurrentIndex(0);
      break;
   case 16:
      ui->xSize->setCurrentIndex(1);
      break;
   case 32:
      ui->xSize->setCurrentIndex(2);
      break;
   case 48:
      ui->xSize->setCurrentIndex(3);
      break;
   case 64:
      ui->xSize->setCurrentIndex(4);
      break;
   case 80:
      ui->xSize->setCurrentIndex(5);
      break;
   case 96:
      ui->xSize->setCurrentIndex(6);
      break;
   case 112:
      ui->xSize->setCurrentIndex(7);
      break;
   case 128:
      ui->xSize->setCurrentIndex(8);
      break;
   }
   switch ( m_ySize )
   {
   case 8:
      ui->ySize->setCurrentIndex(0);
      break;
   case 16:
      ui->ySize->setCurrentIndex(1);
      break;
   case 32:
      ui->ySize->setCurrentIndex(2);
      break;
   case 48:
      ui->ySize->setCurrentIndex(3);
      break;
   case 64:
      ui->ySize->setCurrentIndex(4);
      break;
   case 80:
      ui->ySize->setCurrentIndex(5);
      break;
   case 96:
      ui->ySize->setCurrentIndex(6);
      break;
   case 112:
      ui->ySize->setCurrentIndex(7);
      break;
   case 128:
      ui->ySize->setCurrentIndex(8);
      break;
   }

   // Get mouse events from the renderer here!
   renderer->installEventFilter(this);

   // Connect UI slots here so we don't get confused about editor state.
   QObject::connect(ui->xSize,SIGNAL(currentIndexChanged(int)),this,SLOT(xSize_currentIndexChanged(int)));
   QObject::connect(ui->ySize,SIGNAL(currentIndexChanged(int)),this,SLOT(ySize_currentIndexChanged(int)));
}

TileStampEditorForm::~TileStampEditorForm()
{
   delete ui;
   delete renderer;
   delete imgData;
}

bool TileStampEditorForm::eventFilter(QObject* obj,QEvent* event)
{
   if ( obj == renderer )
   {
      if ( event->type() == QEvent::MouseButtonPress )
      {
         QMouseEvent* mouseEvent = dynamic_cast<QMouseEvent*>(event);
         renderer_mousePressEvent(mouseEvent);
         return true;
      }
      else if ( event->type() == QEvent::MouseButtonRelease )
      {
         QMouseEvent* mouseEvent = dynamic_cast<QMouseEvent*>(event);
         renderer_mouseReleaseEvent(mouseEvent);
         return true;
      }
      else if ( event->type() == QEvent::MouseMove )
      {
         QMouseEvent* mouseEvent = dynamic_cast<QMouseEvent*>(event);
         renderer_mouseMoveEvent(mouseEvent);
         return true;
      }
   }
   return false;
}

void TileStampEditorForm::changeEvent(QEvent* event)
{
   QWidget::changeEvent(event);

   switch (event->type())
   {
      case QEvent::LanguageChange:
         ui->retranslateUi(this);
         break;
      default:
         break;
   }
}

void TileStampEditorForm::contextMenuEvent(QContextMenuEvent *event)
{
}

void TileStampEditorForm::resizeEvent(QResizeEvent* event)
{
   updateScrollbars();
}

void TileStampEditorForm::currentSize(int *xSize, int *ySize)
{
   (*xSize) = ui->xSize->itemData(ui->xSize->currentIndex()).toInt();
   (*ySize) = ui->ySize->itemData(ui->ySize->currentIndex()).toInt();
}

void TileStampEditorForm::colorPicked()
{
   if ( sender() == ui->col0PushButton )
   {
      ui->col0PushButton->setChecked(true);
      ui->col1PushButton->setChecked(false);
      ui->col2PushButton->setChecked(false);
      ui->col3PushButton->setChecked(false);
   }
   else if ( sender() == ui->col1PushButton )
   {
      ui->col0PushButton->setChecked(false);
      ui->col1PushButton->setChecked(true);
      ui->col2PushButton->setChecked(false);
      ui->col3PushButton->setChecked(false);
   }
   else if ( sender() == ui->col2PushButton )
   {
      ui->col0PushButton->setChecked(false);
      ui->col1PushButton->setChecked(false);
      ui->col2PushButton->setChecked(true);
      ui->col3PushButton->setChecked(false);
   }
   else if ( sender() == ui->col3PushButton )
   {
      ui->col0PushButton->setChecked(false);
      ui->col1PushButton->setChecked(false);
      ui->col2PushButton->setChecked(false);
      ui->col3PushButton->setChecked(true);
   }
}

void TileStampEditorForm::renderer_mousePressEvent(QMouseEvent *event)
{
   int pixx;
   int pixy;

   renderer->pointToPixel(event->pos().x(),event->pos().y(),&pixx,&pixy);

   QColor color;
   if ( ui->col0PushButton->isChecked() )
   {
      color = ui->col0PushButton->currentColor();
      imgData[(pixy*256*4)+(pixx*4)+0] = color.red();
      imgData[(pixy*256*4)+(pixx*4)+1] = color.green();
      imgData[(pixy*256*4)+(pixx*4)+2] = color.blue();
      renderer->repaint();
      emit markProjectDirty(true);
      setModified(true);
   }
   else if ( ui->col1PushButton->isChecked() )
   {
      color = ui->col1PushButton->currentColor();
      imgData[(pixy*256*4)+(pixx*4)+0] = color.red();
      imgData[(pixy*256*4)+(pixx*4)+1] = color.green();
      imgData[(pixy*256*4)+(pixx*4)+2] = color.blue();
      renderer->repaint();
      emit markProjectDirty(true);
      setModified(true);
   }
   else if ( ui->col2PushButton->isChecked() )
   {
      color = ui->col2PushButton->currentColor();
      imgData[(pixy*256*4)+(pixx*4)+0] = color.red();
      imgData[(pixy*256*4)+(pixx*4)+1] = color.green();
      imgData[(pixy*256*4)+(pixx*4)+2] = color.blue();
      renderer->repaint();
      emit markProjectDirty(true);
      setModified(true);
   }
   else if ( ui->col3PushButton->isChecked() )
   {
      color = ui->col3PushButton->currentColor();
      imgData[(pixy*256*4)+(pixx*4)+0] = color.red();
      imgData[(pixy*256*4)+(pixx*4)+1] = color.green();
      imgData[(pixy*256*4)+(pixx*4)+2] = color.blue();
      renderer->repaint();
      emit markProjectDirty(true);
      setModified(true);
   }
}

void TileStampEditorForm::renderer_mouseMoveEvent(QMouseEvent *event)
{
   int pixx;
   int pixy;

   renderer->pointToPixel(event->pos().x(),event->pos().y(),&pixx,&pixy);

   QColor color;
   if ( ui->col0PushButton->isChecked() )
   {
      color = ui->col0PushButton->currentColor();
      imgData[(pixy*256*4)+(pixx*4)+0] = color.red();
      imgData[(pixy*256*4)+(pixx*4)+1] = color.green();
      imgData[(pixy*256*4)+(pixx*4)+2] = color.blue();
      renderer->repaint();
      emit markProjectDirty(true);
      setModified(true);
   }
   else if ( ui->col1PushButton->isChecked() )
   {
      color = ui->col1PushButton->currentColor();
      imgData[(pixy*256*4)+(pixx*4)+0] = color.red();
      imgData[(pixy*256*4)+(pixx*4)+1] = color.green();
      imgData[(pixy*256*4)+(pixx*4)+2] = color.blue();
      renderer->repaint();
      emit markProjectDirty(true);
      setModified(true);
   }
   else if ( ui->col2PushButton->isChecked() )
   {
      color = ui->col2PushButton->currentColor();
      imgData[(pixy*256*4)+(pixx*4)+0] = color.red();
      imgData[(pixy*256*4)+(pixx*4)+1] = color.green();
      imgData[(pixy*256*4)+(pixx*4)+2] = color.blue();
      renderer->repaint();
      emit markProjectDirty(true);
      setModified(true);
   }
   else if ( ui->col3PushButton->isChecked() )
   {
      color = ui->col3PushButton->currentColor();
      imgData[(pixy*256*4)+(pixx*4)+0] = color.red();
      imgData[(pixy*256*4)+(pixx*4)+1] = color.green();
      imgData[(pixy*256*4)+(pixx*4)+2] = color.blue();
      renderer->repaint();
      emit markProjectDirty(true);
      setModified(true);
   }
}

void TileStampEditorForm::renderer_mouseReleaseEvent(QMouseEvent *event)
{
   int pixx;
   int pixy;

   renderer->pointToPixel(event->pos().x(),event->pos().y(),&pixx,&pixy);

   QColor color;
   if ( ui->col0PushButton->isChecked() )
   {
      color = ui->col0PushButton->currentColor();
      imgData[(pixy*256*4)+(pixx*4)+0] = color.red();
      imgData[(pixy*256*4)+(pixx*4)+1] = color.green();
      imgData[(pixy*256*4)+(pixx*4)+2] = color.blue();
      renderer->repaint();
      emit markProjectDirty(true);
      setModified(true);
   }
   else if ( ui->col1PushButton->isChecked() )
   {
      color = ui->col1PushButton->currentColor();
      imgData[(pixy*256*4)+(pixx*4)+0] = color.red();
      imgData[(pixy*256*4)+(pixx*4)+1] = color.green();
      imgData[(pixy*256*4)+(pixx*4)+2] = color.blue();
      renderer->repaint();
      emit markProjectDirty(true);
      setModified(true);
   }
   else if ( ui->col2PushButton->isChecked() )
   {
      color = ui->col2PushButton->currentColor();
      imgData[(pixy*256*4)+(pixx*4)+0] = color.red();
      imgData[(pixy*256*4)+(pixx*4)+1] = color.green();
      imgData[(pixy*256*4)+(pixx*4)+2] = color.blue();
      renderer->repaint();
      emit markProjectDirty(true);
      setModified(true);
   }
   else if ( ui->col3PushButton->isChecked() )
   {
      color = ui->col3PushButton->currentColor();
      imgData[(pixy*256*4)+(pixx*4)+0] = color.red();
      imgData[(pixy*256*4)+(pixx*4)+1] = color.green();
      imgData[(pixy*256*4)+(pixx*4)+2] = color.blue();
      renderer->repaint();
      emit markProjectDirty(true);
      setModified(true);
   }
}

void TileStampEditorForm::on_zoomSlider_valueChanged(int value)
{
   value = value-(value%100);
   ui->zoomSlider->setValue(value);
   renderer->changeZoom(value);
   ui->zoomValueLabel->setText(QString::number(value).append("%"));
   updateScrollbars();
}

void TileStampEditorForm::renderData()
{
}

void TileStampEditorForm::updateScrollbars()
{
   int value = ui->zoomSlider->value();
   value = value-(value%100);
   int viewWidth = (float)256 * ((float)value / 100.0f);
   int viewHeight = (float)256 * ((float)value / 100.0f);
   ui->horizontalScrollBar->setMaximum(viewWidth - renderer->width() < 0 ? 0 : ((viewWidth - renderer->width()) / ((float)value / 100.0f)) + 1);
   ui->verticalScrollBar->setMaximum(viewHeight - renderer->height() < 0 ? 0 : ((viewHeight - renderer->height()) / ((float)value / 100.0f)) + 1);
   renderer->setScrollX(ui->horizontalScrollBar->value());
   renderer->setScrollY(ui->verticalScrollBar->value());
}

void TileStampEditorForm::on_horizontalScrollBar_valueChanged(int value)
{
   renderer->setScrollX(ui->horizontalScrollBar->value());
   renderer->repaint();
}

void TileStampEditorForm::on_verticalScrollBar_valueChanged(int value)
{
   renderer->setScrollY(ui->verticalScrollBar->value());
   renderer->repaint();
}

void TileStampEditorForm::xSize_currentIndexChanged(int index)
{
   renderer->setSize(ui->xSize->itemData(ui->xSize->currentIndex()).toInt(),ui->ySize->itemData(ui->ySize->currentIndex()).toInt());
   renderer->repaint();
   emit markProjectDirty(true);
   setModified(true);
}

void TileStampEditorForm::ySize_currentIndexChanged(int index)
{
   renderer->setSize(ui->xSize->itemData(ui->xSize->currentIndex()).toInt(),ui->ySize->itemData(ui->ySize->currentIndex()).toInt());
   renderer->repaint();
   emit markProjectDirty(true);
   setModified(true);
}
