#include "ppuinformationdockwidget.h"
#include "ui_ppuinformationdockwidget.h"

#include "dbg_cnes.h"
#include "dbg_cnesppu.h"

#include "cobjectregistry.h"
#include "main.h"

PPUInformationDockWidget::PPUInformationDockWidget(QWidget *parent) :
    CDebuggerBase(parent),
    ui(new Ui::PPUInformationDockWidget)
{
   ui->setupUi(this);
}

PPUInformationDockWidget::~PPUInformationDockWidget()
{
   delete ui;
}

void PPUInformationDockWidget::updateTargetMachine(QString /*target*/)
{
   QObject* breakpointWatcher = CObjectRegistry::getInstance()->getObject("Breakpoint Watcher");
   QObject* emulator = CObjectRegistry::getInstance()->getObject("Emulator");

   QObject::connect ( emulator, SIGNAL(machineReady()), this, SLOT(updateInformation()) );
   QObject::connect ( emulator, SIGNAL(emulatorReset()), this, SLOT(updateInformation()) );
   QObject::connect ( emulator, SIGNAL(emulatorPaused(bool)), this, SLOT(updateInformation()) );
   QObject::connect ( breakpointWatcher, SIGNAL(breakpointHit()), this, SLOT(updateInformation()) );
}

void PPUInformationDockWidget::changeEvent(QEvent* e)
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

void PPUInformationDockWidget::showEvent(QShowEvent* /*e*/)
{
   QObject* emulator = CObjectRegistry::getInstance()->getObject("Emulator");

   QObject::connect ( emulator, SIGNAL(updateDebuggers()), this, SLOT(updateInformation()) );
   updateInformation();
}

void PPUInformationDockWidget::hideEvent(QHideEvent* /*e*/)
{
   QObject* emulator = CObjectRegistry::getInstance()->getObject("Emulator");

   QObject::disconnect ( emulator, SIGNAL(updateDebuggers()), this, SLOT(updateInformation()) );
}

void PPUInformationDockWidget::updateInformation()
{
   const char* ppuFlipFlopStr [] = { "Low", "High" };
   CBreakpointInfo* pBreakpoints = nesGetBreakpointDatabase();
   int idx;
   char buffer[16];
   unsigned char x,y;

   nesGetCurrentPixel(&x,&y);

   // Only update the UI elements if the inspector is visible...
   if ( isVisible() )
   {
      sprintf ( buffer, "%d", nesGetPPUFrame() );
      ui->frameNumber->setText(buffer);

      sprintf ( buffer, "%d", nesGetPPUCycle() );
      ui->cycleNumber->setText(buffer);

      sprintf ( buffer, "%d", nesGetPPUCycle()%PPU_CYCLES_PER_SCANLINE );
      ui->ppuX->setText(buffer);

      sprintf ( buffer, "%d", nesGetPPUCycle()/PPU_CYCLES_PER_SCANLINE );
      ui->ppuY->setText(buffer);

      sprintf ( buffer, "%02X", nesGetPPUOAMAddress() );
      ui->oamAddress->setText(buffer);

      sprintf ( buffer, "%02X", nesGetPPUOAM(nesGetPPUOAMAddress()) );
      ui->oamData->setText(buffer);

      nesGetCurrentScroll(&x,&y);
      sprintf ( buffer, "%02X", x );
      ui->scrollX->setText(buffer);

      sprintf ( buffer, "%02X", y );
      ui->scrollY->setText(buffer);

      sprintf ( buffer, "%04X", nesGetPPUAddress() );
      ui->ppuAddr->setText(buffer);

      sprintf ( buffer, "%02X", nesGetPPUReadLatch() );
      ui->ppuLatch->setText(buffer);

      sprintf ( buffer, "%02X", nesGetPPUAddressLatch() );
      ui->ppuAddrLatch->setText(buffer);

      ui->ppuFlipFlop->setText(ppuFlipFlopStr[nesGetPPUFlipFlop()]);
   }

   // Check breakpoints for hits and highlight if necessary...
   for ( idx = 0; idx < pBreakpoints->GetNumBreakpoints(); idx++ )
   {
      BreakpointInfo* pBreakpoint = pBreakpoints->GetBreakpoint(idx);

      if ( pBreakpoint->hit )
      {
         if ( (pBreakpoint->type == eBreakOnPPUEvent) ||
               (pBreakpoint->type == eBreakOnPPUPortalAccess) ||
               (pBreakpoint->type == eBreakOnPPUPortalRead) ||
               (pBreakpoint->type == eBreakOnPPUPortalWrite) )
         {
            // Update display...
            show();
         }
      }
   }
}
