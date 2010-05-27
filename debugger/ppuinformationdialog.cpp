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

void PPUInformationDialog::showEvent(QShowEvent*)
{
}

void PPUInformationDialog::updateInformation()
{
   CBreakpointInfo* pBreakpoints = CNES::BREAKPOINTS();
   int idx;
   char buffer[16];
   unsigned char y = CPPU::_Y();

   sprintf ( buffer, "%d", (unsigned char)CPPU::_X() );
   ui->pixelX->setText(buffer);

   // Fix for scanline -1...
   if ( y > 239 )
   {
      sprintf ( buffer, "PRE-RENDER" );
   }
   else
   {
      sprintf ( buffer, "%d", y );
   }
   ui->pixelY->setText(buffer);

   sprintf ( buffer, "%d", CPPU::_FRAME() );
   ui->frameNumber->setText(buffer);

   sprintf ( buffer, "%d", CPPU::CYCLES() );
   ui->cycleNumber->setText(buffer);

   sprintf ( buffer, "$%04X", CPPU::_PPUADDR() );
   ui->ppuAddr->setText(buffer);

   sprintf ( buffer, "$%02X", CPPU::_PPULATCH() );
   ui->ppuLatch->setText(buffer);

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
