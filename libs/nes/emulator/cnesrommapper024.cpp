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

#include "cnesrommapper024.h"
#include "cnes6502.h"
#include "cnesppu.h"

#include "cregisterdata.h"

// Mapper 024 Registers
static CBitfieldData* tbl8000Bitfields [] =
{
   new CBitfieldData("PRG Bank", 0, 8, "%02X", 0)
};

static CBitfieldData* tbl9000Bitfields [] =
{
   new CBitfieldData("Volume",0, 4, "%X", 0),
   new CBitfieldData("Duty cycle", 4, 3, "%X", 8, "1/16", "1/8", "3/16", "1/4", "5/16", "3/8", "7/16", "1/2"),
   new CBitfieldData("Mode", 7, 1, "%X", 2, "Normal", "Digitized")
};

static CBitfieldData* tbl9001Bitfields [] =
{
   new CBitfieldData("Frequency LSB", 0, 8, "%02X", 0)
};

static CBitfieldData* tbl9002Bitfields [] =
{
   new CBitfieldData("Frequency MSN", 0, 4, "%X", 0),
   new CBitfieldData("Enabled", 7, 1, "%X", 2, "No", "Yes")
};

static CBitfieldData* tblB000Bitfields [] =
{
   new CBitfieldData("Accum Rate", 0, 6, "%X", 0)
};

static CBitfieldData* tblB003Bitfields [] =
{
   new CBitfieldData("Mirroring", 2, 2, "%X", 4, "Vertical", "Horizontal", "One-screen A", "One-screen B")
};

static CBitfieldData* tblC000Bitfields [] =
{
   new CBitfieldData("PRG Bank", 0, 8, "%02X", 0)
};

static CBitfieldData* tblD000Bitfields [] =
{
   new CBitfieldData("CHR Bank", 0, 8, "%02X", 0)
};

static CBitfieldData* tblF000Bitfields [] =
{
   new CBitfieldData("IRQ Latch", 0, 8, "%02X", 0)
};

static CBitfieldData* tblF001Bitfields [] =
{
   new CBitfieldData("IRQ Enable after ack", 0, 1, "%X", 2, "No", "Yes"),
   new CBitfieldData("IRQ Enable", 1, 1, "%X", 2, "No", "Yes"),
   new CBitfieldData("IRQ Mode", 2, 1, "%X", 2, "Scanline mode", "Cycle mode")
};

static CBitfieldData* tblF002Bitfields [] =
{
   new CBitfieldData("IRQ Acknowledge", 0, 8, "%X", 0)
};

static CRegisterData* tblRegisters [] =
{
   new CRegisterData(0x8000, "PRG Control 0", nesMapperHighRead, nesMapperHighWrite, 1, tbl8000Bitfields),
   new CRegisterData(0x9000, "Pulse1 Control", nesMapperHighRead, nesMapperHighWrite, 3, tbl9000Bitfields),
   new CRegisterData(0x9001, "Pulse1 Frequency LSB", nesMapperHighRead, nesMapperHighWrite, 1, tbl9001Bitfields),
   new CRegisterData(0x9002, "Pulse1 Frequency MSN", nesMapperHighRead, nesMapperHighWrite, 2, tbl9002Bitfields),
   new CRegisterData(0xA000, "Pulse2 Control", nesMapperHighRead, nesMapperHighWrite, 3, tbl9000Bitfields),
   new CRegisterData(0xA001, "Pulse2 Frequency LSB", nesMapperHighRead, nesMapperHighWrite, 1, tbl9001Bitfields),
   new CRegisterData(0xA002, "Pulse2 Frequency MSN", nesMapperHighRead, nesMapperHighWrite, 2, tbl9002Bitfields),
   new CRegisterData(0xB000, "Sawtooth Control", nesMapperHighRead, nesMapperHighWrite, 1, tblB000Bitfields),
   new CRegisterData(0xB001, "Sawtooth Frequency LSB", nesMapperHighRead, nesMapperHighWrite, 1, tbl9001Bitfields),
   new CRegisterData(0xB002, "Sawtooth Frequency MSN", nesMapperHighRead, nesMapperHighWrite, 2, tbl9002Bitfields),
   new CRegisterData(0xB003, "Mirroring", nesMapperHighRead, nesMapperHighWrite, 1, tblB003Bitfields),
   new CRegisterData(0xC000, "PRG Control 1", nesMapperHighRead, nesMapperHighWrite, 1, tblC000Bitfields),
   new CRegisterData(0xD000, "CHR Control 0", nesMapperHighRead, nesMapperHighWrite, 1, tblD000Bitfields),
   new CRegisterData(0xD001, "CHR Control 1", nesMapperHighRead, nesMapperHighWrite, 1, tblD000Bitfields),
   new CRegisterData(0xD002, "CHR Control 2", nesMapperHighRead, nesMapperHighWrite, 1, tblD000Bitfields),
   new CRegisterData(0xD003, "CHR Control 3", nesMapperHighRead, nesMapperHighWrite, 1, tblD000Bitfields),
   new CRegisterData(0xE000, "CHR Control 4", nesMapperHighRead, nesMapperHighWrite, 1, tblD000Bitfields),
   new CRegisterData(0xE001, "CHR Control 5", nesMapperHighRead, nesMapperHighWrite, 1, tblD000Bitfields),
   new CRegisterData(0xE002, "CHR Control 6", nesMapperHighRead, nesMapperHighWrite, 1, tblD000Bitfields),
   new CRegisterData(0xE003, "CHR Control 7", nesMapperHighRead, nesMapperHighWrite, 1, tblD000Bitfields),
   new CRegisterData(0xF000, "IRQ Latch", nesMapperHighRead, nesMapperHighWrite, 1, tblF000Bitfields),
   new CRegisterData(0xF001, "IRQ Control", nesMapperHighRead, nesMapperHighWrite, 3, tblF001Bitfields),
   new CRegisterData(0xF002, "IRQ Acknowledge", nesMapperHighRead, nesMapperHighWrite, 1, tblF002Bitfields)
};

static const char* rowHeadings [] =
{
   ""
};

static const char* columnHeadings [] =
{
   "8000",
   "9000","9001","9002",
   "A000","A001","A002",
   "B000","B001","B002",
   "B003",
   "C000",
   "D000","D001","D002","D003","E000","E001","E002","E003",
   "F000","F001","F002"
};

static CRegisterDatabase* dbRegisters = new CRegisterDatabase(eMemory_cartMapper,1,23,23,tblRegisters,rowHeadings,columnHeadings);

uint32_t CROMMapper024::m_soundEnableMask = 0xffffffff;

CROMMapper024::CROMMapper024()
   : CROM(24)
{
   memset(m_reg,0,sizeof(m_reg));
   m_irqReload = 0;
   m_irqCounter = 0;
   m_irqPrescaler = 0;
   m_irqPrescalerPhase = 0;
   m_irqEnabled = false;
}

CROMMapper024::~CROMMapper024()
{
}

void CROMMapper024::RESET ( bool soft )
{
   m_mapper = 24;

   m_dbCartRegisters = dbRegisters;

   m_pulse[0].RESET();
   m_pulse[1].RESET();
   m_sawtooth.RESET();

   CROM::RESET ( soft );

   m_irqReload = 0;
   m_irqCounter = 0;
   m_irqPrescaler = 0;
   m_irqPrescalerPhase = 0;
   m_irqEnabled = false;

   m_PRGROMmemory.REMAP(2,m_numPrgBanks-2);
   m_PRGROMmemory.REMAP(3,m_numPrgBanks-1);

   // CHR ROM/RAM already set up in CROM::RESET()...
}

void CROMMapper024::SYNCCPU ( void )
{
   uint8_t phases[3] = { 114, 114, 113 };

   m_pulse[0].TIMERTICK();
   m_pulse[1].TIMERTICK();
   m_sawtooth.TIMERTICK();

   if ( m_reg[21]&0x02 )
   {
      if ( m_reg[21]&0x04 )
      {
         // Cycle mode counter
         if ( m_irqCounter == 0xFF )
         {
            m_irqCounter = m_irqReload;
            CNES::NES()->CPU()->ASSERTIRQ(eNESSource_Mapper);

            if ( nesIsDebuggable() )
            {
               // Check for IRQ breakpoint...
               CNES::NES()->CHECKBREAKPOINT(eBreakInMapper,eBreakOnMapperEvent,0,MAPPER_EVENT_IRQ);
            }
         }
         else
         {
            m_irqCounter++;
         }
      }
      else
      {
         // Scanline mode counter
         m_irqPrescaler++;
         if ( m_irqPrescaler >= phases[m_irqPrescalerPhase] )
         {
            m_irqPrescaler = 0;
            m_irqPrescalerPhase++;
            m_irqPrescalerPhase %= 3;

            if ( m_irqCounter == 0xFF )
            {
               m_irqCounter = m_irqReload;
               CNES::NES()->CPU()->ASSERTIRQ(eNESSource_Mapper);

               if ( nesIsDebuggable() )
               {
                  // Check for IRQ breakpoint...
                  CNES::NES()->CHECKBREAKPOINT(eBreakInMapper,eBreakOnMapperEvent,0,MAPPER_EVENT_IRQ);
               }
            }
            else
            {
               m_irqCounter++;
            }
         }
      }
   }
}

uint32_t CROMMapper024::DEBUGINFO ( uint32_t addr )
{
   switch ( addr )
   {
   case 0x8000:
   case 0x8001:
   case 0x8002:
   case 0x8003:
      return m_reg[0];
      break;
   case 0x9000:
      return m_reg[1];
      break;
   case 0x9001:
      return m_reg[2];
      break;
   case 0x9002:
      return m_reg[3];
      break;
   case 0xA000:
      return m_reg[4];
      break;
   case 0xA001:
      return m_reg[5];
      break;
   case 0xA002:
      return m_reg[6];
      break;
   case 0xB000:
      return m_reg[7];
      break;
   case 0xB001:
      return m_reg[8];
      break;
   case 0xB002:
      return m_reg[9];
      break;
   case 0xB003:
      return m_reg[10];
      break;
   case 0xC000:
   case 0xC001:
   case 0xC002:
   case 0xC003:
      return m_reg[11];
      break;
   case 0xD000:
      return m_reg[12];
      break;
   case 0xD001:
      return m_reg[13];
      break;
   case 0xD002:
      return m_reg[14];
      break;
   case 0xD003:
      return m_reg[15];
      break;
   case 0xE000:
      return m_reg[16];
      break;
   case 0xE001:
      return m_reg[17];
      break;
   case 0xE002:
      return m_reg[18];
      break;
   case 0xE003:
      return m_reg[19];
      break;
   case 0xF000:
      return m_reg[20];
      break;
   case 0xF001:
      return m_reg[21];
      break;
   case 0xF002:
      return m_reg[22];
      break;
   }
   return 0xA1; // garbage
}

void CROMMapper024::HMAPPER ( uint32_t addr, uint8_t data )
{
   uint32_t reg = 0xFFFFFFFF;

   switch ( addr )
   {
   case 0x8000:
   case 0x8001:
   case 0x8002:
   case 0x8003:
      reg = 0;
      m_reg[0] = data;
      m_PRGROMmemory.REMAP(0,(data<<1)+0);
      m_PRGROMmemory.REMAP(1,(data<<1)+1);
      break;
   case 0x9000:
      reg = 1;
      m_reg[1] = data;
      m_pulse[0].REG(0x9000,data);
      break;
   case 0x9001:
      reg = 2;
      m_reg[2] = data;
      m_pulse[0].REG(0x9001,data);
      break;
   case 0x9002:
      reg = 3;
      m_reg[3] = data;
      m_pulse[0].REG(0x9002,data);
      break;
   case 0xA000:
      reg = 4;
      m_reg[4] = data;
      m_pulse[1].REG(0xA000,data);
      break;
   case 0xA001:
      reg = 5;
      m_reg[5] = data;
      m_pulse[1].REG(0xA001,data);
      break;
   case 0xA002:
      reg = 6;
      m_reg[6] = data;
      m_pulse[1].REG(0xA002,data);
      break;
   case 0xB000:
      reg = 7;
      m_reg[7] = data;
      m_sawtooth.REG(0xB000,data);
      break;
   case 0xB001:
      reg = 8;
      m_reg[8] = data;
      m_sawtooth.REG(0xB001,data);
      break;
   case 0xB002:
      reg = 9;
      m_reg[9] = data;
      m_sawtooth.REG(0xB002,data);
      break;
   case 0xB003:
      reg = 10;
      m_reg[10] = data;
      switch ( (data&0x0C)>>2 )
      {
      case 0:
         CNES::NES()->PPU()->MIRRORVERT();
         break;
      case 1:
         CNES::NES()->PPU()->MIRRORHORIZ();
         break;
      case 2:
         CNES::NES()->PPU()->MIRROR(0,0,0,0);
         break;
      case 3:
         CNES::NES()->PPU()->MIRROR(1,1,1,1);
         break;
      }
      break;
   case 0xC000:
   case 0xC001:
   case 0xC002:
   case 0xC003:
      reg = 11;
      m_reg[11] = data;
      m_PRGROMmemory.REMAP(2,data);
      break;
   case 0xD000:
      reg = 12;
      m_reg[12] = data;
      m_chr[0] = data;
      m_CHRmemory.REMAP(0,m_chr[0]);
      break;
   case 0xD001:
      reg = 13;
      m_reg[13] = data;
      m_chr[1] = data;
      m_CHRmemory.REMAP(1,m_chr[1]);
      break;
   case 0xD002:
      reg = 14;
      m_reg[14] = data;
      m_chr[2] = data;
      m_CHRmemory.REMAP(2,m_chr[2]);
      break;
   case 0xD003:
      reg = 15;
      m_reg[15] = data;
      m_chr[3] = data;
      m_CHRmemory.REMAP(3,m_chr[3]);
      break;
   case 0xE000:
      reg = 16;
      m_reg[16] = data;
      m_chr[4] = data;
      m_CHRmemory.REMAP(4,m_chr[4]);
      break;
   case 0xE001:
      reg = 17;
      m_reg[17] = data;
      m_chr[5] = data;
      m_CHRmemory.REMAP(5,m_chr[5]);
      break;
   case 0xE002:
      reg = 18;
      m_reg[18] = data;
      m_chr[6] = data;
      m_CHRmemory.REMAP(6,m_chr[6]);
      break;
   case 0xE003:
      reg = 19;
      m_reg[19] = data;
      m_chr[7] = data;
      m_CHRmemory.REMAP(7,m_chr[7]);
      break;
   case 0xF000:
      reg = 20;
      m_reg[20] = data;
      m_irqReload = data;
      break;
   case 0xF001:
      reg = 21;
      m_reg[21] = data;
      CNES::NES()->CPU()->RELEASEIRQ(eNESSource_Mapper);
      if ( m_reg[21]&0x02 )
      {
         m_irqCounter = m_irqReload;
         m_irqPrescaler = 0;
         m_irqPrescalerPhase = 0;
         m_irqEnabled = true;
      }
      else
      {
         m_irqEnabled = false;
      }
      break;
   case 0xF002:
      reg = 22;
      m_reg[22] = data;
      CNES::NES()->CPU()->RELEASEIRQ(eNESSource_Mapper);
      m_reg[21] &= 0xFD;
      m_reg[21] |= ((m_reg[21]&0x01)<<1);
      if ( m_reg[21]&0x02 )
      {
         m_irqEnabled = true;
      }
      else
      {
         m_irqEnabled = false;
      }
      break;
   }

   if ( nesIsDebuggable() && (reg != 0xFFFFFFFF) )
   {
      // Check mapper state breakpoints...
      CNES::NES()->CHECKBREAKPOINT(eBreakInMapper,eBreakOnMapperState,reg);
   }
}

void VRC6PulseChannel::TIMERTICK()
{
   if ( periodCounter )
   {
      periodCounter--;
   }
   else
   {
      periodCounter = period;
      sequencerStep++;
      sequencerStep %= 16;
   }

   if ( enabled && !muted )
   {
      if ( mode )
      {
         // Digitized mode
         SETDAC(volume);
      }
      else
      {
         if ( sequencerStep <= dutyCycle )
         {
            SETDAC(volume);
         }
         else
         {
            SETDAC(0);
         }
      }
   }
   else
   {
      SETDAC(0);
   }
}

void VRC6SawtoothChannel::TIMERTICK()
{
   if ( periodCounter )
   {
      periodCounter--;
   }
   else
   {
      periodCounter = period;
      if ( accumulatorDivider )
      {
         accumulator += accumulatorRate;
         accumulatorCount++;
         if ( accumulatorCount == 7 )
         {
            accumulator = 0;
            accumulatorCount = 0;
         }
         accumulatorDivider = 0;
      }
      else
      {
         accumulatorDivider++;
      }
   }

   if ( enabled && !muted )
   {
      SETDAC(accumulator>>3);
   }
   else
   {
      SETDAC(0);
   }
}

uint16_t CROMMapper024::AMPLITUDE()
{
   float famp;
   int16_t amp;
   int16_t delta;
   int16_t out[100] = { 0, };
   static int16_t outLast = 0;
   uint8_t sample;
   uint8_t* p1dacSamples = m_pulse[0].GETDACSAMPLES();
   uint8_t* p2dacSamples = m_pulse[1].GETDACSAMPLES();
   uint8_t* sdacSamples = m_sawtooth.GETDACSAMPLES();
   static int32_t outDownsampled = 0;

   m_pulse[0].muted = !(m_soundEnableMask&0x01);
   m_pulse[1].muted = !(m_soundEnableMask&0x02);
   m_sawtooth.muted = !(m_soundEnableMask&0x04);

   for ( sample = 0; sample < m_pulse[0].GETDACSAMPLECOUNT(); sample++ )
   {
      famp = 0.0;
      if ( (*(p1dacSamples+sample))+(*(p2dacSamples+sample))+(*(sdacSamples+sample)) )
      {
         famp = (95.88/((8128.0/((*(p1dacSamples+sample))+(*(p2dacSamples+sample))+(*(sdacSamples+sample))))+100.0));
      }
      amp = (int16_t)(float)(65535.0*famp*0.50);

      (*(out+sample)) = amp;

      outDownsampled += (*(out+sample));
   }

   outDownsampled = (int32_t)((float)outDownsampled/((float)m_pulse[0].GETDACSAMPLECOUNT()));

   delta = outDownsampled - outLast;
   outDownsampled = outLast+((delta*65371)/65536); // 65371/65536 is 0.9975 adjusted to 16-bit fixed point.

   outLast = outDownsampled;

   // Reset DAC averaging...
   m_pulse[0].CLEARDACAVG();
   m_pulse[1].CLEARDACAVG();
   m_sawtooth.CLEARDACAVG();

   return outDownsampled;
}
