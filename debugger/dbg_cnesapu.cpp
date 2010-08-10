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

#include "dbg_cnesapu.h"

#include "ctracer.h"

// APU Event breakpoints
bool apuIRQEvent(BreakpointInfo* pBreakpoint,int data)
{
   // This breakpoint is checked in the right place
   // so if this breakpoint is enabled it should always fire when called.
   return true;
}

bool apuLengthCounterClockedEvent(BreakpointInfo* pBreakpoint,int data)
{
   // This breakpoint is checked in the right place
   // so if this breakpoint is enabled it should always fire when called.
   return true;
}

bool apuDMAEvent(BreakpointInfo* pBreakpoint,int data)
{
   // This breakpoint is checked in the right place
   // so if this breakpoint is enabled it should always fire when called.
   return true;
}

static CBreakpointEventInfo* tblAPUEvents [] =
{
   new CBreakpointEventInfo("IRQ", apuIRQEvent, 0, "Break if APU asserts IRQ", 10),
   new CBreakpointEventInfo("DMC channel DMA", apuDMAEvent, 0, "Break if APU DMC channel DMA occurs", 10),
   new CBreakpointEventInfo("Length Counter Clocked", apuLengthCounterClockedEvent, 0, "Break if APU sequencer clocks Length Counter", 10),
};

// APU Registers
static CBitfieldData* tblAPUSQCTRLBitfields [] =
{
   new CBitfieldData("Duty Cycle", 6, 2, "%X", 4, "25%", "50%", "75%", "12.5%"),
   new CBitfieldData("Channel State", 5, 1, "%X", 2, "Running", "Halted"),
   new CBitfieldData("Envelope Enabled", 4, 1, "%X", 2, "No", "Yes"),
   new CBitfieldData("Volume/Envelope", 0, 4, "%X", 0)
};

static CBitfieldData* tblAPUSQSWEEPBitfields [] =
{
   new CBitfieldData("Sweep Enabled", 7, 1, "%X", 2, "No", "Yes"),
   new CBitfieldData("Sweep Divider", 4, 3, "%X", 0),
   new CBitfieldData("Sweep Direction", 3, 1, "%X", 2, "Down", "Up"),
   new CBitfieldData("Sweep Shift", 0, 3, "%X", 0)
};

static CBitfieldData* tblAPUPERIODLOBitfields [] =
{
   new CBitfieldData("Period Low-bits", 0, 8, "%02X", 0)
};

static CBitfieldData* tblAPUPERIODLENBitfields [] =
{
   new CBitfieldData("Length", 3, 5, "%X", 32, "$0A","$FE","$14","$02","$28","$04","$50","$06","$A0","$08","$3C","$0A","$0E","$0C","$1A","$0E","$0C","$10","$18","$12","$30","$14","$60","$16","$C0","$18","$48","$1A","$10","$1C","$20","$1E"),
   new CBitfieldData("Period High-bits", 0, 3, "%X", 0)
};

static CBitfieldData* tblAPUTRICTRLBitfields [] =
{
   new CBitfieldData("Channel State", 7, 1, "%X", 2, "Running", "Halted"),
   new CBitfieldData("Linear Counter Reload", 0, 6, "%X", 0)
};

static CBitfieldData* tblAPUDMZBitfields [] =
{
   new CBitfieldData("Unused", 0, 8, "%02X", 0),
};

static CBitfieldData* tblAPUNZCTRLBitfields [] =
{
   new CBitfieldData("Channel State", 5, 1, "%X", 2, "Running", "Halted"),
   new CBitfieldData("Envelope Enabled", 4, 1, "%X", 2, "No", "Yes"),
   new CBitfieldData("Volume/Envelope", 0, 4, "%X", 0)
};

static CBitfieldData* tblAPUNZPERIODBitfields [] =
{
   new CBitfieldData("Mode", 7, 1, "%X", 2, "32,767 samples", "93 samples"),
   new CBitfieldData("Period", 0, 4, "%X", 16, "$004","$008","$010","$020","$040","$060","$080","$0A0","$0CA","$0FE","$17C","$1FC","$2FA","$3F8","$7F2","$FE4")
};

static CBitfieldData* tblAPUNZLENBitfields [] =
{
   new CBitfieldData("Length", 3, 5, "%X", 32, "$0A","$FE","$14","$02","$28","$04","$50","$06","$A0","$08","$3C","$0A","$0E","$0C","$1A","$0E","$0C","$10","$18","$12","$30","$14","$60","$16","$C0","$18","$48","$1A","$10","$1C","$20","$1E")
};

static CBitfieldData* tblAPUDMCCTRLBitfields [] =
{
   new CBitfieldData("IRQ Enabled", 7, 1, "%X", 2, "No", "Yes"),
   new CBitfieldData("Loop", 6, 1, "%X", 2, "No", "Yes"),
   new CBitfieldData("Period", 0, 4, "%X", 16, "$1AC","$17C","$154","$140","$11E","$0FE","$0E2","$0D6","$0BE","$0A0","$08E","$080","$06A","$054","$048","$036")
};

static CBitfieldData* tblAPUDMCVOLBitfields [] =
{
   new CBitfieldData("Volume", 0, 7, "%02X", 0)
};

static CBitfieldData* tblAPUDMCADDRBitfields [] =
{
   new CBitfieldData("Sample Address", 0, 8, "%02X", 0)
};

static CBitfieldData* tblAPUDMCLENBitfields [] =
{
   new CBitfieldData("Sample Length", 0, 8, "%02X", 0)
};

static CBitfieldData* tblAPUCTRLBitfields [] =
{
   new CBitfieldData("DMC IRQ", 7, 1, "%X", 2, "Not Asserted", "Asserted"),
   new CBitfieldData("Frame IRQ", 6, 1, "%X", 2, "Not Asserted", "Asserted"),
   new CBitfieldData("Delta Modulation Channel", 4, 1, "%X", 2, "Disabled", "Enabled"),
   new CBitfieldData("Noise Channel", 3, 1, "%X", 2, "Disabled", "Enabled"),
   new CBitfieldData("Triangle Channel", 2, 1, "%X", 2, "Disabled", "Enabled"),
   new CBitfieldData("Square2 Channel", 1, 1, "%X", 2, "Disabled", "Enabled"),
   new CBitfieldData("Square1 Channel", 0, 1, "%X", 2, "Disabled", "Enabled"),
};

static CBitfieldData* tblAPUMASKBitfields [] =
{
   new CBitfieldData("Sequencer Mode", 7, 1, "%X", 2, "4-step", "5-step"),
   new CBitfieldData("IRQ", 6, 1, "%X", 2, "Enabled", "Disabled")
};

static CRegisterData* tblAPURegisters [] =
{
   new CRegisterData(0x4000, "SQ1CTRL", 4, tblAPUSQCTRLBitfields),
   new CRegisterData(0x4001, "SQ1SWEEP", 4, tblAPUSQSWEEPBitfields),
   new CRegisterData(0x4002, "SQ1PERLO", 1, tblAPUPERIODLOBitfields),
   new CRegisterData(0x4003, "SQ1PERLEN", 2, tblAPUPERIODLENBitfields),
   new CRegisterData(0x4004, "SQ2CTRL", 4, tblAPUSQCTRLBitfields),
   new CRegisterData(0x4005, "SQ2SWEEP", 4, tblAPUSQSWEEPBitfields),
   new CRegisterData(0x4006, "SQ2PERLO", 1, tblAPUPERIODLOBitfields),
   new CRegisterData(0x4007, "SQ2PERLEN", 2, tblAPUPERIODLENBitfields),
   new CRegisterData(0x4008, "TRICTRL", 2, tblAPUTRICTRLBitfields),
   new CRegisterData(0x4009, "TRIDMZ", 1, tblAPUDMZBitfields),
   new CRegisterData(0x400A, "TRIPERLO", 1, tblAPUPERIODLOBitfields),
   new CRegisterData(0x400B, "TRIPERLEN", 2, tblAPUPERIODLENBitfields),
   new CRegisterData(0x400C, "NOISECTRL", 3, tblAPUNZCTRLBitfields),
   new CRegisterData(0x400D, "NOISEDMZ", 1, tblAPUDMZBitfields),
   new CRegisterData(0x400E, "NOISEPERIOD", 2, tblAPUNZPERIODBitfields),
   new CRegisterData(0x400F, "NOISELEN", 1, tblAPUNZLENBitfields),
   new CRegisterData(0x4010, "DMCCTRL", 3, tblAPUDMCCTRLBitfields),
   new CRegisterData(0x4011, "DMCVOL", 1, tblAPUDMCVOLBitfields),
   new CRegisterData(0x4012, "DMCADDR", 1, tblAPUDMCADDRBitfields),
   new CRegisterData(0x4013, "DMCLEN", 1, tblAPUDMCLENBitfields),
   new CRegisterData(0x4014, "APUDMZ", 1, tblAPUDMZBitfields),
   new CRegisterData(0x4015, "APUCTRL", 7, tblAPUCTRLBitfields),
   new CRegisterData(0x4016, "APUDMZ", 1, tblAPUDMZBitfields),
   new CRegisterData(0x4017, "APUMASK", 2, tblAPUMASKBitfields),
};

CRegisterData** CAPUDBG::m_tblRegisters = tblAPURegisters;
int             CAPUDBG::m_numRegisters = NUM_APU_REGISTERS;

CBreakpointEventInfo** CAPUDBG::m_tblBreakpointEvents = tblAPUEvents;
int                    CAPUDBG::m_numBreakpointEvents = NUM_APU_EVENTS;

static CAPUDBG __init __attribute((unused));

CAPUDBG::CAPUDBG()
{
}

CAPUDBG::~CAPUDBG()
{
}
