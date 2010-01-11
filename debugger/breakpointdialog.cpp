#include "breakpointdialog.h"
#include "ui_breakpointdialog.h"

#include "cnesrom.h"

static QStringList brkptConditions;

BreakpointDialog::BreakpointDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BreakpointDialog)
{
   m_pRegister = NULL;
   ui->setupUi(this);
   brkptConditions.append ( "Equals" );
   brkptConditions.append ( "Does Not Equal" );
   brkptConditions.append ( "Is Less Than" );
   brkptConditions.append ( "Is Greater Than" );
   ui->itemWidget->setCurrentIndex ( 0 );
   ui->conditionWidget->setCurrentIndex ( 0 );
   ui->dataWidget->setCurrentIndex ( 0 );
   ui->type->setCurrentIndex ( 0 );
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
   int idx1, idx2;

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
         for ( idx1 = 0; idx1 < NUM_CPU_REGISTERS; idx1++ )
         {
            ui->register_2->addItem ( tblCPURegisters[idx1]->GetName() );
         }
//         for ( idx2 = 0; idx2 < tblCPURegisters[0]->GetNumBitfields(); idx2++ )
//         {
//            CBitfieldData* pBitfield = tblCPURegisters[0]->GetBitfield(idx2);
//            ui->bitfield->addItem ( pBitfield->GetName() );
//         }
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
         for ( idx1 = 0; idx1 < NUM_PPU_REGISTERS; idx1++ )
         {
            ui->register_2->addItem ( tblPPURegisters[idx1]->GetName() );
         }
//         for ( idx2 = 0; idx2 < tblPPURegisters[0]->GetNumBitfields(); idx2++ )
//         {
//            CBitfieldData* pBitfield = tblPPURegisters[0]->GetBitfield(idx2);
//            ui->bitfield->addItem ( pBitfield->GetName() );
//         }
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
         for ( idx1 = 0; idx1 < NUM_APU_REGISTERS; idx1++ )
         {
            ui->register_2->addItem ( tblAPURegisters[idx1]->GetName() );
         }
//         for ( idx2 = 0; idx2 < tblAPURegisters[0]->GetNumBitfields(); idx2++ )
//         {
//            CBitfieldData* pBitfield = tblAPURegisters[0]->GetBitfield(idx2);
//            ui->bitfield->addItem ( pBitfield->GetName() );
//         }
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
         for ( idx1 = 0; idx1 < CROM::NUMREGISTERS(); idx1++ )
         {
            ui->register_2->addItem ( CROM::REGISTERS()[idx1]->GetName() );
         }
//         for ( idx2 = 0; idx2 < CROM::REGISTERS()[0]->GetNumBitfields(); idx2++ )
//         {
//            CBitfieldData* pBitfield = CROM::REGISTERS()[0]->GetBitfield(idx2);
//            ui->bitfield->addItem ( pBitfield->GetName() );
//         }
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
   int idx2;
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
      for ( idx2 = 0; idx2 < m_pRegister->GetNumBitfields(); idx2++ )
      {
         CBitfieldData* pBitfield = m_pRegister->GetBitfield(idx2);
         ui->bitfield->addItem ( pBitfield->GetName() );
      }
   }
}
