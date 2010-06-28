#if !defined ( PPU_H )
#define PPU_H

#include "cnesicidecommon.h"

#include "ctracer.h"
#include "ccodedatalogger.h"

#include "cnesrom.h"

#include "cregisterdata.h"
#include "cbreakpointinfo.h"

// Breakpoint event identifiers.
// These event identifiers must match the ordering
// of breakpoint events defined in the source module.
enum
{
   PPU_EVENT_PIXEL_XY = 0,
   PPU_EVENT_PRE_RENDER_SCANLINE_START,
   PPU_EVENT_PRE_RENDER_SCANLINE_END,
   PPU_EVENT_SCANLINE_START,
   PPU_EVENT_SCANLINE_END,
   PPU_EVENT_SPRITE_DMA,
   PPU_EVENT_SPRITE0_HIT,
   PPU_EVENT_SPRITE_IN_MULTIPLEXER,
   PPU_EVENT_SPRITE_SELECTED,
   PPU_EVENT_SPRITE_RENDERING,
   PPU_EVENT_SPRITE_OVERFLOW,
   NUM_PPU_EVENTS
};

// Number of scanlines for various calculations.
#define SCANLINES_VISIBLE (240)
#define SCANLINES_QUIET (1)
#define SCANLINES_VBLANK_NTSC (20)
#define SCANLINES_VBLANK_PAL (70)

// NTSC and PAL have different total scanline counts.
#define SCANLINES_TOTAL_NTSC (SCANLINES_VISIBLE+SCANLINES_VBLANK_NTSC+SCANLINES_QUIET+1)
#define SCANLINES_TOTAL_PAL (SCANLINES_VISIBLE+SCANLINES_VBLANK_PAL+SCANLINES_QUIET+1)

// PPU clock cycles 341 times during *most* scanlines.
#define PPU_CYCLES_PER_SCANLINE 341

// The PPU frame driven by the NES object starts at pixel 0, 0
// of the visible screen and goes on through the post-render scanline
// and VBLANK and the pre-render scanline.
#define PPU_CYCLE_START_VBLANK ((SCANLINES_VISIBLE+SCANLINES_QUIET)*PPU_CYCLES_PER_SCANLINE)

// PPU cycles are used as the master cycle of the emulation system.
// To achieve an integer ratio of PPU/CPU/APU cycles the PPU cycle
// counter is incremented by 5 each time.  To arrive at the appropriate
// ratio of PPU-to-CPU cycles the PPU cycle is divided by 15 for NTSC
// and 16 for PAL.  The CPU-to-APU ratio is 1:1 in both NTSC and PAL.
#define CPU_CYCLE_ADJUST   5
#define PPU_CPU_RATIO_NTSC 15
#define PPU_CPU_RATIO_PAL  16

// The Execution Visualizer debugger inspector converts PPU cycles to
// (x,y) coordinates for visualization of the execution of code in the
// PPU frame.
#define CYCLE_TO_VISX(c) (c%PPU_CYCLES_PER_SCANLINE)
#define CYCLE_TO_VISY(c) (c/PPU_CYCLES_PER_SCANLINE)
#define VISY_VISX_TO_CYCLE(y,x) ((y*PPU_CYCLES_PER_SCANLINE)+x)

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

// PPU OAM definitions.
// Total number of sprites in OAM.
#define NUM_SPRITES  64
// OAM 'register' offsets.  Each sprite has
// an entry of each of these four types.
#define SPRITEY      0
#define SPRITEPAT    1
#define SPRITEATT    2
#define SPRITEX      3
#define OAM_SIZE     4
#define NUM_OAM_REGS (NUM_SPRITES*OAM_SIZE)

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

// This structure represents a sprite entry in the secondary OAM
// which is the temporary storage used by the PPU
// on each scanline.  It holds the 8 sprite entries
// that are found to be in-range on the next scanline.
// It is filled during rendering on PPU cycles 256-319
// of each scanline.
typedef struct _SpriteBufferData
{
   // Sprite's X-coordinate.
   unsigned char spriteX;

   // Sprite's first bitplane slice.
   unsigned char patternData1;

   // Sprite's second bitplane slice.
   unsigned char patternData2;

   // Sprite's third and fourth bitplane slices.
   unsigned char attribData;

   // Sprite attributes.
   bool          spriteBehind;
   bool          spriteFlipVert;
   bool          spriteFlipHoriz;

   // Index of sprite in OAM.  Used for prioritization
   // and sprite-0 flagging.
   unsigned char spriteIdx;
} SpriteBufferData;

// This structure represents the secondary OAM
// inside the PPU.  It contains SpriteBufferData
// entries for maximum of 8 possible sprites.
typedef struct _SpriteBuffer
{
   // How many sprite entries are valid in the secondary OAM?
   unsigned char count;

   // Priority order for sprite displaying.
   unsigned char order [ 8 ];

   // Sprite entries.
   SpriteBufferData data [ 8 ];
} SpriteBuffer;

// This structure represents an entry in the 2-entry tile slice
// buffer that is filled by the PPU during cycles 320-335 of
// a scanline.
// The data is fetched at the end of a scanline
// for immediate display on the next scanline to accomodate
// an X-scroll of non-zero where one or more bits of the second
// tile would need to be rendered before it had been completely
// fetched by the tile fetch cycles at the start of the scanline.
// The tile fetched at the start of a scanline is the third tile
// rendered onscreen.
typedef struct _BackgroundBufferData
{
   // Tile's first bitplane slice.
   unsigned char patternData1;

   // Tile's second bitplane slice.
   unsigned char patternData2;

   // Tile's third bitplane slice.
   unsigned char attribData1;

   // Tile's fourth bitplane slice.
   unsigned char attribData2;
} BackgroundBufferData;

// This structure represents the 2-entry tile slice buffer
// stored in the PPU.
typedef struct _BackgroundBuffer
{
   // Tile entries.
   BackgroundBufferData data [ 2 ];
} BackgroundBuffer;

// Macros for internal access to PPU data for use within the PPU object.
#define rPALETTE(addr) ( (*(m_PALETTEmemory+(addr))&0x3F) )
#define rPPU(addr) ( *(m_PPUreg+((addr)&0x0007)) )
#define wPPU(addr,data) { *(m_PPUreg+((addr)&0x0007)) = (data); }
#define rPPUADDR() ( m_ppuAddr )
#define rSCROLLX() ( m_ppuScrollX )

// The CPPU class is the implementation of the PPU of the NES.
// It provides PPU-fetch-cycle granular emulation of the PPU core.
// It is implemented as a static object so that accesses to its
// internal data via accessor functions does not require a class object.
//
// The class internally maintains the state of the PPU's registers
// and of the 2KB of RAM that is on the NES mainboard, dedicated to
// the PPU.  It also internally maintains the state of 32 bytes of
// palette memory and 256 bytes of Object Attribute Map (OAM) memory.
// The class also maintains several data structures used by debugger inspectors.
//
// The PPU object is currently driven by the NES object (see CNES).
// An entire PPU-frame's worth of execution is contained within one
// iteration of the CNES::RUN method.  This may change in the
// future so that the PPU runs itself and the CNES merely provides
// clock ticks.  But, for now, the current CNES::RUN is accurate
// enough.
//
// The current architecture of the emulation engine is:
//
// CNES->CPPU->C6502->CAPU
//
// where the NES object (CNES) uses PPU object APIs to drive
// sub-frame related information down to the PPU (CPPU).  The PPU object
// then executes enough PPU cycles to perform the
// requested action from the NES object (ie. render a scanline).
// During execution of PPU cycles the PPU filters cycles down to
// the CPU core (C6502) by using the current video-mode's filter
// (ie. either 3:1 for NTSC or 3.2:1 for PAL).  The CPU object
// executes instruction cycles when kicked by the PPU object.
// The CPU object also executes APU cycles by passing the execution
// over to the APU object (CAPU).
// There are some slight complications in this scheme such as
// PPU or APU DMA events, but those do not perturb the general
// flow described here too drastically.  The CPU/APU share the same
// cycle frequency (1,789,772.72Hz).  The APU 'frame', however, is
// not at all related to the PPU frame.
class CPPU
{
public:
    CPPU();
    virtual ~CPPU();

   // Emulation routine.  Emulates one PPU cycle.
   static void EMULATE ( void );

   // Routine invoked on reset of the emulation engine.
   // Cleans up the PPU state as if a NES reset had just occurred.
   static void RESET ( void );

   // NTSC/PAL-dependent interfaces.
   // NTSC: 262 scanlines
   // PAL: 312 scanlines
   static inline unsigned int SCANLINES ( void ) { return CNES::VIDEOMODE()==MODE_NTSC?SCANLINES_TOTAL_NTSC:SCANLINES_TOTAL_PAL; }

   // State and internal data accessor interfaces.
   // Read a PPU register, affecting the PPU's internal state.
   // This function is used during emulation.
   static UINT PPU ( UINT addr );

   // Write to a PPU register, affecting the PPU's internal state.
   // This function is used during emulation and also by the
   // debugger inspectors in order to affect the state of the PPU
   // from within the debugger environment.  In other words,
   // either the emulated code or the debugger inspector can write a
   // value to a register that causes a change in the PPU's internal
   // state (such as firing off an NMI).
   static void PPU ( UINT addr, unsigned char data );

   // Read a byte from the PPU's internal OAM memory.
   static inline UINT OAM ( UINT oam, UINT sprite ) { return *(m_PPUoam+(sprite*OAM_SIZE)+oam); }

   // Write a byte to the PPU's internal OAM memory.
   static inline void OAM ( UINT oam, UINT sprite, unsigned char data ) { *(m_PPUoam+(sprite*OAM_SIZE)+oam) = data; }

   // Return the current cycle index of the PPU core.
   // The cycle index is a counter of executed PPU cycles.
   // It will reset to 0 at the end of the current PPU frame.
   // PPU frames are different lengths for NTSC and for PAL.
   // For NTSC, it gets tricky.  If background rendering is ON
   // then all PPU frames are exactly 89,342 cycles long.  If
   // background rendering is OFF then every odd frame is one
   // cycle shorter, or 89,341 cycles long.
   // For PAL, every frame is always 106,392 cycles long.
   // The NES object aligns the PPU frame boundary such that the
   // last cycle of a 'frame' is the last cycle of the pre-render
   // scanline.  This makes it visually appealing for such things
   // as the Execution Visualizer (the emulated image appears in the
   // top left corner always).  For NTSC, also, the missing cycle
   // is at the very bottom of the Execution Visualizer display
   // and thus does not cause the emulated image to shake due to
   // its presence or lack thereof.
   static inline unsigned int _CYCLES ( void ) { return m_cycles; }

   // Return the current cycle index of a DMA transfer between the
   // CPU and PPU.  Used to determine cycle impact of APU DMA.
   static inline unsigned int _DMACYCLES ( void ) { return m_dmaCounter; }

   // Every PPU frame starts at PPU cycle 0.
   static inline void RESETCYCLECOUNTER ( void ) { m_cycles = 0; m_frame++; }

   // Accessor methods to set up or clear the state of the nametable memory
   // maintained internally by the PPU object.
   static void MEMSET ( UINT addr, unsigned char* data, UINT length ) { memcpy(m_PPUmemory+addr,data,length); }
   static void MEMCLR ( void ) { memset(m_PPUmemory,0,MEM_4KB); }

   // Accessor methods to set up or clear the state of the OAM memory
   // maintained internally by the PPU object.
   static void OAMSET ( UINT addr, unsigned char* data, UINT length ) { memcpy(m_PPUoam+addr,data,length); }
   static void OAMCLR ( void ) { memset(m_PPUoam,0,MEM_256B); }

   // Routines to configure or retrieve information about the current
   // memory-mirroring being done by the PPU over its available nametable RAM.
   static void MIRROR ( int oneScreen = -1, bool vert = true, bool extraVRAM = false );
   static void MIRRORVERT ( void );
   static void MIRRORHORIZ ( void );
   static void MIRROR ( int nt1, int nt2, int nt3, int nt4 );
   static inline bool FOURSCREEN ( void ) { return m_extraVRAM; }

   // Framing interfaces.  These are invoked by the NES object at
   // particular places within the PPU frame in order to change
   // the state of the PPU object such that video rendering is accurately
   // portrayed.
   static inline void FRAMESTART ( void );
   static inline void SCANLINESTART ( void );
   static inline void SCANLINEEND ( void );

   // Rendering interfaces.  These are invoked by the NES object at
   // particular places within the PPU frame to run the PPU for a
   // specific number of PPU cycles, usually a multiple of the number
   // of PPU cycles per scanline.
   static void RENDERSCANLINE ( int scanline );
   static void QUIETSCANLINE ( void );
   static void VBLANKSCANLINES ( void );

   // Interface to handle the special case where the setting of the
   // VBLANK flag in the PPU registers is choked by the reading of the
   // PPU register containing the VBLANK flag at a precise point within
   // the PPU frame.
   static inline void VBLANKCHOKED ( bool choked ) { m_vblankChoked = choked; }
   static bool VBLANKCHOKED () { bool choked = m_vblankChoked; m_vblankChoked = false; return choked; }

   // Interface to handle the special case where assertion of NMI can be
   // choked by reading PPU register $2002 at precise points within the
   // PPU frame.
   static inline void NMICHOKED ( bool choked ) { m_nmiChoked = choked; }
   static bool NMICHOKED () { bool choked = m_nmiChoked; m_nmiChoked = false; return choked; }

   // Read a PPU register without changing the PPU's internal state.
   // This routine is used by the debugger in order to retrieve information
   // about the internal state of the PPU without affecting said internal state.
   // It is also used by the emulation to determine what steps to perform as
   // part of the emulation process (ie. generate an NMI to the CPU core or not?)
   static inline UINT _PPU ( UINT addr ) { return *(m_PPUreg+(addr&0x0007)); }

   // Write a PPU register.
   // This routine is used by the emuation in order to change the internal state
   // of the PPU from the perspective of the emulated machine, not because any
   // emulated code made it change.  For example, setting or clearing the VBLANK
   // flag are not necessarily dependent on executed code.
   static inline void _PPU ( UINT addr, unsigned char data ) { *(m_PPUreg+(addr&0x0007)) = data; }

   // Silently read from a memory location visible to the PPU.
   // This routine is used by the debuggers to gather PPU information without
   // impacting the state of the emulation or the visual aspect of any inspectors.
   static inline UINT _MEM ( UINT addr ) { return LOAD(addr,0,0,false); }

   // Silently write to a memory location visible to the PPU.
   // This routine is used by the debuggers to change PPU information without
   // impacting the state of the emulation or the visual aspect of any inspectors.
   static inline void _MEM ( UINT addr, unsigned char data ) { STORE(addr,data,0,0,false); }

   // Silently read from memory locations visible to the PPU.
   // These routines are used by the debuggers to gather PPU information without
   // impacting the state of the emulation.
   static inline unsigned char _NAMETABLE ( unsigned short addr ) { return *((*(m_pPPUmemory+((addr&0x1FFF)>>10)))+(addr&0x3FF)); }
   static inline unsigned char _ATTRTABLE ( unsigned short addr ) { return *((*(m_pPPUmemory+((addr&0x1FFF)>>10)))+(addr&0x3FF)); }
   static inline unsigned char _PATTERNDATA ( unsigned short addr ) { return CROM::CHRMEM ( addr ); }
   static inline unsigned char _PALETTE ( unsigned char addr ) { if ( !(addr&0x3) ) addr = 0x00; return *(m_PALETTEmemory+addr); }

   // Return the internal state of the PPU's hardware such as the OAM address latch, the PPU
   // address latch, and the read-buffer.
   static inline unsigned short _OAMADDR ( void ) { return m_oamAddr; }
   static inline unsigned short _PPUADDR ( void ) { return m_ppuAddr; }
   static inline unsigned char _PPULATCH ( void ) { return m_ppuReadLatch; }

   // Accessor functions for the database of scroll values indexed by pixel
   // location.  At each PPU cycle that is rendering a visible pixel the scroll
   // register values are stored so that a representation of the visible portions
   // of the nametable may be overlaid upon the actual nametable in the nametable visual
   // inspector.
   static inline unsigned short _SCROLLX ( int x, int y ) { return *(*(m_2005x+x)+y); }
   static inline unsigned short _SCROLLY ( int x, int y ) { return *(*(m_2005y+x)+y); }

   // Accessor functions for the pixel coordinate of the last sprite-0 hit.
   static inline unsigned char _SPRITE0HITX ( void ) { return m_lastSprite0HitX; }
   static inline unsigned char _SPRITE0HITY ( void ) { return m_lastSprite0HitY; }

   // Accessor functions for the pixel coordinate of the PPU during rendering.
   static inline unsigned char _X ( void ) { return m_x; }
   static inline unsigned char _Y ( void ) { return m_y; }

   // Returns a running counter of the number of frames that have elapsed
   // during emulation.  At 60Hz this counter will not roll-over until
   // approximately 828 days of emulation have passed.  Note, however, that
   // even then, roll-over of this counter is not a significant event.
   static inline unsigned int _FRAME ( void ) { return m_frame; }

   // Accessor routines to get or set the rendering surface memory to
   // be used by the PPU to render the emulated machine's image.
   static inline void TV ( char* pTV ) { m_pTV = pTV; }
   static inline char* TV ( void ) { return m_pTV; }

   // The CHR memory rendering is performed by the PPU.  The CHR
   // memory is maintained by the ROM object (see CROM).  However,
   // the PPU is the portal through which access to this memory is made.
   // The CHR memory visualization inspector allocates the memory to be
   // rendered to, and passes it to the PPU.  If the CHR memory
   // visualization inspector is not visible the rendering is disabled
   // to save host CPU cycles.
   static inline void EnableCHRMEMInspector ( bool enable ) { m_bCHRMEMInspector = enable; }
   static inline void CHRMEMInspectorTV ( char* pTV ) { m_pCHRMEMInspectorTV = pTV; }

   // The OAM memory rendering is performed by the PPU.  The OAM
   // visualization inspector allocates the memory to be rendered to,
   // and passes it to the PPU.  If the OAM visualization inspector is
   // not visible the rendering is disabled to save host CPU cycles.
   static inline void EnableOAMInspector ( bool enable ) { m_bOAMInspector = enable; }
   static inline void OAMInspectorTV ( char* pTV ) { m_pOAMInspectorTV = pTV; }

   // The nametable memory rendering is performed by the PPU.  The nametable
   // visualization inspector allocates the memory to be rendered to,
   // and passes it to the PPU.  If the nametable visualization inspector
   // is not visible the rendering is disabled to save host CPU cycles.
   static inline void EnableNameTableInspector ( bool enable ) { m_bNameTableInspector = enable; }
   static inline void NameTableInspectorTV ( char* pTV ) { m_pNameTableInspectorTV = pTV; }

   // These functions are invoked at appropriate points in the PPU
   // rendering frame to cause the PPU to render the current state
   // of the CHR, OAM, and nametable memories.  For example, an emulated
   // game might change the CHR memory map in mid PPU frame.  The CHR
   // memory inspector allows visualization of the CHR memory at a
   // specified scanline, thus showing the state of the CHR memory
   // before or after the change.
   static void RENDERCHRMEM ( void );
   static void RENDEROAM ( void );
   static void RENDERNAMETABLE ( void );

   // The CHR memory visualization inspector and the OAM visualization
   // inspector are triggered on a user-defined scanline.  These accessor
   // methods allow the UI to change the scanline-of-interest for the
   // debugger inspectors.
   static void SetPPUViewerScanline ( UINT scanline ) { m_iPPUViewerScanline = scanline; }
   static UINT GetPPUViewerScanline ( void ) { return m_iPPUViewerScanline; }
   static void SetOAMViewerScanline ( UINT scanline ) { m_iOAMViewerScanline = scanline; }
   static UINT GetOAMViewerScanline ( void ) { return m_iOAMViewerScanline; }

   // Accessor method to retrieve the database managed by the PPU
   // of accesses to its internally managed memories for the Code/Data
   // Logger debugger inspector.
   static inline CCodeDataLogger& LOGGER ( void ) { return m_logger; }

   // Interface to retrieve the database defining the registers
   // of the PPU core in a "human readable" form that is used by
   // the Register-type debugger inspector.  The hexadecimal world
   // of the PPU core registers is transformed into textual description
   // and manipulatable bitfield texts by means of this database.
   // PPU registers are declared in the source file.
   static CRegisterData** REGISTERS() { return m_tblRegisters; }
   static int NUMREGISTERS() { return m_numRegisters; }

   // Interface to retrieve the database of PPU core-specific
   // breakpoint events.  A breakpoint event is an event that
   // is typically internal to the PPU core that the user would
   // like to trigger a program halt on for inspection of the emulated
   // machine.  PPU breakpoint events are declared in the source
   // file.
   static CBreakpointEventInfo** BREAKPOINTEVENTS() { return m_tblBreakpointEvents; }
   static int NUMBREAKPOINTEVENTS() { return m_numBreakpointEvents; }

   // The PPU's Code/Data Logger display is generated by the PPU core
   // because the PPU core maintains all of the information necessary
   // to generate it.
   static inline void CodeDataLoggerInspectorTV ( char* pTV ) { m_pCodeDataLoggerInspectorTV = pTV; }
   static void RENDERCODEDATALOGGER ( void );

   // Accessor method used by some ROM mappers that can remap the
   // nametable memory in a more complicated fashion than straight mirroring.
   static inline void Move1KBank ( int bank, unsigned char* point ) { if ( bank >= 8 ) m_pPPUmemory[bank-8] = point; }

protected:
   // Routines to access the RAM maintained by the PPU core object.
   // These are used internally by the PPU core during emulation.
   static void STORE ( UINT addr, unsigned char data, char source = eSource_PPU, char type = eTracer_Unknown, bool trace = true );
   static UINT LOAD ( UINT addr, char source = eSource_PPU, char type = eTracer_Unknown, bool trace = true );

   // Routines to access the RAM maintained by the PPU core object for rendering.
   // These are used internally by the PPU core during emulation.
   static UINT RENDER ( UINT addr, char target );
   static void GARBAGE ( UINT addr, char target );
   static void EXTRA ();

   // Routines that mimic the PPU bus behavior down to the PPU cycle.
   // These are used internally by the PPU core during emulation.
   static void GATHERBKGND ( char phase );
   static void GATHERSPRITES ( int scanline );

   // Routine that mimics the PPU's background barrel-shifters and
   // X-scroll pickoff.
   static inline void PIXELPIPELINES ( int pickoff, unsigned char* a, unsigned char* b1, unsigned char* b2 );

   // Routine that initializes the PPU's palette memory on reset.
   static void PALETTESET ( unsigned char* data ) { memcpy(m_PALETTEmemory,data,MEM_32B);}

protected:
   // The PPU core maintains 32B of palette RAM.
   static unsigned char  m_PALETTEmemory [ MEM_32B ];

   // The PPU core maintains 2KB of video RAM.
   // Some mappers extend this by providing more memory on
   // the cartridge.
   static unsigned char  m_PPUmemory [ MEM_4KB ];

   // The PPU's video memory can be rearranged by some mappers.
   // These pointers support that rearrangement.
   static unsigned char* m_pPPUmemory [ 8 ];

   // The PPU has an internal flip-flop which delivers
   // bytes written to $2005 or $2006 to different locations
   // within the PPU.  The flip-flop flips on each write to
   // PPU address $2005, or $2007.  It is reset on reads from
   // PPU address $2002.
   static int            m_ppuRegByte;

   // The PPU has an internal OAM address register that is
   // accessed via PPU address $2003.  It is incremented on
   // writes to $2004.
   static unsigned char  m_oamAddr;

   // The PPU has an internal address register that is used to
   // generate the bus addresses accessed by the PPU during rendering.
   // It is incremented on read/write of PPU address $2007.  It
   // is updated from the PPU address latch (see below) on every
   // other write to PPU address $2006 (ie. when the flip-flop is flipped
   // in the right direction).
   static unsigned short m_ppuAddr;

   // The PPU has an internal address latch that is programmed
   // via writes to PPU address $2000, $2005, $2006, and $2007.
   // The latch is copied to the address register (see above)
   // on every other write to PPU address $2006.
   static unsigned short m_ppuAddrLatch;

   // The PPU keeps track internally of whether it should be
   // auto-incrementing its internal address register by 1
   // (for horizontal updating) or by 32 (for vertical updating).
   // This is set on writes to PPU address $2000.
   static unsigned char  m_ppuAddrIncrement;

   // The PPU has an internal latch that keeps the last value
   // read by the PPU on an external memory fetch.  This provides
   // a one-read-cycle delay between the PPU and CPU when the
   // CPU requests data (via a read of PPU address $2007) from
   // the PPU's visible memory space.  This latch is necessary so
   // the CPU does not have to be held up while the PPU does the
   // external memory fetch.  The latch does not apply to CPU
   // accesses to PPU addresses in the range $3F00-$3FFF (the PPU's
   // internal palette memory).  That data is returned immediately
   // to the CPU on a read access of PPU address $2007 in that range.
   static unsigned char  m_ppuReadLatch;

   // DMA address for DMA transfers.  The PPU sets this on a DMA
   // request from the CPU and then begins its DMA transfer at the
   // appropriate time.  When not DMAing the counter will be 0.
   static unsigned short m_dmaAddr;
   static unsigned char m_oamAddrForDma;
   static int m_dmaCounter;

   // The PPU has eight registers visible to the CPU in the CPU's
   // memory map at address $2000 through $2007.  These are further
   // mirrored throughout the CPU's memory map up to address $3FFF.
   // The PPU keeps track internally of values written to the register
   // ports so that the debugger inspectors can return actual data
   // written to the registers instead of "open bus".  If the debugger
   // inspectors want to, they can implement a mode where the inspector
   // does infact reflect the actual state of the PPU as seen by the CPU.
   // Typically, however, a user likes to see what has been written.
   // I'll keep the "open bus" in mind though to provide accurate feedback
   // as if the user were staring at an actual PPU.
   static unsigned char  m_PPUreg [ NUM_PPU_REGS ];

   // The PPU has space for 64 objects (sprites) in its internal
   // Object Attribute Map (OAM) memory.
   static unsigned char  m_PPUoam [ NUM_OAM_REGS ];

   // The PPU maintains fine-X (0..7) scroll value which is used as the
   // pick-off location in the barrel-shifter that does the rendering.
   static unsigned char  m_ppuScrollX;

   // The PPU maintains the state of mirroring of its internal video RAM.
   // Some mappers only use one video screen's worth of RAM, others provide
   // extra RAM for up to four independent video screens.  Most mappers use
   // the two video screen's worth of RAM dedicated to the PPU in some
   // mirrored fashion across the memory region which supports up to four
   // video screen's worth.
   static int            m_oneScreen;
   static bool           m_extraVRAM;

   // Database used by the Code/Data Logger debugger inspector.  The data structure
   // is maintained by the PPU core as it performs fetches, reads,
   // and writes to/from its managed RAM.  The
   // Code/Data Logger displays the collected information graphically.
   static CCodeDataLogger m_logger;

   // Bounded counter of PPU cycles executed.  It resets to zero at the
   // start of each PPU frame.
   static unsigned int   m_cycles;

   // Running counter of PPU frames drawn.  It will roll over after
   // approximately 40 minutes of emulation.  However, this roll-over
   // is not a significant event.
   static unsigned int   m_frame;

   // The current number of PPU cycles ready to be executed by
   // the PPU core.
   static int            m_curCycles;

   // If the CPU reads PPU address $2002 at a precise point within the
   // PPU frame it can choke the setting of the VBLANK flag in that register.
   static bool           m_vblankChoked;

   // If the CPU reads PPU address $2002 at precise points within the
   // PPU frame it can choke the assertion of NMI by the PPU.
   static bool           m_nmiChoked;

   // These items are the database that keeps track of the status of the
   // x and y scroll values for each rendered pixel.  This information is
   // used by the nametable visualizer to highlight areas of the nametable
   // memory internal to the PPU that are being rendered to the screen.
   static unsigned char  m_last2005x;
   static unsigned char  m_last2005y;
   static unsigned short m_2005x [ 256 ][ 240 ];
   static unsigned short m_2005y [ 256 ][ 240 ];

   // These items are the position of the last sprite-0 hit event on the
   // last rendered PPU frame.  They are invalidated at the start of each
   // new frame.
   static unsigned char  m_lastSprite0HitX;
   static unsigned char  m_lastSprite0HitY;

   // These items are the position of the pixel being rendered by the
   // PPU on any given PPU cycle.  This information is used by the debugger.
   static unsigned char  m_x;
   static unsigned char  m_y;

   // This is the secondary OAM that is used to keep track of in-view
   // sprites on each scanline.
   static SpriteBuffer     m_spriteBuffer;

   // This is the background buffer of fetched tile data to be
   // rendered on the next scanline.
   static BackgroundBuffer m_bkgndBuffer;

   // This is the rendering surface on which the PPU draws the
   // emulated frame representing the true visual state of the
   // NES as would be seen by a player.  The memory is allocated
   // by the dialog class and passed to the PPU.
   static char*          m_pTV;

   // Flags defining whether or not the various debugger inspectors
   // that are visualizers are enabled (visible) or not.
   static bool           m_bCHRMEMInspector;
   static bool           m_bOAMInspector;
   static bool           m_bNameTableInspector;

   // These are the rendering surfaces on which the PPU draws
   // the data to be visualized by the various debugger inspectors
   // that provide visual information on the state of the NES.
   static char*          m_pCHRMEMInspectorTV;
   static char*          m_pOAMInspectorTV;
   static char*          m_pNameTableInspectorTV;
   static char*          m_pCodeDataLoggerInspectorTV;

   // These are the current scanlines on which the visualizer
   // debugger inspectors that are scanline-triggered are to be updated.
   static UINT           m_iPPUViewerScanline;
   static UINT           m_iOAMViewerScanline;

   // The database for PPU core registers.  Declaration
   // is in source file.
   static CRegisterData** m_tblRegisters;
   static int             m_numRegisters;

   // The database for PPU core breakpoint events.  Declaration
   // is in source file.
   static CBreakpointEventInfo** m_tblBreakpointEvents;
   static int                    m_numBreakpointEvents;
};

#endif
