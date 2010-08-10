#if !defined ( NESICIDE_COMMON_H )
#define NESICIDE_COMMON_H

#if 0
// Common enumerations for emulated items.
enum
{
   eSource_CPU,
   eSource_PPU,
   eSource_APU,
   eSource_Mapper
};

enum
{
   eTracer_Unknown = 0,
   eTracer_InstructionFetch,
   eTracer_DataRead,
   eTracer_DataWrite,
   eTracer_DMA,
   eTracer_RESET,
   eTracer_NMI,
   eTracer_IRQ,
   eTracer_GarbageRead,
   eTracer_RenderBkgnd,
   eTracer_RenderSprite,
   eTracer_StartPPUFrame,
   eTracer_Sprite0Hit,
   eTracer_VBLANKStart,
   eTracer_VBLANKEnd,
   eTracer_PreRenderStart,
   eTracer_PreRenderEnd,
   eTracer_QuietStart,
   eTracer_QuietEnd,
   eTracer_EndPPUFrame,
   eTracer_StartAPUFrame,
   eTracer_SequencerStep,
   eTracer_EndAPUFrame
};

enum
{
   eTarget_Unknown = 0,
   eTarget_RAM,
   eTarget_PPURegister,
   eTarget_APURegister,
   eTarget_IORegister,
   eTarget_SRAM,
   eTarget_EXRAM,
   eTarget_ROM,
   eTarget_Mapper,
   eTarget_PatternMemory,
   eTarget_NameTable,
   eTarget_AttributeTable,
   eTarget_Palette,
   eTarget_ExtraCycle
};

typedef enum
{
   eMemory_CPU = 0,
   eMemory_CPUregs,
   eMemory_PPUregs,
   eMemory_PPU,
   eMemory_PPUpalette,
   eMemory_PPUoam,
   eMemory_IOregs,
   eMemory_cartROM,
   eMemory_cartSRAM,
   eMemory_cartEXRAM,
   eMemory_cartCHRMEM,
   eMemory_cartMapper
} eMemoryType;

// Video modes.  The emulator supports both NTSC and PAL.
#define MODE_NTSC 0
#define MODE_PAL  1

#define NUM_ROM_BANKS 64
#define NUM_SRAM_BANKS 8

#define PATTERN_SIZE 8

#define NUM_PALETTES 64

// Samples per video frame for 44.1KHz audio output.
// NTSC is 60Hz, PAL is 50Hz.  The number of samples
// drives the rate at which the SDL library will invoke
// the callback method to retrieve more audio samples to
// play.
#define APU_SAMPLES_NTSC 735
#define APU_SAMPLES_PAL  882

#define APU_BUFFER_PRERENDER_THRESHOLD (735*3)   // When to start doing more rendering
#define APU_BUFFER_PRERENDER           (735*3)  // How much rendering to do

#pragma pack(1)
typedef struct
{
   unsigned int id;
   unsigned char numPrgBanks;
   unsigned char numChrBanks;
   unsigned char flags:4;
   unsigned char mapper1:4;
   unsigned char romType:2;
   unsigned char unused1:2;
   unsigned char mapper2:4;
   unsigned char unused2 [ 8 ];
} iNES_header_struct;
#pragma pack()

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
#define UPSHIFT_32KB 15
#define MEM_64KB 0x10000
#define MASK_64KB 0xFFFF
#define SHIFT_64KB_8KB 13

#define EXRAM_START 0x5C00
#define SRAM_START 0x6000
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

// Joypads
#define NUM_JOY    2
#define JOY1       0
#define JOY2       1
#define JOY_A      0x01
#define JOY_B      0x02
#define JOY_SELECT 0x04
#define JOY_START  0x08
#define JOY_UP     0x10
#define JOY_DOWN   0x20
#define JOY_LEFT   0x40
#define JOY_RIGHT  0x80

#define JOY1_LEFT   'a'
#define JOY1_RIGHT  'd'
#define JOY1_UP     'w'
#define JOY1_DOWN   's'
#define JOY1_SELECT '['
#define JOY1_START  ']'
#define JOY1_A      '?'
#define JOY1_B      '>'

#define JOY2_LEFT   'j'
#define JOY2_RIGHT  'l'
#define JOY2_UP     'i'
#define JOY2_DOWN   'k'
#define JOY2_SELECT '-'
#define JOY2_START  '='
#define JOY2_A      '3'
#define JOY2_B      '2'

#define idxJOY_CONNECTED 0
#define idxJOY_LEFT      1
#define idxJOY_RIGHT     2
#define idxJOY_UP        3
#define idxJOY_DOWN      4
#define idxJOY_SELECT    5
#define idxJOY_START     6
#define idxJOY_B         7
#define idxJOY_A         8

#endif

#endif // #if !defined ( NESICIDE_COMMON_H )
