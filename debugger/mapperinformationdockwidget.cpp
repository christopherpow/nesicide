#include "mapperinformationdockwidget.h"
#include "ui_mapperinformationdockwidget.h"

#include "dbg_cnes.h"
#include "dbg_cnesrom.h"
#include "dbg_cnesrommapper001.h"
#include "dbg_cnesrommapper004.h"
#include "dbg_cnesmappers.h"

#include "main.h"

MapperInformationDockWidget::MapperInformationDockWidget(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::MapperInformationDockWidget)
{
   ui->setupUi(this);
//   QObject::connect ( emulator, SIGNAL(emulatedFrame()), this, SLOT(updateInformation()) );
   QObject::connect ( emulator, SIGNAL(cartridgeLoaded()), this, SLOT(cartridgeLoaded()) );
   QObject::connect ( emulator, SIGNAL(emulatorReset()), this, SLOT(updateInformation()) );
   QObject::connect ( emulator, SIGNAL(emulatorPaused(bool)), this, SLOT(updateInformation()) );
   QObject::connect ( breakpointWatcher, SIGNAL(breakpointHit()), this, SLOT(updateInformation()) );
}

MapperInformationDockWidget::~MapperInformationDockWidget()
{
    delete ui;
}

void MapperInformationDockWidget::changeEvent(QEvent* e)
{
   QDockWidget::changeEvent(e);

   switch (e->type())
   {
      case QEvent::LanguageChange:
         ui->retranslateUi(this);
         break;
      default:
         break;
   }
}

void MapperInformationDockWidget::showEvent(QShowEvent* e)
{
   QDockWidget::showEvent(e);
   updateInformation();
}

void MapperInformationDockWidget::cartridgeLoaded()
{
   char buffer [ 128 ];
   sprintf ( buffer, "Mapper %d: %s", CROMDBG::MAPPER(), mapperNameFromID(CROMDBG::MAPPER()) );
   ui->info->setText ( buffer );
   ui->tabWidget->setTabEnabled(0, CROMDBG::PRGREMAPABLE());
   ui->tabWidget->setTabEnabled(1, CROMDBG::CHRREMAPABLE());
   ui->internalInfo->setCurrentIndex(CROMDBG::MAPPER());
}

void MapperInformationDockWidget::updateInformation()
{
   CBreakpointInfo* pBreakpoints = nesGetBreakpointDatabase();
   int idx;
   char buffer [ 16 ];
   nesMapper001Info mapper001Info;
   nesMapper004Info mapper004Info;

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
         nesMapper001GetInformation(&mapper001Info);
         sprintf ( buffer, "$%02X", mapper001Info.shiftRegister );
         ui->shiftRegister->setText ( buffer );
         sprintf ( buffer, "%d", mapper001Info.shiftRegisterBit );
         ui->shiftRegisterBit->setText ( buffer );
         break;

      case 4:
         nesMapper004GetInformation(&mapper004Info);
         ui->irqEnabled->setChecked ( mapper004Info.irqEnabled );
         ui->irqAsserted->setChecked ( mapper004Info.irqAsserted );
         ui->ppuAddrA12->setChecked ( mapper004Info.ppuAddrA12 );
         sprintf ( buffer, "$%02X", mapper004Info.irqReload );
         ui->irqReload->setText ( buffer );
         sprintf ( buffer, "$%02X", mapper004Info.irqCounter );
         ui->irqCounter->setText ( buffer );
         sprintf ( buffer, "%d", mapper004Info.ppuCycle );
         ui->lastA12Cycle->setText ( buffer );
         break;
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
            show();
         }
      }
   }
}
