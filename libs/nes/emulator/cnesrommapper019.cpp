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

#include "cnesrommapper019.h"
#include "cnes6502.h"
#include "cnesppu.h"

#include "cregisterdata.h"

// Mapper 019 Registers
static CBitfieldData* tbl4800Bitfields [] =
{
   new CBitfieldData("Sound Data", 0, 8, "%02X", 0)
};

static CBitfieldData* tbl5000Bitfields [] =
{
   new CBitfieldData("IRQ Counter LSB", 0, 8, "%02X", 0)
};

static CBitfieldData* tbl5800Bitfields [] =
{
   new CBitfieldData("IRQ Counter MSB", 0, 7, "%X", 0),
   new CBitfieldData("IRQ Enable", 7, 1, "%X", 2, "No", "Yes"),
};

static CBitfieldData* tbl8000Bitfields [] =
{
   new CBitfieldData("CHR Bank", 0, 8, "%02X", 0)
};

static CBitfieldData* tblC000Bitfields [] =
{
   new CBitfieldData("Mirroring", 0, 8, "%02X", 0)
};

static CBitfieldData* tblE000Bitfields [] =
{
   new CBitfieldData("PRG Bank", 0, 6, "%02X", 0)
};

static CBitfieldData* tblE800Bitfields [] =
{
   new CBitfieldData("PRG Bank", 0, 6, "%02X", 0),
   new CBitfieldData("Low CHR RAM", 6, 1, "%X", 2, "Enabled", "Disabled"),
   new CBitfieldData("High CHR RAM", 7, 1, "%X", 2, "Enabled", "Disabled")
};

static CBitfieldData* tblF800Bitfields [] =
{
   new CBitfieldData("Sound Address", 0, 7, "%02X", 0),
   new CBitfieldData("Sound Address Autoincrement", 7, 1, "%X", 2, "No", "Yes")
};

static CRegisterData* tblRegisters [] =
{
   new CRegisterData(0x4800, "Sound Data Port", nesMapperLowRead, nesMapperLowWrite, 1, tbl4800Bitfields),
   new CRegisterData(0x5000, "IRQ Counter LSB", nesMapperLowRead, nesMapperLowWrite, 1, tbl5000Bitfields),
   new CRegisterData(0x5800, "IRQ Counter & Control", nesMapperLowRead, nesMapperLowWrite, 2, tbl5800Bitfields),
   new CRegisterData(0x8000, "CHR Control 0", nesMapperHighRead, nesMapperHighWrite, 1, tbl8000Bitfields),
   new CRegisterData(0x8800, "CHR Control 1", nesMapperHighRead, nesMapperHighWrite, 1, tbl8000Bitfields),
   new CRegisterData(0x9000, "CHR Control 2", nesMapperHighRead, nesMapperHighWrite, 1, tbl8000Bitfields),
   new CRegisterData(0x9800, "CHR Control 3", nesMapperHighRead, nesMapperHighWrite, 1, tbl8000Bitfields),
   new CRegisterData(0xA000, "CHR Control 4", nesMapperHighRead, nesMapperHighWrite, 1, tbl8000Bitfields),
   new CRegisterData(0xA800, "CHR Control 5", nesMapperHighRead, nesMapperHighWrite, 1, tbl8000Bitfields),
   new CRegisterData(0xB000, "CHR Control 6", nesMapperHighRead, nesMapperHighWrite, 1, tbl8000Bitfields),
   new CRegisterData(0xB800, "CHR Control 7", nesMapperHighRead, nesMapperHighWrite, 1, tbl8000Bitfields),
   new CRegisterData(0xC000, "NT A Mirroring", nesMapperHighRead, nesMapperHighWrite, 1, tblC000Bitfields),
   new CRegisterData(0xC800, "NT B Mirroring", nesMapperHighRead, nesMapperHighWrite, 1, tblC000Bitfields),
   new CRegisterData(0xD000, "NT C Mirroring", nesMapperHighRead, nesMapperHighWrite, 1, tblC000Bitfields),
   new CRegisterData(0xD800, "NT D Mirroring", nesMapperHighRead, nesMapperHighWrite, 1, tblC000Bitfields),
   new CRegisterData(0xE000, "PRG Control 0", nesMapperHighRead, nesMapperHighWrite, 1, tblE000Bitfields),
   new CRegisterData(0xE800, "PRG Control 1", nesMapperHighRead, nesMapperHighWrite, 3, tblE800Bitfields),
   new CRegisterData(0xF000, "PRG Control 2", nesMapperHighRead, nesMapperHighWrite, 1, tblE000Bitfields),
   new CRegisterData(0xF800, "Sound Address Port", nesMapperHighRead, nesMapperHighWrite, 1, tblF800Bitfields)
};

static const char* rowHeadings [] =
{
   ""
};

static const char* columnHeadings [] =
{
   "4800","5000","5800",
   "8000","8800","9000","9800","A000","A800","B000","B800",
   "C000","C800","D000","D800",
   "E000","E800","F000",
   "F800"
};

static CRegisterDatabase* dbRegisters = new CRegisterDatabase(eMemory_cartMapper,1,19,19,tblRegisters,rowHeadings,columnHeadings);

uint32_t CROMMapper019::m_soundEnableMask = 0xffffffff;

CROMMapper019::CROMMapper019()
   : CROM(19)
{
   delete m_pSRAMmemory; // Remove open-bus default
   m_pSRAMmemory = new CMEMORY(0x6000,MEM_8KB);
   m_prgRemappable = true;
   m_chrRemappable = true;
   memset(m_reg,0,sizeof(m_reg));
   m_irqCounter = 0;
   m_irqEnabled = false;
   m_soundChansEnabled = 0;
}

CROMMapper019::~CROMMapper019()
{
}

void CROMMapper019::RESET ( bool soft )
{
   int32_t idx;

   m_dbCartRegisters = dbRegisters;

   for ( idx = 0; idx < 8; idx++ )
   {
      m_wave[idx].SOUNDRAM(m_soundRAM);
      m_wave[idx].RESET();
   }
   m_soundRAMAddr = 0;
   m_soundChansEnabled = 0;

   CROM::RESET ( soft );

   m_irqCounter = 0;
   m_irqEnabled = false;

   m_PRGROMmemory.REMAP(2,m_numPrgBanks-2);
   m_PRGROMmemory.REMAP(3,m_numPrgBanks-1);

   // CHR ROM/RAM already set up in CROM::RESET()...
}

void CROMMapper019::SYNCCPU ( bool write, uint16_t addr, uint8_t data )
{
   int32_t idx;

   for ( idx = 0; idx < 8; idx++ )
   {
      m_wave[idx].TIMERTICK(m_soundChansEnabled);
   }

   if ( m_irqEnabled )
   {
      if ( m_irqCounter == 0x7FFF )
      {
         CNES::NES()->CPU()->ASSERTIRQ(eNESSource_Mapper);

         if ( nesIsDebuggable )
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

uint32_t CROMMapper019::DEBUGINFO ( uint32_t addr )
{
   switch ( addr )
   {
   case 0x4800:
      return m_reg[0];
      break;
   case 0x5000:
      return m_reg[1];
      break;
   case 0x5800:
      return m_reg[2];
      break;
   case 0x8000:
      return m_reg[3];
      break;
   case 0x8800:
      return m_reg[4];
      break;
   case 0x9000:
      return m_reg[5];
      break;
   case 0x9800:
      return m_reg[6];
      break;
   case 0xA000:
      return m_reg[7];
      break;
   case 0xA800:
      return m_reg[8];
      break;
   case 0xB000:
      return m_reg[9];
      break;
   case 0xB800:
      return m_reg[10];
      break;
   case 0xC000:
      return m_reg[11];
      break;
   case 0xC800:
      return m_reg[12];
      break;
   case 0xD000:
      return m_reg[13];
      break;
   case 0xD800:
      return m_reg[14];
      break;
   case 0xE000:
      return m_reg[15];
      break;
   case 0xE800:
      return m_reg[16];
      break;
   case 0xF000:
      return m_reg[17];
      break;
   case 0xF800:
      return m_reg[18];
      break;
   }
   return 0xA1; // garbage
}

uint32_t CROMMapper019::LMAPPER ( uint32_t addr )
{
   uint32_t reg = 0;
   uint8_t data = CNES::NES()->CPU()->OPENBUS();

   switch ( addr )
   {
   case 0x4800:
      reg = 0;
      data = m_soundRAM[m_soundRAMAddr];
      if ( m_reg[18]&0x80 )
      {
         m_soundRAMAddr++;
         m_soundRAMAddr &= 0x7F;
      }
      break;
   case 0x5000:
      reg = 1;
      data = m_reg[1];
      CNES::NES()->CPU()->RELEASEIRQ(eNESSource_Mapper);
      break;
   case 0x5800:
      reg = 2;
      data = m_reg[2];
      CNES::NES()->CPU()->RELEASEIRQ(eNESSource_Mapper);
      break;
   }

   if ( nesIsDebuggable )
   {
      // Check mapper state breakpoints...
      CNES::NES()->CHECKBREAKPOINT(eBreakInMapper,eBreakOnMapperState,reg);
   }

   return data;
}

void CROMMapper019::LMAPPER ( uint32_t addr, uint8_t data )
{
   uint32_t reg = 0;

   switch ( addr )
   {
   case 0x4800:
      reg = 0;
      m_reg[0] = data;
      m_soundRAM[m_soundRAMAddr] = data;
      if ( m_soundRAMAddr >= 0x40 )
      {
         m_wave[(m_soundRAMAddr-0x40)/0x8].REG(m_soundRAMAddr&0x7,data);
      }

      // Special case "register" 0x7F for channel enabled count.
      if ( m_soundRAMAddr == 0x7F )
      {
         m_soundChansEnabled = ((data>>4)&0x7);
      }

      if ( m_reg[18]&0x80 )
      {
         m_soundRAMAddr++;
         m_soundRAMAddr &= 0x7F;
      }
      break;
   case 0x5000:
      reg = 1;
      m_reg[1] = data;
      CNES::NES()->CPU()->RELEASEIRQ(eNESSource_Mapper);
      m_irqCounter &= 0xFF00;
      m_irqCounter |= data;
      break;
   case 0x5800:
      reg = 2;
      m_reg[2] = data;
      CNES::NES()->CPU()->RELEASEIRQ(eNESSource_Mapper);
      m_irqCounter &= 0x00FF;
      m_irqCounter |= ((data&0x7F)<<8);
      m_irqEnabled = (data&0x80);
      break;
   }

   if ( nesIsDebuggable )
   {
      // Check mapper state breakpoints...
      CNES::NES()->CHECKBREAKPOINT(eBreakInMapper,eBreakOnMapperState,reg);
   }
}

void CROMMapper019::HMAPPER ( uint32_t addr, uint8_t data )
{
   uint32_t reg = 0;

   switch ( addr )
   {
   case 0x8000:
      reg = 3;
      m_reg[3] = data;
      m_chr[0] = data;
      m_CHRmemory.REMAP(0,m_chr[0]);
      if ( (!(m_reg[16]&0x40)) &&
           (data >= 0xE0) )
      {
         data &= 0x1F;
         // Shove this into unused CHR memory, we'll call it CHR-RAM.
         m_CHRmemory.REMAP(0,0x100+data);
      }
      break;
   case 0x8800:
      reg = 4;
      m_reg[4] = data;
      m_chr[1] = data;
      m_CHRmemory.REMAP(1,m_chr[1]);
      if ( (!(m_reg[16]&0x40)) &&
           (data >= 0xE0) )
      {
         data &= 0x1F;
         // Shove this into unused CHR memory, we'll call it CHR-RAM.
         m_CHRmemory.REMAP(1,0x100+data);
      }
      break;
   case 0x9000:
      reg = 5;
      m_reg[5] = data;
      m_chr[2] = data;
      m_CHRmemory.REMAP(2,m_chr[2]);
      if ( (!(m_reg[16]&0x40)) &&
           (data >= 0xE0) )
      {
         data &= 0x1F;
         // Shove this into unused CHR memory, we'll call it CHR-RAM.
         m_CHRmemory.REMAP(2,0x100+data);
      }
      break;
   case 0x9800:
      reg = 6;
      m_reg[6] = data;
      m_chr[3] = data;
      m_CHRmemory.REMAP(3,m_chr[3]);
      if ( (!(m_reg[16]&0x40)) &&
           (data >= 0xE0) )
      {
         data &= 0x1F;
         // Shove this into unused CHR memory, we'll call it CHR-RAM.
         m_CHRmemory.REMAP(3,0x100+data);
      }
      break;
   case 0xA000:
      reg = 7;
      m_reg[7] = data;
      m_chr[4] = data;
      m_CHRmemory.REMAP(4,m_chr[4]);
      if ( (!(m_reg[16]&0x80)) &&
           (data >= 0xE0) )
      {
         data &= 0x1F;
         // Shove this into unused CHR memory, we'll call it CHR-RAM.
         m_CHRmemory.REMAP(4,0x100+data);
      }
      break;
   case 0xA800:
      reg = 8;
      m_reg[8] = data;
      m_chr[5] = data;
      m_CHRmemory.REMAP(5,m_chr[5]);
      if ( (!(m_reg[16]&0x80)) &&
           (data >= 0xE0) )
      {
         data &= 0x1F;
         // Shove this into unused CHR memory, we'll call it CHR-RAM.
         m_CHRmemory.REMAP(5,0x100+data);
      }
      break;
   case 0xB000:
      reg = 9;
      m_reg[9] = data;
      m_chr[6] = data;
      m_CHRmemory.REMAP(6,m_chr[6]);
      if ( (!(m_reg[16]&0x80)) &&
           (data >= 0xE0) )
      {
         data &= 0x1F;
         // Shove this into unused CHR memory, we'll call it CHR-RAM.
         m_CHRmemory.REMAP(6,0x100+data);
      }
      break;
   case 0xB800:
      reg = 10;
      m_reg[10] = data;
      m_chr[7] = data;
      m_CHRmemory.REMAP(7,m_chr[7]);
      if ( (!(m_reg[16]&0x80)) &&
           (data >= 0xE0) )
      {
         data &= 0x1F;
         // Shove this into unused CHR memory, we'll call it CHR-RAM.
         m_CHRmemory.REMAP(7,0x100+data);
      }
      break;
   case 0xC000:
      reg = 11;
      m_reg[11] = data;
      if ( data < 0xE0 )
      {
         CNES::NES()->PPU()->VRAM()->REMAPEXT ( 0x0, m_CHRmemory.PHYSBANK(m_reg[11]) );
      }
      else
      {
         CNES::NES()->PPU()->MIRROR(m_reg[11]&0x01,-1,-1,-1);
      }
      break;
   case 0xC800:
      reg = 12;
      m_reg[12] = data;
      if ( data < 0xE0 )
      {
         CNES::NES()->PPU()->VRAM()->REMAPEXT ( 0x1, m_CHRmemory.PHYSBANK(m_reg[12]) );
      }
      else
      {
         CNES::NES()->PPU()->MIRROR(-1,m_reg[12]&0x01,-1,-1);
      }
      break;
   case 0xD000:
      reg = 13;
      m_reg[13] = data;
      if ( data < 0xE0 )
      {
         CNES::NES()->PPU()->VRAM()->REMAPEXT ( 0x2, m_CHRmemory.PHYSBANK(m_reg[13]) );
      }
      else
      {
         CNES::NES()->PPU()->MIRROR(-1,-1,m_reg[13]&0x01,-1);
      }
      break;
   case 0xD800:
      reg = 14;
      m_reg[14] = data;
      if ( data < 0xE0 )
      {
         CNES::NES()->PPU()->VRAM()->REMAPEXT ( 0x3, m_CHRmemory.PHYSBANK(m_reg[14]) );
      }
      else
      {
         CNES::NES()->PPU()->MIRROR(-1,-1,-1,m_reg[14]&0x01);
      }
      break;
   case 0xE000:
      reg = 15;
      m_reg[15] = data;
      m_PRGROMmemory.REMAP(0,data&(m_numPrgBanks-1));
      break;
   case 0xE800:
      reg = 16;
      m_reg[16] = data;
      m_PRGROMmemory.REMAP(1,data&(m_numPrgBanks-1));
      break;
   case 0xF000:
      reg = 17;
      m_reg[17] = data;
      m_PRGROMmemory.REMAP(2,data&(m_numPrgBanks-1));
      break;
   case 0xF800:
      reg = 18;
      m_reg[18] = data;
      m_soundRAMAddr = data&0x7F;
      break;
   }

   if ( nesIsDebuggable )
   {
      // Check mapper state breakpoints...
      CNES::NES()->CHECKBREAKPOINT(eBreakInMapper,eBreakOnMapperState,reg);
   }
}

void N106WaveChannel::TIMERTICK(uint8_t enabled)
{
   uint8_t data;

   if ( period )
   {
      if ( periodCounter )
      {
         periodCounter--;
      }
      else
      {
         periodCounter = (0xF0000*(enabled+1))/period;
         if ( instrumentLength )
         {
            instrumentStep++;
            instrumentStep %= instrumentLength;
         }
      }
   }

   if ( period && !muted && instrumentLength )
   {
      // Get instrument sample
      data = (*(pSoundRAM+((instrumentAddress+instrumentStep)>>1)));
      if ( (instrumentAddress+instrumentStep)&1 )
      {
         SETDAC((data>>4)*volume);
      }
      else
      {
         SETDAC((data&0x0F)*volume);
      }
   }
   else
   {
      SETDAC(0);
   }
}

uint16_t CROMMapper019::AMPLITUDE()
{
   float famp;
   int16_t amp;
   int16_t delta;
   int16_t out[100] = { 0, };
   static int16_t outLast = 0;
   uint8_t sample;
   uint8_t* wdacSamples[8];
   static int32_t outDownsampled = 0;
   int32_t idx;

   wdacSamples[0] = m_wave[0].GETDACSAMPLES();
   wdacSamples[1] = m_wave[1].GETDACSAMPLES();
   wdacSamples[2] = m_wave[2].GETDACSAMPLES();
   wdacSamples[3] = m_wave[3].GETDACSAMPLES();
   wdacSamples[4] = m_wave[4].GETDACSAMPLES();
   wdacSamples[5] = m_wave[5].GETDACSAMPLES();
   wdacSamples[6] = m_wave[6].GETDACSAMPLES();
   wdacSamples[7] = m_wave[7].GETDACSAMPLES();

   uint32_t bit;
   for ( bit = 0; bit < 8; bit++ )
   {
      m_wave[bit].muted = !(m_soundEnableMask&(0x01<<bit));
   }

   for ( sample = 0; sample < m_wave[0].GETDACSAMPLECOUNT(); sample++ )
   {
      amp = 0;
      for ( idx = 7-m_soundChansEnabled; idx < 8; idx++ )
      {
         amp += (*(*(wdacSamples+idx)+sample));
      }
      if ( amp )
      {
         famp = (95.88/((35254.0/(float)amp)+100.0));
      }
      else
      {
         famp = 0.0;
      }
      amp = (int16_t)(float)(65535.0*famp*0.50);

      (*(out+sample)) = amp;

      outDownsampled += (*(out+sample));
   }

   outDownsampled = (int32_t)((float)outDownsampled/((float)m_wave[0].GETDACSAMPLECOUNT()));

   delta = outDownsampled - outLast;
   outDownsampled = outLast+((delta*65371)/65536); // 65371/65536 is 0.9975 adjusted to 16-bit fixed point.

   outLast = outDownsampled;

   // Reset DAC averaging...
   m_wave[0].CLEARDACAVG();
   m_wave[1].CLEARDACAVG();
   m_wave[2].CLEARDACAVG();
   m_wave[3].CLEARDACAVG();
   m_wave[4].CLEARDACAVG();
   m_wave[5].CLEARDACAVG();
   m_wave[6].CLEARDACAVG();
   m_wave[7].CLEARDACAVG();

   return outDownsampled;
}
