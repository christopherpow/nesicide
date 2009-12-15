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
    
// ROM.cpp: implementation of the CROM class.
//
//////////////////////////////////////////////////////////////////////

#include "ROM.h"
#include "PPU.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

unsigned char  CROM::m_SRAMmemory [] = { 0, };
unsigned char  CROM::m_EXRAMmemory [] = { 0, };
unsigned char  CROM::m_PRGROMmemory [][ MEM_16KB ] = { { 0, }, };
unsigned char  CROM::m_CHRROMmemory [][ MEM_8KB ] = { { 0, }, };
unsigned char  CROM::m_CHRRAMmemory [ MEM_8KB ] = { 0, };
UINT           CROM::m_PRGROMbank [] = { 0, 0, 0, 0 };
unsigned char* CROM::m_pPRGROMmemory [] = { NULL, NULL, NULL, NULL };
unsigned char* CROM::m_pCHRmemory [] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };
unsigned char* CROM::m_pSRAMmemory = m_SRAMmemory; 

UINT           CROM::m_mapper = 0; 
UINT           CROM::m_numPrgBanks = 0;
UINT           CROM::m_numChrBanks = 0;

CCodeDataLogger* CROM::m_pLogger [] = { NULL, };

static CROM rom = CROM();

CROM::CROM()
{
   int bank;

   for ( bank = 0; bank < NUM_ROM_BANKS; bank++ )
   {
      m_pLogger [ bank ] = new CCodeDataLogger ();
   }

   CROM::RESET ();
}

CROM::~CROM()
{
   int bank;

   for ( bank = 0; bank < NUM_ROM_BANKS; bank++ )
   {
      delete m_pLogger [ bank ];
   }
}

void CROM::Set16KBank ( int bank, unsigned char* data )
{
   memcpy ( m_PRGROMmemory[bank], data, MEM_16KB );
   m_numPrgBanks = bank + 1;
}

void CROM::Set8KBank ( int bank, unsigned char* data )
{
   memcpy ( m_CHRROMmemory[bank], data, MEM_8KB );
   m_numChrBanks = bank + 1;
}

void CROM::DoneLoadingBanks ()
{
   // This is called when the ROM loader is done so that fixup can be done...
   if ( m_numPrgBanks == 1 )
   {
       // If the ROM contains only one PRG-ROM bank then it needs to be replicated
       // to the second PRG-ROM bank slot...
       memcpy ( m_PRGROMmemory[1], m_PRGROMmemory[0], MEM_16KB );

       m_numPrgBanks += 1;
   }
}

void CROM::RESET ()
{
   int bank;

   m_mapper = 0;

   m_pPRGROMmemory [ 0 ] = m_PRGROMmemory [ 0 ] + (0<<UPSHIFT_8KB);
   m_PRGROMbank [ 0 ] = 0;
   m_pPRGROMmemory [ 1 ] = m_PRGROMmemory [ 0 ] + (1<<UPSHIFT_8KB);
   m_PRGROMbank [ 1 ] = 0;
   m_pPRGROMmemory [ 2 ] = m_PRGROMmemory [ 1 ] + (0<<UPSHIFT_8KB);
   m_PRGROMbank [ 2 ] = 1;
   m_pPRGROMmemory [ 3 ] = m_PRGROMmemory [ 1 ] + (1<<UPSHIFT_8KB);
   m_PRGROMbank [ 3 ] = 1;

   // Assume no VROM...point to VRAM...
   for ( bank = 0; bank < 8; bank++ )
   {
      m_pCHRmemory [ bank ] = m_CHRRAMmemory + (bank<<UPSHIFT_1KB);
   }

   // If the cartridge has VROM, map it instead...
   if ( m_numChrBanks > 0 )
   {
      for ( bank = 0; bank < 8; bank++ )
      {
         m_pCHRmemory [ bank ] = m_CHRROMmemory [ 0 ] + (bank<<UPSHIFT_1KB);
      }
   }

   // Clear Code/Data Logger info...
   for ( bank = 0; bank < NUM_ROM_BANKS; bank++ )
   {
      m_pLogger [ bank ]->ClearData ();
   }
}

void CROM::DISPLAY ( char* sz )
{
   sprintf ( sz, "" );
}

void CROM::LOAD ( MapperState* data )
{
   int idx;

   for ( idx = 0; idx < 4; idx++ )
   {
      m_pPRGROMmemory [ idx ] = (unsigned char*)m_PRGROMmemory + data->PRGROMOffset [ idx ];
   }
   for ( idx = 0; idx < 8; idx++ )
   {
      if ( m_numChrBanks )
      {
         m_pCHRmemory [ idx ] = (unsigned char*)m_CHRROMmemory + data->CHRMEMOffset [ idx ];
      }
      else
      {
         m_pCHRmemory [ idx ] = (unsigned char*)m_CHRRAMmemory + data->CHRMEMOffset [ idx ];
      }
   }
}

void CROM::SAVE ( MapperState* data )
{
   int idx;

   for ( idx = 0; idx < 4; idx++ )
   {
      data->PRGROMOffset [ idx ] = m_pPRGROMmemory [ idx ] - (unsigned char*)m_PRGROMmemory;
   }
   for ( idx = 0; idx < 8; idx++ )
   {
      if ( m_numChrBanks )
      {
         data->CHRMEMOffset [ idx ] = m_pCHRmemory [ idx ] - (unsigned char*)m_CHRROMmemory;
      }
      else
      {
         data->CHRMEMOffset [ idx ] = m_pCHRmemory [ idx ] - (unsigned char*)m_CHRRAMmemory;
      }
   }
}
