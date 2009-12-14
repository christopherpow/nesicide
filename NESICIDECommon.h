#if !defined ( NESICIDE_COMMON_H )
#define NESICIDE_COMMON_H

#include <string.h> // for memcpy...
#include <stdio.h> // for sprintf...
#include <windef.h>

// CPTODO: removed everything from here (for now) that is not needed yet...
#define SWAPRB(x) ((x>>16)|(x&0xFF00)|(x<<16))

// CPTODO: temporaries to prevent having to do glob srch/rplc yet
typedef unsigned int UINT;
#define RGB(r,g,b) (r<<16|g<<8|b)

#if 0
extern char hex_char[];
#define sprintf02x(b,v) \
{ \
   (**b) = *(hex_char+((v)>>4)); (*b)++; \
   (**b) = *(hex_char+((v)&0xF)); (*b)++; \
   (**b) = 0; \
}
#define sprintf03x(b,v) \
{ \
   (**b) = *(hex_char+(((v)>>8)&0xF)); (*b)++; \
   (**b) = *(hex_char+(((v)>>4)&0xF)); (*b)++; \
   (**b) = *(hex_char+((v)&0xF)); (*b)++; \
   (**b) = 0; \
}
#define sprintf04x(b,v) \
{ \
   (**b) = *(hex_char+((v)>>12)); (*b)++; \
   (**b) = *(hex_char+(((v)>>8)&0xF)); (*b)++; \
   (**b) = *(hex_char+(((v)>>4)&0xF)); (*b)++; \
   (**b) = *(hex_char+((v)&0xF)); (*b)++; \
   (**b) = 0; \
}
#define sprintf04xc(b,v) \
{ \
   (**b) = *(hex_char+((v)>>12)); (*b)++; \
   (**b) = *(hex_char+(((v)>>8)&0xF)); (*b)++; \
   (**b) = *(hex_char+(((v)>>4)&0xF)); (*b)++; \
   (**b) = *(hex_char+((v)&0xF)); (*b)++; \
   (**b) = ':'; (*b)++; \
   (**b) = 0; \
}
#define sprintfcrlf(b) \
{ \
   (**b) = '\r'; (*b)++; \
   (**b) = '\n'; (*b)++; \
   (**b) = 0; \
}
#endif

// APU emulation stuff
#define FREQ_50HZ  (50.0)
#define FREQ_60HZ  (60.0)
#define NUM_APU_BUFS 240

#if 0
// Project configuration IDs
#define PCI_BUILD_TOOL     0x100000
#define PCI_GLOBAL_DEFINES 0x100001

// File New Project Dialog stuff
typedef enum
{
   e_PROJECT_EMPTY,
   e_PROJECT_NES_CARTRIDGE
};

typedef struct _FileNewRecentDataStruct
{
   int iMRU;
} FileNewRecentDataStruct;

typedef struct _FileNewProjectDataStruct
{
   char szProjectName [ 1024 ];
   char szProjectPath [ 1024 ];
   char szProjectFullPath [ 1024 ];
   int iMapper;
   int iNumPrgBanks;
   int iNumChrBanks;
   BOOL bLaunchBrowser;
   int  type; 
} FileNewProjectDataStruct;

#define SPECIAL 0xFF000000 
class CItemInfo;
typedef struct _FileNewFileDataStruct
{
   char szFileName [ 1024 ];
   char szFilePath [ 1024 ];
   int  type; // Use same enumeration as ItemInfo types...
   int  special; // Use to indicate which type of ROM bank...
   CItemInfo* pIIRom;
} FileNewFileDataStruct;

typedef union _FileNewDataUnion
{
   struct _FileNewRecentDataStruct  rcnt;
   struct _FileNewProjectDataStruct proj;
   struct _FileNewFileDataStruct    file;
} FileNewDataUnion;

typedef struct _FileNewDataStruct
{
   int type;
   FileNewDataUnion data;
} FileNewDataStruct;

// Where popup menu originated...
typedef enum
{
   e_POPUP_ON_TREE,
   e_POPUP_ON_LIST,
   e_POPUP_ON_VIEW
};

// Item types...
typedef enum
{
   e_TYPE_NULL = 0,
   e_TYPE_PRIMITIVES,
   e_TYPE_PRIMITIVES_PAL,
   e_TYPE_PRIMITIVES_PAT,
   e_TYPE_BKGNDS,
   e_TYPE_BKGNDS_ATT,
   e_TYPE_BKGNDS_ASS,
   e_TYPE_BKGNDS_NAM,
   e_TYPE_FGNDS,
   e_TYPE_FGNDS_ASS,
   e_TYPE_SOUNDS,
   e_TYPE_SOUNDS_MUS,
   e_TYPE_SOUNDS_EFX,
   e_TYPE_SCODE,
   e_TYPE_SCODE_SUB,
   e_TYPE_ROM,
   e_TYPE_ROM_INFO,
   e_TYPE_ROM_BANK,
   e_TYPE_ITEM_START,
   e_TYPE_ROM_EMUL_ITEM = e_TYPE_ITEM_START,
   e_TYPE_PRIMITIVES_PAL_ITEM,
   e_TYPE_PRIMITIVES_PAT_ITEM,
   e_TYPE_BKGNDS_ATT_ITEM,
   e_TYPE_BKGNDS_ASS_ITEM,
   e_TYPE_BKGNDS_NAM_ITEM,
   e_TYPE_FGNDS_ASS_ITEM,
   e_TYPE_SOUNDS_MUS_ITEM,
   e_TYPE_SOUNDS_EFX_ITEM,
   e_TYPE_SCODE_SUB_ITEM,
   e_TYPE_ROM_INFO_ITEM,
   e_TYPE_ROM_BANK_ITEM,
   e_TYPE_MAX
};

// Menu IDs...
#define PRIMITIVES_MENU_ID 0
#define BKGNDS_MENU_ID 1
#define FGNDS_MENU_ID 2
#define SOUNDS_MENU_ID 3
#define SOURCECODE_MENU_ID 4
#define ROM_MENU_ID 5
#define PRIMITIVES_PAL_MENU_ID 6
#define PRIMITIVES_PAL_ITEM_MENU_ID 7
#define PRIMITIVES_PAT_MENU_ID 8
#define PRIMITIVES_PAT_ITEM_MENU_ID 9
#define BKGNDS_ATT_MENU_ID 10
#define BKGNDS_ATT_ITEM_MENU_ID 11
#define BKGNDS_ASS_MENU_ID 12
#define BKGNDS_ASS_ITEM_MENU_ID 13
#define BKGNDS_NAM_MENU_ID 14
#define BKGNDS_NAM_ITEM_MENU_ID 15
#define FGNDS_ASS_MENU_ID 16
#define FGNDS_ASS_ITEM_MENU_ID 17
#define SOUNDS_MUS_MENU_ID 18
#define SOUNDS_MUS_ITEM_MENU_ID 19
#define SOUNDS_EFX_MENU_ID 20
#define SOUNDS_EFX_ITEM_MENU_ID 21
#define SOURCECODE_SUB_MENU_ID 22
#define SOURCECODE_SUB_ITEM_MENU_ID 23
#define ROM_INFO_MENU_ID 24
#define ROM_BANK_MENU_ID 25
#define ROM_INFO_ITEM_MENU_ID 26
#define ROM_BANK_ITEM_MENU_ID 27
#define BKGNDS_ATT_VIEW_MENU_ID 28
#define BKGNDS_ASS_VIEW_MENU_ID 29
#define BKGNDS_NAM_VIEW_MENU_ID 30
#define ROM_BANK_VIEW_MENU_ID 31
#define SOUNDS_MUS_VIEW_MENU_ID 32
#define ROM_EMUL_VIEW_MENU_ID 33

#define MODE_PAL_NORMAL 0
#define MODE_PAL_BKGND_COLOR_ONLY 1
#define MODE_SHOW_FOR_LISTBOX 2
#define MODE_PAT_LARGE 3
#define MODE_ASS_LARGE 4
#define MODE_NAM_LARGE 5
#define MODE_USE_EXTRADATA 6

#define TREE_REINIT 0x10000

#define INSERT_ITEM 0x12000
#define SHOW_ITEM 0x12001
#define CHANGE_ITEM_NAME 0x12002
#define DELETE_ITEM 0x12003
#define FORCE_UPDATE 0x12004

#define LIST_SELCHANGED 0x20000
#define LIST_SELCHANGING 0x20001

#define FORM_UPDATE 0x24000

#define TREE_SELCHANGING 0x28000
#define TREE_SELCHANGED 0x28001

#define REDRAW_VIEW 0x29000
#define FINAL_NOTICE 0x80000
#define SHUTDOWN_NOTICE 0x80001

#define PATTERN_ATTRTABLE_LINK_UPDATE 0x30000

#define ATTRTABLE_PALETTE_SELCHANGED 0x40000
#define ATTRTABLE_GLOBAL_BKGND_SELCHANGED 0x40001

#define NAMETABLE_ATTRTABLE_LINK_UPDATE 0x50000
#define NAMETABLE_FORCE_UPDATE 0x50001

#define ASSEMBLY_ATTRTABLE_LINK_UPDATE 0x60000

#define ROM_BANK_UPDATE_PATTERN 0x70000
#define ROM_BANK_VIEW_CHANGED_TO_SOURCE 0x70001
#define ROM_BANK_VIEW_CHANGED_TO_BINARY 0x70002

#define UNDO_STACK_SIZE 100
#endif

#define PATTERN_SIZE 8

#if 0
#define PATTERN_SCALE 4

#define ASSEMBLY_SIZE 16
#define ASSEMBLY_DIM 2

#define NAMETABLE_SIZE_X 256
#define NAMETABLE_DIM_X 32
#define NAMETABLE_SIZE_Y 240
#define NAMETABLE_DIM_Y 30
#endif

#define NUM_PALETTES 64

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

#if 0
#define CPU_MEM 0
#define PPU_REG 1
#define IO_REG 2
#define SRAM_MEM 3
#define EXRAM_MEM 4
#define CHR_MEM 5
#define PPU_MEM 6
#define PPU_PAL 7
#define OAM_MEM 8
#define MAPPER_MEM 9
#define NUM_MEMS 9
#endif

#define EXRAM_START 0x5C00
#define SRAM_START 0x6000
#define PPUMEM_START 0x2000
#define PPUPAL_START 0x3F00
#define PPUREG_START 0x2000
#define IOREG_START 0x4000

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
#define PRG_BANK 0
#define CHR_BANK 1
#define TRN_BANK 2
#define PRG_BANK_SIZE ( 16*1024 )
#define CHR_BANK_SIZE ( 8*1024 )
#define TRN_BANK_SIZE ( 512 )

#define ROM_BANK_BIN_LINE_LEN 16

#define NUM_PLANES 2
#define NUM_BYTES_PER_PLANE 8

#if 0
typedef struct
{
   int  index;
   unsigned char palette [ 4 ];
   unsigned char bit [ PATTERN_SIZE ] [ PATTERN_SIZE ];
} pattern_raw_data_struct;
#endif

// Emulator threads stuff

#define BRKPT_NONE       0
#define BRKPT_CPU_EXEC   1
#define BRKPT_CPU_ACCESS 2
#define BRKPT_CPU_READ   3
#define BRKPT_CPU_WRITE  4
#define BRKPT_CPU_A      5
#define BRKPT_CPU_X      6
#define BRKPT_CPU_Y      7
#define BRKPT_CPU_F      8
#define BRKPT_CPU_SP     9
#define BRKPT_IO_ACCESS  10
#define BRKPT_IO_READ    11
#define BRKPT_IO_WRITE   12
#define BRKPT_PPU_ACCESS 13
#define BRKPT_PPU_READ   14
#define BRKPT_PPU_WRITE  15

#define BRKPT_NA 0
#define BRKPT_EQ 1
#define BRKPT_NE 2
#define BRKPT_LT 3
#define BRKPT_GT 4
#define BRKPT_BS 5
#define BRKPT_BC 6

#define IOPORTREGBASE  0x4000
#define IOSPRITEDMA    0x4014
#define APUCTRL        0x4015
#define IOJOY1         0x4016
#define IOJOY2         0x4017

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

#if 0
typedef enum
{
   eResize_SnapRight    = 0x001,
   eResize_SnapBottom   = 0x002,
   eResize_Resize       = 0x004,
   eResize_AdjustRight  = 0x008,
   eResize_AdjustBottom = 0x010,
   eResize_AdjustBoth   = 0x020,
   eResize_MoveRight    = 0x040,
   eResize_MoveBottom   = 0x080,
   eResize_MoveBoth     = 0x100,
   eResize_SlideRight   = 0x200,
   eResize_SlideBottom  = 0x400,
   eResize_SlideBoth    = 0x800
};

typedef struct _WindowResizes
{
   int id;
   int type;
   int cx;
   int cy;
   int xmin;
   int ymin;
   int xmax;
   int ymax;
} WindowResizes;

// screen coordinate to pixel conversion...
void PointToPixel(CRect& rRect, CPoint &rPoint);
void PointToNameTable ( CRect& rRect, CPoint& rPoint, int size );

#define ASSEMBLER_PASS_IF_PRESENT 0
#define ASSEMBLER_FAIL_IF_PRESENT 1
#define ASSEMBLER_PASS_IF_MISSING 2
#define ASSEMBLER_FAIL_IF_MISSING 3
#endif

#endif // #if !defined ( NESICIDE_COMMON_H )
