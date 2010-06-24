#include "apuinformationdialog.h"
#include "ui_apuinformationdialog.h"

#include "main.h"

#include "cnes.h"
#include "cnesapu.h"

APUInformationDialog::APUInformationDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::APUInformationDialog)
{
    ui->setupUi(this);
    QObject::connect ( emulator, SIGNAL(emulatedFrame()), this, SLOT(updateInformation()) );
    QObject::connect ( emulator, SIGNAL(emulatorReset()), this, SLOT(updateInformation()) );
    QObject::connect ( breakpointWatcher, SIGNAL(breakpointHit()), this, SLOT(updateInformation()) );
}

APUInformationDialog::~APUInformationDialog()
{
    delete ui;
}

void APUInformationDialog::changeEvent(QEvent *e)
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

void APUInformationDialog::showEvent(QShowEvent*)
{
}

void APUInformationDialog::updateInformation()
{
   CBreakpointInfo* pBreakpoints = CNES::BREAKPOINTS();
   int idx;
   char buffer[16];
   unsigned char temp1, temp2, temp3, temp4, temp5;
   bool tempb1, tempb2;
   unsigned short tempus1, tempus2, tempus3, tempus4, tempus5;

   sprintf ( buffer, "%d", CAPU::CYCLES() );
   ui->apuCycle->setText ( buffer );

   CAPU::LENGTHCOUNTERS ( &tempus1, &tempus2, &tempus3, &tempus4, &tempus5 );
   ui->lengthCounter1->setValue ( tempus1 );
   ui->lengthCounter2->setValue ( tempus2 );
   ui->lengthCounter3->setValue ( tempus3 );
   ui->lengthCounter4->setValue ( tempus4 );
   ui->lengthCounter5->setValue ( tempus5 );

   CAPU::LINEARCOUNTER ( &temp3 );
   ui->linearCounter3->setValue ( temp3 );

   CAPU::GETDACS ( &temp1, &temp2, &temp3, &temp4, &temp5 );
   ui->dac1->setValue ( temp1 );
   ui->dac2->setValue ( temp2 );
   ui->dac3->setValue ( temp3 );
   ui->dac4->setValue ( temp4 );
   ui->dac5->setValue ( temp5 );

   CAPU::DMCIRQ ( &tempb1, &tempb2 );
   ui->irqEnabled5->setChecked ( tempb1 );
   ui->irqAsserted5->setChecked ( tempb2 );

   CAPU::SAMPLEINFO ( &tempus1, &tempus2, &tempus3 );
   sprintf ( buffer, "$%04X", tempus1 );
   ui->sampleAddr5->setText ( buffer );
   sprintf ( buffer, "$%04X", tempus2 );
   ui->sampleLength5->setText ( buffer );
   sprintf ( buffer, "$%04X", tempus3 );
   ui->samplePos5->setText ( buffer );

   CAPU::DMAINFO ( &temp1, &tempb1 );
   sprintf ( buffer, "$%02X", temp1 );
   ui->sampleBufferContents5->setText ( buffer );
   ui->sampleBufferFull5->setChecked ( tempb1 );

   // Check breakpoints for hits and highlight if necessary...
   for ( idx = 0; idx < pBreakpoints->GetNumBreakpoints(); idx++ )
   {
      BreakpointInfo* pBreakpoint = pBreakpoints->GetBreakpoint(idx);
      if ( pBreakpoint->hit )
      {
         if ( pBreakpoint->type == eBreakOnAPUEvent )
         {
            // Update display...
            emit showMe();
         }
      }
   }
}
