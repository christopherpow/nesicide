#include "mapperinformationdialog.h"
#include "ui_mapperinformationdialog.h"

#include "main.h"

#include "cnes.h"
#include "cnesrom.h"
#include "cnesrommapper001.h"
#include "cnesrommapper004.h"

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

void MapperInformationDialog::showEvent(QShowEvent*)
{
}

void MapperInformationDialog::cartridgeLoaded()
{
   char buffer [ 128 ];
   sprintf ( buffer, "Mapper %d: %s", CROM::MAPPER(), mapperNameFromID(CROM::MAPPER()) );
   ui->info->setText ( buffer );
   ui->tabWidget->setTabEnabled(0, mapperfunc[CROM::MAPPER()].remapPrg);
   ui->tabWidget->setTabEnabled(1, mapperfunc[CROM::MAPPER()].remapChr);
   ui->internalInfo->setCurrentIndex(CROM::MAPPER());
}

void MapperInformationDialog::updateInformation()
{
   CBreakpointInfo* pBreakpoints = CNES::BREAKPOINTS();
   int idx;
   char buffer [ 16 ];

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

   // Show PRG-ROM absolute addresses...
   sprintf ( buffer, "$%X", CROM::ABSADDR(0x8000) );
   ui->prg0->setText ( buffer );
   sprintf ( buffer, "$%X", CROM::ABSADDR(0xA000) );
   ui->prg1->setText ( buffer );
   sprintf ( buffer, "$%X", CROM::ABSADDR(0xC000) );
   ui->prg2->setText ( buffer );
   sprintf ( buffer, "$%X", CROM::ABSADDR(0xE000) );
   ui->prg3->setText ( buffer );

   // Show CHR memory absolute addresses...
   sprintf ( buffer, "$%X", CROM::CHRMEMABSADDR(0x0000) );
   ui->chr0->setText ( buffer );
   sprintf ( buffer, "$%X", CROM::CHRMEMABSADDR(0x0400) );
   ui->chr1->setText ( buffer );
   sprintf ( buffer, "$%X", CROM::CHRMEMABSADDR(0x0800) );
   ui->chr2->setText ( buffer );
   sprintf ( buffer, "$%X", CROM::CHRMEMABSADDR(0x0C00) );
   ui->chr3->setText ( buffer );
   sprintf ( buffer, "$%X", CROM::CHRMEMABSADDR(0x1000) );
   ui->chr4->setText ( buffer );
   sprintf ( buffer, "$%X", CROM::CHRMEMABSADDR(0x1400) );
   ui->chr5->setText ( buffer );
   sprintf ( buffer, "$%X", CROM::CHRMEMABSADDR(0x1800) );
   ui->chr6->setText ( buffer );
   sprintf ( buffer, "$%X", CROM::CHRMEMABSADDR(0x1C00) );
   ui->chr7->setText ( buffer );

   switch ( CROM::MAPPER() )
   {
      case 1:
         sprintf ( buffer, "$%02X", CROMMapper001::SHIFTREGISTER() );
         ui->shiftRegister->setText ( buffer );
         sprintf ( buffer, "%d", CROMMapper001::SHIFTREGISTERBIT() );
         ui->shiftRegisterBit->setText ( buffer );
      break;

      case 4:
         ui->irqEnabled->setChecked ( CROMMapper004::IRQENABLED() );
         ui->irqAsserted->setChecked ( CROMMapper004::IRQASSERTED() );
         ui->ppuAddrA12->setChecked ( CROMMapper004::PPUADDRA12() );
         sprintf ( buffer, "$%02X", CROMMapper004::IRQRELOAD() );
         ui->irqReload->setText ( buffer );
         sprintf ( buffer, "$%02X", CROMMapper004::IRQCOUNTER() );
         ui->irqCounter->setText ( buffer );
         sprintf ( buffer, "%d", CROMMapper004::PPUCYCLE() );
         ui->lastA12Cycle->setText ( buffer );
      break;
   }
}
