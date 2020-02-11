#include "c64_emulator_core.h"

#include "cc646502.h"
#include "cc64sid.h"

static char __emu_version__ [] = "v2.0.0"
#if defined ( QT_NO_DEBUG )
" RELEASE";
#else
" DEBUG";
#endif

char* c64GetVersion()
{
   return __emu_version__;
}

const char* hex_char = "0123456789ABCDEF";

bool __c64debug = false;

void c64EnableDebug ( void )
{
   __c64debug = true;
}

void c64DisableDebug ( void )
{
   __c64debug = false;
}

static void (*breakpointHook)(void) = NULL;

void c64SetBreakpointHook ( void (*hook)(void) )
{
   breakpointHook = hook;
}

void c64Break ( void )
{
   if ( breakpointHook )
   {
      breakpointHook();
   }
}

uint32_t c64GetNumColors ( void )
{
   return 16;
}

void c64GetPrintableAddress ( char* buffer, uint32_t addr )
{
   sprintf(buffer,"%04X",addr);
}

void c64GetPrintablePhysicalAddress ( char* buffer, uint32_t addr, uint32_t absAddr )
{
   sprintf(buffer,"%04X",addr);
}

void c64EnableBreakpoints ( bool enable )
{
}

void c64StepCpu ( void )
{
}

CBreakpointInfo* c64GetBreakpointDatabase ( void )
{
   return CC646502::BREAKPOINTS();
}

CRegisterDatabase* c64GetCpuRegisterDatabase()
{
   return CC646502::REGISTERS();
}

CMemoryDatabase* c64GetCpuMemoryDatabase()
{
   return CC646502::MEMORY();
}

void c64Disassemble ()
{
   CC646502::DISASSEMBLE();
}

void c64DisassembleSingle ( uint8_t* pOpcode, char* buffer )
{
   CC646502::Disassemble(pOpcode,buffer);
}

char* c64GetDisassemblyAtAddress ( uint32_t addr )
{
   return CC646502::DISASSEMBLY(addr);
}

void c64GetDisassemblyAtPhysicalAddress ( uint32_t absAddr, char* buffer )
{
   CC646502::DISASSEMBLYATPHYSADDR(absAddr,buffer);
}

uint32_t c64GetPhysicalAddressFromAddress ( uint32_t addr )
{
   return addr;
}

uint32_t c64GetAddressFromSLOC ( uint16_t sloc )
{
   return CC646502::SLOC2VIRTADDR(sloc);
}

uint16_t c64GetSLOCFromAddress ( uint32_t addr )
{
   return CC646502::ADDR2SLOC(addr);
}

uint32_t c64GetSLOC ( uint32_t addr )
{
   return CC646502::SLOC();
}

void c64ClearOpcodeMasks ( void )
{
   CC646502::OPCODEMASKCLR ();
}

void c64SetOpcodeMask ( uint32_t addr, uint8_t mask )
{
   CC646502::OPCODEMASK(addr, mask);
}

void c64SetGotoAddress ( uint32_t addr )
{
   CC646502::GOTO(addr);
}

uint32_t c64GetGotoAddress ( void )
{
   return CC646502::GOTO();
}

void c64SetCPURegister ( uint32_t addr, uint32_t data )
{
   switch ( addr )
   {
   case CPU_PC:
      CC646502::__PC(data);
      break;
   case CPU_SP:
      CC646502::_SP(data);
      break;
   case CPU_A:
      CC646502::_A(data);
      break;
   case CPU_X:
      CC646502::_X(data);
      break;
   case CPU_Y:
      CC646502::_Y(data);
      break;
   case CPU_F:
      CC646502::_F(data);
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

uint32_t c64GetCPURegister( uint32_t addr )
{
   switch ( addr )
   {
   case CPU_PC:
      return CC646502::__PC();
      break;
   case CPU_SP:
      return CC646502::_SP();
      break;
   case CPU_A:
      return CC646502::_A();
      break;
   case CPU_X:
      return CC646502::_X();
      break;
   case CPU_Y:
      return CC646502::_Y();
      break;
   case CPU_F:
      return CC646502::_F();
      break;
   case VECTOR_IRQ:
      break;
   case VECTOR_RESET:
      break;
   case VECTOR_NMI:
      break;
   }
   return 0;
}

uint32_t c64GetCPUFlagNegative ( void )
{
   return 0;
}

uint32_t c64GetCPUFlagOverflow ( void )
{
   return 0;
}

uint32_t c64GetCPUFlagBreak ( void )
{
   return 0;
}

uint32_t c64GetCPUFlagDecimal ( void )
{
   return 0;
}

uint32_t c64GetCPUFlagInterrupt ( void )
{
   return 0;
}

uint32_t c64GetCPUFlagZero ( void )
{
   return 0;
}

uint32_t c64GetCPUFlagCarry ( void )
{
   return 0;
}

void c64SetCPUFlagNegative ( uint32_t set )
{
}

void c64SetCPUFlagOverflow ( uint32_t set )
{
}

void c64SetCPUFlagBreak ( uint32_t set )
{
}

void c64SetCPUFlagDecimal ( uint32_t set )
{
}

void c64SetCPUFlagInterrupt ( uint32_t set )
{
}

void c64SetCPUFlagZero ( uint32_t set )
{
}

void c64SetCPUFlagCarry ( uint32_t set )
{
}

uint32_t c64GetMemory ( uint32_t addr )
{
   return CC646502::_MEM(addr);
}

void c64SetMemory ( uint32_t addr, uint32_t data )
{
   CC646502::_MEM(addr,data);
}

uint32_t c64GetPaletteRedComponent(uint32_t idx)
{
//   return CBasePalette::GetPaletteR(idx)&0xFF;
   return 0;
}

uint32_t c64GetPaletteGreenComponent(uint32_t idx)
{
//   return CBasePalette::GetPaletteG(idx)&0xFF;
   return 0;
}

uint32_t c64GetPaletteBlueComponent(uint32_t idx)
{
//   return CBasePalette::GetPaletteB(idx)&0xFF;
   return 0;
}

void    c64SetPaletteRedComponent(uint32_t idx,uint32_t r)
{
//   CBasePalette::SetPaletteR(idx,r&0xFF);
   return;
}

void    c64SetPaletteGreenComponent(uint32_t idx,uint32_t g)
{
//   CBasePalette::SetPaletteG(idx,g&0xFF);
   return;
}

void    c64SetPaletteBlueComponent(uint32_t idx,uint32_t b)
{
//   CBasePalette::SetPaletteB(idx,b&0xFF);
   return;
}

CRegisterDatabase* c64GetSidRegisterDatabase()
{
   return CSID::REGISTERS();
}

uint32_t c64GetSID8Register ( uint32_t addr )
{
   return CSID::_MEM8(addr);
}

uint32_t c64GetSID16Register ( uint32_t addr )
{
   return CSID::_MEM16(addr);
}

void c64SetSID8Register ( uint32_t addr, uint32_t data )
{
   CSID::_MEM8(addr,data);
}

void c64SetSID16Register ( uint32_t addr, uint32_t data )
{
   CSID::_MEM16(addr,data);
}

void c64GetCpuSnapshot(C64CpuStateSnapshot* pSnapshot)
{
   int idx;
//   pSnapshot->pc = CC646502::__PC();
//   pSnapshot->pcAtSync = CC646502::__PCSYNC();
//   pSnapshot->sp = CC646502::_SP();
//   pSnapshot->a = CC646502::_A();
//   pSnapshot->x = CC646502::_X();
//   pSnapshot->y = CC646502::_Y();
//   pSnapshot->f = CC646502::_F();
   for ( idx = 0; idx < MEM_64KB; idx++ )
   {
      *(pSnapshot->memory+idx) = 0;//CC646502::_MEM(idx);
   }
}
