//    NESICIDE - an IDE for the 8-bit NES.
//    Copyright (C) 2009  Christopher S. Pow

//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.

//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.

//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "cnesrommapper016.h"
#include "cnes6502.h"
#include "cnesppu.h"

#include "cregisterdata.h"

// Mapper 016 Registers
static CBitfieldData* tbl0000Bitfields [] =
{
   new CBitfieldData("CHR Bank", 0, 8, "%02X", 0)
};

static CBitfieldData* tbl0008Bitfields [] =
{
   new CBitfieldData("PRG Bank", 0, 8, "%02X", 0)
};

static CBitfieldData* tbl0009Bitfields [] =
{
   new CBitfieldData("Mirroring", 0, 2, "%X", 4, "Vertical", "Horizontal", "One-screen A", "One-screen B")
};

static CBitfieldData* tbl000ABitfields [] =
{
   new CBitfieldData("IRQ Enabled", 0, 1, "%X", 2, "No", "Yes")
};

static CBitfieldData* tbl000BBitfields [] =
{
   new CBitfieldData("IRQ Counter LSB", 0, 8, "%02X", 0)
};

static CBitfieldData* tbl000CBitfields [] =
{
   new CBitfieldData("IRQ Counter MSB", 0, 8, "%X", 0)
};

static CBitfieldData* tbl000DBitfields [] =
{
   new CBitfieldData("EEPROM I/O", 0, 8, "%02X", 0)
};

static CRegisterData* tblRegisters [] =
{
   new CRegisterData(0x6000, "CHR Control 0", nesMapperLowRead, nesMapperLowWrite, 1, tbl0000Bitfields),
   new CRegisterData(0x6001, "CHR Control 1", nesMapperLowRead, nesMapperLowWrite, 1, tbl0000Bitfields),
   new CRegisterData(0x6002, "CHR Control 2", nesMapperLowRead, nesMapperLowWrite, 1, tbl0000Bitfields),
   new CRegisterData(0x6003, "CHR Control 3", nesMapperLowRead, nesMapperLowWrite, 1, tbl0000Bitfields),
   new CRegisterData(0x6004, "CHR Control 4", nesMapperLowRead, nesMapperLowWrite, 1, tbl0000Bitfields),
   new CRegisterData(0x6005, "CHR Control 5", nesMapperLowRead, nesMapperLowWrite, 1, tbl0000Bitfields),
   new CRegisterData(0x6006, "CHR Control 6", nesMapperLowRead, nesMapperLowWrite, 1, tbl0000Bitfields),
   new CRegisterData(0x6007, "CHR Control 7", nesMapperLowRead, nesMapperLowWrite, 1, tbl0000Bitfields),
   new CRegisterData(0x6008, "PRG Control", nesMapperLowRead, nesMapperLowWrite, 1, tbl0008Bitfields),
   new CRegisterData(0x6009, "Mirroring", nesMapperLowRead, nesMapperLowWrite, 1, tbl0009Bitfields),
   new CRegisterData(0x600A, "IRQ Control", nesMapperLowRead, nesMapperLowWrite, 1, tbl000ABitfields),
   new CRegisterData(0x600B, "IRQ Counter LSB", nesMapperLowRead, nesMapperLowWrite, 1, tbl000BBitfields),
   new CRegisterData(0x600C, "IRQ Counter MSB", nesMapperLowRead, nesMapperLowWrite, 1, tbl000CBitfields),
   new CRegisterData(0x600D, "EEPROM I/O", nesMapperLowRead, nesMapperLowWrite, 1, tbl000DBitfields)
};

static const char* rowHeadings [] =
{
   ""
};

static const char* columnHeadings [] =
{
   "6000","6001","6002","6003","6004","6005","6006","6007","6008","6009","600A","600B","600C","600D"
};

static CRegisterDatabase* dbRegisters = new CRegisterDatabase(eMemory_cartMapper,1,14,14,tblRegisters,rowHeadings,columnHeadings);

uint8_t  CROMMapper016::m_reg [] = { 0x00, };
uint16_t CROMMapper016::m_irqCounter = 0;
bool     CROMMapper016::m_irqEnabled = false;
bool     CROMMapper016::m_irqAsserted = false;
uint8_t  CROMMapper016::m_eepromBitCounter = 0;
uint8_t  CROMMapper016::m_eepromState = 0;
uint8_t  CROMMapper016::m_eepromCmd;
uint8_t  CROMMapper016::m_eepromAddr;
uint8_t  CROMMapper016::m_eepromDataBuf;
uint8_t  CROMMapper016::m_eepromRWBit;

CROMMapper016::CROMMapper016()
{
}

CROMMapper016::~CROMMapper016()
{
}

void CROMMapper016::RESET016 ( bool soft )
{
   m_mapper = 16;

   m_dbRegisters = dbRegisters;

   CROM::RESET ( m_mapper, soft );

   m_irqCounter = 0;
   m_irqEnabled = false;
   m_irqAsserted = false;
   m_eepromState = 0;
   m_eepromBitCounter = 0;

   m_pPRGROMmemory [ 0 ] = m_PRGROMmemory [ 0 ];
   m_pPRGROMmemory [ 1 ] = m_PRGROMmemory [ 1 ];
   m_pPRGROMmemory [ 2 ] = m_PRGROMmemory [ m_numPrgBanks-2 ];
   m_pPRGROMmemory [ 3 ] = m_PRGROMmemory [ m_numPrgBanks-1 ];

   // CHR ROM/RAM already set up in CROM::RESET()...
}


void CROMMapper016::RESET159 ( bool soft )
{
   m_mapper = 159;

   m_dbRegisters = dbRegisters;

   CROM::RESET ( m_mapper, soft );

   m_irqCounter = 0;
   m_irqEnabled = false;
   m_irqAsserted = false;
   m_eepromState = 0;
   m_eepromBitCounter = 0;

   m_pPRGROMmemory [ 0 ] = m_PRGROMmemory [ 0 ];
   m_pPRGROMmemory [ 1 ] = m_PRGROMmemory [ 1 ];
   m_pPRGROMmemory [ 2 ] = m_PRGROMmemory [ m_numPrgBanks-2 ];
   m_pPRGROMmemory [ 3 ] = m_PRGROMmemory [ m_numPrgBanks-1 ];

   // CHR ROM/RAM already set up in CROM::RESET()...
}

void CROMMapper016::SYNCCPU ( void )
{
   if ( m_irqEnabled )
   {
      m_irqCounter--;

      if ( !m_irqCounter )
      {
         m_irqAsserted = true;
         C6502::ASSERTIRQ(eNESSource_Mapper);

         if ( nesIsDebuggable() )
         {
            // Check for IRQ breakpoint...
            CNES::CHECKBREAKPOINT(eBreakInMapper,eBreakOnMapperEvent,0,MAPPER_EVENT_IRQ);
         }
      }
   }
}

uint32_t CROMMapper016::DEBUGINFO ( uint32_t addr )
{
   switch ( addr&0x000F )
   {
   case 0x0000:
      return m_reg[0];
      break;
   case 0x0001:
      return m_reg[1];
      break;
   case 0x0002:
      return m_reg[2];
      break;
   case 0x0003:
      return m_reg[3];
      break;
   case 0x0004:
      return m_reg[4];
      break;
   case 0x0005:
      return m_reg[5];
      break;
   case 0x0006:
      return m_reg[6];
      break;
   case 0x0007:
      return m_reg[7];
      break;
   case 0x0008:
      return m_reg[8];
      break;
   case 0x0009:
      return m_reg[9];
      break;
   case 0x000A:
      return m_reg[10];
      break;
   case 0x000B:
      return m_reg[11];
      break;
   case 0x000C:
      return m_reg[12];
      break;
   case 0x000D:
      return m_reg[13];
      break;
   }
   return 0xA1; // garbage
}

uint32_t CROMMapper016::LMAPPER ( uint32_t addr )
{
   uint8_t data = C6502::OPENBUS();

   switch ( m_eepromState )
   {
   case 1:
      // Need to ack and skip to state 3 on a read command.
      if ( m_eepromBitCounter == 9 )
      {
         m_eepromBitCounter = 0;
         if ( m_eepromCmd&0x01 )
         {
            m_eepromState = 3;
         }
         else
         {
            m_eepromState = 2;
         }
         data = 0x00;
      }
      break;
   case 2:
      // Only need to ack.
      if ( m_eepromBitCounter == 9 )
      {
         m_eepromBitCounter = 0;
         m_eepromState++;
         data = 0x00;
      }
      break;
   case 3:
      // Command execution
      if ( m_eepromBitCounter == 9 )
      {
         m_eepromBitCounter = 0;
         data = 0x00;
      }

      // Read
      if ( m_mapper == 16 )
      {
         data = CROM::SRAMVIRT(0x6000+m_eepromAddr);
      }
      else
      {
         data = CROM::SRAMVIRT(0x6000+(m_eepromAddr>>1));
      }
      data >>= m_eepromBitCounter;
      data &= 0x01;
      data <<= 4;

      m_eepromBitCounter++;
      break;
   }

   return data;
}

void CROMMapper016::HMAPPER ( uint32_t addr, uint8_t data )
{
   uint32_t reg = 0;

   switch ( addr&0x000F )
   {
   case 0x0000:
      reg = 0;
      m_reg[0] = data;
      m_pCHRmemory[0] = m_CHRmemory[data];
      break;
   case 0x0001:
      reg = 1;
      m_reg[1] = data;
      m_pCHRmemory[1] = m_CHRmemory[data];
      break;
   case 0x0002:
      reg = 2;
      m_reg[2] = data;
      m_pCHRmemory[2] = m_CHRmemory[data];
      break;
   case 0x0003:
      reg = 3;
      m_reg[3] = data;
      m_pCHRmemory[3] = m_CHRmemory[data];
      break;
   case 0x0004:
      reg = 4;
      m_reg[4] = data;
      m_pCHRmemory[4] = m_CHRmemory[data];
      break;
   case 0x0005:
      reg = 5;
      m_reg[5] = data;
      m_pCHRmemory[5] = m_CHRmemory[data];
      break;
   case 0x0006:
      reg = 6;
      m_reg[6] = data;
      m_pCHRmemory[6] = m_CHRmemory[data];
      break;
   case 0x0007:
      reg = 7;
      m_reg[7] = data;
      m_pCHRmemory[7] = m_CHRmemory[data];
      break;
   case 0x0008:
      reg = 8;
      m_reg[8] = data;
      m_pPRGROMmemory[0] = m_PRGROMmemory[((data<<1)%m_numPrgBanks)+0];
      m_pPRGROMmemory[1] = m_PRGROMmemory[((data<<1)%m_numPrgBanks)+1];
      break;
   case 0x0009:
      reg = 9;
      m_reg[9] = data;
      switch ( data&0x03 )
      {
      case 0:
         CPPU::MIRRORVERT();
         break;
      case 1:
         CPPU::MIRRORHORIZ();
         break;
      case 2:
         CPPU::MIRROR(0,0,0,0);
         break;
      case 3:
         CPPU::MIRROR(1,1,1,1);
         break;
      }
      break;
   case 0x000A:
      reg = 10;
      m_reg[10] = data;
      m_irqEnabled = data&0x01;
      m_irqAsserted = false;
      C6502::RELEASEIRQ(eNESSource_Mapper);
      break;
   case 0x000B:
      reg = 11;
      m_reg[11] = data;
      m_irqCounter &= 0xFF00;
      m_irqCounter |= data;
      break;
   case 0x000C:
      reg = 12;
      m_reg[12] = data;
      m_irqCounter &= 0x00FF;
      m_irqCounter |= (data<<8);
      break;
   case 0x000D:
      reg = 13;
      // Check for START/STOP
      if ( (m_reg[13]&0x20) && (data&0x20) )
      {
         // Clock is high, did data go low?  If so, START
         if ( (m_reg[13]&0x40) && (!(data&0x40)) )
         {
            // EEPROM in mapper 159 skips address phase
            if ( m_mapper == 16 )
            {
               m_eepromState = 1;
            }
            else
            {
               m_eepromState = 2;
            }
            m_eepromBitCounter = 0;
         }
         // Clock is high, did data go high?  If so, STOP
         if ( (!(m_reg[13]&0x40)) && (data&0x40) )
         {
            m_eepromState = 0;
         }
      }

      // Did clock transition?
      if ( (!(m_reg[13]&0x20)) && (data&0x20) )
      {
         // positive edge
         switch ( m_eepromState )
         {
         case 1:
            // Accepting command
            if ( m_eepromBitCounter < 8 )
            {
               m_eepromCmd <<= 1;
               m_eepromCmd &= 0xFF;
               m_eepromCmd |= ((data&0x40)>>6);
            }
            m_eepromBitCounter++;
            break;
         case 2:
            // Accepting address
            if ( m_eepromBitCounter < 8 )
            {
               m_eepromAddr <<= 1;
               m_eepromAddr &= 0xFF;
               m_eepromAddr |= ((data&0x40)>>6);
            }
            m_eepromBitCounter++;
            break;
         case 3:
            // Command execution
            if ( m_mapper == 16 )
            {
               m_eepromRWBit = m_eepromCmd&0x01;
            }
            else
            {
               m_eepromRWBit = m_eepromAddr&0x01;
            }
            switch ( m_eepromRWBit )
            {
            case 0:
               // Write
               m_eepromDataBuf <<= 1;
               m_eepromDataBuf |= ((data&0x40)>>6);

               m_eepromBitCounter++;
               if ( m_eepromBitCounter == 8 )
               {
                  if ( m_mapper == 16 )
                  {
                     CROM::SRAMVIRT(0x6000+m_eepromAddr,m_eepromDataBuf);
                  }
                  else
                  {
                     CROM::SRAMVIRT(0x6000+(m_eepromAddr>>1),m_eepromDataBuf);
                  }
                  CNES::FORCEBREAKPOINT();
                  if ( m_mapper == 16 )
                  {
                     m_eepromAddr++;
                     m_eepromAddr %= 0x100;
                  }
                  else
                  {
                     m_eepromAddr += 2;
                     m_eepromAddr %= 0x100;
                  }
               }
               break;
            case 1:
               if ( m_eepromBitCounter == 8 )
               {
                  m_eepromBitCounter++;
                  if ( m_mapper == 16 )
                  {
                     m_eepromAddr++;
                     m_eepromAddr %= 0x100;
                  }
                  else
                  {
                     m_eepromAddr += 2;
                     m_eepromAddr %= 0x100;
                  }
               }
            }
            break;
         }
      }

      // NOTE: m_reg[13] is used to keep track of the previous pin states!
      m_reg[13] = data;
      break;
   }

   if ( nesIsDebuggable() )
   {
      // Check mapper state breakpoints...
      CNES::CHECKBREAKPOINT(eBreakInMapper,eBreakOnMapperState,reg);
   }
}
