#include "mapperinformationdockwidget.h"
#include "ui_mapperinformationdockwidget.h"

#include "nes_emulator_core.h"

#include "dbg_cnesmappers.h"

#include "cobjectregistry.h"
#include "main.h"

enum
{
   CPUToPRGPage = 0,
   PPUToCHRPage,
   PPUMirroringPage,
   InternalsPage
};

MapperInformationDockWidget::MapperInformationDockWidget(QWidget *parent) :
    CDebuggerBase(parent),
    ui(new Ui::MapperInformationDockWidget)
{
   int idx;

   ui->setupUi(this);

   ui->tabWidget->setCurrentIndex(InternalsPage);

   // Set up default mapper internal info page map.
   for ( idx = 0; idx < 256; idx++ )
   {
      // Set up internal info page to "nothing".
      internalPageMap.insert(idx,ui->noInternals);
   }

   // Set up specific mapper internal info page maps.
   internalPageMap.insert(1,ui->mapper1);
   internalPageMap.insert(4,ui->mapper4);
   internalPageMap.insert(5,ui->mapper5);
   internalPageMap.insert(9,ui->mapper9and10);
   internalPageMap.insert(10,ui->mapper9and10);
   internalPageMap.insert(16,ui->mapper16);
   internalPageMap.insert(19,ui->mapper19);
   internalPageMap.insert(28,ui->mapper28);
   internalPageMap.insert(69,ui->mapper69);
   internalPageMap.insert(111,ui->mapper111);
   internalPageMap.insert(159,ui->mapper16);

   // Force UI update so it doesn't look uninitialized completely.
   machineReady();
}

MapperInformationDockWidget::~MapperInformationDockWidget()
{
    delete ui;
}

void MapperInformationDockWidget::updateTargetMachine(QString /*target*/)
{
   QObject* breakpointWatcher = CObjectRegistry::getInstance()->getObject("Breakpoint Watcher");
   QObject* emulator = CObjectRegistry::getInstance()->getObject("Emulator");

   QObject::connect ( emulator, SIGNAL(machineReady()), this, SLOT(machineReady()) );
   QObject::connect ( emulator, SIGNAL(emulatorReset()), this, SLOT(updateInformation()) );
   QObject::connect ( emulator, SIGNAL(emulatorPaused(bool)), this, SLOT(updateInformation()) );
   QObject::connect ( breakpointWatcher, SIGNAL(breakpointHit()), this, SLOT(updateInformation()) );
}

void MapperInformationDockWidget::changeEvent(QEvent* e)
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

void MapperInformationDockWidget::showEvent(QShowEvent* /*e*/)
{
   QObject* emulator = CObjectRegistry::getInstance()->getObject("Emulator");

   QObject::connect ( emulator, SIGNAL(updateDebuggers()), this, SLOT(updateInformation()) );
   updateInformation();
}

void MapperInformationDockWidget::hideEvent(QHideEvent* /*e*/)
{
   QObject* emulator = CObjectRegistry::getInstance()->getObject("Emulator");

   QObject::disconnect ( emulator, SIGNAL(updateDebuggers()), this, SLOT(updateInformation()) );
}

void MapperInformationDockWidget::machineReady()
{
   char buffer [ 128 ];
   sprintf ( buffer, "Mapper %d: %s Information", nesGetMapper(), mapperNameFromID(nesGetMapper()) );
   setWindowTitle(buffer);
   ui->tabWidget->setTabEnabled(CPUToPRGPage, nesMapperRemapsPRGROM());
   ui->tabWidget->setTabEnabled(PPUToCHRPage, nesMapperRemapsCHRMEM());
   ui->internalInfo->setCurrentWidget(internalPageMap.value(nesGetMapper()));
   if ( ui->internalInfo->currentIndex() )
   {
      ui->tabWidget->setTabEnabled(InternalsPage, true);
   }
   else
   {
      ui->tabWidget->setTabEnabled(InternalsPage, false);
   }
   ui->tabWidget->setCurrentIndex(PPUMirroringPage);
}

void MapperInformationDockWidget::updateInformation()
{
   CBreakpointInfo* pBreakpoints = nesGetBreakpointDatabase();
   int idx;
   char buffer [ 32 ];
   nesMapper001Info mapper001Info;
   nesMapper004Info mapper004Info;
   nesMapper005Info mapper005Info;
   nesMapper009010Info mapper009010Info;
   nesMapper016Info mapper016Info;
   nesMapper028Info mapper028Info;
   nesMapper069Info mapper069Info;
   uint16_t mirroring[4];
   uint8_t reg;

   // Show mirroring...
   nesGetMirroring(&(mirroring[0]),&(mirroring[1]),&(mirroring[2]),&(mirroring[3]));
   sprintf ( buffer, "%04X", mirroring[0] );
   ui->nt0->setText ( buffer );
   sprintf ( buffer, "%04X", mirroring[1] );
   ui->nt1->setText ( buffer );
   sprintf ( buffer, "%04X", mirroring[2] );
   ui->nt2->setText ( buffer );
   sprintf ( buffer, "%04X", mirroring[3] );
   ui->nt3->setText ( buffer );

   // Show Bank information...
   sprintf ( buffer, "%d (%dKB)", nesGetNumPRGROMBanks(), nesGetNumPRGROMBanks()<<3 );
   ui->numPrgBanks->setText ( buffer );
   sprintf ( buffer, "%d (%dKB)", nesGetNumCHRROMBanks(), nesGetNumCHRROMBanks()<<3 );
   ui->numChrBanks->setText ( buffer );

   // Show PRG-ROM absolute addresses...
   sprintf ( buffer, "%02X(%05X)", nesGetPRGROMPhysicalAddress(0x8000)>>13, nesGetPRGROMPhysicalAddress(0x8000) );
   ui->prg0->setText ( buffer );
   sprintf ( buffer, "%02X(%05X)", nesGetPRGROMPhysicalAddress(0xA000)>>13, nesGetPRGROMPhysicalAddress(0xA000) );
   ui->prg1->setText ( buffer );
   sprintf ( buffer, "%02X(%05X)", nesGetPRGROMPhysicalAddress(0xC000)>>13, nesGetPRGROMPhysicalAddress(0xC000) );
   ui->prg2->setText ( buffer );
   sprintf ( buffer, "%02X(%05X)", nesGetPRGROMPhysicalAddress(0xE000)>>13, nesGetPRGROMPhysicalAddress(0xE000) );
   ui->prg3->setText ( buffer );

   // Show CHR memory absolute addresses...
   sprintf ( buffer, "%02X:%04X(%05X)", nesGetCHRMEMPhysicalAddress(0x0000)>>13, nesGetCHRMEMPhysicalAddress(0x0000)&MASK_8KB, nesGetCHRMEMPhysicalAddress(0x0000) );
   ui->chr0->setText ( buffer );
   sprintf ( buffer, "%02X:%04X(%05X)", nesGetCHRMEMPhysicalAddress(0x0400)>>13, nesGetCHRMEMPhysicalAddress(0x0400)&MASK_8KB, nesGetCHRMEMPhysicalAddress(0x0400) );
   ui->chr1->setText ( buffer );
   sprintf ( buffer, "%02X:%04X(%05X)", nesGetCHRMEMPhysicalAddress(0x0800)>>13, nesGetCHRMEMPhysicalAddress(0x0800)&MASK_8KB, nesGetCHRMEMPhysicalAddress(0x0800) );
   ui->chr2->setText ( buffer );
   sprintf ( buffer, "%02X:%04X(%05X)", nesGetCHRMEMPhysicalAddress(0x0C00)>>13, nesGetCHRMEMPhysicalAddress(0x0C00)&MASK_8KB, nesGetCHRMEMPhysicalAddress(0x0C00) );
   ui->chr3->setText ( buffer );
   sprintf ( buffer, "%02X:%04X(%05X)", nesGetCHRMEMPhysicalAddress(0x1000)>>13, nesGetCHRMEMPhysicalAddress(0x1000)&MASK_8KB, nesGetCHRMEMPhysicalAddress(0x1000) );
   ui->chr4->setText ( buffer );
   sprintf ( buffer, "%02X:%04X(%05X)", nesGetCHRMEMPhysicalAddress(0x1400)>>13, nesGetCHRMEMPhysicalAddress(0x1400)&MASK_8KB, nesGetCHRMEMPhysicalAddress(0x1400) );
   ui->chr5->setText ( buffer );
   sprintf ( buffer, "%02X:%04X(%05X)", nesGetCHRMEMPhysicalAddress(0x1800)>>13, nesGetCHRMEMPhysicalAddress(0x1800)&MASK_8KB, nesGetCHRMEMPhysicalAddress(0x1800) );
   ui->chr6->setText ( buffer );
   sprintf ( buffer, "%02X:%04X(%05X)", nesGetCHRMEMPhysicalAddress(0x1C00)>>13, nesGetCHRMEMPhysicalAddress(0x1C00)&MASK_8KB, nesGetCHRMEMPhysicalAddress(0x1C00) );
   ui->chr7->setText ( buffer );

   switch ( nesGetMapper() )
   {
   case 1:
      nesMapper001GetInformation(&mapper001Info);
      sprintf ( buffer, "%02X", mapper001Info.shiftRegister );
      ui->shiftRegister1->setText ( buffer );
      sprintf ( buffer, "%d", mapper001Info.shiftRegisterBit );
      ui->shiftRegisterBit1->setText ( buffer );
      break;

   case 4:
      nesMapper004GetInformation(&mapper004Info);
      ui->irqEnabled4->setChecked ( mapper004Info.irqEnabled );
      ui->irqAsserted4->setChecked ( mapper004Info.irqAsserted );
      ui->ppuAddrA124->setChecked ( mapper004Info.ppuAddrA12 );
      sprintf ( buffer, "%02X", mapper004Info.irqLatch );
      ui->irqLatch4->setText ( buffer );
      sprintf ( buffer, "%02X", mapper004Info.irqCounter );
      ui->irqCounter4->setText ( buffer );
      sprintf ( buffer, "%d", mapper004Info.ppuCycle );
      ui->lastA12Cycle4->setText ( buffer );
      ui->irqReload4->setChecked ( mapper004Info.irqReload );
      break;

   case 5:
      nesMapper005GetInformation(&mapper005Info);
      ui->irqEnabled5->setChecked ( mapper005Info.irqEnabled );
      ui->irqAsserted5->setChecked ( mapper005Info.irqAsserted );
      ui->sprite8x16Mode5->setChecked ( mapper005Info.sprite8x16Mode );
      sprintf ( buffer, "%02X", mapper005Info.irqScanline );
      ui->irqScanline5->setText ( buffer );
      break;

   case 9:
      nesMapper009GetInformation(&mapper009010Info);
      sprintf ( buffer, "%02X", mapper009010Info.latch0FD );
      ui->latch0FD->setText ( buffer );
      sprintf ( buffer, "%02X", mapper009010Info.latch0FE );
      ui->latch0FE->setText ( buffer );
      sprintf ( buffer, "%02X", mapper009010Info.latch1FD );
      ui->latch1FD->setText ( buffer );
      sprintf ( buffer, "%02X", mapper009010Info.latch1FE );
      ui->latch1FE->setText ( buffer );
      if ( mapper009010Info.latch0 == 0xFD )
      {
         ui->latch0FD->setEnabled(true);
         ui->latch0FE->setEnabled(false);
      }
      else
      {
         ui->latch0FD->setEnabled(false);
         ui->latch0FE->setEnabled(true);
      }
      if ( mapper009010Info.latch1 == 0xFD )
      {
         ui->latch1FD->setEnabled(true);
         ui->latch1FE->setEnabled(false);
      }
      else
      {
         ui->latch1FD->setEnabled(false);
         ui->latch1FE->setEnabled(true);
      }
      break;

   case 10:
      nesMapper010GetInformation(&mapper009010Info);
      sprintf ( buffer, "%02X", mapper009010Info.latch0FD );
      ui->latch0FD->setText ( buffer );
      sprintf ( buffer, "%02X", mapper009010Info.latch0FE );
      ui->latch0FE->setText ( buffer );
      sprintf ( buffer, "%02X", mapper009010Info.latch1FD );
      ui->latch1FD->setText ( buffer );
      sprintf ( buffer, "%02X", mapper009010Info.latch1FE );
      ui->latch1FE->setText ( buffer );
      if ( mapper009010Info.latch0 == 0xFD )
      {
         ui->latch0FD->setEnabled(true);
         ui->latch0FE->setEnabled(false);
      }
      else
      {
         ui->latch0FD->setEnabled(false);
         ui->latch0FE->setEnabled(true);
      }
      if ( mapper009010Info.latch1 == 0xFD )
      {
         ui->latch1FD->setEnabled(true);
         ui->latch1FE->setEnabled(false);
      }
      else
      {
         ui->latch1FD->setEnabled(false);
         ui->latch1FE->setEnabled(true);
      }
      break;

   case 16:
   case 159:
      nesMapper016GetInformation(&mapper016Info);
      ui->irqEnabled16->setChecked ( mapper016Info.irqEnabled );
      ui->irqAsserted16->setChecked ( mapper016Info.irqAsserted );
      sprintf ( buffer, "%04X", mapper016Info.irqCounter );
      ui->irqCounter16->setText ( buffer );
      sprintf ( buffer, "%d", mapper016Info.eepromState );
      ui->eepromState16->setText ( buffer );
      sprintf ( buffer, "%d", mapper016Info.eepromBitCounter );
      ui->eepromBitCounter16->setText ( buffer );
      sprintf ( buffer, "%02X", mapper016Info.eepromCmd );
      ui->eepromCmd16->setText ( buffer );
      sprintf ( buffer, "%02X", mapper016Info.eepromAddr );
      ui->eepromAddr16->setText ( buffer );
      sprintf ( buffer, "%02X", mapper016Info.eepromDataBuf );
      ui->eepromDataBuf16->setText ( buffer );
      break;

   case 28:
      nesMapper028GetInformation(&mapper028Info);
      ui->supervisorRegSel28->setChecked(mapper028Info.regSel&0x80);
      sprintf ( buffer, "%X", mapper028Info.regSel&0x01 );
      ui->regSel28->setText ( buffer );
      sprintf ( buffer, "%02X", mapper028Info.prgMode );
      ui->prgMode28->setText ( buffer );
      sprintf ( buffer, "%02X", mapper028Info.prgSize );
      ui->prgSize28->setText ( buffer );
      sprintf ( buffer, "%02X", mapper028Info.prgOuterBank );
      ui->prgOuterBank28->setText ( buffer );
      sprintf ( buffer, "%02X", mapper028Info.prgInnerBank );
      ui->prgInnerBank28->setText ( buffer );
      break;
   case 69:
      nesMapper069GetInformation(&mapper069Info);
      ui->irqEnabled69->setChecked ( mapper069Info.irqEnabled );
      ui->irqCounterEnabled69->setChecked ( mapper069Info.irqCountEnabled );
      ui->irqAsserted69->setChecked ( mapper069Info.irqAsserted );
      sprintf ( buffer, "%04X", mapper069Info.irqCounter );
      ui->irqCounter69->setText ( buffer );
      sprintf ( buffer, "%02X(6000)",
                mapper069Info.sramOrPrgBank );
      ui->sramOrPrgBank69->setText(buffer);
      ui->sramEnabled69->setChecked(mapper069Info.sramEnabled);
      ui->sramIsSram69->setChecked(mapper069Info.sramIsSram);
      sprintf ( buffer, "%X", mapper069Info.regSelected );
      ui->regSelected69->setText(buffer);
      sprintf ( buffer, "%02X", mapper069Info.regValue );
      ui->regValue69->setText(buffer);
      break;
   case 111:
      reg = nesMapperLowRead(0x5000);
      // red LED
      if ( reg&0x40 )
      {
         ui->redLED->setStyleSheet("");
      }
      else
      {
         ui->redLED->setStyleSheet("QLabel { background-color: red; }");
      }
      // green LED
      if ( reg&0x80 )
      {
         ui->greenLED->setStyleSheet("");
      }
      else
      {
         ui->greenLED->setStyleSheet("QLabel { background-color: green; }");
      }
      ui->nameTableMapping->setText(QString::number((reg&0x20)>>5));
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
