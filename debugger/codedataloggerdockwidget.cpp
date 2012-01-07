#include "codedataloggerdockwidget.h"
#include "ui_codedataloggerdockwidget.h"

#include "dbg_cnes6502.h"
#include "dbg_cnesppu.h"

#include "main.h"

CodeDataLoggerDockWidget::CodeDataLoggerDockWidget(QWidget *parent) :
    CDebuggerBase(parent),
    ui(new Ui::CodeDataLoggerDockWidget)
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
   C6502DBG::CodeDataLoggerInspectorTV ( (int8_t*)imgData );
   CPPUDBG::CodeDataLoggerInspectorTV ( (int8_t*)imgData );

   renderer = new PanZoomRenderer(256,256,10000,imgData,false,ui->frame);
   ui->frame->layout()->addWidget(renderer);
   ui->frame->layout()->update();

   pThread = new DebuggerUpdateThread(&C6502DBG::RENDERCODEDATALOGGER);

   QObject::connect(emulator,SIGNAL(cartridgeLoaded()),pThread,SLOT(updateDebuggers()));
   QObject::connect(emulator,SIGNAL(emulatorReset()),pThread,SLOT(updateDebuggers()));
   QObject::connect(emulator,SIGNAL(emulatorPaused(bool)),pThread,SLOT(updateDebuggers()));
   QObject::connect(breakpointWatcher,SIGNAL(breakpointHit()),pThread,SLOT(updateDebuggers()));
   QObject::connect(pThread,SIGNAL(updateComplete()),this,SLOT(renderData()));
}

CodeDataLoggerDockWidget::~CodeDataLoggerDockWidget()
{
   delete ui;
   delete imgData;
   delete renderer;
   delete pThread;
}

void CodeDataLoggerDockWidget::changeEvent(QEvent* e)
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

void CodeDataLoggerDockWidget::showEvent(QShowEvent* event)
{
   QObject::connect(emulator,SIGNAL(updateDebuggers()),pThread,SLOT(updateDebuggers()));

   pThread->updateDebuggers();
}

void CodeDataLoggerDockWidget::hideEvent(QHideEvent* event)
{
   QObject::disconnect(emulator,SIGNAL(updateDebuggers()),pThread,SLOT(updateDebuggers()));
}

void CodeDataLoggerDockWidget::renderData()
{
   switch ( ui->displaySelect->currentIndex() )
   {
      case CodeDataLogger_CPU:
         renderer->reloadData(imgData);
         break;
      case CodeDataLogger_PPU:
         renderer->reloadData(imgData);
         break;
   }
}

void CodeDataLoggerDockWidget::on_displaySelect_currentIndexChanged(int index)
{
   int i;

   // Clear image...
   for ( i = 0; i < 256*256*4; i+=4 )
   {
      imgData[i] = 0;
      imgData[i+1] = 0;
      imgData[i+2] = 0;
      imgData[i+3] = 0xFF;
   }

   switch ( index )
   {
      case CodeDataLogger_CPU:
         pThread->changeFunction(&C6502DBG::RENDERCODEDATALOGGER);
         break;
      case CodeDataLogger_PPU:
         pThread->changeFunction(&CPPUDBG::RENDERCODEDATALOGGER);
         break;
   }

   pThread->updateDebuggers();
}

//CDL FORMAT
//xPdcAADC
//C  = Whether it was accessed as code.
//D  = Whether it was accessed as data.
//AA = Into which ROM bank it was mapped when last accessed:
//        00 = $8000-$9FFF        01 = $A000-$BFFF
//        10 = $C000-$DFFF        11 = $E000-$FFFF
//c  = Whether indirectly accessed as code.
//        (e.g. as the destination of a JMP ($nnnn) instruction)
//d  = Whether indirectly accessed as data.
//        (e.g. as the destination of an LDA ($nn),Y instruction)
//P  = If logged as PCM audio data.
//x  = unused.

void CodeDataLoggerDockWidget::on_exportData_clicked()
{
   QString fileName = QFileDialog::getSaveFileName(NULL,"Export Code/Data Log",QDir::currentPath(),"Code+Data Log File (*.cdl)");
   int addr;
   int size = nesGetPRGROMSize();
   int byte;
   QByteArray cdls;

   if ( !fileName.isEmpty() )
   {
      QFile file(fileName);

      if ( file.open(QIODevice::ReadWrite|QIODevice::Truncate) )
      {
         for ( addr = 0; addr < size; addr += MEM_8KB )
         {
            CCodeDataLogger* pLogger = nesGetPhysicalPRGROMCodeDataLoggerDatabase(addr);
            LoggerInfo* pEntry;
            unsigned char cdl;

            cdls.clear();
            for ( byte = 0; byte < MEM_8KB; byte++ )
            {
               cdl = 0x00;
               pEntry = pLogger->GetLogEntry(byte);
               if ( pEntry->count )
               {
                  if ( (pEntry->type == eLogger_InstructionFetch) ||
                       (pEntry->type == eLogger_OperandFetch) )
                  {
                     cdl |= 0x01;
                  }
                  else if ( (pEntry->type == eLogger_DataRead) ||
                            (pEntry->type == eLogger_DataWrite) ||
                            (pEntry->type == eLogger_DMA) )
                  {
                     cdl |= 0x02;
                  }
                  cdl |= ((pEntry->cpuAddr>>SHIFT_64KB_8KB)&0x3)<<2;
                  // No information available (yet) to fill in the indirect code use bit.
                  // No information available (yet) to fill in the indirect data use bit.
                  if ( (pEntry->type == eLogger_DMA) &&
                       (pEntry->source == eSource_APU) )
                  {
                     cdl |= 0x40;
                  }
               }
               cdls.append(cdl);
            }
            file.write(cdls);
         }
         file.close();
      }
   }
}
