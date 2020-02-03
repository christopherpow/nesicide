#include <QMessageBox>

#include "graphicsbankeditorform.h"
#include "ui_graphicsbankeditorform.h"

#include "cdesignercommon.h"

#include "nes_emulator_core.h"
#include "cnessystempalette.h"

GraphicsBankEditorForm::GraphicsBankEditorForm(QList<IChrRomBankItem*> bankItems,IProjectTreeViewItem* link,QWidget* parent) :
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

   ui->gauge->setMaximum(MEM_8KB);

   updateChrRomBankItemList(bankItems);

   // Get mouse events from the renderer here!
   renderer->installEventFilter(this);

   QObject::connect(model,SIGNAL(rowsInserted(QModelIndex,int,int)),this,SLOT(updateUi()));
   QObject::connect(model,SIGNAL(layoutChanged()),this,SLOT(updateUi()));
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

void GraphicsBankEditorForm::updateUi()
{
   int idx;

   ui->tableView->resizeRowsToContents();

   emit prepareToTilify();

   for (idx = 0; idx < model->bankItems().count(); idx++ )
   {
      emit addToTilificator(model->bankItems().at(idx));
   }

   emit tilify();

   setModified(true);
   emit markProjectDirty(true);
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

   if ( event->key() == Qt::Key_Delete )
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

void GraphicsBankEditorForm::renderData(QByteArray output)
{
   int idx;

   ui->gauge->setValue(output.count());

   // Pad to 8KB.
   for ( idx = output.count(); idx < MEM_8KB; idx++ )
   {
      output.append((char)0);
   }

   tilifiedData.replace(0,MEM_8KB,output);

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

   color[0] = renderer->getColor(0);
   color[1] = renderer->getColor(1);
   color[2] = renderer->getColor(2);
   color[3] = renderer->getColor(3);

   for (int y = 0; y < 128; y++)
   {
      for (int x = 0; x < 256; x += 8)
      {
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
