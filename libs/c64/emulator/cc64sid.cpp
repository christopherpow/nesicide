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

#include "cc64sid.h"

#include "c64_emulator_core.h"

// CPU Registers
static CBitfieldData* tblFrequencyBitfields [] =
{
   new CBitfieldData("Frequency", 0, 16, "%04X", 0),
};

static CBitfieldData* tblPulseWidthBitfields [] =
{
   new CBitfieldData("Pulse Width", 0, 16, "%04X", 0),
};

static CBitfieldData* tblControlBitfields [] =
{
   new CBitfieldData("Status", 0, 1, "%X", 2, "Voice off, Release cycle", "Voice on, Attack-Decay-Sustain cycle"),
   new CBitfieldData("Synchronization", 1, 1, "%X", 2, "Synchronization disabled", "Synchronization enabled"),
   new CBitfieldData("Ring Modulation", 2, 1, "%X", 2, "Ring modulation disabled", "Ring modulation enabled"),
   new CBitfieldData("Voice Enable", 3, 1, "%X", 2, "Enable voice", "Disable voice, reset noise generator"),
   new CBitfieldData("Triangle Waveform", 4, 1, "%X", 2, "Disabled", "Enabled"),
   new CBitfieldData("Saw Waveform", 5, 1, "%X", 2, "Disabled", "Enabled"),
   new CBitfieldData("Rectangle Waveform", 6, 1, "%X", 2, "Disabled", "Enabled"),
   new CBitfieldData("Noise", 7, 1, "%X", 2, "Disabled", "Enabled"),
};

static CBitfieldData* tblAttackDecayLengthBitfields [] =
{
   new CBitfieldData("Decay length", 0, 4, "%X", 16, "6 ms", "24 ms", "48 ms", "72 ms", "114 ms", "168 ms", "204 ms", "240 ms", "300 ms", "750 ms", "1.5 s", "2.4 s", "3 s", "9 s", "15 s", "24 s"),
   new CBitfieldData("Attack length", 4, 4, "%X", 16, "2 ms", "8 ms", "16 ms", "24 ms", "38 ms", "56 ms", "68 ms", "80 ms", "100 ms", "250 ms", "500 ms", "800 ms", "1 s", "3 s", "5 s", "8 s"),
};

static CBitfieldData* tblSustainVolumeReleaseLengthBitfields [] =
{
   new CBitfieldData("Release length", 0, 4, "%X", 16, "6 ms", "24 ms", "48 ms", "72 ms", "114 ms", "168 ms", "204 ms", "240 ms", "300 ms", "750 ms", "1.5 s", "2.4 s", "3 s", "9 s", "15 s", "24 s"),
   new CBitfieldData("Sustain volume", 4, 4, "%X", 0)
};

static CRegisterData* tblSIDRegisters [] =
{
   new CRegisterData(0xd400, "Frequency", c64GetSID16Register, c64SetSID16Register, 1, tblFrequencyBitfields),
   new CRegisterData(0xd402, "Pulse Width", c64GetSID16Register, c64SetSID16Register, 1, tblPulseWidthBitfields),
   new CRegisterData(0xd404, "Control", c64GetSID8Register, c64SetSID8Register, 8, tblControlBitfields),
   new CRegisterData(0xd405, "Attack/Decay Length", c64GetSID8Register, c64SetSID8Register, 2, tblAttackDecayLengthBitfields),
   new CRegisterData(0xd406, "Sustain Volume/Release Length", c64GetSID8Register, c64SetSID8Register, 2, tblSustainVolumeReleaseLengthBitfields),
   new CRegisterData(0xd407, "Frequency", c64GetSID16Register, c64SetSID16Register, 1, tblFrequencyBitfields),
   new CRegisterData(0xd409, "Pulse Width", c64GetSID16Register, c64SetSID16Register, 1, tblPulseWidthBitfields),
   new CRegisterData(0xd40b, "Control", c64GetSID8Register, c64SetSID8Register, 8, tblControlBitfields),
   new CRegisterData(0xd40c, "Attack/Decay Length", c64GetSID8Register, c64SetSID8Register, 2, tblAttackDecayLengthBitfields),
   new CRegisterData(0xd40d, "Sustain Volume/Release Length", c64GetSID8Register, c64SetSID8Register, 2, tblSustainVolumeReleaseLengthBitfields),
   new CRegisterData(0xd40e, "Frequency", c64GetSID16Register, c64SetSID16Register, 1, tblFrequencyBitfields),
   new CRegisterData(0xd410, "Pulse Width", c64GetSID16Register, c64SetSID16Register, 1, tblPulseWidthBitfields),
   new CRegisterData(0xd412, "Control", c64GetSID8Register, c64SetSID8Register, 8, tblControlBitfields),
   new CRegisterData(0xd413, "Attack/Decay Length", c64GetSID8Register, c64SetSID8Register, 2, tblAttackDecayLengthBitfields),
   new CRegisterData(0xd414, "Sustain Volume/Release Length", c64GetSID8Register, c64SetSID8Register, 2, tblSustainVolumeReleaseLengthBitfields),
};

static const char* rowHeadings [] =
{
   "Voice #1",
   "Voice #2",
   "Voice #3"
};

static const char* columnHeadings [] =
{
   "x0","x2","x4","x5","x6"
};

static CRegisterDatabase* dbRegisters = new CRegisterDatabase(eMemory_SIDregs,3,5,15,tblSIDRegisters,rowHeadings,columnHeadings);

CRegisterDatabase* CSID::m_dbRegisters = dbRegisters;

uint8_t CSID::m_SIDmemory[] = { 0, };
