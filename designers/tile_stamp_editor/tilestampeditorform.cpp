#include "tilestampeditorform.h"
#include "ui_tilestampeditorform.h"

#include "cnessystempalette.h"

#include "cattributetable.h"

#include "cdesignercommon.h"

#include "main.h"

TileStampEditorForm::TileStampEditorForm(QByteArray data,QString attrTblUUID,int xSize,int ySize,bool grid,IProjectTreeViewItem* link,QWidget *parent) :
    CDesignerEditorBase(link,parent),
    ui(new Ui::TileStampEditorForm)
{
   IProjectTreeViewItemIterator iter(nesicideProject->getProject());
   IProjectTreeViewItem* item;
   CAttributeTable* pAttrTbl;
   QColor color;
   int idx;

   ui->setupUi(this);

   QList<int> sizes;
   sizes.append(800);
   sizes.append(200);
   ui->splitter->setSizes(sizes);

   QList<IChrRomBankItem*> tileList;
   while ( iter.current() )
   {
      IChrRomBankItem* pChrItem = dynamic_cast<IChrRomBankItem*>(iter.current());
      if ( pChrItem && (pChrItem->getItemType() == "Tile") )
      {
         tileList.append(pChrItem);
      }

      // Move through tree.
      iter.next();
   }
   tileListModel = new CChrRomItemListDisplayModel();
   tileListModel->setBankItems(tileList);
   ui->tileList->setModel(tileListModel);
   ui->tileList->setModelColumn(ChrRomBankItemCol_Image);

   imgData = new char[128*128*4];
   colorData = new char[128*128];

   m_xSize = xSize;
   m_ySize = ySize;
   m_attrTblUUID = attrTblUUID;
   m_gridEnabled = grid;

   // Set up list of color widgets.
   m_colors.append(ui->palbkgnd);
   m_colors.append(ui->pal0col1);
   m_colors.append(ui->pal0col2);
   m_colors.append(ui->pal0col3);
   m_colors.append(ui->palbkgnd);
   m_colors.append(ui->pal1col1);
   m_colors.append(ui->pal1col2);
   m_colors.append(ui->pal1col3);
   m_colors.append(ui->palbkgnd);
   m_colors.append(ui->pal2col1);
   m_colors.append(ui->pal2col2);
   m_colors.append(ui->pal2col3);
   m_colors.append(ui->palbkgnd);
   m_colors.append(ui->pal3col1);
   m_colors.append(ui->pal3col2);
   m_colors.append(ui->pal3col3);

   renderer = new CTileStampRenderer(ui->frame,imgData);
   renderer->setSize(m_xSize,m_ySize);
   renderer->setBGColor(QColor(100,100,100));
   ui->frame->layout()->addWidget(renderer);
   ui->frame->layout()->update();
   renderer->setMouseTracking(true);

   previewer = new CTileStampRenderer(ui->frame,imgData);
   previewer->setSize(m_xSize,m_ySize);
   previewer->setBGColor(QColor(100,100,100));
   ui->preview->layout()->addWidget(previewer);
   ui->preview->layout()->update();

   item = findProjectItem(attrTblUUID);
   if ( item )
   {
      pAttrTbl = dynamic_cast<CAttributeTable*>(item);
      if ( pAttrTbl )
      {
         QList<uint8_t> palette = pAttrTbl->getPalette();
         for ( idx = 0; idx < m_colors.count(); idx++ )
         {
            m_colors.at(idx)->setCurrentColor(QColor(nesGetPaletteRedComponent(pAttrTbl->getPalette().at(idx)),nesGetPaletteGreenComponent(pAttrTbl->getPalette().at(idx)),nesGetPaletteBlueComponent(pAttrTbl->getPalette().at(idx))));
         }
      }
   }
   else
   {
      for ( idx = 0; idx < m_colors.count(); idx+=4 )
      {
         m_colors.at(idx+0)->setCurrentColor(QColor(nesGetPaletteRedComponent(0x0D),nesGetPaletteGreenComponent(0x0D),nesGetPaletteBlueComponent(0x0D)));
         m_colors.at(idx+1)->setCurrentColor(QColor(nesGetPaletteRedComponent(0x00),nesGetPaletteGreenComponent(0x00),nesGetPaletteBlueComponent(0x00)));
         m_colors.at(idx+2)->setCurrentColor(QColor(nesGetPaletteRedComponent(0x10),nesGetPaletteGreenComponent(0x10),nesGetPaletteBlueComponent(0x10)));
         m_colors.at(idx+3)->setCurrentColor(QColor(nesGetPaletteRedComponent(0x20),nesGetPaletteGreenComponent(0x20),nesGetPaletteBlueComponent(0x20)));
      }
   }

   // Set up image...
   for ( idx = 0; idx < 128*128; idx++ )
   {
      colorData[idx] = 0;
   }
   initializeTile(data);
   for ( idx = 0; idx < 128*128*4; idx +=4 )
   {
      color = m_colors.at(colorData[idx>>2])->currentColor();
      imgData[idx+0] = color.blue();
      imgData[idx+1] = color.green();
      imgData[idx+2] = color.red();
      imgData[idx+3] = 0xFF;
   }

   for ( idx = 0; idx < m_colors.count(); idx++ )
   {
      QObject::connect(m_colors.at(idx),SIGNAL(clicked(bool)),this,SLOT(colorPicked(bool)));
   }

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

   // Set up the attribute table list.
   iter.reset(nesicideProject->getProject()->getProjectPrimitives()->getAttributeTables());
   idx = 0;
   int selIdx = -1;
   while ( iter.current() )
   {
      pAttrTbl = dynamic_cast<CAttributeTable*>(iter.current());
      if ( pAttrTbl )
      {
         ui->attributeTable->addItem(pAttrTbl->caption(),pAttrTbl->uuid());
         if ( pAttrTbl->uuid() == m_attrTblUUID )
         {
            selIdx = idx;
         }
         idx++;
      }
      iter.next();
   }
   ui->attributeTable->setCurrentIndex(selIdx);

   ui->grid->setChecked(m_gridEnabled);
   renderer->setGrid(m_gridEnabled);

   ui->paintAttr->setChecked(false);

   // Get mouse events from the renderer here!
   renderer->installEventFilter(this);

   // Connect UI slots here so we don't get confused about editor state.
   QObject::connect(ui->xSize,SIGNAL(currentIndexChanged(int)),this,SLOT(xSize_currentIndexChanged(int)));
   QObject::connect(ui->ySize,SIGNAL(currentIndexChanged(int)),this,SLOT(ySize_currentIndexChanged(int)));
   QObject::connect(ui->attributeTable,SIGNAL(currentIndexChanged(int)),this,SLOT(attributeTable_currentIndexChanged(int)));
   QObject::connect(ui->grid,SIGNAL(toggled(bool)),this,SLOT(grid_toggled(bool)));
}

TileStampEditorForm::~TileStampEditorForm()
{
   delete imgData;
   delete renderer;
   delete previewer;
   delete ui;
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
      else if ( event->type() == QEvent::MouseButtonDblClick )
      {
         QMouseEvent* mouseEvent = dynamic_cast<QMouseEvent*>(event);
         renderer_mouseDoubleClickEvent(mouseEvent);
         return true;
      }
      else if ( event->type() == QEvent::Enter )
      {
         QEvent* event = dynamic_cast<QEvent*>(event);
         renderer_enterEvent(event);
         return true;
      }
      else if ( event->type() == QEvent::Leave )
      {
         QEvent* event = dynamic_cast<QEvent*>(event);
         renderer_leaveEvent(event);
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

void TileStampEditorForm::colorPicked(bool value)
{
   int idx;
   bool bkgndPicked;

   if ( !value ) return;

   for ( idx = 0; idx < m_colors.count(); idx++ )
   {
      if ( sender() != m_colors.at(idx) )
      {
         m_colors.at(idx)->setChecked(false);
         ui->tileList->clearSelection();
      }
   }
}

void TileStampEditorForm::renderer_enterEvent(QEvent *event)
{
   int pixx;
   int pixy;
   int boxX1;
   int boxY1;
   int boxX2;
   int boxY2;
   bool visible;

   visible = renderer->pointToPixel(QCursor::pos().x(),QCursor::pos().y(),&pixx,&pixy);

   if ( visible )
   {
      boxX1 = pixx-(pixx%16);
      boxY1 = pixy-(pixy%16);
      boxX2 = boxX1+16;
      boxY2 = boxY1+16;
      renderer->setBox(boxX1,boxY1,boxX2,boxY2);
      renderer->repaint();
   }
}

void TileStampEditorForm::renderer_leaveEvent(QEvent *event)
{
   renderer->setBox();
   renderer->repaint();
}

void TileStampEditorForm::renderer_mouseDoubleClickEvent(QMouseEvent *event)
{
}

void TileStampEditorForm::renderer_mousePressEvent(QMouseEvent *event)
{
   QColor color;
   int pixx;
   int pixy;
   int boxX1;
   int boxY1;
   int boxX2;
   int boxY2;
   int idx;
   bool visible;

   visible = renderer->pointToPixel(event->pos().x(),event->pos().y(),&pixx,&pixy);

   if ( visible )
   {
      if ( ui->tileList->currentIndex().isValid() )
      {
         boxX1 = pixx-(pixx%16);
         boxY1 = pixy-(pixy%16);
         boxX2 = boxX1+16;
         boxY2 = boxY1+16;
         renderer->setBox(boxX1,boxY1,boxX2,boxY2);
         renderer->repaint();
      }
      else
      {
         if ( event->buttons() == Qt::LeftButton )
         {
            for ( idx = 0; idx < m_colors.count(); idx++ )
            {
               if ( m_colors.at(idx)->isChecked() )
               {
                  if ( ui->paintAttr->isChecked() )
                  {
                     recolorTiles(pixx,pixy,idx/4);
                  }
                  else
                  {
                     color = m_colors.at(idx)->currentColor();
                     if ( (idx%4) && (colorData[(pixy*128)+pixx] != idx) )
                     {
                        recolorTiles(pixx,pixy,idx/4);
                     }
                     colorData[(pixy*128)+pixx] = idx;
                     imgData[(pixy*128*4)+(pixx*4)+0] = color.blue();
                     imgData[(pixy*128*4)+(pixx*4)+1] = color.green();
                     imgData[(pixy*128*4)+(pixx*4)+2] = color.red();
                  }
                  renderer->repaint();
                  previewer->repaint();
                  emit markProjectDirty(true);
                  setModified(true);
                  break;
               }
            }
         }
         else if ( event->buttons() == Qt::RightButton )
         {
            color = m_colors.at(0)->currentColor();
            colorData[(pixy*128)+pixx] = 0;
            imgData[(pixy*128*4)+(pixx*4)+0] = color.blue();
            imgData[(pixy*128*4)+(pixx*4)+1] = color.green();
            imgData[(pixy*128*4)+(pixx*4)+2] = color.red();
            renderer->repaint();
            previewer->repaint();
            emit markProjectDirty(true);
            setModified(true);
         }
      }
   }
}

void TileStampEditorForm::renderer_mouseMoveEvent(QMouseEvent *event)
{
   QColor color;
   int pixx;
   int pixy;
   int boxX1;
   int boxY1;
   int boxX2;
   int boxY2;
   int idx;
   bool visible;

   visible = renderer->pointToPixel(event->pos().x(),event->pos().y(),&pixx,&pixy);

   if ( ui->tileList->currentIndex().isValid() )
   {
      renderer->getBox(&boxX1,&boxY1,&boxX2,&boxY2);
      boxX2 = pixx+(16-(pixx%16));
      boxY2 = pixy+(16-(pixy%16));
      renderer->setBox(boxX1,boxY1,boxX2,boxY2);
      renderer->repaint();
   }
   else
   {
      if ( visible )
      {
         boxX1 = pixx-(pixx%16);
         boxY1 = pixy-(pixy%16);
         boxX2 = boxX1+16;
         boxY2 = boxY1+16;
         renderer->setBox(boxX1,boxY1,boxX2,boxY2);
         renderer->repaint();
      }
      else
      {
         renderer->setBox();
         renderer->repaint();
      }

      if ( event->buttons() == Qt::LeftButton )
      {
         if ( visible )
         {
            for ( idx = 0; idx < m_colors.count(); idx++ )
            {
               if ( m_colors.at(idx)->isChecked() )
               {
                  if ( ui->paintAttr->isChecked() )
                  {
                     recolorTiles(pixx,pixy,idx/4);
                  }
                  else
                  {
                     color = m_colors.at(idx)->currentColor();
                     if ( (idx%4) && (colorData[(pixy*128)+pixx] != idx) )
                     {
                        recolorTiles(pixx,pixy,idx/4);
                     }
                     colorData[(pixy*128)+pixx] = idx;
                     imgData[(pixy*128*4)+(pixx*4)+0] = color.blue();
                     imgData[(pixy*128*4)+(pixx*4)+1] = color.green();
                     imgData[(pixy*128*4)+(pixx*4)+2] = color.red();
                  }
                  renderer->repaint();
                  previewer->repaint();
                  emit markProjectDirty(true);
                  setModified(true);
                  break;
               }
            }
         }
      }
      else if ( event->buttons() == Qt::RightButton )
      {
         color = m_colors.at(0)->currentColor();
         colorData[(pixy*128)+pixx] = 0;
         imgData[(pixy*128*4)+(pixx*4)+0] = color.blue();
         imgData[(pixy*128*4)+(pixx*4)+1] = color.green();
         imgData[(pixy*128*4)+(pixx*4)+2] = color.red();
         renderer->repaint();
         previewer->repaint();
         emit markProjectDirty(true);
         setModified(true);
      }
   }
}

void TileStampEditorForm::renderer_mouseReleaseEvent(QMouseEvent *event)
{
   QColor color;
   int pixx;
   int pixy;
   int idx;
   bool visible;

   visible = renderer->pointToPixel(event->pos().x(),event->pos().y(),&pixx,&pixy);

   if ( ui->tileList->currentIndex().isValid() )
   {
      renderer->setBox();
      renderer->repaint();
   }
   else
   {
      if ( visible )
      {
         if ( event->buttons() == Qt::LeftButton )
         {
            for ( idx = 0; idx < m_colors.count(); idx++ )
            {
               if ( m_colors.at(idx)->isChecked() )
               {
                  if ( ui->paintAttr->isChecked() )
                  {
                     recolorTiles(pixx,pixy,idx/4);
                  }
                  else
                  {
                     color = m_colors.at(idx)->currentColor();
                     if ( (idx%4) && (colorData[(pixy*128)+pixx] != idx) )
                     {
                        recolorTiles(pixx,pixy,idx/4);
                     }
                     colorData[(pixy*128)+pixx] = idx;
                     imgData[(pixy*128*4)+(pixx*4)+0] = color.blue();
                     imgData[(pixy*128*4)+(pixx*4)+1] = color.green();
                     imgData[(pixy*128*4)+(pixx*4)+2] = color.red();
                  }
                  renderer->repaint();
                  previewer->repaint();
                  emit markProjectDirty(true);
                  setModified(true);
                  break;
               }
            }
         }
         else if ( event->buttons() == Qt::RightButton )
         {
            color = m_colors.at(0)->currentColor();
            colorData[(pixy*128)+pixx] = 0;
            imgData[(pixy*128*4)+(pixx*4)+0] = color.blue();
            imgData[(pixy*128*4)+(pixx*4)+1] = color.green();
            imgData[(pixy*128*4)+(pixx*4)+2] = color.red();
            renderer->repaint();
            previewer->repaint();
            emit markProjectDirty(true);
            setModified(true);
         }
      }
   }
}

void TileStampEditorForm::on_zoomSlider_valueChanged(int value)
{
   renderer->changeZoom(pow(2.0,value)*100.0);
   ui->zoomValueLabel->setText(QString::number(pow(2.0,value)*100.0).append("%"));
   updateScrollbars();
}

void TileStampEditorForm::renderData()
{
}

void TileStampEditorForm::updateScrollbars()
{
   int value = ui->zoomSlider->value();
   int zf = pow(2.0,value);

   ui->horizontalScrollBar->setMaximum(m_xSize-(m_xSize/zf));
   ui->verticalScrollBar->setMaximum(m_ySize-(m_ySize/zf));
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
   m_xSize = ui->xSize->itemData(ui->xSize->currentIndex()).toInt();
   m_ySize = ui->ySize->itemData(ui->ySize->currentIndex()).toInt();
   renderer->setSize(m_xSize,m_ySize);
   renderer->repaint();
   previewer->setSize(m_xSize,m_ySize);
   previewer->repaint();
   emit markProjectDirty(true);
   setModified(true);
}

void TileStampEditorForm::ySize_currentIndexChanged(int index)
{
   m_xSize = ui->xSize->itemData(ui->xSize->currentIndex()).toInt();
   m_ySize = ui->ySize->itemData(ui->ySize->currentIndex()).toInt();
   renderer->setSize(m_xSize,m_ySize);
   renderer->repaint();
   previewer->setSize(m_xSize,m_ySize);
   previewer->repaint();
   emit markProjectDirty(true);
   setModified(true);
}

void TileStampEditorForm::attributeTable_currentIndexChanged(int index)
{
   IProjectTreeViewItem* item;
   CAttributeTable* pAttrTbl;
   QColor color;
   int idx;

   m_attrTblUUID = ui->attributeTable->itemData(index).toString();

   item = findProjectItem(m_attrTblUUID);
   if ( item )
   {
      pAttrTbl = dynamic_cast<CAttributeTable*>(item);
      if ( pAttrTbl )
      {
         QList<uint8_t> palette = pAttrTbl->getPalette();
         for ( idx = 0; idx < m_colors.count(); idx++ )
         {
            m_colors.at(idx)->setCurrentColor(QColor(nesGetPaletteRedComponent(pAttrTbl->getPalette().at(idx)),nesGetPaletteGreenComponent(pAttrTbl->getPalette().at(idx)),nesGetPaletteBlueComponent(pAttrTbl->getPalette().at(idx))));
            m_colors.at(idx)->repaint();
         }
      }
   }

   // Re-color image.
   for ( idx = 0; idx < 128*128*4; idx += 4 )
   {
      color = m_colors.at(colorData[idx>>2])->currentColor();
      imgData[idx+0] = color.blue();
      imgData[idx+1] = color.green();
      imgData[idx+2] = color.red();
   }
   renderer->repaint();
   previewer->repaint();

   emit markProjectDirty(true);
   setModified(true);
}

void TileStampEditorForm::recolorTiles(int pixx,int pixy,int newColorTable)
{
   int aqx_in = PIXEL_TO_ATTRQUAD(pixx);
   int aqy_in = PIXEL_TO_ATTRQUAD(pixy);
   int as_in = PIXEL_TO_ATTRSECTION(pixx,pixy);
   int aqx;
   int aqy;
   int as;
   int idxx;
   int idxy;
   int idx;
   QColor color;

   for ( idxy = 0; idxy < m_ySize; idxy++ )
   {
      for ( idxx = 0; idxx < m_xSize; idxx++ )
      {
         aqx = PIXEL_TO_ATTRQUAD(idxx);
         aqy = PIXEL_TO_ATTRQUAD(idxy);
         as = PIXEL_TO_ATTRSECTION(idxx,idxy);
         if ( (aqx == aqx_in) &&
              (aqy == aqy_in) &&
              (as == as_in) )
         {
            colorData[(idxy*128)+idxx] &= 0x3;
            colorData[(idxy*128)+idxx] |= (newColorTable<<2);
         }
      }
   }

   // Re-color image.
   for ( idx = 0; idx < 128*128*4; idx += 4 )
   {
      color = m_colors.at(colorData[idx>>2])->currentColor();
      imgData[idx+0] = color.blue();
      imgData[idx+1] = color.green();
      imgData[idx+2] = color.red();
   }
}

void TileStampEditorForm::applyChangesToTab(QString uuid)
{
   IProjectTreeViewItemIterator iter;
   IProjectTreeViewItem* item;
   CAttributeTable* pAttrTbl;
   QColor color;
   int idx;

   // Set up the attribute table list.
   iter.reset(nesicideProject->getProject()->getProjectPrimitives()->getAttributeTables());
   idx = 0;
   int selIdx = -1;
   ui->attributeTable->blockSignals(true);
   ui->attributeTable->clear();
   while ( iter.current() )
   {
      pAttrTbl = dynamic_cast<CAttributeTable*>(iter.current());
      if ( pAttrTbl )
      {
         ui->attributeTable->addItem(pAttrTbl->caption(),pAttrTbl->uuid());
         if ( pAttrTbl->uuid() == m_attrTblUUID )
         {
            selIdx = idx;
         }
         idx++;
      }
      iter.next();
   }
   ui->attributeTable->setCurrentIndex(selIdx);
   ui->attributeTable->blockSignals(false);

   // Update attribute table in use.
   if ( m_attrTblUUID == uuid )
   {
      item = findProjectItem(m_attrTblUUID);
      if ( item )
      {
         pAttrTbl = dynamic_cast<CAttributeTable*>(item);
         if ( pAttrTbl )
         {
            QList<uint8_t> palette = pAttrTbl->getPalette();
            for ( idx = 0; idx < m_colors.count(); idx++ )
            {
               m_colors.at(idx)->setCurrentColor(QColor(nesGetPaletteRedComponent(pAttrTbl->getPalette().at(idx)),nesGetPaletteGreenComponent(pAttrTbl->getPalette().at(idx)),nesGetPaletteBlueComponent(pAttrTbl->getPalette().at(idx))));
               m_colors.at(idx)->repaint();
            }
         }
      }

      // Re-color image.
      for ( idx = 0; idx < 128*128*4; idx += 4 )
      {
         color = m_colors.at(colorData[idx>>2])->currentColor();
         imgData[idx+0] = color.blue();
         imgData[idx+1] = color.green();
         imgData[idx+2] = color.red();
      }
      renderer->repaint();
      previewer->repaint();
   }

   // Update tile list.
   QList<IChrRomBankItem*> tileList;
   iter.reset(nesicideProject->getProject());
   while ( iter.current() )
   {
      IChrRomBankItem* pChrItem = dynamic_cast<IChrRomBankItem*>(iter.current());
      if ( pChrItem && (pChrItem->getItemType() == "Tile") )
      {
         tileList.append(pChrItem);
      }

      // Move through tree.
      iter.next();
   }
   tileListModel->setBankItems(tileList);
   tileListModel->update();
}

void TileStampEditorForm::grid_toggled(bool checked)
{
   m_gridEnabled = checked;
   renderer->setGrid(m_gridEnabled);
   renderer->repaint();

   emit markProjectDirty(true);
   setModified(true);
}

void TileStampEditorForm::initializeTile(QByteArray tileData)
{
   int x;
   int y;
   int tile;
   int width = m_xSize;
   int height = m_ySize;
   int tileX;
   int tileY;
   int tileWidth = width/8;
   int tileHeight = height/8;
   int numTiles = tileData.count()/0x10;
   char plane1;
   char plane2;
   char pixel;

   for ( tile = 0; tile < numTiles; tile++ )
   {
      tileX = (tile%tileWidth)*8;
      tileY = (tile/tileWidth)*8;

      for ( y = 0; y < 8; y++ )
      {
         plane1 = tileData.at((tile<<4)+y);
         plane2 = tileData.at((tile<<4)+y+8);

         for ( x = 0; x < 8; x++ )
         {
            pixel = (!!(plane1&0x80))
                  | ((!!(plane2&0x80))<<1);
            colorData[((tileY+y)*128)+tileX+x] = pixel;
            plane1 <<= 1;
            plane2 <<= 1;
         }
      }
   }
}

QByteArray TileStampEditorForm::tileData()
{
   QByteArray chrOut;
   int x = 0;
   int y = 0;
   int width = m_xSize;
   int height = m_ySize;
   int tile;
   int tileWidth = width/8;
   int tileHeight = height/8;
   int tileX;
   int tileY;
   int numTiles = (tileWidth*tileHeight);
   char plane1[8];
   char plane2[8];

   for ( tile = 0; tile < numTiles; tile++ )
   {
      tileX = (tile%tileWidth)*8;
      tileY = (tile/tileWidth)*8;

      for ( y = 0; y < 8; y++ )
      {
         for ( x = 0; x < 8; x++ )
         {
            plane1[y] <<= 1;
            plane2[y] <<= 1;

            plane1[y] |= colorData[((tileY+y)*128)+tileX+x]&0x01;
            plane2[y] |= ((colorData[((tileY+y)*128)+tileX+x]&0x02)>>1);
         }
      }
      chrOut.append(plane1,8);
      chrOut.append(plane2,8);
   }
   return chrOut;
}

void TileStampEditorForm::on_clear_clicked()
{
   int idx;
   QColor color;

   // Set up image...
   for ( idx = 0; idx < 128*128; idx++ )
   {
      colorData[idx] = 0;
   }
   for ( idx = 0; idx < 128*128*4; idx +=4 )
   {
      color = m_colors.at(colorData[idx>>2])->currentColor();
      imgData[idx+0] = color.blue();
      imgData[idx+1] = color.green();
      imgData[idx+2] = color.red();
      imgData[idx+3] = 0xFF;
   }
   renderer->repaint();
   previewer->repaint();

   emit markProjectDirty(true);
   setModified(true);
}

void TileStampEditorForm::on_paint_clicked()
{

}

void TileStampEditorForm::on_tileList_activated(QModelIndex index)
{
   int idx;

   for ( idx = 0; idx < m_colors.count(); idx++ )
   {
      m_colors.at(idx)->setChecked(false);
   }
}

void TileStampEditorForm::on_tileList_clicked(QModelIndex index)
{
   int idx;

   for ( idx = 0; idx < m_colors.count(); idx++ )
   {
      m_colors.at(idx)->setChecked(false);
   }
}
