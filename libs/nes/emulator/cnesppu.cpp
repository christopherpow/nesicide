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

// PPU Registers
static CBitfieldData* tblPPUCTRLBitfields [] =
{
   new CBitfieldData("Generate NMI", 7, 1, "%X", 2, "No", "Yes"),
   new CBitfieldData("PPU Master/Slave", 6, 1, "%X", 2, "Master", "Slave"),
   new CBitfieldData("Sprite Size", 5, 1, "%X", 2, "8x8", "8x16"),
   new CBitfieldData("Playfield Pattern Table", 4, 1, "%X", 2, "0000", "1000"),
   new CBitfieldData("Sprite Pattern Table", 3, 1, "%X", 2, "0000", "1000"),
   new CBitfieldData("VRAM Address Increment", 2, 1, "%X", 2, "+1", "+32"),
   new CBitfieldData("NameTable", 0, 2, "%X", 4, "NT1", "NT2", "NT3", "NT4")
};

static CBitfieldData* tblPPUMASKBitfields [] =
{
   new CBitfieldData("Blue Emphasis", 7, 1, "%X", 2, "Off", "On"),
   new CBitfieldData("Green Emphasis", 6, 1, "%X", 2, "Off", "On"),
   new CBitfieldData("Red Emphasis", 5, 1, "%X", 2, "Off", "On"),
   new CBitfieldData("Sprite Rendering", 4, 1, "%X", 2, "Off", "On"),
   new CBitfieldData("Playfield Rendering", 3, 1, "%X", 2, "Off", "On"),
   new CBitfieldData("Sprite Clipping", 2, 1, "%X", 2, "Yes", "No"),
   new CBitfieldData("Playfield Clipping", 1, 1, "%X", 2, "Yes", "No"),
   new CBitfieldData("Greyscale", 0, 1, "%X", 2, "No", "Yes")
};

static CBitfieldData* tblPPUSTATUSBitfields [] =
{
   new CBitfieldData("Vertical Blank", 7, 1, "%X", 2, "No", "Yes"),
   new CBitfieldData("Sprite 0 Hit", 6, 1, "%X", 2, "No", "Yes"),
   new CBitfieldData("Sprite Overflow", 5, 1, "%X", 2, "No", "Yes")
};

static CBitfieldData* tblOAMADDRBitfields [] =
{
   new CBitfieldData("OAM Address", 0, 8, "%02X", 0)
};

static CBitfieldData* tblOAMDATABitfields [] =
{
   new CBitfieldData("OAM Data", 0, 8, "%02X", 0)
};

static CBitfieldData* tblPPUSCROLLBitfields [] =
{
   new CBitfieldData("PPU Scroll", 0, 8, "%02X", 0)
};

static CBitfieldData* tblPPUADDRBitfields [] =
{
   new CBitfieldData("PPU Address", 0, 8, "%02X", 0)
};

static CBitfieldData* tblPPUDATABitfields [] =
{
   new CBitfieldData("PPU Data", 0, 8, "%02X", 0)
};

static CRegisterData* tblPPURegisters [] =
{
   new CRegisterData(0x2000, "PPUCTRL", nesGetPPURegister, nesSetPPURegister, 7, tblPPUCTRLBitfields),
   new CRegisterData(0x2001, "PPUMASK", nesGetPPURegister, nesSetPPURegister, 8, tblPPUMASKBitfields),
   new CRegisterData(0x2002, "PPUSTATUS", nesGetPPURegister, nesSetPPURegister, 3, tblPPUSTATUSBitfields),
   new CRegisterData(0x2003, "OAMADDR", nesGetPPURegister, nesSetPPURegister, 1, tblOAMADDRBitfields),
   new CRegisterData(0x2004, "OAMDATA", nesGetPPURegister, nesSetPPURegister, 1, tblOAMDATABitfields),
   new CRegisterData(0x2005, "PPUSCROLL", nesGetPPURegister, nesSetPPURegister, 1, tblPPUSCROLLBitfields),
   new CRegisterData(0x2006, "PPUADDR", nesGetPPURegister, nesSetPPURegister, 1, tblPPUADDRBitfields),
   new CRegisterData(0x2007, "PPUDATA", nesGetPPURegister, nesSetPPURegister, 1, tblPPUDATABitfields)
};

static const char* rowHeadings [] =
{
   "2000"
};

static const char* columnHeadings [] =
{
   "x0","x1","x2","x3","x4","x5","x6","x7"
};

static CRegisterDatabase* dbRegisters = new CRegisterDatabase(eMemory_PPUregs,1,8,8,tblPPURegisters,rowHeadings,columnHeadings);

CRegisterDatabase* CPPU::m_dbRegisters = dbRegisters;

// OAM 'Registers'
static CBitfieldData* tblOAMYBitfields [] =
{
   new CBitfieldData("Y Position", 0, 8, "%02X", 0),
};

static CBitfieldData* tblOAMPATBitfields [] =
{
   new CBitfieldData("Tile Index", 0, 8, "%02X", 0),
};

static CBitfieldData* tblOAMATTBitfields [] =
{
   new CBitfieldData("Flip Vertical", 7, 1, "%X", 2, "No", "Yes"),
   new CBitfieldData("Flip Horizontal", 6, 1, "%X", 2, "No", "Yes"),
   new CBitfieldData("Priority", 5, 1, "%X", 2, "In-front of Playfield", "Behind Playfield"),
   new CBitfieldData("Palette Index", 0, 2, "%X", 4, "3F10", "3F14", "3F18", "3F1C")
};

static CBitfieldData* tblOAMXBitfields [] =
{
   new CBitfieldData("X Position", 0, 8, "%02X", 0),
};

static CRegisterData* tblOAMRegisters [] =
{
   // Sprite 0
   new CRegisterData(0x00, "OAMY", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMYBitfields),
   new CRegisterData(0x01, "OAMPAT", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMPATBitfields),
   new CRegisterData(0x02, "OAMATT", nesGetPPUOAM, nesSetPPUOAM, 4, tblOAMATTBitfields),
   new CRegisterData(0x03, "OAMX", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMXBitfields),

   // Sprite 1
   new CRegisterData(0x04, "OAMY", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMYBitfields),
   new CRegisterData(0x05, "OAMPAT", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMPATBitfields),
   new CRegisterData(0x06, "OAMATT", nesGetPPUOAM, nesSetPPUOAM, 4, tblOAMATTBitfields),
   new CRegisterData(0x07, "OAMX", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMXBitfields),

   // Sprite 2
   new CRegisterData(0x08, "OAMY", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMYBitfields),
   new CRegisterData(0x09, "OAMPAT", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMPATBitfields),
   new CRegisterData(0x0A, "OAMATT", nesGetPPUOAM, nesSetPPUOAM, 4, tblOAMATTBitfields),
   new CRegisterData(0x0B, "OAMX", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMXBitfields),

   // Sprite 3
   new CRegisterData(0x0C, "OAMY", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMYBitfields),
   new CRegisterData(0x0D, "OAMPAT", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMPATBitfields),
   new CRegisterData(0x0E, "OAMATT", nesGetPPUOAM, nesSetPPUOAM, 4, tblOAMATTBitfields),
   new CRegisterData(0x0F, "OAMX", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMXBitfields),

   // Sprite 4
   new CRegisterData(0x10, "OAMY", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMYBitfields),
   new CRegisterData(0x11, "OAMPAT", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMPATBitfields),
   new CRegisterData(0x12, "OAMATT", nesGetPPUOAM, nesSetPPUOAM, 4, tblOAMATTBitfields),
   new CRegisterData(0x13, "OAMX", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMXBitfields),

   // Sprite 5
   new CRegisterData(0x14, "OAMY", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMYBitfields),
   new CRegisterData(0x15, "OAMPAT", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMPATBitfields),
   new CRegisterData(0x16, "OAMATT", nesGetPPUOAM, nesSetPPUOAM, 4, tblOAMATTBitfields),
   new CRegisterData(0x17, "OAMX", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMXBitfields),

   // Sprite 6
   new CRegisterData(0x18, "OAMY", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMYBitfields),
   new CRegisterData(0x19, "OAMPAT", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMPATBitfields),
   new CRegisterData(0x1A, "OAMATT", nesGetPPUOAM, nesSetPPUOAM, 4, tblOAMATTBitfields),
   new CRegisterData(0x1B, "OAMX", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMXBitfields),

   // Sprite 7
   new CRegisterData(0x1C, "OAMY", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMYBitfields),
   new CRegisterData(0x1D, "OAMPAT", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMPATBitfields),
   new CRegisterData(0x1E, "OAMATT", nesGetPPUOAM, nesSetPPUOAM, 4, tblOAMATTBitfields),
   new CRegisterData(0x1F, "OAMX", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMXBitfields),

   // Sprite 8
   new CRegisterData(0x20, "OAMY", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMYBitfields),
   new CRegisterData(0x21, "OAMPAT", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMPATBitfields),
   new CRegisterData(0x22, "OAMATT", nesGetPPUOAM, nesSetPPUOAM, 4, tblOAMATTBitfields),
   new CRegisterData(0x23, "OAMX", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMXBitfields),

   // Sprite 9
   new CRegisterData(0x24, "OAMY", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMYBitfields),
   new CRegisterData(0x25, "OAMPAT", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMPATBitfields),
   new CRegisterData(0x26, "OAMATT", nesGetPPUOAM, nesSetPPUOAM, 4, tblOAMATTBitfields),
   new CRegisterData(0x27, "OAMX", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMXBitfields),

   // Sprite 10
   new CRegisterData(0x28, "OAMY", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMYBitfields),
   new CRegisterData(0x29, "OAMPAT", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMPATBitfields),
   new CRegisterData(0x2A, "OAMATT", nesGetPPUOAM, nesSetPPUOAM, 4, tblOAMATTBitfields),
   new CRegisterData(0x2B, "OAMX", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMXBitfields),

   // Sprite 11
   new CRegisterData(0x2C, "OAMY", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMYBitfields),
   new CRegisterData(0x2D, "OAMPAT", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMPATBitfields),
   new CRegisterData(0x2E, "OAMATT", nesGetPPUOAM, nesSetPPUOAM, 4, tblOAMATTBitfields),
   new CRegisterData(0x2F, "OAMX", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMXBitfields),

   // Sprite 12
   new CRegisterData(0x30, "OAMY", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMYBitfields),
   new CRegisterData(0x31, "OAMPAT", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMPATBitfields),
   new CRegisterData(0x32, "OAMATT", nesGetPPUOAM, nesSetPPUOAM, 4, tblOAMATTBitfields),
   new CRegisterData(0x33, "OAMX", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMXBitfields),

   // Sprite 13
   new CRegisterData(0x34, "OAMY", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMYBitfields),
   new CRegisterData(0x35, "OAMPAT", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMPATBitfields),
   new CRegisterData(0x36, "OAMATT", nesGetPPUOAM, nesSetPPUOAM, 4, tblOAMATTBitfields),
   new CRegisterData(0x37, "OAMX", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMXBitfields),

   // Sprite 14
   new CRegisterData(0x38, "OAMY", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMYBitfields),
   new CRegisterData(0x39, "OAMPAT", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMPATBitfields),
   new CRegisterData(0x3A, "OAMATT", nesGetPPUOAM, nesSetPPUOAM, 4, tblOAMATTBitfields),
   new CRegisterData(0x3B, "OAMX", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMXBitfields),

   // Sprite 15
   new CRegisterData(0x3C, "OAMY", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMYBitfields),
   new CRegisterData(0x3D, "OAMPAT", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMPATBitfields),
   new CRegisterData(0x3E, "OAMATT", nesGetPPUOAM, nesSetPPUOAM, 4, tblOAMATTBitfields),
   new CRegisterData(0x3F, "OAMX", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMXBitfields),

   // Sprite 16
   new CRegisterData(0x40, "OAMY", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMYBitfields),
   new CRegisterData(0x41, "OAMPAT", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMPATBitfields),
   new CRegisterData(0x42, "OAMATT", nesGetPPUOAM, nesSetPPUOAM, 4, tblOAMATTBitfields),
   new CRegisterData(0x43, "OAMX", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMXBitfields),

   // Sprite 17
   new CRegisterData(0x44, "OAMY", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMYBitfields),
   new CRegisterData(0x45, "OAMPAT", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMPATBitfields),
   new CRegisterData(0x46, "OAMATT", nesGetPPUOAM, nesSetPPUOAM, 4, tblOAMATTBitfields),
   new CRegisterData(0x47, "OAMX", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMXBitfields),

   // Sprite 18
   new CRegisterData(0x48, "OAMY", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMYBitfields),
   new CRegisterData(0x49, "OAMPAT", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMPATBitfields),
   new CRegisterData(0x4A, "OAMATT", nesGetPPUOAM, nesSetPPUOAM, 4, tblOAMATTBitfields),
   new CRegisterData(0x4B, "OAMX", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMXBitfields),

   // Sprite 19
   new CRegisterData(0x4C, "OAMY", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMYBitfields),
   new CRegisterData(0x4D, "OAMPAT", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMPATBitfields),
   new CRegisterData(0x4E, "OAMATT", nesGetPPUOAM, nesSetPPUOAM, 4, tblOAMATTBitfields),
   new CRegisterData(0x4F, "OAMX", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMXBitfields),

   // Sprite 20
   new CRegisterData(0x50, "OAMY", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMYBitfields),
   new CRegisterData(0x51, "OAMPAT", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMPATBitfields),
   new CRegisterData(0x52, "OAMATT", nesGetPPUOAM, nesSetPPUOAM, 4, tblOAMATTBitfields),
   new CRegisterData(0x53, "OAMX", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMXBitfields),

   // Sprite 21
   new CRegisterData(0x54, "OAMY", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMYBitfields),
   new CRegisterData(0x55, "OAMPAT", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMPATBitfields),
   new CRegisterData(0x56, "OAMATT", nesGetPPUOAM, nesSetPPUOAM, 4, tblOAMATTBitfields),
   new CRegisterData(0x57, "OAMX", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMXBitfields),

   // Sprite 22
   new CRegisterData(0x58, "OAMY", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMYBitfields),
   new CRegisterData(0x59, "OAMPAT", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMPATBitfields),
   new CRegisterData(0x5A, "OAMATT", nesGetPPUOAM, nesSetPPUOAM, 4, tblOAMATTBitfields),
   new CRegisterData(0x5B, "OAMX", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMXBitfields),

   // Sprite 23
   new CRegisterData(0x5C, "OAMY", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMYBitfields),
   new CRegisterData(0x5D, "OAMPAT", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMPATBitfields),
   new CRegisterData(0x5E, "OAMATT", nesGetPPUOAM, nesSetPPUOAM, 4, tblOAMATTBitfields),
   new CRegisterData(0x5F, "OAMX", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMXBitfields),

   // Sprite 24
   new CRegisterData(0x60, "OAMY", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMYBitfields),
   new CRegisterData(0x61, "OAMPAT", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMPATBitfields),
   new CRegisterData(0x62, "OAMATT", nesGetPPUOAM, nesSetPPUOAM, 4, tblOAMATTBitfields),
   new CRegisterData(0x63, "OAMX", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMXBitfields),

   // Sprite 25
   new CRegisterData(0x64, "OAMY", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMYBitfields),
   new CRegisterData(0x65, "OAMPAT", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMPATBitfields),
   new CRegisterData(0x66, "OAMATT", nesGetPPUOAM, nesSetPPUOAM, 4, tblOAMATTBitfields),
   new CRegisterData(0x67, "OAMX", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMXBitfields),

   // Sprite 26
   new CRegisterData(0x68, "OAMY", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMYBitfields),
   new CRegisterData(0x69, "OAMPAT", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMPATBitfields),
   new CRegisterData(0x6A, "OAMATT", nesGetPPUOAM, nesSetPPUOAM, 4, tblOAMATTBitfields),
   new CRegisterData(0x6B, "OAMX", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMXBitfields),

   // Sprite 27
   new CRegisterData(0x6C, "OAMY", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMYBitfields),
   new CRegisterData(0x6D, "OAMPAT", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMPATBitfields),
   new CRegisterData(0x6E, "OAMATT", nesGetPPUOAM, nesSetPPUOAM, 4, tblOAMATTBitfields),
   new CRegisterData(0x6F, "OAMX", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMXBitfields),

   // Sprite 28
   new CRegisterData(0x70, "OAMY", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMYBitfields),
   new CRegisterData(0x71, "OAMPAT", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMPATBitfields),
   new CRegisterData(0x72, "OAMATT", nesGetPPUOAM, nesSetPPUOAM, 4, tblOAMATTBitfields),
   new CRegisterData(0x73, "OAMX", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMXBitfields),

   // Sprite 29
   new CRegisterData(0x74, "OAMY", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMYBitfields),
   new CRegisterData(0x75, "OAMPAT", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMPATBitfields),
   new CRegisterData(0x76, "OAMATT", nesGetPPUOAM, nesSetPPUOAM, 4, tblOAMATTBitfields),
   new CRegisterData(0x77, "OAMX", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMXBitfields),

   // Sprite 30
   new CRegisterData(0x78, "OAMY", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMYBitfields),
   new CRegisterData(0x79, "OAMPAT", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMPATBitfields),
   new CRegisterData(0x7A, "OAMATT", nesGetPPUOAM, nesSetPPUOAM, 4, tblOAMATTBitfields),
   new CRegisterData(0x7B, "OAMX", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMXBitfields),

   // Sprite 31
   new CRegisterData(0x7C, "OAMY", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMYBitfields),
   new CRegisterData(0x7D, "OAMPAT", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMPATBitfields),
   new CRegisterData(0x7E, "OAMATT", nesGetPPUOAM, nesSetPPUOAM, 4, tblOAMATTBitfields),
   new CRegisterData(0x7F, "OAMX", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMXBitfields),

   // Sprite 32
   new CRegisterData(0x80, "OAMY", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMYBitfields),
   new CRegisterData(0x81, "OAMPAT", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMPATBitfields),
   new CRegisterData(0x82, "OAMATT", nesGetPPUOAM, nesSetPPUOAM, 4, tblOAMATTBitfields),
   new CRegisterData(0x83, "OAMX", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMXBitfields),

   // Sprite 33
   new CRegisterData(0x84, "OAMY", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMYBitfields),
   new CRegisterData(0x85, "OAMPAT", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMPATBitfields),
   new CRegisterData(0x86, "OAMATT", nesGetPPUOAM, nesSetPPUOAM, 4, tblOAMATTBitfields),
   new CRegisterData(0x87, "OAMX", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMXBitfields),

   // Sprite 34
   new CRegisterData(0x88, "OAMY", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMYBitfields),
   new CRegisterData(0x89, "OAMPAT", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMPATBitfields),
   new CRegisterData(0x8A, "OAMATT", nesGetPPUOAM, nesSetPPUOAM, 4, tblOAMATTBitfields),
   new CRegisterData(0x8B, "OAMX", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMXBitfields),

   // Sprite 35
   new CRegisterData(0x8C, "OAMY", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMYBitfields),
   new CRegisterData(0x8D, "OAMPAT", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMPATBitfields),
   new CRegisterData(0x8E, "OAMATT", nesGetPPUOAM, nesSetPPUOAM, 4, tblOAMATTBitfields),
   new CRegisterData(0x8F, "OAMX", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMXBitfields),

   // Sprite 36
   new CRegisterData(0x90, "OAMY", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMYBitfields),
   new CRegisterData(0x91, "OAMPAT", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMPATBitfields),
   new CRegisterData(0x92, "OAMATT", nesGetPPUOAM, nesSetPPUOAM, 4, tblOAMATTBitfields),
   new CRegisterData(0x93, "OAMX", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMXBitfields),

   // Sprite 37
   new CRegisterData(0x94, "OAMY", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMYBitfields),
   new CRegisterData(0x95, "OAMPAT", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMPATBitfields),
   new CRegisterData(0x96, "OAMATT", nesGetPPUOAM, nesSetPPUOAM, 4, tblOAMATTBitfields),
   new CRegisterData(0x97, "OAMX", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMXBitfields),

   // Sprite 38
   new CRegisterData(0x98, "OAMY", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMYBitfields),
   new CRegisterData(0x99, "OAMPAT", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMPATBitfields),
   new CRegisterData(0x9A, "OAMATT", nesGetPPUOAM, nesSetPPUOAM, 4, tblOAMATTBitfields),
   new CRegisterData(0x9B, "OAMX", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMXBitfields),

   // Sprite 39
   new CRegisterData(0x9C, "OAMY", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMYBitfields),
   new CRegisterData(0x9D, "OAMPAT", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMPATBitfields),
   new CRegisterData(0x9E, "OAMATT", nesGetPPUOAM, nesSetPPUOAM, 4, tblOAMATTBitfields),
   new CRegisterData(0x9F, "OAMX", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMXBitfields),

   // Sprite 40
   new CRegisterData(0xA0, "OAMY", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMYBitfields),
   new CRegisterData(0xA1, "OAMPAT", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMPATBitfields),
   new CRegisterData(0xA2, "OAMATT", nesGetPPUOAM, nesSetPPUOAM, 4, tblOAMATTBitfields),
   new CRegisterData(0xA3, "OAMX", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMXBitfields),

   // Sprite 41
   new CRegisterData(0xA4, "OAMY", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMYBitfields),
   new CRegisterData(0xA5, "OAMPAT", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMPATBitfields),
   new CRegisterData(0xA6, "OAMATT", nesGetPPUOAM, nesSetPPUOAM, 4, tblOAMATTBitfields),
   new CRegisterData(0xA7, "OAMX", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMXBitfields),

   // Sprite 42
   new CRegisterData(0xA8, "OAMY", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMYBitfields),
   new CRegisterData(0xA9, "OAMPAT", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMPATBitfields),
   new CRegisterData(0xAA, "OAMATT", nesGetPPUOAM, nesSetPPUOAM, 4, tblOAMATTBitfields),
   new CRegisterData(0xAB, "OAMX", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMXBitfields),

   // Sprite 43
   new CRegisterData(0xAC, "OAMY", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMYBitfields),
   new CRegisterData(0xAD, "OAMPAT", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMPATBitfields),
   new CRegisterData(0xAE, "OAMATT", nesGetPPUOAM, nesSetPPUOAM, 4, tblOAMATTBitfields),
   new CRegisterData(0xAF, "OAMX", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMXBitfields),

   // Sprite 44
   new CRegisterData(0xB0, "OAMY", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMYBitfields),
   new CRegisterData(0xB1, "OAMPAT", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMPATBitfields),
   new CRegisterData(0xB2, "OAMATT", nesGetPPUOAM, nesSetPPUOAM, 4, tblOAMATTBitfields),
   new CRegisterData(0xB3, "OAMX", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMXBitfields),

   // Sprite 45
   new CRegisterData(0xB4, "OAMY", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMYBitfields),
   new CRegisterData(0xB5, "OAMPAT", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMPATBitfields),
   new CRegisterData(0xB6, "OAMATT", nesGetPPUOAM, nesSetPPUOAM, 4, tblOAMATTBitfields),
   new CRegisterData(0xB7, "OAMX", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMXBitfields),

   // Sprite 46
   new CRegisterData(0xB8, "OAMY", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMYBitfields),
   new CRegisterData(0xB9, "OAMPAT", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMPATBitfields),
   new CRegisterData(0xBA, "OAMATT", nesGetPPUOAM, nesSetPPUOAM, 4, tblOAMATTBitfields),
   new CRegisterData(0xBB, "OAMX", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMXBitfields),

   // Sprite 47
   new CRegisterData(0xBC, "OAMY", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMYBitfields),
   new CRegisterData(0xBD, "OAMPAT", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMPATBitfields),
   new CRegisterData(0xBE, "OAMATT", nesGetPPUOAM, nesSetPPUOAM, 4, tblOAMATTBitfields),
   new CRegisterData(0xBF, "OAMX", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMXBitfields),

   // Sprite 48
   new CRegisterData(0xC0, "OAMY", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMYBitfields),
   new CRegisterData(0xC1, "OAMPAT", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMPATBitfields),
   new CRegisterData(0xC2, "OAMATT", nesGetPPUOAM, nesSetPPUOAM, 4, tblOAMATTBitfields),
   new CRegisterData(0xC3, "OAMX", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMXBitfields),

   // Sprite 49
   new CRegisterData(0xC4, "OAMY", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMYBitfields),
   new CRegisterData(0xC5, "OAMPAT", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMPATBitfields),
   new CRegisterData(0xC6, "OAMATT", nesGetPPUOAM, nesSetPPUOAM, 4, tblOAMATTBitfields),
   new CRegisterData(0xC7, "OAMX", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMXBitfields),

   // Sprite 50
   new CRegisterData(0xC8, "OAMY", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMYBitfields),
   new CRegisterData(0xC9, "OAMPAT", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMPATBitfields),
   new CRegisterData(0xCA, "OAMATT", nesGetPPUOAM, nesSetPPUOAM, 4, tblOAMATTBitfields),
   new CRegisterData(0xCB, "OAMX", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMXBitfields),

   // Sprite 51
   new CRegisterData(0xCC, "OAMY", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMYBitfields),
   new CRegisterData(0xCD, "OAMPAT", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMPATBitfields),
   new CRegisterData(0xCE, "OAMATT", nesGetPPUOAM, nesSetPPUOAM, 4, tblOAMATTBitfields),
   new CRegisterData(0xCF, "OAMX", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMXBitfields),

   // Sprite 52
   new CRegisterData(0xD0, "OAMY", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMYBitfields),
   new CRegisterData(0xD1, "OAMPAT", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMPATBitfields),
   new CRegisterData(0xD2, "OAMATT", nesGetPPUOAM, nesSetPPUOAM, 4, tblOAMATTBitfields),
   new CRegisterData(0xD3, "OAMX", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMXBitfields),

   // Sprite 53
   new CRegisterData(0xD4, "OAMY", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMYBitfields),
   new CRegisterData(0xD5, "OAMPAT", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMPATBitfields),
   new CRegisterData(0xD6, "OAMATT", nesGetPPUOAM, nesSetPPUOAM, 4, tblOAMATTBitfields),
   new CRegisterData(0xD7, "OAMX", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMXBitfields),

   // Sprite 54
   new CRegisterData(0xD8, "OAMY", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMYBitfields),
   new CRegisterData(0xD9, "OAMPAT", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMPATBitfields),
   new CRegisterData(0xDA, "OAMATT", nesGetPPUOAM, nesSetPPUOAM, 4, tblOAMATTBitfields),
   new CRegisterData(0xDB, "OAMX", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMXBitfields),

   // Sprite 55
   new CRegisterData(0xDC, "OAMY", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMYBitfields),
   new CRegisterData(0xDD, "OAMPAT", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMPATBitfields),
   new CRegisterData(0xDE, "OAMATT", nesGetPPUOAM, nesSetPPUOAM, 4, tblOAMATTBitfields),
   new CRegisterData(0xDF, "OAMX", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMXBitfields),

   // Sprite 56
   new CRegisterData(0xE0, "OAMY", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMYBitfields),
   new CRegisterData(0xE1, "OAMPAT", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMPATBitfields),
   new CRegisterData(0xE2, "OAMATT", nesGetPPUOAM, nesSetPPUOAM, 4, tblOAMATTBitfields),
   new CRegisterData(0xE3, "OAMX", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMXBitfields),

   // Sprite 57
   new CRegisterData(0xE4, "OAMY", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMYBitfields),
   new CRegisterData(0xE5, "OAMPAT", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMPATBitfields),
   new CRegisterData(0xE6, "OAMATT", nesGetPPUOAM, nesSetPPUOAM, 4, tblOAMATTBitfields),
   new CRegisterData(0xE7, "OAMX", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMXBitfields),

   // Sprite 58
   new CRegisterData(0xE8, "OAMY", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMYBitfields),
   new CRegisterData(0xE9, "OAMPAT", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMPATBitfields),
   new CRegisterData(0xEA, "OAMATT", nesGetPPUOAM, nesSetPPUOAM, 4, tblOAMATTBitfields),
   new CRegisterData(0xEB, "OAMX", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMXBitfields),

   // Sprite 59
   new CRegisterData(0xEC, "OAMY", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMYBitfields),
   new CRegisterData(0xED, "OAMPAT", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMPATBitfields),
   new CRegisterData(0xEE, "OAMATT", nesGetPPUOAM, nesSetPPUOAM, 4, tblOAMATTBitfields),
   new CRegisterData(0xEF, "OAMX", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMXBitfields),

   // Sprite 60
   new CRegisterData(0xF0, "OAMY", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMYBitfields),
   new CRegisterData(0xF1, "OAMPAT", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMPATBitfields),
   new CRegisterData(0xF2, "OAMATT", nesGetPPUOAM, nesSetPPUOAM, 4, tblOAMATTBitfields),
   new CRegisterData(0xF3, "OAMX", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMXBitfields),

   // Sprite 61
   new CRegisterData(0xF4, "OAMY", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMYBitfields),
   new CRegisterData(0xF5, "OAMPAT", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMPATBitfields),
   new CRegisterData(0xF6, "OAMATT", nesGetPPUOAM, nesSetPPUOAM, 4, tblOAMATTBitfields),
   new CRegisterData(0xF7, "OAMX", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMXBitfields),

   // Sprite 62
   new CRegisterData(0xF8, "OAMY", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMYBitfields),
   new CRegisterData(0xF9, "OAMPAT", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMPATBitfields),
   new CRegisterData(0xFA, "OAMATT", nesGetPPUOAM, nesSetPPUOAM, 4, tblOAMATTBitfields),
   new CRegisterData(0xFB, "OAMX", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMXBitfields),

   // Sprite 63
   new CRegisterData(0xFC, "OAMY", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMYBitfields),
   new CRegisterData(0xFD, "OAMPAT", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMPATBitfields),
   new CRegisterData(0xFE, "OAMATT", nesGetPPUOAM, nesSetPPUOAM, 4, tblOAMATTBitfields),
   new CRegisterData(0xFF, "OAMX", nesGetPPUOAM, nesSetPPUOAM, 1, tblOAMXBitfields),
};

static const char* oamRowHeadings [] =
{
   "0","1","2","3","4","5","6","7",
   "8","9","10","11","12","13","14","15",
   "16","17","18","19","20","21","22","23",
   "24","25","26","27","28","29","30","31",
   "32","33","34","35","36","37","38","39",
   "40","41","42","43","44","45","46","47",
   "48","49","50","51","52","53","54","55",
   "56","57","58","59","60","61","62","63"
};

static const char* oamColumnHeadings [] =
{
   "x0","x1","x2","x3"
};

static CRegisterDatabase* dbOamRegisters = new CRegisterDatabase(eMemory_PPUoam,64,4,256,tblOAMRegisters,oamRowHeadings,oamColumnHeadings);

CRegisterDatabase* CPPU::m_dbOamRegisters = dbOamRegisters;

static CMemoryDatabase* dbPaletteMemory = new CMemoryDatabase(eMemory_PPUpalette,
                                                              0x3F00,
                                                              MEM_32B,
                                                              4,
                                                              "Palette",
                                                              nesGetPPUMemory,
                                                              nesSetPPUMemory,
                                                              nesGetPrintableAddress,
                                                              false,
                                                              nesGetPaletteRedComponent,
                                                              nesGetPaletteGreenComponent,
                                                              nesGetPaletteBlueComponent);

CMemoryDatabase* CPPU::m_dbPaletteMemory = dbPaletteMemory;

static CMemoryDatabase* dbNameTableMemory = new CMemoryDatabase(eMemory_PPU,
                                                                0x2000,
                                                                MEM_2KB,
                                                                32,
                                                                "NameTable",
                                                                nesGetPPUMemory,
                                                                nesSetPPUMemory,
                                                                nesGetPrintableAddress,
                                                                true);

CMemoryDatabase* CPPU::m_dbNameTableMemory = dbNameTableMemory;

// PPU Event breakpoints
bool ppuAlwaysFireEvent(BreakpointInfo* pBreakpoint,int data)
{
   // This breakpoint is checked in the right place for each scanline
   // so if this breakpoint is enabled it should always fire when called.
   return true;
}

bool ppuRasterPositionEvent(BreakpointInfo* pBreakpoint,int data)
{
   if ( (pBreakpoint->item1 == CPPU::_X()) &&
         (pBreakpoint->item2 == CPPU::_Y()) )
   {
      return true;
   }

   return false;
}

bool ppuSpriteDmaEvent(BreakpointInfo* pBreakpoint,int data)
{
   if ( pBreakpoint->item1 == C6502::WRITEDMAADDR() )
   {
      return true;
   }

   return false;
}

bool ppuSpriteInMultiplexerEvent(BreakpointInfo* pBreakpoint,int data)
{
   if ( (uint32_t)data == pBreakpoint->item1 )
   {
      return true;
   }

   return false;
}

bool ppuSpriteSelectedEvent(BreakpointInfo* pBreakpoint,int data)
{
   if ( (uint32_t)data == pBreakpoint->item1 )
   {
      return true;
   }

   return false;
}

bool ppuSpriteRenderingEvent(BreakpointInfo* pBreakpoint,int data)
{
   if ( (uint32_t)data == pBreakpoint->item1 )
   {
      return true;
   }

   return false;
}

bool ppuAddressEqualsEvent(BreakpointInfo* pBreakpoint,int data)
{
   if ( (uint32_t)data == pBreakpoint->item1 )
   {
      return true;
   }

   return false;
}

static CBreakpointEventInfo* tblPPUEvents [] =
{
   new CBreakpointEventInfo("NMI", ppuAlwaysFireEvent, 0, "Break if PPU asserts NMI", 10),
   new CBreakpointEventInfo("Raster Position", ppuRasterPositionEvent, 2, "Break at pixel (%d,%d)", 10, "X:", "Y:"),
   new CBreakpointEventInfo("PPU Address Equals", ppuAddressEqualsEvent, 1, "Break if PPU address is %04X", 16, "PPU Address:"),
   new CBreakpointEventInfo("Pre-render Scanline Start (X=0,Y=-1)", ppuAlwaysFireEvent, 0, "Break at start of pre-render scanline", 10),
   new CBreakpointEventInfo("Pre-render Scanline End (X=256,Y=-1)", ppuAlwaysFireEvent, 0, "Break at end of pre-render scanline", 10),
   new CBreakpointEventInfo("Scanline Start (X=0,Y=[0,239])", ppuAlwaysFireEvent, 0, "Break at start of scanline", 10),
   new CBreakpointEventInfo("Scanline End (X=256,Y=[0,239])", ppuAlwaysFireEvent, 0, "Break at end of scanline", 10),
   new CBreakpointEventInfo("X Scroll Updated", ppuAlwaysFireEvent, 0, "Break if PPU's X scroll is updated", 10),
   new CBreakpointEventInfo("Y Scroll Updated", ppuAlwaysFireEvent, 0, "Break if PPU's Y scroll is updated", 10),
   new CBreakpointEventInfo("Sprite DMA", ppuSpriteDmaEvent, 1, "Break on sprite DMA at byte %d", 10, "Addr:"),
   new CBreakpointEventInfo("Sprite 0 Hit", ppuAlwaysFireEvent, 0, "Break on sprite 0 hit", 10),
   new CBreakpointEventInfo("Sprite enters multiplexer", ppuSpriteInMultiplexerEvent, 1, "Break if sprite %d enters multiplexer", 10, "Sprite:"),
   new CBreakpointEventInfo("Sprite selected by multiplexer", ppuSpriteSelectedEvent, 1, "Break if sprite %d is selected by multiplexer", 10, "Sprite:"),
   new CBreakpointEventInfo("Sprite rendering", ppuSpriteRenderingEvent, 1, "Break if rendering sprite %d on scanline", 10, "Sprite:"),
   new CBreakpointEventInfo("Sprite overflow", ppuAlwaysFireEvent, 0, "Break on sprite-per-scanline overflow", 10)
};

CBreakpointEventInfo** CPPU::m_tblBreakpointEvents = tblPPUEvents;
int32_t                CPPU::m_numBreakpointEvents = NUM_PPU_EVENTS;

uint8_t* CPPU::m_PPUmemory = NULL;
uint8_t  CPPU::m_PALETTEmemory [] = { 0, };
uint8_t* CPPU::m_pPPUmemory [] = { NULL, };
uint8_t  CPPU::m_oamAddr = 0x00;
int32_t  CPPU::m_ppuRegByte = 0;
uint16_t CPPU::m_ppuAddr = 0x0000;
uint8_t  CPPU::m_ppuAddrIncrement = 1;
uint16_t CPPU::m_ppuAddrLatch = 0x0000;
uint8_t  CPPU::m_ppuReadLatch = 0x00;
uint8_t  CPPU::m_ppuIOLatch = 0x00;
uint8_t  CPPU::m_ppuIOLatchDecayFrames [] = { 0, };
uint8_t  CPPU::m_PPUreg [] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
uint8_t  CPPU::m_PPUoam [] = { 0x00, };
uint8_t  CPPU::m_ppuScrollX = 0x00;
int32_t        CPPU::m_oneScreen = -1;
bool           CPPU::m_extraVRAM = false;

uint32_t   CPPU::m_cycles = 0;

uint32_t CPPU::startVblank;
uint32_t CPPU::quietScanlines;
uint32_t CPPU::vblankScanlines;
uint32_t CPPU::vblankEndCycle;
uint32_t CPPU::prerenderScanline;
uint32_t CPPU::cycleRatio;
uint32_t CPPU::memoryDecayFrames;

bool           CPPU::m_vblankChoked = false;
bool           CPPU::m_nmiChoked = false;
bool           CPPU::m_nmiReenabled = false;

int8_t*          CPPU::m_pTV = NULL;

uint32_t       CPPU::m_frame = 0;
int32_t         CPPU::m_curCycles = 0;

SpriteTemporaryMemory CPPU::m_spriteTemporaryMemory;
SpriteBuffer          CPPU::m_spriteBuffer;

BackgroundBuffer CPPU::m_bkgndBuffer;

CCodeDataLogger* CPPU::m_logger = NULL;

uint8_t  CPPU::m_last2005x = 0;
uint8_t  CPPU::m_last2005y = 0;
uint16_t** CPPU::m_2005x = NULL;
uint16_t** CPPU::m_2005y = NULL;
uint8_t  CPPU::m_lastSprite0HitX = 0;
uint8_t  CPPU::m_lastSprite0HitY = 0;
uint8_t  CPPU::m_x = 0xFF;
uint8_t  CPPU::m_y = 0xFF;

static CPPU __init __attribute__((unused));

CPPU::CPPU()
{
   int idx;

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

   m_PPUmemory = new uint8_t[MEM_4KB];

   // Set up default mapping.
   for ( idx = 0; idx < 8; idx++ )
   {
      m_pPPUmemory[idx] = m_PPUmemory+((idx&1)<<UPSHIFT_1KB);
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

   delete [] m_PPUmemory;
}

void CPPU::EMULATE(uint32_t cycles)
{
   uint32_t idxx = 0xffffffff;
   uint32_t idxy = 0xffffffff;

   for ( ; cycles > 0; cycles-- )
   {
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
         if ( ((m_cycles%PPU_CYCLES_PER_SCANLINE) == 257) &&
               ((m_cycles/PPU_CYCLES_PER_SCANLINE) < SCANLINES_VISIBLE) )
         {
            m_ppuAddr &= 0xFBE0;
            m_ppuAddr |= m_ppuAddrLatch&0x41F;
         }
         else if ( ((m_cycles%PPU_CYCLES_PER_SCANLINE) == 304) &&
                   ((m_cycles/PPU_CYCLES_PER_SCANLINE) == prerenderScanline) )
         {
            m_ppuAddr = m_ppuAddrLatch;
         }
         else if ( (m_cycles/PPU_CYCLES_PER_SCANLINE) < SCANLINES_VISIBLE )
         {
            if ( (m_cycles%PPU_CYCLES_PER_SCANLINE) == 251 )
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

            if ( (((m_cycles%PPU_CYCLES_PER_SCANLINE)%8) == 3) &&
                  (((m_cycles%PPU_CYCLES_PER_SCANLINE) < 256) ||
                   ((m_cycles%PPU_CYCLES_PER_SCANLINE) == 323) ||
                   ((m_cycles%PPU_CYCLES_PER_SCANLINE) == 331)) )
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
      }

      // Run 0 or 1 CPU cycles...
      C6502::EMULATE ( m_curCycles/cycleRatio );

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

      if ( nesIsDebuggable() )
      {
         // Check for breakpoints...
         CNES::CHECKBREAKPOINT ( eBreakInPPU, eBreakOnPPUCycle );
      }

      if ( (rPPU(PPUCTRL)&PPUCTRL_GENERATE_NMI) &&
            (((!NMICHOKED()) && (idxy == 0) && (idxx == 1)) ||
             ((NMIREENABLED()) && (idxy <= vblankScanlines-1) && (idxx < PPU_CYCLES_PER_SCANLINE-1))) )
      {
         C6502::ASSERTNMI ();

         // Check for PPU NMI breakpoint...
         CNES::CHECKBREAKPOINT ( eBreakInPPU, eBreakOnPPUEvent, 0, PPU_EVENT_NMI );
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
      data = CROM::CHRMEM ( addr );

      // Add Tracer sample...
      if ( nesIsDebuggable() && trace )
      {
         CNES::TRACER()->AddSample ( m_cycles, type, source, eTarget_PatternMemory, addr, data );
      }

      return data;
   }

   if ( addr >= 0x3F00 )
   {
      data = *(m_PALETTEmemory+(addr&0x1F));

      // Add Tracer sample...
      if ( nesIsDebuggable() && trace )
      {
         CNES::TRACER()->AddSample ( m_cycles, type, source, eTarget_Palette, addr, data );
      }

      return data;
   }

   addr &= 0x1FFF;

   if ( nesMapperRemapsVMEM() )
   {
      data = CROM::VRAM(addr);
   }
   else
   {
      data = *((*(m_pPPUmemory+((addr&0x1FFF)>>10)))+(addr&0x3FF));
   }

   // Add Tracer sample...
   if ( nesIsDebuggable() && trace )
   {
      if ( (addr&0x3FF) < 0x3C0 )
      {
         CNES::TRACER()->AddSample ( m_cycles, type, source, eTarget_NameTable, addr, data );
      }
      else
      {
         CNES::TRACER()->AddSample ( m_cycles, type, source, eTarget_AttributeTable, addr, data );
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
      if ( nesIsDebuggable() && trace )
      {
         CNES::TRACER()->AddSample ( m_cycles, type, source, eTarget_PatternMemory, addr, data );
      }

      if ( CROM::IsWriteProtected() == false )
      {
         CROM::CHRMEM ( addr, data );
      }

      return;
   }

   if ( addr >= 0x3F00 )
   {
      // Add Tracer sample...
      if ( nesIsDebuggable() && trace )
      {
         CNES::TRACER()->AddSample ( m_cycles, type, source, eTarget_Palette, addr, data );
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

   addr &= 0x1FFF;

   // Add Tracer sample...
   if ( nesIsDebuggable() && trace )
   {
      if ( (addr&0x3FF) < 0x3C0 )
      {
         CNES::TRACER()->AddSample ( m_cycles, type, source, eTarget_NameTable, addr, data );
      }
      else
      {
         CNES::TRACER()->AddSample ( m_cycles, type, source, eTarget_AttributeTable, addr, data );
      }
   }

   if ( nesMapperRemapsVMEM() )
   {
      CROM::VRAM(addr,data);
   }
   else
   {
      *((*(m_pPPUmemory+((addr&0x1FFF)>>10)))+(addr&0x3FF)) = data;
   }
}

uint32_t CPPU::RENDER ( uint32_t addr, int8_t target )
{
   uint32_t data;

   data = LOAD ( addr, eNESSource_PPU, target );

   if ( nesIsDebuggable() )
   {
      m_logger->LogAccess ( C6502::_CYCLES()/*m_cycles*/, addr, data, eLogger_DataRead, eNESSource_PPU );
   }

   // Provide PPU cycle and address to mappers that watch such things!
   MAPPERFUNC->sync_ppu(m_cycles,addr);

   // Address/Data bus multiplexed thus 2 cycles required per access...
   EMULATE(1);

   if ( nesIsDebuggable() )
   {
      // Check for PPU address breakpoint...
      CNES::CHECKBREAKPOINT ( eBreakInPPU, eBreakOnPPUEvent, rPPUADDR(), PPU_EVENT_ADDRESS_EQUALS );

      // Check for breakpoint...
      CNES::CHECKBREAKPOINT ( eBreakInPPU, eBreakOnPPUFetch, data );
   }

   return data;
}

void CPPU::GARBAGE ( uint32_t addr, int8_t target )
{
   if ( nesIsDebuggable() )
   {
      CNES::TRACER()->AddGarbageFetch ( m_cycles, target, addr );
   }

   // Provide PPU cycle and address to mappers that watch such things!
   MAPPERFUNC->sync_ppu(m_cycles,addr);

   // Address/Data bus multiplexed thus 2 cycles required per access...
   EMULATE(1);

   if ( nesIsDebuggable() )
   {
      // Check for PPU address breakpoint...
      CNES::CHECKBREAKPOINT ( eBreakInPPU, eBreakOnPPUEvent, rPPUADDR(), PPU_EVENT_ADDRESS_EQUALS );

      // Check for breakpoint...
      CNES::CHECKBREAKPOINT ( eBreakInPPU, eBreakOnPPUFetch );
   }
}

void CPPU::EXTRA ()
{
   if ( nesIsDebuggable() )
   {
      CNES::TRACER()->AddGarbageFetch ( m_cycles, eTarget_ExtraCycle, 0 );
   }

   // Idle cycle...
   EMULATE(1);

   if ( nesIsDebuggable() )
   {
      // Check for PPU address breakpoint...
      CNES::CHECKBREAKPOINT ( eBreakInPPU, eBreakOnPPUEvent, rPPUADDR(), PPU_EVENT_ADDRESS_EQUALS );

      // Check for breakpoint...
      CNES::CHECKBREAKPOINT ( eBreakInPPU, eBreakOnPPUFetch );
   }
}

void CPPU::RESET ( bool soft )
{
   int idx;

   startVblank = (CNES::VIDEOMODE()==MODE_NTSC)?PPU_CYCLE_START_VBLANK_NTSC:(CNES::VIDEOMODE()==MODE_PAL)?PPU_CYCLE_START_VBLANK_PAL:PPU_CYCLE_START_VBLANK_DENDY;
   quietScanlines = (CNES::VIDEOMODE()==MODE_NTSC)?SCANLINES_QUIET_NTSC:(CNES::VIDEOMODE()==MODE_PAL)?SCANLINES_QUIET_PAL:SCANLINES_QUIET_DENDY;
   vblankScanlines = (CNES::VIDEOMODE()==MODE_NTSC)?SCANLINES_VBLANK_NTSC:(CNES::VIDEOMODE()==MODE_PAL)?SCANLINES_VBLANK_PAL:SCANLINES_VBLANK_DENDY;
   vblankEndCycle = (CNES::VIDEOMODE()==MODE_NTSC)?PPU_CYCLE_END_VBLANK_NTSC:(CNES::VIDEOMODE()==MODE_PAL)?PPU_CYCLE_END_VBLANK_PAL:PPU_CYCLE_END_VBLANK_DENDY;
   prerenderScanline = (CNES::VIDEOMODE()==MODE_NTSC)?SCANLINE_PRERENDER_NTSC:(CNES::VIDEOMODE()==MODE_PAL)?SCANLINE_PRERENDER_PAL:SCANLINE_PRERENDER_DENDY;
   cycleRatio = (CNES::VIDEOMODE()==MODE_NTSC)?PPU_CPU_RATIO_NTSC:(CNES::VIDEOMODE()==MODE_PAL)?PPU_CPU_RATIO_PAL:PPU_CPU_RATIO_DENDY;
   memoryDecayFrames = (CNES::VIDEOMODE()==MODE_NTSC)?PPU_DECAY_FRAME_COUNT_NTSC:(CNES::VIDEOMODE()==MODE_PAL)?PPU_DECAY_FRAME_COUNT_PAL:PPU_DECAY_FRAME_COUNT_DENDY;

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

   // Set up default mapping.
   for ( idx = 0; idx < 8; idx++ )
   {
      m_pPPUmemory[idx] = m_PPUmemory+((idx&1)<<UPSHIFT_1KB);
   }

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

      if ( nesIsDebuggable() )
      {
         // Check for breakpoint...
         CNES::CHECKBREAKPOINT ( eBreakInPPU, eBreakOnOAMPortalRead, data );
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
         m_ppuReadLatch = *((*(m_pPPUmemory+(((oldPpuAddr&(~0x1000))&0x1FFF)>>10)))+((oldPpuAddr&(~0x1000))&0x3FF));
      }

      if ( nesIsDebuggable() )
      {
         // Check for breakpoint...
         CNES::CHECKBREAKPOINT ( eBreakInPPU, eBreakOnPPUPortalRead, data );

         // Log Code/Data logger...
         m_logger->LogAccess ( C6502::_CYCLES()/*m_cycles*/, oldPpuAddr, data, eLogger_DataRead, eNESSource_CPU );
      }

      // Toggling A12 causes IRQ count in some mappers...
      MAPPERFUNC->sync_ppu(m_cycles,m_ppuAddr);
   }
   else
   {
      data = m_ppuIOLatch;
   }

   if ( nesIsDebuggable() )
   {
      // Check for breakpoint...
      CNES::CHECKBREAKPOINT ( eBreakInPPU, eBreakOnPPUState, fixAddr );
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

      if ( nesIsDebuggable() )
      {
         // Check for breakpoint...
         CNES::CHECKBREAKPOINT ( eBreakInPPU, eBreakOnOAMPortalWrite, data );
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

         if ( nesIsDebuggable() )
         {
            // Check for PPU Y scroll update breakpoint...
            CNES::CHECKBREAKPOINT ( eBreakInPPU, eBreakOnPPUEvent, 0, PPU_EVENT_YSCROLL_UPDATE );
         }
      }
      else
      {
         m_last2005x = data;
         m_ppuScrollX = data&0x07;
         m_ppuAddrLatch &= 0xFFE0;
         m_ppuAddrLatch |= ((((uint16_t)data&0xF8))>>3);

         if ( nesIsDebuggable() )
         {
            // Check for PPU X scroll update breakpoint...
            CNES::CHECKBREAKPOINT ( eBreakInPPU, eBreakOnPPUEvent, 0, PPU_EVENT_XSCROLL_UPDATE );
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
         MAPPERFUNC->sync_ppu(m_cycles,m_ppuAddr);
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

      if ( nesIsDebuggable() )
      {
         m_logger->LogAccess ( C6502::_CYCLES()/*m_cycles*/, oldPpuAddr, data, eLogger_DataWrite, eNESSource_CPU );

         // Check for breakpoint...
         CNES::CHECKBREAKPOINT ( eBreakInPPU, eBreakOnPPUPortalWrite, data );
      }

      // Increment PPUADDR
      m_ppuAddr += m_ppuAddrIncrement;

      // Toggling A12 causes IRQ count in some mappers...
      MAPPERFUNC->sync_ppu(m_cycles,m_ppuAddr);
   }

   if ( nesIsDebuggable() )
   {
      // Check for breakpoint...
      CNES::CHECKBREAKPOINT ( eBreakInPPU, eBreakOnPPUState, fixAddr );
   }
}

void CPPU::MIRROR ( int32_t oneScreen, bool vert, bool extraVRAM )
{
   m_oneScreen = oneScreen;
   m_extraVRAM = extraVRAM;

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
   if ( m_extraVRAM )
   {
      CROM::REMAPVRAM ( 0x0, 0x0 );
      CROM::REMAPVRAM ( 0x1, 0x1 );
      CROM::REMAPVRAM ( 0x2, 0x2 );
      CROM::REMAPVRAM ( 0x3, 0x3 );
      CROM::REMAPVRAM ( 0x4, 0x4 );
      CROM::REMAPVRAM ( 0x5, 0x5 );
      CROM::REMAPVRAM ( 0x6, 0x6 );
      CROM::REMAPVRAM ( 0x7, 0x7 );
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
      REMAPVRAM ( 0x0, &(m_PPUmemory[(b0<<UPSHIFT_1KB)]) );
      REMAPVRAM ( 0x4, &(m_PPUmemory[(b0<<UPSHIFT_1KB)]) );
   }
   if ( b1 >= 0 )
   {
      b1 &= 0x3;
      REMAPVRAM ( 0x1, &(m_PPUmemory[(b1<<UPSHIFT_1KB)]) );
      REMAPVRAM ( 0x5, &(m_PPUmemory[(b1<<UPSHIFT_1KB)]) );
   }
   if ( b2 >= 0 )
   {
      b2 &= 0x3;
      REMAPVRAM ( 0x2, &(m_PPUmemory[(b2<<UPSHIFT_1KB)]) );
      REMAPVRAM ( 0x6, &(m_PPUmemory[(b2<<UPSHIFT_1KB)]) );
   }
   if ( b3 >= 0 )
   {
      b3 &= 0x3;
      REMAPVRAM ( 0x3, &(m_PPUmemory[(b3<<UPSHIFT_1KB)]) );
      REMAPVRAM ( 0x7, &(m_PPUmemory[(b3<<UPSHIFT_1KB)]) );
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

      if ( nesIsDebuggable() )
      {
         // Check for start-of-scanline breakpoints...
         if ( scanline == -1 )
         {
            CNES::CHECKBREAKPOINT(eBreakInPPU,eBreakOnPPUEvent,0,PPU_EVENT_PRE_RENDER_SCANLINE_START);
         }
         else
         {
            CNES::CHECKBREAKPOINT(eBreakInPPU,eBreakOnPPUEvent,0,PPU_EVENT_SCANLINE_START);
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
            if ( nesIsDebuggable() )
            {
               m_x = idxx;

               // Update variables for PPU viewer
               *(*(m_2005x+m_x)+m_y) = m_last2005x+((rPPU(PPUCTRL)&0x1)<<8);
               *(*(m_2005y+m_x)+m_y) = m_last2005y+(((rPPU(PPUCTRL)&0x2)>>1)*240);

               // Check for PPU pixel-at breakpoint...
               CNES::CHECKBREAKPOINT(eBreakInPPU,eBreakOnPPUEvent,0,PPU_EVENT_PIXEL_XY);
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
                  if ( nesIsDebuggable() )
                  {
                     // Check for sprite-in-multiplexer event breakpoint...
                     CNES::CHECKBREAKPOINT(eBreakInPPU,eBreakOnPPUEvent,pSpriteTemp->spriteIdx,PPU_EVENT_SPRITE_IN_MULTIPLEXER);
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
                     if ( nesIsDebuggable() )
                     {
                        // Check for sprite selected event breakpoint...
                        CNES::CHECKBREAKPOINT(eBreakInPPU,eBreakOnPPUEvent,pSpriteTemp->spriteIdx,PPU_EVENT_SPRITE_SELECTED);
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

            // Sprite/background pixel rendering determination...
            if ( rPPU(PPUMASK)&(PPUMASK_RENDER_BKGND|PPUMASK_RENDER_SPRITES) )
            {
               if ( (pSelectedSprite) &&
                     ((!(pSelectedSprite->spriteBehind)) ||
                      ((bkgndColorIdx == 0) &&
                       (spriteColorIdx != 0))) )
               {
                  if ( nesIsDebuggable() )
                  {
                     // Check for sprite rendering event breakpoint...
                     CNES::CHECKBREAKPOINT(eBreakInPPU,eBreakOnPPUEvent,pSelectedSpriteTemp->spriteIdx,PPU_EVENT_SPRITE_RENDERING);
                  }

                  // Draw sprite...
                  *pTV = CBasePalette::GetPaletteR(rPALETTE(0x10+spriteColorIdx), !!(rPPU(PPUMASK)&PPUMASK_GREYSCALE), !!(rPPU(PPUMASK)&PPUMASK_INTENSIFY_REDS), !!(rPPU(PPUMASK)&PPUMASK_INTENSIFY_GREENS), !!(rPPU(PPUMASK)&PPUMASK_INTENSIFY_BLUES));
                  *(pTV+1) = CBasePalette::GetPaletteG(rPALETTE(0x10+spriteColorIdx), !!(rPPU(PPUMASK)&PPUMASK_GREYSCALE), !!(rPPU(PPUMASK)&PPUMASK_INTENSIFY_REDS), !!(rPPU(PPUMASK)&PPUMASK_INTENSIFY_GREENS), !!(rPPU(PPUMASK)&PPUMASK_INTENSIFY_BLUES));
                  *(pTV+2) = CBasePalette::GetPaletteB(rPALETTE(0x10+spriteColorIdx), !!(rPPU(PPUMASK)&PPUMASK_GREYSCALE), !!(rPPU(PPUMASK)&PPUMASK_INTENSIFY_REDS), !!(rPPU(PPUMASK)&PPUMASK_INTENSIFY_GREENS), !!(rPPU(PPUMASK)&PPUMASK_INTENSIFY_BLUES));
               }
               else if ( p>=startBkgnd )
               {
                  // Draw background...
                  *pTV = CBasePalette::GetPaletteR(rPALETTE(bkgndColorIdx), !!(rPPU(PPUMASK)&PPUMASK_GREYSCALE), !!(rPPU(PPUMASK)&PPUMASK_INTENSIFY_REDS), !!(rPPU(PPUMASK)&PPUMASK_INTENSIFY_GREENS), !!(rPPU(PPUMASK)&PPUMASK_INTENSIFY_BLUES));
                  *(pTV+1) = CBasePalette::GetPaletteG(rPALETTE(bkgndColorIdx), !!(rPPU(PPUMASK)&PPUMASK_GREYSCALE), !!(rPPU(PPUMASK)&PPUMASK_INTENSIFY_REDS), !!(rPPU(PPUMASK)&PPUMASK_INTENSIFY_GREENS), !!(rPPU(PPUMASK)&PPUMASK_INTENSIFY_BLUES));
                  *(pTV+2) = CBasePalette::GetPaletteB(rPALETTE(bkgndColorIdx), !!(rPPU(PPUMASK)&PPUMASK_GREYSCALE), !!(rPPU(PPUMASK)&PPUMASK_INTENSIFY_REDS), !!(rPPU(PPUMASK)&PPUMASK_INTENSIFY_GREENS), !!(rPPU(PPUMASK)&PPUMASK_INTENSIFY_BLUES));
               }
               else
               {
                  // Draw 'nothing'...
                  *pTV = CBasePalette::GetPaletteR(rPALETTE(0), !!(rPPU(PPUMASK)&PPUMASK_GREYSCALE), !!(rPPU(PPUMASK)&PPUMASK_INTENSIFY_REDS), !!(rPPU(PPUMASK)&PPUMASK_INTENSIFY_GREENS), !!(rPPU(PPUMASK)&PPUMASK_INTENSIFY_BLUES));
                  *(pTV+1) = CBasePalette::GetPaletteG(rPALETTE(0), !!(rPPU(PPUMASK)&PPUMASK_GREYSCALE), !!(rPPU(PPUMASK)&PPUMASK_INTENSIFY_REDS), !!(rPPU(PPUMASK)&PPUMASK_INTENSIFY_GREENS), !!(rPPU(PPUMASK)&PPUMASK_INTENSIFY_BLUES));
                  *(pTV+2) = CBasePalette::GetPaletteB(rPALETTE(0), !!(rPPU(PPUMASK)&PPUMASK_GREYSCALE), !!(rPPU(PPUMASK)&PPUMASK_INTENSIFY_REDS), !!(rPPU(PPUMASK)&PPUMASK_INTENSIFY_GREENS), !!(rPPU(PPUMASK)&PPUMASK_INTENSIFY_BLUES));
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

                     if ( nesIsDebuggable() )
                     {
                        // Save last sprite 0 hit coords for OAM viewer...
                        m_lastSprite0HitX = p;
                        m_lastSprite0HitY = scanline;

                        // Add trace tag for Sprice 0 hit...
                        CNES::TRACER()->AddSample ( m_cycles, eTracer_Sprite0Hit, eNESSource_PPU, 0, 0, 0 );

                        // Check for Sprite 0 Hit breakpoint...
                        CNES::CHECKBREAKPOINT(eBreakInPPU,eBreakOnPPUEvent,0,PPU_EVENT_SPRITE0_HIT);
                     }
                  }
               }
            }
            else
            {
               if ( (m_ppuAddr&0x3F00) == 0x3F00 )
               {
                  *pTV = CBasePalette::GetPaletteR(rPALETTE(m_ppuAddr&0x1F), !!(rPPU(PPUMASK)&PPUMASK_GREYSCALE), !!(rPPU(PPUMASK)&PPUMASK_INTENSIFY_REDS), !!(rPPU(PPUMASK)&PPUMASK_INTENSIFY_GREENS), !!(rPPU(PPUMASK)&PPUMASK_INTENSIFY_BLUES));
                  *(pTV+1) = CBasePalette::GetPaletteG(rPALETTE(m_ppuAddr&0x1F), !!(rPPU(PPUMASK)&PPUMASK_GREYSCALE), !!(rPPU(PPUMASK)&PPUMASK_INTENSIFY_REDS), !!(rPPU(PPUMASK)&PPUMASK_INTENSIFY_GREENS), !!(rPPU(PPUMASK)&PPUMASK_INTENSIFY_BLUES));
                  *(pTV+2) = CBasePalette::GetPaletteB(rPALETTE(m_ppuAddr&0x1F), !!(rPPU(PPUMASK)&PPUMASK_GREYSCALE), !!(rPPU(PPUMASK)&PPUMASK_INTENSIFY_REDS), !!(rPPU(PPUMASK)&PPUMASK_INTENSIFY_GREENS), !!(rPPU(PPUMASK)&PPUMASK_INTENSIFY_BLUES));
               }
               else
               {
                  *pTV = CBasePalette::GetPaletteR(rPALETTE(0), !!(rPPU(PPUMASK)&PPUMASK_GREYSCALE), !!(rPPU(PPUMASK)&PPUMASK_INTENSIFY_REDS), !!(rPPU(PPUMASK)&PPUMASK_INTENSIFY_GREENS), !!(rPPU(PPUMASK)&PPUMASK_INTENSIFY_BLUES));
                  *(pTV+1) = CBasePalette::GetPaletteG(rPALETTE(0), !!(rPPU(PPUMASK)&PPUMASK_GREYSCALE), !!(rPPU(PPUMASK)&PPUMASK_INTENSIFY_REDS), !!(rPPU(PPUMASK)&PPUMASK_INTENSIFY_GREENS), !!(rPPU(PPUMASK)&PPUMASK_INTENSIFY_BLUES));
                  *(pTV+2) = CBasePalette::GetPaletteB(rPALETTE(0), !!(rPPU(PPUMASK)&PPUMASK_GREYSCALE), !!(rPPU(PPUMASK)&PPUMASK_INTENSIFY_REDS), !!(rPPU(PPUMASK)&PPUMASK_INTENSIFY_GREENS), !!(rPPU(PPUMASK)&PPUMASK_INTENSIFY_BLUES));
               }
            }

            // Move to next pixel...
            pTV += 4;
            p++;
         }

         // Secondary OAM reads occur on even PPU cycles...
         if ( !(idxx&1) )
         {
            BUILDSPRITELIST ( scanline, idxx );
         }
         GATHERBKGND ( idxx%8 );
      }

      if ( nesIsDebuggable() )
      {
         // Check for end-of-scanline breakpoints...
         if ( scanline == -1 )
         {
            CNES::CHECKBREAKPOINT(eBreakInPPU,eBreakOnPPUEvent,0,PPU_EVENT_PRE_RENDER_SCANLINE_END);
         }
         else
         {
            CNES::CHECKBREAKPOINT(eBreakInPPU,eBreakOnPPUEvent,0,PPU_EVENT_SCANLINE_END);
         }
      }

      GATHERSPRITES ( scanline );

      // Fill pipeline for next scanline...
      m_bkgndBuffer.data[0].attribData1 = m_bkgndBuffer.data[1].attribData1;
      m_bkgndBuffer.data[0].attribData2 = m_bkgndBuffer.data[1].attribData2;
      m_bkgndBuffer.data[0].patternData1 = m_bkgndBuffer.data[1].patternData1;
      m_bkgndBuffer.data[0].patternData2 = m_bkgndBuffer.data[1].patternData2;

      for ( p = 0; p < 8; p++ )
      {
         GATHERBKGND ( p );
      }

      // Fill pipeline for next scanline...
      m_bkgndBuffer.data[0].attribData1 = m_bkgndBuffer.data[1].attribData1;
      m_bkgndBuffer.data[0].attribData2 = m_bkgndBuffer.data[1].attribData2;
      m_bkgndBuffer.data[0].patternData1 = m_bkgndBuffer.data[1].patternData1;
      m_bkgndBuffer.data[0].patternData2 = m_bkgndBuffer.data[1].patternData2;

      for ( p = 0; p < 8; p++ )
      {
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
         if ( (CNES::VIDEOMODE() == MODE_DENDY) || (CNES::VIDEOMODE() == MODE_PAL) || ((CNES::VIDEOMODE() == MODE_NTSC) && ((!(m_frame&1)) || (!(rPPU(PPUMASK)&(PPUMASK_RENDER_BKGND))))) )
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

   if ( !(phase&1) )
   {
      EMULATE(1);
      return;
   }

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

   // Loop invariants...
   spriteSize = ((!!(rPPU(PPUCTRL)&PPUCTRL_SPRITE_SIZE))+1)<<3;

   scanline++;

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

                     if ( nesIsDebuggable() )
                     {
                        // Check for breakpoint...
                        CNES::CHECKBREAKPOINT ( eBreakInPPU, eBreakOnPPUEvent, 0, PPU_EVENT_SPRITE_OVERFLOW );
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
