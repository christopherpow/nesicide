#include <QFileDialog>
#include <QDir>

#include "codedataloggerdockwidget.h"
#include "ui_codedataloggerdockwidget.h"

#include "dbg_cnes6502.h"
#include "dbg_cnesppu.h"

#include "cobjectregistry.h"

CodeDataLoggerDockWidget::CodeDataLoggerDockWidget(QWidget *parent) :
    CDebuggerBase(parent),
    ui(new Ui::CodeDataLoggerDockWidget)
{
   ui->setupUi(this);

   RENDERCPUCODEDATALOGGER();
   RENDERPPUCODEDATALOGGER();

   renderer = new PanZoomRenderer(256,256,10000,CPUCODEDATALOGGERTV(),false,ui->frame);
   ui->frame->layout()->addWidget(renderer);
   ui->frame->layout()->update();

   renderer->installEventFilter(this);

#if defined(Q_OS_MAC) || defined(Q_OS_MACX) || defined(Q_OS_MAC64)
   ui->address->setFont(QFont("Monaco", 11));
#endif
#ifdef Q_OS_LINUX
   ui->address->setFont(QFont("Monospace", 10));
#endif
#ifdef Q_OS_WIN
   ui->address->setFont(QFont("Consolas", 11));
#endif

   pThread = new DebuggerUpdateThread(&RENDERCPUCODEDATALOGGER);
   QObject::connect(pThread,SIGNAL(updateComplete()),this,SLOT(renderData()));
}

CodeDataLoggerDockWidget::~CodeDataLoggerDockWidget()
{
   delete pThread;
   delete ui;
   delete renderer;
}

void CodeDataLoggerDockWidget::updateTargetMachine(QString target)
{
   if ( target.compare("none") )
   {
      QObject* breakpointWatcher = CObjectRegistry::instance()->getObject("Breakpoint Watcher");
      QObject* emulator = CObjectRegistry::instance()->getObject("Emulator");

      QObject::connect(emulator,SIGNAL(machineReady()),pThread,SLOT(updateDebuggers()));
      QObject::connect(emulator,SIGNAL(emulatorReset()),pThread,SLOT(updateDebuggers()));
      QObject::connect(emulator,SIGNAL(emulatorPaused(bool)),pThread,SLOT(updateDebuggers()));
      QObject::connect(breakpointWatcher,SIGNAL(breakpointHit()),pThread,SLOT(updateDebuggers()));
   }
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

bool CodeDataLoggerDockWidget::eventFilter(QObject* obj,QEvent* event)
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

void CodeDataLoggerDockWidget::renderer_enterEvent(QEvent */*event*/)
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

void CodeDataLoggerDockWidget::renderer_leaveEvent(QEvent */*event*/)
{
   updateInfoText();
}

void CodeDataLoggerDockWidget::renderer_mouseMoveEvent(QMouseEvent */*event*/)
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

void CodeDataLoggerDockWidget::updateInfoText(int x, int y)
{
   int addr;

   if ( (x >= 0) && (y >= 0) )
   {
      QString str;

      addr = x;
      addr |= (y<<8);

      str.sprintf("$%04X",
                  addr);

      ui->address->setText(str);
   }
   else
   {
      ui->address->clear();
   }
}

void CodeDataLoggerDockWidget::showEvent(QShowEvent* /*event*/)
{
   QObject* emulator = CObjectRegistry::instance()->getObject("Emulator");

   QObject::connect(emulator,SIGNAL(updateDebuggers()),pThread,SLOT(updateDebuggers()));

   pThread->updateDebuggers();
}

void CodeDataLoggerDockWidget::hideEvent(QHideEvent* /*event*/)
{
   QObject* emulator = CObjectRegistry::instance()->getObject("Emulator");

   QObject::disconnect(emulator,SIGNAL(updateDebuggers()),pThread,SLOT(updateDebuggers()));
}

void CodeDataLoggerDockWidget::renderData()
{
   switch ( ui->displaySelect->currentIndex() )
   {
      case CodeDataLogger_CPU:
         renderer->reloadData(CPUCODEDATALOGGERTV());
         break;
      case CodeDataLogger_PPU:
         renderer->reloadData(PPUCODEDATALOGGERTV());
         break;
   }
}

void CodeDataLoggerDockWidget::on_displaySelect_currentIndexChanged(int index)
{
   // Clear image...
   CLEARCPUCODEDATALOGGER();
   CLEARPPUCODEDATALOGGER();

   switch ( index )
   {
      case CodeDataLogger_CPU:
         pThread->changeFunction(&RENDERCPUCODEDATALOGGER);
         break;
      case CodeDataLogger_PPU:
         pThread->changeFunction(&RENDERPPUCODEDATALOGGER);
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
                       (pEntry->source == eNESSource_APU) )
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
