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
   ui->setupUi(this);

   ui->tabWidget->setCurrentIndex(InternalsPage);

   // Force UI update so it doesn't look uninitialized completely.
   machineReady();
}

MapperInformationDockWidget::~MapperInformationDockWidget()
{
    delete ui;
}

void MapperInformationDockWidget::updateTargetMachine(QString target)
{
   QObject* breakpointWatcher = CObjectRegistry::getObject("Breakpoint Watcher");
   QObject* emulator = CObjectRegistry::getObject("Emulator");

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

void MapperInformationDockWidget::showEvent(QShowEvent* e)
{
   QObject* emulator = CObjectRegistry::getObject("Emulator");

   QObject::connect ( emulator, SIGNAL(updateDebuggers()), this, SLOT(updateInformation()) );
   updateInformation();
}

void MapperInformationDockWidget::hideEvent(QHideEvent* e)
{
   QObject* emulator = CObjectRegistry::getObject("Emulator");

   QObject::disconnect ( emulator, SIGNAL(updateDebuggers()), this, SLOT(updateInformation()) );
}

void MapperInformationDockWidget::machineReady()
{
   char buffer [ 128 ];
   sprintf ( buffer, "Mapper %d: %s Information", nesGetMapper(), mapperNameFromID(nesGetMapper()) );
   setWindowTitle(buffer);
   ui->tabWidget->setTabEnabled(CPUToPRGPage, nesMapperRemapsPRGROM());
   ui->tabWidget->setTabEnabled(PPUToCHRPage, nesMapperRemapsCHRMEM());
   ui->internalInfo->setCurrentIndex(mapperInspectorPageFromID(nesGetMapper()));
   if ( ui->internalInfo->currentIndex() )
   {
      ui->tabWidget->setTabEnabled(InternalsPage, true);
   }
   else
   {
      ui->tabWidget->setTabEnabled(InternalsPage, false);
   }
}

void MapperInformationDockWidget::updateInformation()
{
   CBreakpointInfo* pBreakpoints = nesGetBreakpointDatabase();
   int idx;
   char buffer [ 32 ];
   nesMapper001Info mapper001Info;
   nesMapper004Info mapper004Info;
   nesMapper069Info mapper069Info;
   uint16_t mirroring[4];

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
   sprintf ( buffer, "%d (%dKB)", nesGetNumPRGROMBanks(), nesGetNumPRGROMBanks()*MEM_8KB  );
   ui->numPrgBanks->setText ( buffer );
   sprintf ( buffer, "%d (%dKB)", nesGetNumCHRROMBanks(), nesGetNumCHRROMBanks()*MEM_8KB );
   ui->numChrBanks->setText ( buffer );

   // Show PRG-ROM absolute addresses...
   sprintf ( buffer, "%02X(%05X)", nesGetPRGROMAbsoluteAddress(0x8000)>>13, nesGetPRGROMAbsoluteAddress(0x8000) );
   ui->prg0->setText ( buffer );
   sprintf ( buffer, "%02X(%05X)", nesGetPRGROMAbsoluteAddress(0xA000)>>13, nesGetPRGROMAbsoluteAddress(0xA000) );
   ui->prg1->setText ( buffer );
   sprintf ( buffer, "%02X(%05X)", nesGetPRGROMAbsoluteAddress(0xC000)>>13, nesGetPRGROMAbsoluteAddress(0xC000) );
   ui->prg2->setText ( buffer );
   sprintf ( buffer, "%02X(%05X)", nesGetPRGROMAbsoluteAddress(0xE000)>>13, nesGetPRGROMAbsoluteAddress(0xE000) );
   ui->prg3->setText ( buffer );

   // Show CHR memory absolute addresses...
   sprintf ( buffer, "%02X:%04X(%05X)", nesGetCHRMEMAbsoluteAddress(0x0000)>>13, nesGetCHRMEMAbsoluteAddress(0x0000)&MASK_8KB, nesGetCHRMEMAbsoluteAddress(0x0000) );
   ui->chr0->setText ( buffer );
   sprintf ( buffer, "%02X:%04X(%05X)", nesGetCHRMEMAbsoluteAddress(0x0400)>>13, nesGetCHRMEMAbsoluteAddress(0x0400)&MASK_8KB, nesGetCHRMEMAbsoluteAddress(0x0400) );
   ui->chr1->setText ( buffer );
   sprintf ( buffer, "%02X:%04X(%05X)", nesGetCHRMEMAbsoluteAddress(0x0800)>>13, nesGetCHRMEMAbsoluteAddress(0x0800)&MASK_8KB, nesGetCHRMEMAbsoluteAddress(0x0800) );
   ui->chr2->setText ( buffer );
   sprintf ( buffer, "%02X:%04X(%05X)", nesGetCHRMEMAbsoluteAddress(0x0C00)>>13, nesGetCHRMEMAbsoluteAddress(0x0C00)&MASK_8KB, nesGetCHRMEMAbsoluteAddress(0x0C00) );
   ui->chr3->setText ( buffer );
   sprintf ( buffer, "%02X:%04X(%05X)", nesGetCHRMEMAbsoluteAddress(0x1000)>>13, nesGetCHRMEMAbsoluteAddress(0x1000)&MASK_8KB, nesGetCHRMEMAbsoluteAddress(0x1000) );
   ui->chr4->setText ( buffer );
   sprintf ( buffer, "%02X:%04X(%05X)", nesGetCHRMEMAbsoluteAddress(0x1400)>>13, nesGetCHRMEMAbsoluteAddress(0x1400)&MASK_8KB, nesGetCHRMEMAbsoluteAddress(0x1400) );
   ui->chr5->setText ( buffer );
   sprintf ( buffer, "%02X:%04X(%05X)", nesGetCHRMEMAbsoluteAddress(0x1800)>>13, nesGetCHRMEMAbsoluteAddress(0x1800)&MASK_8KB, nesGetCHRMEMAbsoluteAddress(0x1800) );
   ui->chr6->setText ( buffer );
   sprintf ( buffer, "%02X:%04X(%05X)", nesGetCHRMEMAbsoluteAddress(0x1C00)>>13, nesGetCHRMEMAbsoluteAddress(0x1C00)&MASK_8KB, nesGetCHRMEMAbsoluteAddress(0x1C00) );
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
