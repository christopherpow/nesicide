#include "breakpointdialog.h"
#include "ui_breakpointdialog.h"

#include "cnesrom.h"
#include "cnes6502.h"
#include "cnesppu.h"
#include "cnesapu.h"

BreakpointDialog::BreakpointDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BreakpointDialog)
{
   ui->setupUi(this);
   ui->itemWidget->setCurrentIndex ( eBreakpointItemAddress );
   ui->conditionWidget->setCurrentIndex ( eBreakpointConditionNone );
   ui->dataWidget->setCurrentIndex ( eBreakpointDataNone );
   ui->type->setCurrentIndex ( eBreakOnCPUExecution );

   model = new CBreakpointDisplayModel();
   ui->listView->setModel ( model );

   m_pRegister = NULL;
   m_pBitfield = NULL;
   m_pEvent = NULL;
}

BreakpointDialog::~BreakpointDialog()
{
    delete ui;
}

void BreakpointDialog::changeEvent(QEvent *e)
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

void BreakpointDialog::on_type_currentIndexChanged(int index)
{
   int idx;

   switch ( index )
   {
      case eBreakOnCPUExecution:
         ui->itemWidget->setCurrentIndex ( eBreakpointItemAddress );
         ui->conditionWidget->setCurrentIndex ( eBreakpointConditionNone );
         ui->dataWidget->setCurrentIndex ( eBreakpointDataNone );
      break;
      case eBreakOnCPUMemoryAccess:
      case eBreakOnCPUMemoryRead:
      case eBreakOnCPUMemoryWrite:
         ui->itemWidget->setCurrentIndex ( eBreakpointItemAddress );
         ui->conditionWidget->setCurrentIndex ( eBreakpointConditionTest );
         ui->dataWidget->setCurrentIndex ( eBreakpointDataPure );
      break;
      case eBreakOnCPUState:
         ui->itemWidget->setCurrentIndex ( eBreakpointItemRegister );
         ui->conditionWidget->setCurrentIndex ( eBreakpointConditionTest );
         ui->dataWidget->setCurrentIndex ( eBreakpointDataPick );
         ui->reg->clear();
         ui->bitfield->clear();
         for ( idx = 0; idx < NUM_CPU_REGISTERS; idx++ )
         {
            ui->reg->addItem ( C6502::REGISTERS()[idx]->GetName() );
         }
      break;
      case eBreakOnCPUEvent:
         ui->itemWidget->setCurrentIndex ( eBreakpointItemEvent );
         ui->conditionWidget->setCurrentIndex ( eBreakpointConditionNone );
         ui->dataWidget->setCurrentIndex ( eBreakpointDataNone );
      break;
      case eBreakOnPPUFetch:
         ui->itemWidget->setCurrentIndex ( eBreakpointItemAddress );
         ui->conditionWidget->setCurrentIndex ( eBreakpointConditionTest );
         ui->dataWidget->setCurrentIndex ( eBreakpointDataPure );
      break;
      case eBreakOnPPUState:
         ui->itemWidget->setCurrentIndex ( eBreakpointItemRegister );
         ui->conditionWidget->setCurrentIndex ( eBreakpointConditionTest );
         ui->dataWidget->setCurrentIndex ( eBreakpointDataPick );
         ui->reg->clear();
         ui->bitfield->clear();
         for ( idx = 0; idx < NUM_PPU_REGISTERS; idx++ )
         {
            ui->reg->addItem ( CPPU::REGISTERS()[idx]->GetName() );
         }
      break;
      case eBreakOnPPUEvent:
         ui->itemWidget->setCurrentIndex ( eBreakpointItemEvent );
         ui->conditionWidget->setCurrentIndex ( eBreakpointConditionNone );
         ui->dataWidget->setCurrentIndex ( eBreakpointDataNone );
         for ( idx = 0; idx < CPPU::NUMBREAKPOINTEVENTS(); idx++ )
         {
            ui->event->addItem ( CPPU::BREAKPOINTEVENTS()[idx]->GetName() );
         }
      break;
      case eBreakOnAPUState:
         ui->itemWidget->setCurrentIndex ( eBreakpointItemRegister );
         ui->conditionWidget->setCurrentIndex ( eBreakpointConditionTest );
         ui->dataWidget->setCurrentIndex ( eBreakpointDataPick );
         ui->reg->clear();
         ui->bitfield->clear();
         for ( idx = 0; idx < NUM_APU_REGISTERS; idx++ )
         {
            ui->reg->addItem ( CAPU::REGISTERS()[idx]->GetName() );
         }
      break;
      case eBreakOnAPUEvent:
         ui->itemWidget->setCurrentIndex ( eBreakpointItemEvent );
         ui->conditionWidget->setCurrentIndex ( eBreakpointConditionNone );
         ui->dataWidget->setCurrentIndex ( eBreakpointDataNone );
      break;
      case eBreakOnMapperState:
         ui->itemWidget->setCurrentIndex ( eBreakpointItemRegister );
         ui->conditionWidget->setCurrentIndex ( eBreakpointConditionTest );
         ui->dataWidget->setCurrentIndex ( eBreakpointDataPick );
         ui->reg->clear();
         ui->bitfield->clear();
         for ( idx = 0; idx < CROM::NUMREGISTERS(); idx++ )
         {
            ui->reg->addItem ( CROM::REGISTERS()[idx]->GetName() );
         }
      break;
      case eBreakOnMapperEvent:
         ui->itemWidget->setCurrentIndex ( eBreakpointItemEvent );
         ui->conditionWidget->setCurrentIndex ( eBreakpointConditionNone );
         ui->dataWidget->setCurrentIndex ( eBreakpointDataNone );
      break;
   }
}

void BreakpointDialog::on_reg_currentIndexChanged(int index)
{
   int idx;
   switch ( ui->type->currentIndex() )
   {
      case eBreakOnCPUState:
         m_pRegister = C6502::REGISTERS() [ ui->reg->currentIndex() ];
      break;
      case eBreakOnPPUState:
         m_pRegister = CPPU::REGISTERS() [ ui->reg->currentIndex() ];
      break;
      case eBreakOnAPUState:
         m_pRegister = CAPU::REGISTERS() [ ui->reg->currentIndex() ];
      break;
      case eBreakOnMapperState:
         m_pRegister = CROM::REGISTERS() [ ui->reg->currentIndex() ];
      break;
      default:
         m_pRegister = NULL;
      break;
   }
   ui->bitfield->clear();
   if ( m_pRegister )
   {
      for ( idx = 0; idx < m_pRegister->GetNumBitfields(); idx++ )
      {
         CBitfieldData* pBitfield = m_pRegister->GetBitfield(idx);
         ui->bitfield->addItem ( pBitfield->GetName() );
      }
      if ( m_pRegister->GetNumBitfields() > 1 )
      {
         ui->bitfield->setEnabled ( true );
      }
      else
      {
         ui->dataWidget->setCurrentIndex ( eBreakpointDataPure );
         ui->bitfield->setEnabled ( false );
      }
   }
}

void BreakpointDialog::on_bitfield_currentIndexChanged(int index)
{
   int idx;

   ui->data2->clear();
   if ( m_pRegister )
   {
      if ( index >= 0 )
      {
         m_pBitfield = m_pRegister->GetBitfield ( index );
      }
      else
      {
         m_pBitfield = NULL;
      }
      if ( m_pBitfield )
      {
         if ( m_pBitfield->GetNumValues() )
         {
            ui->dataWidget->setCurrentIndex ( eBreakpointDataPick );
            for ( idx = 0; idx < m_pBitfield->GetNumValues(); idx++ )
            {
               ui->data2->addItem ( m_pBitfield->GetValueByIndex(idx) );
            }
         }
         else
         {
            ui->dataWidget->setCurrentIndex ( eBreakpointDataPure );
         }
      }
   }
}

void BreakpointDialog::on_addButton_clicked()
{
   CBreakpointInfo* pBreakpoints = CNES::BREAKPOINTS();
   bool ok;
   int  item1;
   int  item2;
   int  data;
   int  event = 0;

   switch ( ui->itemWidget->currentIndex() )
   {
      case eBreakpointItemNone:
         // No item...
      break;
      case eBreakpointItemAddress:
         // Address item...
         item1 = ui->addr1->text().toInt(&ok, 16);
         item2 = ui->addr2->text().toInt(&ok, 16);
      break;
      case eBreakpointItemRegister:
         // Register item...
         item1 = ui->reg->currentIndex ();
         item2 = ui->bitfield->currentIndex ();
         // sometimes no bitfield...
         if ( item2 < 0 ) item2 = 0;
      break;
      case eBreakpointItemEvent:
         if ( m_pEvent )
         {
            item1 = ui->eventData1->text().toInt(&ok, m_pEvent->GetElementRadix());
            item2 = ui->eventData2->text().toInt(&ok, m_pEvent->GetElementRadix());
            event = ui->event->currentIndex ();
         }
      break;
   }
   switch ( ui->dataWidget->currentIndex() )
   {
      case eBreakpointDataNone:
         // No data...
      break;
      case eBreakpointDataPure:
         // Direct value data...
         data = ui->data1->text().toInt(&ok, 16);
      break;
      case eBreakpointDataPick:
         // Picklist data...
         data = ui->data2->currentIndex ();
      break;
   }

   pBreakpoints->AddBreakpoint ( (eBreakpointType)ui->type->currentIndex(),
                                 (eBreakpointItemType)ui->itemWidget->currentIndex(),
                                 event,
                                 item1,
                                 item2,
                                 (eBreakpointConditionType)ui->conditionWidget->currentIndex(),
                                 ui->condition->currentIndex(),
                                 (eBreakpointDataType)ui->dataWidget->currentIndex(),
                                 data );

   model->layoutChangedEvent();
}

void BreakpointDialog::DisplayBreakpoint ( int idx )
{
   char buffer [ 16 ];
   CBreakpointInfo* pBreakpoints = CNES::BREAKPOINTS();
   BreakpointInfo* pBreakpoint = pBreakpoints->GetBreakpoint ( idx );

   ui->type->setCurrentIndex ( pBreakpoint->type );
   ui->itemWidget->setCurrentIndex ( pBreakpoint->itemType );
   switch ( pBreakpoint->itemType )
   {
      case eBreakpointItemAddress:
         sprintf ( buffer, "%X", pBreakpoint->item1 );
         ui->addr1->setText ( buffer );
         sprintf ( buffer, "%X", pBreakpoint->item2 );
         ui->addr2->setText ( buffer );
      break;
      case eBreakpointItemRegister:
         ui->reg->setCurrentIndex ( pBreakpoint->item1 );
         ui->bitfield->setCurrentIndex ( pBreakpoint->item2 );
      break;
   }
   ui->conditionWidget->setCurrentIndex ( pBreakpoint->conditionType );
   ui->condition->setCurrentIndex ( pBreakpoint->condition );
   ui->dataWidget->setCurrentIndex ( pBreakpoint->dataType );
   switch ( pBreakpoint->dataType )
   {
      case eBreakpointDataPure:
         sprintf ( buffer, "%X", pBreakpoint->data );
         ui->data1->setText ( buffer );
      break;
      case eBreakpointDataPick:
         ui->data2->setCurrentIndex ( pBreakpoint->data );
      break;
   }
}

void BreakpointDialog::on_listView_activated(QModelIndex index)
{
   DisplayBreakpoint ( index.row() );
}

void BreakpointDialog::on_listView_clicked(QModelIndex index)
{
   DisplayBreakpoint ( index.row() );
}

void BreakpointDialog::on_listView_doubleClicked(QModelIndex index)
{
   DisplayBreakpoint ( index.row() );
}

void BreakpointDialog::on_listView_entered(QModelIndex index)
{
   DisplayBreakpoint ( index.row() );
}

void BreakpointDialog::on_listView_pressed(QModelIndex index)
{
   DisplayBreakpoint ( index.row() );
}

void BreakpointDialog::on_event_currentIndexChanged(int index)
{
   switch ( ui->type->currentIndex() )
   {
      case eBreakOnCPUEvent:
         // No events yet...
         m_pEvent = NULL;
      break;
      case eBreakOnPPUEvent:
         m_pEvent = CPPU::BREAKPOINTEVENTS()[ui->event->currentIndex()];
      break;
      case eBreakOnAPUEvent:
         // No events yet...
         m_pEvent = NULL;
      break;
      case eBreakOnMapperEvent:
         // No events yet...
         m_pEvent = NULL;
      break;
      default:
         // No events...
         m_pEvent = NULL;
      break;
   }
   if ( m_pEvent )
   {
      if ( m_pEvent->GetNumElements() == 2 )
      {
         ui->eventData1->setEnabled ( true );
         ui->eventData2->setEnabled ( true );
      }
      else if ( m_pEvent->GetNumElements() == 1 )
      {
         ui->eventData1->setEnabled ( true );
         ui->eventData2->setEnabled ( false );
      }
      else
      {
         ui->eventData1->setEnabled ( false );
         ui->eventData2->setEnabled ( false );
      }
   }
}

void BreakpointDialog::on_removeButton_clicked()
{
   CBreakpointInfo* pBreakpoints = CNES::BREAKPOINTS();

   if ( ui->listView->currentIndex().row() >= 0 )
   {
      pBreakpoints->RemoveBreakpoint(ui->listView->currentIndex().row());
   }

   model->layoutChangedEvent();
}
