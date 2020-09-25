#ifndef NES_EMULATOR_CORE_H
#define NES_EMULATOR_CORE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h> // for standard base types...
#include <string.h> // for memcpy...
#include <stdio.h> // for sprintf...

#include "cmemorydata.h"
#include "cregisterdata.h"
#include "cmarker.h"
#include "cbreakpointinfo.h"

// Common enumerations for emulated items.
typedef enum
{
   eNESSource_CPU = 0,
   eNESSource_PPU,
   eNESSource_APU,
   eNESSource_Mapper
} eNESSourceType;

// Breakpoint event identifiers.
// These event identifiers must match the ordering
// of breakpoint events defined in the source module.
enum
{
   CPU_EVENT_EXECUTE_EXACT = 0,
   CPU_EVENT_UNDOCUMENTED,
   CPU_EVENT_UNDOCUMENTED_EXACT,
   CPU_EVENT_RESET,
   CPU_EVENT_IRQ_FIRES,
   CPU_EVENT_IRQ_ENTERED,
   CPU_EVENT_NMI_FIRES,
   CPU_EVENT_NMI_ENTERED,
   CPU_EVENT_STOLEN_CYCLE,
   NUM_CPU_EVENTS
};

// These event identifiers must match the ordering
// of breakpoint events defined in the source module.
enum
{
   APU_EVENT_IRQ = 0,
   APU_EVENT_DMC_DMA,
   APU_EVENT_LENGTH_COUNTER_CLOCKED,
   APU_EVENT_SQUARE1_DAC_VALUE,
   APU_EVENT_SQUARE2_DAC_VALUE,
   APU_EVENT_TRIANGLE_DAC_VALUE,
   APU_EVENT_NOISE_DAC_VALUE,
   APU_EVENT_DMC_DAC_VALUE,
   NUM_APU_EVENTS
};

// These event identifiers must match the ordering
// of breakpoint events defined in the source module.
enum
{
   PPU_EVENT_NMI = 0,
   PPU_EVENT_PIXEL_XY,
   PPU_EVENT_ADDRESS_EQUALS,
   PPU_EVENT_PRE_RENDER_SCANLINE_START,
   PPU_EVENT_PRE_RENDER_SCANLINE_END,
   PPU_EVENT_SCANLINE_START,
   PPU_EVENT_SCANLINE_END,
   PPU_EVENT_XSCROLL_UPDATE,
   PPU_EVENT_YSCROLL_UPDATE,
   PPU_EVENT_SPRITE_DMA,
   PPU_EVENT_SPRITE0_HIT,
   PPU_EVENT_SPRITE_IN_MULTIPLEXER,
   PPU_EVENT_SPRITE_SELECTED,
   PPU_EVENT_SPRITE_RENDERING,
   PPU_EVENT_SPRITE_OVERFLOW,
   NUM_PPU_EVENTS
};

// These event identifiers must match the ordering
// of breakpoint events defined in the source module.
enum
{
   MAPPER_EVENT_IRQ = 0,
   NUM_MAPPER_EVENTS
};

// This will be set by a cartridge mapper that has registers
extern CRegisterDatabase*  m_dbCartRegisters;

// Video modes.  The emulator supports both NTSC and PAL.
#define MODE_NTSC  0
#define MODE_PAL   1
#define MODE_DENDY 2

#define MAKE16(lo,hi) ((((lo)&0xFF)|(((hi)&0xFF)<<8)))

// CPU interrupt vector memory addresses.
#define VECTOR_NMI   0xFFFA
#define VECTOR_RESET 0xFFFC
#define VECTOR_IRQ   0xFFFE

// Instructions with special handling needs.
#define BRK_IMPLIED  0x00
#define JSR_ABSOLUTE 0x20
#define PLP_IMPLIED  0x28
#define ROL_ABS_X    0x3E
#define RTI_IMPLIED  0x40
#define CLI_IMPLIED  0x58
#define SEI_IMPLIED  0x78

// CPU register 'numbers' (correspond only to UI elements as there's no
// physical memory address associated with them).
#define CPU_PC    0
#define CPU_A     1
#define CPU_X     2
#define CPU_Y     3
#define CPU_SP    4
#define CPU_F     5
#define CPU_IRQ   6
#define CPU_NMI   7
#define CPU_RESET 8

// CPU instruction addressing modes.
#define AM_IMPLIED              0
#define AM_IMMEDIATE            1
#define AM_ABSOLUTE             2
#define AM_ZEROPAGE             3
#define AM_ACCUMULATOR          4
#define AM_ABSOLUTE_INDEXED_X   5
#define AM_ABSOLUTE_INDEXED_Y   6
#define AM_ZEROPAGE_INDEXED_X   7
#define AM_ZEROPAGE_INDEXED_Y   8
#define AM_INDIRECT             9
#define AM_PREINDEXED_INDIRECT  10
#define AM_POSTINDEXED_INDIRECT 11
#define AM_RELATIVE             12
#define NUM_ADDRESSING_MODES    13

#define NUM_ROM_BANKS 128
#define NUM_CHR_BANKS 1024+32 // 32 extra banks for CHR-ROM+CHR-RAM mappers like N106.
#define NUM_SRAM_BANKS 8

#define PATTERN_SIZE 8

// PPU register definitions.
// PPU register absolute addresses.
#define PPUREGBASE 0x2000
#define PPUCTRL    0x2000
#define PPUMASK    0x2001
#define PPUSTATUS  0x2002
#define OAMADDR    0x2003
#define OAMDATA    0x2004
#define PPUSCROLL  0x2005
#define PPUADDR    0x2006
#define PPUDATA    0x2007
// PPU register offsets from base address.
// These are used for register mirroring.
#define PPUCTRL_REG   0
#define PPUMASK_REG   1
#define PPUSTATUS_REG 2
#define OAMADDR_REG   3
#define OAMDATA_REG   4
#define PPUSCROLL_REG 5
#define PPUADDR_REG   6
#define PPUDATA_REG   7
#define NUM_PPU_REGS 8

#define NUM_APU_REGS 32
#define NUM_APU_CHANNELS 5

// Sprites with a Y-coordinate at or above (higher in
// numeric value) this value will not be visible on the
// NES screen.
#define SPRITE_YMAX  0xF0

// OAM attribute bit definitions.
#define SPRITE_PALETTE_IDX_MSK 0x03
#define SPRITE_PRIORITY        0x20
#define SPRITE_FLIP_HORIZ      0x40
#define SPRITE_FLIP_VERT       0x80

// PPUCTRL register bit definitions.
#define PPUCTRL_BASE_NAM_TBL_ADDR_MSK 0x03
#define PPUCTRL_VRAM_ADDR_INC         0x04
#define PPUCTRL_SPRITE_PAT_TBL_ADDR   0x08
#define PPUCTRL_BKGND_PAT_TBL_ADDR    0x10
#define PPUCTRL_SPRITE_SIZE           0x20
#define PPUCTRL_PPU_MASTER_SLAVE      0x40
#define PPUCTRL_GENERATE_NMI          0x80

// PPUMASK register bit definitions.
#define PPUMASK_GREYSCALE               0x01
#define PPUMASK_BKGND_CLIPPING          0x02
#define PPUMASK_SPRITE_CLIPPING         0x04
#define PPUMASK_RENDER_BKGND            0x08
#define PPUMASK_RENDER_SPRITES          0x10
#define PPUMASK_INTENSIFY_REDS          0x20
#define PPUMASK_INTENSIFY_GREENS        0x40
#define PPUMASK_INTENSIFY_BLUES         0x80

// PPUSTATUS register bit definitions.
#define PPUSTATUS_SPRITE_OVFLO        0x20
#define PPUSTATUS_SPRITE_0_HIT        0x40
#define PPUSTATUS_VBLANK              0x80

// Number of scanlines for various calculations.
#define SCANLINES_VISIBLE (240)
#define SCANLINES_QUIET_NTSC (1)
#define SCANLINES_QUIET_PAL (1)
#define SCANLINES_QUIET_DENDY (51)
#define SCANLINES_VBLANK_NTSC (20)
#define SCANLINES_VBLANK_PAL (70)
#define SCANLINES_VBLANK_DENDY (20)
#define SCANLINE_PRERENDER_NTSC (SCANLINES_VISIBLE+SCANLINES_VBLANK_NTSC+SCANLINES_QUIET_NTSC)
#define SCANLINE_PRERENDER_PAL (SCANLINES_VISIBLE+SCANLINES_VBLANK_PAL+SCANLINES_QUIET_PAL)
#define SCANLINE_PRERENDER_DENDY (SCANLINES_VISIBLE+SCANLINES_VBLANK_DENDY+SCANLINES_QUIET_DENDY)

// NTSC and PAL have different total scanline counts.
#define SCANLINES_TOTAL_NTSC (SCANLINES_VISIBLE+SCANLINES_VBLANK_NTSC+SCANLINES_QUIET_NTSC+1)
#define SCANLINES_TOTAL_PAL (SCANLINES_VISIBLE+SCANLINES_VBLANK_PAL+SCANLINES_QUIET_PAL+1)
#define SCANLINES_TOTAL_DENDY (SCANLINES_VISIBLE+SCANLINES_VBLANK_DENDY+SCANLINES_QUIET_DENDY+1)

// PPU clock cycles 341 times during *most* scanlines.
#define PPU_CYCLES_PER_SCANLINE 341

// The Execution Visualizer debugger inspector converts PPU cycles to
// (x,y) coordinates for visualization of the execution of code in the
// PPU frame.
#define CYCLE_TO_VISX(c) (c%PPU_CYCLES_PER_SCANLINE)
#define CYCLE_TO_VISY(c) (c/PPU_CYCLES_PER_SCANLINE)
#define VISY_VISX_TO_CYCLE(y,x) ((y*PPU_CYCLES_PER_SCANLINE)+x)

// The PPU frame driven by the NES object starts at pixel 0, 0
// of the visible screen and goes on through the post-render scanline
// and VBLANK and the pre-render scanline.
#define PPU_CYCLE_START_VBLANK_NTSC ((SCANLINES_VISIBLE+SCANLINES_QUIET_NTSC)*PPU_CYCLES_PER_SCANLINE)
#define PPU_CYCLE_START_VBLANK_PAL ((SCANLINES_VISIBLE+SCANLINES_QUIET_PAL)*PPU_CYCLES_PER_SCANLINE)
#define PPU_CYCLE_START_VBLANK_DENDY ((SCANLINES_VISIBLE+SCANLINES_QUIET_DENDY)*PPU_CYCLES_PER_SCANLINE)
#define PPU_CYCLE_END_VBLANK_NTSC ((SCANLINES_VISIBLE+SCANLINES_QUIET_NTSC+SCANLINES_VBLANK_NTSC)*PPU_CYCLES_PER_SCANLINE)
#define PPU_CYCLE_END_VBLANK_PAL ((SCANLINES_VISIBLE+SCANLINES_QUIET_PAL+SCANLINES_VBLANK_PAL)*PPU_CYCLES_PER_SCANLINE)
#define PPU_CYCLE_END_VBLANK_DENDY ((SCANLINES_VISIBLE+SCANLINES_QUIET_DENDY+SCANLINES_VBLANK_DENDY)*PPU_CYCLES_PER_SCANLINE)

// PPU OAM definitions.
// Total number of sprites in OAM.
#define NUM_SPRITES  64

// Total number of sprites per scanline.
#define NUM_SPRITES_PER_SCANLINE 8

// OAM 'register' offsets.  Each sprite has
// an entry of each of these four types.
#define SPRITEY      0
#define SPRITEPAT    1
#define SPRITEATT    2
#define SPRITEX      3
#define OAM_SIZE     4
#define NUM_OAM_REGS (NUM_SPRITES*OAM_SIZE)

// Samples per video frame for 44.1KHz audio output.
// NTSC is 60Hz, PAL is 50Hz.  The number of samples
// drives the rate at which the SDL library will invoke
// the callback method to retrieve more audio samples to
// play.
#define APU_SAMPLES           (1024)

#define SDL_SAMPLE_RATE       (44100)

#define APU_SAMPLE_SPACE_NTSC  ((89341.5/3.0)/(SDL_SAMPLE_RATE/60.0))
#define APU_SAMPLE_SPACE_PAL   ((106392.0/3.2)/(SDL_SAMPLE_RATE/50.0))
#define APU_SAMPLE_SPACE_DENDY ((106392.0/3.0)/(SDL_SAMPLE_RATE/50.0))

#define APU_BUFFER_PRERENDER           (APU_SAMPLES*2)   // How much rendering to do

#pragma pack(1)
typedef struct
{
   unsigned int id;
   uint8_t numPrgBanks;
   uint8_t numChrBanks;
   uint8_t flags:4;
   uint8_t mapper1:4;
   uint8_t romType:2;
   uint8_t unused1:2;
   uint8_t mapper2:4;
   uint8_t unused2 [ 8 ];
} iNES_header_struct;
#pragma pack()

#define MEM_0B 0x0
#define MEM_8B 0x8
#define MASK_8B 0x7
#define MEM_32B 0x20
#define MASK_32B 0x1F
#define MEM_256B 0x100
#define MASK_256B 0xFF
#define MEM_512B 0x200
#define MASK_512B 0x1FF
#define MEM_1KB 0x400
#define MASK_1KB 0x3FF
#define UPSHIFT_1KB 10
#define MEM_2KB 0x800
#define MASK_2KB 0x7FF
#define UPSHIFT_2KB 11
#define MEM_4KB 0x1000
#define MASK_4KB 0xFFF
#define UPSHIFT_4KB 12
#define MEM_8KB 0x2000
#define SHIFT_8KB_1KB 10
#define MASK_8KB 0x1FFF
#define UPSHIFT_8KB 13
#define MEM_16KB 0x4000
#define MASK_16KB 0x3FFF
#define UPSHIFT_16KB 14
#define MEM_32KB 0x8000
#define MASK_32KB 0x7FFF
#define SHIFT_32KB_8KB 13
#define SHIFT_32KB_4KB 14
#define UPSHIFT_32KB 15
#define MEM_64KB 0x10000
#define MASK_64KB 0xFFFF
#define SHIFT_64KB_8KB 13

#define EXRAM_START 0x5C00
#define SRAM_START 0x6000
#define VRAM_START 0x2000
#define PPUMEM_START 0x2000
#define PPUPAL_START 0x3F00
#define PPUREG_START 0x2000
#define IOREG_START 0x4000
#define IOPORTREGBASE  0x4000
#define IOSPRITEDMA    0x4014
#define APUCTRL        0x4015
#define IOJOY1         0x4016
#define IOJOY2         0x4017

enum
{
   FLAG_MIRROR          = 0x01,
   FLAG_MIRROR_HORIZ    = 0x00,
   FLAG_MIRROR_VERT     = 0x01,
   FLAG_SRAM            = 0x02,
   FLAG_SRAM_DISABLED   = 0x00,
   FLAG_SRAM_ENABLED    = 0x02,
   FLAG_TRAINER         = 0x04,
   FLAG_NO_TRAINER      = 0x00,
   FLAG_VRAM            = 0x08,
   FLAG_FOURSCREEN_VRAM = 0x08,
   FLAG_NORMAL_VRAM     = 0x00
};

enum
{
   ROM_TYPE_NES          = 0x00,
   ROM_TYPE_VS_UNISYSTEM = 0x01,
   ROM_TYPE_PLAYCHOICE   = 0x02
};

#define INES_HEADER_ID 0x1a53454e

// Supported NES input (controller) types:
// Standard joypad
// Zapper (light gun)
enum
{
   IO_Disconnected = 0,
   IO_StandardJoypad,
   IO_TurboJoypad,
   IO_Zapper,
   IO_Vaus
};

// Standard Joypad keymap enum
enum
{
   IO_StandardJoypad_LEFT = 0,
   IO_StandardJoypad_RIGHT,
   IO_StandardJoypad_UP,
   IO_StandardJoypad_DOWN,
   IO_StandardJoypad_SELECT,
   IO_StandardJoypad_START,
   IO_StandardJoypad_B,
   IO_StandardJoypad_A,
   IO_StandardJoypad_MAX
};

// Turbo Joypad keymap enum
enum
{
   IO_TurboJoypad_LEFT = 0,
   IO_TurboJoypad_RIGHT,
   IO_TurboJoypad_UP,
   IO_TurboJoypad_DOWN,
   IO_TurboJoypad_SELECT,
   IO_TurboJoypad_START,
   IO_TurboJoypad_B,
   IO_TurboJoypad_A,
   IO_TurboJoypad_BTURBO,
   IO_TurboJoypad_ATURBO,
   IO_TurboJoypad_MAX
};

// Zapper keymap enum
enum
{
   IO_Zapper_FIRE = 0,
   IO_Zapper_MAX
};

// Vaus (Arkanoid) pad keymap enum
enum
{
   IO_Vaus_CCW = 0,
   IO_Vaus_CW,
   IO_Vaus_FIRE,
   IO_Vaus_MAX
};

// Joypads
#define NUM_CONTROLLERS   2
#define CONTROLLER1       0
#define CONTROLLER2       1

// Standard joypad interface
#define JOY_A      0x01
#define JOY_B      0x02
#define JOY_SELECT 0x04
#define JOY_START  0x08
#define JOY_UP     0x10
#define JOY_DOWN   0x20
#define JOY_LEFT   0x40
#define JOY_RIGHT  0x80

// Turbo joypad interface (extends standard joypad interface)
#define JOY_ATURBO 0x100
#define JOY_BTURBO 0x200

// Zapper interface
#define ZAPPER_DETECT  0x08
#define ZAPPER_TRIGGER 0x10

// Vaus(Arkanoid) pad interface
#define VAUS_FIRE 0x08
#define VAUS_POT  0x10
#define VAUS_POT_RANGE 0xA0

// sprintf() replacement macros for speed.
extern const char* hex_char;
#define sprintf_opcode(b,v) \
{ \
   (*(b++)) = (*(v++)); \
   (*(b++)) = (*(v++)); \
   (*(b++)) = (*(v++)); \
   (*(b)) = 0; \
   v-=3; \
}
#define sprintf_db(b) \
{ \
   (*(b++)) = '.'; \
   (*(b++)) = 'D'; \
   (*(b++)) = 'B'; \
   (*(b++)) = ' '; \
   (*(b++)) = '$'; \
   (*(b)) = 0; \
}
#define sprintf_02x(b,v) \
{ \
   (*(b++)) = *(hex_char+((v)>>4)); \
   (*(b++)) = *(hex_char+((v)&0xF)); \
   (*(b)) = 0; \
}

// Version control interfaces.
char* nesGetVersion();

// Exported interfaces.
// The following interfaces are to be used by a UI to interact with the emulation
// core and perform the necessary steps to emulate a NES game.  Those steps are:
// 1. Set the NES system mode to MODE_NTSC or MODE_PAL using nesSetSystemMode().
// 2. Provide a 256x256x3-byte chunk of memory to the emulator core for it to
//    render the NES TV surface onto, using nesSetTVOut().
// 3. Clear any emulation state by using nesUnload().
// 4. Pass 16KB PRG-ROM banks in order and 8KB CHR-ROM banks in order to the emulation
//    core by using nesLoadPRGROMBank() and nesLoadCHRROMBank() respectively.  If no
//    CHR-ROM banks are present, do not call nesLoadCHRROMBank().
// 5. If the game has fixed mirroring, tell the emulator core which one it is by
//    using nesSetHorizontalMirroring() or nesSetVerticalMirroring().
// 6. Tell the emulator core you're done passing it ROM data by using
//    nesLoadROM().
// 7. Cause the loaded ROM to be recognized by the emulator core by issuing a
//    reset.  Use the nesReset(mapper) variant to set up the emulator core with the
//    appropriate mapper for the loaded ROM.  Subsequent resets can use the nesReset()
//    variant.
// 8. Run a PPU (video) frame's worth of NES emulation, which generates a full
//    rendered video frame and an appropriate amount of audio data, by using
//    nesRun().  Pass in collected joypad input data.
// 9. If the emulator supports sound output, the number of audio samples available
//    can be retrieved by using nesGetAudioSamplesAvailable().  A pointer to the
//    next buffer of audio samples to play can be retrieved using nesGetAudioSamples().
//    Once retrieved, the number of available audio samples should be reset using
//    nesClearAudioSamplesAvailable.

// Emulation interfaces.
void nesSetSystemMode ( uint32_t mode );
uint32_t nesGetSystemMode ( void );
void nesSetTVOut ( int8_t* tv );
void nesFrontload ( uint32_t mapper );
void nesLoadPRGROMBank ( uint32_t bank, uint8_t* bankData );
void nesLoadCHRROMBank ( uint32_t bank, uint8_t* bankData );
void nesSetHorizontalMirroring ( void );
void nesSetVerticalMirroring ( void );
void nesSetFourScreen ( void );
void nesFinalizeLoad ( void );
void nesResetInitial ();
void nesReset ( bool soft );
void nesRun ( uint32_t* joypads );
int32_t nesGetAudioSamplesAvailable ( void );
void nesClearAudioSamplesAvailable ( void );
uint8_t* nesGetAudioSamples ( uint16_t samples );
void nesSetControllerType ( int32_t port, int32_t type );
void nesSetControllerScreenPosition ( int32_t port, int32_t px, int32_t py, int32_t wx1, int32_t wy1, int32_t wx2, int32_t wy2 );
void nesSetControllerSpecial ( int32_t port, int32_t special );
bool nesROMIsLoaded ( void );

// Internal debug interfaces.
extern bool __nesdebug;
#define nesIsDebuggable ( __nesdebug )
void nesBreak ( void );
void nesBreakAudio ( void );

CBreakpointInfo* nesGetBreakpointDatabase ( void );
CBreakpointEventInfo** nesGetCpuBreakpointEventDatabase ( void );
int32_t nesGetSizeOfCpuBreakpointEventDatabase ( void );
CBreakpointEventInfo** nesGetPpuBreakpointEventDatabase ( void );
int32_t nesGetSizeOfPpuBreakpointEventDatabase ( void );
CBreakpointEventInfo** nesGetApuBreakpointEventDatabase ( void );
int32_t nesGetSizeOfApuBreakpointEventDatabase ( void );
CBreakpointEventInfo** nesGetCartridgeBreakpointEventDatabase ( void );
int32_t nesGetSizeOfCartridgeBreakpointEventDatabase ( void );

CMemoryDatabase* nesGetCpuMemoryDatabase ( void );
CMemoryDatabase* nesGetPpuNameTableMemoryDatabase ( void );
CMemoryDatabase* nesGetPpuPaletteMemoryDatabase ( void );
CMemoryDatabase* nesGetCartridgeEXRAMMemoryDatabase ( void );
CMemoryDatabase* nesGetCartridgeSRAMMemoryDatabase ( void );
CMemoryDatabase* nesGetCartridgePRGROMMemoryDatabase ( void );
CMemoryDatabase* nesGetCartridgeCHRMemoryDatabase ( void );
CMemoryDatabase* nesGetCartridgeVRAMMemoryDatabase ( void );

CRegisterDatabase* nesGetCpuRegisterDatabase ( void );
CRegisterDatabase* nesGetPpuRegisterDatabase ( void );
CRegisterDatabase* nesGetPpuOamRegisterDatabase ( void );
CRegisterDatabase* nesGetApuRegisterDatabase ( void );
CRegisterDatabase* nesGetCartridgeRegisterDatabase ( void );

CMarker* nesGetExecutionMarkerDatabase ( void );

// General debug interfaces.
void nesEnableDebug ( void );
void nesDisableDebug ( void );
uint32_t nesGetNumColors ( void );
uint32_t nesGetPaletteRedComponent(uint32_t idx);
uint32_t nesGetPaletteGreenComponent(uint32_t idx);
uint32_t nesGetPaletteBlueComponent(uint32_t idx);
void    nesSetPaletteRedComponent(uint32_t idx,uint32_t r);
void    nesSetPaletteGreenComponent(uint32_t idx,uint32_t g);
void    nesSetPaletteBlueComponent(uint32_t idx,uint32_t b);
void nesSetBreakOnKIL ( bool breakOnKIL );
int8_t* nesGetTVOut ( void );
void nesSetMMC5AudioChannelMask ( uint32_t mask );
void nesSetVRC6AudioChannelMask ( uint32_t mask );
void nesSetN106AudioChannelMask ( uint32_t mask );
void nesSetAudioChannelMask ( uint8_t mask );
uint8_t nesGetMemory ( uint32_t addr );
void nesDisassemble ();
void nesDisassembleSingle ( uint8_t* pOpcode, char* buffer );
char* nesGetDisassemblyAtAddress ( uint32_t addr );
void nesGetDisassemblyAtPhysicalAddress ( uint32_t absAddr, char* buffer );
uint32_t nesGetVirtualAddressFromSLOC ( uint16_t sloc );
uint16_t nesGetSLOCFromAddress ( uint32_t addr );
uint32_t nesGetSLOC ( uint32_t addr );
uint32_t nesGetPhysicalAddressFromAddress ( uint32_t addr );
void nesClearOpcodeMasks ( void );
void nesSetOpcodeMask ( uint32_t addr, uint8_t mask );
void nesSetBreakpointHook ( void (*hook)(void) );
void nesSetAudioHook ( void (*hook)(void) );
void nesEnableBreakpoints ( bool enable );
void nesStepCpu ( void );
void nesStepPpu ( void );
void nesStepPpuFrame ( void );
void nesResetInputRecording ( void );
void nesSetInputPlayback ( bool enable );
void nesSetInputRecording ( bool enable );
uint32_t nesGetInputSamplesAvailable ( int32_t port );
void nesGetPrintableAddress ( char* buffer, uint32_t addr );
void nesGetPrintablePhysicalAddress ( char* buffer, uint32_t addr, uint32_t absAddr );

// 6502 debug interfaces.
uint32_t nesGetCPUCycle ( void );
void nesSetGotoAddress ( uint32_t addr );
bool nesCPUIsFetchingOpcode ( void );
bool nesCPUIsWritingMemory ( void );
uint32_t nesGetCPUEffectiveAddress ( void );
uint32_t nesGetCPUMemory ( uint32_t addr );
void nesSetCPUMemory ( uint32_t addr, uint32_t data );
uint32_t nesGetCPURegister ( uint32_t addr );
void nesSetCPURegister ( uint32_t addr, uint32_t data );
uint32_t nesGetCPUProgramCounterOfLastSync ( void );
uint32_t nesGetCPUFlagNegative ( void );
uint32_t nesGetCPUFlagOverflow ( void );
uint32_t nesGetCPUFlagBreak ( void );
uint32_t nesGetCPUFlagDecimal ( void );
uint32_t nesGetCPUFlagInterrupt ( void );
uint32_t nesGetCPUFlagZero ( void );
uint32_t nesGetCPUFlagCarry ( void );
void nesSetCPUFlagNegative ( uint32_t set );
void nesSetCPUFlagOverflow ( uint32_t set );
void nesSetCPUFlagBreak ( uint32_t set );
void nesSetCPUFlagDecimal ( uint32_t set );
void nesSetCPUFlagInterrupt ( uint32_t set );
void nesSetCPUFlagZero ( uint32_t set );
void nesSetCPUFlagCarry ( uint32_t set );

// PPU debug interfaces.
uint32_t nesGetPPUMemory ( uint32_t addr );
void nesSetPPUMemory ( uint32_t addr, uint32_t data );
uint32_t nesGetPPUCycle ( void );
uint32_t nesGetPPURegister ( uint32_t addr );
void nesSetPPURegister ( uint32_t addr, uint32_t data );
uint32_t nesGetPPUFrame ( void );
uint16_t nesGetPPUAddress ( void );
uint8_t nesGetPPUOAMAddress ( void );
uint8_t nesGetPPUReadLatch ( void );
uint16_t nesGetPPUAddressLatch ( void );
int32_t nesGetPPUFlipFlop ( void );
uint8_t nesGetPPUNameTableData ( uint16_t addr );
uint8_t nesGetPPUAttributeTableData ( uint16_t addr );
uint8_t nesGetPPUPatternData ( uint16_t addr );
uint8_t nesGetPPUPaletteData ( uint8_t addr );
uint32_t nesGetPPUOAM ( uint32_t addr );
void nesSetPPUOAM ( uint32_t addr, uint32_t data );
uint16_t nesGetScrollXAtXY ( int32_t x, int32_t y );
uint16_t nesGetScrollYAtXY ( int32_t x, int32_t y );
void nesGetLastSprite0Hit ( uint8_t* x, uint8_t* y );
void nesGetCurrentPixel ( uint8_t* x, uint8_t* y );
void nesGetCurrentScroll ( uint8_t* x, uint8_t* y );
void nesGetMirroring ( uint16_t* sc1, uint16_t* sc2, uint16_t* sc3, uint16_t* sc4 );

// APU debug interfaces.
uint32_t nesGetAPUCycle ( void );
uint32_t nesGetAPURegister ( uint32_t addr );
void nesSetAPURegister ( uint32_t addr, uint32_t data );
int32_t nesGetAPUSequencerMode ( void );
void nesGetAPULengthCounters( uint16_t* sq1,
                              uint16_t* sq2,
                              uint16_t* triangle,
                              uint16_t* noise,
                              uint16_t* dmc);
void nesGetAPUTriangleLinearCounter ( uint8_t* triangle );
void nesGetAPUDACs ( uint8_t* sq1,
                     uint8_t* sq2,
                     uint8_t* triangle,
                     uint8_t* noise,
                     uint8_t* dmc );
void nesGetAPUDMCIRQ ( bool* enabled,
                       bool* asserted );
void nesGetAPUDMCSampleInfo ( uint16_t* addr,
                              uint16_t* length,
                              uint16_t* pos );
void nesGetAPUDMCDMAInfo ( uint8_t* buffer,
                           bool* full );

// Cartridge debug interfaces.
uint32_t nesGetNumPRGROMBanks ( void );
uint32_t nesGetPRGROMSize ( void );
uint32_t nesGetNumCHRROMBanks ( void );
bool nesIsCHRRAM ( void );
uint8_t nesGetMapper ( void );
uint32_t nesMapperLowRead ( uint32_t addr );
void nesMapperLowWrite ( uint32_t addr, uint32_t data );
uint32_t nesMapperHighRead ( uint32_t addr );
void nesMapperHighWrite ( uint32_t addr, uint32_t data );
uint32_t nesGetPRGROMPhysicalAddress ( uint32_t addr );
uint32_t nesGetCHRMEMPhysicalAddress ( uint32_t addr );
uint32_t nesGetPRGROMData ( uint32_t addr );
uint32_t nesGetCHRMEMData ( uint32_t addr );
void nesSetCHRMEMData ( uint32_t addr, uint32_t data );
uint32_t nesGetSRAMPhysicalAddress ( uint32_t addr );
uint32_t nesGetSRAMDataVirtual ( uint32_t addr );
void nesSetSRAMDataVirtual ( uint32_t addr, uint32_t data );
uint32_t nesGetSRAMDataPhysical ( uint32_t addr );
void nesSetSRAMDataPhysical ( uint32_t addr, uint32_t data );
void nesLoadSRAMDataPhysical ( uint32_t addr, uint32_t data );
bool nesIsSRAMDirty ();
uint32_t nesGetEXRAMPhysicalAddress ( uint32_t addr );
uint32_t nesGetEXRAMData ( uint32_t addr );
void nesSetEXRAMData ( uint32_t addr, uint32_t data );
uint32_t nesGetVRAMData ( uint32_t addr );
void nesSetVRAMData ( uint32_t addr, uint32_t data );
bool nesMapperRemapsPRGROM ( void );
bool nesMapperRemapsCHRMEM ( void );
uint32_t nesMapperRemappedVMEMSize ( void );
typedef struct
{
   uint32_t cycle;
   uint8_t memory[MEM_2KB];
   uint16_t pc;
   uint16_t pcAtSync;
   uint8_t sp;
   uint8_t a;
   uint8_t x;
   uint8_t y;
   uint8_t f;
} NESCpuStateSnapshot;

void nesGetCpuSnapshot(NESCpuStateSnapshot* pSnapshot);

typedef struct
{
   uint32_t frame;
   uint32_t cycle;
   uint8_t memory[MEM_16KB];
   uint8_t oamMemory[MEM_256B];
   uint8_t paletteMemory[MEM_32B];
   uint8_t reg[NUM_PPU_REGS];
   uint16_t xOffset[256][240];
   uint16_t yOffset[256][240];
} PpuStateSnapshot;

void nesGetPpuSnapshot(PpuStateSnapshot* pSnapshot);

typedef struct
{
   uint32_t cycle;
   int32_t sequencerMode;
   uint8_t reg[NUM_APU_REGS];
   uint16_t lengthCounter[NUM_APU_CHANNELS];
   uint8_t triangleLinearCounter;
   uint8_t dac[NUM_APU_CHANNELS];
   bool dmcIrqEnabled;
   bool dmcIrqAsserted;
   uint8_t dmcDmaBuffer;
   bool dmcDmaFull;
   uint16_t dmaSampleAddress;
   uint16_t dmaSampleLength;
   uint16_t dmaSamplePosition;
} ApuStateSnapshot;

void nesGetApuSnapshot(ApuStateSnapshot* pSnapshot);

typedef struct
{
   NESCpuStateSnapshot cpu;
   PpuStateSnapshot ppu;
   ApuStateSnapshot apu;
} NesStateSnapshot;

void nesGetNesSnapshot(NesStateSnapshot* pSnapshot);

// Mapper-specific debug interfaces
typedef struct _nesMapper001Info
{
   uint32_t shiftRegister;
   uint32_t shiftRegisterBit;
} nesMapper001Info;
void nesMapper001GetInformation ( nesMapper001Info* pInfo );

typedef struct _nesMapper004Info
{
   uint32_t irqEnabled;
   uint32_t irqAsserted;
   uint32_t irqReload;
   uint32_t irqLatch;
   uint32_t irqCounter;
   uint32_t ppuAddrA12;
   uint32_t ppuCycle;
} nesMapper004Info;
void nesMapper004GetInformation ( nesMapper004Info* pInfo );

typedef struct _nesMapper005Info
{
   uint32_t irqEnabled;
   uint32_t irqAsserted;
   uint32_t irqScanline;
   uint32_t sprite8x16Mode;
   uint32_t ppuCycle;
} nesMapper005Info;
void nesMapper005GetInformation ( nesMapper005Info* pInfo );

typedef struct _nesMapper009010Info
{
   uint32_t latch0FD;
   uint32_t latch0FE;
   uint32_t latch1FD;
   uint32_t latch1FE;
   uint32_t latch0;
   uint32_t latch1;
} nesMapper009010Info;
void nesMapper009GetInformation ( nesMapper009010Info* pInfo );
void nesMapper010GetInformation ( nesMapper009010Info* pInfo );

typedef struct _nesMapper016Info
{
   uint32_t irqEnabled;
   uint32_t irqAsserted;
   uint32_t irqCounter;
   uint32_t eepromState;
   uint32_t eepromBitCounter;
   uint32_t eepromCmd;
   uint32_t eepromAddr;
   uint32_t eepromDataBuf;
} nesMapper016Info;
void nesMapper016GetInformation ( nesMapper016Info* pInfo );

typedef struct _nesMapper028Info
{
   uint32_t regSel;
   uint32_t prgMode;
   uint32_t prgSize;
   uint32_t prgOuterBank;
   uint32_t prgInnerBank;
   uint32_t chrBank;
} nesMapper028Info;
void nesMapper028GetInformation ( nesMapper028Info* pInfo );

typedef struct _nesMapper069Info
{
   uint32_t irqEnabled;
   uint32_t irqCountEnabled;
   uint32_t irqAsserted;
   uint32_t irqCounter;
   uint32_t sramEnabled;
   uint32_t sramIsSram;
   uint32_t sramOrPrgBank;
   uint32_t regSelected;
   uint32_t regValue;
} nesMapper069Info;
void nesMapper069GetInformation ( nesMapper069Info* pInfo );

#ifdef __cplusplus
}
#endif

#endif // EMULATOR_CORE_H
