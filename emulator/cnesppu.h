// cnesppu.h: interface for the CPPU class.
//
//////////////////////////////////////////////////////////////////////

#if !defined ( PPU_H )
#define PPU_H

#include "cnesicidecommon.h"

#include "ctracer.h"
#include "ccodedatalogger.h"

#include "cnesrom.h"

#include "cregisterdata.h"
#include "cbreakpointinfo.h"

// Breakpoint event identifiers
enum
{
   PPU_EVENT_PIXEL_XY = 0,
   PPU_EVENT_PRE_RENDER_SCANLINE_START,
   PPU_EVENT_PRE_RENDER_SCANLINE_END,
   PPU_EVENT_SCANLINE_START,
   PPU_EVENT_SCANLINE_END,
   PPU_EVENT_SPRITE0_HIT,
   PPU_EVENT_SPRITE_SLICE_RENDERING,
   NUM_PPU_EVENTS
};

#define PPUREGBASE 0x2000
#define PPUCTRL    0x2000
#define PPUMASK    0x2001
#define PPUSTATUS  0x2002
#define OAMADDR    0x2003
#define OAMDATA    0x2004
#define PPUSCROLL  0x2005
#define PPUADDR    0x2006
#define PPUDATA    0x2007
#define PPUCTRL_REG   0
#define PPUMASK_REG   1
#define PPUSTATUS_REG 2
#define OAMADDR_REG   3
#define OAMDATA_REG   4
#define PPUSCROLL_REG 5
#define PPUADDR_REG   6
#define PPUDATA_REG   7
#define NUM_PPU_REGS 8

#define MODE_NTSC 0
#define MODE_PAL  1

#define NUM_SPRITES  64
#define SPRITEY      0
#define SPRITEPAT    1
#define SPRITEATT    2
#define SPRITEX      3
#define OAM_SIZE     4
#define NUM_OAM_REGS (NUM_SPRITES*OAM_SIZE)
#define SPRITE_YMAX  0xF0

// OAM attributes
#define SPRITE_PALETTE_IDX_MSK 0x03
#define SPRITE_PRIORITY        0x20
#define SPRITE_FLIP_HORIZ      0x40
#define SPRITE_FLIP_VERT       0x80

// PPUCTRL register
#define PPUCTRL_BASE_NAM_TBL_ADDR_MSK 0x03
#define PPUCTRL_VRAM_ADDR_INC         0x04
#define PPUCTRL_SPRITE_PAT_TBL_ADDR   0x08
#define PPUCTRL_BKGND_PAT_TBL_ADDR    0x10
#define PPUCTRL_SPRITE_SIZE           0x20
#define PPUCTRL_PPU_MASTER_SLAVE      0x40
#define PPUCTRL_GENERATE_NMI          0x80

// PPUMASK register
#define PPUMASK_GREYSCALE               0x01
#define PPUMASK_BKGND_CLIPPING          0x02
#define PPUMASK_SPRITE_CLIPPING         0x04
#define PPUMASK_RENDER_BKGND            0x08
#define PPUMASK_RENDER_SPRITES          0x10
#define PPUMASK_INTENSIFY_REDS          0x20
#define PPUMASK_INTENSIFY_GREENS        0x40
#define PPUMASK_INTENSIFY_BLUES         0x80

// PPUSTATUS register
#define PPUSTATUS_SPRITE_OVFLO        0x20
#define PPUSTATUS_SPRITE_0_HIT        0x40
#define PPUSTATUS_VBLANK              0x80

typedef struct _SpriteBufferData
{
   unsigned char spriteX;
   unsigned char patternData1;
   unsigned char patternData2;
   unsigned char attribData;
   bool          spriteBehind;
   bool          spriteFlipVert;
   bool          spriteFlipHoriz;
   unsigned char spriteIdx;
} SpriteBufferData;

typedef struct _SpriteBuffer
{
   unsigned char count;
   unsigned char order [ 8 ];
   SpriteBufferData data [ 8 ];
} SpriteBuffer;

typedef struct _BackgroundBufferData
{
   unsigned char  patternData1;
   unsigned char  patternData2;
   unsigned char  attribData;
} BackgroundBufferData;

typedef struct _BackgroundBuffer
{
   BackgroundBufferData data [ 2 ];
} BackgroundBuffer;

#define rPALETTE(addr) ( *(m_PALETTEmemory+(addr)) )
#define rPPU(addr) ( *(m_PPUreg+((addr)&0x0007)) )
#define wPPU(addr,data) { *(m_PPUreg+((addr)&0x0007)) = (data); }
#define rPPUADDR() ( m_ppuAddr )
#define rSCROLLX() ( m_ppuScrollX )

class CPPU
{
public:
    CPPU();
    virtual ~CPPU();

   // Emulation routines
   static inline void Move1KBank ( int bank, unsigned char* point ) { if ( bank >= 8 ) m_pPPUmemory[bank-8] = point; }

   static void RESET ( void );
   static void PPU ( UINT addr, unsigned char data );
   static UINT PPU ( UINT addr );
   static void STORE ( UINT addr, unsigned char data, char source = eSource_PPU, char type = eTracer_Unknown );
   static UINT LOAD ( UINT addr, char source = eSource_PPU, char type = eTracer_Unknown );
   static UINT RENDER ( UINT addr, char source );
   static void GARBAGE ( char target );
   static void EXTRA ();
   static inline void _MEM ( UINT addr, unsigned char data ) { STORE(addr,data,false,false); }
   static inline UINT _MEM ( UINT addr ) { return LOAD(addr,false,false); }
   static inline unsigned char _NAMETABLE ( unsigned short addr ) { return *((*(m_pPPUmemory+((addr&0x1FFF)>>10)))+(addr&0x3FF)); }
   static inline unsigned char _ATTRTABLE ( unsigned short addr ) { return *((*(m_pPPUmemory+((addr&0x1FFF)>>10)))+(addr&0x3FF)); }
   static inline unsigned char _PATTERNDATA ( unsigned short addr ) { return CROM::CHRMEM ( addr ); }
   static inline unsigned char _PALETTE ( unsigned char addr ) { if ( !(addr&0x3) ) addr = 0x00; return *(m_PALETTEmemory+addr); }
   static void MEMSET ( UINT addr, unsigned char* data, UINT length ) { memcpy(m_PPUmemory+addr,data,length); }
   static void MEMCLR ( void ) { memset(m_PPUmemory,0,MEM_4KB); }
   static unsigned char* MEMPTR ( void ) { return m_PPUmemory; }
   static void PALETTESET ( unsigned char* data ) { memcpy(m_PALETTEmemory,data,MEM_32B);}
   static void OAMSET ( UINT addr, unsigned char* data, UINT length ) { memcpy(m_PPUoam+addr,data,length); }
   static void OAMCLR ( void ) { memset(m_PPUoam,0,MEM_256B); }
   static unsigned char* OAMPTR ( UINT addr ) { return &(m_PPUoam[addr]); }
   static inline unsigned short _OAMADDR ( void ) { return m_oamAddr; }
   static inline UINT OAM ( UINT oam, UINT sprite ) { return *(m_PPUoam+(sprite*OAM_SIZE)+oam); }
   static inline void OAM ( UINT oam, UINT sprite, unsigned char data ) { *(m_PPUoam+(sprite*OAM_SIZE)+oam) = data; }
   static inline unsigned short _PPUADDR ( void ) { return m_ppuAddr; }
   static inline void _PPU ( UINT addr, unsigned char data ) { *(m_PPUreg+(addr&0x0007)) = data; }
   static inline UINT _PPU ( UINT addr ) { return *(m_PPUreg+(addr&0x0007)); }
   static void MIRROR ( int oneScreen = -1, bool vert = true, bool extraVRAM = false );
   static void MIRRORVERT ( void );
   static void MIRRORHORIZ ( void );
   static void MIRROR ( int nt1, int nt2, int nt3, int nt4 );
   static inline bool VERTMIRROR ( void ) { return m_mirrorVert; }
   static inline bool HORIZMIRROR ( void ) { return m_mirrorHoriz; }
   static inline bool FOURSCREEN ( void ) { return m_extraVRAM; }

   static inline unsigned short _SCROLLX ( int x, int y ) { return *(*(m_2005x+x)+y); }
   static inline unsigned short _SCROLLY ( int x, int y ) { return *(*(m_2005y+x)+y); }

   static inline unsigned char _SPRITE0HITX ( void ) { return m_lastSprite0HitX; }
   static inline unsigned char _SPRITE0HITY ( void ) { return m_lastSprite0HitY; }

   static inline unsigned char _X ( void ) { return m_x; }
   static inline unsigned char _Y ( void ) { return m_y; }

   static inline void FRAMESTART ( void );
   static inline void SCANLINESTART ( void );
   static inline void SCANLINEEND ( void );
   static void RENDERSCANLINE ( int scanline );
   static void NONRENDERSCANLINE ( int scanline );
   static void GATHERBKGND ( void );
   static inline void PIXELPIPELINES ( int x, int off, unsigned char* a, unsigned char* b1, unsigned char* b2 );
   static void GATHERSPRITES ( int scanline );

   static inline void TV ( char* pTV ) { m_pTV = pTV; }

   static inline void EnableCHRMEMInspector ( bool enable ) { m_bCHRMEMInspector = enable; }
   static inline void CHRMEMInspectorTV ( char* pTV ) { m_pCHRMEMInspectorTV = pTV; }
   static inline void EnableOAMInspector ( bool enable ) { m_bOAMInspector = enable; }
   static inline void OAMInspectorTV ( char* pTV ) { m_pOAMInspectorTV = pTV; }
   static inline void EnableNameTableInspector ( bool enable ) { m_bNameTableInspector = enable; }
   static inline void NameTableInspectorTV ( char* pTV ) { m_pNameTableInspectorTV = pTV; }
   static void RENDERCHRMEM ( void );
   static void RENDEROAM ( void );
   static void RENDERNAMETABLE ( void );

   static inline CCodeDataLogger& LOGGER ( void ) { return m_logger; }
   static inline unsigned int CYCLES ( void ) { return m_cycles; }
   static inline void INCCYCLE ( void );
   static inline void RESETCYCLECOUNTER ( void ) { m_cycles = 0; }

   static void SetPPUViewerScanline ( UINT scanline ) { m_iPPUViewerScanline = scanline; }
   static UINT GetPPUViewerScanline ( void ) { return m_iPPUViewerScanline; }
   static void SetOAMViewerScanline ( UINT scanline ) { m_iOAMViewerScanline = scanline; }
   static UINT GetOAMViewerScanline ( void ) { return m_iOAMViewerScanline; }

   static CRegisterData** REGISTERS() { return m_tblRegisters; }
   static int NUMREGISTERS() { return m_numRegisters; }

   static CBreakpointEventInfo** BREAKPOINTEVENTS() { return m_tblBreakpointEvents; }
   static int NUMBREAKPOINTEVENTS() { return m_numBreakpointEvents; }

protected:
   static unsigned char  m_PALETTEmemory [ MEM_32B ];
   static unsigned char  m_PPUmemory [ MEM_4KB ];
   static unsigned char* m_pPPUmemory [ 8 ];
   static int            m_ppuRegByte;
   static unsigned char  m_oamAddr;
   static unsigned short m_ppuAddr;
   static unsigned short m_ppuAddrLatch;
   static unsigned char  m_ppuAddrIncrement;
   static unsigned char  m_ppuReadLatch;
   static unsigned char  m_ppuReadLatch2007;
   static unsigned char  m_PPUreg [ NUM_PPU_REGS ];
   static unsigned char  m_PPUoam [ NUM_OAM_REGS ];
   static unsigned char  m_ppuScrollX;
   static bool           m_mirrorVert;
   static bool           m_mirrorHoriz;
   static int            m_oneScreen;
   static bool           m_extraVRAM;

   static CCodeDataLogger m_logger;
   static unsigned int   m_cycles;
   static unsigned char  m_frame;
   static int            m_curCycles;
   static int            m_mode;

   static unsigned char  m_last2005x;
   static unsigned char  m_last2005y;
   static unsigned short m_2005x [ 256 ][ 240 ];
   static unsigned short m_2005y [ 256 ][ 240 ];

   static unsigned char  m_lastSprite0HitX;
   static unsigned char  m_lastSprite0HitY;

   static unsigned char  m_x;
   static unsigned char  m_y;

   static char           m_szBinaryText [ 70000 ];

   static SpriteBuffer     m_spriteBuffer;
   static BackgroundBuffer m_bkgndBuffer;

   static char*          m_pTV;

   static bool           m_bCHRMEMInspector;
   static char*          m_pCHRMEMInspectorTV;
   static bool           m_bOAMInspector;
   static char*          m_pOAMInspectorTV;
   static bool           m_bNameTableInspector;
   static char*          m_pNameTableInspectorTV;

   static UINT           m_iPPUViewerScanline;
   static UINT           m_iOAMViewerScanline;

   static CRegisterData** m_tblRegisters;
   static int             m_numRegisters;

   static CBreakpointEventInfo** m_tblBreakpointEvents;
   static int                    m_numBreakpointEvents;
};

#endif
