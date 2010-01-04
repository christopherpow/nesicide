#include "cregisterdata.h"

CBitfieldData* tblPPUCTRLBitfields [] =
{
   new CBitfieldData("Generate NMI", 7, 1, 2, "No", "Yes"),
   new CBitfieldData("PPU Master/Slave", 6, 1, 2, "Master", "Slave"),
   new CBitfieldData("Sprite Size", 5, 1, 2, "8x8", "8x16"),
   new CBitfieldData("Playfield Pattern Table", 4, 1, 2, "$0000", "$1000"),
   new CBitfieldData("Sprite Pattern Table", 3, 1, 2, "$0000", "$1000"),
   new CBitfieldData("VRAM Address Increment", 2, 1, 2, "+1", "+32"),
   new CBitfieldData("NameTable", 0, 2, 4, "NT1", "NT2", "NT3", "NT4")
};

CBitfieldData* tblPPUMASKBitfields [] =
{
   new CBitfieldData("Blue Emphasis", 7, 1, 2, "Off", "On"),
   new CBitfieldData("Green Emphasis", 6, 1, 2, "Off", "On"),
   new CBitfieldData("Red Emphasis", 5, 1, 2, "Off", "On"),
   new CBitfieldData("Sprite Rendering", 4, 1, 2, "Off", "On"),
   new CBitfieldData("Playfield Rendering", 3, 1, 2, "Off", "On"),
   new CBitfieldData("Sprite Clipping", 2, 1, 2, "Yes", "No"),
   new CBitfieldData("Playfield Clipping", 1, 1, 2, "Yes", "No"),
   new CBitfieldData("Greyscale", 0, 1, 2, "No", "Yes")
};

CBitfieldData* tblPPUSTATUSBitfields [] =
{
   new CBitfieldData("Vertical Blank", 7, 1, 2, "No", "Yes"),
   new CBitfieldData("Sprite 0 Hit", 6, 1, 2, "No", "Yes"),
   new CBitfieldData("Sprite Overflow", 5, 1, 2, "No", "Yes")
};

CBitfieldData* tblOAMADDRBitfields [] =
{
   new CBitfieldData("OAM Address", 0, 8, 0)
};

CBitfieldData* tblOAMDATABitfields [] =
{
   new CBitfieldData("OAM Data", 0, 8, 0)
};

CBitfieldData* tblPPUSCROLLBitfields [] =
{
   new CBitfieldData("PPU Scroll", 0, 8, 0)
};

CBitfieldData* tblPPUADDRBitfields [] =
{
   new CBitfieldData("PPU Address", 0, 8, 0)
};

CBitfieldData* tblPPUDATABitfields [] =
{
   new CBitfieldData("PPU Data", 0, 8, 0)
};

CRegisterData* tblPPURegisters [] =
{
   new CRegisterData("PPUCTRL", 7, tblPPUCTRLBitfields),
   new CRegisterData("PPUMASK", 8, tblPPUMASKBitfields),
   new CRegisterData("PPUSTATUS", 3, tblPPUSTATUSBitfields),
   new CRegisterData("OAMADDR", 1, tblOAMADDRBitfields),
   new CRegisterData("OAMDATA", 1, tblOAMDATABitfields),
   new CRegisterData("PPUSCROLL", 1, tblPPUSCROLLBitfields),
   new CRegisterData("PPUADDR", 1, tblPPUADDRBitfields),
   new CRegisterData("PPUDATA", 1, tblPPUDATABitfields)
};

CBitfieldData* tblAPUSQCTRLBitfields [] =
{
   new CBitfieldData("Duty Cycle", 6, 2, 4, "25%", "50%", "75%", "12.5%"),
   new CBitfieldData("Channel State", 5, 1, 2, "Running", "Halted"),
   new CBitfieldData("Envelope Enabled", 4, 1, 2, "No", "Yes"),
   new CBitfieldData("Volume/Envelope", 0, 4, 0)
};

CBitfieldData* tblAPUSQSWEEPBitfields [] =
{
   new CBitfieldData("Sweep Enabled", 7, 1, 2, "No", "Yes"),
   new CBitfieldData("Sweep Divider", 4, 3, 0),
   new CBitfieldData("Sweep Direction", 3, 1, 2, "Down", "Up"),
   new CBitfieldData("Sweep Shift", 0, 3, 0)
};

CBitfieldData* tblAPUPERIODLOBitfields [] =
{
   new CBitfieldData("Period Low-bits", 0, 8, 0)
};

CBitfieldData* tblAPUPERIODLENBitfields [] =
{
   new CBitfieldData("Length", 3, 5, 32, "$0A","$FE","$14","$02","$28","$04","$50","$06","$A0","$08","$3C","$0A","$0E","$0C","$1A","$0E","$0C","$10","$18","$12","$30","$14","$60","$16","$C0","$18","$48","$1A","$10","$1C","$20","$1E"),
   new CBitfieldData("Period High-bits", 0, 3, 0)
};

CBitfieldData* tblAPUTRICTRLBitfields [] =
{
   new CBitfieldData("Channel State", 7, 1, 2, "Running", "Halted"),
   new CBitfieldData("Linear Counter Reload", 0, 6, 0)
};

CBitfieldData* tblAPUDMZBitfields [] =
{
   new CBitfieldData("Unused", 0, 8, 0),
};

CBitfieldData* tblAPUNZCTRLBitfields [] =
{
   new CBitfieldData("Channel State", 5, 1, 2, "Running", "Halted"),
   new CBitfieldData("Envelope Enabled", 4, 1, 2, "No", "Yes"),
   new CBitfieldData("Volume/Envelope", 0, 4, 0)
};

CBitfieldData* tblAPUNZPERIODBitfields [] =
{
   new CBitfieldData("Mode", 7, 1, 2, "32,767 samples", "93 samples"),
   new CBitfieldData("Period", 0, 4, 16, "$004","$008","$010","$020","$040","$060","$080","$0A0","$0CA","$0FE","$17C","$1FC","$2FA","$3F8","$7F2","$FE4")
};

CBitfieldData* tblAPUNZLENBitfields [] =
{
   new CBitfieldData("Length", 3, 5, 32, "$0A","$FE","$14","$02","$28","$04","$50","$06","$A0","$08","$3C","$0A","$0E","$0C","$1A","$0E","$0C","$10","$18","$12","$30","$14","$60","$16","$C0","$18","$48","$1A","$10","$1C","$20","$1E")
};

CBitfieldData* tblAPUDMCCTRLBitfields [] =
{
   new CBitfieldData("IRQ Enabled", 7, 1, 2, "No", "Yes"),
   new CBitfieldData("Loop", 6, 1, 2, "No", "Yes"),
   new CBitfieldData("Period", 0, 4, 16, "$1AC","$17C","$154","$140","$11E","$0FE","$0E2","$0D6","$0BE","$0A0","$08E","$080","$06A","$054","$048","$036")
};

CBitfieldData* tblAPUDMCVOLBitfields [] =
{
   new CBitfieldData("Volume", 0, 7, 0)
};

CBitfieldData* tblAPUDMCADDRBitfields [] =
{
   new CBitfieldData("Sample Address", 0, 8, 0)
};

CBitfieldData* tblAPUDMCLENBitfields [] =
{
   new CBitfieldData("Sample Length", 0, 8, 0)
};

CBitfieldData* tblAPUCTRLBitfields [] =
{
   new CBitfieldData("DMC IRQ", 7, 1, 2, "Not Asserted", "Asserted"),
   new CBitfieldData("APU IRQ", 6, 1, 2, "Not Asserted", "Asserted"),
   new CBitfieldData("Delta Modulation Channel", 4, 1, 2, "Disabled", "Enabled"),
   new CBitfieldData("Noise Channel", 3, 1, 2, "Disabled", "Enabled"),
   new CBitfieldData("Triangle Channel", 2, 1, 2, "Disabled", "Enabled"),
   new CBitfieldData("Square2 Channel", 1, 1, 2, "Disabled", "Enabled"),
   new CBitfieldData("Square1 Channel", 0, 1, 2, "Disabled", "Enabled"),
};

CBitfieldData* tblAPUMASKBitfields [] =
{
   new CBitfieldData("IRQ", 7, 1, 2, "Disabled", "Enabled"),
   new CBitfieldData("Sequencer Mode", 6, 1, 2, "4-step", "5-step")
};

CRegisterData* tblAPURegisters [] =
{
   new CRegisterData("SQ1CTRL", 4, tblAPUSQCTRLBitfields),
   new CRegisterData("SQ1SWEEP", 4, tblAPUSQSWEEPBitfields),
   new CRegisterData("SQ1PERLO", 1, tblAPUPERIODLOBitfields),
   new CRegisterData("SQ1PERLEN", 2, tblAPUPERIODLENBitfields),
   new CRegisterData("SQ2CTRL", 4, tblAPUSQCTRLBitfields),
   new CRegisterData("SQ2SWEEP", 4, tblAPUSQSWEEPBitfields),
   new CRegisterData("SQ2PERLO", 1, tblAPUPERIODLOBitfields),
   new CRegisterData("SQ2PERLEN", 2, tblAPUPERIODLENBitfields),
   new CRegisterData("TRICTRL", 2, tblAPUTRICTRLBitfields),
   new CRegisterData("TRIDMZ", 1, tblAPUDMZBitfields),
   new CRegisterData("TRIPERLO", 1, tblAPUPERIODLOBitfields),
   new CRegisterData("TRIPERLEN", 2, tblAPUPERIODLENBitfields),
   new CRegisterData("NOISECTRL", 3, tblAPUNZCTRLBitfields),
   new CRegisterData("NOISEDMZ", 1, tblAPUDMZBitfields),
   new CRegisterData("NOISEPERIOD", 2, tblAPUNZPERIODBitfields),
   new CRegisterData("NOISELEN", 1, tblAPUNZLENBitfields),
   new CRegisterData("DMCCTRL", 3, tblAPUDMCCTRLBitfields),
   new CRegisterData("DMCVOL", 1, tblAPUDMCVOLBitfields),
   new CRegisterData("DMCADDR", 1, tblAPUDMCADDRBitfields),
   new CRegisterData("DMCLEN", 1, tblAPUDMCLENBitfields),
   new CRegisterData("APUDMZ", 1, tblAPUDMZBitfields),
   new CRegisterData("APUCTRL", 7, tblAPUCTRLBitfields),
   new CRegisterData("APUDMZ", 1, tblAPUDMZBitfields),
   new CRegisterData("APUMASK", 2, tblAPUMASKBitfields),
};

CBitfieldData* tblOAMYBitfields [] =
{
   new CBitfieldData("Y Position", 0, 8, 0),
};

CBitfieldData* tblOAMPATBitfields [] =
{
   new CBitfieldData("Tile Index", 0, 8, 0),
};

CBitfieldData* tblOAMATTBitfields [] =
{
   new CBitfieldData("Flip Vertical", 7, 1, 2, "No", "Yes"),
   new CBitfieldData("Flip Horizontal", 6, 1, 2, "No", "Yes"),
   new CBitfieldData("Priority", 5, 1, 2, "In-front of Playfield", "Behind Playfield"),
   new CBitfieldData("Palette Index", 0, 2, 4, "$3F10", "$3F14", "$3F18", "$3F1C")
};

CBitfieldData* tblOAMXBitfields [] =
{
   new CBitfieldData("X Position", 0, 8, 0),
};

CRegisterData* tblOAMRegisters [] =
{
   // Sprite
   new CRegisterData("OAMX", 1, tblOAMXBitfields),
   new CRegisterData("OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData("OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData("OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData("OAMX", 1, tblOAMXBitfields),
   new CRegisterData("OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData("OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData("OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData("OAMX", 1, tblOAMXBitfields),
   new CRegisterData("OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData("OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData("OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData("OAMX", 1, tblOAMXBitfields),
   new CRegisterData("OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData("OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData("OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData("OAMX", 1, tblOAMXBitfields),
   new CRegisterData("OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData("OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData("OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData("OAMX", 1, tblOAMXBitfields),
   new CRegisterData("OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData("OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData("OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData("OAMX", 1, tblOAMXBitfields),
   new CRegisterData("OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData("OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData("OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData("OAMX", 1, tblOAMXBitfields),
   new CRegisterData("OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData("OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData("OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData("OAMX", 1, tblOAMXBitfields),
   new CRegisterData("OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData("OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData("OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData("OAMX", 1, tblOAMXBitfields),
   new CRegisterData("OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData("OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData("OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData("OAMX", 1, tblOAMXBitfields),
   new CRegisterData("OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData("OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData("OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData("OAMX", 1, tblOAMXBitfields),
   new CRegisterData("OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData("OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData("OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData("OAMX", 1, tblOAMXBitfields),
   new CRegisterData("OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData("OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData("OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData("OAMX", 1, tblOAMXBitfields),
   new CRegisterData("OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData("OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData("OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData("OAMX", 1, tblOAMXBitfields),
   new CRegisterData("OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData("OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData("OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData("OAMX", 1, tblOAMXBitfields),
   new CRegisterData("OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData("OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData("OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData("OAMX", 1, tblOAMXBitfields),
   new CRegisterData("OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData("OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData("OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData("OAMX", 1, tblOAMXBitfields),
   new CRegisterData("OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData("OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData("OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData("OAMX", 1, tblOAMXBitfields),
   new CRegisterData("OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData("OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData("OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData("OAMX", 1, tblOAMXBitfields),
   new CRegisterData("OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData("OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData("OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData("OAMX", 1, tblOAMXBitfields),
   new CRegisterData("OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData("OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData("OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData("OAMX", 1, tblOAMXBitfields),
   new CRegisterData("OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData("OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData("OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData("OAMX", 1, tblOAMXBitfields),
   new CRegisterData("OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData("OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData("OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData("OAMX", 1, tblOAMXBitfields),
   new CRegisterData("OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData("OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData("OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData("OAMX", 1, tblOAMXBitfields),
   new CRegisterData("OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData("OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData("OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData("OAMX", 1, tblOAMXBitfields),
   new CRegisterData("OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData("OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData("OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData("OAMX", 1, tblOAMXBitfields),
   new CRegisterData("OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData("OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData("OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData("OAMX", 1, tblOAMXBitfields),
   new CRegisterData("OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData("OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData("OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData("OAMX", 1, tblOAMXBitfields),
   new CRegisterData("OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData("OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData("OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData("OAMX", 1, tblOAMXBitfields),
   new CRegisterData("OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData("OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData("OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData("OAMX", 1, tblOAMXBitfields),
   new CRegisterData("OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData("OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData("OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData("OAMX", 1, tblOAMXBitfields),
   new CRegisterData("OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData("OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData("OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData("OAMX", 1, tblOAMXBitfields),
   new CRegisterData("OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData("OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData("OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData("OAMX", 1, tblOAMXBitfields),
   new CRegisterData("OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData("OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData("OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData("OAMX", 1, tblOAMXBitfields),
   new CRegisterData("OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData("OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData("OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData("OAMX", 1, tblOAMXBitfields),
   new CRegisterData("OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData("OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData("OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData("OAMX", 1, tblOAMXBitfields),
   new CRegisterData("OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData("OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData("OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData("OAMX", 1, tblOAMXBitfields),
   new CRegisterData("OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData("OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData("OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData("OAMX", 1, tblOAMXBitfields),
   new CRegisterData("OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData("OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData("OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData("OAMX", 1, tblOAMXBitfields),
   new CRegisterData("OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData("OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData("OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData("OAMX", 1, tblOAMXBitfields),
   new CRegisterData("OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData("OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData("OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData("OAMX", 1, tblOAMXBitfields),
   new CRegisterData("OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData("OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData("OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData("OAMX", 1, tblOAMXBitfields),
   new CRegisterData("OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData("OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData("OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData("OAMX", 1, tblOAMXBitfields),
   new CRegisterData("OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData("OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData("OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData("OAMX", 1, tblOAMXBitfields),
   new CRegisterData("OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData("OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData("OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData("OAMX", 1, tblOAMXBitfields),
   new CRegisterData("OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData("OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData("OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData("OAMX", 1, tblOAMXBitfields),
   new CRegisterData("OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData("OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData("OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData("OAMX", 1, tblOAMXBitfields),
   new CRegisterData("OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData("OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData("OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData("OAMX", 1, tblOAMXBitfields),
   new CRegisterData("OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData("OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData("OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData("OAMX", 1, tblOAMXBitfields),
   new CRegisterData("OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData("OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData("OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData("OAMX", 1, tblOAMXBitfields),
   new CRegisterData("OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData("OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData("OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData("OAMX", 1, tblOAMXBitfields),
   new CRegisterData("OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData("OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData("OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData("OAMX", 1, tblOAMXBitfields),
   new CRegisterData("OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData("OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData("OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData("OAMX", 1, tblOAMXBitfields),
   new CRegisterData("OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData("OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData("OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData("OAMX", 1, tblOAMXBitfields),
   new CRegisterData("OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData("OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData("OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData("OAMX", 1, tblOAMXBitfields),
   new CRegisterData("OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData("OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData("OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData("OAMX", 1, tblOAMXBitfields),
   new CRegisterData("OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData("OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData("OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData("OAMX", 1, tblOAMXBitfields),
   new CRegisterData("OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData("OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData("OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData("OAMX", 1, tblOAMXBitfields),
   new CRegisterData("OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData("OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData("OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData("OAMX", 1, tblOAMXBitfields),
   new CRegisterData("OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData("OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData("OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData("OAMX", 1, tblOAMXBitfields),
   new CRegisterData("OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData("OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData("OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData("OAMX", 1, tblOAMXBitfields),
   new CRegisterData("OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData("OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData("OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData("OAMX", 1, tblOAMXBitfields),
   new CRegisterData("OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData("OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData("OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData("OAMX", 1, tblOAMXBitfields),
   new CRegisterData("OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData("OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData("OAMY", 1, tblOAMYBitfields)
};
