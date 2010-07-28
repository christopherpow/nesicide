#include "ppuinformationdialog.h"
#include "ui_ppuinformationdialog.h"

#include "main.h"

#include "cnes.h"
#include "cnesppu.h"

PPUInformationDialog::PPUInformationDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PPUInformationDialog)
{
    ui->setupUi(this);
    QObject::connect ( emulator, SIGNAL(emulatedFrame()), this, SLOT(updateInformation()) );
    QObject::connect ( emulator, SIGNAL(emulatorReset()), this, SLOT(updateInformation()) );
    QObject::connect ( breakpointWatcher, SIGNAL(breakpointHit()), this, SLOT(updateInformation()) );
}

PPUInformationDialog::~PPUInformationDialog()
{
    delete ui;
}

void PPUInformationDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void PPUInformationDialog::showEvent(QShowEvent* e)
{
   QDialog::showEvent(e);

   updateInformation();
}

void PPUInformationDialog::updateInformation()
{
   const char* ppuFlipFlopStr [] = { "Low", "High" };
   CBreakpointInfo* pBreakpoints = CNES::BREAKPOINTS();
   int idx;
   char buffer[16];
   unsigned char y = CPPU::_Y();

   // Only update the UI elements if the inspector is visible...
   if ( isVisible() )
   {
      sprintf ( buffer, "%d", (unsigned char)CPPU::_X() );
      ui->pixelX->setText(buffer);

      // Fix for scanline -1...
      sprintf ( buffer, "%d", y );
      ui->pixelY->setText(buffer);

      sprintf ( buffer, "%d", CPPU::_FRAME() );
      ui->frameNumber->setText(buffer);

      sprintf ( buffer, "%d", CPPU::_CYCLES() );
      ui->cycleNumber->setText(buffer);

      sprintf ( buffer, "%d", CPPU::_CYCLES()%PPU_CYCLES_PER_SCANLINE );
      ui->ppuX->setText(buffer);

      sprintf ( buffer, "%d", CPPU::_CYCLES()/PPU_CYCLES_PER_SCANLINE );
      ui->ppuY->setText(buffer);

      sprintf ( buffer, "$%04X", CPPU::_PPUADDR() );
      ui->ppuAddr->setText(buffer);

      sprintf ( buffer, "$%02X", CPPU::_PPUREADLATCH() );
      ui->ppuLatch->setText(buffer);

      sprintf ( buffer, "$%02X", CPPU::_PPUADDRLATCH() );
      ui->ppuAddrLatch->setText(buffer);

      ui->ppuFlipFlop->setText(ppuFlipFlopStr[CPPU::_PPUFLIPFLOP()]);
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
            emit showMe();
         }
      }
   }
}
