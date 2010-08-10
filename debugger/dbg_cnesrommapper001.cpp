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

#include "dbg_cnesrommapper001.h"

#include "cregisterdata.h"

// Mapper 001 Registers
static CBitfieldData* tbl8000_9FFFBitfields [] =
{
   new CBitfieldData("CHR Mode", 4, 1, "%X", 2, "8KB mapping", "4KB mapping"),
   new CBitfieldData("PRG Size", 3, 1, "%X", 2, "32KB", "16KB"),
   new CBitfieldData("Slot Select", 2, 1, "%X", 2, "$C000 swappable, $8000 fixed to page $00", "$8000 swappable, $C000 fixed to page $0F"),
   new CBitfieldData("Mirroring", 0, 2, "%X", 4, "One-screen $2000", "One-screen $2400", "Vertical", "Horizontal")
};

static CBitfieldData* tblA000_BFFFBitfields [] =
{
   new CBitfieldData("CHR Bank 0", 0, 5, "%02X", 0)
};

static CBitfieldData* tblC000_DFFFBitfields [] =
{
   new CBitfieldData("CHR Bank 1", 0, 5, "%02X", 0)
};

static CBitfieldData* tblE000_FFFFBitfields [] =
{
   new CBitfieldData("WRAM State", 4, 1, "%X", 2, "Enabled", "Disabled"),
   new CBitfieldData("PRG Bank", 0, 4, "%X", 0)
};

static CRegisterData* tblRegisters [] =
{
   new CRegisterData(0x8000, "Control", 4, tbl8000_9FFFBitfields),
   new CRegisterData(0xA000, "CHR Mapping 1", 1, tblA000_BFFFBitfields),
   new CRegisterData(0xC000, "CHR Mapping 2", 1, tblC000_DFFFBitfields),
   new CRegisterData(0xE000, "PRG Mapping", 2, tblE000_FFFFBitfields)
};

CROMMapper001DBG::CROMMapper001DBG()
{

}

CROMMapper001DBG::~CROMMapper001DBG()
{

}
