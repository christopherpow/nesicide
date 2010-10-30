#include "mapperinformationdialog.h"
#include "ui_mapperinformationdialog.h"

#include "main.h"

#include "dbg_cnes.h"
#include "dbg_cnesrom.h"
#include "dbg_cnesrommapper001.h"
#include "dbg_cnesrommapper004.h"
#include "dbg_cnesmappers.h"

MapperInformationDialog::MapperInformationDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MapperInformationDialog)
{
    ui->setupUi(this);
    QObject::connect ( emulator, SIGNAL(cartridgeLoaded()), this, SLOT(cartridgeLoaded()) );
    QObject::connect ( emulator, SIGNAL(emulatedFrame()), this, SLOT(updateInformation()) );
    QObject::connect ( breakpointWatcher, SIGNAL(breakpointHit()), this, SLOT(updateInformation()) );
}

MapperInformationDialog::~MapperInformationDialog()
{
    delete ui;
}

void MapperInformationDialog::changeEvent(QEvent *e)
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

void MapperInformationDialog::showEvent(QShowEvent* e)
{
   QDialog::showEvent(e);
   updateInformation();
}

void MapperInformationDialog::cartridgeLoaded()
{
   char buffer [ 128 ];
   sprintf ( buffer, "Mapper %d: %s", CROMDBG::MAPPER(), mapperNameFromID(CROMDBG::MAPPER()) );
   ui->info->setText ( buffer );
   ui->tabWidget->setTabEnabled(0, CROMDBG::PRGREMAPABLE());
   ui->tabWidget->setTabEnabled(1, CROMDBG::CHRREMAPABLE());
   ui->internalInfo->setCurrentIndex(CROMDBG::MAPPER());
}

void MapperInformationDialog::updateInformation()
{
   CBreakpointInfo* pBreakpoints = nesGetBreakpointDatabase();
   int idx;
   char buffer [ 16 ];

   // Only update UI elements if the inspector is visible...
   if ( isVisible() )
   {
      // Show PRG-ROM absolute addresses...
      sprintf ( buffer, "$%X", CROMDBG::ABSADDR(0x8000) );
      ui->prg0->setText ( buffer );
      sprintf ( buffer, "$%X", CROMDBG::ABSADDR(0xA000) );
      ui->prg1->setText ( buffer );
      sprintf ( buffer, "$%X", CROMDBG::ABSADDR(0xC000) );
      ui->prg2->setText ( buffer );
      sprintf ( buffer, "$%X", CROMDBG::ABSADDR(0xE000) );
      ui->prg3->setText ( buffer );

      // Show CHR memory absolute addresses...
      sprintf ( buffer, "$%X", CROMDBG::CHRMEMABSADDR(0x0000) );
      ui->chr0->setText ( buffer );
      sprintf ( buffer, "$%X", CROMDBG::CHRMEMABSADDR(0x0400) );
      ui->chr1->setText ( buffer );
      sprintf ( buffer, "$%X", CROMDBG::CHRMEMABSADDR(0x0800) );
      ui->chr2->setText ( buffer );
      sprintf ( buffer, "$%X", CROMDBG::CHRMEMABSADDR(0x0C00) );
      ui->chr3->setText ( buffer );
      sprintf ( buffer, "$%X", CROMDBG::CHRMEMABSADDR(0x1000) );
      ui->chr4->setText ( buffer );
      sprintf ( buffer, "$%X", CROMDBG::CHRMEMABSADDR(0x1400) );
      ui->chr5->setText ( buffer );
      sprintf ( buffer, "$%X", CROMDBG::CHRMEMABSADDR(0x1800) );
      ui->chr6->setText ( buffer );
      sprintf ( buffer, "$%X", CROMDBG::CHRMEMABSADDR(0x1C00) );
      ui->chr7->setText ( buffer );

      switch ( CROMDBG::MAPPER() )
      {
         case 1:
            sprintf ( buffer, "$%02X", CROMMapper001DBG::SHIFTREGISTER() );
            ui->shiftRegister->setText ( buffer );
            sprintf ( buffer, "%d", CROMMapper001DBG::SHIFTREGISTERBIT() );
            ui->shiftRegisterBit->setText ( buffer );
         break;

         case 4:
            ui->irqEnabled->setChecked ( CROMMapper004DBG::IRQENABLED() );
            ui->irqAsserted->setChecked ( CROMMapper004DBG::IRQASSERTED() );
            ui->ppuAddrA12->setChecked ( CROMMapper004DBG::PPUADDRA12() );
            sprintf ( buffer, "$%02X", CROMMapper004DBG::IRQRELOAD() );
            ui->irqReload->setText ( buffer );
            sprintf ( buffer, "$%02X", CROMMapper004DBG::IRQCOUNTER() );
            ui->irqCounter->setText ( buffer );
            sprintf ( buffer, "%d", CROMMapper004DBG::PPUCYCLE() );
            ui->lastA12Cycle->setText ( buffer );
         break;
      }
   }

   // Check breakpoints for hits and highlight if necessary...
   for ( idx = 0; idx < pBreakpoints->GetNumBreakpoints(); idx++ )
   {
      BreakpointInfo* pBreakpoint = pBreakpoints->GetBreakpoint(idx);
      if ( pBreakpoint->hit )
      {
         if ( pBreakpoint->type == eBreakOnMapperEvent )
         {
            // Update display...
            emit showMe();
         }
      }
   }
}
