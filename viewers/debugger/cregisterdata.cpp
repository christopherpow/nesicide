#include "cregisterdata.h"

// CPU Registers
CBitfieldData* tblCPUPCBitfields [] =
{
   new CBitfieldData("Instruction Pointer", 0, 16, "%04X", 0),
};

CBitfieldData* tblCPUABitfields [] =
{
   new CBitfieldData("Accumulator", 0, 8, "%02X", 0),
};

CBitfieldData* tblCPUXBitfields [] =
{
   new CBitfieldData("X Index", 0, 8, "%02X", 0),
};

CBitfieldData* tblCPUYBitfields [] =
{
   new CBitfieldData("Y Index", 0, 8, "%02X", 0),
};

CBitfieldData* tblCPUSPBitfields [] =
{
   new CBitfieldData("Stack Pointer", 0, 12, "%03X", 0),
};

CBitfieldData* tblCPUFBitfields [] =
{
   new CBitfieldData("Negative", 7, 1, "%X", 2, "No", "Yes"),
   new CBitfieldData("Overflow", 6, 1, "%X", 2, "No", "Yes"),
   new CBitfieldData("Break", 4, 1, "%X", 2, "No", "Yes"),
   new CBitfieldData("Decimal Mode", 3, 1, "%X", 2, "No", "Yes"),
   new CBitfieldData("Interrupt", 2, 1, "%X", 2, "No", "Yes"),
   new CBitfieldData("Zero", 1, 1, "%X", 2, "No", "Yes"),
   new CBitfieldData("Carry", 0, 1, "%X", 2, "No", "Yes")
};

CRegisterData* tblCPURegisters [] =
{
   new CRegisterData(0, "CPUPC", 1, tblCPUPCBitfields),
   new CRegisterData(1, "CPUA", 1, tblCPUABitfields),
   new CRegisterData(2, "CPUX", 1, tblCPUXBitfields),
   new CRegisterData(3, "CPUY", 1, tblCPUYBitfields),
   new CRegisterData(4, "CPUSP", 1, tblCPUSPBitfields),
   new CRegisterData(5, "CPUF", 7, tblCPUFBitfields)
};

// PPU Registers
CBitfieldData* tblPPUCTRLBitfields [] =
{
   new CBitfieldData("Generate NMI", 7, 1, "%X", 2, "No", "Yes"),
   new CBitfieldData("PPU Master/Slave", 6, 1, "%X", 2, "Master", "Slave"),
   new CBitfieldData("Sprite Size", 5, 1, "%X", 2, "8x8", "8x16"),
   new CBitfieldData("Playfield Pattern Table", 4, 1, "%X", 2, "$0000", "$1000"),
   new CBitfieldData("Sprite Pattern Table", 3, 1, "%X", 2, "$0000", "$1000"),
   new CBitfieldData("VRAM Address Increment", 2, 1, "%X", 2, "+1", "+32"),
   new CBitfieldData("NameTable", 0, 2, "%X", 4, "NT1", "NT2", "NT3", "NT4")
};

CBitfieldData* tblPPUMASKBitfields [] =
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

CBitfieldData* tblPPUSTATUSBitfields [] =
{
   new CBitfieldData("Vertical Blank", 7, 1, "%X", 2, "No", "Yes"),
   new CBitfieldData("Sprite 0 Hit", 6, 1, "%X", 2, "No", "Yes"),
   new CBitfieldData("Sprite Overflow", 5, 1, "%X", 2, "No", "Yes")
};

CBitfieldData* tblOAMADDRBitfields [] =
{
   new CBitfieldData("OAM Address", 0, 8, "%02X", 0)
};

CBitfieldData* tblOAMDATABitfields [] =
{
   new CBitfieldData("OAM Data", 0, 8, "%02X", 0)
};

CBitfieldData* tblPPUSCROLLBitfields [] =
{
   new CBitfieldData("PPU Scroll", 0, 8, "%02X", 0)
};

CBitfieldData* tblPPUADDRBitfields [] =
{
   new CBitfieldData("PPU Address", 0, 8, "%02X", 0)
};

CBitfieldData* tblPPUDATABitfields [] =
{
   new CBitfieldData("PPU Data", 0, 8, "%02X", 0)
};

CRegisterData* tblPPURegisters [] =
{
   new CRegisterData(0x2000, "PPUCTRL", 7, tblPPUCTRLBitfields),
   new CRegisterData(0x2001, "PPUMASK", 8, tblPPUMASKBitfields),
   new CRegisterData(0x2002, "PPUSTATUS", 3, tblPPUSTATUSBitfields),
   new CRegisterData(0x2003, "OAMADDR", 1, tblOAMADDRBitfields),
   new CRegisterData(0x2004, "OAMDATA", 1, tblOAMDATABitfields),
   new CRegisterData(0x2005, "PPUSCROLL", 1, tblPPUSCROLLBitfields),
   new CRegisterData(0x2006, "PPUADDR", 1, tblPPUADDRBitfields),
   new CRegisterData(0x2007, "PPUDATA", 1, tblPPUDATABitfields)
};

// APU Registers
CBitfieldData* tblAPUSQCTRLBitfields [] =
{
   new CBitfieldData("Duty Cycle", 6, 2, "%X", 4, "25%", "50%", "75%", "12.5%"),
   new CBitfieldData("Channel State", 5, 1, "%X", 2, "Running", "Halted"),
   new CBitfieldData("Envelope Enabled", 4, 1, "%X", 2, "No", "Yes"),
   new CBitfieldData("Volume/Envelope", 0, 4, "%X", 0)
};

CBitfieldData* tblAPUSQSWEEPBitfields [] =
{
   new CBitfieldData("Sweep Enabled", 7, 1, "%X", 2, "No", "Yes"),
   new CBitfieldData("Sweep Divider", 4, 3, "%X", 0),
   new CBitfieldData("Sweep Direction", 3, 1, "%X", 2, "Down", "Up"),
   new CBitfieldData("Sweep Shift", 0, 3, "%X", 0)
};

CBitfieldData* tblAPUPERIODLOBitfields [] =
{
   new CBitfieldData("Period Low-bits", 0, 8, "%02X", 0)
};

CBitfieldData* tblAPUPERIODLENBitfields [] =
{
   new CBitfieldData("Length", 3, 5, "%X", 32, "$0A","$FE","$14","$02","$28","$04","$50","$06","$A0","$08","$3C","$0A","$0E","$0C","$1A","$0E","$0C","$10","$18","$12","$30","$14","$60","$16","$C0","$18","$48","$1A","$10","$1C","$20","$1E"),
   new CBitfieldData("Period High-bits", 0, 3, "%X", 0)
};

CBitfieldData* tblAPUTRICTRLBitfields [] =
{
   new CBitfieldData("Channel State", 7, 1, "%X", 2, "Running", "Halted"),
   new CBitfieldData("Linear Counter Reload", 0, 6, "%X", 0)
};

CBitfieldData* tblAPUDMZBitfields [] =
{
   new CBitfieldData("Unused", 0, 8, "%02X", 0),
};

CBitfieldData* tblAPUNZCTRLBitfields [] =
{
   new CBitfieldData("Channel State", 5, 1, "%X", 2, "Running", "Halted"),
   new CBitfieldData("Envelope Enabled", 4, 1, "%X", 2, "No", "Yes"),
   new CBitfieldData("Volume/Envelope", 0, 4, "%X", 0)
};

CBitfieldData* tblAPUNZPERIODBitfields [] =
{
   new CBitfieldData("Mode", 7, 1, "%X", 2, "32,767 samples", "93 samples"),
   new CBitfieldData("Period", 0, 4, "%X", 16, "$004","$008","$010","$020","$040","$060","$080","$0A0","$0CA","$0FE","$17C","$1FC","$2FA","$3F8","$7F2","$FE4")
};

CBitfieldData* tblAPUNZLENBitfields [] =
{
   new CBitfieldData("Length", 3, 5, "%X", 32, "$0A","$FE","$14","$02","$28","$04","$50","$06","$A0","$08","$3C","$0A","$0E","$0C","$1A","$0E","$0C","$10","$18","$12","$30","$14","$60","$16","$C0","$18","$48","$1A","$10","$1C","$20","$1E")
};

CBitfieldData* tblAPUDMCCTRLBitfields [] =
{
   new CBitfieldData("IRQ Enabled", 7, 1, "%X", 2, "No", "Yes"),
   new CBitfieldData("Loop", 6, 1, "%X", 2, "No", "Yes"),
   new CBitfieldData("Period", 0, 4, "%X", 16, "$1AC","$17C","$154","$140","$11E","$0FE","$0E2","$0D6","$0BE","$0A0","$08E","$080","$06A","$054","$048","$036")
};

CBitfieldData* tblAPUDMCVOLBitfields [] =
{
   new CBitfieldData("Volume", 0, 7, "%02X", 0)
};

CBitfieldData* tblAPUDMCADDRBitfields [] =
{
   new CBitfieldData("Sample Address", 0, 8, "%02X", 0)
};

CBitfieldData* tblAPUDMCLENBitfields [] =
{
   new CBitfieldData("Sample Length", 0, 8, "%02X", 0)
};

CBitfieldData* tblAPUCTRLBitfields [] =
{
   new CBitfieldData("DMC IRQ", 7, 1, "%X", 2, "Not Asserted", "Asserted"),
   new CBitfieldData("APU IRQ", 6, 1, "%X", 2, "Not Asserted", "Asserted"),
   new CBitfieldData("Delta Modulation Channel", 4, 1, "%X", 2, "Disabled", "Enabled"),
   new CBitfieldData("Noise Channel", 3, 1, "%X", 2, "Disabled", "Enabled"),
   new CBitfieldData("Triangle Channel", 2, 1, "%X", 2, "Disabled", "Enabled"),
   new CBitfieldData("Square2 Channel", 1, 1, "%X", 2, "Disabled", "Enabled"),
   new CBitfieldData("Square1 Channel", 0, 1, "%X", 2, "Disabled", "Enabled"),
};

CBitfieldData* tblAPUMASKBitfields [] =
{
   new CBitfieldData("IRQ", 7, 1, "%X", 2, "Disabled", "Enabled"),
   new CBitfieldData("Sequencer Mode", 6, 1, "%X", 2, "4-step", "5-step")
};

CRegisterData* tblAPURegisters [] =
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

// OAM 'Registers'
CBitfieldData* tblOAMYBitfields [] =
{
   new CBitfieldData("Y Position", 0, 8, "%02X", 0),
};

CBitfieldData* tblOAMPATBitfields [] =
{
   new CBitfieldData("Tile Index", 0, 8, "%02X", 0),
};

CBitfieldData* tblOAMATTBitfields [] =
{
   new CBitfieldData("Flip Vertical", 7, 1, "%X", 2, "No", "Yes"),
   new CBitfieldData("Flip Horizontal", 6, 1, "%X", 2, "No", "Yes"),
   new CBitfieldData("Priority", 5, 1, "%X", 2, "In-front of Playfield", "Behind Playfield"),
   new CBitfieldData("Palette Index", 0, 2, "%X", 4, "$3F10", "$3F14", "$3F18", "$3F1C")
};

CBitfieldData* tblOAMXBitfields [] =
{
   new CBitfieldData("X Position", 0, 8, "%02X", 0),
};

CRegisterData* tblOAMRegisters [] =
{
   // Sprite
   new CRegisterData(0x00, "OAMX", 1, tblOAMXBitfields),
   new CRegisterData(0x01, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0x02, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0x03, "OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData(0x04, "OAMX", 1, tblOAMXBitfields),
   new CRegisterData(0x05, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0x06, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0x07, "OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData(0x08, "OAMX", 1, tblOAMXBitfields),
   new CRegisterData(0x09, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0x0A, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0x0B, "OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData(0x0C, "OAMX", 1, tblOAMXBitfields),
   new CRegisterData(0x0D, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0x0E, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0x0F, "OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData(0x10, "OAMX", 1, tblOAMXBitfields),
   new CRegisterData(0x11, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0x12, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0x13, "OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData(0x14, "OAMX", 1, tblOAMXBitfields),
   new CRegisterData(0x15, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0x16, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0x17, "OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData(0x18, "OAMX", 1, tblOAMXBitfields),
   new CRegisterData(0x19, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0x1A, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0x1B, "OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData(0x1C, "OAMX", 1, tblOAMXBitfields),
   new CRegisterData(0x1D, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0x1E, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0x1F, "OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData(0x20, "OAMX", 1, tblOAMXBitfields),
   new CRegisterData(0x21, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0x22, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0x23, "OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData(0x24, "OAMX", 1, tblOAMXBitfields),
   new CRegisterData(0x25, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0x26, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0x27, "OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData(0x28, "OAMX", 1, tblOAMXBitfields),
   new CRegisterData(0x29, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0x2A, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0x2B, "OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData(0x2C, "OAMX", 1, tblOAMXBitfields),
   new CRegisterData(0x2D, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0x2E, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0x2F, "OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData(0x30, "OAMX", 1, tblOAMXBitfields),
   new CRegisterData(0x31, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0x32, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0x33, "OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData(0x34, "OAMX", 1, tblOAMXBitfields),
   new CRegisterData(0x35, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0x36, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0x37, "OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData(0x38, "OAMX", 1, tblOAMXBitfields),
   new CRegisterData(0x39, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0x3A, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0x3B, "OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData(0x3C, "OAMX", 1, tblOAMXBitfields),
   new CRegisterData(0x3D, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0x3E, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0x3F, "OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData(0x40, "OAMX", 1, tblOAMXBitfields),
   new CRegisterData(0x41, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0x42, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0x43, "OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData(0x44, "OAMX", 1, tblOAMXBitfields),
   new CRegisterData(0x45, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0x46, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0x47, "OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData(0x48, "OAMX", 1, tblOAMXBitfields),
   new CRegisterData(0x49, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0x4A, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0x4B, "OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData(0x4C, "OAMX", 1, tblOAMXBitfields),
   new CRegisterData(0x4D, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0x4E, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0x4F, "OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData(0x50, "OAMX", 1, tblOAMXBitfields),
   new CRegisterData(0x51, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0x52, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0x53, "OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData(0x54, "OAMX", 1, tblOAMXBitfields),
   new CRegisterData(0x55, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0x56, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0x57, "OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData(0x58, "OAMX", 1, tblOAMXBitfields),
   new CRegisterData(0x59, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0x5A, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0x5B, "OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData(0x5C, "OAMX", 1, tblOAMXBitfields),
   new CRegisterData(0x5D, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0x5E, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0x5F, "OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData(0x60, "OAMX", 1, tblOAMXBitfields),
   new CRegisterData(0x61, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0x62, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0x63, "OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData(0x64, "OAMX", 1, tblOAMXBitfields),
   new CRegisterData(0x65, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0x66, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0x67, "OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData(0x68, "OAMX", 1, tblOAMXBitfields),
   new CRegisterData(0x69, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0x6A, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0x6B, "OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData(0x6C, "OAMX", 1, tblOAMXBitfields),
   new CRegisterData(0x6D, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0x6E, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0x6F, "OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData(0x70, "OAMX", 1, tblOAMXBitfields),
   new CRegisterData(0x71, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0x72, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0x73, "OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData(0x74, "OAMX", 1, tblOAMXBitfields),
   new CRegisterData(0x75, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0x76, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0x77, "OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData(0x78, "OAMX", 1, tblOAMXBitfields),
   new CRegisterData(0x79, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0x7A, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0x7B, "OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData(0x7C, "OAMX", 1, tblOAMXBitfields),
   new CRegisterData(0x7D, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0x7E, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0x7F, "OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData(0x80, "OAMX", 1, tblOAMXBitfields),
   new CRegisterData(0x81, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0x82, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0x83, "OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData(0x84, "OAMX", 1, tblOAMXBitfields),
   new CRegisterData(0x85, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0x86, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0x87, "OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData(0x88, "OAMX", 1, tblOAMXBitfields),
   new CRegisterData(0x89, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0x8A, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0x8B, "OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData(0x8C, "OAMX", 1, tblOAMXBitfields),
   new CRegisterData(0x8D, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0x8E, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0x8F, "OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData(0x90, "OAMX", 1, tblOAMXBitfields),
   new CRegisterData(0x91, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0x92, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0x93, "OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData(0x94, "OAMX", 1, tblOAMXBitfields),
   new CRegisterData(0x95, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0x96, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0x97, "OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData(0x98, "OAMX", 1, tblOAMXBitfields),
   new CRegisterData(0x99, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0x9A, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0x9B, "OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData(0x9C, "OAMX", 1, tblOAMXBitfields),
   new CRegisterData(0x9D, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0x9E, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0x9F, "OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData(0xA0, "OAMX", 1, tblOAMXBitfields),
   new CRegisterData(0xA1, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0xA2, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0xA3, "OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData(0xA4, "OAMX", 1, tblOAMXBitfields),
   new CRegisterData(0xA5, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0xA6, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0xA7, "OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData(0xA8, "OAMX", 1, tblOAMXBitfields),
   new CRegisterData(0xA9, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0xAA, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0xAB, "OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData(0xAC, "OAMX", 1, tblOAMXBitfields),
   new CRegisterData(0xAD, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0xAE, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0xAF, "OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData(0xB0, "OAMX", 1, tblOAMXBitfields),
   new CRegisterData(0xB1, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0xB2, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0xB3, "OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData(0xB4, "OAMX", 1, tblOAMXBitfields),
   new CRegisterData(0xB5, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0xB6, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0xB7, "OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData(0xB8, "OAMX", 1, tblOAMXBitfields),
   new CRegisterData(0xB9, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0xBA, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0xBB, "OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData(0xBC, "OAMX", 1, tblOAMXBitfields),
   new CRegisterData(0xBD, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0xBE, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0xBF, "OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData(0xC0, "OAMX", 1, tblOAMXBitfields),
   new CRegisterData(0xC1, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0xC2, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0xC3, "OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData(0xC4, "OAMX", 1, tblOAMXBitfields),
   new CRegisterData(0xC5, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0xC6, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0xC7, "OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData(0xC8, "OAMX", 1, tblOAMXBitfields),
   new CRegisterData(0xC9, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0xCA, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0xCB, "OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData(0xCC, "OAMX", 1, tblOAMXBitfields),
   new CRegisterData(0xCD, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0xCE, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0xCF, "OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData(0xD0, "OAMX", 1, tblOAMXBitfields),
   new CRegisterData(0xD1, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0xD2, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0xD3, "OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData(0xD4, "OAMX", 1, tblOAMXBitfields),
   new CRegisterData(0xD5, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0xD6, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0xD7, "OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData(0xD8, "OAMX", 1, tblOAMXBitfields),
   new CRegisterData(0xD9, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0xDA, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0xDB, "OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData(0xDC, "OAMX", 1, tblOAMXBitfields),
   new CRegisterData(0xDD, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0xDE, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0xDF, "OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData(0xE0, "OAMX", 1, tblOAMXBitfields),
   new CRegisterData(0xE1, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0xE2, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0xE3, "OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData(0xE4, "OAMX", 1, tblOAMXBitfields),
   new CRegisterData(0xE5, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0xE6, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0xE7, "OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData(0xE8, "OAMX", 1, tblOAMXBitfields),
   new CRegisterData(0xE9, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0xEA, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0xEB, "OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData(0xEC, "OAMX", 1, tblOAMXBitfields),
   new CRegisterData(0xED, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0xEE, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0xEF, "OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData(0xF0, "OAMX", 1, tblOAMXBitfields),
   new CRegisterData(0xF1, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0xF2, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0xF3, "OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData(0xF4, "OAMX", 1, tblOAMXBitfields),
   new CRegisterData(0xF5, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0xF6, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0xF7, "OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData(0xF8, "OAMX", 1, tblOAMXBitfields),
   new CRegisterData(0xF9, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0xFA, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0xFB, "OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData(0xFC, "OAMX", 1, tblOAMXBitfields),
   new CRegisterData(0xFD, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0xFE, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0xFF, "OAMY", 1, tblOAMYBitfields)
};
