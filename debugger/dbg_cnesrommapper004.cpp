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

#include "dbg_cnesrommapper004.h"

#include "cregisterdata.h"

// Mapper 004 Registers
static CBitfieldData* tbl8000Bitfields [] =
{
   new CBitfieldData("CHR Mode", 7, 1, "%X", 2, "2KBx2,1KBx4", "1KBx4,2KBx2"),
   new CBitfieldData("PRG Mode", 6, 1, "%X", 2, "$8000 and $A000 swappable, $C000 and $E000 fixed to last bank", "$8000 and $E000 swappable, $A000 and $C000 fixed to last bank"),
   new CBitfieldData("Address", 0, 3, "%X", 8, "CHR Register 0", "CHR Register 1", "CHR Register 2", "CHR Register 3", "CHR Register 4", "CHR Register 5", "PRG Register 0", "PRG Register 1")
};

static CBitfieldData* tbl8001Bitfields [] =
{
   new CBitfieldData("Data", 0, 8, "%02X", 0)
};

static CBitfieldData* tblA000Bitfields [] =
{
   new CBitfieldData("Mirroring", 0, 1, "%X", 2, "Vertical", "Horizontal")
};

static CBitfieldData* tblA001Bitfields [] =
{
   new CBitfieldData("WRAM", 7, 1, "%X", 2, "Disabled", "Enabled"),
   new CBitfieldData("WRAM Write-protect", 6, 1, "%X", 2, "Off", "On")
};

static CBitfieldData* tblC000Bitfields [] =
{
   new CBitfieldData("IRQ Reload", 0, 8, "%02X", 0)
};

static CBitfieldData* tblC001Bitfields [] =
{
   new CBitfieldData("IRQ Clear", 0, 8, "%02X", 0)
};

static CBitfieldData* tblE000Bitfields [] =
{
   new CBitfieldData("IRQ Ack/Disable", 0, 8, "%02X", 0)
};

static CBitfieldData* tblE001Bitfields [] =
{
   new CBitfieldData("IRQ Enable", 0, 8, "%02X", 0)
};

static CRegisterData* tblRegisters [] =
{
   new CRegisterData(0x8000, "Control", 3, tbl8000Bitfields),
   new CRegisterData(0x8001, "Data", 1, tbl8001Bitfields),
   new CRegisterData(0xA000, "Mirroring", 1, tblA000Bitfields),
   new CRegisterData(0xA001, "WRAM", 2, tblA001Bitfields),
   new CRegisterData(0xC000, "IRQ Reload", 1, tblC000Bitfields),
   new CRegisterData(0xC001, "IRQ Clear", 1, tblC001Bitfields),
   new CRegisterData(0xE000, "IRQ Ack/Disable", 1, tblE000Bitfields),
   new CRegisterData(0xE001, "IRQ Enable", 1, tblE001Bitfields),
};

CROMMapper004DBG::CROMMapper004DBG()
{

}

CROMMapper004DBG::~CROMMapper004DBG()
{

}
