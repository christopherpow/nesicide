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
   C6502::BREAKONKIL(breakOnKIL);
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
   CNES::VIDEOMODE(mode);
}

uint32_t nesGetSystemMode ( void )
{
   return CNES::VIDEOMODE();
}

void nesGetPrintableAddress ( char* buffer, uint32_t addr )
{
   CNES::PRINTABLEADDR(buffer,addr);
}

void nesGetPrintableAddressWithAbsolute ( char* buffer, uint32_t addr, uint32_t absAddr )
{
   CNES::PRINTABLEADDR(buffer,addr,absAddr);
}

void nesSetControllerType ( int32_t port, int32_t type )
{
   CNES::CONTROLLER(port,type);
}

void nesSetControllerScreenPosition ( int32_t port, int32_t px, int32_t py, int32_t wx1, int32_t wy1, int32_t wx2, int32_t wy2 )
{
   CNES::CONTROLLERPOSITION(port,px,py,wx1,wy1,wx2,wy2);
}

void nesSetControllerSpecial ( int32_t port, int32_t special )
{
   // Note this function must be called after nesSetControllerType is
   // called otherwise the wrong controller type's special setting will
   // be modified.
   if ( iofunc[CNES::CONTROLLER(port)].special )
   {
      iofunc[CNES::CONTROLLER(port)].special(port,special);
   }
}

void nesEnableBreakpoints ( bool enable )
{
   CNES::BREAKPOINTS(enable);
}

void nesStepCpu ( void )
{
   CNES::STEPCPUBREAKPOINT();
}

void nesStepPpu ( void )
{
   CNES::STEPPPUBREAKPOINT();
}

void nesStepPpuFrame ( void )
{
   CNES::STEPPPUBREAKPOINT(true);
}

void nesResetInputRecording ( void )
{
   CIOStandardJoypad::LOGGER(0)->ClearSampleBuffer();
   CIOStandardJoypad::LOGGER(1)->ClearSampleBuffer();
}

void nesSetInputPlayback ( bool enable )
{
   CNES::REPLAY(enable);
}

void nesSetInputRecording ( bool enable )
{
   CNES::RECORD(enable);
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
   return CNES::TRACER();
}

CMarker* nesGetExecutionMarkerDatabase ( void )
{
   return C6502::MARKERS();
}

void nesClearCodeDataLoggerDatabases ( void )
{
   unsigned int addr;

   C6502::LOGGER()->ClearData();

   for ( addr = 0; addr < nesGetPRGROMSize(); addr += MEM_8KB )
   {
      CROM::LOGGERPHYS(addr)->ClearData();
   }
}

CCodeDataLogger* nesGetCpuCodeDataLoggerDatabase ( void )
{
   return C6502::LOGGER();
}

CCodeDataLogger* nesGetVirtualPRGROMCodeDataLoggerDatabase ( uint32_t addr )
{
   return CROM::LOGGERVIRT(addr);
}

CCodeDataLogger* nesGetPhysicalPRGROMCodeDataLoggerDatabase ( uint32_t addr )
{
   return CROM::LOGGERPHYS(addr);
}

CCodeDataLogger* nesGetEXRAMCodeDataLoggerDatabase ( void )
{
   return CROM::EXRAMLOGGER();
}

CCodeDataLogger* nesGetVirtualSRAMCodeDataLoggerDatabase ( uint32_t addr )
{
   return CROM::SRAMLOGGERVIRT(addr);
}

CCodeDataLogger* nesGetPhysicalSRAMCodeDataLoggerDatabase ( uint32_t addr )
{
   return CROM::SRAMLOGGERPHYS(addr);
}

CCodeDataLogger* nesGetPpuCodeDataLoggerDatabase ( void )
{
   return CPPU::LOGGER();
}

CBreakpointInfo* nesGetBreakpointDatabase ( void )
{
   return CNES::BREAKPOINTS();
}

CRegisterDatabase* nesGetCpuRegisterDatabase ( void )
{
   return C6502::REGISTERS();
}

CRegisterDatabase* nesGetPpuRegisterDatabase ( void )
{
   return CPPU::REGISTERS();
}

CRegisterDatabase* nesGetPpuOamRegisterDatabase ( void )
{
   return CPPU::OAMREGISTERS();
}

CRegisterDatabase* nesGetApuRegisterDatabase ( void )
{
   return CAPU::REGISTERS();
}

CRegisterDatabase* nesGetCartridgeRegisterDatabase ( void )
{
   return CROM::REGISTERS();
}

CMemoryDatabase* nesGetCpuMemoryDatabase ( void )
{
   return C6502::MEMORY();
}

CMemoryDatabase* nesGetPpuNameTableMemoryDatabase ( void )
{
   return CPPU::NAMETABLEMEMORY();
}

CMemoryDatabase* nesGetPpuPaletteMemoryDatabase ( void )
{
   return CPPU::PALETTEMEMORY();
}

CMemoryDatabase* nesGetCartridgeEXRAMMemoryDatabase ( void )
{
   return CROM::EXRAMMEMORY();
}

CMemoryDatabase* nesGetCartridgeSRAMMemoryDatabase ( void )
{
   return CROM::SRAMMEMORY();
}

CMemoryDatabase* nesGetCartridgePRGROMMemoryDatabase ( void )
{
   return CROM::PRGROMMEMORY();
}

CMemoryDatabase* nesGetCartridgeCHRMemoryDatabase ( void )
{
   return CROM::CHRMEMORY();
}

CMemoryDatabase* nesGetCartridgeVRAMMemoryDatabase ( void )
{
   return CROM::VRAMMEMORY();
}

int32_t nesGetSizeOfCpuBreakpointEventDatabase ( void )
{
   return C6502::NUMBREAKPOINTEVENTS();
}

CBreakpointEventInfo** nesGetCpuBreakpointEventDatabase ( void )
{
   return C6502::BREAKPOINTEVENTS();
}

int32_t nesGetSizeOfPpuBreakpointEventDatabase ( void )
{
   return CPPU::NUMBREAKPOINTEVENTS();
}

CBreakpointEventInfo** nesGetPpuBreakpointEventDatabase ( void )
{
   return CPPU::BREAKPOINTEVENTS();
}

int32_t nesGetSizeOfApuBreakpointEventDatabase ( void )
{
   return CAPU::NUMBREAKPOINTEVENTS();
}

CBreakpointEventInfo** nesGetApuBreakpointEventDatabase ( void )
{
   return CAPU::BREAKPOINTEVENTS();
}

int32_t nesGetSizeOfCartridgeBreakpointEventDatabase ( void )
{
   return CROM::NUMBREAKPOINTEVENTS();
}

CBreakpointEventInfo** nesGetCartridgeBreakpointEventDatabase ( void )
{
   return CROM::BREAKPOINTEVENTS();
}

void nesDisassemble ()
{
   CNES::DISASSEMBLE();
}

void nesDisassembleSingle ( uint8_t* pOpcode, char* buffer )
{
   C6502::Disassemble(pOpcode,buffer);
}

char* nesGetDisassemblyAtAddress ( uint32_t addr )
{
   return CNES::DISASSEMBLY(addr);
}

void nesGetDisassemblyAtAbsoluteAddress ( uint32_t absAddr, char* buffer )
{
   CROM::PRGROMDISASSEMBLYATABSADDR(absAddr,buffer);
}

uint32_t nesGetAddressFromSLOC ( uint16_t sloc )
{
   return CNES::SLOC2ADDR(sloc);
}

uint16_t nesGetSLOCFromAddress ( uint32_t addr )
{
   return CNES::ADDR2SLOC(addr);
}

uint32_t nesGetSLOC ( uint32_t addr )
{
   return CNES::SLOC(addr);
}

uint32_t nesGetAbsoluteAddressFromAddress ( uint32_t addr )
{
   return CNES::ABSADDR(addr);
}

void nesClearOpcodeMasks ( void )
{
   C6502::OPCODEMASKCLR ();
   CROM::PRGROMOPCODEMASKCLR ();
   CROM::SRAMOPCODEMASKCLR ();
   CROM::EXRAMOPCODEMASKCLR ();
}

void nesSetOpcodeMask ( uint32_t addr, uint8_t mask )
{
   CROM::PRGROMOPCODEMASKATABSADDR(addr, mask);
}

void nesSetTVOut ( int8_t* tv )
{
   CPPU::TV ( tv );
}

void nesUnloadROM ( void )
{
   CROM::ClearPRGBanks ();
   CROM::ClearCHRBanks ();
   CROM::RESET(0);
}

void nesLoadPRGROMBank ( uint32_t bank, uint8_t* bankData )
{
   CROM::SetPRGBank ( bank, bankData );
}

void nesLoadCHRROMBank ( uint32_t bank, uint8_t* bankData )
{
   CROM::SetCHRBank ( bank, bankData );
}

void nesLoadROM ( void )
{
   CROM::DoneLoadingBanks();
}

bool nesROMIsLoaded ( void )
{
   return ( (CROM::NUMPRGROMBANKS()>0)?true:false );
}

void nesSetHorizontalMirroring ( void )
{
   CPPU::MIRRORHORIZ();
}

void nesSetVerticalMirroring ( void )
{
   CPPU::MIRRORVERT();
}

void nesSetFourScreen ( void )
{
   CPPU::MIRROR(-1,true,true);
}

void nesReset ( bool soft )
{
   CNES::RESET(CROM::MAPPER(),soft);
}

void nesResetInitial ( uint32_t mapper )
{
   CNES::RESET(mapper,false);
}

void nesRun ( uint32_t* joypads )
{
   CNES::RUN(joypads);
}

uint8_t* nesGetAudioSamples ( uint16_t samples )
{
   return CAPU::PLAY(samples);
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
   return C6502::_CYCLES();
}

void nesSetGotoAddress ( uint32_t addr )
{
   if ( addr == 0xFFFFFFFF )
   {
      C6502::GOTO ();
   }
   else
   {
      C6502::GOTO ( addr );
   }
}

bool nesCPUIsFetchingOpcode ( void )
{
   return C6502::_SYNC();
}

bool nesCPUIsWritingMemory ( void )
{
   return C6502::_WRITING();
}

int8_t* nesGetTVOut ( void )
{
   return CPPU::TV();
}

void nesSetVRC6AudioChannelMask ( uint32_t mask )
{
   _mapperfunc[24].soundenable(mask);
}

void nesSetN106AudioChannelMask ( uint32_t mask )
{
   _mapperfunc[19].soundenable(mask);
}

void nesSetAudioChannelMask ( uint8_t mask )
{
   CAPU::MUTE(mask);
}

uint32_t nesGetCPUEffectiveAddress ( void )
{
   return C6502::_EA();
}

uint32_t nesGetCPUProgramCounter( void )
{
   return C6502::__PC();
}

uint32_t nesGetCPUProgramCounterOfLastSync( void )
{
   return C6502::__PCSYNC();
}

void nesSetCPURegister ( uint32_t addr, uint32_t data )
{
   switch ( addr )
   {
   case CPU_PC:
      C6502::__PC(data);
      break;
   case CPU_SP:
      C6502::_SP(data);
      break;
   case CPU_A:
      C6502::_A(data);
      break;
   case CPU_X:
      C6502::_X(data);
      break;
   case CPU_Y:
      C6502::_Y(data);
      break;
   case CPU_F:
      C6502::_F(data);
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
      return C6502::__PC();
      break;
   case CPU_SP:
      return 0x100|C6502::_SP();
      break;
   case CPU_A:
      return C6502::_A();
      break;
   case CPU_X:
      return C6502::_X();
      break;
   case CPU_Y:
      return C6502::_Y();
      break;
   case CPU_F:
      return C6502::_F();
      break;
   case VECTOR_IRQ:
      return MAKE16(C6502::_MEM(VECTOR_IRQ),C6502::_MEM(VECTOR_IRQ+1));
      break;
   case VECTOR_RESET:
      return MAKE16(C6502::_MEM(VECTOR_RESET),C6502::_MEM(VECTOR_RESET+1));
      break;
   case VECTOR_NMI:
      return MAKE16(C6502::_MEM(VECTOR_NMI),C6502::_MEM(VECTOR_NMI+1));
      break;
   }
   return 0;
}

uint32_t nesGetCPUFlagNegative ( void )
{
   return C6502::_N();
}

uint32_t nesGetCPUFlagOverflow ( void )
{
   return C6502::_V();
}

uint32_t nesGetCPUFlagBreak ( void )
{
   return C6502::_B();
}

uint32_t nesGetCPUFlagDecimal ( void )
{
   return C6502::_D();
}

uint32_t nesGetCPUFlagInterrupt ( void )
{
   return C6502::_I();
}

uint32_t nesGetCPUFlagZero ( void )
{
   return C6502::_Z();
}

uint32_t nesGetCPUFlagCarry ( void )
{
   return C6502::_C();
}

void nesSetCPUFlagNegative ( uint32_t set )
{
   C6502::_N(set);
}

void nesSetCPUFlagOverflow ( uint32_t set )
{
   C6502::_V(set);
}

void nesSetCPUFlagBreak ( uint32_t set )
{
   C6502::_B(set);
}

void nesSetCPUFlagDecimal ( uint32_t set )
{
   C6502::_D(set);
}

void nesSetCPUFlagInterrupt ( uint32_t set )
{
   C6502::_I(set);
}

void nesSetCPUFlagZero ( uint32_t set )
{
   C6502::_Z(set);
}

void nesSetCPUFlagCarry ( uint32_t set )
{
   C6502::_C(set);
}

uint32_t nesGetPPUMemory ( uint32_t addr )
{
   return CPPU::_MEM(addr);
}

void nesSetPPUMemory ( uint32_t addr, uint32_t data )
{
   CPPU::_MEM(addr,data);
}

uint32_t nesGetCPUMemory ( uint32_t addr )
{
   return C6502::_MEM(addr);
}

uint8_t* nesGetCPUMemoryPtr ( void )
{
   return C6502::_MEMPTR();
}

void nesSetCPUMemory ( uint32_t addr, uint32_t data )
{
   C6502::_MEM(addr,data);
}

uint8_t nesGetMemory ( uint32_t addr )
{
   return CNES::_MEM(addr);
}

uint32_t nesGetPPUCycle ( void )
{
   return CPPU::_CYCLES();
}

uint32_t nesGetPPURegister ( uint32_t addr )
{
   return CPPU::_PPU(addr);
}

void nesSetPPURegister ( uint32_t addr, uint32_t data )
{
   CPPU::_PPU(addr,data);
}

uint16_t nesGetScrollXAtXY ( int32_t x, int32_t y )
{
   return CPPU::_SCROLLX(x,y);
}

uint16_t nesGetScrollYAtXY ( int32_t x, int32_t y )
{
   return CPPU::_SCROLLY(x,y);
}

void nesGetCurrentScroll ( uint8_t* x, uint8_t* y )
{
   return CPPU::_SCROLL(x,y);
}

void nesGetLastSprite0Hit ( uint8_t* x, uint8_t* y )
{
   (*x) = CPPU::_SPRITE0HITX();
   (*y) = CPPU::_SPRITE0HITY();
}

void nesGetCurrentPixel ( uint8_t* x, uint8_t* y )
{
   (*x) = CPPU::_X();
   (*y) = CPPU::_Y();
}

void nesGetMirroring ( uint16_t* sc1, uint16_t* sc2, uint16_t* sc3, uint16_t* sc4 )
{
   CPPU::_MIRROR(sc1,sc2,sc3,sc4);
}

uint32_t nesGetAPUCycle ( void )
{
   return CAPU::CYCLES();
}

uint32_t nesGetAPURegister ( uint32_t addr )
{
   return CAPU::_APU(addr);
}

void nesSetAPURegister ( uint32_t addr, uint32_t data )
{
   CAPU::_APU(addr,data);
}

int32_t nesGetAPUSequencerMode ( void )
{
   return CAPU::SEQUENCERMODE();
}

void nesGetAPULengthCounters( uint16_t* sq1,
                              uint16_t* sq2,
                              uint16_t* triangle,
                              uint16_t* noise,
                              uint16_t* dmc)
{
   CAPU::LENGTHCOUNTERS(sq1,sq2,triangle,noise,dmc);
}

void nesGetAPUTriangleLinearCounter ( uint8_t* triangle )
{
   CAPU::LINEARCOUNTER(triangle);
}

void nesGetAPUDACs ( uint8_t* sq1,
                     uint8_t* sq2,
                     uint8_t* triangle,
                     uint8_t* noise,
                     uint8_t* dmc )
{
   CAPU::GETDACS(sq1,sq2,triangle,noise,dmc);
}

void nesGetAPUDMCIRQ ( bool* enabled,
                       bool* asserted )
{
   CAPU::DMCIRQ(enabled,asserted);
}

void nesGetAPUDMCSampleInfo ( uint16_t* addr,
                              uint16_t* length,
                              uint16_t* pos )
{
   CAPU::SAMPLEINFO(addr,length,pos);
}

void nesGetAPUDMCDMAInfo ( uint8_t* buffer,
                           bool* full )
{
   CAPU::DMAINFO(buffer,full);
}

uint32_t nesGetPRGROMSize ( void )
{
   return CROM::NUMPRGROMBANKS()*MEM_8KB;
}

uint32_t nesGetNumPRGROMBanks ( void )
{
   return CROM::NUMPRGROMBANKS();
}

uint32_t nesGetNumCHRROMBanks ( void )
{
   return CROM::NUMCHRROMBANKS();
}

bool nesIsCHRRAM ( void )
{
   return !CROM::IsWriteProtected();
}

uint8_t nesGetMapper ( void )
{
   return CROM::MAPPER();
}

uint32_t nesMapperLowRead ( uint32_t addr )
{
   return MAPPERFUNC->debuginfo(addr);
}

void nesMapperLowWrite ( uint32_t addr, uint32_t data )
{
   MAPPERFUNC->lowwrite(addr,data);
}

uint32_t nesMapperHighRead ( uint32_t addr )
{
   return MAPPERFUNC->debuginfo(addr);
}

void nesMapperHighWrite ( uint32_t addr, uint32_t data )
{
   MAPPERFUNC->highwrite(addr,data);
}

uint32_t nesGetPPUOAM ( uint32_t addr )
{
   return CPPU::_OAM(addr&3,addr>>2);
}

void nesSetPPUOAM ( uint32_t addr, uint32_t data )
{
   CPPU::_OAM(addr&3,addr>>2,data);
}

uint32_t nesGetPPUFrame ( void )
{
   return CPPU::_FRAME();
}

uint16_t nesGetPPUAddress ( void )
{
   return CPPU::_PPUADDR();
}

uint8_t nesGetPPUOAMAddress ( void )
{
   return CPPU::_OAMADDR();
}

uint8_t nesGetPPUReadLatch ( void )
{
   return CPPU::_PPUREADLATCH();
}

uint16_t nesGetPPUAddressLatch ( void )
{
   return CPPU::_PPUADDRLATCH();
}

int32_t nesGetPPUFlipFlop ( void )
{
   return CPPU::_PPUFLIPFLOP();
}

uint32_t nesGetPRGROMAbsoluteAddress ( uint32_t addr )
{
   return CROM::PRGROMABSADDR(addr);
}

uint32_t nesGetCHRMEMAbsoluteAddress ( uint32_t addr )
{
   return CROM::CHRMEMABSADDR(addr);
}

uint32_t nesGetPRGROMData ( uint32_t addr )
{
   return CROM::PRGROM(addr);
}

uint32_t nesGetCHRMEMData ( uint32_t addr )
{
   return CROM::CHRMEM(addr);
}

void nesSetCHRMEMData ( uint32_t addr, uint32_t data )
{
   CROM::CHRMEM(addr,data);
}

uint32_t nesGetSRAMAbsoluteAddress ( uint32_t addr )
{
   return CROM::SRAMABSADDR(addr);
}

uint32_t nesGetSRAMDataVirtual ( uint32_t addr )
{
   return CROM::SRAMVIRT(addr);
}

void nesSetSRAMDataVirtual ( uint32_t addr, uint32_t data )
{
   CROM::SRAMVIRT(addr,data);
}

uint32_t nesGetSRAMDataPhysical ( uint32_t addr )
{
   return CROM::SRAMPHYS(addr);
}

void nesSetSRAMDataPhysical ( uint32_t addr, uint32_t data )
{
   CROM::SRAMPHYS(addr,data);
}

void nesLoadSRAMDataPhysical ( uint32_t addr, uint32_t data )
{
   CROM::SRAMPHYS(addr,data,false);
}

bool nesIsSRAMDirty ()
{
   return CROM::SRAMDIRTY();
}

uint32_t nesGetEXRAMAbsoluteAddress ( uint32_t addr )
{
   return CROM::EXRAMABSADDR(addr);
}

uint32_t nesGetEXRAMData ( uint32_t addr )
{
   return CROM::EXRAM(addr);
}

void nesSetEXRAMData ( uint32_t addr, uint32_t data )
{
   CROM::EXRAM(addr,data);
}

uint32_t nesGetVRAMData ( uint32_t addr )
{
   return CROM::VRAM(addr);
}

void nesSetVRAMData ( uint32_t addr, uint32_t data )
{
   CROM::VRAM(addr,data);
}

bool nesMapperRemapsPRGROM ( void )
{
   return MAPPERFUNC->remapPrg;
}

bool nesMapperRemapsCHRMEM ( void )
{
   return MAPPERFUNC->remapChr;
}

bool nesMapperRemapsVMEM ( void )
{
   return MAPPERFUNC->remapVram;
}

uint32_t nesMapperRemappedVMEMSize ( void )
{
   return MAPPERFUNC->vramSize;
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
   pInfo->shiftRegister = CROMMapper001::SHIFTREGISTER();
   pInfo->shiftRegisterBit = CROMMapper001::SHIFTREGISTERBIT();
}

void nesMapper004GetInformation ( nesMapper004Info* pInfo )
{
   pInfo->irqAsserted = CROMMapper004::IRQASSERTED();
   pInfo->irqCounter = CROMMapper004::IRQCOUNTER();
   pInfo->irqEnabled = CROMMapper004::IRQENABLED();
   pInfo->irqReload = CROMMapper004::IRQRELOAD();
   pInfo->irqLatch = CROMMapper004::IRQLATCH();
   pInfo->ppuAddrA12 = CROMMapper004::PPUADDRA12();
   pInfo->ppuCycle = CROMMapper004::PPUCYCLE();
}

void nesMapper009GetInformation ( nesMapper009010Info* pInfo )
{
   pInfo->latch0FD = CROMMapper009::LATCH0FD();
   pInfo->latch0FE = CROMMapper009::LATCH0FE();
   pInfo->latch1FD = CROMMapper009::LATCH1FD();
   pInfo->latch1FE = CROMMapper009::LATCH1FE();
   pInfo->latch0 = CROMMapper009::LATCH0();
   pInfo->latch1 = CROMMapper009::LATCH0();
}

void nesMapper010GetInformation ( nesMapper009010Info* pInfo )
{
   pInfo->latch0FD = CROMMapper010::LATCH0FD();
   pInfo->latch0FE = CROMMapper010::LATCH0FE();
   pInfo->latch1FD = CROMMapper010::LATCH1FD();
   pInfo->latch1FE = CROMMapper010::LATCH1FE();
   pInfo->latch0 = CROMMapper010::LATCH0();
   pInfo->latch1 = CROMMapper010::LATCH0();
}

void nesMapper016GetInformation ( nesMapper016Info* pInfo )
{
   pInfo->irqAsserted = CROMMapper016::IRQASSERTED();
   pInfo->irqCounter = CROMMapper016::IRQCOUNTER();
   pInfo->irqEnabled = CROMMapper016::IRQENABLED();
   pInfo->eepromState = CROMMapper016::EEPROMSTATE();
   pInfo->eepromBitCounter = CROMMapper016::EEPROMBITCOUNTER();
   pInfo->eepromCmd = CROMMapper016::EEPROMCMD();
   pInfo->eepromAddr = CROMMapper016::EEPROMADDR();
   pInfo->eepromDataBuf = CROMMapper016::EEPROMDATABUF();
}

void nesMapper028GetInformation ( nesMapper028Info* pInfo )
{
   pInfo->regSel = CROMMapper028::REGSEL();
   pInfo->prgMode = CROMMapper028::PRGMODE();
   pInfo->prgSize = CROMMapper028::PRGSIZE();
   pInfo->prgOuterBank = CROMMapper028::PRGOUTERBANK();
   pInfo->prgInnerBank = CROMMapper028::PRGINNERBANK();
   pInfo->chrBank = CROMMapper028::CHRBANK();
}

void nesMapper069GetInformation ( nesMapper069Info* pInfo )
{
   pInfo->irqAsserted = CROMMapper069::IRQASSERTED();
   pInfo->irqCounter = CROMMapper069::IRQCOUNTER();
   pInfo->irqEnabled = CROMMapper069::IRQENABLED();
   pInfo->irqCountEnabled = CROMMapper069::IRQCOUNTENABLED();
   pInfo->sramEnabled = CROMMapper069::SRAMENABLED();
   pInfo->sramIsSram = CROMMapper069::ISSRAM();
   pInfo->sramOrPrgBank = CROMMapper069::SRAMPRGBANK();
   pInfo->regSelected = CROMMapper069::REGSELECTED();
   pInfo->regValue = CROMMapper069::REGVALUE();
}

void nesGetCpuSnapshot(NESCpuStateSnapshot* pSnapshot)
{
   int idx;
   pSnapshot->cycle = C6502::_CYCLES();
   pSnapshot->pc = C6502::__PC();
   pSnapshot->pcAtSync = C6502::__PCSYNC();
   pSnapshot->sp = C6502::_SP();
   pSnapshot->a = C6502::_A();
   pSnapshot->x = C6502::_X();
   pSnapshot->y = C6502::_Y();
   pSnapshot->f = C6502::_F();
   for ( idx = 0; idx < MEM_2KB; idx++ )
   {
      *(pSnapshot->memory+idx) = C6502::_MEM(idx);
   }
}

void nesGetPpuSnapshot(PpuStateSnapshot* pSnapshot)
{
   int idx;
   int x,y;
   pSnapshot->frame = CPPU::_FRAME();
   pSnapshot->cycle = CPPU::_CYCLES();
   for ( idx = 0; idx < NUM_PPU_REGS; idx++ )
   {
      *(pSnapshot->reg+idx) = CPPU::_PPU(idx);
   }
   for ( idx = 0; idx < MEM_256B; idx++ )
   {
      *(pSnapshot->oamMemory+idx) = CPPU::_OAM(idx&3,idx>>2);
   }
   for ( idx = 0; idx < MEM_32B; idx++ )
   {
      *(pSnapshot->paletteMemory+idx) = CPPU::_PALETTE(idx);
   }
   for ( idx = 0; idx < MEM_32KB; idx++ )
   {
      *(pSnapshot->memory+idx) = CPPU::_MEM(idx);
   }
   for ( y = 0; y < 240; y++ )
   {
      for ( x = 0; x < 256; x++ )
      {
         *(*(pSnapshot->xOffset+x)+y) = CPPU::_SCROLLX(x,y);
         *(*(pSnapshot->yOffset+x)+y) = CPPU::_SCROLLY(x,y);
      }
   }
}

void nesGetApuSnapshot(ApuStateSnapshot* pSnapshot)
{
   int idx;
   pSnapshot->cycle = CAPU::CYCLES();
   for ( idx = 0; idx < NUM_APU_REGS; idx++ )
   {
      *(pSnapshot->reg+idx) = CAPU::_APU(idx);
   }
   pSnapshot->sequencerMode = CAPU::SEQUENCERMODE();
   CAPU::LINEARCOUNTER(&pSnapshot->triangleLinearCounter);
   CAPU::LENGTHCOUNTERS(&pSnapshot->lengthCounter[0],
                        &pSnapshot->lengthCounter[1],
                        &pSnapshot->lengthCounter[2],
                        &pSnapshot->lengthCounter[3],
                        &pSnapshot->lengthCounter[4]);
   CAPU::GETDACS(&pSnapshot->dac[0],
                 &pSnapshot->dac[1],
                 &pSnapshot->dac[2],
                 &pSnapshot->dac[3],
                 &pSnapshot->dac[4]);
   CAPU::DMCIRQ(&pSnapshot->dmcIrqEnabled,&pSnapshot->dmcIrqAsserted);
   CAPU::SAMPLEINFO(&pSnapshot->dmaSampleAddress,&pSnapshot->dmaSampleLength,&pSnapshot->dmaSamplePosition);
   CAPU::DMAINFO(&pSnapshot->dmcDmaBuffer,&pSnapshot->dmcDmaFull);
}

void nesGetNesSnapshot(NesStateSnapshot *pSnapshot)
{
   nesGetCpuSnapshot(&pSnapshot->cpu);
   nesGetApuSnapshot(&pSnapshot->apu);
   nesGetPpuSnapshot(&pSnapshot->ppu);
}
