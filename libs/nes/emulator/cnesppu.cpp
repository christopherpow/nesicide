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

#include "cnessystempalette.h"

#include "cnesppu.h"
#include "cnes6502.h"
#include "cnesrom.h"
#include "cnesapu.h"

#include "nes_emulator_core.h"

// The sprite pattern garbage fetch cycles use sprite tile index $FF.
#define GARBAGE_SPRITE_FETCH 0xFF

// Default palette used in order to pass Blargg's default palette on reset test, even
// though it may not be representative of all NES out there in the world.
static uint8_t tblDefaultPalette [] =
{
   0x09,0x01,0x00,0x01,0x00,0x02,0x02,0x0D,0x08,0x10,0x08,0x24,0x00,0x00,0x04,0x2C,
   0x09,0x01,0x34,0x03,0x00,0x04,0x00,0x14,0x08,0x3A,0x00,0x02,0x00,0x20,0x2C,0x08
};

CPPU::CPPU()
   : m_PPUmemory(CMEMORY(0x2000,MEM_1KB,2,8))
{
   int idx;

   m_oamAddr = 0x00;
   m_ppuRegByte = 0;
   m_ppuAddr = 0x0000;
   m_ppuAddrIncrement = 1;
   m_ppuAddrLatch = 0x0000;
   m_ppuReadLatch = 0x00;
   m_ppuIOLatch = 0x00;
   memset(m_ppuIOLatchDecayFrames,0,sizeof(m_ppuIOLatchDecayFrames));
   memset(m_PPUreg,0,sizeof(m_PPUreg));
   memset(m_PPUoam,0,sizeof(m_PPUoam));
   m_ppuScrollX = 0x00;
   m_oneScreen = -1;

   m_cycles = 0;

   m_vblankChoked = false;
   m_nmiChoked = false;
   m_nmiReenabled = false;

   m_pTV = NULL;

   m_frame = 0;
   m_curCycles = 0;

   m_last2005x = 0;
   m_last2005y = 0;
   m_lastSprite0HitX = 0;
   m_lastSprite0HitY = 0;
   m_x = 0xFF;
   m_y = 0xFF;

   m_logger = new CCodeDataLogger ( MEM_16KB, MASK_16KB );

   m_2005x = new uint16_t*[256];
   for ( idx = 0; idx < 256; idx++ )
   {
      m_2005x[idx] = new uint16_t[240];
   }
   m_2005y = new uint16_t*[256];
   for ( idx = 0; idx < 256; idx++ )
   {
      m_2005y[idx] = new uint16_t[240];
   }
}

CPPU::~CPPU()
{
   int idx;

   delete m_logger;
   for ( idx = 0; idx < 256; idx++ )
   {
      delete [] m_2005x[idx];
   }
   delete [] m_2005x;
   for ( idx = 0; idx < 256; idx++ )
   {
      delete [] m_2005y[idx];
   }
   delete [] m_2005y;
}

void CPPU::EMULATE(uint32_t cycles)
{
   uint32_t idxx = 0xffffffff;
   uint32_t idxy = 0xffffffff;
   uint32_t cycleInScanline;
   uint32_t scanlineInPlane;

   for ( ; cycles > 0; cycles-- )
   {
      // Reduce some maths
      cycleInScanline = m_cycles%PPU_CYCLES_PER_SCANLINE;
      scanlineInPlane = m_cycles/PPU_CYCLES_PER_SCANLINE;

      // Get VBLANK raster position.
      if ( m_cycles >= startVblank )
      {
         idxy = (m_cycles-startVblank)/PPU_CYCLES_PER_SCANLINE;
         idxx = (m_cycles-startVblank)%PPU_CYCLES_PER_SCANLINE;
      }

      // We're emulating one PPU cycle...
      m_curCycles += CPU_CYCLE_ADJUST;

      // Update PPU address from latch at appropriate times...
      // Re-latch PPU address...
      if ( rPPU(PPUMASK)&(PPUMASK_RENDER_BKGND|PPUMASK_RENDER_SPRITES) )
      {
         if ( scanlineInPlane < SCANLINES_VISIBLE )
         {
            if ( cycleInScanline == 257 )
            {
               m_ppuAddr &= 0xFBE0;
               m_ppuAddr |= m_ppuAddrLatch&0x41F;
            }
            else if ( cycleInScanline == 251 )
            {
               if ( (m_ppuAddr&0x7000) == 0x7000 )
               {
                  m_ppuAddr &= 0x8FFF;

                  if ( (m_ppuAddr&0x03E0) == 0x03A0 )
                  {
                     m_ppuAddr ^= 0x0800;
                     m_ppuAddr &= 0xFC1F;
                  }
                  else
                  {
                     if ( (m_ppuAddr&0x03E0) == 0x03E0 )
                     {
                        m_ppuAddr &= 0xFC1F;
                     }
                     else
                     {
                        m_ppuAddr += 0x0020;
                     }
                  }
               }
               else
               {
                  m_ppuAddr += 0x1000;
               }
            }

            if ( ((cycleInScanline&7) == 3) &&
                  ((cycleInScanline < 256) ||
                   (cycleInScanline == 323) ||
                   (cycleInScanline == 331)) )
            {
               if ( (m_ppuAddr&0x001F) != 0x001F )
               {
                  m_ppuAddr++;
               }
               else
               {
                  m_ppuAddr ^= 0x041F;
               }
            }
         }
         else if ( (scanlineInPlane == prerenderScanline) &&
                   (cycleInScanline == 304) )
         {
            m_ppuAddr = m_ppuAddrLatch;
         }
      }

      // Run 0 or 1 CPU cycles...
      CNES::NES()->CPU()->EMULATE ( m_curCycles/cycleRatio );

      // Adjust current cycle count...
      m_curCycles %= cycleRatio;

      // Turn off NMI choking if it shouldn't be...
      if ( m_cycles > startVblank+1 )
      {
         NMICHOKED ( false );
      }

      // Turn off NMI re-enablement if it shouldn't be...
      if ( m_cycles > vblankEndCycle )
      {
         NMIREENABLED ( false );
      }

      if ( nesIsDebuggable )
      {
         // Check for breakpoints...
         CNES::NES()->CHECKBREAKPOINT ( eBreakInPPU, eBreakOnPPUCycle );
      }

      if ( (rPPU(PPUCTRL)&PPUCTRL_GENERATE_NMI) &&
            (((!NMICHOKED()) && (idxy == 0) && (idxx == 1)) ||
             ((NMIREENABLED()) && (idxy <= vblankScanlines-1) && (idxx < PPU_CYCLES_PER_SCANLINE-1))) )
      {
         CNES::NES()->CPU()->ASSERTNMI ();

         // Check for PPU NMI breakpoint...
         CNES::NES()->CHECKBREAKPOINT ( eBreakInPPU, eBreakOnPPUEvent, 0, PPU_EVENT_NMI );
      }

      // Clear OAM at appropriate point...
      // Note the appropriate point comes from blargg's discussion on nesdev forum:
      // http://nesdev.parodius.com/bbs/viewtopic.php?t=1366&highlight=sprite+address+clear
      if ( ((rPPU(PPUMASK)&(PPUMASK_RENDER_BKGND|PPUMASK_RENDER_SPRITES)) == (PPUMASK_RENDER_BKGND|PPUMASK_RENDER_SPRITES)) &&
           (idxy == 19) && (idxx == 316) )
      {
         m_oamAddr = 0x00;
      }

      // Internal cycle counter keeps track of stuff needing to happen
      // at particular PPU frame cycles.  It is reset at the end of a frame.
      m_cycles++;
   }
}

uint32_t CPPU::LOAD ( uint32_t addr, int8_t source, int8_t type, bool trace )
{
   uint8_t data = 0xFF;

   addr &= 0x3FFF;

   if ( addr < 0x2000 )
   {
      data = CNES::NES()->CART()->CHRMEM ( addr );

      // Add Tracer sample...
      if ( nesIsDebuggable && trace )
      {
         CNES::NES()->TRACER()->AddSample ( m_cycles, type, source, eTarget_PatternMemory, addr, data );
      }

      return data;
   }

   if ( addr >= 0x3F00 )
   {
      data = *(m_PALETTEmemory+(addr&0x1F));

      // Add Tracer sample...
      if ( nesIsDebuggable && trace )
      {
         CNES::NES()->TRACER()->AddSample ( m_cycles, type, source, eTarget_Palette, addr, data );
      }

      return data;
   }

   addr -= 0x2000;

   uint32_t tryCart = CNES::NES()->CART()->VRAM(addr);
   data = tryCart&0xFF;
   if ( tryCart == CART_UNCLAIMED )
   {
      data = m_PPUmemory.MEM(addr);
   }

   // Add Tracer sample...
   if ( nesIsDebuggable && trace )
   {
      if ( (addr&0x3FF) < 0x3C0 )
      {
         CNES::NES()->TRACER()->AddSample ( m_cycles, type, source, eTarget_NameTable, addr, data );
      }
      else
      {
         CNES::NES()->TRACER()->AddSample ( m_cycles, type, source, eTarget_AttributeTable, addr, data );
      }
   }

   return data;
}

void CPPU::STORE ( uint32_t addr, uint8_t data, int8_t source, int8_t type, bool trace )
{
   addr &= 0x3FFF;

   if ( addr < 0x2000 )
   {
      // Add Tracer sample...
      if ( nesIsDebuggable && trace )
      {
         CNES::NES()->TRACER()->AddSample ( m_cycles, type, source, eTarget_PatternMemory, addr, data );
      }

      if ( CNES::NES()->CART()->IsWriteProtected() == false )
      {
         CNES::NES()->CART()->CHRMEM ( addr, data );
      }

      return;
   }

   if ( addr >= 0x3F00 )
   {
      // Add Tracer sample...
      if ( nesIsDebuggable && trace )
      {
         CNES::NES()->TRACER()->AddSample ( m_cycles, type, source, eTarget_Palette, addr, data );
      }

      if ( !(addr&0xF) )
      {
         *(m_PALETTEmemory+0x00) = data;
         *(m_PALETTEmemory+0x10) = data;
      }
      else
      {
         *(m_PALETTEmemory+(addr&0x1F)) = data;
      }

      return;
   }

   addr -= 0x2000;

   // Add Tracer sample...
   if ( nesIsDebuggable && trace )
   {
      if ( (addr&0x3FF) < 0x3C0 )
      {
         CNES::NES()->TRACER()->AddSample ( m_cycles, type, source, eTarget_NameTable, addr, data );
      }
      else
      {
         CNES::NES()->TRACER()->AddSample ( m_cycles, type, source, eTarget_AttributeTable, addr, data );
      }
   }

   uint32_t tryCart = CNES::NES()->CART()->VRAM(addr,data);
   if ( tryCart == CART_UNCLAIMED )
   {
      m_PPUmemory.MEM(addr,data);
   }
}

uint32_t CPPU::RENDER ( uint32_t addr, int8_t target )
{
   uint32_t data;

   data = LOAD ( addr, eNESSource_PPU, target );

   if ( nesIsDebuggable )
   {
      m_logger->LogAccess ( CNES::NES()->CPU()->_CYCLES()/*m_cycles*/, addr, data, eLogger_DataRead, eNESSource_PPU );
   }

   // Provide PPU cycle and address to mappers that watch such things!
   CNES::NES()->CART()->SYNCPPU(m_cycles,addr);

   // Address/Data bus multiplexed thus 2 cycles required per access...
   EMULATE(1);

   if ( nesIsDebuggable )
   {
      // Check for PPU address breakpoint...
      CNES::NES()->CHECKBREAKPOINT ( eBreakInPPU, eBreakOnPPUEvent, rPPUADDR(), PPU_EVENT_ADDRESS_EQUALS );

      // Check for breakpoint...
      CNES::NES()->CHECKBREAKPOINT ( eBreakInPPU, eBreakOnPPUFetch, data );
   }

   return data;
}

void CPPU::GARBAGE ( uint32_t addr, int8_t target )
{
   if ( nesIsDebuggable )
   {
      CNES::NES()->TRACER()->AddGarbageFetch ( m_cycles, target, addr );
   }

   // Provide PPU cycle and address to mappers that watch such things!
   CNES::NES()->CART()->SYNCPPU(m_cycles,addr);

   // Address/Data bus multiplexed thus 2 cycles required per access...
   EMULATE(1);

   if ( nesIsDebuggable )
   {
      // Check for PPU address breakpoint...
      CNES::NES()->CHECKBREAKPOINT ( eBreakInPPU, eBreakOnPPUEvent, rPPUADDR(), PPU_EVENT_ADDRESS_EQUALS );

      // Check for breakpoint...
      CNES::NES()->CHECKBREAKPOINT ( eBreakInPPU, eBreakOnPPUFetch );
   }
}

void CPPU::EXTRA ()
{
   if ( nesIsDebuggable )
   {
      CNES::NES()->TRACER()->AddGarbageFetch ( m_cycles, eTarget_ExtraCycle, 0 );
   }

   // Idle cycle...
   EMULATE(1);

   if ( nesIsDebuggable )
   {
      // Check for PPU address breakpoint...
      CNES::NES()->CHECKBREAKPOINT ( eBreakInPPU, eBreakOnPPUEvent, rPPUADDR(), PPU_EVENT_ADDRESS_EQUALS );

      // Check for breakpoint...
      CNES::NES()->CHECKBREAKPOINT ( eBreakInPPU, eBreakOnPPUFetch );
   }
}

void CPPU::RESET ( bool soft )
{
   startVblank = (CNES::NES()->VIDEOMODE()==MODE_NTSC)?PPU_CYCLE_START_VBLANK_NTSC:(CNES::NES()->VIDEOMODE()==MODE_PAL)?PPU_CYCLE_START_VBLANK_PAL:PPU_CYCLE_START_VBLANK_DENDY;
   quietScanlines = (CNES::NES()->VIDEOMODE()==MODE_NTSC)?SCANLINES_QUIET_NTSC:(CNES::NES()->VIDEOMODE()==MODE_PAL)?SCANLINES_QUIET_PAL:SCANLINES_QUIET_DENDY;
   vblankScanlines = (CNES::NES()->VIDEOMODE()==MODE_NTSC)?SCANLINES_VBLANK_NTSC:(CNES::NES()->VIDEOMODE()==MODE_PAL)?SCANLINES_VBLANK_PAL:SCANLINES_VBLANK_DENDY;
   vblankEndCycle = (CNES::NES()->VIDEOMODE()==MODE_NTSC)?PPU_CYCLE_END_VBLANK_NTSC:(CNES::NES()->VIDEOMODE()==MODE_PAL)?PPU_CYCLE_END_VBLANK_PAL:PPU_CYCLE_END_VBLANK_DENDY;
   prerenderScanline = (CNES::NES()->VIDEOMODE()==MODE_NTSC)?SCANLINE_PRERENDER_NTSC:(CNES::NES()->VIDEOMODE()==MODE_PAL)?SCANLINE_PRERENDER_PAL:SCANLINE_PRERENDER_DENDY;
   cycleRatio = (CNES::NES()->VIDEOMODE()==MODE_NTSC)?PPU_CPU_RATIO_NTSC:(CNES::NES()->VIDEOMODE()==MODE_PAL)?PPU_CPU_RATIO_PAL:PPU_CPU_RATIO_DENDY;
   memoryDecayFrames = (CNES::NES()->VIDEOMODE()==MODE_NTSC)?PPU_DECAY_FRAME_COUNT_NTSC:(CNES::NES()->VIDEOMODE()==MODE_PAL)?PPU_DECAY_FRAME_COUNT_PAL:PPU_DECAY_FRAME_COUNT_DENDY;

   m_PPUreg [ 0 ] = 0x00;
   m_PPUreg [ 1 ] = 0x00;
   m_PPUreg [ 2 ] = 0x00;

   m_frame = 0;
   m_cycles = 0;
   m_curCycles = 0;

   m_vblankChoked = false;
   m_nmiChoked = false;
   m_nmiReenabled = false;

   m_ppuAddr = 0x0000;
   m_ppuAddrLatch = 0x0000;
   m_ppuAddrIncrement = 1;
   m_ppuReadLatch = 0x00;
   m_ppuRegByte = 0;
   m_ppuIOLatch = 0x00;
   m_ppuIOLatchDecayFrames [ 0 ] = 0;
   m_ppuIOLatchDecayFrames [ 1 ] = 0;
   m_ppuIOLatchDecayFrames [ 2 ] = 0;
   m_ppuIOLatchDecayFrames [ 3 ] = 0;
   m_ppuIOLatchDecayFrames [ 4 ] = 0;
   m_ppuIOLatchDecayFrames [ 5 ] = 0;
   m_ppuIOLatchDecayFrames [ 6 ] = 0;
   m_ppuIOLatchDecayFrames [ 7 ] = 0;

   m_spriteTemporaryMemory.count = 0;
   m_spriteTemporaryMemory.yByte = SPRITEY;
   m_spriteTemporaryMemory.rolling = 0;

   m_lastSprite0HitX = 0;
   m_lastSprite0HitY = 0;

   // Set up default palette in a way that passes the default palette test.
   PALETTESET ( tblDefaultPalette );

   // Clear memory...
   if ( !soft )
   {
      MEMCLR ();
      OAMCLR ();
   }
}

uint32_t CPPU::PPU ( uint32_t addr )
{
   uint8_t data = 0xFF;
   uint16_t fixAddr;
   uint16_t oldPpuAddr;

   fixAddr = addr&0x0007;

   if ( fixAddr == PPUSTATUS_REG )
   {
      data = *(m_PPUreg+fixAddr);
      *(m_PPUreg+fixAddr) &= (~PPUSTATUS_VBLANK); // VBLANK clear-on-read
      m_ppuRegByte = 0; // Clear PPUADDR address latch
      data = (data&0xE0)|(m_ppuIOLatch&0x1F);

      // Refresh I/O latch...
      m_ppuIOLatch &= (0x1F);
      m_ppuIOLatch |= (data&0xE0);

      // Kill VBLANK flag if register is read at 'wrong' time...
      if ( m_cycles == startVblank-1 )
      {
         VBLANKCHOKED ( true );
      }

      // Kill NMI assertion if register is read at 'wrong' time...
      if ( (m_cycles >= startVblank-1) &&
            (m_cycles <= startVblank+1) )
      {
         NMICHOKED ( true );
      }
   }
   else if ( fixAddr == OAMDATA_REG )
   {
      uint8_t mask[4] = { 0xFF, 0xFF, 0xE3, 0xFF };

      // Third sprite byte should be masked with E3 on read.
      data = (*(m_PPUoam+m_oamAddr))&mask[m_oamAddr&0x3];

      // Refresh I/O latch...
      m_ppuIOLatch = data;

      if ( nesIsDebuggable )
      {
         // Check for breakpoint...
         CNES::NES()->CHECKBREAKPOINT ( eBreakInPPU, eBreakOnOAMPortalRead, data );
      }
   }
   else if ( fixAddr == PPUDATA_REG )
   {
      oldPpuAddr = m_ppuAddr;

      m_ppuAddr += m_ppuAddrIncrement;

      if ( oldPpuAddr < 0x3F00 )
      {
         data = m_ppuReadLatch;

         // Refresh I/O latch...
         m_ppuIOLatch = data;

         m_ppuReadLatch = LOAD ( oldPpuAddr, eNESSource_CPU, eTracer_DataRead );
      }
      else
      {
         data = LOAD ( oldPpuAddr, eNESSource_CPU, eTracer_DataRead );

         // Mask off unused palette RAM bits.
         data &= 0x3F;

         // Fill in read-latch bits...
         data |= (m_ppuIOLatch&0xC0);

         // Refresh I/O latch...
         m_ppuIOLatch &= (0xC0);
         m_ppuIOLatch |= (data&0x3F);

         // Shadow palette/VRAM read, don't use regular LOAD or it will be logged in Tracer...
         m_ppuReadLatch = m_PPUmemory.MEM(oldPpuAddr);
//         m_ppuReadLatch = *((*(m_pPPUmemory+(((oldPpuAddr&(~0x1000))&0x1FFF)>>10)))+((oldPpuAddr&(~0x1000))&0x3FF));
      }

      if ( nesIsDebuggable )
      {
         // Check for breakpoint...
         CNES::NES()->CHECKBREAKPOINT ( eBreakInPPU, eBreakOnPPUPortalRead, data );

         // Log Code/Data logger...
         m_logger->LogAccess ( CNES::NES()->CPU()->_CYCLES()/*m_cycles*/, oldPpuAddr, data, eLogger_DataRead, eNESSource_CPU );
      }

      // Toggling A12 causes IRQ count in some mappers...
      CNES::NES()->CART()->SYNCPPU(m_cycles,m_ppuAddr);
   }
   else
   {
      data = m_ppuIOLatch;
   }

   if ( nesIsDebuggable )
   {
      // Check for breakpoint...
      CNES::NES()->CHECKBREAKPOINT ( eBreakInPPU, eBreakOnPPUState, fixAddr );
   }

   return data;
}

void CPPU::PPU ( uint32_t addr, uint8_t data )
{
   uint16_t fixAddr;
   uint16_t oldPpuAddr;
   uint8_t  old2000;
   int32_t  bit;

   // Set I/O latch for bus hold-up emulation...
   m_ppuIOLatch = data;

   // Set crude 'frame counters' for each 1 bit to decay...
   for ( bit = 0; bit < 8; bit++ )
   {
      if ( m_ppuIOLatch&(1<<bit) )
      {
         // ~600msec
         m_ppuIOLatchDecayFrames [ bit ] = memoryDecayFrames;
      }
      else
      {
         // No decay necessary.
         m_ppuIOLatchDecayFrames [ bit ] = 0;
      }
   }

   fixAddr = addr&0x0007;

   if ( fixAddr != PPUSTATUS_REG )
   {
      old2000 = *(m_PPUreg+PPUCTRL_REG);
      *(m_PPUreg+fixAddr) = data;

      // Check for need to re-assert NMI if NMI is enabled and we're in VBLANK...
      if ( (fixAddr == PPUCTRL_REG) &&
            (!(old2000&PPUCTRL_GENERATE_NMI)) &&
            (rPPU(PPUSTATUS)&PPUSTATUS_VBLANK) &&
            (data&PPUCTRL_GENERATE_NMI) )
      {
         NMIREENABLED ( true );
      }
   }

   if ( fixAddr == PPUCTRL_REG )
   {
      m_ppuAddrLatch &= 0x73FF;
      m_ppuAddrLatch |= ((((uint16_t)data&PPUCTRL_BASE_NAM_TBL_ADDR_MSK))<<10);
      m_ppuAddrIncrement = (((!!(data&PPUCTRL_VRAM_ADDR_INC))*31)+1);
   }
   else if ( fixAddr == OAMADDR_REG )
   {
      m_oamAddr = data;
   }
   else if ( fixAddr == OAMDATA_REG )
   {
      *(m_PPUoam+m_oamAddr) = data;

      if ( nesIsDebuggable )
      {
         // Check for breakpoint...
         CNES::NES()->CHECKBREAKPOINT ( eBreakInPPU, eBreakOnOAMPortalWrite, data );
      }

      m_oamAddr++;
   }
   else if ( fixAddr == PPUSCROLL_REG )
   {
      if ( m_ppuRegByte )
      {
         m_last2005y = data;
         m_ppuAddrLatch &= 0x8C1F;
         m_ppuAddrLatch |= ((((uint16_t)data&0xF8))<<2);
         m_ppuAddrLatch |= ((((uint16_t)data&0x07))<<12);

         if ( nesIsDebuggable )
         {
            // Check for PPU Y scroll update breakpoint...
            CNES::NES()->CHECKBREAKPOINT ( eBreakInPPU, eBreakOnPPUEvent, 0, PPU_EVENT_YSCROLL_UPDATE );
         }
      }
      else
      {
         m_last2005x = data;
         m_ppuScrollX = data&0x07;
         m_ppuAddrLatch &= 0xFFE0;
         m_ppuAddrLatch |= ((((uint16_t)data&0xF8))>>3);

         if ( nesIsDebuggable )
         {
            // Check for PPU X scroll update breakpoint...
            CNES::NES()->CHECKBREAKPOINT ( eBreakInPPU, eBreakOnPPUEvent, 0, PPU_EVENT_XSCROLL_UPDATE );
         }
      }

      m_ppuRegByte = !m_ppuRegByte;
   }
   else if ( fixAddr == PPUADDR_REG )
   {
      if ( m_ppuRegByte )
      {
         m_ppuAddrLatch &= 0x7F00;
         m_ppuAddrLatch |= data;

         m_ppuAddr = m_ppuAddrLatch;

         // Toggling A12 causes IRQ count in some mappers...
         CNES::NES()->CART()->SYNCPPU(m_cycles,m_ppuAddr);
      }
      else
      {
         m_ppuAddrLatch &= 0x00FF;
         m_ppuAddrLatch |= ((((uint16_t)data&0x3F))<<8);
      }

      m_ppuRegByte = !m_ppuRegByte;
   }
   else if ( fixAddr == PPUDATA_REG )
   {
      STORE ( m_ppuAddr, data, eNESSource_CPU, eTracer_DataWrite );

      oldPpuAddr = m_ppuAddr;

      if ( nesIsDebuggable )
      {
         m_logger->LogAccess ( CNES::NES()->CPU()->_CYCLES()/*m_cycles*/, oldPpuAddr, data, eLogger_DataWrite, eNESSource_CPU );

         // Check for breakpoint...
         CNES::NES()->CHECKBREAKPOINT ( eBreakInPPU, eBreakOnPPUPortalWrite, data );
      }

      // Increment PPUADDR
      m_ppuAddr += m_ppuAddrIncrement;

      // Toggling A12 causes IRQ count in some mappers...
      CNES::NES()->CART()->SYNCPPU(m_cycles,m_ppuAddr);
   }

   if ( nesIsDebuggable )
   {
      // Check for breakpoint...
      CNES::NES()->CHECKBREAKPOINT ( eBreakInPPU, eBreakOnPPUState, fixAddr );
   }
}

void CPPU::MIRROR ( int32_t oneScreen, bool vert )
{
   m_oneScreen = oneScreen;

   if ( m_oneScreen >= 0 )
   {
      MIRROR ( m_oneScreen, m_oneScreen, m_oneScreen, m_oneScreen );
   }
   else if ( vert )
   {
      MIRRORVERT ();
   }
   else
   {
      MIRRORHORIZ ();
   }
}

void CPPU::MIRRORVERT ( void )
{
   MIRROR ( 0, 1, 0, 1 );
}

void CPPU::MIRRORHORIZ ( void )
{
   MIRROR ( 0, 0, 1, 1 );
}

void CPPU::MIRROR ( int32_t b0, int32_t b1, int32_t b2, int32_t b3 )
{
   if ( b0 >= 0 )
   {
      b0 &= 0x3;
      m_PPUmemory.REMAP(0,b0);
      m_PPUmemory.REMAP(4,b0);
   }
   if ( b1 >= 0 )
   {
      b1 &= 0x3;
      m_PPUmemory.REMAP(1,b1);
      m_PPUmemory.REMAP(5,b1);
   }
   if ( b2 >= 0 )
   {
      b2 &= 0x3;
      m_PPUmemory.REMAP(2,b2);
      m_PPUmemory.REMAP(6,b2);
   }
   if ( b3 >= 0 )
   {
      b3 &= 0x3;
      m_PPUmemory.REMAP(3,b3);
      m_PPUmemory.REMAP(7,b3);
   }
}

void CPPU::QUIETSCANLINES ( void )
{
   int32_t bit;

   EMULATE(PPU_CYCLES_PER_SCANLINE*quietScanlines);

   // Do I/O latch decay...this is just a convenient place to put
   // this decay because this function is called once per frame and
   // the current emulation of PPU register decay is a frame-count.
   for ( bit = 0; bit < 8; bit++ )
   {
      if ( m_ppuIOLatch&(1<<bit) )
      {
         if ( m_ppuIOLatchDecayFrames[bit] )
         {
            m_ppuIOLatchDecayFrames[bit]--;
         }

         if ( !(m_ppuIOLatchDecayFrames[bit]) )
         {
            m_ppuIOLatch &= (~(1<<bit));
         }
      }
   }
}

void CPPU::VBLANKSCANLINES ( void )
{
   // Set VBLANK flag...
   if ( !VBLANKCHOKED() )
   {
      wPPU ( PPUSTATUS, rPPU(PPUSTATUS)|PPUSTATUS_VBLANK );
   }

   EMULATE(vblankScanlines*PPU_CYCLES_PER_SCANLINE);

   // Clear VBLANK, Sprite 0 Hit flag and sprite overflow...
   wPPU ( PPUSTATUS, rPPU(PPUSTATUS)&(~(PPUSTATUS_VBLANK|PPUSTATUS_SPRITE_0_HIT|PPUSTATUS_SPRITE_OVFLO)) );
}

void CPPU::PIXELPIPELINES ( int32_t pickoff, uint8_t* a, uint8_t* b1, uint8_t* b2 )
{
   BackgroundBufferData* pBkgnd1 = m_bkgndBuffer.data;
   BackgroundBufferData* pBkgnd2 = m_bkgndBuffer.data+1;

   // Adjust pickoff point to place within stored data...
   pickoff = 7-pickoff;

   // Retrieve tile color-bit values at pickoff point...
   (*b1) = (pBkgnd1->patternData1>>pickoff)&0x1;
   (*b2) = (pBkgnd1->patternData2>>pickoff)&0x1;

   // Retrieve attribute table color-bit values at pickoff point...
   (*a) = ((pBkgnd1->attribData1>>pickoff)&0x1)|(((pBkgnd1->attribData2>>pickoff)&0x1)<<1);

   // Shift to the left to line up for next pickoff...
   pBkgnd1->patternData1 <<= 1;
   pBkgnd1->patternData2 <<= 1;
   pBkgnd1->patternData1 |= ((pBkgnd2->patternData1&0x80)>>7);
   pBkgnd1->patternData2 |= ((pBkgnd2->patternData2&0x80)>>7);
   pBkgnd2->patternData1 <<= 1;
   pBkgnd2->patternData2 <<= 1;
   pBkgnd1->attribData1 <<= 1;
   pBkgnd1->attribData2 <<= 1;
   pBkgnd1->attribData1 |= ((pBkgnd2->attribData1&0x80)>>7);
   pBkgnd1->attribData2 |= ((pBkgnd2->attribData2&0x80)>>7);
   pBkgnd2->attribData1 <<= 1;
   pBkgnd2->attribData2 <<= 1;
}

void CPPU::RENDERSCANLINE ( int32_t scanlines )
{
   int32_t idxx;
   int32_t sprite;
   SpriteBufferData* pSprite;
   SpriteTemporaryMemoryData* pSpriteTemp;
   SpriteBufferData* pSelectedSprite;
   SpriteTemporaryMemoryData* pSelectedSpriteTemp;
   int32_t idx2;
   int32_t spriteColorIdx;
   int32_t bkgndColorIdx;
   int32_t startBkgnd;
   int32_t startSprite;
   int start = -1;
   int scanline;
   int32_t rasttv;
   int8_t* pTV;
   int32_t p;
   int greyscale, intenser, intenseg, intenseb;

   if ( scanlines == SCANLINES_VISIBLE )
   {
      start = 0;

      // Get to 239 for loop below.
      scanlines--;
   }

   for ( scanline = start; scanline <= scanlines; scanline++ )
   {
      rasttv = ((scanline<<8)<<2);
      pTV = (int8_t*)(m_pTV+rasttv);
      p = 0;

      m_x = 0;
      m_y = scanline;

      if ( nesIsDebuggable )
      {
         // Check for start-of-scanline breakpoints...
         if ( scanline == -1 )
         {
            CNES::NES()->CHECKBREAKPOINT(eBreakInPPU,eBreakOnPPUEvent,0,PPU_EVENT_PRE_RENDER_SCANLINE_START);
         }
         else
         {
            CNES::NES()->CHECKBREAKPOINT(eBreakInPPU,eBreakOnPPUEvent,0,PPU_EVENT_SCANLINE_START);
         }
      }

      for ( idxx = 0; idxx < 256; idxx++ )
      {
         uint8_t a, b1, b2;

         spriteColorIdx = 0;
         bkgndColorIdx = 0;
         startBkgnd = (!(rPPU(PPUMASK)&PPUMASK_BKGND_CLIPPING))<<3;
         startSprite = (!(rPPU(PPUMASK)&PPUMASK_SPRITE_CLIPPING))<<3;

         // Only render to the screen on the visible scanlines...
         if ( scanline >= 0 )
         {
            if ( nesIsDebuggable )
            {
               m_x = idxx;

               // Update variables for PPU viewer
               *(*(m_2005x+m_x)+m_y) = m_last2005x+((rPPU(PPUCTRL)&0x1)<<8);
               *(*(m_2005y+m_x)+m_y) = m_last2005y+(((rPPU(PPUCTRL)&0x2)>>1)*240);

               // Check for PPU pixel-at breakpoint...
               CNES::NES()->CHECKBREAKPOINT(eBreakInPPU,eBreakOnPPUEvent,0,PPU_EVENT_PIXEL_XY);
            }

            // Run sprite multiplexer to figure out what, if any,
            // sprite pixel to draw here...
            pSelectedSprite = NULL;
            pSelectedSpriteTemp = NULL;

            for ( sprite = 0; sprite < m_spriteBuffer.count; sprite++ )
            {
               pSprite = m_spriteBuffer.data + sprite;
               pSpriteTemp = &(pSprite->temp);
               idx2 = p - pSpriteTemp->spriteX;

               if ( (idx2 >= 0) && (idx2 < PATTERN_SIZE) &&
                     (pSpriteTemp->spriteX+idx2 >= startSprite) &&
                     (pSpriteTemp->spriteX+idx2 >= startBkgnd) )
               {
                  if ( nesIsDebuggable )
                  {
                     // Check for sprite-in-multiplexer event breakpoint...
                     CNES::NES()->CHECKBREAKPOINT(eBreakInPPU,eBreakOnPPUEvent,pSpriteTemp->spriteIdx,PPU_EVENT_SPRITE_IN_MULTIPLEXER);
                  }

                  if ( pSprite->spriteFlipHoriz )
                  {
                     spriteColorIdx = ((pSprite->patternData1>>idx2)&0x01)|((((pSprite->patternData2>>idx2)&0x01)<<1) );
                  }
                  else
                  {
                     spriteColorIdx = ((pSprite->patternData1>>(7-idx2))&0x01)|((((pSprite->patternData2>>(7-idx2))&0x01)<<1) );
                  }

                  spriteColorIdx |= (pSpriteTemp->attribData<<2);

                  // Render background color if necessary...
                  if ( !(spriteColorIdx&0x3) )
                  {
                     spriteColorIdx = 0;
                  }

                  if ( !(rPPU(PPUMASK)&PPUMASK_RENDER_SPRITES) )
                  {
                     spriteColorIdx = 0;
                  }

                  if ( spriteColorIdx&0x3 )
                  {
                     if ( nesIsDebuggable )
                     {
                        // Check for sprite selected event breakpoint...
                        CNES::NES()->CHECKBREAKPOINT(eBreakInPPU,eBreakOnPPUEvent,pSpriteTemp->spriteIdx,PPU_EVENT_SPRITE_SELECTED);
                     }

                     // Save rendered sprite for multiplexing with background...
                     pSelectedSprite = pSprite;
                     pSelectedSpriteTemp = &(pSprite->temp);
                     break;
                  }
               }
            }

            // Background pixel determination...
            PIXELPIPELINES ( rSCROLLX(), &a, &b1, &b2 );
            bkgndColorIdx = ((a<<2)|b1|(b2<<1));

            // Render background color if necessary...
            if ( !(bkgndColorIdx&0x3) )
            {
               bkgndColorIdx = 0;
            }

            if ( !(rPPU(PPUMASK)&PPUMASK_RENDER_BKGND) )
            {
               bkgndColorIdx = 0;
            }

            // invariants
            greyscale = !!(rPPU(PPUMASK)&PPUMASK_GREYSCALE);
            intenser = !!(rPPU(PPUMASK)&PPUMASK_INTENSIFY_REDS);
            intenseg = !!(rPPU(PPUMASK)&PPUMASK_INTENSIFY_GREENS);
            intenseb = !!(rPPU(PPUMASK)&PPUMASK_INTENSIFY_BLUES);

            // Sprite/background pixel rendering determination...
            if ( rPPU(PPUMASK)&(PPUMASK_RENDER_BKGND|PPUMASK_RENDER_SPRITES) )
            {
               if ( (pSelectedSprite) &&
                     ((!(pSelectedSprite->spriteBehind)) ||
                      ((bkgndColorIdx == 0) &&
                       (spriteColorIdx != 0))) )
               {
                  if ( nesIsDebuggable )
                  {
                     // Check for sprite rendering event breakpoint...
                     CNES::NES()->CHECKBREAKPOINT(eBreakInPPU,eBreakOnPPUEvent,pSelectedSpriteTemp->spriteIdx,PPU_EVENT_SPRITE_RENDERING);
                  }

                  // Draw sprite...
                  *pTV++ = CBasePalette::GetPaletteR(rPALETTE(0x10+spriteColorIdx), greyscale, intenser, intenseg, intenseb);
                  *pTV++ = CBasePalette::GetPaletteG(rPALETTE(0x10+spriteColorIdx), greyscale, intenser, intenseg, intenseb);
                  *pTV++ = CBasePalette::GetPaletteB(rPALETTE(0x10+spriteColorIdx), greyscale, intenser, intenseg, intenseb);
               }
               else if ( p>=startBkgnd )
               {
                  // Draw background...
                  *pTV++ = CBasePalette::GetPaletteR(rPALETTE(bkgndColorIdx), greyscale, intenser, intenseg, intenseb);
                  *pTV++ = CBasePalette::GetPaletteG(rPALETTE(bkgndColorIdx), greyscale, intenser, intenseg, intenseb);
                  *pTV++ = CBasePalette::GetPaletteB(rPALETTE(bkgndColorIdx), greyscale, intenser, intenseg, intenseb);
               }
               else
               {
                  // Draw 'nothing'...
                  *pTV++ = CBasePalette::GetPaletteR(rPALETTE(0), greyscale, intenser, intenseg, intenseb);
                  *pTV++ = CBasePalette::GetPaletteG(rPALETTE(0), greyscale, intenser, intenseg, intenseb);
                  *pTV++ = CBasePalette::GetPaletteB(rPALETTE(0), greyscale, intenser, intenseg, intenseb);
               }

               // Sprite 0 hit checks...
               if ( (pSelectedSprite) &&
                    (pSelectedSpriteTemp->spriteIdx == 0) &&
                    (!(rPPU(PPUSTATUS)&PPUSTATUS_SPRITE_0_HIT)) &&
                    (bkgndColorIdx != 0) &&
                    (p < 255) )
               {
                  if ( ((rPPU(PPUMASK)&(PPUMASK_RENDER_BKGND|PPUMASK_RENDER_SPRITES)) == (PPUMASK_RENDER_BKGND|PPUMASK_RENDER_SPRITES)) )
                  {
                     wPPU ( PPUSTATUS, rPPU(PPUSTATUS)|PPUSTATUS_SPRITE_0_HIT );

                     if ( nesIsDebuggable )
                     {
                        // Save last sprite 0 hit coords for OAM viewer...
                        m_lastSprite0HitX = p;
                        m_lastSprite0HitY = scanline;

                        // Add trace tag for Sprice 0 hit...
                        CNES::NES()->TRACER()->AddSample ( m_cycles, eTracer_Sprite0Hit, eNESSource_PPU, 0, 0, 0 );

                        // Check for Sprite 0 Hit breakpoint...
                        CNES::NES()->CHECKBREAKPOINT(eBreakInPPU,eBreakOnPPUEvent,0,PPU_EVENT_SPRITE0_HIT);
                     }
                  }
               }
            }
            else
            {
               if ( (m_ppuAddr&0x3F00) == 0x3F00 )
               {
                  *pTV++ = CBasePalette::GetPaletteR(rPALETTE(m_ppuAddr&0x1F), greyscale, intenser, intenseg, intenseb);
                  *pTV++ = CBasePalette::GetPaletteG(rPALETTE(m_ppuAddr&0x1F), greyscale, intenser, intenseg, intenseb);
                  *pTV++ = CBasePalette::GetPaletteB(rPALETTE(m_ppuAddr&0x1F), greyscale, intenser, intenseg, intenseb);
               }
               else
               {
                  *pTV++ = CBasePalette::GetPaletteR(rPALETTE(0), greyscale, intenser, intenseg, intenseb);
                  *pTV++ = CBasePalette::GetPaletteG(rPALETTE(0), greyscale, intenser, intenseg, intenseb);
                  *pTV++ = CBasePalette::GetPaletteB(rPALETTE(0), greyscale, intenser, intenseg, intenseb);
               }
            }

            // Move to next pixel...
            pTV++; // skip alpha
            p++;
         }

         // Secondary OAM reads occur on even PPU cycles...
         if ( !(idxx&1) )
         {
            BUILDSPRITELIST ( scanline, idxx );
            EMULATE(1);
         }
         else
         {
            GATHERBKGND ( idxx&7 );
         }
      }

      if ( nesIsDebuggable )
      {
         // Check for end-of-scanline breakpoints...
         if ( scanline == -1 )
         {
            CNES::NES()->CHECKBREAKPOINT(eBreakInPPU,eBreakOnPPUEvent,0,PPU_EVENT_PRE_RENDER_SCANLINE_END);
         }
         else
         {
            CNES::NES()->CHECKBREAKPOINT(eBreakInPPU,eBreakOnPPUEvent,0,PPU_EVENT_SCANLINE_END);
         }
      }

      GATHERSPRITES ( scanline );

      // Fill pipeline for next scanline...
      m_bkgndBuffer.data[0].attribData1 = m_bkgndBuffer.data[1].attribData1;
      m_bkgndBuffer.data[0].attribData2 = m_bkgndBuffer.data[1].attribData2;
      m_bkgndBuffer.data[0].patternData1 = m_bkgndBuffer.data[1].patternData1;
      m_bkgndBuffer.data[0].patternData2 = m_bkgndBuffer.data[1].patternData2;

      for ( p = 1; p < 9; p+=2 )
      {
         EMULATE(1);
         GATHERBKGND ( p );
      }

      // Fill pipeline for next scanline...
      m_bkgndBuffer.data[0].attribData1 = m_bkgndBuffer.data[1].attribData1;
      m_bkgndBuffer.data[0].attribData2 = m_bkgndBuffer.data[1].attribData2;
      m_bkgndBuffer.data[0].patternData1 = m_bkgndBuffer.data[1].patternData1;
      m_bkgndBuffer.data[0].patternData2 = m_bkgndBuffer.data[1].patternData2;

      for ( p = 1; p < 9; p+=2 )
      {
         EMULATE(1);
         GATHERBKGND ( p );
      }

      // Finish off scanline render clock cycles...
      EMULATE(1);

      // If this is a visible scanline it is 341 clocks long both NTSC and PAL...
      // The exact skipped cycle appears to be cycle 337, which is right here.
      if ( scanline >= 0 )
      {
         // ...account for extra clock (341)
         EXTRA ();
      }
      // Otherwise, if this is the pre-render scanline it is:
      // 341 dots for PAL, always
      // 340 dots for NTSC odd frames and 341 dots for NTSC even frames if rendering is off, 341 dots for all frames otherwise
      else
      {
         if ( (CNES::NES()->VIDEOMODE() == MODE_DENDY) || (CNES::NES()->VIDEOMODE() == MODE_PAL) || ((CNES::NES()->VIDEOMODE() == MODE_NTSC) && ((!(m_frame&1)) || (!(rPPU(PPUMASK)&(PPUMASK_RENDER_BKGND))))) )
         {
            // account for extra clock (341)
            EXTRA ();
         }
      }

      // Finish off scanline render clock cycles...
      if ( rPPU(PPUMASK)&(PPUMASK_RENDER_BKGND|PPUMASK_RENDER_SPRITES) )
      {
         GARBAGE ( 0x2000, eTarget_NameTable );
      }
      else
      {
         EMULATE(1);
      }
      EMULATE(1);
      if ( rPPU(PPUMASK)&(PPUMASK_RENDER_BKGND|PPUMASK_RENDER_SPRITES) )
      {
         GARBAGE ( 0x2000, eTarget_NameTable );
      }
      else
      {
         EMULATE(1);
      }
   }
}

void CPPU::PIXELRGB ( int32_t x, int32_t y, uint8_t* r, uint8_t* g, uint8_t* b )
{
   if ( (x>=0) && (x<=255) && (y>=0) && (y<=239) )
   {
      int32_t rasttv = (y<<8)<<2;
      int8_t* pTV = (int8_t*)(m_pTV+rasttv);

      pTV += (x<<2);

      (*r) = (*pTV);
      (*g) = (*pTV+1);
      (*b) = (*pTV+2);
   }
}

void CPPU::GATHERBKGND ( int8_t phase )
{
   static uint16_t patternIdx;
   static BackgroundBufferData bkgndTemp;

   uint32_t ppuAddr = rPPUADDR();
   int32_t tileX = ppuAddr&0x001F;
   int32_t tileY = (ppuAddr&0x03E0)>>5;
   int32_t nameAddr = 0x2000 + (ppuAddr&0x0FFF);
   int32_t attribAddr = 0x2000 + (ppuAddr&0x0C00) + 0x03C0 + ((tileY&0xFFFC)<<1) + (tileX>>2);
   uint8_t attribData = 0x00;
   int32_t bkgndPatBase = (!!(rPPU(PPUCTRL)&PPUCTRL_BKGND_PAT_TBL_ADDR))<<12;
   BackgroundBufferData* pBkgnd = m_bkgndBuffer.data + 1;

   if ( phase == 1 )
   {
      if ( rPPU(PPUMASK)&(PPUMASK_RENDER_BKGND|PPUMASK_RENDER_SPRITES) )
      {
         patternIdx = bkgndPatBase+(RENDER(nameAddr,eTracer_RenderBkgnd)<<4)+((ppuAddr&0x7000)>>12);
      }
      else
      {
         EMULATE(1);
      }
   }
   else if ( phase == 3 )
   {
      if ( rPPU(PPUMASK)&(PPUMASK_RENDER_BKGND|PPUMASK_RENDER_SPRITES) )
      {
         attribData = RENDER ( attribAddr,eTracer_RenderBkgnd );
      }
      else
      {
         EMULATE(1);
      }

      if ( (tileY&0x0002) == 0 )
      {
         if ( (tileX&0x0002) == 0 )
         {
            if ( attribData&0x01 )
            {
               bkgndTemp.attribData1 = 0xFF;
            }
            else
            {
               bkgndTemp.attribData1 = 0x00;
            }

            if ( attribData&0x02 )
            {
               bkgndTemp.attribData2 = 0xFF;
            }
            else
            {
               bkgndTemp.attribData2 = 0x00;
            }
         }
         else
         {
            if ( attribData&0x04 )
            {
               bkgndTemp.attribData1 = 0xFF;
            }
            else
            {
               bkgndTemp.attribData1 = 0x00;
            }

            if ( attribData&0x08 )
            {
               bkgndTemp.attribData2 = 0xFF;
            }
            else
            {
               bkgndTemp.attribData2 = 0x00;
            }
         }
      }
      else
      {
         if ( (tileX&0x0002) == 0 )
         {
            if ( attribData&0x10 )
            {
               bkgndTemp.attribData1 = 0xFF;
            }
            else
            {
               bkgndTemp.attribData1 = 0x00;
            }

            if ( attribData&0x20 )
            {
               bkgndTemp.attribData2 = 0xFF;
            }
            else
            {
               bkgndTemp.attribData2 = 0x00;
            }
         }
         else
         {
            if ( attribData&0x40 )
            {
               bkgndTemp.attribData1 = 0xFF;
            }
            else
            {
               bkgndTemp.attribData1 = 0x00;
            }

            if ( attribData&0x80 )
            {
               bkgndTemp.attribData2 = 0xFF;
            }
            else
            {
               bkgndTemp.attribData2 = 0x00;
            }
         }
      }
   }
   else if ( phase == 5 )
   {
      if ( rPPU(PPUMASK)&(PPUMASK_RENDER_BKGND|PPUMASK_RENDER_SPRITES) )
      {
         bkgndTemp.patternData1 = RENDER ( patternIdx,eTracer_RenderBkgnd );
      }
      else
      {
         EMULATE(1);
      }
   }
   else if ( phase == 7 )
   {
      if ( rPPU(PPUMASK)&(PPUMASK_RENDER_BKGND|PPUMASK_RENDER_SPRITES) )
      {
         bkgndTemp.patternData2 = RENDER ( patternIdx+PATTERN_SIZE,eTracer_RenderBkgnd );
      }
      else
      {
         EMULATE(1);
      }

      pBkgnd->attribData1 = bkgndTemp.attribData1;
      pBkgnd->attribData2 = bkgndTemp.attribData2;
      pBkgnd->patternData1 = bkgndTemp.patternData1;
      pBkgnd->patternData2 = bkgndTemp.patternData2;

   }
}

void CPPU::BUILDSPRITELIST ( int32_t scanline, int32_t cycle )
{
   static SpriteTemporaryMemoryData  devNull;
   static SpriteTemporaryMemoryData* pSprite = &devNull;
   static int32_t                    spritesFound = 0;

   int32_t          idx1;
   int32_t          spriteY;
   int32_t          spriteSize;

   // On the first cycle of the scanline, clear the
   // secondary OAM data...
   if ( cycle == 0 )
   {
      spritesFound = 0;
      m_oamAddr = 0;
      pSprite = &devNull;
      m_spriteTemporaryMemory.count = 0;
      m_spriteTemporaryMemory.sprite = (m_oamAddr>>2);
      m_spriteTemporaryMemory.phase = 0;
      m_spriteTemporaryMemory.yByte = (m_oamAddr&0x3);
      m_spriteTemporaryMemory.rolling = 0;
   }

   // Secondary OAM is actually cleared during first 64 cycles of scanline...
   if ( cycle < 64 )
   {
      return;
   }

   // Secondary OAM reads stop after all sprites evaluated...
   if ( m_spriteTemporaryMemory.sprite == NUM_SPRITES )
   {
      return;
   }

   // Loop invariants...
   spriteSize = ((!!(rPPU(PPUCTRL)&PPUCTRL_SPRITE_SIZE))+1)<<3;

   scanline++;

   if ( m_spriteTemporaryMemory.phase == 0 )
   {
      // Populate sprite buffer...
      // Retrieve OAM byte for scanline check...
      // Note: obscure PPU 'bug' in that 9th sprite on scanline
      // causes other sprite data to be used as the Y-coordinate.
      spriteY = OAM ( m_spriteTemporaryMemory.yByte, m_spriteTemporaryMemory.sprite ) + 1;

      // idx1 is sprite slice (it will be in range 0-7 or 0-15 if the sprite
      // is on the next scanline.
      idx1 = scanline-spriteY;

      // Is the sprite on this scanline?
      if ( (idx1 >= 0) && (idx1 < spriteSize) )
      {
         // Calculate sprite-per-scanline limit...
         spritesFound++;

         if ( spritesFound > 8 )
         {
            // Found 8 sprites, point to /dev/null for future sprites on this scanline...
            pSprite = &devNull;

            // Move to next sprite...
            m_spriteTemporaryMemory.sprite++;
            m_spriteTemporaryMemory.phase = 0;

            // Should we assert sprite overflow?
            if ( spritesFound == 9 )
            {
               if ( rPPU(PPUMASK)&(PPUMASK_RENDER_BKGND|PPUMASK_RENDER_SPRITES) )
               {
                  if ( !(rPPU(PPUSTATUS)&PPUSTATUS_SPRITE_OVFLO) )
                  {
                     wPPU(PPUSTATUS,rPPU(PPUSTATUS)|PPUSTATUS_SPRITE_OVFLO );

                     if ( nesIsDebuggable )
                     {
                        // Check for breakpoint...
                        CNES::NES()->CHECKBREAKPOINT ( eBreakInPPU, eBreakOnPPUEvent, 0, PPU_EVENT_SPRITE_OVERFLOW );
                     }
                  }
               }
            }
         }
         else
         {
            // Cause fetch phases to occur for this in-range sprite...
            m_spriteTemporaryMemory.phase++;

            // Found a sprite...
            pSprite = m_spriteTemporaryMemory.data+m_spriteTemporaryMemory.count;
            pSprite->spriteIdx = m_spriteTemporaryMemory.sprite;
            pSprite->spriteSlice = idx1;
         }
      }
      else
      {
         // Sprite not on this scanline, move to next one...
         m_spriteTemporaryMemory.sprite++;
      }

      // If we've found 8 sprites on this scanline, enable the
      // obscure PPU 'bug'.
      if ( (spritesFound == 8) && (!(idx1 >= 0) && (idx1 < spriteSize)) )
      {
         m_spriteTemporaryMemory.rolling = 1;
      }

      // The obscure PPU 'bug' behavior rolls through different
      // sprite data bytes as the Y-coordinate...
      if ( m_spriteTemporaryMemory.rolling )
      {
         m_spriteTemporaryMemory.yByte++;
         m_spriteTemporaryMemory.yByte %= OAM_SIZE;
      }

      if ( spritesFound < NUM_SPRITES_PER_SCANLINE )
      {
         m_spriteTemporaryMemory.count = spritesFound;
      }
      else
      {
         m_spriteTemporaryMemory.count = NUM_SPRITES_PER_SCANLINE;
      }
   }
   else if ( (pSprite != &devNull) && (m_spriteTemporaryMemory.phase == 1) )
   {
      // Cause fetch phases to occur for this in-range sprite...
      m_spriteTemporaryMemory.phase++;

      pSprite->patternIdx = OAM ( SPRITEPAT, pSprite->spriteIdx );
   }
   else if ( (pSprite != &devNull) && (m_spriteTemporaryMemory.phase == 2) )
   {
      // Cause fetch phases to occur for this in-range sprite...
      m_spriteTemporaryMemory.phase++;

      pSprite->attribData = OAM ( SPRITEATT, pSprite->spriteIdx );
   }
   else if ( (pSprite != &devNull) && (m_spriteTemporaryMemory.phase == 3) )
   {
      // Move to next sprite...
      m_spriteTemporaryMemory.sprite++;
      m_spriteTemporaryMemory.phase = 0;

      pSprite->spriteX = OAM ( SPRITEX, pSprite->spriteIdx );
   }

   m_oamAddr = (m_spriteTemporaryMemory.sprite<<2)|(m_spriteTemporaryMemory.phase);
}

void CPPU::GATHERSPRITES ( int32_t scanline )
{
   int32_t idx1;
   int32_t sprite;
   uint16_t spritePatBase = 0x0000;
   int32_t           spriteSize;
   SpriteTemporaryMemoryData* pSpriteTemp;
   SpriteBufferData* pSprite;
   uint8_t patternIdx;

   // Loop invariants...
   spriteSize = ((!!(rPPU(PPUCTRL)&PPUCTRL_SPRITE_SIZE))+1)<<3;

   if ( spriteSize == 8 )
   {
      spritePatBase = (!!(rPPU(PPUCTRL)&PPUCTRL_SPRITE_PAT_TBL_ADDR))<<12;
   }

   // Populate sprite buffer...
   for ( sprite = 0; sprite < m_spriteTemporaryMemory.count; sprite++ )
   {
      // Move sprite data...
      pSpriteTemp = (m_spriteTemporaryMemory.data+sprite);
      pSprite = m_spriteBuffer.data+sprite;

      pSprite->temp.attribData = pSpriteTemp->attribData;
      pSprite->temp.patternIdx = pSpriteTemp->patternIdx;
      pSprite->temp.spriteIdx = pSpriteTemp->spriteIdx;
      pSprite->temp.spriteSlice = pSpriteTemp->spriteSlice;
      pSprite->temp.spriteX = pSpriteTemp->spriteX;
   }

   m_spriteBuffer.count = m_spriteTemporaryMemory.count;

   // Work with sprite buffer...
   for ( sprite = 0; sprite < m_spriteBuffer.count; sprite++ )
   {
      // Get sprite data...
      pSprite = m_spriteBuffer.data+sprite;
      pSpriteTemp = &(pSprite->temp);

      // Get sprite 'slice'...
      idx1 = pSpriteTemp->spriteSlice;

      // Get sprite pattern index...
      patternIdx = pSpriteTemp->patternIdx;

      pSprite->spriteBehind = !!(pSpriteTemp->attribData&SPRITE_PRIORITY);
      pSprite->spriteFlipVert = !!(pSpriteTemp->attribData&SPRITE_FLIP_VERT);
      pSprite->spriteFlipHoriz = !!(pSpriteTemp->attribData&SPRITE_FLIP_HORIZ);

      pSpriteTemp->attribData = pSpriteTemp->attribData&SPRITE_PALETTE_IDX_MSK;

      // For 8x16 sprites...
      if ( spriteSize == 16 )
      {
         spritePatBase = (patternIdx&0x01)<<12;
         patternIdx &= 0xFE;
      }

      if ( (spriteSize == 16) &&
            (((!pSprite->spriteFlipVert) && (idx1 >= PATTERN_SIZE)) ||
             ((pSprite->spriteFlipVert) && (idx1 < PATTERN_SIZE))) )
      {
         patternIdx++;
      }

      if ( pSprite->spriteFlipVert )
      {
         idx1 = spriteSize-1-idx1;
      }

      // Garbage nametable fetches according to Samus Aran...
      if ( rPPU(PPUMASK)&(PPUMASK_RENDER_BKGND|PPUMASK_RENDER_SPRITES) )
      {
         EMULATE(1);
         GARBAGE ( 0x2000, eTarget_NameTable );
         EMULATE(1);
         GARBAGE ( 0x2000, eTarget_NameTable );
      }
      else
      {
         EMULATE(4);
      }

      // Get sprite's pattern data...
      EMULATE(1);

      if ( rPPU(PPUMASK)&(PPUMASK_RENDER_BKGND|PPUMASK_RENDER_SPRITES) )
      {
         pSprite->patternData1 = RENDER ( spritePatBase+(patternIdx<<4)+(idx1&0x7), eTracer_RenderSprite );
      }
      else
      {
         EMULATE(1);
      }

      EMULATE(1);

      if ( rPPU(PPUMASK)&(PPUMASK_RENDER_BKGND|PPUMASK_RENDER_SPRITES) )
      {
         pSprite->patternData2 = RENDER ( spritePatBase+(patternIdx<<4)+(idx1&0x7)+PATTERN_SIZE, eTracer_RenderSprite );
      }
      else
      {
         EMULATE(1);
      }
   }

   // Perform remaining garbage fetches to finish out the scanline's
   // PPU cycles...
   for ( sprite = m_spriteBuffer.count; sprite < NUM_SPRITES_PER_SCANLINE; sprite++ )
   {
      // Garbage nametable fetches according to Samus Aran...
      if ( rPPU(PPUMASK)&(PPUMASK_RENDER_BKGND|PPUMASK_RENDER_SPRITES) )
      {
         EMULATE(1);
         GARBAGE ( 0x2000, eTarget_NameTable );
         EMULATE(1);
         GARBAGE ( 0x2000, eTarget_NameTable );
      }
      else
      {
         EMULATE(4);
      }

      if ( spriteSize == 16 )
      {
         spritePatBase = (GARBAGE_SPRITE_FETCH&0x01)<<12;
      }

      if ( rPPU(PPUMASK)&(PPUMASK_RENDER_BKGND|PPUMASK_RENDER_SPRITES) )
      {
         EMULATE(1);
         GARBAGE ( spritePatBase+(GARBAGE_SPRITE_FETCH<<4), eTarget_PatternMemory );
         EMULATE(1);
         GARBAGE ( spritePatBase+(GARBAGE_SPRITE_FETCH<<4)+PATTERN_SIZE, eTarget_PatternMemory );
      }
      else
      {
         EMULATE(4);
      }
   }
}
