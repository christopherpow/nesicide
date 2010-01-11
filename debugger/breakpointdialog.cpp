#include "breakpointdialog.h"
#include "ui_breakpointdialog.h"

#include "cnesrom.h"

static QStringList brkptConditions;

BreakpointDialog::BreakpointDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BreakpointDialog)
{
   ui->setupUi(this);
   brkptConditions.append ( "Equals" );
   brkptConditions.append ( "Does Not Equal" );
   brkptConditions.append ( "Is Less Than" );
   brkptConditions.append ( "Is Greater Than" );
   ui->itemWidget->setCurrentIndex ( 0 );
   ui->conditionWidget->setCurrentIndex ( 0 );
   ui->dataWidget->setCurrentIndex ( 0 );
   ui->type->setCurrentIndex ( 0 );

   m_pRegister = NULL;
   m_pBitfield = NULL;
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

   ui->condition->clear();
   ui->condition->addItems ( brkptConditions );

   switch ( index )
   {
      case 0:
         ui->itemWidget->setCurrentIndex ( 0 );
         ui->conditionWidget->setCurrentIndex ( 0 );
         ui->dataWidget->setCurrentIndex ( 0 );
      break;
      case 1:
      case 2:
      case 3:
         ui->itemWidget->setCurrentIndex ( 0 );
         ui->conditionWidget->setCurrentIndex ( 1 );
         ui->dataWidget->setCurrentIndex ( 1 );
      break;
      case 4:
         ui->itemWidget->setCurrentIndex ( 1 );
         ui->conditionWidget->setCurrentIndex ( 1 );
         ui->dataWidget->setCurrentIndex ( 2 );
         ui->register_2->clear();
         ui->bitfield->clear();
         for ( idx = 0; idx < NUM_CPU_REGISTERS; idx++ )
         {
            ui->register_2->addItem ( tblCPURegisters[idx]->GetName() );
         }
      break;
      case 5:
         ui->itemWidget->setCurrentIndex ( 2 );
         ui->conditionWidget->setCurrentIndex ( 0 );
         ui->dataWidget->setCurrentIndex ( 2 );
      break;
      case 6:
      case 7:
         ui->itemWidget->setCurrentIndex ( 0 );
         ui->conditionWidget->setCurrentIndex ( 1 );
         ui->dataWidget->setCurrentIndex ( 1 );
      break;
      case 8:
         ui->itemWidget->setCurrentIndex ( 1 );
         ui->conditionWidget->setCurrentIndex ( 1 );
         ui->dataWidget->setCurrentIndex ( 2 );
         ui->register_2->clear();
         ui->bitfield->clear();
         for ( idx = 0; idx < NUM_PPU_REGISTERS; idx++ )
         {
            ui->register_2->addItem ( tblPPURegisters[idx]->GetName() );
         }
      break;
      case 9:
         ui->itemWidget->setCurrentIndex ( 2 );
         ui->conditionWidget->setCurrentIndex ( 0 );
         ui->dataWidget->setCurrentIndex ( 2 );
      break;
      case 10:
         ui->itemWidget->setCurrentIndex ( 0 );
         ui->conditionWidget->setCurrentIndex ( 1 );
         ui->dataWidget->setCurrentIndex ( 1 );
      break;
      case 11:
         ui->itemWidget->setCurrentIndex ( 1 );
         ui->conditionWidget->setCurrentIndex ( 1 );
         ui->dataWidget->setCurrentIndex ( 2 );
         ui->register_2->clear();
         ui->bitfield->clear();
         for ( idx = 0; idx < NUM_APU_REGISTERS; idx++ )
         {
            ui->register_2->addItem ( tblAPURegisters[idx]->GetName() );
         }
      break;
      case 12:
         ui->itemWidget->setCurrentIndex ( 2 );
         ui->conditionWidget->setCurrentIndex ( 0 );
         ui->dataWidget->setCurrentIndex ( 2 );
      break;
      case 13:
         ui->itemWidget->setCurrentIndex ( 1 );
         ui->conditionWidget->setCurrentIndex ( 1 );
         ui->dataWidget->setCurrentIndex ( 2 );
         ui->register_2->clear();
         ui->bitfield->clear();
         for ( idx = 0; idx < CROM::NUMREGISTERS(); idx++ )
         {
            ui->register_2->addItem ( CROM::REGISTERS()[idx]->GetName() );
         }
      break;
      case 14:
         ui->itemWidget->setCurrentIndex ( 2 );
         ui->conditionWidget->setCurrentIndex ( 0 );
         ui->dataWidget->setCurrentIndex ( 2 );
      break;
   }
}

void BreakpointDialog::on_pushButton_clicked()
{

}

void BreakpointDialog::on_register_2_currentIndexChanged(int index)
{
   int idx;
   switch ( ui->type->currentIndex() )
   {
      case 4:
         m_pRegister = tblCPURegisters [ ui->register_2->currentIndex() ];
      break;
      case 8:
         m_pRegister = tblPPURegisters [ ui->register_2->currentIndex() ];
      break;
      case 11:
         m_pRegister = tblAPURegisters [ ui->register_2->currentIndex() ];
      break;
      case 13:
         m_pRegister = CROM::REGISTERS() [ ui->register_2->currentIndex() ];
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
            ui->dataWidget->setCurrentIndex ( 2 );
            for ( idx = 0; idx < m_pBitfield->GetNumValues(); idx++ )
            {
               ui->data2->addItem ( m_pBitfield->GetValueByIndex(idx) );
            }
         }
         else
         {
            ui->dataWidget->setCurrentIndex ( 1 );
         }
      }
   }
}
