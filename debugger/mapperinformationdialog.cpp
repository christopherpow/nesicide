#include "mapperinformationdialog.h"
#include "ui_mapperinformationdialog.h"

#include "main.h"

#include "cnes.h"
#include "cnesrom.h"

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
   if ( mapperfunc[CROM::MAPPER()].remapPrg )
   {
      ui->prgBanking->setCurrentIndex(1);
   }
   else
   {
      ui->prgBanking->setCurrentIndex(0);
   }
   if ( mapperfunc[CROM::MAPPER()].remapChr )
   {
      ui->chrBanking->setCurrentIndex(1);
   }
   else
   {
      ui->chrBanking->setCurrentIndex(0);
   }
   ui->internalInfo->setCurrentIndex(CROM::MAPPER());
}

void MapperInformationDialog::updateInformation()
{
   CBreakpointInfo* pBreakpoints = CNES::BREAKPOINTS();
   int idx;

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
