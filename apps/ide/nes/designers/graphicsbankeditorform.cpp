#include "graphicsbankeditorform.h"
#include "ui_graphicsbankeditorform.h"

#include <QMessageBox>

#include "cdesignercommon.h"
#include "ichrrombankitem.h"

#include "nes_emulator_core.h"
#include "cnessystempalette.h"

GraphicsBankEditorForm::GraphicsBankEditorForm(uint32_t size, QList<IChrRomBankItem*> bankItems,IProjectTreeViewItem* link,QWidget* parent) :
   CDesignerEditorBase(link,parent),
   ui(new Ui::GraphicsBankEditorForm)
{
   int i;

   ui->setupUi(this);

   info = new QLabel(this);

   pThread = new TilificationThread();
   QObject::connect(this,SIGNAL(prepareToTilify()),pThread,SLOT(prepareToTilify()));
   QObject::connect(this,SIGNAL(addToTilificator(IChrRomBankItem*)),pThread,SLOT(addToTilificator(IChrRomBankItem*)));
   QObject::connect(this,SIGNAL(tilify()),pThread,SLOT(tilify()));
   QObject::connect(pThread,SIGNAL(tilificationComplete(QByteArray)),this,SLOT(renderData(QByteArray)));

   imgData = new int8_t[256*256*4];

   // Clear image...
   for ( i = 0; i < 256*256*4; i+=4 )
   {
      imgData[i] = 0;
      imgData[i+1] = 0;
      imgData[i+2] = 0;
      imgData[i+3] = 0xFF;
   }

   renderer = new PanZoomRenderer(256,128,2000,imgData,true,ui->frame);
   ui->frame->layout()->addWidget(renderer);
   ui->frame->update();
   setCentralWidget(ui->window);

   QObject::connect(renderer,SIGNAL(repaintNeeded()),this,SLOT(renderData()));

   model = new CChrRomItemTableDisplayModel(true);

   delegate = new CChrRomBankItemDelegate();

   ui->tableView->setModel(model);
   ui->tableView->setItemDelegateForColumn(ChrRomBankItemCol_Name,delegate);
   ui->tableView->setColumnWidth(ChrRomBankItemCol_Name,400);

   ui->gauge->setMaximum(size);

   updateChrRomBankItemList(bankItems);

   // Get mouse events from the renderer here!
   renderer->installEventFilter(this);

   ui->bankSize->setItemData(0,MEM_8KB);
   if ( size == MEM_8KB ) ui->bankSize->setCurrentIndex(0);
   ui->bankSize->setItemData(1,MEM_4KB);
   if ( size == MEM_4KB ) ui->bankSize->setCurrentIndex(1);
   ui->bankSize->setItemData(2,MEM_2KB);
   if ( size == MEM_2KB ) ui->bankSize->setCurrentIndex(2);
   ui->bankSize->setItemData(3,MEM_1KB);
   if ( size == MEM_1KB ) ui->bankSize->setCurrentIndex(3);

   QObject::connect(model,SIGNAL(rowsInserted(QModelIndex,int,int)),this,SLOT(updateList()));
   QObject::connect(model,SIGNAL(layoutChanged()),this,SLOT(updateList()));
   QObject::connect(ui->tableView,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(tableView_doubleClicked(QModelIndex)));
}

GraphicsBankEditorForm::~GraphicsBankEditorForm()
{
   if ( info->parent() == this )
   {
      delete info;
   }
   delete pThread;
   delete ui;
   delete model;
   delete imgData;
   delete renderer;
   delete delegate;
}

QList<IChrRomBankItem*> GraphicsBankEditorForm::bankItems()
{
   return model->bankItems();
}

uint32_t GraphicsBankEditorForm::bankSize()
{
   return ui->bankSize->itemData(ui->bankSize->currentIndex()).toInt();
}

bool GraphicsBankEditorForm::eventFilter(QObject* obj,QEvent* event)
{
   if ( obj == renderer )
   {
      if ( event->type() == QEvent::MouseMove )
      {
         QMouseEvent* mouseEvent = dynamic_cast<QMouseEvent*>(event);
         renderer_mouseMoveEvent(mouseEvent);
      }
      else if ( event->type() == QEvent::Enter )
      {
         QEvent* enterEvent = dynamic_cast<QEvent*>(event);
         renderer_enterEvent(enterEvent);
      }
      else if ( event->type() == QEvent::Leave )
      {
         QEvent* leaveEvent = dynamic_cast<QEvent*>(event);
         renderer_leaveEvent(leaveEvent);
      }
   }
   return false;
}

void GraphicsBankEditorForm::tableView_doubleClicked(QModelIndex index)
{
   if ( (index.row() >= 0) && (index.row() < bankItems().count()) )
   {
      IChrRomBankItem* pChrItem = bankItems().at(index.row());
      IProjectTreeViewItem* pProjItem = dynamic_cast<IProjectTreeViewItem*>(pChrItem);

      emit snapToTab("Tile,"+pProjItem->uuid());
   }
}

void GraphicsBankEditorForm::renderer_enterEvent(QEvent */*event*/)
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

void GraphicsBankEditorForm::renderer_leaveEvent(QEvent */*event*/)
{
   updateInfoText();
}

void GraphicsBankEditorForm::renderer_mouseMoveEvent(QMouseEvent */*event*/)
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

void GraphicsBankEditorForm::updateList()
{
   updateUi();
   setModified(true);
   emit markProjectDirty(true);
}

void GraphicsBankEditorForm::updateUi()
{
   int idx;

   ui->tableView->resizeRowsToContents();

   ui->gauge->setMaximum(ui->bankSize->itemData(ui->bankSize->currentIndex()).toInt());

   emit prepareToTilify();

   for (idx = 0; idx < model->bankItems().count(); idx++ )
   {
      emit addToTilificator(model->bankItems().at(idx));
   }

   emit tilify();
}

void GraphicsBankEditorForm::changeEvent(QEvent* event)
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

void GraphicsBankEditorForm::keyPressEvent(QKeyEvent *event)
{
   int idx;

   if ( event->key() == Qt::Key_Delete || event->key() == Qt::Key_Backspace )
   {
      if ( ui->tableView->hasFocus() )
      {
         QModelIndex index = ui->tableView->currentIndex();

         if ( index.isValid() )
         {
            model->removeRow(index.row(),QModelIndex());

            emit prepareToTilify();

            for (idx = 0; idx < model->bankItems().count(); idx++ )
            {
               emit addToTilificator(model->bankItems().at(idx));
            }

            emit tilify();

            setModified(true);
            emit markProjectDirty(true);
         }
      }
   }
   else if ( (event->key() == Qt::Key_F) &&
             (event->modifiers() == Qt::ControlModifier) )
   {
      emit activateSearchBar("");
   }
   CDesignerEditorBase::keyPressEvent(event);
}

void GraphicsBankEditorForm::updateChrRomBankItemList(QList<IChrRomBankItem*> bankItems)
{
   int idx;

   if ( bankItems != model->bankItems() )
   {
      model->setBankItems(bankItems);
      model->update();
      ui->tableView->resizeRowsToContents();

      emit prepareToTilify();

      for (idx = 0; idx < bankItems.count(); idx++ )
      {
         emit addToTilificator(bankItems.at(idx));
      }

      emit tilify();
   }
}

void GraphicsBankEditorForm::renderData(QByteArray output)
{
   int idx;
   int max = ui->bankSize->itemData(ui->bankSize->currentIndex()).toInt();

   ui->gauge->setValue(output.count());
   QPalette p = ui->gauge->palette();
   if ( output.count() > max )
   {
      ui->gauge->setValue(max);
      ui->gauge->setStyleSheet("QProgressBar::chunk {background: #ff0000;border-top-right-radius: 5px;border-top-left-radius: 5px;border-bottom-right-radius: 5px;border-bottom-left-radius: 5px;border: .px solid black;}");
   }
   else
   {
      ui->gauge->setStyleSheet("QProgressBar::chunk {background: #00ff00;border-top-right-radius: 5px;border-top-left-radius: 5px;border-bottom-right-radius: 5px;border-bottom-left-radius: 5px;border: .px solid black;}");
   }
   ui->gauge->setPalette(p);

   // Pad to boundary-KB.
   for ( idx = output.count(); idx < max; idx++ )
   {
      output.append((char)0);
   }

   tilifiedData.replace(0,max,output);

   renderData();
}

void GraphicsBankEditorForm::renderData()
{
   unsigned int ppuAddr = 0x0000;
   unsigned char patternData1;
   unsigned char patternData2;
   unsigned char bit1, bit2;
   unsigned char colorIdx;
   QColor color[4];
   IChrRomBankItem* item;
   int x, y, maxx, maxy;
   int max = ui->bankSize->itemData(ui->bankSize->currentIndex()).toInt();

   color[0] = renderer->getColor(0);
   color[1] = renderer->getColor(1);
   color[2] = renderer->getColor(2);
   color[3] = renderer->getColor(3);

   maxx = (max==MEM_8KB)?256:128;
   maxy = (max>=MEM_4KB)?128:(max==MEM_2KB)?64:32;

   for (y = 0; y < 128; y++)
   {
      for (x = 0; x < 256; x += 8)
      {
         if ( (y >= maxy) || (x >= maxx) )
         {
            for ( int xf = 0; xf < 8; xf++ )
            {
               imgData[((y<<8)<<2) + (x<<2) + (xf<<2) + 0] = 0xaa;
               imgData[((y<<8)<<2) + (x<<2) + (xf<<2) + 1] = 0xaa;
               imgData[((y<<8)<<2) + (x<<2) + (xf<<2) + 2] = 0xaa;
            }
            continue;
         }
         ppuAddr = ((y>>3)<<8)+((x&0x7F)<<1)+(y&0x7);

         if ( x >= 128 )
         {
            ppuAddr += 0x1000;
         }

         if ( ppuAddr < tilifiedData.count() )
         {
            patternData1 = tilifiedData.at(ppuAddr);
            patternData2 = tilifiedData.at((ppuAddr)+8);

            for ( int xf = 0; xf < 8; xf++ )
            {
               bit1 = (patternData1>>(7-(xf)))&0x1;
               bit2 = (patternData2>>(7-(xf)))&0x1;
               colorIdx = (bit1|(bit2<<1));
               imgData[((y<<8)<<2) + (x<<2) + (xf<<2) + 0] = color[colorIdx].red();
               imgData[((y<<8)<<2) + (x<<2) + (xf<<2) + 1] = color[colorIdx].green();
               imgData[((y<<8)<<2) + (x<<2) + (xf<<2) + 2] = color[colorIdx].blue();
            }
         }
         else
         {
            for ( int xf = 0; xf < 8; xf++ )
            {
               imgData[((y<<8)<<2) + (x<<2) + (xf<<2) + 0] = 0x00;
               imgData[((y<<8)<<2) + (x<<2) + (xf<<2) + 1] = 0x00;
               imgData[((y<<8)<<2) + (x<<2) + (xf<<2) + 2] = 0x00;
            }
         }
      }
   }

   renderer->reloadData(imgData);
}

void GraphicsBankEditorForm::itemRemoved(QUuid uuid)
{
   QList<IChrRomBankItem*> newItems = bankItems();

   foreach ( IChrRomBankItem* pChrItem, bankItems() )
   {
      IProjectTreeViewItem* pProjItem = dynamic_cast<IProjectTreeViewItem*>(pChrItem);

      if ( pProjItem->uuid() == uuid.toString() )
      {
         newItems.removeAll(pChrItem);
      }
   }

   if ( newItems != bankItems() )
   {
      model->setBankItems(newItems);
      model->update();
      setModified(true);
      emit markProjectDirty(true);
   }
}

void GraphicsBankEditorForm::snapTo(QString item)
{
   QStringList splits;
   QModelIndexList list;

   // Make sure item is something we care about
   if ( item.startsWith("SearchBar,") )
   {
      if ( isVisible() )
      {
         splits = item.split(QRegExp("[,]"));
         list = model->match(model->index(ui->tableView->currentIndex().row()+1,ChrRomBankItemCol_Name),Qt::DisplayRole,splits.at(4),1,Qt::MatchFixedString|Qt::MatchContains|Qt::MatchWrap);
         if ( list.count() )
         {
            ui->tableView->setCurrentIndex(list.at(0));
         }
      }
   }
}

void GraphicsBankEditorForm::showEvent(QShowEvent */*event*/)
{
   emit addStatusBarWidget(info);
   info->show();
}

void GraphicsBankEditorForm::hideEvent(QHideEvent */*event*/)
{
   emit removeStatusBarWidget(info);
}

void GraphicsBankEditorForm::updateInfoText(int x, int y)
{
   int tileX;
   int tileY;
   int side;
   const char* sideStr[] = { "LEFT", "RIGHT" };

   if ( (x >= 0) && (y >= 0) )
   {
      QString str;

      tileX = PIXEL_TO_TILE(x);
      tileY = PIXEL_TO_TILE(y);

      if ( tileX >= 16 )
      {
         side = 1;
         tileX -= 16;
      }
      else
      {
         side = 0;
      }

      str.sprintf("Cursor:Pixel(%d,%d) Tile(%d,%d) %s",
                  x,y,
                  tileX,tileY,
                  sideStr[side]);

      info->setText(str);
   }
   else
   {
      info->clear();
   }
}

void GraphicsBankEditorForm::applyChangesToTab(QString /*uuid*/)
{
   updateChrRomBankItemList(bankItems());
}

void GraphicsBankEditorForm::applyProjectPropertiesToTab()
{
}

void GraphicsBankEditorForm::on_moveUp_clicked()
{
   QModelIndexList indexes = ui->tableView->selectionModel()->selectedIndexes();
   QList<IChrRomBankItem*> items = model->bankItems();
   if ( !indexes.isEmpty() )
   {
      foreach ( QModelIndex index, indexes )
      {
         if ( index.column() == 0 )
         {
            if ( index.row() > 0 )
            {
               items.swap(index.row()-1,index.row());
            }
            else
            {
               break;
            }
         }
      }
      model->setBankItems(items);
      model->update();
      setModified(true);
      emit markProjectDirty(true);
   }
}

void GraphicsBankEditorForm::on_moveDown_clicked()
{
   QModelIndexList indexes = ui->tableView->selectionModel()->selectedIndexes();
   QList<IChrRomBankItem*> items = model->bankItems();
   if ( !indexes.isEmpty() )
   {
      foreach ( QModelIndex index, indexes )
      {
         if ( index.column() == 0 )
         {
            if ( index.row() < (items.count()-1) )
            {
               items.swap(index.row()+1,index.row());
            }
            else
            {
               break;
            }
         }
      }
      model->setBankItems(items);
      model->update();
      setModified(true);
      emit markProjectDirty(true);
   }
}

void GraphicsBankEditorForm::on_bankSize_currentIndexChanged(int index)
{
   updateUi();
   setModified(true);
   emit markProjectDirty(true);
}
