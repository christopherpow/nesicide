#include "tilestampeditorform.h"
#include "ui_tilestampeditorform.h"

#include "cnessystempalette.h"

#include "cattributetable.h"
#include "cdesignercommon.h"

#include "main.h"

TileStampEditorForm::TileStampEditorForm(QByteArray data,QByteArray attr,QString attrTblUUID,int xSize,int ySize,bool grid,IProjectTreeViewItem* link,QWidget *parent) :
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

   // Start with pencil tool active.
   m_activeTool = ui->pencilTool;
   m_activeTool->setChecked(true);

   // Pull the anchor.
   m_anchor = QPoint(-1,-1);

   // No part of the image is selected.
   m_selection = false;

   // Nothing is on the clipboard.
   m_clipboard = false;

   // Set up default color selected.
   ui->pal0col1->setChecked(true);

   QList<IChrRomBankItem*> tileListChrItem;
   while ( iter.current() )
   {
      IChrRomBankItem* pChrItem = dynamic_cast<IChrRomBankItem*>(iter.current());
      if ( pChrItem )
      {
         tileListChrItem.append(pChrItem);
      }

      // Move through tree.
      iter.next();
   }
   tileListModel = new CChrRomItemListDisplayModel(false);
   tileListModel->setBankItems(tileListChrItem);

   ui->tileList->setModel(tileListModel);
   ui->tileList->setModelColumn(ChrRomBankItemCol_Image);

   imgData = new char[128*128*4];
   colorData = new char[128*128];
   colorDataOverlay = new char[128*128];
   colorDataClipboard = new char[128*128];

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
      colorDataOverlay[idx] = 0;
      colorDataClipboard[idx] = 0;
   }
   initializeTile(data,attr);
   paintNormal();

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
         QEvent* enterEvent = dynamic_cast<QEvent*>(event);
         renderer_enterEvent(enterEvent);
         return true;
      }
      else if ( event->type() == QEvent::Leave )
      {
         QEvent* leaveEvent = dynamic_cast<QEvent*>(event);
         renderer_leaveEvent(leaveEvent);
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

void TileStampEditorForm::keyPressEvent(QKeyEvent *event)
{
   int boxX1;
   int boxY1;
   int boxX2;
   int boxY2;

   if ( event->modifiers() == Qt::ControlModifier )
   {
      if ( event->key() == Qt::Key_C )
      {
         if ( (m_activeTool == ui->selectionTool) &&
              (m_selection) && (!m_clipboard) )
         {
            copyNormalToClipboard(m_selectionRect.left(),m_selectionRect.top(),m_selectionRect.right(),m_selectionRect.bottom());
         }
      }
      else if ( event->key() == Qt::Key_V )
      {
         if ( (m_activeTool == ui->selectionTool) &&
              (m_clipboard) )
         {
            renderer->setBox(m_selectionRect.left(),m_selectionRect.top(),m_selectionRect.right(),m_selectionRect.bottom());
            paintOverlay(Overlay_PasteClipboard,0,m_selectionRect.left(),m_selectionRect.top(),m_selectionRect.right(),m_selectionRect.bottom());
            copyOverlayToNormal();
            renderer->repaint();
            previewer->repaint();
            emit markProjectDirty(true);
            setModified(true);
         }
      }
      else if ( event->key() == Qt::Key_X)
      {
         if ( (m_activeTool == ui->selectionTool) &&
              (m_selection) )
         {
            renderer->getBox(&boxX1,&boxY1,&boxX2,&boxY2);
            copyNormalToClipboard(boxX1,boxY1,boxX2,boxY2);
            paintOverlay(Overlay_Erase,0,boxX1,boxY1,boxX2,boxY2);
            copyOverlayToNormal();
            renderer->repaint();
            previewer->repaint();
            emit markProjectDirty(true);
            setModified(true);
         }
      }
   }
   CDesignerEditorBase::keyPressEvent(event);
}

void TileStampEditorForm::renderer_enterEvent(QEvent *event)
{
   int pixx;
   int pixy;
   bool visible;

   visible = renderer->pointToPixel(QCursor::pos().x(),QCursor::pos().y(),&pixx,&pixy);

   if ( visible )
   {
      updateInfoText(pixx,pixy);
   }
}

void TileStampEditorForm::renderer_leaveEvent(QEvent *event)
{
   if ( (m_activeTool == ui->pencilTool) ||
        (m_activeTool == ui->paintAttr) )
   {
      renderer->setBox();
      renderer->repaint();
   }

   QMouseEvent mouseEvent(QEvent::MouseButtonRelease,QCursor::pos(),Qt::LeftButton,Qt::NoButton,Qt::NoModifier);
   renderer_mouseReleaseEvent(&mouseEvent);

   updateInfoText();
}

void TileStampEditorForm::renderer_mouseDoubleClickEvent(QMouseEvent *event)
{
}

void TileStampEditorForm::renderer_mousePressEvent(QMouseEvent *event)
{
   int pixx;
   int pixy;
   bool visible;

   visible = renderer->pointToPixel(event->pos().x(),event->pos().y(),&pixx,&pixy);

   if ( visible )
   {
      updateInfoText(pixx,pixy);

      if ( (ui->pencilTool->isChecked()) ||
           (ui->paintAttr->isChecked()) )
      {
         pencilTool(event);
      }
      else if ( ui->paintTool->isChecked() )
      {
         paintTool(event);
      }
      else if ( ui->textTool->isChecked() )
      {
         textTool(event);
      }
      else if ( ui->boxTool->isChecked() )
      {
         hollowBoxTool(event);
      }
      else if ( ui->areaTool->isChecked() )
      {
         filledBoxTool(event);
      }
      else if ( ui->lineTool->isChecked() )
      {
         lineTool(event);
      }
      else if ( ui->selectionTool->isChecked() )
      {
         selectionTool(event);
      }
      else if ( ui->tileList->currentIndex().isValid() )
      {
         tileTool(event);
      }
   }
}

void TileStampEditorForm::renderer_mouseMoveEvent(QMouseEvent *event)
{
   int pixx;
   int pixy;

   renderer->pointToPixel(event->pos().x(),event->pos().y(),&pixx,&pixy);

   updateInfoText(pixx,pixy);

   if ( (ui->pencilTool->isChecked()) ||
        (ui->paintAttr->isChecked()) )
   {
      pencilTool(event);
   }
   else if ( ui->paintTool->isChecked() )
   {
      paintTool(event);
   }
   else if ( ui->textTool->isChecked() )
   {
      textTool(event);
   }
   else if ( ui->boxTool->isChecked() )
   {
      hollowBoxTool(event);
   }
   else if ( ui->areaTool->isChecked() )
   {
      filledBoxTool(event);
   }
   else if ( ui->lineTool->isChecked() )
   {
      lineTool(event);
   }
   else if ( ui->selectionTool->isChecked() )
   {
      selectionTool(event);
   }
   else if ( ui->tileList->currentIndex().isValid() )
   {
      tileTool(event);
   }
}

void TileStampEditorForm::renderer_mouseReleaseEvent(QMouseEvent *event)
{
   int pixx;
   int pixy;
   bool visible;

   renderer->pointToPixel(event->pos().x(),event->pos().y(),&pixx,&pixy);

   updateInfoText(pixx,pixy);

   if ( (ui->pencilTool->isChecked()) ||
        (ui->paintAttr->isChecked()) )
   {
      pencilTool(event);
   }
   else if ( ui->paintTool->isChecked() )
   {
      paintTool(event);
   }
   else if ( ui->textTool->isChecked() )
   {
      textTool(event);
   }
   else if ( ui->boxTool->isChecked() )
   {
      hollowBoxTool(event);
   }
   else if ( ui->areaTool->isChecked() )
   {
      filledBoxTool(event);
   }
   else if ( ui->lineTool->isChecked() )
   {
      lineTool(event);
   }
   else if ( ui->selectionTool->isChecked() )
   {
      selectionTool(event);
   }
   else if ( ui->tileList->currentIndex().isValid() )
   {
      tileTool(event);
   }
}

void TileStampEditorForm::on_zoomSlider_valueChanged(int value)
{
   renderer->changeZoom(pow(2.0,value)*100.0);
   ui->zoomValueLabel->setText(QString::number(pow(2.0,value)*100.0).append("%"));
   updateScrollbars();
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

void TileStampEditorForm::grid_toggled(bool checked)
{
   m_gridEnabled = checked;
   renderer->setGrid(m_gridEnabled);
   renderer->repaint();

   emit markProjectDirty(true);
   setModified(true);
}

void TileStampEditorForm::on_clear_clicked()
{
   int idx;
   QColor color;

   // Set up image...
   for ( idx = 0; idx < 128*128; idx++ )
   {
      colorData[idx] = 0;
      colorDataOverlay[idx] = 0;
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

void TileStampEditorForm::on_selectionTool_clicked()
{
   m_activeTool = ui->selectionTool;

   // Clear selected tile if any.
   ui->tileList->setCurrentIndex(QModelIndex());
   ui->pencilTool->setChecked(false);
   ui->textTool->setChecked(false);
   ui->boxTool->setChecked(false);
   ui->areaTool->setChecked(false);
   ui->lineTool->setChecked(false);
   ui->paintTool->setChecked(false);
   ui->paintAttr->setChecked(false);
   ui->selectionTool->setChecked(true);

   // Haven't selected anything yet.
   m_selection = false;

   // Clipboard is not valid.
   m_clipboard = false;
}

void TileStampEditorForm::on_paintTool_clicked()
{
   m_activeTool = ui->paintTool;

   // Clear selected tile if any.
   ui->tileList->setCurrentIndex(QModelIndex());
   ui->pencilTool->setChecked(false);
   ui->textTool->setChecked(false);
   ui->boxTool->setChecked(false);
   ui->areaTool->setChecked(false);
   ui->lineTool->setChecked(false);
   ui->selectionTool->setChecked(false);
   ui->paintAttr->setChecked(false);
   ui->paintTool->setChecked(true);

   // Haven't selected anything yet.
   m_selection = false;

   // Clipboard is not valid.
   m_clipboard = false;
}

void TileStampEditorForm::on_pencilTool_clicked()
{
   m_activeTool = ui->pencilTool;

   // Clear selected tile if any.
   ui->tileList->setCurrentIndex(QModelIndex());
   ui->paintTool->setChecked(false);
   ui->textTool->setChecked(false);
   ui->boxTool->setChecked(false);
   ui->areaTool->setChecked(false);
   ui->lineTool->setChecked(false);
   ui->selectionTool->setChecked(false);
   ui->paintAttr->setChecked(false);
   ui->pencilTool->setChecked(true);

   // Haven't selected anything yet.
   m_selection = false;

   // Clipboard is not valid.
   m_clipboard = false;
}

void TileStampEditorForm::on_textTool_clicked()
{
   m_activeTool = ui->textTool;

   // Clear selected tile if any.
   ui->tileList->setCurrentIndex(QModelIndex());
   ui->paintTool->setChecked(false);
   ui->pencilTool->setChecked(false);
   ui->boxTool->setChecked(false);
   ui->areaTool->setChecked(false);
   ui->lineTool->setChecked(false);
   ui->selectionTool->setChecked(false);
   ui->paintAttr->setChecked(false);
   ui->textTool->setChecked(true);

   // Haven't selected anything yet.
   m_selection = false;

   // Clipboard is not valid.
   m_clipboard = false;
}

void TileStampEditorForm::on_boxTool_clicked()
{
   m_activeTool = ui->boxTool;

   // Clear selected tile if any.
   ui->tileList->setCurrentIndex(QModelIndex());
   ui->paintTool->setChecked(false);
   ui->pencilTool->setChecked(false);
   ui->textTool->setChecked(false);
   ui->areaTool->setChecked(false);
   ui->lineTool->setChecked(false);
   ui->selectionTool->setChecked(false);
   ui->paintAttr->setChecked(false);
   ui->boxTool->setChecked(true);

   // Haven't selected anything yet.
   m_selection = false;

   // Clipboard is not valid.
   m_clipboard = false;
}

void TileStampEditorForm::on_areaTool_clicked()
{
   m_activeTool = ui->areaTool;

   // Clear selected tile if any.
   ui->tileList->setCurrentIndex(QModelIndex());
   ui->paintTool->setChecked(false);
   ui->pencilTool->setChecked(false);
   ui->textTool->setChecked(false);
   ui->boxTool->setChecked(false);
   ui->lineTool->setChecked(false);
   ui->selectionTool->setChecked(false);
   ui->paintAttr->setChecked(false);
   ui->areaTool->setChecked(true);

   // Haven't selected anything yet.
   m_selection = false;

   // Clipboard is not valid.
   m_clipboard = false;
}

void TileStampEditorForm::on_lineTool_clicked()
{
   m_activeTool = ui->lineTool;

   // Clear selected tile if any.
   ui->tileList->setCurrentIndex(QModelIndex());
   ui->paintTool->setChecked(false);
   ui->pencilTool->setChecked(false);
   ui->textTool->setChecked(false);
   ui->boxTool->setChecked(false);
   ui->areaTool->setChecked(false);
   ui->selectionTool->setChecked(false);
   ui->paintAttr->setChecked(false);
   ui->lineTool->setChecked(true);

   // Haven't selected anything yet.
   m_selection = false;

   // Clipboard is not valid.
   m_clipboard = false;
}

void TileStampEditorForm::on_paintAttr_clicked()
{
   m_activeTool = ui->paintAttr;

   // Clear selected tile if any.
   ui->tileList->setCurrentIndex(QModelIndex());
   ui->paintTool->setChecked(false);
   ui->pencilTool->setChecked(false);
   ui->textTool->setChecked(false);
   ui->boxTool->setChecked(false);
   ui->areaTool->setChecked(false);
   ui->selectionTool->setChecked(false);
   ui->lineTool->setChecked(false);
   ui->paintAttr->setChecked(true);

   // Haven't selected anything yet.
   m_selection = false;

   // Clipboard is not valid.
   m_clipboard = false;
}

void TileStampEditorForm::on_tileList_activated(QModelIndex index)
{
   if ( ui->textTool->isChecked() )
   {
      m_activeTool = ui->textTool;
   }
   else if ( ui->paintTool->isChecked() )
   {
      m_activeTool = ui->paintTool;
   }
   else if ( ui->pencilTool->isChecked() )
   {
      m_activeTool = ui->pencilTool;
   }
   else if ( ui->boxTool->isChecked() )
   {
      m_activeTool = ui->boxTool;
   }
   else if ( ui->lineTool->isChecked() )
   {
      m_activeTool = ui->lineTool;
   }
   else if ( ui->selectionTool->isChecked() )
   {
      m_activeTool = ui->selectionTool;
   }
   else if ( ui->paintAttr->isChecked() )
   {
      m_activeTool = ui->paintAttr;
   }
   ui->textTool->setChecked(false);
   ui->paintTool->setChecked(false);
   ui->pencilTool->setChecked(false);
   ui->boxTool->setChecked(false);
   ui->areaTool->setChecked(false);
   ui->lineTool->setChecked(false);
   ui->selectionTool->setChecked(false);
   ui->paintAttr->setChecked(false);

   // Haven't selected anything yet.
   m_selection = false;

   // Clipboard is not valid.
   m_clipboard = false;
}

void TileStampEditorForm::on_tileList_clicked(QModelIndex index)
{
   if ( ui->textTool->isChecked() )
   {
      m_activeTool = ui->textTool;
   }
   else if ( ui->paintTool->isChecked() )
   {
      m_activeTool = ui->paintTool;
   }
   else if ( ui->pencilTool->isChecked() )
   {
      m_activeTool = ui->pencilTool;
   }
   else if ( ui->boxTool->isChecked() )
   {
      m_activeTool = ui->boxTool;
   }
   else if ( ui->lineTool->isChecked() )
   {
      m_activeTool = ui->lineTool;
   }
   else if ( ui->selectionTool->isChecked() )
   {
      m_activeTool = ui->selectionTool;
   }
   else if ( ui->paintAttr->isChecked() )
   {
      m_activeTool = ui->paintAttr;
   }
   ui->textTool->setChecked(false);
   ui->paintTool->setChecked(false);
   ui->pencilTool->setChecked(false);
   ui->boxTool->setChecked(false);
   ui->areaTool->setChecked(false);
   ui->lineTool->setChecked(false);
   ui->selectionTool->setChecked(false);
   ui->paintAttr->setChecked(false);

   // Haven't selected anything yet.
   m_selection = false;

   // Clipboard is not valid.
   m_clipboard = false;
}

void TileStampEditorForm::colorPicked(bool value)
{
   int idx;

   for ( idx = 0; idx < m_colors.count(); idx++ )
   {
      if ( sender() != m_colors.at(idx) )
      {
         m_colors.at(idx)->setChecked(false);
      }
      else
      {
         m_colors.at(idx)->setChecked(true);
      }
   }

   // Clear selected tile if any.
   ui->tileList->setCurrentIndex(QModelIndex());

   // Re-enable the last-selected tool.
   if ( m_activeTool )
   {
      m_activeTool->setChecked(true);
   }

   // Haven't selected anything yet.
   m_selection = false;

   // Clipboard is not valid.
   m_clipboard = false;
}

void TileStampEditorForm::on_cwRotate_clicked()
{
   int xSrc;
   int ySrc;
   int xDest;
   int yDest;
   int swap;

   // Use the overlay area to rotate.
   for ( ySrc = 0; ySrc < m_ySize; ySrc++ )
   {
      for ( xSrc = 0; xSrc < m_xSize; xSrc++ )
      {
         yDest = xSrc;
         xDest = m_ySize-ySrc-1;
         colorDataOverlay[(yDest*128)+xDest] = colorData[(ySrc*128)+xSrc];
      }
   }

   // Swap the overlay.
   copyOverlayToNormal();
   paintNormal();
   renderer->setBox();
   renderer->repaint();
   previewer->repaint();
   emit markProjectDirty(true);
   setModified(true);

   // Lastly, swap the x/y sizes.
   swap = ui->ySize->currentIndex();
   ui->ySize->blockSignals(true);
   ui->ySize->setCurrentIndex(ui->xSize->currentIndex());
   ui->ySize->blockSignals(false);
   ui->xSize->setCurrentIndex(swap);
}

void TileStampEditorForm::on_ccwRotate_clicked()
{
   int xSrc;
   int ySrc;
   int xDest;
   int yDest;
   int swap;

   // Use the overlay area to rotate.
   for ( ySrc = 0; ySrc < m_ySize; ySrc++ )
   {
      for ( xSrc = 0; xSrc < m_xSize; xSrc++ )
      {
         xDest = ySrc;
         yDest = m_xSize-xSrc-1;
         colorDataOverlay[(yDest*128)+xDest] = colorData[(ySrc*128)+xSrc];
      }
   }

   // Swap the overlay.
   copyOverlayToNormal();
   paintNormal();
   renderer->setBox();
   renderer->repaint();
   previewer->repaint();
   emit markProjectDirty(true);
   setModified(true);

   // Lastly, swap the x/y sizes.
   swap = ui->ySize->currentIndex();
   ui->ySize->blockSignals(true);
   ui->ySize->setCurrentIndex(ui->xSize->currentIndex());
   ui->ySize->blockSignals(false);
   ui->xSize->setCurrentIndex(swap);
}

void TileStampEditorForm::on_flipHorizontal_clicked()
{
   int xSrc;
   int ySrc;
   int xDest;
   int yDest;
   int swap;

   // Use the overlay area to rotate.
   for ( ySrc = 0; ySrc < m_ySize; ySrc++ )
   {
      for ( xSrc = 0; xSrc < m_xSize; xSrc++ )
      {
         xDest = m_xSize-xSrc-1;
         yDest = ySrc;
         colorDataOverlay[(yDest*128)+xDest] = colorData[(ySrc*128)+xSrc];
      }
   }

   // Swap the overlay.
   copyOverlayToNormal();
   paintNormal();
   renderer->setBox();
   renderer->repaint();
   previewer->repaint();
   emit markProjectDirty(true);
   setModified(true);
}

void TileStampEditorForm::on_flipVertical_clicked()
{
   int xSrc;
   int ySrc;
   int xDest;
   int yDest;
   int swap;

   // Use the overlay area to rotate.
   for ( ySrc = 0; ySrc < m_ySize; ySrc++ )
   {
      for ( xSrc = 0; xSrc < m_xSize; xSrc++ )
      {
         xDest = xSrc;
         yDest = m_ySize-ySrc-1;
         colorDataOverlay[(yDest*128)+xDest] = colorData[(ySrc*128)+xSrc];
      }
   }

   // Swap the overlay.
   copyOverlayToNormal();
   paintNormal();
   renderer->setBox();
   renderer->repaint();
   previewer->repaint();
   emit markProjectDirty(true);
   setModified(true);
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

void TileStampEditorForm::renderData()
{
}

void TileStampEditorForm::paintNormal()
{
   int idx;
   QColor color;

   for ( idx = 0; idx < 128*128*4; idx +=4 )
   {
      color = m_colors.at(colorData[idx>>2])->currentColor();
      imgData[idx+0] = color.blue();
      imgData[idx+1] = color.green();
      imgData[idx+2] = color.red();
      imgData[idx+3] = 0xFF;

      // Reset overlay.
      colorDataOverlay[idx>>2] = colorData[idx>>2];
   }
}

void TileStampEditorForm::paintOverlay(QByteArray overlayData,QByteArray overlayAttr,int overlayXSize,int overlayYSize,int boxX1,int boxY1,int boxX2,int boxY2)
{
   int idx;
   QColor color;
   int x;
   int y;
   int posX;
   int posY;
   int tile;
   int width = overlayXSize;
   int height = overlayYSize;
   int tileX;
   int tileY;
   int tileWidth;
   int tileHeight;
   int numTiles;
   char plane1;
   char plane2;
   char plane34;
   char pixel;
   int attrQuadsX;
   int attrQuadsY;
   int attrQuadX;
   int attrQuadY;
   int attrQuad;
   QList<QPoint> recolorPointQueue;
   QList<int> recolorColorQueue;

   tileWidth = width/8;
   tileHeight = height/8;
   numTiles = overlayData.count()/0x10;
   posX = boxX1;
   posY = boxY1;
   attrQuadsX = (tileWidth>>2);
   attrQuadsY = (tileHeight>>2);

   if ( attrQuadsX == 0 ) attrQuadsX = 1;
   if ( attrQuadsY == 0 ) attrQuadsY = 1;

   for ( idx = 0; idx < 128*128; idx++ )
   {
      // Reset overlay.
      colorDataOverlay[idx] = colorData[idx];
   }

   while ( (posY < boxY2) && (posX < boxX2) )
   {
      for ( tile = 0; tile < numTiles; tile++ )
      {
         tileX = (tile%tileWidth)*8;
         tileY = (tile/tileWidth)*8;

         if ( ((posY+tileY) < boxY2) && ((posX+tileX) < boxX2) )
         {
            for ( y = 0; y < 8; y++ )
            {
               // Get bitplanes from image data.
               plane1 = overlayData.at((tile<<4)+y);
               plane2 = overlayData.at((tile<<4)+y+8);

               // Get bitplanes from attribute data.
               attrQuadX = PIXEL_TO_ATTRQUAD(tileX);
               attrQuadY = PIXEL_TO_ATTRQUAD(tileY);
               attrQuad = (attrQuadY*attrQuadsX)+attrQuadX;
               if ( attrQuad < overlayAttr.count() )
               {
                  plane34 = overlayAttr.at(attrQuad);
               }
               else
               {
                  plane34 = 0x00;
               }
               plane34 >>= (PIXEL_TO_ATTRSECTION(tileX,tileY)<<1);
               plane34 &= 0x03;
               plane34 <<= 2;

               for ( x = 0; x < 8; x++ )
               {
                  pixel = (!!(plane1&0x80))
                        | ((!!(plane2&0x80))<<1)
                        | plane34;
                  colorDataOverlay[((posY+tileY+y)*128)+posX+tileX+x] = pixel;
                  plane1 <<= 1;
                  plane2 <<= 1;

                  // Recolor edge tiles if necessary.
                  if ( !recolorPointQueue.contains(QPoint(posX+tileX+x-((posX+tileX+x)%16),posY+tileY+y-((posY+tileY+y)%16))) )
                  {
                     recolorPointQueue.append(QPoint(posX+tileX+x-((posX+tileX+x)%16),posY+tileY+y-((posY+tileY+y)%16)));
                     recolorColorQueue.append(plane34/4);
                  }
               }
            }
         }
      }

      posX += width;
      if ( posX >= boxX2 )
      {
         posX = boxX1;
         posY += height;
      }
   }
   for ( idx = 0; idx < recolorPointQueue.count(); idx++ )
   {
      recolorTiles(recolorPointQueue.at(idx).x(),recolorPointQueue.at(idx).y(),recolorColorQueue.at(idx));
   }

   for ( idx = 0; idx < 128*128*4; idx +=4 )
   {
      color = m_colors.at(colorDataOverlay[idx>>2])->currentColor();
      imgData[idx+0] = color.blue();
      imgData[idx+1] = color.green();
      imgData[idx+2] = color.red();
      imgData[idx+3] = 0xFF;
   }
}

void TileStampEditorForm::paintOverlay(OverlayType type,int selectedColor,int boxX1,int boxY1,int boxX2,int boxY2)
{
   int idxx;
   int idxy;
   int idxcx;
   int idxcy;
   int idx;
   QColor color;
   int swap;
   int dY;
   int dX;
   int sX;
   int sY;
   int err;
   int err2;
   QList<QPoint> fillQueue;
   QList<QPoint> recolorQueue;
   QPoint point;
   int targetColor;

   for ( idx = 0; idx < 128*128; idx++ )
   {
      // Reset overlay.
      colorDataOverlay[idx] = colorData[idx];
   }

   switch ( type )
   {
   case Overlay_Line:
      // Bresenham's line drawing algorithm
      dX = abs((boxX2-1)-boxX1);
      dY = abs((boxY2-1)-boxY1);
      if ( boxX1 < boxX2 )
      {
         sX = 1;
      }
      else
      {
         sX = -1;
      }
      if ( boxY1 < boxY2 )
      {
         sY = 1;
      }
      else
      {
         sY = -1;
      }
      err = dX-dY;

      while ( 1 )
      {
         colorDataOverlay[(boxY1*128)+boxX1] = selectedColor;
         if ( !recolorQueue.contains(QPoint(boxX1-(boxX1%16),boxY1-(boxY1%16))) )
         {
            recolorQueue.append(QPoint(boxX1-(boxX1%16),boxY1-(boxY1%16)));
         }
         if ( (boxX1 == (boxX2-1)) &&
              (boxY1 == (boxY2-1)) )
         {
            break;
         }
         err2 = 2*err;
         if ( err2 > -dY )
         {
            err = err - dY;
            boxX1 = boxX1 + sX;
         }
         if ( err2 < dX )
         {
            err = err + dX;
            boxY1 = boxY1 + sY;
         }
      }
      foreach ( QPoint point, recolorQueue )
      {
         recolorTiles(point.x(),point.y(),selectedColor/4);
      }
      break;
   case Overlay_FloodFill:
//Flood-fill (node, target-color, replacement-color):
      targetColor = colorDataOverlay[(boxY1*128)+boxX1];
//1. Set Q to the empty queue.
//2. If the color of node is not equal to target-color, return.
//3. Add node to the end of Q.
      fillQueue.append(QPoint(boxX1,boxY1));
//4. While Q is not empty:
      while ( !fillQueue.isEmpty() )
      {
//5.     Set n equal to the first element of Q
         point = fillQueue.takeFirst();
//6.     If the color of n is equal to target-color, set the color of n to replacement-color.
         colorDataOverlay[(point.y()*128)+point.x()] = selectedColor;
         if ( !recolorQueue.contains(QPoint(point.x()-(point.x()%16),point.y()-(point.y()%16))) )
         {
            recolorQueue.append(QPoint(point.x()-(point.x()%16),point.y()-(point.y()%16)));
         }
//7.     Remove first element from Q
//8.     If the color of the node to the west of n is target-color:
         if ( ((point.x()-1) >= 0) &&
              (colorDataOverlay[(point.y()*128)+point.x()-1] == targetColor) )
         {
//9.         Add that node to the end of Q
            fillQueue.append(QPoint(point.x()-1,point.y()));
            colorDataOverlay[(point.y()*128)+point.x()-1] = selectedColor;
            if ( !recolorQueue.contains(QPoint((point.x()-1)-((point.x()-1)%16),point.y()-(point.y()%16))) )
            {
               recolorQueue.append(QPoint((point.x()-1)-((point.x()-1)%16),point.y()-(point.y()%16)));
            }
         }
//10.     If the color of the node to the east of n is target-color:
         if ( ((point.x()+1) < m_xSize) &&
              (colorDataOverlay[(point.y()*128)+point.x()+1] == targetColor) )
         {
//11.         Add that node to the end of Q
            fillQueue.append(QPoint(point.x()+1,point.y()));
            colorDataOverlay[(point.y()*128)+point.x()+1] = selectedColor;
            if ( !recolorQueue.contains(QPoint((point.x()+1)-((point.x()+1)%16),point.y()-(point.y()%16))) )
            {
               recolorQueue.append(QPoint((point.x()+1)-((point.x()+1)%16),point.y()-(point.y()%16)));
            }
         }
//12.     If the color of the node to the north of n is target-color:
         if ( ((point.y()-1) >= 0) &&
              (colorDataOverlay[((point.y()-1)*128)+point.x()] == targetColor) )
         {
//13.         Add that node to the end of Q
            fillQueue.append(QPoint(point.x(),point.y()-1));
            colorDataOverlay[((point.y()-1)*128)+point.x()] = selectedColor;
            if ( !recolorQueue.contains(QPoint(point.x()-(point.x()%16),(point.y()-1)-((point.y()-1)%16))) )
            {
               recolorQueue.append(QPoint(point.x()-(point.x()%16),(point.y()-1)-((point.y()-1)%16)));
            }
         }
//14.     If the color of the node to the south of n is target-color:
         if ( ((point.y()+1) < m_ySize) &&
              (colorDataOverlay[((point.y()+1)*128)+point.x()] == targetColor) )
         {
//15.         Add that node to the end of Q
            fillQueue.append(QPoint(point.x(),point.y()+1));
            colorDataOverlay[((point.y()+1)*128)+point.x()] = selectedColor;
            if ( !recolorQueue.contains(QPoint(point.x()-(point.x()%16),(point.y()+1)-((point.y()+1)%16))) )
            {
               recolorQueue.append(QPoint(point.x()-(point.x()%16),(point.y()+1)-((point.y()+1)%16)));
            }
         }
      }
      //16. Return.
      foreach ( QPoint point, recolorQueue )
      {
         recolorTiles(point.x(),point.y(),selectedColor/4);
      }
      break;
   case Overlay_HollowBox:
      // Normalize...
      if ( boxX1 > boxX2 )
      {
         swap = boxX1;
         boxX1 = boxX2;
         boxX2 = swap;
      }
      if ( boxY1 > boxY2 )
      {
         swap = boxY1;
         boxY1 = boxY2;
         boxY2 = swap;
      }
      // Draw a hollow box...
      // Draw the horizontals.
      for ( idx = boxX1; idx < boxX2; idx++ )
      {
         colorDataOverlay[(boxY1*128)+idx] = selectedColor;
         if ( !recolorQueue.contains(QPoint(idx-(idx%16),boxY1-(boxY1%16))) )
         {
            recolorQueue.append(QPoint(idx-(idx%16),boxY1-(boxY1%16)));
         }
         colorDataOverlay[((boxY2-1)*128)+idx] = selectedColor;
         if ( !recolorQueue.contains(QPoint(idx-(idx%16),(boxY2-1)-((boxY2-1)%16))) )
         {
            recolorQueue.append(QPoint(idx-(idx%16),(boxY2-1)-((boxY2-1)%16)));
         }
      }
      // Draw the verticals.
      for ( idx = boxY1; idx < boxY2; idx++ )
      {
         colorDataOverlay[(idx*128)+boxX1] = selectedColor;
         if ( !recolorQueue.contains(QPoint(boxX1-(boxX1%16),idx-(idx%16))) )
         {
            recolorQueue.append(QPoint(boxX1-(boxX1%16),idx-(idx%16)));
         }
         colorDataOverlay[(idx*128)+(boxX2-1)] = selectedColor;
         if ( !recolorQueue.contains(QPoint((boxX2-1)-((boxX2-1)%16),idx-(idx%16))) )
         {
            recolorQueue.append(QPoint((boxX2-1)-((boxX2-1)%16),idx-(idx%16)));
         }
      }
      foreach ( QPoint point, recolorQueue )
      {
         recolorTiles(point.x(),point.y(),selectedColor/4);
      }
      break;
   case Overlay_SolidBox:
      // Normalize...
      if ( boxX1 > boxX2 )
      {
         swap = boxX1;
         boxX1 = boxX2;
         boxX2 = swap;
      }
      if ( boxY1 > boxY2 )
      {
         swap = boxY1;
         boxY1 = boxY2;
         boxY2 = swap;
      }
      // Draw a hollow box...
      // Draw the horizontals.
      for ( idx = boxX1; idx < boxX2; idx++ )
      {
         colorDataOverlay[(boxY1*128)+idx] = selectedColor;
         if ( !recolorQueue.contains(QPoint(idx-(idx%16),boxY1-(boxY1%16))) )
         {
            recolorQueue.append(QPoint(idx-(idx%16),boxY1-(boxY1%16)));
         }
         colorDataOverlay[((boxY2-1)*128)+idx] = selectedColor;
         if ( !recolorQueue.contains(QPoint(idx-(idx%16),(boxY2-1)-((boxY2-1)%16))) )
         {
            recolorQueue.append(QPoint(idx-(idx%16),(boxY2-1)-((boxY2-1)%16)));
         }
      }
      // Draw the verticals.
      for ( idx = boxY1; idx < boxY2; idx++ )
      {
         colorDataOverlay[(idx*128)+boxX1] = selectedColor;
         if ( !recolorQueue.contains(QPoint(boxX1-(boxX1%16),idx-(idx%16))) )
         {
            recolorQueue.append(QPoint(boxX1-(boxX1%16),idx-(idx%16)));
         }
         colorDataOverlay[(idx*128)+(boxX2-1)] = selectedColor;
         if ( !recolorQueue.contains(QPoint((boxX2-1)-((boxX2-1)%16),idx-(idx%16))) )
         {
            recolorQueue.append(QPoint((boxX2-1)-((boxX2-1)%16),idx-(idx%16)));
         }
      }
      // Draw a solid box...
      for ( idxy = boxY1+1; idxy < boxY2-1; idxy++ )
      {
         for ( idxx = boxX1+1; idxx < boxX2-1; idxx++ )
         {
            colorDataOverlay[(idxy*128)+idxx] = selectedColor;
         }
      }
      foreach ( QPoint point, recolorQueue )
      {
         recolorTiles(point.x(),point.y(),selectedColor/4);
      }
      break;
   case Overlay_Erase:
      // Normalize...
      if ( boxX1 > boxX2 )
      {
         swap = boxX1;
         boxX1 = boxX2;
         boxX2 = swap;
      }
      if ( boxY1 > boxY2 )
      {
         swap = boxY1;
         boxY1 = boxY2;
         boxY2 = swap;
      }
      // Draw a solid box...
      for ( idxy = boxY1; idxy < boxY2; idxy++ )
      {
         for ( idxx = boxX1; idxx < boxX2; idxx++ )
         {
            colorDataOverlay[(idxy*128)+idxx] = colorDataOverlay[(idxy*128)+idxx]&0xFC;
         }
      }
      break;
   case Overlay_PasteClipboard:
      // Normalize...
      if ( boxX1 > boxX2 )
      {
         swap = boxX1;
         boxX1 = boxX2;
         boxX2 = swap;
      }
      if ( boxY1 > boxY2 )
      {
         swap = boxY1;
         boxY1 = boxY2;
         boxY2 = swap;
      }
      // Draw the clipboard...
      for ( idxy = boxY1, idxcy = 0; idxy < boxY2; idxy++, idxcy++ )
      {
         for ( idxx = boxX1, idxcx = 0; idxx < boxX2; idxx++, idxcx++ )
         {
            if ( (idxx >= 0) &&
                 (idxy >= 0) &&
                 (idxx < m_xSize) &&
                 (idxy < m_ySize) )
            {
               colorDataOverlay[(idxy*128)+idxx] = colorDataClipboard[(idxcy*128)+idxcx];
            }
         }
      }
      break;
   }

   for ( idx = 0; idx < 128*128*4; idx +=4 )
   {
      color = m_colors.at(colorDataOverlay[idx>>2])->currentColor();
      imgData[idx+0] = color.blue();
      imgData[idx+1] = color.green();
      imgData[idx+2] = color.red();
      imgData[idx+3] = 0xFF;
   }
}

void TileStampEditorForm::paintOverlay(int selectedColor,int pixx,int pixy)
{
   QColor color;
   int idx;

   colorDataOverlay[(pixy*128)+pixx] = selectedColor;
   recolorTiles(pixx,pixy,selectedColor/4);

   for ( idx = 0; idx < 128*128*4; idx +=4 )
   {
      color = m_colors.at(colorDataOverlay[idx>>2])->currentColor();
      imgData[idx+0] = color.blue();
      imgData[idx+1] = color.green();
      imgData[idx+2] = color.red();
      imgData[idx+3] = 0xFF;
   }
}

void TileStampEditorForm::copyOverlayToNormal()
{
   int idx;
   QColor color;

   for ( idx = 0; idx < 128*128; idx++ )
   {
      colorData[idx] = colorDataOverlay[idx];
   }
}

void TileStampEditorForm::copyNormalToClipboard(int boxX1,int boxY1,int boxX2,int boxY2)
{
   int idxx;
   int idxy;
   int idxcx;
   int idxcy;
   QColor color;

   for ( idxy = boxY1, idxcy = 0; idxy < boxY2; idxy++, idxcy++ )
   {
      for ( idxx = boxX1, idxcx = 0; idxx < boxX2; idxx++, idxcx++ )
      {
         colorDataClipboard[(idxcy*128)+idxcx] = colorData[(idxy*128)+idxx];
      }
   }

   // Clipboard is valid.
   m_clipboard = true;
}

void TileStampEditorForm::copyClipboardToOverlay()
{
   int idx;
   QColor color;

   for ( idx = 0; idx < 128*128; idx++ )
   {
      colorDataOverlay[idx] = colorDataClipboard[idx];
   }
}

void TileStampEditorForm::recolorClipboard(int boxX1, int boxY1, int boxX2, int boxY2)
{
   int idxx;
   int idxy;
   int idxcx;
   int idxcy;

   for ( idxy = boxY1, idxcy = 0; idxy < boxY2; idxy++, idxcy++ )
   {
      for ( idxx = boxX1, idxcx = 0; idxx < boxX2; idxx++, idxcx++ )
      {
         colorDataClipboard[(idxcy*128)+idxcx] &= 0x03;
         colorDataClipboard[(idxcy*128)+idxcx] |= (colorDataOverlay[(idxy*128)+idxx]&0xFC);
      }
   }
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
            colorDataOverlay[(idxy*128)+idxx] &= 0x3;
            colorDataOverlay[(idxy*128)+idxx] |= (newColorTable<<2);
         }
      }
   }

   // Re-color image.
   for ( idx = 0; idx < 128*128*4; idx += 4 )
   {
      color = m_colors.at(colorDataOverlay[idx>>2])->currentColor();
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
      if ( pChrItem )
      {
         tileList.append(pChrItem);
      }

      // Move through tree.
      iter.next();
   }
   tileListModel->setBankItems(tileList);
   tileListModel->update();
}

void TileStampEditorForm::initializeTile(QByteArray tileData,QByteArray attrData)
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
   char plane34;
   char pixel;
   int attrQuadsX = (tileWidth>>2);
   int attrQuadsY = (tileHeight>>2);
   int attrQuadX;
   int attrQuadY;
   int attrQuad;

   if ( attrQuadsX == 0 ) attrQuadsX = 1;
   if ( attrQuadsY == 0 ) attrQuadsY = 1;

   for ( tile = 0; tile < numTiles; tile++ )
   {
      tileX = (tile%tileWidth)*8;
      tileY = (tile/tileWidth)*8;

      for ( y = 0; y < 8; y++ )
      {
         // Get bitplanes from image data.
         plane1 = tileData.at((tile<<4)+y);
         plane2 = tileData.at((tile<<4)+y+8);

         // Get bitplanes from attribute data.
         attrQuadX = PIXEL_TO_ATTRQUAD(tileX);
         attrQuadY = PIXEL_TO_ATTRQUAD(tileY);
         attrQuad = (attrQuadY*attrQuadsX)+attrQuadX;
         if ( attrQuad < attrData.count() )
         {
            plane34 = attrData.at(attrQuad);
         }
         else
         {
            plane34 = 0x00;
         }
         plane34 >>= (PIXEL_TO_ATTRSECTION(tileX,tileY)<<1);
         plane34 &= 0x03;
         plane34 <<= 2;

         for ( x = 0; x < 8; x++ )
         {
            pixel = (!!(plane1&0x80))
                  | ((!!(plane2&0x80))<<1)
                  | (plane34);
            colorData[((tileY+y)*128)+tileX+x] = pixel;
            colorDataOverlay[((tileY+y)*128)+tileX+x] = pixel;
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
   int tile;
   int tileWidth = m_xSize/8;
   int tileHeight = m_ySize/8;
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

QByteArray TileStampEditorForm::attributeData()
{
   QByteArray attrOut;
   unsigned char attrTable [ 256 ];
   int tileWidth = m_xSize/8;
   int tileHeight = m_ySize/8;
   int tileX;
   int tileY;
   int idx;
   int attrQuadsX = (tileWidth>>2);
   int attrQuadsY = (tileHeight>>2);
   int attrQuads;
   int atx;
   int aty;

   if ( attrQuadsX == 0 ) attrQuadsX = 1;
   if ( attrQuadsY == 0 ) attrQuadsY = 1;
   attrQuads = attrQuadsX*attrQuadsY;

   for ( idx = 0; idx < 256; idx++ )
   {
      attrTable [ idx ] = 0x00;
   }

   for ( tileY = 0; tileY < tileHeight; tileY++ )
   {
      for ( tileX = 0; tileX < tileWidth; tileX++ )
      {
         // find palette in attribute table...only do this for 2x2 8x8's...
         if ( ((tileX&0x1) == 0) && ((tileY&0x1) == 0) )
         {
            attrTable [ ((tileY>>1)*(tileWidth>>1))+(tileX>>1) ] = colorData[(tileY*8*128)+(tileX*8)]>>2;
         }
      }
   }

   aty = 0;
   atx = 0;
   for ( idx = 0; idx < attrQuads; idx++ )
   {
      // condense attribute table...
      attrTable [ idx ] = attrTable[(aty*(tileWidth>>1))+atx];
      attrTable [ idx ] |= (attrTable[(aty*(tileWidth>>1))+atx+1]<<2);
      attrTable [ idx ] |= (attrTable[(aty*(tileWidth>>1))+atx+(tileWidth>>1)]<<4);
      attrTable [ idx ] |= (attrTable[(aty*(tileWidth>>1))+atx+(tileWidth>>1)+1]<<6);

      atx += 2;
      if ( atx == (tileWidth>>1) )
      {
         atx = 0;
         aty += 2;
      }
   }
   attrOut.clear();
   attrOut.append((char*)attrTable,attrQuads);

   return attrOut;
}

void TileStampEditorForm::selectionTool(QMouseEvent *event)
{
   int pixx;
   int pixy;
   int idx;
   int boxX1;
   int boxY1;
   int boxX2;
   int boxY2;

   renderer->pointToPixel(event->pos().x(),event->pos().y(),&pixx,&pixy);

   // Where's the box in the renderer?
   renderer->getBox(&boxX1,&boxY1,&boxX2,&boxY2);

   switch ( event->type() )
   {
   case QEvent::MouseButtonPress:
      if ( !m_selectionRect.contains(pixx,pixy) )
      {
         if ( m_selection && m_clipboard )
         {
            m_selectionRect = QRect(QPoint(boxX1,boxY1),QPoint(boxX2,boxY2));
            m_selectionRect = m_selectionRect.normalized();
            paintOverlay(Overlay_PasteClipboard,0,m_selectionRect.left(),m_selectionRect.top(),m_selectionRect.right(),m_selectionRect.bottom());
            copyOverlayToNormal();
            renderer->repaint();
            previewer->repaint();
            emit markProjectDirty(true);
            setModified(true);
         }

         m_selection = false;
         m_clipboard = false;

         m_anchor = QPoint(pixx,pixy);
         renderer->setBox(m_anchor.x(),m_anchor.y(),pixx+1,pixy+1);
         renderer->repaint();
      }
      else
      {
         m_anchor = QPoint(pixx,pixy);
         if ( (m_selection) &&
              (!m_clipboard) )
         {
            copyNormalToClipboard(m_selectionRect.left(),m_selectionRect.top(),m_selectionRect.right(),m_selectionRect.bottom());
            paintOverlay(Overlay_Erase,0,m_selectionRect.left(),m_selectionRect.top(),m_selectionRect.right(),m_selectionRect.bottom());
            copyOverlayToNormal();
            renderer->repaint();
            previewer->repaint();
            emit markProjectDirty(true);
            setModified(true);
         }
      }
      break;
   case QEvent::MouseMove:
      if ( event->buttons() == Qt::LeftButton )
      {
         if ( m_selection && m_clipboard )
         {
            m_selectionRect = m_selectionRect.translated(pixx-m_anchor.x(),pixy-m_anchor.y());
            m_selectionRect = m_selectionRect.normalized();
            m_anchor = QPoint(pixx,pixy);
            renderer->setBox(m_selectionRect.left(),m_selectionRect.top(),m_selectionRect.right(),m_selectionRect.bottom());
            recolorClipboard(m_selectionRect.left(),m_selectionRect.top(),m_selectionRect.right(),m_selectionRect.bottom());
            paintOverlay(Overlay_PasteClipboard,0,m_selectionRect.left(),m_selectionRect.top(),m_selectionRect.right(),m_selectionRect.bottom());
            renderer->repaint();
            previewer->repaint();
            emit markProjectDirty(true);
            setModified(true);
         }
         else
         {
            renderer->setBox(m_anchor.x(),m_anchor.y(),pixx+1,pixy+1);
            renderer->repaint();
            previewer->repaint();
         }
      }
      break;
   case QEvent::MouseButtonRelease:
      if ( (!m_selection) &&
           (boxX1 >= 0) &&
           (boxY1 >= 0) &&
           (boxX2 >= 0) &&
           (boxY2 >= 0) )
      {
         m_selection = true;
      }
      if ( m_selection )
      {
         m_selectionRect = QRect(QPoint(boxX1,boxY1),QPoint(boxX2,boxY2));
         m_selectionRect = m_selectionRect.normalized();
      }
      else
      {
         m_selectionRect = QRect(-1,-1,-1,-1);
         m_clipboard = false;
      }
      renderer->repaint();
      previewer->repaint();
      break;
   default:
      // Stupid warnings.
      break;
   }
}

void TileStampEditorForm::paintTool(QMouseEvent *event)
{
   int idx;
   int pixx;
   int pixy;
   int selectedColor = -1;

   renderer->pointToPixel(event->pos().x(),event->pos().y(),&pixx,&pixy);

   // Find the selected color.
   for ( idx = 0; idx < m_colors.count(); idx++ )
   {
      if ( m_colors.at(idx)->isChecked() )
      {
         selectedColor = idx;
      }
   }

   if ( selectedColor != -1 )
   {
      switch ( event->type() )
      {
      case QEvent::MouseButtonPress:
         paintOverlay(Overlay_FloodFill,selectedColor,pixx,pixy,0,0);
         renderer->repaint();
         previewer->repaint();
         break;
      case QEvent::MouseMove:
         break;
      case QEvent::MouseButtonRelease:
         copyOverlayToNormal();
         paintNormal();
         renderer->repaint();
         previewer->repaint();
         emit markProjectDirty(true);
         setModified(true);
         break;
      default:
         // Stupid warnings.
         break;
      }
   }
}

void TileStampEditorForm::pencilTool(QMouseEvent *event)
{
   int pixx;
   int pixy;
   int boxX1;
   int boxY1;
   int boxX2;
   int boxY2;
   int idx;

   renderer->pointToPixel(event->pos().x(),event->pos().y(),&pixx,&pixy);

   boxX1 = pixx-(pixx%16);
   boxY1 = pixy-(pixy%16);
   boxX2 = boxX1+16;
   boxY2 = boxY1+16;
   renderer->setBox(boxX1,boxY1,boxX2,boxY2);
   renderer->repaint();

   switch ( event->type() )
   {
   case QEvent::MouseButtonPress:
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
                  if ( (idx%4) && (colorData[(pixy*128)+pixx] != idx) )
                  {
                     recolorTiles(pixx,pixy,idx/4);
                  }
                  paintOverlay(idx,pixx,pixy);
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
         paintOverlay(0,pixx,pixy);
         renderer->repaint();
         previewer->repaint();
         emit markProjectDirty(true);
         setModified(true);
      }
      break;
   case QEvent::MouseMove:
      boxX1 = pixx-(pixx%16);
      boxY1 = pixy-(pixy%16);
      boxX2 = boxX1+16;
      boxY2 = boxY1+16;
      renderer->setBox(boxX1,boxY1,boxX2,boxY2);
      renderer->repaint();

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
                  if ( (idx%4) && (colorData[(pixy*128)+pixx] != idx) )
                  {
                     recolorTiles(pixx,pixy,idx/4);
                  }
                  paintOverlay(idx,pixx,pixy);
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
         paintOverlay(0,pixx,pixy);
         renderer->repaint();
         previewer->repaint();
         emit markProjectDirty(true);
         setModified(true);
      }
      break;
   case QEvent::MouseButtonRelease:
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
                  if ( (idx%4) && (colorData[(pixy*128)+pixx] != idx) )
                  {
                     recolorTiles(pixx,pixy,idx/4);
                  }
                  paintOverlay(idx,pixx,pixy);
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
         paintOverlay(0,pixx,pixy);
         renderer->repaint();
         previewer->repaint();
         emit markProjectDirty(true);
         setModified(true);
      }

      copyOverlayToNormal();

      paintNormal();
      renderer->setBox();
      renderer->repaint();
      previewer->repaint();
      emit markProjectDirty(true);
      setModified(true);
      break;
   default:
      // Stupid warnings.
      break;
   }
}

void TileStampEditorForm::filledBoxTool(QMouseEvent *event)
{
   boxTool(event,true);
}

void TileStampEditorForm::hollowBoxTool(QMouseEvent *event)
{
   boxTool(event,false);
}

void TileStampEditorForm::boxTool(QMouseEvent *event,bool filled)
{
   int selectedColor;
   int pixx;
   int pixy;
   int idx;
   OverlayType overlayType;

   // Filled or hollow?
   if ( filled )
   {
      overlayType = Overlay_SolidBox;
   }
   else
   {
      overlayType = Overlay_HollowBox;
   }

   renderer->pointToPixel(event->pos().x(),event->pos().y(),&pixx,&pixy);

   // Find the selected color.
   for ( idx = 0; idx < m_colors.count(); idx++ )
   {
      if ( m_colors.at(idx)->isChecked() )
      {
         selectedColor = idx;
         break;
      }
   }

   if ( selectedColor >= 0 )
   {
      switch ( event->type() )
      {
      case QEvent::MouseButtonPress:
         m_anchor = QPoint(pixx,pixy);
         paintOverlay(overlayType,selectedColor,m_anchor.x(),m_anchor.y(),pixx+1,pixy+1);
         renderer->setBox(m_anchor.x(),m_anchor.y(),pixx+1,pixy+1);
         renderer->repaint();
         previewer->repaint();
         break;
      case QEvent::MouseMove:
         if ( event->buttons() == Qt::LeftButton )
         {
            paintOverlay(overlayType,selectedColor,m_anchor.x(),m_anchor.y(),pixx+1,pixy+1);
            renderer->setBox(m_anchor.x(),m_anchor.y(),pixx+1,pixy+1);
            renderer->repaint();
            previewer->repaint();
         }
         break;
      case QEvent::MouseButtonRelease:
         copyOverlayToNormal();
         paintNormal();
         renderer->setBox();
         renderer->repaint();
         previewer->repaint();
         emit markProjectDirty(true);
         setModified(true);
         break;
      default:
         // Stupid warnings.
         break;
      }
   }
}

void TileStampEditorForm::lineTool(QMouseEvent *event)
{
   int selectedColor;
   int pixx;
   int pixy;
   int idx;

   renderer->pointToPixel(event->pos().x(),event->pos().y(),&pixx,&pixy);

   // Find the selected color.
   for ( idx = 0; idx < m_colors.count(); idx++ )
   {
      if ( m_colors.at(idx)->isChecked() )
      {
         selectedColor = idx;
      }
   }

   if ( selectedColor >= 0 )
   {
      switch ( event->type() )
      {
      case QEvent::MouseButtonPress:
         // Set anchor.
         m_anchor = QPoint(pixx,pixy);
         paintOverlay(Overlay_Line,selectedColor,m_anchor.x(),m_anchor.y(),pixx+1,pixy+1);
         renderer->repaint();
         previewer->repaint();
         break;
      case QEvent::MouseMove:
         if ( event->buttons() == Qt::LeftButton )
         {
            paintOverlay(Overlay_Line,selectedColor,m_anchor.x(),m_anchor.y(),pixx+1,pixy+1);
            renderer->repaint();
            previewer->repaint();
         }
         else
         {
            renderer->setBox();
            renderer->repaint();
         }
         break;
      case QEvent::MouseButtonRelease:
         copyOverlayToNormal();

         paintNormal();
         renderer->repaint();
         previewer->repaint();
         emit markProjectDirty(true);
         setModified(true);
         break;
      default:
         // Stupid warnings.
         break;
      }
   }
}

void TileStampEditorForm::textTool(QMouseEvent *event)
{
}

void TileStampEditorForm::tileTool(QMouseEvent *event)
{
   QColor color;
   int pixx;
   int pixy;
   int boxX1;
   int boxY1;
   int boxX2;
   int boxY2;
   IChrRomBankItem* pChrItem = reinterpret_cast<IChrRomBankItem*>(ui->tileList->currentIndex().internalPointer());
   CTileStamp* pSelectedTile = dynamic_cast<CTileStamp*>(pChrItem);
   CBinaryFile* pSelectedBinary = dynamic_cast<CBinaryFile*>(pChrItem);
   int xSize;
   int ySize;

   renderer->pointToPixel(event->pos().x(),event->pos().y(),&pixx,&pixy);

   switch ( event->type() )
   {
   case QEvent::MouseButtonPress:
      if ( pSelectedTile )
      {
         pSelectedTile->getSize(&xSize,&ySize);
         boxX1 = pixx-(pixx%8);
         boxY1 = pixy-(pixy%8);
         if ( (boxX1+xSize) < m_xSize )
         {
            boxX2 = boxX1+xSize;
         }
         else
         {
            boxX2 = m_xSize;
         }
         if ( (boxY1+ySize) < m_ySize )
         {
            boxY2 = boxY1+ySize;
         }
         else
         {
            boxY2 = m_ySize;
         }
         paintOverlay(pSelectedTile->getTileData(),pSelectedTile->getAttributeData(),xSize,ySize,boxX1,boxY1,boxX2,boxY2);
         renderer->setBox(boxX1,boxY1,boxX2,boxY2);
         renderer->repaint();
         previewer->repaint();
      }
      else if ( pSelectedBinary )
      {
         pSelectedBinary->getSize(&xSize,&ySize);
         boxX1 = pixx-(pixx%8);
         boxY1 = pixy-(pixy%8);
         if ( (boxX1+xSize) < m_xSize )
         {
            boxX2 = boxX1+xSize;
         }
         else
         {
            boxX2 = m_xSize;
         }
         if ( (boxY1+ySize) < m_ySize )
         {
            boxY2 = boxY1+ySize;
         }
         else
         {
            boxY2 = m_ySize;
         }
         paintOverlay(pSelectedBinary->getBinaryData(),QByteArray(),xSize,ySize,boxX1,boxY1,boxX2,boxY2);
         renderer->setBox(boxX1,boxY1,boxX2,boxY2);
         renderer->repaint();
         previewer->repaint();
      }
      break;
   case QEvent::MouseMove:
      if ( event->buttons() == Qt::LeftButton )
      {
         if ( pSelectedTile )
         {
            pSelectedTile->getSize(&xSize,&ySize);
            renderer->getBox(&boxX1,&boxY1,&boxX2,&boxY2);
            boxX2 = pixx+(8-(pixx%8));
            boxY2 = pixy+(8-(pixy%8));
            if ( (boxY2 > boxY1) && (boxX2 > boxX1) )
            {
               paintOverlay(pSelectedTile->getTileData(),pSelectedTile->getAttributeData(),xSize,ySize,boxX1,boxY1,boxX2,boxY2);
               renderer->setBox(boxX1,boxY1,boxX2,boxY2);
               renderer->repaint();
               previewer->repaint();
            }
         }
         else if ( pSelectedBinary )
         {
            pSelectedBinary->getSize(&xSize,&ySize);
            renderer->getBox(&boxX1,&boxY1,&boxX2,&boxY2);
            boxX2 = pixx+(8-(pixx%8));
            boxY2 = pixy+(8-(pixy%8));
            if ( (boxY2 > boxY1) && (boxX2 > boxX1) )
            {
               paintOverlay(pSelectedBinary->getBinaryData(),QByteArray(),xSize,ySize,boxX1,boxY1,boxX2,boxY2);
               renderer->setBox(boxX1,boxY1,boxX2,boxY2);
               renderer->repaint();
               previewer->repaint();
            }
         }
      }
      else
      {
         renderer->setBox();
         renderer->repaint();
      }
      break;
   case QEvent::MouseButtonRelease:
      copyOverlayToNormal();

      paintNormal();
      renderer->setBox();
      renderer->repaint();
      previewer->repaint();
      emit markProjectDirty(true);
      setModified(true);
      break;
   default:
      // Stupid warnings.
      break;
   }
}

void TileStampEditorForm::updateInfoText(int x,int y)
{
   const char* attrSection[4] = { "Upper Left", "Upper Right", "Lower Left", "Lower Right" };

   if ( (x >= 0) && (y >= 0) )
   {
      QString str;
      str.sprintf("Pixel(%d,%d) Tile(%d,%d) AttrQuad(%d,%d) AttrSeq(%s)",
                  x,y,
                  PIXEL_TO_TILE(x),PIXEL_TO_TILE(y),
                  PIXEL_TO_ATTRQUAD(x),PIXEL_TO_ATTRQUAD(y),
                  attrSection[PIXEL_TO_ATTRSECTION(x,y)]);
      ui->info->setText(str);
   }
   else
   {
      ui->info->clear();
   }
}
