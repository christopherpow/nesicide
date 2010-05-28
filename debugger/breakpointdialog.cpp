#include "breakpointdialog.h"
#include "ui_breakpointdialog.h"

#include "cnesrom.h"
#include "cnes6502.h"
#include "cnesppu.h"
#include "cnesapu.h"

#include "main.h"

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
   ui->tableView->setModel ( model );

   m_pRegister = NULL;
   m_pBitfield = NULL;
   m_pEvent = NULL;

   QObject::connect(breakpointWatcher, SIGNAL(breakpointHit()), this, SLOT(updateData()));
   QObject::connect(emulator, SIGNAL(breakpointClear()), this, SLOT(updateData()));
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

void BreakpointDialog::showEvent(QShowEvent *)
{
   CBreakpointInfo* pBreakpoints = CNES::BREAKPOINTS();
   if ( pBreakpoints->GetNumBreakpoints() == NUM_BREAKPOINTS )
   {
      ui->addButton->setEnabled(false);
   }
   else
   {
      ui->addButton->setEnabled(true);
   }
   model->layoutChangedEvent();
   ui->tableView->resizeColumnToContents(0);
}

void BreakpointDialog::updateData()
{
   CBreakpointInfo* pBreakpoints = CNES::BREAKPOINTS();
   int idx;

   model->layoutChangedEvent();

   for ( idx = 0; idx < pBreakpoints->GetNumBreakpoints(); idx++ )
   {
      BreakpointInfo* pBreakpoint = pBreakpoints->GetBreakpoint(idx);
      if ( pBreakpoint->hit )
      {
         emit showMe();
         ui->tableView->setCurrentIndex(model->index(idx,0));
      }
   }
}

void BreakpointDialog::on_type_currentIndexChanged(int index)
{
   CBreakpointEventInfo** pBreakpointEventInfo = NULL;
   int idx;

   ui->addButton->setEnabled(true);
   ui->item1label->setText("Data1:");
   ui->item2label->setText("Data2:");
   ui->event->setCurrentIndex(0);
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
      case eBreakOnOAMPortalAccess:
      case eBreakOnOAMPortalRead:
      case eBreakOnOAMPortalWrite:
      case eBreakOnPPUPortalAccess:
      case eBreakOnPPUPortalRead:
      case eBreakOnPPUPortalWrite:
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
         ui->event->clear();
         pBreakpointEventInfo = C6502::BREAKPOINTEVENTS();
         for ( idx = 0; idx < C6502::NUMBREAKPOINTEVENTS(); idx++ )
         {
            ui->event->addItem ( pBreakpointEventInfo[idx]->GetName() );
         }
         ui->event->setCurrentIndex ( 0 );
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
         ui->event->clear();
         pBreakpointEventInfo = CPPU::BREAKPOINTEVENTS();
         for ( idx = 0; idx < CPPU::NUMBREAKPOINTEVENTS(); idx++ )
         {
            ui->event->addItem ( pBreakpointEventInfo[idx]->GetName() );
         }
         ui->event->setCurrentIndex ( 0 );
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
         ui->event->clear();
         pBreakpointEventInfo = CAPU::BREAKPOINTEVENTS();
         for ( idx = 0; idx < CAPU::NUMBREAKPOINTEVENTS(); idx++ )
         {
            ui->event->addItem ( pBreakpointEventInfo[idx]->GetName() );
         }
         ui->event->setCurrentIndex ( 0 );
      break;
      case eBreakOnMapperState:
         ui->itemWidget->setCurrentIndex ( eBreakpointItemRegister );
         ui->conditionWidget->setCurrentIndex ( eBreakpointConditionTest );
         ui->dataWidget->setCurrentIndex ( eBreakpointDataPick );
         ui->reg->clear();
         ui->bitfield->clear();
         if ( CROM::REGISTERS() )
         {
            for ( idx = 0; idx < CROM::NUMREGISTERS(); idx++ )
            {
               ui->reg->addItem ( CROM::REGISTERS()[idx]->GetName() );
            }
         }
         else
         {
            ui->addButton->setEnabled(false);
         }
      break;
      case eBreakOnMapperEvent:
         ui->itemWidget->setCurrentIndex ( eBreakpointItemEvent );
         ui->conditionWidget->setCurrentIndex ( eBreakpointConditionNone );
         ui->dataWidget->setCurrentIndex ( eBreakpointDataNone );
         ui->event->clear();
         pBreakpointEventInfo = CROM::BREAKPOINTEVENTS();
         for ( idx = 0; idx < CROM::NUMBREAKPOINTEVENTS(); idx++ )
         {
            ui->event->addItem ( pBreakpointEventInfo[idx]->GetName() );
         }
         ui->event->setCurrentIndex ( 0 );
      break;
   }
}

void BreakpointDialog::on_reg_currentIndexChanged(int index)
{
   int idx;
   if ( index >= 0 )
   {
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
            if ( CROM::NUMREGISTERS() > 0 )
            {
               m_pRegister = CROM::REGISTERS() [ ui->reg->currentIndex() ];
            }
            else
            {
               m_pRegister = NULL;
            }
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
   int  item1 = 0;
   int  item2 = 0;
   int  data = 0;
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

   if ( pBreakpoints->GetNumBreakpoints() == NUM_BREAKPOINTS )
   {
      ui->addButton->setEnabled(false);
   }

   model->layoutChangedEvent();

   emit breakpointsChanged();
}

void BreakpointDialog::DisplayBreakpoint ( int idx )
{
   char buffer [ 16 ];
   CBreakpointInfo* pBreakpoints = CNES::BREAKPOINTS();
   BreakpointInfo* pBreakpoint = pBreakpoints->GetBreakpoint ( idx );

   ui->type->setCurrentIndex ( pBreakpoint->type );
   ui->itemWidget->setCurrentIndex ( pBreakpoint->itemType );
   ui->item1label->setText("Data1:");
   ui->item2label->setText("Data2:");
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
      case eBreakpointItemEvent:
         if ( pBreakpoint->pEvent->GetElementRadix() == 16 )
         {
            sprintf ( buffer, "%X", pBreakpoint->item1 );
         }
         else
         {
            sprintf ( buffer, "%d", pBreakpoint->item1 );
         }
         ui->eventData1->setText ( buffer );
         if ( pBreakpoint->pEvent->GetElementRadix() == 16 )
         {
            sprintf ( buffer, "%X", pBreakpoint->item2 );
         }
         else
         {
            sprintf ( buffer, "%d", pBreakpoint->item2 );
         }
         ui->eventData2->setText ( buffer );
         ui->item1label->setVisible ( false );
         ui->eventData1->setVisible ( false );
         ui->item2label->setVisible ( false );
         ui->eventData2->setVisible ( false );
         if ( pBreakpoint->pEvent->GetItemName(0) )
         {
            ui->item1label->setText(pBreakpoint->pEvent->GetItemName(0));
            ui->eventData1->setVisible ( true );
            ui->item1label->setVisible ( true );
         }
         if ( pBreakpoint->pEvent->GetItemName(1) )
         {
            ui->item2label->setText(pBreakpoint->pEvent->GetItemName(1));
            ui->eventData2->setVisible ( true );
            ui->item2label->setVisible ( true );
         }
      break;
      case eBreakpointItemNone:
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
      case eBreakpointDataNone:
      break;
   }
}

void BreakpointDialog::on_tableView_activated(QModelIndex index)
{
   DisplayBreakpoint ( index.row() );
}

void BreakpointDialog::on_tableView_clicked(QModelIndex index)
{
   DisplayBreakpoint ( index.row() );
}

void BreakpointDialog::on_tableView_doubleClicked(QModelIndex index)
{
   DisplayBreakpoint ( index.row() );
}

void BreakpointDialog::on_tableView_entered(QModelIndex index)
{
   DisplayBreakpoint ( index.row() );
}

void BreakpointDialog::on_tableView_pressed(QModelIndex index)
{
   DisplayBreakpoint ( index.row() );
}

void BreakpointDialog::on_event_currentIndexChanged(int)
{
   if ( ui->event->currentIndex() >= 0 )
   {
      switch ( ui->type->currentIndex() )
      {
         case eBreakOnCPUEvent:
            m_pEvent = C6502::BREAKPOINTEVENTS()[ui->event->currentIndex()];
         break;
         case eBreakOnPPUEvent:
            m_pEvent = CPPU::BREAKPOINTEVENTS()[ui->event->currentIndex()];
         break;
         case eBreakOnAPUEvent:
            m_pEvent = CAPU::BREAKPOINTEVENTS()[ui->event->currentIndex()];
         break;
         case eBreakOnMapperEvent:
            m_pEvent = CROM::BREAKPOINTEVENTS()[ui->event->currentIndex()];
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
            ui->eventData1->setVisible ( true );
            ui->eventData2->setVisible ( true );
            ui->item1label->setVisible ( true );
            ui->item2label->setVisible ( true );
         }
         else if ( m_pEvent->GetNumElements() == 1 )
         {
            ui->eventData1->setVisible ( true );
            ui->eventData2->setVisible ( false );
            ui->item1label->setVisible ( true );
            ui->item2label->setVisible ( false );
         }
         else
         {
            ui->eventData1->setVisible ( false );
            ui->eventData2->setVisible ( false );
            ui->item1label->setVisible ( false );
            ui->item2label->setVisible ( false );
         }
         if ( m_pEvent->GetItemName(0) )
         {
            ui->item1label->setText(m_pEvent->GetItemName(0));
         }
         if ( m_pEvent->GetItemName(1) )
         {
            ui->item2label->setText(m_pEvent->GetItemName(1));
         }
      }
   }
}

void BreakpointDialog::on_removeButton_clicked()
{
   CBreakpointInfo* pBreakpoints = CNES::BREAKPOINTS();

   if ( ui->tableView->currentIndex().row() >= 0 )
   {
      pBreakpoints->RemoveBreakpoint(ui->tableView->currentIndex().row());

      ui->addButton->setEnabled(true);
   }

   model->layoutChangedEvent();

   emit breakpointsChanged();
}

void BreakpointDialog::on_addr1_textChanged(QString )
{
    ui->addr2->setText(ui->addr1->text());
}

void BreakpointDialog::on_modifyButton_clicked()
{
   CBreakpointInfo* pBreakpoints = CNES::BREAKPOINTS();
   bool ok;
   int  item1 = 0;
   int  item2 = 0;
   int  data = 0;
   int  event = 0;
   QModelIndex sel = ui->tableView->currentIndex();

   if ( sel.row() >= 0 )
   {
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
   }

   pBreakpoints->ModifyBreakpoint ( sel.row(),
                                    (eBreakpointType)ui->type->currentIndex(),
                                    (eBreakpointItemType)ui->itemWidget->currentIndex(),
                                    event,
                                    item1,
                                    item2,
                                    (eBreakpointConditionType)ui->conditionWidget->currentIndex(),
                                    ui->condition->currentIndex(),
                                    (eBreakpointDataType)ui->dataWidget->currentIndex(),
                                    data );

   model->layoutChangedEvent();

   emit breakpointsChanged();
}

void BreakpointDialog::on_endisButton_clicked()
{
   CBreakpointInfo* pBreakpoints = CNES::BREAKPOINTS();
   QModelIndex sel = ui->tableView->currentIndex();

   if ( sel.row() >= 0 )
   {
      pBreakpoints->ToggleEnabled ( sel.row() );

      model->layoutChangedEvent();

      emit breakpointsChanged();
   }
}
