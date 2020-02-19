#include "tilestampeditorform.h"
#include "ui_tilestampeditorform.h"

#include "cnessystempalette.h"

#include "cattributetable.h"
#include "cdesignercommon.h"
#include "cimageconverters.h"

#include "main.h"

#include <QUndoView>
#include <QClipboard>

TileStampEditorForm::TileStampEditorForm(QByteArray data,QByteArray attr,QString attrTblUUID,QList<PropertyItem> tileProperties,int xSize,int ySize,bool grid,IProjectTreeViewItem* link,QWidget *parent) :
    CDesignerEditorBase(link,parent),
    ui(new Ui::TileStampEditorForm)
{
   IProjectTreeViewItemIterator iter;
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
   m_selectionCaptured = false;

   // Not currently resizing the selection rectangle.
   m_resizeMode = Resize_None;

   // Set up default color selected.
   ui->pal0col1->setChecked(true);

   QList<IChrRomBankItem*> tileListChrItem;
   iter.reset(nesicideProject->getProject());
   while ( iter.current() )
   {
      IChrRomBankItem* pChrItem = dynamic_cast<IChrRomBankItem*>(iter.current());
      IProjectTreeViewItem* pProjItem = dynamic_cast<IProjectTreeViewItem*>(iter.current());
      if ( pChrItem &&
           (pProjItem != this->treeLink()) )
      {
         tileListChrItem.append(pChrItem);
      }

      // Move through tree.
      iter.next();
   }

   ui->tileTabWidget->setItems(tileListChrItem);
   QObject::connect(ui->tileTabWidget,SIGNAL(snapToTab(QString)),this,SIGNAL(snapToTab(QString)));
   QObject::connect(ui->tileTabWidget,SIGNAL(tileSelected(QModelIndex)),this,SLOT(tileSelected(QModelIndex)));

   tilePropertyValueDelegate = new CPropertyValueDelegate();
   ui->propertyTableView->setItemDelegateForColumn(PropertyCol_Value,tilePropertyValueDelegate);

   tilePropertyListModel = new CPropertyListModel(true);
   m_tileProperties = nesicideProject->getTileProperties();

   // Update local tile properties from saved tile info.
   idx = 0;
   foreach ( PropertyItem globalItem, m_tileProperties )
   {
      foreach ( PropertyItem localItem, tileProperties )
      {
         if ( globalItem.name == localItem.name )
         {
            globalItem.type = localItem.type;
            globalItem.value = localItem.value;
            m_tileProperties.replace(idx,globalItem);
         }
      }
      idx++;
   }

   tilePropertyListModel->setItems(m_tileProperties);

   ui->propertyTableView->setModel(tilePropertyListModel);
   ui->propertyTableView->resizeColumnToContents(0);

   QObject::connect(tilePropertyListModel,SIGNAL(dataChanged(QModelIndex,QModelIndex)),this,SLOT(tilePropertyListModel_dataChanged(QModelIndex,QModelIndex)));
   QObject::connect(ui->propertyTableView->selectionModel(),SIGNAL(currentChanged(QModelIndex,QModelIndex)),this,SLOT(propertyTableView_currentChanged(QModelIndex,QModelIndex)));

   imgData = new char[256*256*4];
   colorData = new char[256*256];
   colorDataOverlay = new char[256*256];
   colorDataSelection = new char[256*256];

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
         m_colorIndexes = palette;
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
   for ( idx = 0; idx < 256*256; idx++ )
   {
      colorData[idx] = 0;
      colorDataOverlay[idx] = 0;
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
   ui->xSize->setItemData(9,144);
   ui->xSize->setItemData(10,160);
   ui->xSize->setItemData(11,176);
   ui->xSize->setItemData(12,192);
   ui->xSize->setItemData(13,208);
   ui->xSize->setItemData(14,224);
   ui->xSize->setItemData(15,240);
   ui->xSize->setItemData(16,256);
   ui->ySize->setItemData(0,8);
   ui->ySize->setItemData(1,16);
   ui->ySize->setItemData(2,32);
   ui->ySize->setItemData(3,48);
   ui->ySize->setItemData(4,64);
   ui->ySize->setItemData(5,80);
   ui->ySize->setItemData(6,96);
   ui->ySize->setItemData(7,112);
   ui->ySize->setItemData(8,128);
   ui->ySize->setItemData(9,144);
   ui->ySize->setItemData(10,160);
   ui->ySize->setItemData(11,176);
   ui->ySize->setItemData(12,192);
   ui->ySize->setItemData(13,208);
   ui->ySize->setItemData(14,224);
   ui->ySize->setItemData(15,240);
   ui->ySize->setItemData(16,256);

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
   case 144:
      ui->xSize->setCurrentIndex(9);
      break;
   case 160:
      ui->xSize->setCurrentIndex(10);
      break;
   case 176:
      ui->xSize->setCurrentIndex(11);
      break;
   case 192:
      ui->xSize->setCurrentIndex(12);
      break;
   case 208:
      ui->xSize->setCurrentIndex(13);
      break;
   case 224:
      ui->xSize->setCurrentIndex(14);
      break;
   case 240:
      ui->xSize->setCurrentIndex(15);
      break;
   case 256:
      ui->xSize->setCurrentIndex(16);
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
   case 144:
      ui->ySize->setCurrentIndex(9);
      break;
   case 160:
      ui->ySize->setCurrentIndex(10);
      break;
   case 176:
      ui->ySize->setCurrentIndex(11);
      break;
   case 192:
      ui->ySize->setCurrentIndex(12);
      break;
   case 208:
      ui->ySize->setCurrentIndex(13);
      break;
   case 224:
      ui->ySize->setCurrentIndex(14);
      break;
   case 240:
      ui->ySize->setCurrentIndex(15);
      break;
   case 256:
      ui->ySize->setCurrentIndex(16);
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

   setCentralWidget(ui->window);
}

TileStampEditorForm::~TileStampEditorForm()
{
   delete imgData;
   delete renderer;
   delete previewer;
   delete tilePropertyListModel;
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
      else if ( event->type() == QEvent::KeyPress )
      {
         QKeyEvent* keyEvent = dynamic_cast<QKeyEvent*>(event);
         keyPressEvent(keyEvent);
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

void TileStampEditorForm::showEvent(QShowEvent */*event*/)
{
   emit addStatusBarWidget(ui->info);
   ui->info->show();
}

void TileStampEditorForm::hideEvent(QHideEvent */*event*/)
{
   emit removeStatusBarWidget(ui->info);
}

void TileStampEditorForm::contextMenuEvent(QContextMenuEvent */*event*/)
{
}

void TileStampEditorForm::resizeEvent(QResizeEvent* /*event*/)
{
   updateScrollbars();
}

void TileStampEditorForm::keyPressEvent(QKeyEvent *event)
{
   QByteArray oldTileData;
   QByteArray oldAttributeData;
   QClipboard* clipboard = QApplication::clipboard();
   QImage image;

   if ( event->modifiers() == Qt::ControlModifier )
   {
      if ( event->key() == Qt::Key_C )
      {
         if ( m_selection && m_selectionCaptured )
         {
            paintOverlay(Overlay_PasteSelection,0,m_selectionRect.left(),m_selectionRect.top(),m_selectionRect.right(),m_selectionRect.bottom());
            copyOverlayToNormal();
            renderer->update();
            previewer->update();
            copySelectionToClipboard(m_selectionRect.left(),m_selectionRect.top(),m_selectionRect.right(),m_selectionRect.bottom());
         }
      }
      else if ( event->key() == Qt::Key_V )
      {
         if ( m_selection )
         {
            clearSelection();
         }
         if ( !(clipboard->image().isNull()) )
         {
            image = clipboard->image();
            paintOverlay(Overlay_PasteClipboard,0,0,0,0,0);
            m_selectionRect = QRect(image.rect().left(),image.rect().top(),image.rect().width()+1,image.rect().height()+1);
            renderer->setBox(m_selectionRect.left(),m_selectionRect.top(),m_selectionRect.right(),m_selectionRect.bottom());
            m_selection = true;
            renderer->update();
            previewer->update();
            setModified(true);
            emit markProjectDirty(true);
         }
      }
      else if ( event->key() == Qt::Key_X )
      {
         if ( m_selection && m_selectionCaptured )
         {
            oldTileData = tileData(true);
            oldAttributeData = attributeData(true);
            copySelectionToClipboard(m_selectionRect.left(),m_selectionRect.top(),m_selectionRect.right(),m_selectionRect.bottom());
            paintOverlay(Overlay_Erase,0,m_selectionRect.left(),m_selectionRect.top(),m_selectionRect.right(),m_selectionRect.bottom());
            copyOverlayToNormal();
            m_undoStack.push(new TileStampPaintCommand(this,oldTileData,oldAttributeData));
            m_selection = false;
            renderer->setBox();
            renderer->update();
            previewer->update();
            setModified(true);
            emit markProjectDirty(true);
         }
      }
      else if ( event->key() == Qt::Key_Z )
      {
         clearSelection();
         m_undoStack.undo();
         renderer->update();
         previewer->update();
         if ( m_undoStack.isClean() )
         {
            setModified(false);
         }
      }
      else if ( event->key() == Qt::Key_Y )
      {
         clearSelection();
         m_undoStack.redo();
         renderer->update();
         previewer->update();
         setModified(true);
         emit markProjectDirty(true);
      }
   }
   CDesignerEditorBase::keyPressEvent(event);
}

void TileStampEditorForm::onSave()
{
   clearSelection();
   if ( treeLink() )
   {
      // This editor is paired with a project item, use the normal
      // project mechanics to do the saving.
      CDesignerEditorBase::onSave();
   }
}

void TileStampEditorForm::renderer_enterEvent(QEvent */*event*/)
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

void TileStampEditorForm::renderer_leaveEvent(QEvent */*event*/)
{
   if ( (m_activeTool == ui->pencilTool) ||
        (m_activeTool == ui->paintAttr) )
   {
      renderer->setBox();
      renderer->update();
   }

   updateInfoText();
}

void TileStampEditorForm::renderer_mouseDoubleClickEvent(QMouseEvent */*event*/)
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
      if ( (ui->pencilTool->isChecked()) ||
           (ui->paintAttr->isChecked()) )
      {
         pencilTool(event);
      }
      else if ( ui->paintTool->isChecked() )
      {
         paintTool(event);
      }
      else if ( ui->hollowBoxTool->isChecked() )
      {
         hollowBoxTool(event);
      }
      else if ( ui->filledBoxTool->isChecked() )
      {
         filledBoxTool(event);
      }
      else if ( ui->hollowCircleTool->isChecked() )
      {
         hollowCircleTool(event);
      }
      else if ( ui->filledCircleTool->isChecked() )
      {
         filledCircleTool(event);
      }
      else if ( ui->lineTool->isChecked() )
      {
         lineTool(event);
      }
      else if ( ui->selectionTool->isChecked() )
      {
         selectionTool(event);
      }
      else if ( ui->tileTabWidget->currentIndex().isValid() )
      {
         tileTool(event);
      }

      updateInfoText(pixx,pixy);
   }

   event->accept();
}

void TileStampEditorForm::renderer_mouseMoveEvent(QMouseEvent *event)
{
   int pixx;
   int pixy;

   renderer->pointToPixel(event->pos().x(),event->pos().y(),&pixx,&pixy);

   if ( (ui->pencilTool->isChecked()) ||
        (ui->paintAttr->isChecked()) )
   {
      pencilTool(event);
   }
   else if ( ui->paintTool->isChecked() )
   {
      paintTool(event);
   }
   else if ( ui->hollowBoxTool->isChecked() )
   {
      hollowBoxTool(event);
   }
   else if ( ui->filledBoxTool->isChecked() )
   {
      filledBoxTool(event);
   }
   else if ( ui->hollowCircleTool->isChecked() )
   {
      hollowCircleTool(event);
   }
   else if ( ui->filledCircleTool->isChecked() )
   {
      filledCircleTool(event);
   }
   else if ( ui->lineTool->isChecked() )
   {
      lineTool(event);
   }
   else if ( ui->selectionTool->isChecked() )
   {
      selectionTool(event);
   }
   else if ( ui->tileTabWidget->currentIndex().isValid() )
   {
      tileTool(event);
   }

   updateInfoText(pixx,pixy);

   event->accept();
}

void TileStampEditorForm::renderer_mouseReleaseEvent(QMouseEvent *event)
{
   int pixx;
   int pixy;

   renderer->pointToPixel(event->pos().x(),event->pos().y(),&pixx,&pixy);

   if ( (ui->pencilTool->isChecked()) ||
        (ui->paintAttr->isChecked()) )
   {
      pencilTool(event);
   }
   else if ( ui->paintTool->isChecked() )
   {
      paintTool(event);
   }
   else if ( ui->hollowBoxTool->isChecked() )
   {
      hollowBoxTool(event);
   }
   else if ( ui->filledBoxTool->isChecked() )
   {
      filledBoxTool(event);
   }
   else if ( ui->hollowCircleTool->isChecked() )
   {
      hollowCircleTool(event);
   }
   else if ( ui->filledCircleTool->isChecked() )
   {
      filledCircleTool(event);
   }
   else if ( ui->lineTool->isChecked() )
   {
      lineTool(event);
   }
   else if ( ui->selectionTool->isChecked() )
   {
      selectionTool(event);
   }
   else if ( ui->tileTabWidget->currentIndex().isValid() )
   {
      tileTool(event);
   }

   updateInfoText(pixx,pixy);

   event->accept();
}

void TileStampEditorForm::on_zoomSlider_valueChanged(int value)
{
   renderer->changeZoom(pow(2.0,value)*100.0);
   ui->zoomValueLabel->setText(QString::number(pow(2.0,value)*100.0).append("%"));
   updateScrollbars();
}

void TileStampEditorForm::on_horizontalScrollBar_valueChanged(int /*value*/)
{
   renderer->setScrollX(ui->horizontalScrollBar->value());
   renderer->update();
}

void TileStampEditorForm::on_verticalScrollBar_valueChanged(int /*value*/)
{
   renderer->setScrollY(ui->verticalScrollBar->value());
   renderer->update();
}

void TileStampEditorForm::xSize_currentIndexChanged(int /*index*/)
{
   int oldXSize = m_xSize;
   int oldYSize = m_ySize;
   m_xSize = ui->xSize->itemData(ui->xSize->currentIndex()).toInt();
   m_ySize = ui->ySize->itemData(ui->ySize->currentIndex()).toInt();
   m_undoStack.push(new TileStampResizeCommand(this,oldXSize,oldYSize));
   renderer->setSize(m_xSize,m_ySize);
   renderer->update();
   previewer->setSize(m_xSize,m_ySize);
   previewer->update();
   setModified(true);
   emit markProjectDirty(true);
}

void TileStampEditorForm::ySize_currentIndexChanged(int /*index*/)
{
   int oldXSize = m_xSize;
   int oldYSize = m_ySize;
   m_xSize = ui->xSize->itemData(ui->xSize->currentIndex()).toInt();
   m_ySize = ui->ySize->itemData(ui->ySize->currentIndex()).toInt();
   m_undoStack.push(new TileStampResizeCommand(this,oldXSize,oldYSize));
   renderer->setSize(m_xSize,m_ySize);
   renderer->update();
   previewer->setSize(m_xSize,m_ySize);
   previewer->update();
   setModified(true);
   emit markProjectDirty(true);
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
         m_colorIndexes = palette;
         for ( idx = 0; idx < m_colors.count(); idx++ )
         {
            m_colors.at(idx)->setCurrentColor(QColor(nesGetPaletteRedComponent(pAttrTbl->getPalette().at(idx)),nesGetPaletteGreenComponent(pAttrTbl->getPalette().at(idx)),nesGetPaletteBlueComponent(pAttrTbl->getPalette().at(idx))));
            m_colors.at(idx)->update();
         }
      }
   }

   // Re-color image.
   for ( idx = 0; idx < 256*256*4; idx += 4 )
   {
      color = m_colors.at(colorData[idx>>2])->currentColor();
      imgData[idx+0] = color.red();
      imgData[idx+1] = color.green();
      imgData[idx+2] = color.blue();
   }
   renderer->update();
   previewer->update();

   setModified(true);
   emit markProjectDirty(true);
}

void TileStampEditorForm::grid_toggled(bool checked)
{
   m_gridEnabled = checked;
   renderer->setGrid(m_gridEnabled);
   renderer->update();

   setModified(true);
   emit markProjectDirty(true);
}

void TileStampEditorForm::on_clear_clicked()
{
   int idx;
   QColor color;

   // Set up image...
   for ( idx = 0; idx < 256*256; idx++ )
   {
      colorData[idx] = 0;
      colorDataOverlay[idx] = 0;
   }
   for ( idx = 0; idx < 256*256*4; idx +=4 )
   {
      color = m_colors.at(colorData[idx>>2])->currentColor();
      imgData[idx+0] = color.red();
      imgData[idx+1] = color.green();
      imgData[idx+2] = color.blue();
      imgData[idx+3] = 0xFF;
   }
   renderer->update();
   previewer->update();

   setModified(true);
   emit markProjectDirty(true);
}

void TileStampEditorForm::on_selectionTool_clicked()
{
   m_activeTool = ui->selectionTool;

   // Clear selected tile if any.
   ui->tileTabWidget->setCurrentIndex(QModelIndex());

   // Clear other selected tools if any.
   ui->pencilTool->setChecked(false);
   ui->hollowBoxTool->setChecked(false);
   ui->filledBoxTool->setChecked(false);
   ui->lineTool->setChecked(false);
   ui->paintTool->setChecked(false);
   ui->paintAttr->setChecked(false);
   ui->hollowCircleTool->setChecked(false);
   ui->filledCircleTool->setChecked(false);
   ui->selectionTool->setChecked(true);

   // Haven't selected anything yet.
   clearSelection();
}

void TileStampEditorForm::on_paintTool_clicked()
{
   m_activeTool = ui->paintTool;

   // Clear selected tile if any.
   ui->tileTabWidget->setCurrentIndex(QModelIndex());

   // Clear other selected tools if any.
   ui->pencilTool->setChecked(false);
   ui->hollowBoxTool->setChecked(false);
   ui->filledBoxTool->setChecked(false);
   ui->lineTool->setChecked(false);
   ui->selectionTool->setChecked(false);
   ui->paintAttr->setChecked(false);
   ui->hollowCircleTool->setChecked(false);
   ui->filledCircleTool->setChecked(false);
   ui->paintTool->setChecked(true);

   // Haven't selected anything yet.
   clearSelection();
}

void TileStampEditorForm::on_pencilTool_clicked()
{
   m_activeTool = ui->pencilTool;

   // Clear selected tile if any.
   ui->tileTabWidget->setCurrentIndex(QModelIndex());

   // Clear other selected tools if any.
   ui->paintTool->setChecked(false);
   ui->hollowBoxTool->setChecked(false);
   ui->filledBoxTool->setChecked(false);
   ui->lineTool->setChecked(false);
   ui->selectionTool->setChecked(false);
   ui->paintAttr->setChecked(false);
   ui->hollowCircleTool->setChecked(false);
   ui->filledCircleTool->setChecked(false);
   ui->pencilTool->setChecked(true);

   // Haven't selected anything yet.
   clearSelection();
}

void TileStampEditorForm::on_hollowCircleTool_clicked()
{
   m_activeTool = ui->hollowCircleTool;

   // Clear selected tile if any.
   ui->tileTabWidget->setCurrentIndex(QModelIndex());

   // Clear other selected tools if any.
   ui->paintTool->setChecked(false);
   ui->pencilTool->setChecked(false);
   ui->hollowBoxTool->setChecked(false);
   ui->filledBoxTool->setChecked(false);
   ui->lineTool->setChecked(false);
   ui->selectionTool->setChecked(false);
   ui->paintAttr->setChecked(false);
   ui->filledCircleTool->setChecked(false);
   ui->hollowCircleTool->setChecked(true);

   // Haven't selected anything yet.
   clearSelection();
}

void TileStampEditorForm::on_filledCircleTool_clicked()
{
   m_activeTool = ui->filledCircleTool;

   // Clear selected tile if any.
   ui->tileTabWidget->setCurrentIndex(QModelIndex());

   // Clear other selected tools if any.
   ui->paintTool->setChecked(false);
   ui->pencilTool->setChecked(false);
   ui->hollowBoxTool->setChecked(false);
   ui->filledBoxTool->setChecked(false);
   ui->lineTool->setChecked(false);
   ui->selectionTool->setChecked(false);
   ui->paintAttr->setChecked(false);
   ui->hollowCircleTool->setChecked(false);
   ui->filledCircleTool->setChecked(true);

   // Haven't selected anything yet.
   clearSelection();
}

void TileStampEditorForm::on_hollowBoxTool_clicked()
{
   m_activeTool = ui->hollowBoxTool;

   // Clear selected tile if any.
   ui->tileTabWidget->setCurrentIndex(QModelIndex());

   // Clear other selected tools if any.
   ui->paintTool->setChecked(false);
   ui->pencilTool->setChecked(false);
   ui->filledBoxTool->setChecked(false);
   ui->lineTool->setChecked(false);
   ui->selectionTool->setChecked(false);
   ui->paintAttr->setChecked(false);
   ui->hollowCircleTool->setChecked(false);
   ui->filledCircleTool->setChecked(false);
   ui->hollowBoxTool->setChecked(true);

   // Haven't selected anything yet.
   clearSelection();
}

void TileStampEditorForm::on_filledBoxTool_clicked()
{
   m_activeTool = ui->filledBoxTool;

   // Clear selected tile if any.
   ui->tileTabWidget->setCurrentIndex(QModelIndex());

   // Clear other selected tools if any.
   ui->paintTool->setChecked(false);
   ui->pencilTool->setChecked(false);
   ui->hollowBoxTool->setChecked(false);
   ui->lineTool->setChecked(false);
   ui->selectionTool->setChecked(false);
   ui->paintAttr->setChecked(false);
   ui->hollowCircleTool->setChecked(false);
   ui->filledCircleTool->setChecked(false);
   ui->filledBoxTool->setChecked(true);

   // Haven't selected anything yet.
   clearSelection();
}

void TileStampEditorForm::on_lineTool_clicked()
{
   m_activeTool = ui->lineTool;

   // Clear selected tile if any.
   ui->tileTabWidget->setCurrentIndex(QModelIndex());

   // Clear other selected tools if any.
   ui->paintTool->setChecked(false);
   ui->pencilTool->setChecked(false);
   ui->hollowBoxTool->setChecked(false);
   ui->filledBoxTool->setChecked(false);
   ui->selectionTool->setChecked(false);
   ui->paintAttr->setChecked(false);
   ui->hollowCircleTool->setChecked(false);
   ui->filledCircleTool->setChecked(false);
   ui->lineTool->setChecked(true);

   // Haven't selected anything yet.
   clearSelection();
}

void TileStampEditorForm::on_paintAttr_clicked()
{
   m_activeTool = ui->paintAttr;

   // Clear selected tile if any.
   ui->tileTabWidget->setCurrentIndex(QModelIndex());

   // Clear other selected tools if any.
   ui->paintTool->setChecked(false);
   ui->pencilTool->setChecked(false);
   ui->hollowBoxTool->setChecked(false);
   ui->filledBoxTool->setChecked(false);
   ui->selectionTool->setChecked(false);
   ui->lineTool->setChecked(false);
   ui->hollowCircleTool->setChecked(false);
   ui->filledCircleTool->setChecked(false);
   ui->paintAttr->setChecked(true);

   // Haven't selected anything yet.
   clearSelection();
}

void TileStampEditorForm::tileSelected(QModelIndex /*index*/)
{
   if ( ui->paintTool->isChecked() )
   {
      m_activeTool = ui->paintTool;
   }
   else if ( ui->pencilTool->isChecked() )
   {
      m_activeTool = ui->pencilTool;
   }
   else if ( ui->hollowBoxTool->isChecked() )
   {
      m_activeTool = ui->hollowBoxTool;
   }
   else if ( ui->filledBoxTool->isChecked() )
   {
      m_activeTool = ui->filledBoxTool;
   }
   else if ( ui->hollowCircleTool->isChecked() )
   {
      m_activeTool = ui->hollowCircleTool;
   }
   else if ( ui->filledCircleTool->isChecked() )
   {
      m_activeTool = ui->filledCircleTool;
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

   // Clear other selected tools if any.
   ui->paintTool->setChecked(false);
   ui->pencilTool->setChecked(false);
   ui->hollowBoxTool->setChecked(false);
   ui->filledBoxTool->setChecked(false);
   ui->lineTool->setChecked(false);
   ui->selectionTool->setChecked(false);
   ui->paintAttr->setChecked(false);
   ui->hollowCircleTool->setChecked(false);
   ui->filledCircleTool->setChecked(false);

   // Haven't selected anything yet.
   clearSelection();
}

void TileStampEditorForm::colorPicked(bool /*value*/)
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
   ui->tileTabWidget->setCurrentIndex(QModelIndex());

   // Re-enable the last-selected tool.
   if ( m_activeTool )
   {
      m_activeTool->setChecked(true);
   }

   // Haven't selected anything yet.
   clearSelection();
}

void TileStampEditorForm::on_cwRotate_clicked()
{
   int xSrc;
   int ySrc;
   int xDest;
   int yDest;
   int swap;
   QByteArray oldTileData;
   QByteArray oldAttributeData;

   // Use the overlay area to rotate.
   for ( ySrc = 0; ySrc < m_ySize; ySrc++ )
   {
      for ( xSrc = 0; xSrc < m_xSize; xSrc++ )
      {
         yDest = xSrc;
         xDest = m_ySize-ySrc-1;
         colorDataOverlay[(yDest*256)+xDest] = colorData[(ySrc*256)+xSrc];
      }
   }

   // Swap the overlay.
   oldTileData = tileData();
   oldAttributeData = attributeData();
   copyOverlayToNormal();
   paintNormal();
   m_undoStack.push(new TileStampPaintCommand(this,oldTileData,oldAttributeData));
   renderer->setBox();
   setModified(true);
   emit markProjectDirty(true);

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
   QByteArray oldTileData;
   QByteArray oldAttributeData;

   // Use the overlay area to rotate.
   for ( ySrc = 0; ySrc < m_ySize; ySrc++ )
   {
      for ( xSrc = 0; xSrc < m_xSize; xSrc++ )
      {
         xDest = ySrc;
         yDest = m_xSize-xSrc-1;
         colorDataOverlay[(yDest*256)+xDest] = colorData[(ySrc*256)+xSrc];
      }
   }

   // Swap the overlay.
   oldTileData = tileData();
   oldAttributeData = attributeData();
   copyOverlayToNormal();
   paintNormal();
   m_undoStack.push(new TileStampPaintCommand(this,oldTileData,oldAttributeData));
   renderer->setBox();
   setModified(true);
   emit markProjectDirty(true);

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
   QByteArray oldTileData;
   QByteArray oldAttributeData;

   // Use the overlay area to rotate.
   for ( ySrc = 0; ySrc < m_ySize; ySrc++ )
   {
      for ( xSrc = 0; xSrc < m_xSize; xSrc++ )
      {
         xDest = m_xSize-xSrc-1;
         yDest = ySrc;
         colorDataOverlay[(yDest*256)+xDest] = colorData[(ySrc*256)+xSrc];
      }
   }

   // Swap the overlay.
   oldTileData = tileData();
   oldAttributeData = attributeData();
   copyOverlayToNormal();
   paintNormal();
   m_undoStack.push(new TileStampPaintCommand(this,oldTileData,oldAttributeData));
   renderer->setBox();
   setModified(true);
   emit markProjectDirty(true);
}

void TileStampEditorForm::on_flipVertical_clicked()
{
   int xSrc;
   int ySrc;
   int xDest;
   int yDest;
   QByteArray oldTileData;
   QByteArray oldAttributeData;

   // Use the overlay area to rotate.
   for ( ySrc = 0; ySrc < m_ySize; ySrc++ )
   {
      for ( xSrc = 0; xSrc < m_xSize; xSrc++ )
      {
         xDest = xSrc;
         yDest = m_ySize-ySrc-1;
         colorDataOverlay[(yDest*256)+xDest] = colorData[(ySrc*256)+xSrc];
      }
   }

   // Swap the overlay.
   oldTileData = tileData();
   oldAttributeData = attributeData();
   copyOverlayToNormal();
   paintNormal();
   m_undoStack.push(new TileStampPaintCommand(this,oldTileData,oldAttributeData));
   renderer->setBox();
   setModified(true);
   emit markProjectDirty(true);
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

   for ( idx = 0; idx < 256*256*4; idx +=4 )
   {
      color = m_colors.at(colorData[idx>>2])->currentColor();
      imgData[idx+0] = color.red();
      imgData[idx+1] = color.green();
      imgData[idx+2] = color.blue();
      imgData[idx+3] = 0xFF;

      // Reset overlay.
      colorDataOverlay[idx>>2] = colorData[idx>>2];
   }
   renderer->update();
   previewer->update();

}

void TileStampEditorForm::clearSelection()
{
   if ( m_selection )
   {
      copyOverlayToNormal();
      paintNormal();
   }
   renderer->setBox();
   renderer->update();
   previewer->update();

   m_selection = false;
   m_selectionCaptured = false;

   // Create "empty" rect.
   m_selectionRect = QRect(QPoint(0,0),QPoint(-1,-1));
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

   for ( idx = 0; idx < 256*256; idx++ )
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
                  colorDataOverlay[((posY+tileY+y)*256)+posX+tileX+x] = pixel;
                  plane1 <<= 1;
                  plane2 <<= 1;

                  // Recolor edge tiles if necessary.
                  if ( !recolorPointQueue.contains(QPoint(posX+tileX+x-((posX+tileX+x)%16),posY+tileY+y-((posY+tileY+y)%16))) )
                  {
                     recolorPointQueue.append(QPoint(posX+tileX+x-((posX+tileX+x)%16),posY+tileY+y-((posY+tileY+y)%16)));
                     recolorColorQueue.append(pixel);
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
      // Force recoloration in this case since we're painting tiles.
      recolorTiles(recolorPointQueue.at(idx).x(),recolorPointQueue.at(idx).y(),recolorColorQueue.at(idx),true);
   }

   for ( idx = 0; idx < 256*256*4; idx +=4 )
   {
      color = m_colors.at(colorDataOverlay[idx>>2])->currentColor();
      imgData[idx+0] = color.red();
      imgData[idx+1] = color.green();
      imgData[idx+2] = color.blue();
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
   int f;
   int ddF_x;
   int ddF_y;
   int x;
   int y;
   int x0;
   int y0;
   int radius;
   QList<QPoint> fillQueue;
   QList<QPoint> recolorQueue;
   QPoint point;
   QClipboard* clipboard = QApplication::clipboard();
   QImage image;
   const uchar* bits;
   int targetColor;

   for ( idx = 0; idx < 256*256; idx++ )
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
         // If the selected color is background, don't change the attribute, just nix the color.
         if ( selectedColor == 0 )
         {
            colorDataOverlay[(boxY1*256)+boxX1] = (colorDataOverlay[(boxY1*256)+boxX1])&0xFC;
         }
         else
         {
            colorDataOverlay[(boxY1*256)+boxX1] = selectedColor;
         }
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
         recolorTiles(point.x(),point.y(),selectedColor);
      }
      break;
   case Overlay_FloodFill:
//Flood-fill (node, target-color, replacement-color):
      targetColor = colorDataOverlay[(boxY1*256)+boxX1];
//1. Set Q to the empty queue.
//2. If the color of node is equal to target-color, return.
      if ( targetColor == selectedColor )
      {
         break;
      }
//3. Add node to the end of Q.
      fillQueue.append(QPoint(boxX1,boxY1));
//4. While Q is not empty:
      while ( !fillQueue.isEmpty() )
      {
//5.     Set n equal to the first element of Q
         point = fillQueue.takeFirst();
//6.     If the color of n is equal to target-color, set the color of n to replacement-color.
         // If the selected color is background, don't change the attribute, just nix the color.
         if ( selectedColor == 0 )
         {
            colorDataOverlay[(point.y()*256)+point.x()] = (colorDataOverlay[(point.y()*256)+point.x()])&0xFC;
         }
         else
         {
            colorDataOverlay[(point.y()*256)+point.x()] = selectedColor;
         }
         if ( !recolorQueue.contains(QPoint(point.x()-(point.x()%16),point.y()-(point.y()%16))) )
         {
            recolorQueue.append(QPoint(point.x()-(point.x()%16),point.y()-(point.y()%16)));
         }
//7.     Remove first element from Q
//8.     If the color of the node to the west of n is target-color:
         if ( ((point.x()-1) >= 0) &&
              (colorDataOverlay[(point.y()*256)+point.x()-1] == targetColor) )
         {
//9.         Add that node to the end of Q
            fillQueue.append(QPoint(point.x()-1,point.y()));
            // If the selected color is background, don't change the attribute, just nix the color.
            if ( selectedColor == 0 )
            {
               colorDataOverlay[(point.y()*256)+point.x()-1] = (colorDataOverlay[(point.y()*256)+point.x()-1])&0xFC;
            }
            else
            {
               colorDataOverlay[(point.y()*256)+point.x()-1] = selectedColor;
            }
            if ( !recolorQueue.contains(QPoint((point.x()-1)-((point.x()-1)%16),point.y()-(point.y()%16))) )
            {
               recolorQueue.append(QPoint((point.x()-1)-((point.x()-1)%16),point.y()-(point.y()%16)));
            }
         }
//10.     If the color of the node to the east of n is target-color:
         if ( ((point.x()+1) < m_xSize) &&
              (colorDataOverlay[(point.y()*256)+point.x()+1] == targetColor) )
         {
//11.         Add that node to the end of Q
            fillQueue.append(QPoint(point.x()+1,point.y()));
            // If the selected color is background, don't change the attribute, just nix the color.
            if ( selectedColor == 0 )
            {
               colorDataOverlay[(point.y()*256)+point.x()+1] = (colorDataOverlay[(point.y()*256)+point.x()+1])&0xFC;
            }
            else
            {
               colorDataOverlay[(point.y()*256)+point.x()+1] = selectedColor;
            }
            if ( !recolorQueue.contains(QPoint((point.x()+1)-((point.x()+1)%16),point.y()-(point.y()%16))) )
            {
               recolorQueue.append(QPoint((point.x()+1)-((point.x()+1)%16),point.y()-(point.y()%16)));
            }
         }
//12.     If the color of the node to the north of n is target-color:
         if ( ((point.y()-1) >= 0) &&
              (colorDataOverlay[((point.y()-1)*256)+point.x()] == targetColor) )
         {
//13.         Add that node to the end of Q
            fillQueue.append(QPoint(point.x(),point.y()-1));
            // If the selected color is background, don't change the attribute, just nix the color.
            if ( selectedColor == 0 )
            {
               colorDataOverlay[((point.y()-1)*256)+point.x()] = (colorDataOverlay[((point.y()-1)*256)+point.x()])&0xFC;
            }
            else
            {
               colorDataOverlay[((point.y()-1)*256)+point.x()] = selectedColor;
            }
            if ( !recolorQueue.contains(QPoint(point.x()-(point.x()%16),(point.y()-1)-((point.y()-1)%16))) )
            {
               recolorQueue.append(QPoint(point.x()-(point.x()%16),(point.y()-1)-((point.y()-1)%16)));
            }
         }
//14.     If the color of the node to the south of n is target-color:
         if ( ((point.y()+1) < m_ySize) &&
              (colorDataOverlay[((point.y()+1)*256)+point.x()] == targetColor) )
         {
//15.         Add that node to the end of Q
            fillQueue.append(QPoint(point.x(),point.y()+1));
            // If the selected color is background, don't change the attribute, just nix the color.
            if ( selectedColor == 0 )
            {
               colorDataOverlay[((point.y()+1)*256)+point.x()] = (colorDataOverlay[((point.y()+1)*256)+point.x()])&0xFC;
            }
            else
            {
               colorDataOverlay[((point.y()+1)*256)+point.x()] = selectedColor;
            }
            if ( !recolorQueue.contains(QPoint(point.x()-(point.x()%16),(point.y()+1)-((point.y()+1)%16))) )
            {
               recolorQueue.append(QPoint(point.x()-(point.x()%16),(point.y()+1)-((point.y()+1)%16)));
            }
         }
      }
      //16. Return.
      foreach ( QPoint point, recolorQueue )
      {
         recolorTiles(point.x(),point.y(),selectedColor);
      }
      break;
   case Overlay_HollowCircle:
   case Overlay_FilledCircle:
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

      radius = ((boxX2-boxX1)/2)<((boxY2-boxY1)/2)?((boxX2-boxX1)/2):((boxY2-boxY1)/2);
      x0 = boxX1+((boxX2-boxX1)/2);
      y0 = boxY1+((boxY2-boxY1)/2);
      f = 1-radius;
      ddF_x = 1;
      ddF_y = -2*radius;
      x = 0;
      y = radius;

      // If the selected color is background, don't change the attribute, just nix the color.
      if ( selectedColor == 0 )
      {
         colorDataOverlay[((y0+radius)*256)+x0] = (colorDataOverlay[((y0+radius)*256)+x0])&0xFC;
      }
      else
      {
         colorDataOverlay[((y0+radius)*256)+x0] = selectedColor;
      }
      if ( !recolorQueue.contains(QPoint(x0,y0+radius)) )
      {
         recolorQueue.append(QPoint(x0,y0+radius));
      }
      // If the selected color is background, don't change the attribute, just nix the color.
      if ( selectedColor == 0 )
      {
         colorDataOverlay[((y0-radius)*256)+x0] = (colorDataOverlay[((y0-radius)*256)+x0])&0xFC;
      }
      else
      {
         colorDataOverlay[((y0-radius)*256)+x0] = selectedColor;
      }
      if ( !recolorQueue.contains(QPoint(x0,y0-radius)) )
      {
         recolorQueue.append(QPoint(x0,y0-radius));
      }
      // If the selected color is background, don't change the attribute, just nix the color.
      if ( selectedColor == 0 )
      {
         colorDataOverlay[(y0*256)+(x0+radius)] = (colorDataOverlay[(y0*256)+(x0+radius)])&0xFC;
      }
      else
      {
         colorDataOverlay[(y0*256)+(x0+radius)] = selectedColor;
      }
      if ( !recolorQueue.contains(QPoint(x0+radius,y0)) )
      {
         recolorQueue.append(QPoint(x0+radius,y0));
      }
      // If the selected color is background, don't change the attribute, just nix the color.
      if ( selectedColor == 0 )
      {
         colorDataOverlay[(y0*256)+(x0-radius)] = (colorDataOverlay[(y0*256)+(x0-radius)])&0xFC;
      }
      else
      {
         colorDataOverlay[(y0*256)+(x0-radius)] = selectedColor;
      }
      if ( !recolorQueue.contains(QPoint(x0-radius,y0)) )
      {
         recolorQueue.append(QPoint(x0-radius,y0));
      }

      while(x < y)
      {
         // ddF_x == 2 * x + 1;
         // ddF_y == -2 * y;
         // f == x*x + y*y - radius*radius + 2*x - y + 1;
         if(f >= 0)
         {
            y--;
            ddF_y += 2;
            f += ddF_y;
         }
         x++;
         ddF_x += 2;
         f += ddF_x;

         // If the selected color is background, don't change the attribute, just nix the color.
         if ( selectedColor == 0 )
         {
            colorDataOverlay[((y0+y)*256)+(x0+x)] = (colorDataOverlay[((y0+y)*256)+(x0+x)])&0xFC;
         }
         else
         {
            colorDataOverlay[((y0+y)*256)+(x0+x)] = selectedColor;
         }
         if ( !recolorQueue.contains(QPoint(x0+x,y0+y)) )
         {
            recolorQueue.append(QPoint(x0+x,y0+y));
         }
         // If the selected color is background, don't change the attribute, just nix the color.
         if ( selectedColor == 0 )
         {
            colorDataOverlay[((y0+y)*256)+(x0-x)] = (colorDataOverlay[((y0+y)*256)+(x0-x)])&0xFC;
         }
         else
         {
            colorDataOverlay[((y0+y)*256)+(x0-x)] = selectedColor;
         }
         if ( !recolorQueue.contains(QPoint(x0-x,y0+y)) )
         {
            recolorQueue.append(QPoint(x0-x,y0+y));
         }
         // If the selected color is background, don't change the attribute, just nix the color.
         if ( selectedColor == 0 )
         {
            colorDataOverlay[((y0-y)*256)+(x0+x)] = (colorDataOverlay[((y0-y)*256)+(x0+x)])&0xFC;
         }
         else
         {
            colorDataOverlay[((y0-y)*256)+(x0+x)] = selectedColor;
         }
         if ( !recolorQueue.contains(QPoint(x0+x,y0-y)) )
         {
            recolorQueue.append(QPoint(x0+x,y0-y));
         }
         // If the selected color is background, don't change the attribute, just nix the color.
         if ( selectedColor == 0 )
         {
            colorDataOverlay[((y0-y)*256)+(x0-x)] = (colorDataOverlay[((y0-y)*256)+(x0-x)])&0xFC;
         }
         else
         {
            colorDataOverlay[((y0-y)*256)+(x0-x)] = selectedColor;
         }
         if ( !recolorQueue.contains(QPoint(x0-x,y0-y)) )
         {
            recolorQueue.append(QPoint(x0-x,y0-y));
         }
         // If the selected color is background, don't change the attribute, just nix the color.
         if ( selectedColor == 0 )
         {
            colorDataOverlay[((y0+x)*256)+(x0+y)] = (colorDataOverlay[((y0+x)*256)+(x0+y)])&0xFC;
         }
         else
         {
            colorDataOverlay[((y0+x)*256)+(x0+y)] = selectedColor;
         }
         if ( !recolorQueue.contains(QPoint(x0+y,y0+x)) )
         {
            recolorQueue.append(QPoint(x0+y,y0+x));
         }
         // If the selected color is background, don't change the attribute, just nix the color.
         if ( selectedColor == 0 )
         {
            colorDataOverlay[((y0+x)*256)+(x0-y)] = (colorDataOverlay[((y0+x)*256)+(x0-y)])&0xFC;
         }
         else
         {
            colorDataOverlay[((y0+x)*256)+(x0-y)] = selectedColor;
         }
         if ( !recolorQueue.contains(QPoint(x0-y,y0+x)) )
         {
            recolorQueue.append(QPoint(x0-y,y0+x));
         }
         // If the selected color is background, don't change the attribute, just nix the color.
         if ( selectedColor == 0 )
         {
            colorDataOverlay[((y0-x)*256)+(x0+y)] = (colorDataOverlay[((y0-x)*256)+(x0+y)])&0xFC;
         }
         else
         {
            colorDataOverlay[((y0-x)*256)+(x0+y)] = selectedColor;
         }
         if ( !recolorQueue.contains(QPoint(x0+y,y0-x)) )
         {
            recolorQueue.append(QPoint(x0+y,y0-x));
         }
         // If the selected color is background, don't change the attribute, just nix the color.
         if ( selectedColor == 0 )
         {
            colorDataOverlay[((y0-x)*256)+(x0-y)] = (colorDataOverlay[((y0-x)*256)+(x0-y)])&0xFC;
         }
         else
         {
            colorDataOverlay[((y0-x)*256)+(x0-y)] = selectedColor;
         }
         if ( !recolorQueue.contains(QPoint(x0-y,y0-x)) )
         {
            recolorQueue.append(QPoint(x0-y,y0-x));
         }
      }
      if ( type == Overlay_FilledCircle )
      {
         for ( y = -radius; y <= radius; y++ )
         {
            for ( x = -radius; x <= radius; x++ )
            {
               if ( x*x+y*y <= radius*radius )
               {
                  // If the selected color is background, don't change the attribute, just nix the color.
                  if ( selectedColor == 0 )
                  {
                     colorDataOverlay[((boxY1+((boxY2-boxY1)/2)+y)*256)+(boxX1+((boxX2-boxX1)/2)+x)] = (colorDataOverlay[((boxY1+((boxY2-boxY1)/2)+y)*256)+(boxX1+((boxX2-boxX1)/2)+x)])&0xFC;
                  }
                  else
                  {
                     colorDataOverlay[((boxY1+((boxY2-boxY1)/2)+y)*256)+(boxX1+((boxX2-boxX1)/2)+x)] = selectedColor;
                  }
               }
            }
         }
      }
      foreach ( QPoint point, recolorQueue )
      {
         recolorTiles(point.x(),point.y(),selectedColor);
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
         // If the selected color is background, don't change the attribute, just nix the color.
         if ( selectedColor == 0 )
         {
            colorDataOverlay[(boxY1*256)+idx] = (colorDataOverlay[(boxY1*256)+idx])&0xFC;
         }
         else
         {
            colorDataOverlay[(boxY1*256)+idx] = selectedColor;
         }
         if ( !recolorQueue.contains(QPoint(idx-(idx%16),boxY1-(boxY1%16))) )
         {
            recolorQueue.append(QPoint(idx-(idx%16),boxY1-(boxY1%16)));
         }
         // If the selected color is background, don't change the attribute, just nix the color.
         if ( selectedColor == 0 )
         {
            colorDataOverlay[((boxY2-1)*256)+idx] = (colorDataOverlay[((boxY2-1)*256)+idx])&0xFC;
         }
         else
         {
            colorDataOverlay[((boxY2-1)*256)+idx] = selectedColor;
         }
         if ( !recolorQueue.contains(QPoint(idx-(idx%16),(boxY2-1)-((boxY2-1)%16))) )
         {
            recolorQueue.append(QPoint(idx-(idx%16),(boxY2-1)-((boxY2-1)%16)));
         }
      }
      // Draw the verticals.
      for ( idx = boxY1; idx < boxY2; idx++ )
      {
         // If the selected color is background, don't change the attribute, just nix the color.
         if ( selectedColor == 0 )
         {
            colorDataOverlay[(idx*256)+boxX1] = (colorDataOverlay[(idx*256)+boxX1])&0xFC;
         }
         else
         {
            colorDataOverlay[(idx*256)+boxX1] = selectedColor;
         }
         if ( !recolorQueue.contains(QPoint(boxX1-(boxX1%16),idx-(idx%16))) )
         {
            recolorQueue.append(QPoint(boxX1-(boxX1%16),idx-(idx%16)));
         }
         // If the selected color is background, don't change the attribute, just nix the color.
         if ( selectedColor == 0 )
         {
            colorDataOverlay[(idx*256)+(boxX2-1)] = (colorDataOverlay[(idx*256)+(boxX2-1)])&0xFC;
         }
         else
         {
            colorDataOverlay[(idx*256)+(boxX2-1)] = selectedColor;
         }
         if ( !recolorQueue.contains(QPoint((boxX2-1)-((boxX2-1)%16),idx-(idx%16))) )
         {
            recolorQueue.append(QPoint((boxX2-1)-((boxX2-1)%16),idx-(idx%16)));
         }
      }
      foreach ( QPoint point, recolorQueue )
      {
         recolorTiles(point.x(),point.y(),selectedColor);
      }
      break;
   case Overlay_FilledBox:
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
         // If the selected color is background, don't change the attribute, just nix the color.
         if ( selectedColor == 0 )
         {
            colorDataOverlay[(boxY1*256)+idx] = (colorDataOverlay[(boxY1*256)+idx])&0xFC;
         }
         else
         {
            colorDataOverlay[(boxY1*256)+idx] = selectedColor;
         }
         if ( !recolorQueue.contains(QPoint(idx-(idx%16),boxY1-(boxY1%16))) )
         {
            recolorQueue.append(QPoint(idx-(idx%16),boxY1-(boxY1%16)));
         }
         // If the selected color is background, don't change the attribute, just nix the color.
         if ( selectedColor == 0 )
         {
            colorDataOverlay[((boxY2-1)*256)+idx] = (colorDataOverlay[((boxY2-1)*256)+idx])&0xFC;
         }
         else
         {
            colorDataOverlay[((boxY2-1)*256)+idx] = selectedColor;
         }
         if ( !recolorQueue.contains(QPoint(idx-(idx%16),(boxY2-1)-((boxY2-1)%16))) )
         {
            recolorQueue.append(QPoint(idx-(idx%16),(boxY2-1)-((boxY2-1)%16)));
         }
      }
      // Draw the verticals.
      for ( idx = boxY1; idx < boxY2; idx++ )
      {
         // If the selected color is background, don't change the attribute, just nix the color.
         if ( selectedColor == 0 )
         {
            colorDataOverlay[(idx*256)+boxX1] = (colorDataOverlay[(idx*256)+boxX1])&0xFC;
         }
         else
         {
            colorDataOverlay[(idx*256)+boxX1] = selectedColor;
         }
         if ( !recolorQueue.contains(QPoint(boxX1-(boxX1%16),idx-(idx%16))) )
         {
            recolorQueue.append(QPoint(boxX1-(boxX1%16),idx-(idx%16)));
         }
         // If the selected color is background, don't change the attribute, just nix the color.
         if ( selectedColor == 0 )
         {
            colorDataOverlay[(idx*256)+(boxX2-1)] = (colorDataOverlay[(idx*256)+(boxX2-1)])&0xFC;
         }
         else
         {
            colorDataOverlay[(idx*256)+(boxX2-1)] = selectedColor;
         }
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
            // If the selected color is background, don't change the attribute, just nix the color.
            if ( selectedColor == 0 )
            {
               colorDataOverlay[(idxy*256)+idxx] = (colorDataOverlay[(idxy*256)+idxx])&0xFC;
            }
            else
            {
               colorDataOverlay[(idxy*256)+idxx] = selectedColor;
            }
         }
      }
      foreach ( QPoint point, recolorQueue )
      {
         recolorTiles(point.x(),point.y(),selectedColor);
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
            colorDataOverlay[(idxy*256)+idxx] = colorDataOverlay[(idxy*256)+idxx]&0xFC;
         }
      }
      break;
   case Overlay_PasteClipboard:
      image = clipboard->image();
      if ( !(image.isNull()) )
      {
         bits = image.bits();
         for ( idxy = boxY1, idxcy = 0; idxy < (boxY1+image.height()); idxy++, idxcy++ )
         {
            for ( idxx = boxX1, idxcx = 0; idxx < (boxX1+image.width()); idxx++, idxcx++ )
            {
               if ( (idxx >= 0) &&
                    (idxx < m_xSize) &&
                    (idxy >= 0) &&
                    (idxy < m_ySize) )
               {
                  colorDataOverlay[(idxy*256)+idxx] &= 0xFC;
                  colorDataOverlay[(idxy*256)+idxx] |= (bits[(idxcy*image.bytesPerLine())+idxcx]&0x03);
               }
            }
         }
      }
      break;
   case Overlay_PasteSelection:
      for ( idxy = boxY1, idxcy = 0; idxy < boxY2; idxy++, idxcy++ )
      {
         for ( idxx = boxX1, idxcx = 0; idxx < boxX2; idxx++, idxcx++ )
         {
            if ( (idxx >= 0) &&
                 (idxx < m_xSize) &&
                 (idxy >= 0) &&
                 (idxy < m_ySize) )
            {
               colorDataOverlay[(idxy*256)+idxx] &= 0xFC;
               colorDataOverlay[(idxy*256)+idxx] |= (colorDataSelection[(idxcy*256)+idxcx]&0x03);
            }
         }
      }
      break;
   }

   for ( idx = 0; idx < 256*256*4; idx +=4 )
   {
      color = m_colors.at(colorDataOverlay[idx>>2])->currentColor();
      imgData[idx+0] = color.red();
      imgData[idx+1] = color.green();
      imgData[idx+2] = color.blue();
      imgData[idx+3] = 0xFF;
   }
}

void TileStampEditorForm::paintOverlay(int selectedColor,int pixx,int pixy)
{
   QColor color;
   int idx;

   // If the selected color is background, don't change the attribute, just nix the color.
   if ( selectedColor == 0 )
   {
      colorDataOverlay[(pixy*256)+pixx] = (colorDataOverlay[(pixy*256)+pixx])&0xFC;
   }
   else
   {
      colorDataOverlay[(pixy*256)+pixx] = selectedColor;
   }
   recolorTiles(pixx,pixy,selectedColor);

   for ( idx = 0; idx < 256*256*4; idx +=4 )
   {
      color = m_colors.at(colorDataOverlay[idx>>2])->currentColor();
      imgData[idx+0] = color.red();
      imgData[idx+1] = color.green();
      imgData[idx+2] = color.blue();
      imgData[idx+3] = 0xFF;
   }
}

void TileStampEditorForm::copyOverlayToNormal()
{
   int idx;
   QColor color;

   for ( idx = 0; idx < 256*256; idx++ )
   {
      colorData[idx] = colorDataOverlay[idx];
   }
}

void TileStampEditorForm::copySelectionToClipboard(int boxX1,int boxY1,int boxX2,int boxY2)
{
   QClipboard* clipboard = QApplication::clipboard();
   QImage image;

   image = CImageConverters::toIndexed8(tileData(true),attributeData(true),m_colorIndexes,m_xSize,m_ySize);
   image = image.copy(boxX1,boxY1,boxX2-boxX1,boxY2-boxY1);
   clipboard->setImage(image);
}

void TileStampEditorForm::copyNormalToSelection(int boxX1, int boxY1, int boxX2, int boxY2)
{
   int idxx;
   int idxy;
   int idxcx;
   int idxcy;

   for ( idxy = boxY1, idxcy = 0; idxy < boxY2; idxy++, idxcy++ )
   {
      for ( idxx = boxX1, idxcx = 0; idxx < boxX2; idxx++, idxcx++ )
      {
         colorDataSelection[(idxcy*256)+idxcx] = colorData[(idxy*256)+idxx];
      }
   }
}

void TileStampEditorForm::recolorTiles(int pixx,int pixy,int newColor,bool force)
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
   int newColorTable;
   QColor color;

   if ( (force) || (newColor%4) )
   {
      newColorTable = newColor/4;

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
               colorDataOverlay[(idxy*256)+idxx] &= 0x3;
               colorDataOverlay[(idxy*256)+idxx] |= (newColorTable<<2);
            }
         }
      }

      // Re-color image.
      for ( idx = 0; idx < 256*256*4; idx += 4 )
      {
         color = m_colors.at(colorDataOverlay[idx>>2])->currentColor();
         imgData[idx+0] = color.red();
         imgData[idx+1] = color.green();
         imgData[idx+2] = color.blue();
      }
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
            m_colorIndexes = palette;
            for ( idx = 0; idx < m_colors.count(); idx++ )
            {
               m_colors.at(idx)->setCurrentColor(QColor(nesGetPaletteRedComponent(pAttrTbl->getPalette().at(idx)),nesGetPaletteGreenComponent(pAttrTbl->getPalette().at(idx)),nesGetPaletteBlueComponent(pAttrTbl->getPalette().at(idx))));
               m_colors.at(idx)->update();
            }
         }
      }

      // Re-color image.
      for ( idx = 0; idx < 256*256*4; idx += 4 )
      {
         color = m_colors.at(colorData[idx>>2])->currentColor();
         imgData[idx+0] = color.red();
         imgData[idx+1] = color.green();
         imgData[idx+2] = color.blue();
      }
      renderer->update();
      previewer->update();
   }

   // Update tile list.
   QList<IChrRomBankItem*> tileListChrItem;
   iter.reset(nesicideProject->getProject());
   while ( iter.current() )
   {
      IChrRomBankItem* pChrItem = dynamic_cast<IChrRomBankItem*>(iter.current());
      IProjectTreeViewItem* pProjItem = dynamic_cast<IProjectTreeViewItem*>(iter.current());
      if ( pChrItem &&
           (pProjItem != this->treeLink()) )
      {
         tileListChrItem.append(pChrItem);
      }

      // Move through tree.
      iter.next();
   }

   ui->tileTabWidget->setItems(tileListChrItem);
}

void TileStampEditorForm::applyProjectPropertiesToTab()
{
   QList<PropertyItem> tileProperties = m_tileProperties;
   int idx = 0;

   m_tileProperties = nesicideProject->getTileProperties();

   // Update local tile properties from saved tile info.
   foreach ( PropertyItem globalItem, m_tileProperties )
   {
      foreach ( PropertyItem localItem, tileProperties )
      {
         if ( globalItem.name == localItem.name )
         {
            globalItem.type = localItem.type;
            globalItem.value = localItem.value;
            m_tileProperties.replace(idx,globalItem);
         }
      }
      idx++;
   }

   tilePropertyListModel->setItems(m_tileProperties);
   tilePropertyListModel->update();
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
            colorData[((tileY+y)*256)+tileX+x] = pixel;
            colorDataOverlay[((tileY+y)*256)+tileX+x] = pixel;
            plane1 <<= 1;
            plane2 <<= 1;
         }
      }
   }
}

QByteArray TileStampEditorForm::tileData(bool useOverlay)
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

            if ( useOverlay )
            {
               plane1[y] |= colorDataOverlay[((tileY+y)*256)+tileX+x]&0x01;
               plane2[y] |= ((colorDataOverlay[((tileY+y)*256)+tileX+x]&0x02)>>1);
            }
            else
            {
               plane1[y] |= colorData[((tileY+y)*256)+tileX+x]&0x01;
               plane2[y] |= ((colorData[((tileY+y)*256)+tileX+x]&0x02)>>1);
            }
         }
      }
      chrOut.append(plane1,8);
      chrOut.append(plane2,8);
   }
   return chrOut;
}

QByteArray TileStampEditorForm::attributeData(bool useOverlay)
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
            if ( useOverlay )
            {
               attrTable [ ((tileY>>1)*(tileWidth>>1))+(tileX>>1) ] = colorDataOverlay[(tileY*8*256)+(tileX*8)]>>2;
            }
            else
            {
               attrTable [ ((tileY>>1)*(tileWidth>>1))+(tileX>>1) ] = colorData[(tileY*8*256)+(tileX*8)]>>2;
            }
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
   int boxX1;
   int boxY1;
   int boxX2;
   int boxY2;
   QCursor cursor;

   renderer->pointToPixel(event->pos().x(),event->pos().y(),&pixx,&pixy);

   // Where's the box in the renderer?
   renderer->getBox(&boxX1,&boxY1,&boxX2,&boxY2);

   switch ( event->type() )
   {
   case QEvent::MouseButtonPress:
      if ( !m_selectionRect.contains(pixx,pixy) )
      {
         clearSelection();
         renderer->update();
         previewer->update();
      }
      else
      {
         m_anchor = QPoint(pixx,pixy);
      }
      if ( m_selection )
      {
         if ( (boxX1 == pixx) && (boxY1 == pixy) )
         {
            m_resizeMode = Resize_TopLeft;
         }
         else if ( (boxX2 == pixx) && (boxY1 == pixy) )
         {
            m_resizeMode = Resize_TopRight;
         }
         else if ( (boxX1 == pixx) && (boxY2 == pixy) )
         {
            m_resizeMode = Resize_BottomLeft;
         }
         else if ( (boxX2 == pixx) && (boxY2 == pixy) )
         {
            m_resizeMode = Resize_BottomRight;
         }
         else if ( boxX1 == pixx )
         {
            m_resizeMode = Resize_Left;
         }
         else if ( boxX2 == pixx )
         {
            m_resizeMode = Resize_Right;
         }
         else if ( boxY1 == pixy )
         {
            m_resizeMode = Resize_Top;
         }
         else if ( boxY2 == pixy )
         {
            m_resizeMode = Resize_Bottom;
         }
         else
         {
            m_resizeMode = Resize_None;
         }

         // Put the selected image portion back on the table if we're
         // resizing the selection.
         if ( m_resizeMode != Resize_None )
         {
            paintOverlay(Overlay_PasteSelection,0,m_selectionRect.left(),m_selectionRect.top(),m_selectionRect.right(),m_selectionRect.bottom());
            copyOverlayToNormal();
         }
      }
      else
      {
         m_resizeMode = Resize_None;
         m_anchor = QPoint(pixx,pixy);
         m_selectionRect = QRect(QPoint(m_anchor.x(),m_anchor.y()),QPoint(pixx+1,pixy+1));
         renderer->setBox(m_anchor.x(),m_anchor.y(),pixx+1,pixy+1);
         renderer->update();
      }
      break;
   case QEvent::MouseMove:
      if ( m_selection )
      {
         if ( (boxX1 == pixx) && (boxY1 == pixy) )
         {
            renderer->setCursor(QCursor(Qt::SizeFDiagCursor));
         }
         else if ( (boxX2 == pixx) && (boxY1 == pixy) )
         {
            renderer->setCursor(QCursor(Qt::SizeBDiagCursor));
         }
         else if ( (boxX1 == pixx) && (boxY2 == pixy) )
         {
            renderer->setCursor(QCursor(Qt::SizeBDiagCursor));
         }
         else if ( (boxX2 == pixx) && (boxY2 == pixy) )
         {
            renderer->setCursor(QCursor(Qt::SizeFDiagCursor));
         }
         else if ( boxX1 == pixx )
         {
            renderer->setCursor(QCursor(Qt::SizeHorCursor));
         }
         else if ( boxX2 == pixx )
         {
            renderer->setCursor(QCursor(Qt::SizeHorCursor));
         }
         else if ( boxY1 == pixy )
         {
            renderer->setCursor(QCursor(Qt::SizeVerCursor));
         }
         else if ( boxY2 == pixy )
         {
            renderer->setCursor(QCursor(Qt::SizeVerCursor));
         }
         else if ( m_selectionRect.contains(pixx,pixy,true) )
         {
            renderer->setCursor(QCursor(Qt::SizeAllCursor));
         }
         else
         {
            renderer->setCursor(QCursor(Qt::CrossCursor));
         }
      }
      else
      {
         renderer->setCursor(QCursor(Qt::CrossCursor));
      }
      if ( event->buttons() == Qt::LeftButton )
      {
         if ( m_selection )
         {
            if ( m_resizeMode != Resize_None )
            {
               switch ( m_resizeMode )
               {
               case Resize_TopLeft:
                  m_selectionRect.setTopLeft(QPoint(pixx,pixy));
                  m_selectionRect = m_selectionRect.normalized();
                  break;
               case Resize_TopRight:
                  m_selectionRect.setTopRight(QPoint(pixx,pixy));
                  m_selectionRect = m_selectionRect.normalized();
                  break;
               case Resize_BottomLeft:
                  m_selectionRect.setBottomLeft(QPoint(pixx,pixy));
                  m_selectionRect = m_selectionRect.normalized();
                  break;
               case Resize_BottomRight:
                  m_selectionRect.setBottomRight(QPoint(pixx,pixy));
                  m_selectionRect = m_selectionRect.normalized();
                  break;
               case Resize_Left:
                  m_selectionRect.setLeft(pixx);
                  m_selectionRect = m_selectionRect.normalized();
                  break;
               case Resize_Right:
                  m_selectionRect.setRight(pixx);
                  m_selectionRect = m_selectionRect.normalized();
                  break;
               case Resize_Top:
                  m_selectionRect.setTop(pixy);
                  m_selectionRect = m_selectionRect.normalized();
                  break;
               case Resize_Bottom:
                  m_selectionRect.setBottom(pixy);
                  m_selectionRect = m_selectionRect.normalized();
                  break;
               default:
                  // Do nothing.
                  break;
               }
               renderer->setBox(m_selectionRect.left(),m_selectionRect.top(),m_selectionRect.right(),m_selectionRect.bottom());
               renderer->update();
            }
            else
            {
               m_selectionRect = m_selectionRect.translated(pixx-m_anchor.x(),pixy-m_anchor.y());
               m_selectionRect = m_selectionRect.normalized();
               paintOverlay(Overlay_PasteSelection,0,m_selectionRect.left(),m_selectionRect.top(),m_selectionRect.right(),m_selectionRect.bottom());
               m_anchor = QPoint(pixx,pixy);
               renderer->setBox(m_selectionRect.left(),m_selectionRect.top(),m_selectionRect.right(),m_selectionRect.bottom());
               renderer->update();
               previewer->update();
            }
         }
         else
         {
            m_selectionRect = QRect(QPoint(m_anchor.x(),m_anchor.y()),QPoint(pixx+1,pixy+1));
            renderer->setBox(m_anchor.x(),m_anchor.y(),pixx+1,pixy+1);
            renderer->update();
         }
      }
      break;
   case QEvent::MouseButtonRelease:
      if ( m_selection )
      {
         if ( m_resizeMode == Resize_None )
         {
            paintOverlay(Overlay_PasteSelection,0,m_selectionRect.left(),m_selectionRect.top(),m_selectionRect.right(),m_selectionRect.bottom());
            renderer->update();
            previewer->update();
         }
         else
         {
            copyNormalToSelection(m_selectionRect.left(),m_selectionRect.top(),m_selectionRect.right(),m_selectionRect.bottom());
            paintOverlay(Overlay_Erase,0,m_selectionRect.left(),m_selectionRect.top(),m_selectionRect.right(),m_selectionRect.bottom());
            copyOverlayToNormal();
            paintOverlay(Overlay_PasteSelection,0,m_selectionRect.left(),m_selectionRect.top(),m_selectionRect.right(),m_selectionRect.bottom());
            m_selectionCaptured = true;
         }
      }
      else
      {
         m_selectionRect = QRect(QPoint(boxX1,boxY1),QPoint(boxX2,boxY2));
         m_selectionRect = m_selectionRect.normalized();
         renderer->setBox(m_selectionRect.left(),m_selectionRect.top(),m_selectionRect.right(),m_selectionRect.bottom());
         renderer->update();
         m_oldTileData = tileData();
         m_oldAttributeData = attributeData();
         copyNormalToSelection(m_selectionRect.left(),m_selectionRect.top(),m_selectionRect.right(),m_selectionRect.bottom());
         paintOverlay(Overlay_Erase,0,m_selectionRect.left(),m_selectionRect.top(),m_selectionRect.right(),m_selectionRect.bottom());
         copyOverlayToNormal();
         paintOverlay(Overlay_PasteSelection,0,m_selectionRect.left(),m_selectionRect.top(),m_selectionRect.right(),m_selectionRect.bottom());
         m_selectionCaptured = true;
         m_selection = true;
      }
      break;
   default:
      // Stupid warnings.
      break;
   }
}

void TileStampEditorForm::paintTool(QMouseEvent *event)
{
   int pixx;
   int pixy;
   int selectedColor = -1;
   QByteArray oldTileData;
   QByteArray oldAttributeData;

   renderer->pointToPixel(event->pos().x(),event->pos().y(),&pixx,&pixy);

   // Find the selected color.
   selectedColor = getSelectedColor();

   if ( selectedColor != -1 )
   {
      switch ( event->type() )
      {
      case QEvent::MouseButtonPress:
         if ( event->button() == Qt::LeftButton )
         {
            paintOverlay(Overlay_FloodFill,selectedColor,pixx,pixy,0,0);
            renderer->update();
            previewer->update();
         }
         else if ( event->button() == Qt::RightButton )
         {
            paintOverlay(Overlay_FloodFill,0,pixx,pixy,0,0);
            renderer->update();
            previewer->update();
         }
         break;
      case QEvent::MouseMove:
         break;
      case QEvent::MouseButtonRelease:
         oldTileData = tileData();
         oldAttributeData = attributeData();
         copyOverlayToNormal();
         paintNormal();
         m_undoStack.push(new TileStampPaintCommand(this,oldTileData,oldAttributeData));
         renderer->update();
         previewer->update();
         setModified(true);
         emit markProjectDirty(true);
         break;
      default:
         // Stupid warnings.
         break;
      }
   }
}

void TileStampEditorForm::pencilTool(QMouseEvent *event)
{
   int selectedColor = -1;
   int pixx;
   int pixy;
   int boxX1;
   int boxY1;
   int boxX2;
   int boxY2;
   QByteArray oldTileData;
   QByteArray oldAttributeData;

   renderer->pointToPixel(event->pos().x(),event->pos().y(),&pixx,&pixy);

   // Find the selected color.
   selectedColor = getSelectedColor();

   boxX1 = pixx-(pixx%16);
   boxY1 = pixy-(pixy%16);
   boxX2 = boxX1+16;
   boxY2 = boxY1+16;
   renderer->setBox(boxX1,boxY1,boxX2,boxY2);
   renderer->update();

   if ( selectedColor >= 0 )
   {
      switch ( event->type() )
      {
      case QEvent::MouseButtonPress:
         m_anchor = QPoint(pixx,pixy);
         if ( event->button() == Qt::LeftButton )
         {
            if ( ui->paintAttr->isChecked() )
            {
               recolorTiles(pixx,pixy,selectedColor);
            }
            else
            {
               if ( (selectedColor%4) && (colorData[(pixy*256)+pixx] != selectedColor) )
               {
                  recolorTiles(pixx,pixy,selectedColor);
               }
               paintOverlay(selectedColor,pixx,pixy);
            }
            renderer->update();
            previewer->update();
         }
         else if ( event->button() == Qt::RightButton )
         {
            paintOverlay(0,pixx,pixy);
            renderer->update();
            previewer->update();
         }
         break;
      case QEvent::MouseMove:
         boxX1 = pixx-(pixx%16);
         boxY1 = pixy-(pixy%16);
         boxX2 = boxX1+16;
         boxY2 = boxY1+16;
         renderer->setBox(boxX1,boxY1,boxX2,boxY2);
         renderer->update();

         if ( event->buttons() == Qt::LeftButton )
         {
            if ( ui->paintAttr->isChecked() )
            {
               recolorTiles(pixx,pixy,selectedColor);
            }
            else
            {
               if ( (selectedColor%4) && (colorData[(pixy*256)+pixx] != selectedColor) )
               {
                  recolorTiles(pixx,pixy,selectedColor);
               }
               paintOverlay(selectedColor,pixx,pixy);
            }
            renderer->update();
            previewer->update();
         }
         else if ( event->buttons() == Qt::RightButton )
         {
            paintOverlay(0,pixx,pixy);
            renderer->update();
            previewer->update();
         }
         m_anchor = QPoint(pixx,pixy);
         break;
      case QEvent::MouseButtonRelease:
         oldTileData = tileData();
         oldAttributeData = attributeData();
         copyOverlayToNormal();
         paintNormal();
         m_undoStack.push(new TileStampPaintCommand(this,oldTileData,oldAttributeData));
         renderer->update();
         previewer->update();
         setModified(true);
         emit markProjectDirty(true);
         break;
      default:
         // Stupid warnings.
         break;
      }
   }
}

void TileStampEditorForm::filledCircleTool(QMouseEvent *event)
{
   boxTool(event,Overlay_FilledCircle);
}

void TileStampEditorForm::hollowCircleTool(QMouseEvent *event)
{
   boxTool(event,Overlay_HollowCircle);
}

void TileStampEditorForm::filledBoxTool(QMouseEvent *event)
{
   boxTool(event,Overlay_FilledBox);
}

void TileStampEditorForm::hollowBoxTool(QMouseEvent *event)
{
   boxTool(event,Overlay_HollowBox);
}

void TileStampEditorForm::boxTool(QMouseEvent *event,OverlayType overlayType)
{
   int selectedColor = -1;
   int pixx;
   int pixy;
   QByteArray oldTileData;
   QByteArray oldAttributeData;

   renderer->pointToPixel(event->pos().x(),event->pos().y(),&pixx,&pixy);

   // Find the selected color.
   selectedColor = getSelectedColor();

   if ( selectedColor >= 0 )
   {
      switch ( event->type() )
      {
      case QEvent::MouseButtonPress:
         m_anchor = QPoint(pixx,pixy);
         if ( event->button() == Qt::LeftButton )
         {
            paintOverlay(overlayType,selectedColor,m_anchor.x(),m_anchor.y(),pixx+1,pixy+1);
            renderer->setBox(m_anchor.x(),m_anchor.y(),pixx+1,pixy+1);
            renderer->update();
            previewer->update();
         }
         else if ( event->button() == Qt::RightButton )
         {
            paintOverlay(overlayType,0,m_anchor.x(),m_anchor.y(),pixx+1,pixy+1);
            renderer->setBox(m_anchor.x(),m_anchor.y(),pixx+1,pixy+1);
            renderer->update();
            previewer->update();
         }
         break;
      case QEvent::MouseMove:
         if ( event->buttons() == Qt::LeftButton )
         {
            paintOverlay(overlayType,selectedColor,m_anchor.x(),m_anchor.y(),pixx+1,pixy+1);
            renderer->setBox(m_anchor.x(),m_anchor.y(),pixx+1,pixy+1);
            renderer->update();
            previewer->update();
         }
         else if ( event->buttons() == Qt::RightButton )
         {
            paintOverlay(overlayType,0,m_anchor.x(),m_anchor.y(),pixx+1,pixy+1);
            renderer->setBox(m_anchor.x(),m_anchor.y(),pixx+1,pixy+1);
            renderer->update();
            previewer->update();
         }
         break;
      case QEvent::MouseButtonRelease:
         oldTileData = tileData();
         oldAttributeData = attributeData();
         copyOverlayToNormal();
         paintNormal();
         m_undoStack.push(new TileStampPaintCommand(this,oldTileData,oldAttributeData));
         renderer->setBox();
         renderer->update();
         previewer->update();
         setModified(true);
         emit markProjectDirty(true);
         break;
      default:
         // Stupid warnings.
         break;
      }
   }
}

void TileStampEditorForm::lineTool(QMouseEvent *event)
{
   int selectedColor = -1;
   int pixx;
   int pixy;
   QByteArray oldTileData;
   QByteArray oldAttributeData;

   renderer->pointToPixel(event->pos().x(),event->pos().y(),&pixx,&pixy);

   // Find the selected color.
   selectedColor = getSelectedColor();

   if ( selectedColor >= 0 )
   {
      switch ( event->type() )
      {
      case QEvent::MouseButtonPress:
         // Set anchor.
         m_anchor = QPoint(pixx,pixy);
         if ( event->button() == Qt::LeftButton )
         {
            paintOverlay(Overlay_Line,selectedColor,m_anchor.x(),m_anchor.y(),pixx+1,pixy+1);
            renderer->update();
            previewer->update();
         }
         else if ( event->button() == Qt::RightButton )
         {
            paintOverlay(Overlay_Line,0,m_anchor.x(),m_anchor.y(),pixx+1,pixy+1);
            renderer->update();
            previewer->update();
         }
         break;
      case QEvent::MouseMove:
         if ( event->buttons() == Qt::LeftButton )
         {
            paintOverlay(Overlay_Line,selectedColor,m_anchor.x(),m_anchor.y(),pixx+1,pixy+1);
            renderer->update();
            previewer->update();
         }
         else if ( event->buttons() == Qt::RightButton )
         {
            paintOverlay(Overlay_Line,0,m_anchor.x(),m_anchor.y(),pixx+1,pixy+1);
            renderer->update();
            previewer->update();
         }
         break;
      case QEvent::MouseButtonRelease:
         oldTileData = tileData();
         oldAttributeData = attributeData();
         copyOverlayToNormal();
         paintNormal();
         m_undoStack.push(new TileStampPaintCommand(this,oldTileData,oldAttributeData));
         renderer->update();
         previewer->update();
         setModified(true);
         emit markProjectDirty(true);
         break;
      default:
         // Stupid warnings.
         break;
      }
   }
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
   IChrRomBankItem* pChrItem = reinterpret_cast<IChrRomBankItem*>(ui->tileTabWidget->currentIndex().internalPointer());
   CTileStamp* pSelectedTile = dynamic_cast<CTileStamp*>(pChrItem);
   CBinaryFile* pSelectedBinary = dynamic_cast<CBinaryFile*>(pChrItem);
   int xSize;
   int ySize;
   QByteArray oldTileData;
   QByteArray oldAttributeData;

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
         renderer->update();
         previewer->update();
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
         renderer->update();
         previewer->update();
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
               renderer->update();
               previewer->update();
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
               renderer->update();
               previewer->update();
            }
         }
      }
      else
      {
         renderer->setBox();
         renderer->update();
      }
      break;
   case QEvent::MouseButtonRelease:
      oldTileData = tileData();
      oldAttributeData = attributeData();
      copyOverlayToNormal();
      paintNormal();
      m_undoStack.push(new TileStampPaintCommand(this,oldTileData,oldAttributeData));
      renderer->setBox();
      renderer->update();
      previewer->update();
      setModified(true);
      emit markProjectDirty(true);
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
      if ( m_selection || !(m_selectionRect.isEmpty()) )
      {
         str.sprintf("Cursor:Pixel(%d,%d) Tile(%d,%d) AttrQuad(%d,%d %s) Selection:(%d,%d)-(%d,%d) %d x %d",
                     x,y,
                     PIXEL_TO_TILE(x),PIXEL_TO_TILE(y),
                     PIXEL_TO_ATTRQUAD(x),PIXEL_TO_ATTRQUAD(y),
                     attrSection[PIXEL_TO_ATTRSECTION(x,y)],
                     m_selectionRect.left(),
                     m_selectionRect.top(),
                     m_selectionRect.right(),
                     m_selectionRect.bottom(),
                     m_selectionRect.width()-1,
                     m_selectionRect.height()-1);
      }
      else
      {
         str.sprintf("Cursor:Pixel(%d,%d) Tile(%d,%d) AttrQuad(%d,%d %s)",
                     x,y,
                     PIXEL_TO_TILE(x),PIXEL_TO_TILE(y),
                     PIXEL_TO_ATTRQUAD(x),PIXEL_TO_ATTRQUAD(y),
                     attrSection[PIXEL_TO_ATTRSECTION(x,y)]);
      }
      ui->info->setText(str);
   }
   else
   {
      ui->info->clear();
   }
}

int TileStampEditorForm::getSelectedColor()
{
   int selectedColor = -1;
   int idx;

   for ( idx = 0; idx < m_colors.count(); idx++ )
   {
      if ( m_colors.at(idx)->isChecked() )
      {
         selectedColor = idx;
         break;
      }
   }
   return selectedColor;
}

void TileStampEditorForm::tilePropertyListModel_dataChanged(QModelIndex /*topLeft*/,QModelIndex /*bottomRight*/)
{
   m_tileProperties = tilePropertyListModel->getItems();
   setModified(true);
   emit markProjectDirty(true);
}

TileStampPaintCommand::TileStampPaintCommand(TileStampEditorForm *pEditor,
                                             QByteArray oldTileData,
                                             QByteArray oldAttributeData,
                                             QUndoCommand *parent)
   : QUndoCommand(parent),
     m_pEditor(pEditor),
     m_oldTileData(oldTileData),
     m_oldAttributeData(oldAttributeData)
{
   setText("painting");
   m_newTileData = m_pEditor->tileData();
   m_newAttributeData = m_pEditor->attributeData();
}

bool TileStampPaintCommand::mergeWith(const QUndoCommand* /*command*/)
{
   return false;
}

void TileStampPaintCommand::redo()
{
   m_pEditor->initializeTile(m_newTileData,m_newAttributeData);
   m_pEditor->paintNormal();
}

void TileStampPaintCommand::undo()
{
   m_pEditor->initializeTile(m_oldTileData,m_oldAttributeData);
   m_pEditor->paintNormal();
}

TileStampResizeCommand::TileStampResizeCommand(TileStampEditorForm *pEditor,
                                             int oldXSize,
                                             int oldYSize,
                                             QUndoCommand *parent)
   : QUndoCommand(parent),
     m_pEditor(pEditor),
     m_oldXSize(oldXSize),
     m_oldYSize(oldYSize)
{
   setText("resize");
   m_pEditor->currentSize(&m_newXSize,&m_newYSize);
}

bool TileStampResizeCommand::mergeWith(const QUndoCommand* /*command*/)
{
   return false;
}

void TileStampResizeCommand::redo()
{
   m_pEditor->setCurrentSize(m_newXSize,m_newYSize);
}

void TileStampResizeCommand::undo()
{
   m_pEditor->setCurrentSize(m_oldXSize,m_oldYSize);
}

void TileStampEditorForm::propertyTableView_currentChanged(QModelIndex index,QModelIndex)
{
   if ( index.isValid() && (index.row() < m_tileProperties.count()) )
   {
      tilePropertyValueDelegate->setItem(m_tileProperties.at(index.row()));
   }
}
