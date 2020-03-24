#include "cnes.h"
#include "cnesio.h"
#include "cnesios.h"
#include "cnesrom.h"
#include "cnesppu.h"
#include "cnesapu.h"
#include "cnes6502.h"
#include "cnesrommapper001.h"
#include "cnesrommapper004.h"
#include "cnesrommapper009.h"
#include "cnesrommapper010.h"
#include "cnesrommapper016.h"
#include "cnesrommapper028.h"
#include "cnesrommapper069.h"

#include "common/cnessystempalette.h"

#include "cregisterdata.h"
#include "cmemorydata.h"
#include "cbreakpointinfo.h"

// CPU Registers
static CBitfieldData* tblCPUPCBitfields [] =
{
   new CBitfieldData("Instruction Pointer", 0, 16, "%04X", 0),
};

static CBitfieldData* tblCPUABitfields [] =
{
   new CBitfieldData("Accumulator", 0, 8, "%02X", 0),
};

static CBitfieldData* tblCPUXBitfields [] =
{
   new CBitfieldData("X Index", 0, 8, "%02X", 0),
};

static CBitfieldData* tblCPUYBitfields [] =
{
   new CBitfieldData("Y Index", 0, 8, "%02X", 0),
};

static CBitfieldData* tblCPUSPBitfields [] =
{
   new CBitfieldData("Stack Pointer", 0, 12, "%03X", 0),
};

static CBitfieldData* tblCPUFBitfields [] =
{
   new CBitfieldData("Negative", 7, 1, "%X", 2, "No", "Yes"),
   new CBitfieldData("Overflow", 6, 1, "%X", 2, "No", "Yes"),
   new CBitfieldData("Break", 4, 1, "%X", 2, "No", "Yes"),
   new CBitfieldData("Decimal Mode", 3, 1, "%X", 2, "No", "Yes"),
   new CBitfieldData("Interrupt", 2, 1, "%X", 2, "No", "Yes"),
   new CBitfieldData("Zero", 1, 1, "%X", 2, "No", "Yes"),
   new CBitfieldData("Carry", 0, 1, "%X", 2, "No", "Yes")
};

static CBitfieldData* tblCPUNMIVectorBitfields [] =
{
   new CBitfieldData("NMI Vector", 0, 16, "%04X", 0),
};

static CBitfieldData* tblCPUIRQVectorBitfields [] =
{
   new CBitfieldData("IRQ Vector", 0, 16, "%04X", 0),
};

static CBitfieldData* tblCPURESETVectorBitfields [] =
{
   new CBitfieldData("RESET Vector", 0, 16, "%04X", 0),
};

static CRegisterData* tblCPURegisters [] =
{
   new CRegisterData(CPU_PC, "Program Counter", nesGetCPURegister, nesSetCPURegister, 1, tblCPUPCBitfields),
   new CRegisterData(CPU_A, "Accumulator", nesGetCPURegister, nesSetCPURegister, 1, tblCPUABitfields),
   new CRegisterData(CPU_X, "X Index", nesGetCPURegister, nesSetCPURegister, 1, tblCPUXBitfields),
   new CRegisterData(CPU_Y, "Y Index", nesGetCPURegister, nesSetCPURegister, 1, tblCPUYBitfields),
   new CRegisterData(CPU_SP, "Stack Pointer", nesGetCPURegister, nesSetCPURegister, 1, tblCPUSPBitfields),
   new CRegisterData(CPU_F, "Flags", nesGetCPURegister, nesSetCPURegister, 7, tblCPUFBitfields),
   new CRegisterData(VECTOR_NMI, "NMI Vector", nesGetCPURegister, nesSetCPURegister, 1, tblCPUNMIVectorBitfields),
   new CRegisterData(VECTOR_RESET, "RESET Vector", nesGetCPURegister, nesSetCPURegister, 1, tblCPURESETVectorBitfields),
   new CRegisterData(VECTOR_IRQ, "IRQ Vector", nesGetCPURegister, nesSetCPURegister, 1, tblCPUIRQVectorBitfields)
};

static const char* tbl6502RowHeadings [] =
{
   "CPU"
};

static const char* tbl6502ColumnHeadings [] =
{
   "PC","A","X","Y","SP","F","NMI","RESET","IRQ"
};

static CRegisterDatabase* m_db6502Registers = new CRegisterDatabase(eMemory_CPUregs,1,9,9,tblCPURegisters,tbl6502RowHeadings,tbl6502ColumnHeadings);

static CMemoryDatabase* m_db6502Memory = new CMemoryDatabase(eMemory_CPU,
                                                       0x0000,
                                                       MEM_2KB,
                                                       16,
                                                       "CPU RAM",
                                                       nesGetCPUMemory,
                                                       nesSetCPUMemory,
                                                       nesGetPrintableAddress,
                                                       true);

// CPU Event breakpoints
static bool cpuAlwaysFireEvent(BreakpointInfo* pBreakpoint,int data)
{
   // This breakpoint is checked in the right place
   // so if this breakpoint is enabled it should always fire when called.
   return true;
}

static bool cpuUndocumentedExactEvent(BreakpointInfo* pBreakpoint,int data)
{
   // If opcode executing is one specified, break...
   if ( pBreakpoint->item1 == data )
   {
      return true;
   }

   return false;
}

static bool cpuExecuteExactEvent(BreakpointInfo* pBreakpoint,int data)
{
   // If opcode executing is one specified, break...
   if ( pBreakpoint->item1 == data )
   {
      return true;
   }

   return false;
}

static CBreakpointEventInfo* m_db6502BreakpointEvents [] =
{
   new CBreakpointEventInfo("Specific Instruction Execution", cpuExecuteExactEvent, 1, "Break if opcode %02X is executed", 16, "Opcode:"),
   new CBreakpointEventInfo("Any Undocumented Instruction Execution", cpuAlwaysFireEvent, 0, "Break if any undocumented opcode is executed", 16),
   new CBreakpointEventInfo("Specific Undocumented Instruction Execution", cpuUndocumentedExactEvent, 1, "Break if undocumented opcode %02X is executed", 16, "Opcode:"),
   new CBreakpointEventInfo("Reset", cpuAlwaysFireEvent, 0, "Break if CPU is reset", 10),
   new CBreakpointEventInfo("IRQ Fires", cpuAlwaysFireEvent, 0, "Break if CPU IRQ fires", 10),
   new CBreakpointEventInfo("IRQ Handler Entered", cpuAlwaysFireEvent, 0, "Break if CPU IRQ handler entered", 10),
   new CBreakpointEventInfo("NMI Fires", cpuAlwaysFireEvent, 0, "Break if CPU NMI fires", 10),
   new CBreakpointEventInfo("NMI Handler Entered", cpuAlwaysFireEvent, 0, "Break if CPU NMI handler entered", 10),
   new CBreakpointEventInfo("Cycle Stolen by PPU or APU", cpuAlwaysFireEvent, 0, "Break if PPU or APU steals a CPU cycle", 10)
};

// APU Registers
static CBitfieldData* tblAPUSQCTRLBitfields [] =
{
   new CBitfieldData("Duty Cycle", 6, 2, "%X", 4, "25%", "50%", "75%", "12.5%"),
   new CBitfieldData("Channel State", 5, 1, "%X", 2, "Running", "Halted"),
   new CBitfieldData("Envelope Disabled", 4, 1, "%X", 2, "No", "Yes"),
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
   new CBitfieldData("Length", 3, 5, "%X", 32, "0A","FE","14","02","28","04","50","06","A0","08","3C","0A","0E","0C","1A","0E","0C","10","18","12","30","14","60","16","C0","18","48","1A","10","1C","20","1E"),
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
   new CBitfieldData("Envelope Disabled", 4, 1, "%X", 2, "No", "Yes"),
   new CBitfieldData("Volume/Envelope", 0, 4, "%X", 0)
};

static CBitfieldData* tblAPUNZPERIODBitfields [] =
{
   new CBitfieldData("Mode", 7, 1, "%X", 2, "32,767 samples", "93 samples"),
   new CBitfieldData("Period", 0, 4, "%X", 16, "004","008","010","020","040","060","080","0A0","0CA","0FE","17C","1FC","2FA","3F8","7F2","FE4")
};

static CBitfieldData* tblAPUNZLENBitfields [] =
{
   new CBitfieldData("Length", 3, 5, "%X", 32, "0A","FE","14","02","28","04","50","06","A0","08","3C","0A","0E","0C","1A","0E","0C","10","18","12","30","14","60","16","C0","18","48","1A","10","1C","20","1E")
};

static CBitfieldData* tblAPUDMCCTRLBitfields [] =
{
   new CBitfieldData("IRQ Enabled", 7, 1, "%X", 2, "No", "Yes"),
   new CBitfieldData("Loop", 6, 1, "%X", 2, "No", "Yes"),
   new CBitfieldData("Period", 0, 4, "%X", 16, "1AC","17C","154","140","11E","0FE","0E2","0D6","0BE","0A0","08E","080","06A","054","048","036")
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
   new CRegisterData(0x4000, "Square1 Control", nesGetAPURegister, nesSetAPURegister, 4, tblAPUSQCTRLBitfields),
   new CRegisterData(0x4001, "Square1 Sweep", nesGetAPURegister, nesSetAPURegister, 4, tblAPUSQSWEEPBitfields),
   new CRegisterData(0x4002, "Square1 Period LSB", nesGetAPURegister, nesSetAPURegister, 1, tblAPUPERIODLOBitfields),
   new CRegisterData(0x4003, "Square1 Length", nesGetAPURegister, nesSetAPURegister, 2, tblAPUPERIODLENBitfields),
   new CRegisterData(0x4004, "Square2 Control", nesGetAPURegister, nesSetAPURegister, 4, tblAPUSQCTRLBitfields),
   new CRegisterData(0x4005, "Square2 Sweep", nesGetAPURegister, nesSetAPURegister, 4, tblAPUSQSWEEPBitfields),
   new CRegisterData(0x4006, "Square2 Period LSB", nesGetAPURegister, nesSetAPURegister, 1, tblAPUPERIODLOBitfields),
   new CRegisterData(0x4007, "Square2 Length", nesGetAPURegister, nesSetAPURegister, 2, tblAPUPERIODLENBitfields),
   new CRegisterData(0x4008, "Triangle Control", nesGetAPURegister, nesSetAPURegister, 2, tblAPUTRICTRLBitfields),
   new CRegisterData(0x4009, "Unused", nesGetAPURegister, nesSetAPURegister, 1, tblAPUDMZBitfields),
   new CRegisterData(0x400A, "Triangle Period LSB", nesGetAPURegister, nesSetAPURegister, 1, tblAPUPERIODLOBitfields),
   new CRegisterData(0x400B, "Triangle Length", nesGetAPURegister, nesSetAPURegister, 2, tblAPUPERIODLENBitfields),
   new CRegisterData(0x400C, "Noise Control", nesGetAPURegister, nesSetAPURegister, 3, tblAPUNZCTRLBitfields),
   new CRegisterData(0x400D, "Unused", nesGetAPURegister, nesSetAPURegister, 1, tblAPUDMZBitfields),
   new CRegisterData(0x400E, "Noise Period", nesGetAPURegister, nesSetAPURegister, 2, tblAPUNZPERIODBitfields),
   new CRegisterData(0x400F, "Noise Length", nesGetAPURegister, nesSetAPURegister, 1, tblAPUNZLENBitfields),
   new CRegisterData(0x4010, "DMC Control", nesGetAPURegister, nesSetAPURegister, 3, tblAPUDMCCTRLBitfields),
   new CRegisterData(0x4011, "DMC DAC", nesGetAPURegister, nesSetAPURegister, 1, tblAPUDMCVOLBitfields),
   new CRegisterData(0x4012, "DMC Address", nesGetAPURegister, nesSetAPURegister, 1, tblAPUDMCADDRBitfields),
   new CRegisterData(0x4013, "DMC Length", nesGetAPURegister, nesSetAPURegister, 1, tblAPUDMCLENBitfields),
   new CRegisterData(0x4014, "Unused", nesGetAPURegister, nesSetAPURegister, 1, tblAPUDMZBitfields),
   new CRegisterData(0x4015, "APU Control", nesGetAPURegister, nesSetAPURegister, 7, tblAPUCTRLBitfields),
   new CRegisterData(0x4016, "Unused", nesGetAPURegister, nesSetAPURegister, 1, tblAPUDMZBitfields),
   new CRegisterData(0x4017, "APU Mask", nesGetAPURegister, nesSetAPURegister, 2, tblAPUMASKBitfields),
};

static const char* tblApuRowHeadings [] =
{
   "4000","4004","4008","400C","4010","4014"
};

static const char* tblApuColumnHeadings [] =
{
   "x0","x1","x2","x3"
};

static CRegisterDatabase* m_dbApuRegisters = new CRegisterDatabase(eMemory_IOregs,6,4,24,tblAPURegisters,tblApuRowHeadings,tblApuColumnHeadings);

// APU Event breakpoints
static bool apuIRQEvent(BreakpointInfo* pBreakpoint,int data)
{
   // This breakpoint is checked in the right place
   // so if this breakpoint is enabled it should always fire when called.
   return true;
}

static bool apuLengthCounterClockedEvent(BreakpointInfo* pBreakpoint,int data)
{
   // This breakpoint is checked in the right place
   // so if this breakpoint is enabled it should always fire when called.
   return true;
}

static bool apuDMAEvent(BreakpointInfo* pBreakpoint,int data)
{
   // This breakpoint is checked in the right place
   // so if this breakpoint is enabled it should always fire when called.
   return true;
}

static bool apuDACEvent(BreakpointInfo* pBreakpoint,int data)
{
   if ( data == pBreakpoint->item1 )
      return true;
   return false;
}

static CBreakpointEventInfo* m_dbApuBreakpointEvents [] =
{
   new CBreakpointEventInfo("IRQ", apuIRQEvent, 0, "Break if APU asserts IRQ", 10),
   new CBreakpointEventInfo("DMC channel DMA", apuDMAEvent, 0, "Break if APU DMC channel DMA occurs", 10),
   new CBreakpointEventInfo("Length Counter Clocked", apuLengthCounterClockedEvent, 0, "Break if APU sequencer clocks Length Counter", 10),
   new CBreakpointEventInfo("Square 1 DAC value", apuDACEvent, 1, "Break if APU square 1 channel DAC equals %d",10,"DAC value"),
   new CBreakpointEventInfo("Square 2 DAC value", apuDACEvent, 1, "Break if APU square 2 channel DAC equals %d",10,"DAC value"),
   new CBreakpointEventInfo("Triangle DAC value", apuDACEvent, 1, "Break if APU triangle channel DAC equals %d",10,"DAC value"),
   new CBreakpointEventInfo("Noise DAC value", apuDACEvent, 1, "Break if APU noise channel DAC equals %d",10,"DAC value"),
   new CBreakpointEventInfo("DMC DAC value", apuDACEvent, 1, "Break if APU DMC channel DAC equals %d",10,"DAC value")
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

static const char* tblPpuRowHeadings [] =
{
   "2000"
};

static const char* tblPpuColumnHeadings [] =
{
   "x0","x1","x2","x3","x4","x5","x6","x7"
};

static CRegisterDatabase* m_dbPpuRegisters = new CRegisterDatabase(eMemory_PPUregs,1,8,8,tblPPURegisters,tblPpuRowHeadings,tblPpuColumnHeadings);

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

static const char* tblOamRowHeadings [] =
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

static const char* tblOamColumnHeadings [] =
{
   "x0","x1","x2","x3"
};

static CRegisterDatabase* m_dbOamRegisters = new CRegisterDatabase(eMemory_PPUoam,64,4,256,tblOAMRegisters,tblOamRowHeadings,tblOamColumnHeadings);

static CMemoryDatabase* m_dbPaletteMemory = new CMemoryDatabase(eMemory_PPUpalette,
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

static CMemoryDatabase* m_dbNameTableMemory = new CMemoryDatabase(eMemory_PPU,
                                                                0x2000,
                                                                MEM_2KB,
                                                                32,
                                                                "NameTable",
                                                                nesGetPPUMemory,
                                                                nesSetPPUMemory,
                                                                nesGetPrintableAddress,
                                                                true);

// PPU Event breakpoints
static bool ppuAlwaysFireEvent(BreakpointInfo* pBreakpoint,int data)
{
   // This breakpoint is checked in the right place for each scanline
   // so if this breakpoint is enabled it should always fire when called.
   return true;
}

static bool ppuRasterPositionEvent(BreakpointInfo* pBreakpoint,int data)
{
   if ( (pBreakpoint->item1 == CNES::NES()->PPU()->_X()) &&
         (pBreakpoint->item2 == CNES::NES()->PPU()->_Y()) )
   {
      return true;
   }

   return false;
}

static bool ppuSpriteDmaEvent(BreakpointInfo* pBreakpoint,int data)
{
   if ( pBreakpoint->item1 == CNES::NES()->CPU()->WRITEDMAADDR() )
   {
      return true;
   }

   return false;
}

static bool ppuSpriteInMultiplexerEvent(BreakpointInfo* pBreakpoint,int data)
{
   if ( data == pBreakpoint->item1 )
   {
      return true;
   }

   return false;
}

static bool ppuSpriteSelectedEvent(BreakpointInfo* pBreakpoint,int data)
{
   if ( data == pBreakpoint->item1 )
   {
      return true;
   }

   return false;
}

static bool ppuSpriteRenderingEvent(BreakpointInfo* pBreakpoint,int data)
{
   if ( data == pBreakpoint->item1 )
   {
      return true;
   }

   return false;
}

static bool ppuAddressEqualsEvent(BreakpointInfo* pBreakpoint,int data)
{
   if ( data == pBreakpoint->item1 )
   {
      return true;
   }

   return false;
}

static CBreakpointEventInfo* m_dbPpuBreakpointEvents [] =
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

// Mapper Event breakpoints
static bool mapperIRQEvent(BreakpointInfo* pBreakpoint,int data)
{
   // This breakpoint is checked in the right place for each scanline
   // so if this breakpoint is enabled it should always fire when called.
   return true;
}

static CBreakpointEventInfo* m_dbCartBreakpointEvents [] =
{
   new CBreakpointEventInfo("IRQ", mapperIRQEvent, 0, "Break if mapper asserts IRQ", 10),
};

// This will be set by a cartridge mapper that has registers
CRegisterDatabase*  m_dbCartRegisters = NULL;

static CMemoryDatabase* m_dbCartSRAMMemory = new CMemoryDatabase(eMemory_cartSRAM,
                                                           SRAM_START,
                                                           MEM_8KB,
                                                           16,
                                                           "Cartridge SRAM Memory",
                                                           nesGetSRAMDataVirtual,
                                                           nesSetSRAMDataVirtual,
                                                           nesGetPrintableAddress,
                                                           true);

static CMemoryDatabase* m_dbCartEXRAMMemory = new CMemoryDatabase(eMemory_cartEXRAM,
                                                            EXRAM_START,
                                                            MEM_1KB,
                                                            16,
                                                            "Cartridge EXRAM Memory",
                                                            nesGetEXRAMData,
                                                            nesSetEXRAMData,
                                                            nesGetPrintableAddress,
                                                            true);

static CMemoryDatabase* m_dbCartVRAMMemory = new CMemoryDatabase(eMemory_cartVRAM,
                                                            VRAM_START,
                                                            MEM_16KB,
                                                            16,
                                                            "Cartridge VRAM Memory",
                                                            nesGetVRAMData,
                                                            nesSetVRAMData,
                                                            nesGetPrintableAddress,
                                                            true);

static bool returnFalse() { return false; }

static CMemoryDatabase* m_dbCartPRGROMMemory = new CMemoryDatabase(eMemory_cartROM,
                                                             MEM_32KB,
                                                             MEM_32KB,
                                                             16,
                                                             "Cartridge PRGROM Memory",
                                                             nesGetPRGROMData,
                                                             NULL,
                                                             nesGetPrintableAddress,
                                                             true,
                                                             NULL,
                                                             NULL,
                                                             NULL,
                                                             returnFalse);

static CMemoryDatabase* m_dbCartCHRMemory = new CMemoryDatabase(eMemory_cartCHRMEM,
                                                          0,
                                                          MEM_8KB,
                                                          16,
                                                          "Cartridge CHR Memory",
                                                          nesGetCHRMEMData,
                                                          nesSetCHRMEMData,
                                                          nesGetPrintableAddress,
                                                          true,
                                                          NULL,
                                                          NULL,
                                                          NULL,
                                                          nesIsCHRRAM);

static char __emu_version__ [] = "v2.0.0"
#if defined ( QT_NO_DEBUG )
" RELEASE";
#else
" DEBUG";
#endif

char* nesGetVersion()
{
   return __emu_version__;
}
extern int32_t apuDataAvailable;

const char* hex_char = "0123456789ABCDEF";

bool __nesdebug = false;

void nesEnableDebug ( void )
{
   __nesdebug = true;
}

void nesDisableDebug ( void )
{
   __nesdebug = false;
}

void nesSetBreakOnKIL ( bool breakOnKIL )
{
   CNES::NES()->CPU()->BREAKONKIL(breakOnKIL);
}

static void (*breakpointHook)(void) = NULL;

void nesSetBreakpointHook ( void (*hook)(void) )
{
   breakpointHook = hook;
}

static void (*audioHook)(void) = NULL;

void nesSetAudioHook ( void (*hook)(void) )
{
   audioHook = hook;
}

void nesBreak ( void )
{
   if ( breakpointHook )
   {
      breakpointHook();
   }
}

void nesBreakAudio ( void )
{
   if ( audioHook )
   {
      audioHook();
   }
}

uint32_t nesGetNumColors ( void )
{
   return 64;
}

void nesSetSystemMode ( uint32_t mode )
{
   CNES::NES()->VIDEOMODE(mode);
}

uint32_t nesGetSystemMode ( void )
{
   return CNES::NES()->VIDEOMODE();
}

void nesGetPrintableAddress ( char* buffer, uint32_t virtAddr )
{
   CNES::NES()->PRINTABLEADDR(buffer,virtAddr);
}

void nesGetPrintablePhysicalAddress ( char* buffer, uint32_t virtAddr, uint32_t physAddr )
{
   CNES::NES()->PRINTABLEADDR(buffer,virtAddr,physAddr);
}

void nesSetControllerType ( int32_t port, int32_t type )
{
   CNES::NES()->CONTROLLER(port,type);
}

void nesSetControllerScreenPosition ( int32_t port, int32_t px, int32_t py, int32_t wx1, int32_t wy1, int32_t wx2, int32_t wy2 )
{
   CNES::NES()->CONTROLLERPOSITION(port,px,py,wx1,wy1,wx2,wy2);
}

void nesSetControllerSpecial ( int32_t port, int32_t special )
{
   // Note this function must be called after nesSetControllerType is
   // called otherwise the wrong controller type's special setting will
   // be modified.
   if ( iofunc[CNES::NES()->CONTROLLER(port)].special )
   {
      iofunc[CNES::NES()->CONTROLLER(port)].special(port,special);
   }
}

void nesEnableBreakpoints ( bool enable )
{
   CNES::NES()->BREAKPOINTS(enable);
}

void nesStepCpu ( void )
{
   CNES::NES()->STEPCPUBREAKPOINT();
}

void nesStepPpu ( void )
{
   CNES::NES()->STEPPPUBREAKPOINT();
}

void nesStepPpuFrame ( void )
{
   CNES::NES()->STEPPPUBREAKPOINT(true);
}

void nesResetInputRecording ( void )
{
   CIOStandardJoypad::LOGGER(0)->ClearSampleBuffer();
   CIOStandardJoypad::LOGGER(1)->ClearSampleBuffer();
}

void nesSetInputPlayback ( bool enable )
{
   CNES::NES()->REPLAY(enable);
}

void nesSetInputRecording ( bool enable )
{
   CNES::NES()->RECORD(enable);
}

uint32_t nesGetInputSamplesAvailable ( int32_t port )
{
   return CIOStandardJoypad::LOGGER(port)->GetNumSamples();
}

JoypadLoggerInfo* nesGetInputSample ( int32_t port, int sample )
{
   return CIOStandardJoypad::LOGGER(port)->GetSample(sample);
}

void nesSetInputSample ( int32_t port, JoypadLoggerInfo* sample )
{
   CIOStandardJoypad::LOGGER(port)->AddSample(sample->cycle,sample->data);
}

void nesGetInputSamples ( int32_t port, JoypadLoggerInfo** samples )
{
   (*samples) = CIOStandardJoypad::LOGGER(port)->GetSample(0);
}

CTracer* nesGetExecutionTracerDatabase ( void )
{
   return CNES::NES()->TRACER();
}

CMarker* nesGetExecutionMarkerDatabase ( void )
{
   return CNES::NES()->CPU()->MARKERS();
}

void nesClearCodeDataLoggerDatabases ( void )
{
   unsigned int addr;

   CNES::NES()->CPU()->LOGGER()->ClearData();

   for ( addr = 0; addr < nesGetPRGROMSize(); addr += MEM_8KB )
   {
      CNES::NES()->CART()->LOGGERPHYS(addr)->ClearData();
   }
}

CCodeDataLogger* nesGetCpuCodeDataLoggerDatabase ( void )
{
   return CNES::NES()->CPU()->LOGGER();
}

CCodeDataLogger* nesGetVirtualPRGROMCodeDataLoggerDatabase ( uint32_t addr )
{
   return CNES::NES()->CART()->LOGGERVIRT(addr);
}

CCodeDataLogger* nesGetPhysicalPRGROMCodeDataLoggerDatabase ( uint32_t addr )
{
   return CNES::NES()->CART()->LOGGERPHYS(addr);
}

CCodeDataLogger* nesGetEXRAMCodeDataLoggerDatabase ( void )
{
   return CNES::NES()->CART()->EXRAMLOGGER();
}

CCodeDataLogger* nesGetVirtualSRAMCodeDataLoggerDatabase ( uint32_t addr )
{
   return CNES::NES()->CART()->SRAMLOGGERVIRT(addr);
}

CCodeDataLogger* nesGetPhysicalSRAMCodeDataLoggerDatabase ( uint32_t addr )
{
   return CNES::NES()->CART()->SRAMLOGGERPHYS(addr);
}

CCodeDataLogger* nesGetPpuCodeDataLoggerDatabase ( void )
{
   return CNES::NES()->PPU()->LOGGER();
}

CBreakpointInfo* nesGetBreakpointDatabase ( void )
{
   return CNES::NES()->BREAKPOINTS();
}

CRegisterDatabase* nesGetCpuRegisterDatabase ( void )
{
   return m_db6502Registers;
}

CRegisterDatabase* nesGetPpuRegisterDatabase ( void )
{
   return m_dbPpuRegisters;
}

CRegisterDatabase* nesGetPpuOamRegisterDatabase ( void )
{
   return m_dbOamRegisters;
}

CRegisterDatabase* nesGetApuRegisterDatabase ( void )
{
   return m_dbApuRegisters;
}

CRegisterDatabase* nesGetCartridgeRegisterDatabase ( void )
{
   return m_dbCartRegisters;
}

CMemoryDatabase* nesGetCpuMemoryDatabase ( void )
{
   return m_db6502Memory;
}

CMemoryDatabase* nesGetPpuNameTableMemoryDatabase ( void )
{
   return m_dbNameTableMemory;
}

CMemoryDatabase* nesGetPpuPaletteMemoryDatabase ( void )
{
   return m_dbPaletteMemory;
}

CMemoryDatabase* nesGetCartridgeEXRAMMemoryDatabase ( void )
{
   return m_dbCartEXRAMMemory;
}

CMemoryDatabase* nesGetCartridgeSRAMMemoryDatabase ( void )
{
   return m_dbCartSRAMMemory;
}

CMemoryDatabase* nesGetCartridgePRGROMMemoryDatabase ( void )
{
   return m_dbCartPRGROMMemory;
}

CMemoryDatabase* nesGetCartridgeCHRMemoryDatabase ( void )
{
   return m_dbCartCHRMemory;
}

CMemoryDatabase* nesGetCartridgeVRAMMemoryDatabase ( void )
{
   return m_dbCartVRAMMemory;
}

int32_t nesGetSizeOfCpuBreakpointEventDatabase ( void )
{
   return NUM_CPU_EVENTS;
}

CBreakpointEventInfo** nesGetCpuBreakpointEventDatabase ( void )
{
   return m_db6502BreakpointEvents;
}

int32_t nesGetSizeOfPpuBreakpointEventDatabase ( void )
{
   return NUM_PPU_EVENTS;
}

CBreakpointEventInfo** nesGetPpuBreakpointEventDatabase ( void )
{
   return m_dbPpuBreakpointEvents;
}

int32_t nesGetSizeOfApuBreakpointEventDatabase ( void )
{
   return NUM_APU_EVENTS;
}

CBreakpointEventInfo** nesGetApuBreakpointEventDatabase ( void )
{
   return m_dbApuBreakpointEvents;
}

int32_t nesGetSizeOfCartridgeBreakpointEventDatabase ( void )
{
   return NUM_MAPPER_EVENTS;
}

CBreakpointEventInfo** nesGetCartridgeBreakpointEventDatabase ( void )
{
   return m_dbCartBreakpointEvents;
}

void nesDisassemble ()
{
   CNES::NES()->DISASSEMBLE();
}

void nesDisassembleSingle ( uint8_t* pOpcode, char* buffer )
{
   DISASSEMBLE(pOpcode,buffer);
}

char* nesGetDisassemblyAtAddress ( uint32_t addr )
{
   return CNES::NES()->DISASSEMBLY(addr);
}

void nesGetDisassemblyAtPhysicalAddress ( uint32_t physAddr, char* buffer )
{
   CNES::NES()->CART()->PRGROMDISASSEMBLYATPHYSADDR(physAddr,buffer);
}

uint32_t nesGetVirtualAddressFromSLOC ( uint16_t sloc )
{
   return CNES::NES()->SLOC2VIRTADDR(sloc);
}

uint16_t nesGetSLOCFromAddress ( uint32_t addr )
{
   return CNES::NES()->ADDR2SLOC(addr);
}

uint32_t nesGetSLOC ( uint32_t addr )
{
   return CNES::NES()->SLOC(addr);
}

uint32_t nesGetPhysicalAddressFromAddress ( uint32_t addr )
{
   return CNES::NES()->PHYSADDR(addr);
}

void nesClearOpcodeMasks ( void )
{
   CNES::NES()->CPU()->OPCODEMASKCLR ();
   CNES::NES()->CART()->PRGROMOPCODEMASKCLR ();
   CNES::NES()->CART()->SRAMOPCODEMASKCLR ();
   CNES::NES()->CART()->EXRAMOPCODEMASKCLR ();
}

void nesSetOpcodeMask ( uint32_t addr, uint8_t mask )
{
   CNES::NES()->CART()->PRGROMOPCODEMASKATPHYSADDR(addr, mask);
}

void nesSetTVOut ( int8_t* tv )
{
   CNES::NES()->PPU()->TV ( tv );
}

void nesFrontload ( uint32_t mapper )
{
   CNES::NES()->FRONTLOAD(mapper);
}

void nesLoadPRGROMBank ( uint32_t bank, uint8_t* bankData )
{
   CNES::NES()->CART()->SetPRGBank ( bank, bankData );
}

void nesLoadCHRROMBank ( uint32_t bank, uint8_t* bankData )
{
   CNES::NES()->CART()->SetCHRBank ( bank, bankData );
}

void nesFinalizeLoad ( void )
{
   CNES::NES()->CART()->DoneLoadingBanks();
}

bool nesROMIsLoaded ( void )
{
   return ( CNES::NES()->CART()?true:false );
}

void nesSetHorizontalMirroring ( void )
{
   CNES::NES()->PPU()->MIRRORHORIZ();
}

void nesSetVerticalMirroring ( void )
{
   CNES::NES()->PPU()->MIRRORVERT();
}

void nesSetFourScreen ( void )
{
   CNES::NES()->PPU()->MIRROR(-1,true);
}

void nesReset ( bool soft )
{
   CNES::NES()->RESET(soft);
}

void nesResetInitial ()
{
   CNES::NES()->RESET(false);
}

void nesRun ( uint32_t* joypads )
{
   CNES::NES()->RUN(joypads);
}

uint8_t* nesGetAudioSamples ( uint16_t samples )
{
   return CNES::NES()->CPU()->APU()->PLAY(samples);
}

int32_t nesGetAudioSamplesAvailable ( void )
{
   return apuDataAvailable;
}

void nesClearAudioSamplesAvailable ( void )
{
   apuDataAvailable = 0;
}

uint32_t nesGetCPUCycle ( void )
{
   return CNES::NES()->CPU()->_CYCLES();
}

void nesSetGotoAddress ( uint32_t addr )
{
   if ( addr == 0xFFFFFFFF )
   {
      CNES::NES()->CPU()->GOTO ();
   }
   else
   {
      CNES::NES()->CPU()->GOTO ( addr );
   }
}

bool nesCPUIsFetchingOpcode ( void )
{
   return CNES::NES()->CPU()->_SYNC();
}

bool nesCPUIsWritingMemory ( void )
{
   return CNES::NES()->CPU()->_WRITING();
}

int8_t* nesGetTVOut ( void )
{
   return CNES::NES()->PPU()->TV();
}

#include "cnesrommapper005.h"
void nesSetMMC5AudioChannelMask ( uint32_t mask )
{
   CROMMapper005::SOUNDENABLE(mask);
}

#include "cnesrommapper024.h"
void nesSetVRC6AudioChannelMask ( uint32_t mask )
{
   CROMMapper024::SOUNDENABLE(mask);
}

#include "cnesrommapper019.h"
void nesSetN106AudioChannelMask ( uint32_t mask )
{
   CROMMapper019::SOUNDENABLE(mask);
}

void nesSetAudioChannelMask ( uint8_t mask )
{
   CNES::NES()->CPU()->APU()->MUTE(mask);
}

uint32_t nesGetCPUEffectiveAddress ( void )
{
   return CNES::NES()->CPU()->_EA();
}

uint32_t nesGetCPUProgramCounter( void )
{
   return CNES::NES()->CPU()->__PC();
}

uint32_t nesGetCPUProgramCounterOfLastSync( void )
{
   return CNES::NES()->CPU()->__PCSYNC();
}

void nesSetCPURegister ( uint32_t addr, uint32_t data )
{
   switch ( addr )
   {
   case CPU_PC:
      CNES::NES()->CPU()->__PC(data);
      break;
   case CPU_SP:
      CNES::NES()->CPU()->_SP(data);
      break;
   case CPU_A:
      CNES::NES()->CPU()->_A(data);
      break;
   case CPU_X:
      CNES::NES()->CPU()->_X(data);
      break;
   case CPU_Y:
      CNES::NES()->CPU()->_Y(data);
      break;
   case CPU_F:
      CNES::NES()->CPU()->_F(data);
      break;
   case VECTOR_IRQ:
      // Can't chage.
      break;
   case VECTOR_RESET:
      // Can't chage.
      break;
   case VECTOR_NMI:
      // Can't chage.
      break;
   }
}

uint32_t nesGetCPURegister( uint32_t addr )
{
   switch ( addr )
   {
   case CPU_PC:
      return CNES::NES()->CPU()->__PC();
      break;
   case CPU_SP:
      return 0x100|CNES::NES()->CPU()->_SP();
      break;
   case CPU_A:
      return CNES::NES()->CPU()->_A();
      break;
   case CPU_X:
      return CNES::NES()->CPU()->_X();
      break;
   case CPU_Y:
      return CNES::NES()->CPU()->_Y();
      break;
   case CPU_F:
      return CNES::NES()->CPU()->_F();
      break;
   case VECTOR_IRQ:
      return MAKE16(CNES::NES()->CPU()->_MEM(VECTOR_IRQ),CNES::NES()->CPU()->_MEM(VECTOR_IRQ+1));
      break;
   case VECTOR_RESET:
      return MAKE16(CNES::NES()->CPU()->_MEM(VECTOR_RESET),CNES::NES()->CPU()->_MEM(VECTOR_RESET+1));
      break;
   case VECTOR_NMI:
      return MAKE16(CNES::NES()->CPU()->_MEM(VECTOR_NMI),CNES::NES()->CPU()->_MEM(VECTOR_NMI+1));
      break;
   }
   return 0;
}

uint32_t nesGetCPUFlagNegative ( void )
{
   return CNES::NES()->CPU()->_N();
}

uint32_t nesGetCPUFlagOverflow ( void )
{
   return CNES::NES()->CPU()->_V();
}

uint32_t nesGetCPUFlagBreak ( void )
{
   return CNES::NES()->CPU()->_B();
}

uint32_t nesGetCPUFlagDecimal ( void )
{
   return CNES::NES()->CPU()->_D();
}

uint32_t nesGetCPUFlagInterrupt ( void )
{
   return CNES::NES()->CPU()->_I();
}

uint32_t nesGetCPUFlagZero ( void )
{
   return CNES::NES()->CPU()->_Z();
}

uint32_t nesGetCPUFlagCarry ( void )
{
   return CNES::NES()->CPU()->_C();
}

void nesSetCPUFlagNegative ( uint32_t set )
{
   CNES::NES()->CPU()->_N(set);
}

void nesSetCPUFlagOverflow ( uint32_t set )
{
   CNES::NES()->CPU()->_V(set);
}

void nesSetCPUFlagBreak ( uint32_t set )
{
   CNES::NES()->CPU()->_B(set);
}

void nesSetCPUFlagDecimal ( uint32_t set )
{
   CNES::NES()->CPU()->_D(set);
}

void nesSetCPUFlagInterrupt ( uint32_t set )
{
   CNES::NES()->CPU()->_I(set);
}

void nesSetCPUFlagZero ( uint32_t set )
{
   CNES::NES()->CPU()->_Z(set);
}

void nesSetCPUFlagCarry ( uint32_t set )
{
   CNES::NES()->CPU()->_C(set);
}

uint32_t nesGetPPUMemory ( uint32_t addr )
{
   return CNES::NES()->PPU()->_MEM(addr);
}

void nesSetPPUMemory ( uint32_t addr, uint32_t data )
{
   CNES::NES()->PPU()->_MEM(addr,data);
}

uint32_t nesGetCPUMemory ( uint32_t addr )
{
   return CNES::NES()->CPU()->_MEM(addr);
}

void nesSetCPUMemory ( uint32_t addr, uint32_t data )
{
   CNES::NES()->CPU()->_MEM(addr,data);
}

uint8_t nesGetMemory ( uint32_t addr )
{
   return CNES::NES()->_MEM(addr);
}

uint32_t nesGetPPUCycle ( void )
{
   return CNES::NES()->PPU()->_CYCLES();
}

uint32_t nesGetPPURegister ( uint32_t addr )
{
   return CNES::NES()->PPU()->_PPU(addr);
}

void nesSetPPURegister ( uint32_t addr, uint32_t data )
{
   CNES::NES()->PPU()->_PPU(addr,data);
}

uint16_t nesGetScrollXAtXY ( int32_t x, int32_t y )
{
   return CNES::NES()->PPU()->_SCROLLX(x,y);
}

uint16_t nesGetScrollYAtXY ( int32_t x, int32_t y )
{
   return CNES::NES()->PPU()->_SCROLLY(x,y);
}

void nesGetCurrentScroll ( uint8_t* x, uint8_t* y )
{
   return CNES::NES()->PPU()->_SCROLL(x,y);
}

void nesGetLastSprite0Hit ( uint8_t* x, uint8_t* y )
{
   (*x) = CNES::NES()->PPU()->_SPRITE0HITX();
   (*y) = CNES::NES()->PPU()->_SPRITE0HITY();
}

void nesGetCurrentPixel ( uint8_t* x, uint8_t* y )
{
   (*x) = CNES::NES()->PPU()->_X();
   (*y) = CNES::NES()->PPU()->_Y();
}

void nesGetMirroring ( uint16_t* sc1, uint16_t* sc2, uint16_t* sc3, uint16_t* sc4 )
{
   CNES::NES()->PPU()->_MIRROR(sc1,sc2,sc3,sc4);
}

uint32_t nesGetAPUCycle ( void )
{
   return CNES::NES()->CPU()->APU()->CYCLES();
}

uint32_t nesGetAPURegister ( uint32_t addr )
{
   return CNES::NES()->CPU()->APU()->_APU(addr);
}

void nesSetAPURegister ( uint32_t addr, uint32_t data )
{
   CNES::NES()->CPU()->APU()->_APU(addr,data);
}

int32_t nesGetAPUSequencerMode ( void )
{
   return CNES::NES()->CPU()->APU()->SEQUENCERMODE();
}

void nesGetAPULengthCounters( uint16_t* sq1,
                              uint16_t* sq2,
                              uint16_t* triangle,
                              uint16_t* noise,
                              uint16_t* dmc)
{
   CNES::NES()->CPU()->APU()->LENGTHCOUNTERS(sq1,sq2,triangle,noise,dmc);
}

void nesGetAPUTriangleLinearCounter ( uint8_t* triangle )
{
   CNES::NES()->CPU()->APU()->LINEARCOUNTER(triangle);
}

void nesGetAPUDACs ( uint8_t* sq1,
                     uint8_t* sq2,
                     uint8_t* triangle,
                     uint8_t* noise,
                     uint8_t* dmc )
{
   CNES::NES()->CPU()->APU()->GETDACS(sq1,sq2,triangle,noise,dmc);
}

void nesGetAPUDMCIRQ ( bool* enabled,
                       bool* asserted )
{
   CNES::NES()->CPU()->APU()->DMCIRQ(enabled,asserted);
}

void nesGetAPUDMCSampleInfo ( uint16_t* addr,
                              uint16_t* length,
                              uint16_t* pos )
{
   CNES::NES()->CPU()->APU()->SAMPLEINFO(addr,length,pos);
}

void nesGetAPUDMCDMAInfo ( uint8_t* buffer,
                           bool* full )
{
   CNES::NES()->CPU()->APU()->DMAINFO(buffer,full);
}

uint32_t nesGetPRGROMSize ( void )
{
   return CNES::NES()->CART()->NUMPRGROMBANKS()*MEM_8KB;
}

uint32_t nesGetNumPRGROMBanks ( void )
{
   return CNES::NES()->CART()->NUMPRGROMBANKS();
}

uint32_t nesGetNumCHRROMBanks ( void )
{
   return CNES::NES()->CART()->NUMCHRROMBANKS();
}

bool nesIsCHRRAM ( void )
{
   return !CNES::NES()->CART()->IsWriteProtected();
}

uint8_t nesGetMapper ( void )
{
   return CNES::NES()->CART()->MAPPER();
}

uint32_t nesMapperLowRead ( uint32_t addr )
{
   return CNES::NES()->CART()->DEBUGINFO(addr);
}

void nesMapperLowWrite ( uint32_t addr, uint32_t data )
{
   CNES::NES()->CART()->LMAPPER(addr,data);
}

uint32_t nesMapperHighRead ( uint32_t addr )
{
   return CNES::NES()->CART()->DEBUGINFO(addr);
}

void nesMapperHighWrite ( uint32_t addr, uint32_t data )
{
   CNES::NES()->CART()->HMAPPER(addr,data);
}

uint32_t nesGetPPUOAM ( uint32_t addr )
{
   return CNES::NES()->PPU()->_OAM(addr&3,addr>>2);
}

void nesSetPPUOAM ( uint32_t addr, uint32_t data )
{
   CNES::NES()->PPU()->_OAM(addr&3,addr>>2,data);
}

uint32_t nesGetPPUFrame ( void )
{
   return CNES::NES()->PPU()->_FRAME();
}

uint16_t nesGetPPUAddress ( void )
{
   return CNES::NES()->PPU()->_PPUADDR();
}

uint8_t nesGetPPUOAMAddress ( void )
{
   return CNES::NES()->PPU()->_OAMADDR();
}

uint8_t nesGetPPUReadLatch ( void )
{
   return CNES::NES()->PPU()->_PPUREADLATCH();
}

uint16_t nesGetPPUAddressLatch ( void )
{
   return CNES::NES()->PPU()->_PPUADDRLATCH();
}

int32_t nesGetPPUFlipFlop ( void )
{
   return CNES::NES()->PPU()->_PPUFLIPFLOP();
}

uint32_t nesGetPRGROMPhysicalAddress ( uint32_t addr )
{
   return CNES::NES()->CART()->PRGROMPHYSADDR(addr);
}

uint32_t nesGetCHRMEMPhysicalAddress ( uint32_t addr )
{
   return CNES::NES()->CART()->CHRMEMPHYSADDR(addr);
}

uint32_t nesGetPRGROMData ( uint32_t addr )
{
   return CNES::NES()->CART()->PRGROM(addr);
}

uint32_t nesGetCHRMEMData ( uint32_t addr )
{
   return CNES::NES()->CART()->CHRMEM(addr);
}

void nesSetCHRMEMData ( uint32_t addr, uint32_t data )
{
   CNES::NES()->CART()->CHRMEM(addr,data);
}

uint32_t nesGetSRAMPhysicalAddress ( uint32_t addr )
{
   return CNES::NES()->CART()->SRAMPHYSADDR(addr);
}

uint32_t nesGetSRAMDataVirtual ( uint32_t addr )
{
   return CNES::NES()->CART()->SRAMVIRT(addr);
}

void nesSetSRAMDataVirtual ( uint32_t addr, uint32_t data )
{
   CNES::NES()->CART()->SRAMVIRT(addr,data);
}

uint32_t nesGetSRAMDataPhysical ( uint32_t addr )
{
   return CNES::NES()->CART()->SRAMPHYS(addr);
}

void nesSetSRAMDataPhysical ( uint32_t addr, uint32_t data )
{
   CNES::NES()->CART()->SRAMPHYS(addr,data);
}

void nesLoadSRAMDataPhysical ( uint32_t addr, uint32_t data )
{
   CNES::NES()->CART()->SRAMPHYS(addr,data,false);
}

bool nesIsSRAMDirty ()
{
   return CNES::NES()->CART()->SRAMDIRTY();
}

uint32_t nesGetEXRAMPhysicalAddress ( uint32_t addr )
{
   return CNES::NES()->CART()->EXRAMPHYSADDR(addr);
}

uint32_t nesGetEXRAMData ( uint32_t addr )
{
   return CNES::NES()->CART()->EXRAM(addr);
}

void nesSetEXRAMData ( uint32_t addr, uint32_t data )
{
   CNES::NES()->CART()->EXRAM(addr,data);
}

uint32_t nesGetVRAMData ( uint32_t addr )
{
   return CNES::NES()->CART()->VRAM(addr);
}

void nesSetVRAMData ( uint32_t addr, uint32_t data )
{
   CNES::NES()->CART()->VRAM(addr,data);
}

bool nesMapperRemapsPRGROM ( void )
{
   return CNES::NES()->CART()->IsPRGRemappable();
}

bool nesMapperRemapsCHRMEM ( void )
{
   return CNES::NES()->CART()->IsCHRRemappable();
}

uint32_t nesMapperRemappedVMEMSize ( void )
{
   return CNES::NES()->CART()->VRAMMEMORY()->TOTALSIZE();
}

uint32_t nesGetPaletteRedComponent(uint32_t idx)
{
   return CBasePalette::GetPaletteR(idx)&0xFF;
}

uint32_t nesGetPaletteGreenComponent(uint32_t idx)
{
   return CBasePalette::GetPaletteG(idx)&0xFF;
}

uint32_t nesGetPaletteBlueComponent(uint32_t idx)
{
   return CBasePalette::GetPaletteB(idx)&0xFF;
}

void    nesSetPaletteRedComponent(uint32_t idx,uint32_t r)
{
   CBasePalette::SetPaletteR(idx,r&0xFF);
}

void    nesSetPaletteGreenComponent(uint32_t idx,uint32_t g)
{
   CBasePalette::SetPaletteG(idx,g&0xFF);
}

void    nesSetPaletteBlueComponent(uint32_t idx,uint32_t b)
{
   CBasePalette::SetPaletteB(idx,b&0xFF);
}

void nesMapper001GetInformation ( nesMapper001Info* pInfo )
{
   CROMMapper001* pCART = (CROMMapper001*)CNES::NES()->CART();
   pInfo->shiftRegister = pCART->SHIFTREGISTER();
   pInfo->shiftRegisterBit = pCART->SHIFTREGISTERBIT();
}

void nesMapper004GetInformation ( nesMapper004Info* pInfo )
{
   CROMMapper004* pCART = (CROMMapper004*)CNES::NES()->CART();
   pInfo->irqAsserted = pCART->IRQASSERTED();
   pInfo->irqCounter = pCART->IRQCOUNTER();
   pInfo->irqEnabled = pCART->IRQENABLED();
   pInfo->irqReload = pCART->IRQRELOAD();
   pInfo->irqLatch = pCART->IRQLATCH();
   pInfo->ppuAddrA12 = pCART->PPUADDRA12();
   pInfo->ppuCycle = pCART->PPUCYCLE();
}

void nesMapper005GetInformation ( nesMapper005Info* pInfo )
{
   CROMMapper005* pCART = (CROMMapper005*)CNES::NES()->CART();
   pInfo->irqAsserted = pCART->IRQASSERTED();
   pInfo->irqScanline = pCART->IRQSCANLINE();
   pInfo->irqEnabled = pCART->IRQENABLED();
   pInfo->sprite8x16Mode = pCART->SPRITEMODE();
   pInfo->ppuCycle = pCART->PPUCYCLE();
}

void nesMapper009GetInformation ( nesMapper009010Info* pInfo )
{
   CROMMapper009* pCART = (CROMMapper009*)CNES::NES()->CART();
   pInfo->latch0FD = pCART->LATCH0FD();
   pInfo->latch0FE = pCART->LATCH0FE();
   pInfo->latch1FD = pCART->LATCH1FD();
   pInfo->latch1FE = pCART->LATCH1FE();
   pInfo->latch0 = pCART->LATCH0();
   pInfo->latch1 = pCART->LATCH0();
}

void nesMapper010GetInformation ( nesMapper009010Info* pInfo )
{
   CROMMapper010* pCART = (CROMMapper010*)CNES::NES()->CART();
   pInfo->latch0FD = pCART->LATCH0FD();
   pInfo->latch0FE = pCART->LATCH0FE();
   pInfo->latch1FD = pCART->LATCH1FD();
   pInfo->latch1FE = pCART->LATCH1FE();
   pInfo->latch0 = pCART->LATCH0();
   pInfo->latch1 = pCART->LATCH0();
}

void nesMapper016GetInformation ( nesMapper016Info* pInfo )
{
   CROMMapper016* pCART = (CROMMapper016*)CNES::NES()->CART();
   pInfo->irqAsserted = pCART->IRQASSERTED();
   pInfo->irqCounter = pCART->IRQCOUNTER();
   pInfo->irqEnabled = pCART->IRQENABLED();
   pInfo->eepromState = pCART->EEPROMSTATE();
   pInfo->eepromBitCounter = pCART->EEPROMBITCOUNTER();
   pInfo->eepromCmd = pCART->EEPROMCMD();
   pInfo->eepromAddr = pCART->EEPROMADDR();
   pInfo->eepromDataBuf = pCART->EEPROMDATABUF();
}

void nesMapper028GetInformation ( nesMapper028Info* pInfo )
{
   CROMMapper028* pCART = (CROMMapper028*)CNES::NES()->CART();
   pInfo->regSel = pCART->REGSEL();
   pInfo->prgMode = pCART->PRGMODE();
   pInfo->prgSize = pCART->PRGSIZE();
   pInfo->prgOuterBank = pCART->PRGOUTERBANK();
   pInfo->prgInnerBank = pCART->PRGINNERBANK();
   pInfo->chrBank = pCART->CHRBANK();
}

void nesMapper069GetInformation ( nesMapper069Info* pInfo )
{
   CROMMapper069* pCART = (CROMMapper069*)CNES::NES()->CART();
   pInfo->irqAsserted = pCART->IRQASSERTED();
   pInfo->irqCounter = pCART->IRQCOUNTER();
   pInfo->irqEnabled = pCART->IRQENABLED();
   pInfo->irqCountEnabled = pCART->IRQCOUNTENABLED();
   pInfo->sramEnabled = pCART->SRAMENABLED();
   pInfo->sramIsSram = pCART->ISSRAM();
   pInfo->sramOrPrgBank = pCART->SRAMPRGBANK();
   pInfo->regSelected = pCART->REGSELECTED();
   pInfo->regValue = pCART->REGVALUE();
}

void nesGetCpuSnapshot(NESCpuStateSnapshot* pSnapshot)
{
   int idx;
   pSnapshot->cycle = CNES::NES()->CPU()->_CYCLES();
   pSnapshot->pc = CNES::NES()->CPU()->__PC();
   pSnapshot->pcAtSync = CNES::NES()->CPU()->__PCSYNC();
   pSnapshot->sp = CNES::NES()->CPU()->_SP();
   pSnapshot->a = CNES::NES()->CPU()->_A();
   pSnapshot->x = CNES::NES()->CPU()->_X();
   pSnapshot->y = CNES::NES()->CPU()->_Y();
   pSnapshot->f = CNES::NES()->CPU()->_F();
   for ( idx = 0; idx < MEM_2KB; idx++ )
   {
      *(pSnapshot->memory+idx) = CNES::NES()->CPU()->_MEM(idx);
   }
}

void nesGetPpuSnapshot(PpuStateSnapshot* pSnapshot)
{
   int idx;
   int x,y;
   pSnapshot->frame = CNES::NES()->PPU()->_FRAME();
   pSnapshot->cycle = CNES::NES()->PPU()->_CYCLES();
   for ( idx = 0; idx < NUM_PPU_REGS; idx++ )
   {
      *(pSnapshot->reg+idx) = CNES::NES()->PPU()->_PPU(idx);
   }
   for ( idx = 0; idx < MEM_256B; idx++ )
   {
      *(pSnapshot->oamMemory+idx) = CNES::NES()->PPU()->_OAM(idx&3,idx>>2);
   }
   for ( idx = 0; idx < MEM_32B; idx++ )
   {
      *(pSnapshot->paletteMemory+idx) = CNES::NES()->PPU()->_PALETTE(idx);
   }
   for ( idx = 0; idx < MEM_16KB; idx++ )
   {
      *(pSnapshot->memory+idx) = CNES::NES()->PPU()->_MEM(idx);
   }
   for ( y = 0; y < 240; y++ )
   {
      for ( x = 0; x < 256; x++ )
      {
         *(*(pSnapshot->xOffset+x)+y) = CNES::NES()->PPU()->_SCROLLX(x,y);
         *(*(pSnapshot->yOffset+x)+y) = CNES::NES()->PPU()->_SCROLLY(x,y);
      }
   }
}

void nesGetApuSnapshot(ApuStateSnapshot* pSnapshot)
{
   int idx;
   pSnapshot->cycle = CNES::NES()->CPU()->APU()->CYCLES();
   for ( idx = 0; idx < NUM_APU_REGS; idx++ )
   {
      *(pSnapshot->reg+idx) = CNES::NES()->CPU()->APU()->_APU(idx);
   }
   pSnapshot->sequencerMode = CNES::NES()->CPU()->APU()->SEQUENCERMODE();
   CNES::NES()->CPU()->APU()->LINEARCOUNTER(&pSnapshot->triangleLinearCounter);
   CNES::NES()->CPU()->APU()->LENGTHCOUNTERS(&pSnapshot->lengthCounter[0],
                        &pSnapshot->lengthCounter[1],
                        &pSnapshot->lengthCounter[2],
                        &pSnapshot->lengthCounter[3],
                        &pSnapshot->lengthCounter[4]);
   CNES::NES()->CPU()->APU()->GETDACS(&pSnapshot->dac[0],
                 &pSnapshot->dac[1],
                 &pSnapshot->dac[2],
                 &pSnapshot->dac[3],
                 &pSnapshot->dac[4]);
   CNES::NES()->CPU()->APU()->DMCIRQ(&pSnapshot->dmcIrqEnabled,&pSnapshot->dmcIrqAsserted);
   CNES::NES()->CPU()->APU()->SAMPLEINFO(&pSnapshot->dmaSampleAddress,&pSnapshot->dmaSampleLength,&pSnapshot->dmaSamplePosition);
   CNES::NES()->CPU()->APU()->DMAINFO(&pSnapshot->dmcDmaBuffer,&pSnapshot->dmcDmaFull);
}

void nesGetNesSnapshot(NesStateSnapshot *pSnapshot)
{
   nesGetCpuSnapshot(&pSnapshot->cpu);
   nesGetApuSnapshot(&pSnapshot->apu);
   nesGetPpuSnapshot(&pSnapshot->ppu);
}
