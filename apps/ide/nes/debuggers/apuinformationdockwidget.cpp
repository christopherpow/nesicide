#include "apuinformationdockwidget.h"
#include "ui_apuinformationdockwidget.h"

#include "nes_emulator_core.h"

#include "cobjectregistry.h"

APUInformationDockWidget::APUInformationDockWidget(QWidget *parent) :
    CDebuggerBase(parent),
    ui(new Ui::APUInformationDockWidget)
{
   ui->setupUi(this);
}

APUInformationDockWidget::~APUInformationDockWidget()
{
    delete ui;
}

void APUInformationDockWidget::updateTargetMachine(QString target)
{
   if ( !target.compare("nes") )
   {
      QObject* emulator = CObjectRegistry::instance()->getObject("Emulator");
      QObject* breakpointWatcher = CObjectRegistry::instance()->getObject("Breakpoint Watcher");

      QObject::connect ( emulator, SIGNAL(machineReady()), this, SLOT(updateInformation()) );
      QObject::connect ( emulator, SIGNAL(emulatorReset()), this, SLOT(updateInformation()) );
      QObject::connect ( emulator, SIGNAL(emulatorPaused(bool)), this, SLOT(updateInformation()) );
      QObject::connect ( breakpointWatcher, SIGNAL(breakpointHit()), this, SLOT(updateInformation()) );
   }
}

void APUInformationDockWidget::changeEvent(QEvent* e)
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

void APUInformationDockWidget::showEvent(QShowEvent* /*e*/)
{
   QObject* emulator = CObjectRegistry::instance()->getObject("Emulator");
   QObject::connect ( emulator, SIGNAL(updateDebuggers()), this, SLOT(updateInformation()) );
   updateInformation();
}

void APUInformationDockWidget::hideEvent(QHideEvent* /*e*/)
{
   QObject* emulator = CObjectRegistry::instance()->getObject("Emulator");
   QObject::disconnect ( emulator, SIGNAL(updateDebuggers()), this, SLOT(updateInformation()) );
}

void APUInformationDockWidget::updateInformation()
{
   CBreakpointInfo* pBreakpoints = nesGetBreakpointDatabase();
   int idx;
   char buffer[16];

   nesGetNesSnapshot(&m_nesState);

   sprintf ( buffer, "%d", m_nesState.apu.cycle );
   ui->apuCycle->setText ( buffer );

   ui->apuSequencerMode->setText ( m_nesState.apu.sequencerMode==0?"4-step":"5-step" );

   ui->lengthCounter1->setValue ( m_nesState.apu.lengthCounter[0] );
   ui->lengthCounter2->setValue ( m_nesState.apu.lengthCounter[1] );
   ui->lengthCounter3->setValue ( m_nesState.apu.lengthCounter[2] );
   ui->lengthCounter4->setValue ( m_nesState.apu.lengthCounter[3] );
   ui->lengthCounter5->setValue ( m_nesState.apu.lengthCounter[4] );

   ui->linearCounter3->setValue ( m_nesState.apu.triangleLinearCounter );

   ui->dac1->setValue ( m_nesState.apu.dac[0] );
   ui->dac2->setValue ( m_nesState.apu.dac[1] );
   ui->dac3->setValue ( m_nesState.apu.dac[2] );
   ui->dac4->setValue ( m_nesState.apu.dac[3] );
   ui->dac5->setValue ( m_nesState.apu.dac[4] );

   ui->irqEnabled5->setChecked ( m_nesState.apu.dmcIrqEnabled );
   ui->irqAsserted5->setChecked ( m_nesState.apu.dmcIrqAsserted );

   sprintf ( buffer, "%04X", m_nesState.apu.dmaSampleAddress );
   ui->sampleAddr5->setText ( buffer );
   sprintf ( buffer, "%04X", m_nesState.apu.dmaSampleLength );
   ui->sampleLength5->setText ( buffer );
   sprintf ( buffer, "%04X", m_nesState.apu.dmaSamplePosition );
   ui->samplePos5->setText ( buffer );

   sprintf ( buffer, "%02X", m_nesState.apu.dmcDmaBuffer );
   ui->sampleBufferContents5->setText ( buffer );
   ui->sampleBufferFull5->setChecked ( m_nesState.apu.dmcDmaFull );

   // Check breakpoints for hits and highlight if necessary...
   for ( idx = 0; idx < pBreakpoints->GetNumBreakpoints(); idx++ )
   {
      BreakpointInfo* pBreakpoint = pBreakpoints->GetBreakpoint(idx);

      if ( pBreakpoint->hit )
      {
         if ( pBreakpoint->type == eBreakOnAPUEvent )
         {
            // Update display...
            show();
         }
      }
   }
}
