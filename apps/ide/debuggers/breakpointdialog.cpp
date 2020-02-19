#include "breakpointdialog.h"
#include "ui_breakpointdialog.h"

#include "dbg_cnes.h"
#include "dbg_cnesrom.h"
#include "dbg_cnes6502.h"
#include "dbg_cnesppu.h"
#include "dbg_cnesapu.h"

#include "compilerthread.h"
#include "main.h"

#include "ccc65interface.h"

#include "cobjectregistry.h"

BreakpointDialog::BreakpointDialog(CBreakpointInfo* pBreakpoints,int bp, QWidget* parent) :
   QDialog(parent),
   ui(new Ui::BreakpointDialog)
{
   ui->setupUi(this);

   m_pBreakpoints = pBreakpoints;

   ui->type->addItem("CPU Execution");
   ui->type->addItem("CPU Memory Access (Read or Write)");
   ui->type->addItem("CPU Memory Read");
   ui->type->addItem("CPU Memory Write");

   if ( !nesicideProject->getProjectTarget().compare("nes",Qt::CaseInsensitive) )
   {
      ui->type->addItem("CPU State");
      ui->type->addItem("CPU Event");
      ui->type->addItem("PPU Fetch (Rendering)");
      ui->type->addItem("OAM Portal Access (Read or Write)");
      ui->type->addItem("OAM Portal Read");
      ui->type->addItem("OAM Portal Write");
      ui->type->addItem("PPU Portal Access (Read or Write)");
      ui->type->addItem("PPU Portal Read");
      ui->type->addItem("PPU Portal Write");
      ui->type->addItem("PPU State");
      ui->type->addItem("PPU Event");
      ui->type->addItem("APU State");
      ui->type->addItem("APU Event");
      ui->type->addItem("Cartridge (Mapper) State");
      ui->type->addItem("Cartridge (Mapper) Event");
   }

   ui->itemWidget->setCurrentIndex ( eBreakpointItemAddress );
   ui->conditionWidget->setCurrentIndex ( eBreakpointConditionNone );
   ui->dataWidget->setCurrentIndex ( eBreakpointDataNone );
   ui->type->setCurrentIndex ( eBreakOnCPUExecution );
   ui->mask->setText("FFFF");

   ui->enabled->setChecked(true);

   if ( !nesicideProject->getProjectTarget().compare("nes",Qt::CaseInsensitive) )
   {
      ui->resolverWidget->setCurrentIndex(nesGetMapper()>0);
      ui->resolve->setChecked(false);
      ui->resolutions->addItem("N/A");
      ui->resolutions->setEnabled(false);
   }
   else if ( !nesicideProject->getProjectTarget().compare("c64",Qt::CaseInsensitive) )
   {
      ui->resolverWidget->setCurrentIndex(0);
      ui->resolve->setChecked(false);
   }

   m_pRegister = NULL;
   m_pBitfield = NULL;
   m_pEvent = NULL;

   if ( bp >= 0 )
   {
      setWindowTitle("Edit Breakpoint");
      ui->addBreakpoint->setText("Edit Breakpoint");
      DisplayBreakpoint(bp);
   }
   else
   {
      setWindowTitle("Add Breakpoint");
      ui->addBreakpoint->setText("Add Breakpoint");
   }
}

BreakpointDialog::~BreakpointDialog()
{
   delete ui;
}

void BreakpointDialog::changeEvent(QEvent* e)
{
   QDialog::changeEvent(e);

   switch (e->type())
   {
      case QEvent::LanguageChange:
         ui->retranslateUi(this);
         break;
      default:
         break;
   }
}

void BreakpointDialog::on_type_currentIndexChanged(int index)
{
   CBreakpointEventInfo** pBreakpointEventInfo = NULL;
   int idx;

   ui->item1label->setText("Data1:");
   ui->item2label->setText("Data2:");
   ui->event->setCurrentIndex(0);

   switch ( index )
   {
      case eBreakOnCPUExecution:
         ui->itemWidget->setCurrentIndex ( eBreakpointItemAddress );
         ui->conditionWidget->setCurrentIndex ( eBreakpointConditionNone );
         ui->dataWidget->setCurrentIndex ( eBreakpointDataNone );
         ui->resolverWidget->setCurrentIndex(nesGetMapper()>0);
         ui->resolve->setChecked(false);
         ui->resolutions->addItem("N/A");
         ui->resolutions->setEnabled(false);
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
         ui->resolverWidget->setCurrentIndex(0);
         ui->resolve->setChecked(false);
         break;
      case eBreakOnCPUState:
         ui->itemWidget->setCurrentIndex ( eBreakpointItemRegister );
         ui->conditionWidget->setCurrentIndex ( eBreakpointConditionTest );
         ui->dataWidget->setCurrentIndex ( eBreakpointDataPick );
         ui->resolverWidget->setCurrentIndex(0);
         ui->resolve->setChecked(false);
         ui->reg->clear();
         ui->bitfield->clear();

         for ( idx = 0; idx < nesGetCpuRegisterDatabase()->GetNumRegisters(); idx++ )
         {
            ui->reg->addItem ( nesGetCpuRegisterDatabase()->GetRegister(idx)->GetName() );
         }

         break;
      case eBreakOnCPUEvent:
         ui->itemWidget->setCurrentIndex ( eBreakpointItemEvent );
         ui->conditionWidget->setCurrentIndex ( eBreakpointConditionNone );
         ui->dataWidget->setCurrentIndex ( eBreakpointDataNone );
         ui->resolverWidget->setCurrentIndex(0);
         ui->resolve->setChecked(false);
         ui->event->clear();
         pBreakpointEventInfo = nesGetCpuBreakpointEventDatabase();

         for ( idx = 0; idx < nesGetSizeOfCpuBreakpointEventDatabase(); idx++ )
         {
            ui->event->addItem ( pBreakpointEventInfo[idx]->GetName() );
         }

         ui->event->setCurrentIndex ( 0 );
         break;
      case eBreakOnPPUFetch:
         ui->itemWidget->setCurrentIndex ( eBreakpointItemAddress );
         ui->conditionWidget->setCurrentIndex ( eBreakpointConditionTest );
         ui->dataWidget->setCurrentIndex ( eBreakpointDataPure );
         ui->resolverWidget->setCurrentIndex(0);
         ui->resolve->setChecked(false);
         break;
      case eBreakOnPPUState:
         ui->itemWidget->setCurrentIndex ( eBreakpointItemRegister );
         ui->conditionWidget->setCurrentIndex ( eBreakpointConditionTest );
         ui->dataWidget->setCurrentIndex ( eBreakpointDataPick );
         ui->resolverWidget->setCurrentIndex(0);
         ui->resolve->setChecked(false);
         ui->reg->clear();
         ui->bitfield->clear();

         for ( idx = 0; idx < nesGetPpuRegisterDatabase()->GetNumRegisters(); idx++ )
         {
            ui->reg->addItem ( nesGetPpuRegisterDatabase()->GetRegister(idx)->GetName() );
         }

         break;
      case eBreakOnPPUEvent:
         ui->itemWidget->setCurrentIndex ( eBreakpointItemEvent );
         ui->conditionWidget->setCurrentIndex ( eBreakpointConditionNone );
         ui->dataWidget->setCurrentIndex ( eBreakpointDataNone );
         ui->resolverWidget->setCurrentIndex(0);
         ui->resolve->setChecked(false);
         ui->event->clear();
         pBreakpointEventInfo = nesGetPpuBreakpointEventDatabase();

         for ( idx = 0; idx < nesGetSizeOfPpuBreakpointEventDatabase(); idx++ )
         {
            ui->event->addItem ( pBreakpointEventInfo[idx]->GetName() );
         }

         ui->event->setCurrentIndex ( 0 );
         break;
      case eBreakOnAPUState:
         ui->itemWidget->setCurrentIndex ( eBreakpointItemRegister );
         ui->conditionWidget->setCurrentIndex ( eBreakpointConditionTest );
         ui->dataWidget->setCurrentIndex ( eBreakpointDataPick );
         ui->resolverWidget->setCurrentIndex(0);
         ui->resolve->setChecked(false);
         ui->reg->clear();
         ui->bitfield->clear();

         for ( idx = 0; idx < nesGetApuRegisterDatabase()->GetNumRegisters(); idx++ )
         {
            ui->reg->addItem ( nesGetApuRegisterDatabase()->GetRegister(idx)->GetName() );
         }

         break;
      case eBreakOnAPUEvent:
         ui->itemWidget->setCurrentIndex ( eBreakpointItemEvent );
         ui->conditionWidget->setCurrentIndex ( eBreakpointConditionNone );
         ui->dataWidget->setCurrentIndex ( eBreakpointDataNone );
         ui->resolverWidget->setCurrentIndex(0);
         ui->resolve->setChecked(false);
         ui->event->clear();
         pBreakpointEventInfo = nesGetApuBreakpointEventDatabase();

         for ( idx = 0; idx < nesGetSizeOfApuBreakpointEventDatabase(); idx++ )
         {
            ui->event->addItem ( pBreakpointEventInfo[idx]->GetName() );
         }

         ui->event->setCurrentIndex ( 0 );
         break;
      case eBreakOnMapperState:
         ui->itemWidget->setCurrentIndex ( eBreakpointItemRegister );
         ui->conditionWidget->setCurrentIndex ( eBreakpointConditionTest );
         ui->dataWidget->setCurrentIndex ( eBreakpointDataPick );
         ui->resolverWidget->setCurrentIndex(0);
         ui->resolve->setChecked(false);
         ui->reg->clear();
         ui->bitfield->clear();

         if ( nesGetCartridgeRegisterDatabase() )
         {
            for ( idx = 0; idx < nesGetCartridgeRegisterDatabase()->GetNumRegisters(); idx++ )
            {
               ui->reg->addItem ( nesGetCartridgeRegisterDatabase()->GetRegister(idx)->GetName() );
            }
         }
         break;
      case eBreakOnMapperEvent:
         ui->itemWidget->setCurrentIndex ( eBreakpointItemEvent );
         ui->conditionWidget->setCurrentIndex ( eBreakpointConditionNone );
         ui->dataWidget->setCurrentIndex ( eBreakpointDataNone );
         ui->resolverWidget->setCurrentIndex(0);
         ui->resolve->setChecked(false);
         ui->event->clear();
         pBreakpointEventInfo = nesGetCartridgeBreakpointEventDatabase();

         for ( idx = 0; idx < nesGetSizeOfCartridgeBreakpointEventDatabase(); idx++ )
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
            m_pRegister = nesGetCpuRegisterDatabase()->GetRegister(ui->reg->currentIndex());
            break;
         case eBreakOnPPUState:
            m_pRegister = nesGetPpuRegisterDatabase()->GetRegister(ui->reg->currentIndex());
            break;
         case eBreakOnAPUState:
            m_pRegister = nesGetApuRegisterDatabase()->GetRegister(ui->reg->currentIndex());
            break;
         case eBreakOnMapperState:

            if ( nesGetCartridgeRegisterDatabase()->GetNumRegisters() > 0 )
            {
               m_pRegister = nesGetCartridgeRegisterDatabase()->GetRegister(ui->reg->currentIndex());
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

void BreakpointDialog::on_event_currentIndexChanged(int)
{
   if ( ui->event->currentIndex() >= 0 )
   {
      switch ( ui->type->currentIndex() )
      {
         case eBreakOnCPUEvent:
            m_pEvent = nesGetCpuBreakpointEventDatabase()[ui->event->currentIndex()];
            break;
         case eBreakOnPPUEvent:
            m_pEvent = nesGetPpuBreakpointEventDatabase()[ui->event->currentIndex()];
            break;
         case eBreakOnAPUEvent:
            m_pEvent = nesGetApuBreakpointEventDatabase()[ui->event->currentIndex()];
            break;
         case eBreakOnMapperEvent:
            m_pEvent = nesGetCartridgeBreakpointEventDatabase()[ui->event->currentIndex()];
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

void BreakpointDialog::on_addr1_textChanged(QString text)
{
   ui->addr2->setText(text);
   DisplayResolutions(NULL);
}

void BreakpointDialog::DisplayResolutions(BreakpointInfo* pBreakpoint)
{
   char address[32];
   char disassembly[32];
   uint32_t size = nesGetPRGROMSize();
   uint32_t originalAddr;
   uint32_t maskedAddr;
   int      linenum;
   QString  source;
   QString  item;
   QString  text;
   QStringList textSplit;
   QFileInfo   fileInfo;
   CompilerThread* compiler = dynamic_cast<CompilerThread*>(CObjectRegistry::getInstance()->getObject("Compiler"));

   // Get address from UI
   originalAddr = ui->addr1->text().toInt(0,16);

   // Only display resolutions for addresses in PRG-ROM space.
   if ( originalAddr < MEM_32KB )
   {
      ui->resolve->setChecked(false);
   }

   // Mask to get all available potential absolute addresses
   maskedAddr = originalAddr&MASK_8KB;

   if ( ui->resolve->isChecked() )
   {
      ui->resolutions->clear();
      for ( ; maskedAddr < size; maskedAddr += MEM_8KB )
      {
         if ( compiler->assembledOk() )
         {
            source = CCC65Interface::getSourceFileFromPhysicalAddress(originalAddr,maskedAddr);
            if ( !source.isEmpty() )
            {
               fileInfo.setFile(source);
               source = fileInfo.fileName();

               text = "put something";//pasm_get_source_file_text_by_addr(maskedAddr);
               linenum = CCC65Interface::getSourceLineFromPhysicalAddress(originalAddr,maskedAddr);
//               textSplit = text.split(QRegExp("[\r\n]"));
//               text = textSplit.at(linenum-1);
               item.sprintf("%s:%d:",
                            source.toLatin1().constData(),
                            linenum);
               item.append(text);
               ui->resolutions->addItem(item);
               ui->resolutions->setItemData(ui->resolutions->count()-1,maskedAddr);
            }
         }
         else
         {
            nesGetDisassemblyAtPhysicalAddress(maskedAddr,disassembly);
            nesGetPrintablePhysicalAddress(address,originalAddr,maskedAddr);
            item.sprintf("%s:%s",address,disassembly);
            ui->resolutions->addItem(item);
            ui->resolutions->setItemData(ui->resolutions->count()-1,maskedAddr);
         }
         if ( pBreakpoint && pBreakpoint->item1Physical == maskedAddr )
         {
            ui->resolutions->setCurrentIndex(ui->resolutions->count()-1);
         }
      }
      if ( ui->resolutions->count() )
      {
         ui->resolutions->setEnabled(true);
      }
   }
   else
   {
      ui->resolutions->clear();
      ui->resolutions->addItem("N/A");
      ui->resolutions->setEnabled(false);
   }
}

void BreakpointDialog::DisplayBreakpoint ( int idx )
{
   char buffer [ 16 ];
   BreakpointInfo* pBreakpoint = m_pBreakpoints->GetBreakpoint ( idx );

   ui->type->setCurrentIndex ( pBreakpoint->type );
   ui->itemWidget->setCurrentIndex ( pBreakpoint->itemType );
   ui->item1label->setText("Data1:");
   ui->item2label->setText("Data2:");
   ui->mask->setText("FFFF");

   ui->enabled->setChecked(pBreakpoint->enabled);

   // Turn resolver on so it populates if the absolute address is known.
   // Only do this for NES platform until it is known whether it is needed
   // for other platforms.
   if ( (!nesicideProject->getProjectTarget().compare("nes",Qt::CaseInsensitive)) && 
        (pBreakpoint->item1Physical >= 0) )
   {
      ui->resolve->setChecked(true);
   }
   else
   {
      ui->resolve->setChecked(false);
   }

   switch ( pBreakpoint->itemType )
   {
      case eBreakpointItemAddress:
         sprintf ( buffer, "%X", pBreakpoint->item1 );
         ui->addr1->setText ( buffer );
         sprintf ( buffer, "%X", pBreakpoint->item2 );
         ui->addr2->setText ( buffer );
         sprintf ( buffer, "%X", pBreakpoint->itemMask );
         ui->mask->setText ( buffer );
         ui->itemMaskScope->setCurrentIndex(pBreakpoint->itemMaskExclusive?0:1);
         break;
      case eBreakpointItemRegister:
         ui->reg->setCurrentIndex ( pBreakpoint->item1 );
         ui->bitfield->setCurrentIndex ( pBreakpoint->item2 );
         break;
      case eBreakpointItemEvent:
         ui->event->setCurrentIndex(pBreakpoint->event);

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
   DisplayResolutions(pBreakpoint);
}


void BreakpointDialog::on_cancel_clicked()
{
   reject();
}

void BreakpointDialog::on_addBreakpoint_clicked()
{
   int  item1 = 0;
   int  item1Physical = 0;
   int  item2 = 0;
   int  mask = 0;
   int  data = 0;
   int  event = 0;
   bool maskExclusive = true;

   switch ( ui->itemWidget->currentIndex() )
   {
      case eBreakpointItemNone:
         // No item...
         break;
      case eBreakpointItemAddress:
         // Address item...
         item1 = ui->addr1->text().toInt(0, 16);
         item2 = ui->addr2->text().toInt(0, 16);
         mask = ui->mask->text().toInt(0, 16);
         maskExclusive = (ui->itemMaskScope->currentIndex()==0)?true:false;
         break;
      case eBreakpointItemRegister:
         // Register item...
         item1 = ui->reg->currentIndex ();
         item2 = ui->bitfield->currentIndex ();

         // sometimes no bitfield...
         if ( item2 < 0 )
         {
            item2 = 0;
         }

         break;
      case eBreakpointItemEvent:

         if ( m_pEvent )
         {
            item1 = ui->eventData1->text().toInt(0, m_pEvent->GetElementRadix());
            item2 = ui->eventData2->text().toInt(0, m_pEvent->GetElementRadix());
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
         data = ui->data1->text().toInt(0, 16);
         break;
      case eBreakpointDataPick:
         // Picklist data...
         data = ui->data2->currentIndex ();
         break;
   }

   if ( ui->resolve->isChecked() )
   {
      item1Physical = ui->resolutions->itemData(ui->resolutions->currentIndex()).toInt();
   }
   else
   {
      // NES
      if ( !nesicideProject->getProjectTarget().compare("nes",Qt::CaseInsensitive) )
      {
         // If the virtual address is in PRG-ROM space, convert it to physical for
         // the absolute address.  Otherwise, the virtual *is* the physical address.
         // CPTODO: This may need to be revisited for mappers that can put PRG-ROM in
         // SRAM space.
         if ( item1 >= MEM_32KB )
         {
            // If there's more than 16KB of PRG-ROM then the
            // physical address is simply the offset into the PRG-ROM.
            item1Physical = (item1-MEM_32KB)%nesGetPRGROMSize();
         }
         else if ( item1 >= 0x6000 )
         {
            // CPTODO: For now assume identity mapped SRAM.
            item1Physical = (item1-0x6000);
         }
         else
         {
            // Virtual is physical if address is less than $8000.
            item1Physical = item1;
         }         
      }
      else
      {
         // Virtual is physical for now.  Might need to revisit.
         item1Physical = item1;
      }
   }

   m_pBreakpoints->ConstructBreakpoint ( &m_breakpoint,
                                       (eBreakpointType)ui->type->currentIndex(),
                                       (eBreakpointItemType)ui->itemWidget->currentIndex(),
                                       event,
                                       item1,
                                       item1Physical,
                                       item2,
                                       mask,
                                       maskExclusive,
                                       (eBreakpointConditionType)ui->conditionWidget->currentIndex(),
                                       ui->condition->currentIndex(),
                                       (eBreakpointDataType)ui->dataWidget->currentIndex(),
                                       data,
                                       ui->enabled->isChecked() );

   accept();
}

void BreakpointDialog::on_resolve_clicked()
{
    DisplayResolutions(NULL);
}
