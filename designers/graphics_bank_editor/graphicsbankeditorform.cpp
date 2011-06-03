#include "graphicsbankeditorform.h"
#include "ui_graphicsbankeditorform.h"

#include "emulator_core.h"
#include "cnessystempalette.h"

GraphicsBankEditorForm::GraphicsBankEditorForm(QList<IChrRomBankItem*> bankItems,IProjectTreeViewItem* link,QWidget* parent) :
   CDesignerEditorBase(link,parent),
   ui(new Ui::GraphicsBankEditorForm)
{
   int i;

   ui->setupUi(this);

   imgData = new char[256*256*4];

   // Clear image...
   for ( i = 0; i < 256*256*4; i+=4 )
   {
      imgData[i] = 0;
      imgData[i+1] = 0;
      imgData[i+2] = 0;
      imgData[i+3] = 0xFF;
   }

   for (int i=0; i<NUM_PALETTES; i++)
   {
      ui->col0PushButton->insertColor(QColor(nesGetPaletteRedComponent(i),nesGetPaletteGreenComponent(i),nesGetPaletteBlueComponent(i)), "", i);
      ui->col1PushButton->insertColor(QColor(nesGetPaletteRedComponent(i),nesGetPaletteGreenComponent(i),nesGetPaletteBlueComponent(i)), "", i);
      ui->col2PushButton->insertColor(QColor(nesGetPaletteRedComponent(i),nesGetPaletteGreenComponent(i),nesGetPaletteBlueComponent(i)), "", i);
      ui->col3PushButton->insertColor(QColor(nesGetPaletteRedComponent(i),nesGetPaletteGreenComponent(i),nesGetPaletteBlueComponent(i)), "", i);
   }

   ui->col0PushButton->setCurrentColor(QColor(nesGetPaletteRedComponent(0x0D),nesGetPaletteGreenComponent(0x0D),nesGetPaletteBlueComponent(0x0D)));
   ui->col1PushButton->setCurrentColor(QColor(nesGetPaletteRedComponent(0x00),nesGetPaletteGreenComponent(0x00),nesGetPaletteBlueComponent(0x00)));
   ui->col2PushButton->setCurrentColor(QColor(nesGetPaletteRedComponent(0x10),nesGetPaletteGreenComponent(0x10),nesGetPaletteBlueComponent(0x10)));
   ui->col3PushButton->setCurrentColor(QColor(nesGetPaletteRedComponent(0x20),nesGetPaletteGreenComponent(0x20),nesGetPaletteBlueComponent(0x20)));

   ui->col0PushButton->setText("");
   ui->col1PushButton->setText("");
   ui->col2PushButton->setText("");
   ui->col3PushButton->setText("");

   connect(ui->col0PushButton, SIGNAL(colorChanged(QColor)), this, SLOT(colorChanged(QColor)));
   connect(ui->col1PushButton, SIGNAL(colorChanged(QColor)), this, SLOT(colorChanged(QColor)));
   connect(ui->col2PushButton, SIGNAL(colorChanged(QColor)), this, SLOT(colorChanged(QColor)));
   connect(ui->col3PushButton, SIGNAL(colorChanged(QColor)), this, SLOT(colorChanged(QColor)));

   renderer = new CCHRROMPreviewRenderer(ui->frame, imgData);
   ui->frame->layout()->addWidget(renderer);
   ui->frame->layout()->update();

   model = new CChrRomItemListDisplayModel();

   delegate = new CChrRomBankItemDelegate();

   ui->tableView->setModel(model);
   ui->tableView->setItemDelegateForColumn(ChrRomBankItemCol_Name,delegate);
   ui->tableView->setColumnWidth(ChrRomBankItemCol_Icon, 26);
   ui->tableView->setColumnWidth(ChrRomBankItemCol_Name,400);

   ui->progressBar->setMaximum(MEM_8KB);

   updateChrRomBankItemList(bankItems);

   // Do this last since we're hanging on layoutChanged to know when to set "modified"
   QObject::connect(model,SIGNAL(layoutChanged()),this,SLOT(updateUi()));
}

GraphicsBankEditorForm::~GraphicsBankEditorForm()
{
   delete ui;
   delete model;
   delete renderer;
   delete delegate;
}

QList<IChrRomBankItem*> GraphicsBankEditorForm::bankItems()
{
   return model->bankItems();
}

void GraphicsBankEditorForm::updateUi()
{
   int dataSize = 0;

   for (int i=0; i<model->bankItems().count(); i++)
   {
      dataSize += model->bankItems().at(i)->getChrRomBankItemSize();
   }

   ui->progressBar->setValue(dataSize);

   renderData();

   setModified(true);
   emit editor_modified(true);
}

void GraphicsBankEditorForm::setBankItems(QList<IChrRomBankItem*> items)
{
   updateChrRomBankItemList(items);
}

void GraphicsBankEditorForm::changeEvent(QEvent* e)
{
   QWidget::changeEvent(e);

   switch (e->type())
   {
      case QEvent::LanguageChange:
         ui->retranslateUi(this);
         break;
      default:
         break;
   }
}

void GraphicsBankEditorForm::resizeEvent(QResizeEvent* event)
{
   updateScrollbars();
}

void GraphicsBankEditorForm::keyPressEvent(QKeyEvent *e)
{
   if ( e->key() == Qt::Key_Delete )
   {
      QModelIndex index = ui->tableView->currentIndex();

      if ( index.isValid() )
      {
         model->removeRow(index.row(),QModelIndex());

         int dataSize = 0;

         for (int i=0; i<model->bankItems().count(); i++)
         {
            dataSize += model->bankItems().at(i)->getChrRomBankItemSize();
         }

         ui->progressBar->setValue(dataSize);

         renderData();

         setModified(true);
         emit editor_modified(true);
      }
   }
   CDesignerEditorBase::keyPressEvent(e);
}

void GraphicsBankEditorForm::updateChrRomBankItemList(QList<IChrRomBankItem*> newList)
{
   model->setBankItems(newList);
   model->update();

   int dataSize = 0;

   for (int i=0; i<newList.count(); i++)
   {
      dataSize += newList.at(i)->getChrRomBankItemSize();
   }

   ui->progressBar->setValue(dataSize);

   renderData();
}

void GraphicsBankEditorForm::colorChanged (const QColor& color)
{
   ui->col0PushButton->setText("");
   ui->col1PushButton->setText("");
   ui->col2PushButton->setText("");
   ui->col3PushButton->setText("");

   renderData();
   renderer->setBGColor(ui->col0PushButton->currentColor());
   renderer->reloadData(imgData);
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
   int itemIdx;
   int offset = 0;

   color[0] = ui->col0PushButton->currentColor();
   color[1] = ui->col1PushButton->currentColor();
   color[2] = ui->col2PushButton->currentColor();
   color[3] = ui->col3PushButton->currentColor();

   for (int y = 0; y < 128; y++)
   {
      for (int x = 0; x < 256; x += 8)
      {
         ppuAddr = ((y>>3)<<8)+((x%128)<<1)+(y&0x7);

         if ( x >= 128 )
         {
            ppuAddr += 0x1000;
         }

         // CPTODO: find the appropriate chr-rom bank item for this byte
         for ( itemIdx = 0, offset = 0; offset <= ppuAddr; )
         {
            if ( model->bankItems().count() )
            {
               item = model->bankItems().at(itemIdx);
               if ( ppuAddr < offset+item->getChrRomBankItemSize() )
               {
                  patternData1 = item->getChrRomBankItemData().at(ppuAddr-offset);
                  patternData2 = item->getChrRomBankItemData().at((ppuAddr-offset)+8);
               }

               // Move along.
               itemIdx++;
               offset += item->getChrRomBankItemSize();
            }
            else
            {
               patternData1 = 0;
               patternData2 = 0;
               break;
            }
         }


         for ( int xf = 0; xf < 8; xf++ )
         {
            bit1 = (patternData1>>(7-(xf)))&0x1;
            bit2 = (patternData2>>(7-(xf)))&0x1;
            colorIdx = (bit1|(bit2<<1));
            imgData[((y<<8)<<2) + (x<<2) + (xf<<2) + 0] = color[colorIdx].blue();
            imgData[((y<<8)<<2) + (x<<2) + (xf<<2) + 1] = color[colorIdx].green();
            imgData[((y<<8)<<2) + (x<<2) + (xf<<2) + 2] = color[colorIdx].red();
         }
      }
   }
   renderer->reloadData(imgData);
}

void GraphicsBankEditorForm::on_zoomSlider_sliderMoved(int position)
{
}

void GraphicsBankEditorForm::on_zoomSlider_actionTriggered(int action)
{
}

void GraphicsBankEditorForm::on_zoomSlider_valueChanged(int value)
{
   renderer->changeZoom(value);
   ui->zoomValueLabel->setText(QString::number(value).append("%"));
   updateScrollbars();
}

void GraphicsBankEditorForm::updateScrollbars()
{
   int value = ui->zoomSlider->value();
   int viewWidth = (float)256 * ((float)value / 100.0f);
   int viewHeight = (float)128 * ((float)value / 100.0f);
   ui->horizontalScrollBar->setMaximum(viewWidth - renderer->width() < 0 ? 0 : ((viewWidth - renderer->width()) / ((float)value / 100.0f)) + 1);
   ui->verticalScrollBar->setMaximum(viewHeight - renderer->height() < 0 ? 0 : ((viewHeight - renderer->height()) / ((float)value / 100.0f)) + 1);
   renderer->scrollX = ui->horizontalScrollBar->value();
   renderer->scrollY = ui->verticalScrollBar->value();
}

void GraphicsBankEditorForm::on_verticalScrollBar_actionTriggered(int action)
{
}

void GraphicsBankEditorForm::on_horizontalScrollBar_actionTriggered(int action)
{
}

void GraphicsBankEditorForm::on_horizontalScrollBar_valueChanged(int value)
{
   renderer->scrollX = ui->horizontalScrollBar->value();
   renderer->repaint();
}

void GraphicsBankEditorForm::on_verticalScrollBar_valueChanged(int value)
{
   renderer->scrollY = ui->verticalScrollBar->value();
   renderer->repaint();
}
