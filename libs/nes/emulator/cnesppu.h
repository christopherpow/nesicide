#if !defined ( PPU_H )
#define PPU_H

#include "nes_emulator_core.h"

#include "ctracer.h"
#include "ccodedatalogger.h"

#include "cnesrom.h"

// Rudimentary PPU I/O bus decay algorithm simply counts PPU frames to get
// "close" to 600 milliseconds of time elapsed for a single bit to decay.
#define PPU_DECAY_FRAME_COUNT_NTSC  36
#define PPU_DECAY_FRAME_COUNT_PAL   30
#define PPU_DECAY_FRAME_COUNT_DENDY 30

// PPU cycles are used as the master cycle of the emulation system.
// To achieve an integer ratio of PPU/CPU/APU cycles the PPU cycle
// counter is incremented by 5 each time.  To arrive at the appropriate
// ratio of PPU-to-CPU cycles the PPU cycle is divided by 15 for NTSC
// and 16 for PAL.  The CPU-to-APU ratio is 1:1 in both NTSC and PAL.
#define CPU_CYCLE_ADJUST    5
#define PPU_CPU_RATIO_NTSC  15
#define PPU_CPU_RATIO_PAL   16
#define PPU_CPU_RATIO_DENDY 15

// This structure represents a sprite entry in the
// sprite temporary memory which is the memory used
// by the PPU during pixel rendering to store accumulated
// OAM data found during sprite evaluation.
typedef struct _SpriteTemporaryMemoryData
{
   // Index of sprite in OAM.  Used for prioritization
   // and sprite-0 flagging.
   uint8_t spriteIdx;

   // Sprite's Y-slice.  (Y-coordinate less scanline).
   uint8_t spriteSlice;

   // Sprite pattern index in CHR memory.
   uint8_t patternIdx;

   // Sprite's third and fourth bitplane slices.
   uint8_t attribData;

   // Sprite's X-coordinate.
   uint8_t spriteX;
} SpriteTemporaryMemoryData;

// This structure represents the secondary OAM
// inside the PPU.  It contains SpriteBufferData
// entries for maximum of 8 possible sprites.
typedef struct _SpriteTemporaryMemory
{
   // How many sprite entries are valid in the temporary memory?
   uint8_t count;

   // Which sprite is being evaluated?
   uint8_t sprite;

   // Which fetch phase is the PPU in?
   uint8_t phase;

   // Which byte is evaluated as sprite Y coordinate?
   uint8_t yByte;

   // Have more than 8 sprites been detected, at which point the
   // sprite prioritizer starts doing funky stuff using the wrong
   // OAM bytes as sprite Y coordinates?
   uint8_t rolling;

   // Sprite entries.
   SpriteTemporaryMemoryData data [ NUM_SPRITES_PER_SCANLINE ];
} SpriteTemporaryMemory;

// This structure represents a sprite entry in the secondary OAM
// which is the temporary storage used by the PPU
// on each scanline.  It holds the 8 sprite entries
// that are found to be in-range on the next scanline.
// It is filled during rendering on PPU cycles 256-319
// of each scanline.
typedef struct _SpriteBufferData
{
   // Copy of temporary memory data for sprite.
   SpriteTemporaryMemoryData temp;

   // Sprite's first bitplane slice.
   uint8_t patternData1;

   // Sprite's second bitplane slice.
   uint8_t patternData2;

   // Sprite attributes.
   bool          spriteBehind;
   bool          spriteFlipVert;
   bool          spriteFlipHoriz;
} SpriteBufferData;

// This structure represents the secondary OAM
// inside the PPU.  It contains SpriteBufferData
// entries for maximum of 8 possible sprites.
typedef struct _SpriteBuffer
{
   // How many sprite entries are valid in the secondary OAM?
   uint8_t count;

   // Sprite entries.
   SpriteBufferData data [ NUM_SPRITES_PER_SCANLINE ];
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
   uint8_t patternData1;

   // Tile's second bitplane slice.
   uint8_t patternData2;

   // Tile's third bitplane slice.
   uint8_t attribData1;

   // Tile's fourth bitplane slice.
   uint8_t attribData2;
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
#define wPPU(addr,data) { *(m_PPUreg+((addr)&0x0007)) = (data); CNES::CHECKBREAKPOINT(eBreakInPPU,eBreakOnPPUState,addr&0x0007); }
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
// sub-frame events (screen rendering, VBLANK, etc.) to the PPU (CPPU).
// The PPU object then executes enough PPU cycles to perform the
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
// not at all related to the PPU frame.  The CPU itself has no concept
// of 'frame'.
class CPPU
{
public:
   CPPU();
   ~CPPU();

   // Emulation routine.  Emulates one PPU cycle.
   static inline void EMULATE ( uint32_t cycles );

   // Routine invoked on reset of the emulation engine.
   // Cleans up the PPU state as if a NES reset had just occurred.
   static void RESET ( bool soft );

   // State and internal data accessor interfaces.
   // Read a PPU register, affecting the PPU's internal state.
   // This function is used during emulation.
   static uint32_t PPU ( uint32_t addr );

   // Write to a PPU register, affecting the PPU's internal state.
   // This function is used during emulation and also by the
   // debugger inspectors in order to affect the state of the PPU
   // from within the debugger environment.  In other words,
   // either the emulated code or the debugger inspector can write a
   // value to a register that causes a change in the PPU's internal
   // state (such as firing off an NMI).
   static void PPU ( uint32_t addr, uint8_t data );

   // Read a byte from the PPU's internal OAM memory.
   static inline uint32_t OAM ( uint32_t oam, uint32_t sprite )
   {
      return *(m_PPUoam+(sprite*OAM_SIZE)+oam);
   }

   // Write a byte to the PPU's internal OAM memory.
   static inline void OAM ( uint32_t oam, uint32_t sprite, uint8_t data )
   {
      *(m_PPUoam+(sprite*OAM_SIZE)+oam) = data;
   }

   // Read a byte from the PPU's internal OAM memory.
   static inline uint32_t _OAM ( uint32_t oam, uint32_t sprite )
   {
      return *(m_PPUoam+(sprite*OAM_SIZE)+oam);
   }

   // Write a byte to the PPU's internal OAM memory.
   static inline void _OAM ( uint32_t oam, uint32_t sprite, uint8_t data )
   {
      *(m_PPUoam+(sprite*OAM_SIZE)+oam) = data;
   }

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
   static inline uint32_t _CYCLES ( void )
   {
      return m_cycles;
   }

   // Every PPU frame starts at PPU cycle 0.
   static inline void RESETCYCLECOUNTER ( void )
   {
      m_cycles = 0;
      m_frame++;
   }

   // Accessor methods to set up or clear the state of the nametable memory
   // maintained internally by the PPU object.
   static void MEMSET ( uint32_t addr, uint8_t* data, uint32_t length )
   {
      memcpy(m_PPUmemory+addr,data,length);
   }
   static void MEMCLR ( void )
   {
      memset(m_PPUmemory,0,MEM_4KB);
   }

   // Accessor methods to set up or clear the state of the OAM memory
   // maintained internally by the PPU object.
   static void OAMSET ( uint32_t addr, uint8_t* data, uint32_t length )
   {
      memcpy(m_PPUoam+addr,data,length);
   }
   static void OAMCLR ( void )
   {
      memset(m_PPUoam,0,MEM_256B);
   }

   // Routines to configure or retrieve information about the current
   // memory-mirroring being done by the PPU over its available nametable RAM.
   static void MIRROR ( int32_t oneScreen = -1, bool vert = true, bool extraVRAM = false );
   static void MIRRORVERT ( void );
   static void MIRRORHORIZ ( void );
   static void MIRROR ( int32_t b0, int32_t b1, int32_t b2, int32_t b3 );
   static inline bool FOURSCREEN ( void )
   {
      return m_extraVRAM;
   }

   // Rendering interfaces.  These are invoked by the NES object at
   // particular places within the PPU frame to run the PPU for a
   // specific number of PPU cycles, usually a multiple of the number
   // of PPU cycles per scanline.
   static void RENDERSCANLINE ( int32_t scanline );
   static void QUIETSCANLINES ( void );
   static void VBLANKSCANLINES ( void );

   // Interface to handle the special case where the setting of the
   // VBLANK flag in the PPU registers is choked by the reading of the
   // PPU register containing the VBLANK flag at a precise point within
   // the PPU frame.
   static inline void VBLANKCHOKED ( bool choked )
   {
      m_vblankChoked = choked;
   }
   static inline bool VBLANKCHOKED ()
   {
      bool choked = m_vblankChoked;
      m_vblankChoked = false;
      return choked;
   }

   // Interface to handle the special case where assertion of NMI can be
   // choked by reading PPU register $2002 at precise points within the
   // PPU frame.
   static inline void NMICHOKED ( bool choked )
   {
      m_nmiChoked = choked;
   }
   static inline bool NMICHOKED ()
   {
      return m_nmiChoked;
   }

   // Interface to handle the special case where assertion of NMI can be
   // forced to occur more than once during VBLANK if the PPU's NMI enablement
   // bit is cleared and re-set during the VBLANK period.
   static inline void NMIREENABLED ( bool reenabled )
   {
      m_nmiReenabled = reenabled;
   }
   static inline bool NMIREENABLED ()
   {
      bool reenabled = m_nmiReenabled;
      m_nmiReenabled = false;
      return reenabled;
   }

   // Interface to retrieve RGB value of a pixel to support things like Zapper.
   static void PIXELRGB ( int32_t x, int32_t y, uint8_t* r, uint8_t* g, uint8_t* b );

   // Read a PPU register without changing the PPU's internal state.
   // This routine is used by the debugger in order to retrieve information
   // about the internal state of the PPU without affecting said internal state.
   // It is also used by the emulation to determine what steps to perform as
   // part of the emulation process (ie. generate an NMI to the CPU core or not?)
   static inline uint32_t _PPU ( uint32_t addr )
   {
      return *(m_PPUreg+(addr&0x0007));
   }

   // Write a PPU register.
   // This routine is used by the emuation in order to change the internal state
   // of the PPU from the perspective of the emulated machine, not because any
   // emulated code made it change.  For example, setting or clearing the VBLANK
   // flag are not necessarily dependent on executed code.
   static inline void _PPU ( uint32_t addr, uint8_t data )
   {
      *(m_PPUreg+(addr&0x0007)) = data;
   }

   // Silently read from a memory location visible to the PPU.
   // This routine is used by the debuggers to gather PPU information without
   // impacting the state of the emulation or the visual aspect of any inspectors.
   static inline uint32_t _MEM ( uint32_t addr )
   {
      return LOAD(addr,0,0,false);
   }

   // Silently write to a memory location visible to the PPU.
   // This routine is used by the debuggers to change PPU information without
   // impacting the state of the emulation or the visual aspect of any inspectors.
   static inline void _MEM ( uint32_t addr, uint8_t data )
   {
      STORE(addr,data,0,0,false);
   }

   // Silently read from memory locations visible to the PPU.
   // These routines are used by the debuggers to gather PPU information without
   // impacting the state of the emulation.
   static inline uint8_t _NAMETABLE ( uint16_t addr )
   {
      return *((*(m_pPPUmemory+((addr&0x1FFF)>>10)))+(addr&0x3FF));
   }
   static inline uint8_t _ATTRTABLE ( uint16_t addr )
   {
      return *((*(m_pPPUmemory+((addr&0x1FFF)>>10)))+(addr&0x3FF));
   }
   static inline uint8_t _PATTERNDATA ( uint16_t addr )
   {
      return CROM::CHRMEM ( addr );
   }
   static inline uint8_t _PALETTE ( uint8_t addr )
   {
      if ( !(addr&0x3) )
      {
         addr = 0x00;
      }

      return *(m_PALETTEmemory+addr);
   }

   // Return the internal state of the PPU's hardware such as the OAM address latch, the PPU
   // address latch, and the read-buffer.
   static inline uint8_t _OAMADDR ( void )
   {
      return m_oamAddr;
   }
   static inline uint16_t _PPUADDR ( void )
   {
      return m_ppuAddr;
   }
   static inline uint8_t _PPUREADLATCH ( void )
   {
      return m_ppuReadLatch;
   }
   static inline uint16_t _PPUADDRLATCH ( void )
   {
      return m_ppuAddrLatch;
   }
   static inline int32_t _PPUFLIPFLOP ( void )
   {
      return m_ppuRegByte;
   }
   static inline void _MIRROR(uint16_t* sc1, uint16_t* sc2, uint16_t* sc3, uint16_t* sc4)
   {
      (*sc1) = ((uint8_t*)m_pPPUmemory[0]-(uint8_t*)m_PPUmemory)+MEM_8KB;
      (*sc2) = ((uint8_t*)m_pPPUmemory[1]-(uint8_t*)m_PPUmemory)+MEM_8KB;
      (*sc3) = ((uint8_t*)m_pPPUmemory[2]-(uint8_t*)m_PPUmemory)+MEM_8KB;
      (*sc4) = ((uint8_t*)m_pPPUmemory[3]-(uint8_t*)m_PPUmemory)+MEM_8KB;
   }

   // Accessor functions for the database of scroll values indexed by pixel
   // location.  At each PPU cycle that is rendering a visible pixel the scroll
   // register values are stored so that a representation of the visible portions
   // of the nametable may be overlaid upon the actual nametable in the nametable visual
   // inspector.
   static inline uint16_t _SCROLLX ( int32_t x, int32_t y )
   {
      return *(*(m_2005x+x)+y);
   }
   static inline uint16_t _SCROLLY ( int32_t x, int32_t y )
   {
      return *(*(m_2005y+x)+y);
   }
   static inline void _SCROLL ( uint8_t* x, uint8_t* y )
   {
      (*x) = m_last2005x;
      (*y) = m_last2005y;
   }

   // Accessor functions for the pixel coordinate of the last sprite-0 hit.
   static inline uint8_t _SPRITE0HITX ( void )
   {
      return m_lastSprite0HitX;
   }
   static inline uint8_t _SPRITE0HITY ( void )
   {
      return m_lastSprite0HitY;
   }

   // Returns a running counter of the number of frames that have elapsed
   // during emulation.  At 60Hz this counter will not roll-over until
   // approximately 828 days of emulation have passed.  Note, however, that
   // even then, roll-over of this counter is not a significant event.
   static inline uint32_t _FRAME ( void )
   {
      return m_frame;
   }

   // Accessor functions for the pixel coordinate of the PPU during rendering.
   static inline uint8_t _X ( void )
   {
      return m_x;
   }
   static inline uint8_t _Y ( void )
   {
      return m_y;
   }

   // Accessor routines to get or set the rendering surface memory to
   // be used by the PPU to render the emulated machine's image.
   static inline void TV ( int8_t* pTV )
   {
      m_pTV = pTV;
   }
   static inline int8_t* TV ( void )
   {
      return m_pTV;
   }

   // Accessor method used by some ROM mappers that can remap the
   // nametable memory in a more complicated fashion than straight mirroring.
   static inline void REMAPVRAM ( int32_t bank, uint8_t* point )
   {
      m_pPPUmemory[bank] = point;
   }

   // Accessor method to retrieve the database managed by the PPU
   // of accesses to its internally managed memories for the Code/Data
   // Logger debugger inspector.
   static inline CCodeDataLogger* LOGGER ( void )
   {
      return m_logger;
   }

   // Interface to retrieve the database defining the registers
   // of the PPU core in a "human readable" form that is used by
   // the Register-type debugger inspector.  The hexadecimal world
   // of the PPU core registers is transformed into textual description
   // and manipulatable bitfield texts by means of this database.
   // PPU registers are declared in the source file.
   static CRegisterDatabase* REGISTERS()
   {
      return m_dbRegisters;
   }
   static CRegisterDatabase* OAMREGISTERS()
   {
      return m_dbOamRegisters;
   }

   // Interface to retrieve the database of PPU core-specific
   // breakpoint events.  A breakpoint event is an event that
   // is typically internal to the PPU core that the user would
   // like to trigger a program halt on for inspection of the emulated
   // machine.  PPU breakpoint events are declared in the source
   // file.
   static CBreakpointEventInfo** BREAKPOINTEVENTS()
   {
      return m_tblBreakpointEvents;
   }
   static int32_t NUMBREAKPOINTEVENTS()
   {
      return m_numBreakpointEvents;
   }

   static CMemoryDatabase* PALETTEMEMORY()
   {
      return m_dbPaletteMemory;
   }

   static CMemoryDatabase* NAMETABLEMEMORY()
   {
      return m_dbNameTableMemory;
   }

protected:
   // Routines to access the RAM maintained by the PPU core object.
   // These are used internally by the PPU core during emulation.
   static void STORE ( uint32_t addr, uint8_t data, int8_t source = eNESSource_PPU, int8_t type = eTracer_Unknown, bool trace = true );
   static uint32_t LOAD ( uint32_t addr, int8_t source = eNESSource_PPU, int8_t type = eTracer_Unknown, bool trace = true );

   // Routines to access the RAM maintained by the PPU core object for rendering.
   // These are used internally by the PPU core during emulation.
   static inline uint32_t RENDER ( uint32_t addr, int8_t target );
   static inline void GARBAGE ( uint32_t addr, int8_t target );
   static inline void EXTRA ();

   // Routines that mimic the PPU bus behavior down to the PPU cycle.
   // These are used internally by the PPU core during emulation.
   static inline void GATHERBKGND ( int8_t phase );
   static inline void GATHERSPRITES ( int32_t scanline );

   // Routines that mimic the PPU OAM behavior down to the PPU cycle.
   // This is used internally by the PPU core during emulation.
   static inline void BUILDSPRITELIST ( int32_t scanline, int32_t cycle );

   // Routine that mimics the PPU's background barrel-shifters and
   // X-scroll pickoff.
   static inline void PIXELPIPELINES ( int32_t pickoff, uint8_t* a, uint8_t* b1, uint8_t* b2 );

   // Routine that initializes the PPU's palette memory on reset.
   static void PALETTESET ( uint8_t* data )
   {
      memcpy(m_PALETTEmemory,data,MEM_32B);
   }

protected:
   // The PPU core maintains 32B of palette RAM.
   static uint8_t  m_PALETTEmemory [ MEM_32B ];

   // The PPU core maintains 2KB of video RAM.
   // Some mappers extend this by providing more memory on
   // the cartridge.
   static uint8_t*  m_PPUmemory;

   // The PPU's video memory can be rearranged by some mappers.
   // These pointers support that rearrangement.
   static uint8_t* m_pPPUmemory [ 8 ];

   // The PPU has an internal flip-flop which delivers
   // bytes written to $2005 or $2006 to different locations
   // within the PPU.  The flip-flop flips on each write to
   // PPU address $2005, or $2007.  It is reset on reads from
   // PPU address $2002.
   static int32_t            m_ppuRegByte;

   // Pre-calculated values based on video mode.
   static uint32_t startVblank;
   static uint32_t quietScanlines;
   static uint32_t vblankScanlines;
   static uint32_t vblankEndCycle;
   static uint32_t prerenderScanline;
   static uint32_t cycleRatio;
   static uint32_t memoryDecayFrames;

   // The PPU has an internal OAM address register that is
   // accessed via PPU address $2003.  It is incremented on
   // writes to $2004.
   static uint8_t  m_oamAddr;

   // The PPU has an internal address register that is used to
   // generate the bus addresses accessed by the PPU during rendering.
   // It is incremented on read/write of PPU address $2007.  It
   // is updated from the PPU address latch (see below) on every
   // other write to PPU address $2006 (ie. when the flip-flop is flipped
   // in the right direction).
   static uint16_t m_ppuAddr;

   // The PPU has an internal address latch that is programmed
   // via writes to PPU address $2000, $2005, $2006, and $2007.
   // The latch is copied to the address register (see above)
   // on every other write to PPU address $2006.
   static uint16_t m_ppuAddrLatch;

   // The PPU keeps track internally of whether it should be
   // auto-incrementing its internal address register by 1
   // (for horizontal updating) or by 32 (for vertical updating).
   // This is set on writes to PPU address $2000.
   static uint8_t  m_ppuAddrIncrement;

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
   static uint8_t  m_ppuReadLatch;

   // The PPU has an internal hold-up on the bus between it and
   // the CPU that causes the last value written to be readable
   // for about 600 milliseconds.
   static uint8_t m_ppuIOLatch;

   // Each bit of the IO latch decays at the same rate but some
   // might have been set to 1 more recently than others.
   static uint8_t m_ppuIOLatchDecayFrames [ 8 ];

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
   static uint8_t  m_PPUreg [ NUM_PPU_REGS ];

   // The PPU has space for 64 objects (sprites) in its internal
   // Object Attribute Map (OAM) memory.
   static uint8_t  m_PPUoam [ NUM_OAM_REGS ];

   // The PPU maintains fine-X (0..7) scroll value which is used as the
   // pick-off location in the barrel-shifter that does the rendering.
   static uint8_t  m_ppuScrollX;

   // The PPU maintains the state of mirroring of its internal video RAM.
   // Some mappers only use one video screen's worth of RAM, others provide
   // extra RAM for up to four independent video screens.  Most mappers use
   // the two video screen's worth of RAM dedicated to the PPU in some
   // mirrored fashion across the memory region which supports up to four
   // video screen's worth.
   static int32_t            m_oneScreen;
   static bool           m_extraVRAM;

   // Bounded counter of PPU cycles executed.  It resets to zero at the
   // start of each PPU frame.
   static uint32_t   m_cycles;

   // Running counter of PPU frames drawn.  It will roll over after
   // approximately 40 minutes of emulation.  However, this roll-over
   // is not a significant event.
   static uint32_t   m_frame;

   // The current number of PPU cycles ready to be executed by
   // the PPU core.
   static int32_t            m_curCycles;

   // If the CPU reads PPU address $2002 at a precise point within the
   // PPU frame it can choke the setting of the VBLANK flag in that register.
   static bool           m_vblankChoked;

   // If the CPU reads PPU address $2002 at precise points within the
   // PPU frame it can choke the assertion of NMI by the PPU.
   static bool           m_nmiChoked;

   // Re-enabling NMI flag in the PPU control register allows for re-assertion
   // of NMI during VBLANK period.
   static bool           m_nmiReenabled;

   // This is the temporary memory used by the PPU to store gathered
   // OAM data during pixel rendering.
   static SpriteTemporaryMemory m_spriteTemporaryMemory;

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
   static int8_t*          m_pTV;

   // These items are the database that keeps track of the status of the
   // x and y scroll values for each rendered pixel.  This information is
   // used by the nametable visualizer to highlight areas of the nametable
   // memory internal to the PPU that are being rendered to the screen.
   static uint8_t  m_last2005x;
   static uint8_t  m_last2005y;
   static uint16_t** m_2005x;
   static uint16_t** m_2005y;

   // These items are the position of the last sprite-0 hit event on the
   // last rendered PPU frame.  They are invalidated at the start of each
   // new frame.
   static uint8_t  m_lastSprite0HitX;
   static uint8_t  m_lastSprite0HitY;

   // These items are the position of the pixel being rendered by the
   // PPU on any given PPU cycle.  This information is used by the debugger.
   static uint8_t  m_x;
   static uint8_t  m_y;

   // Database used by the Code/Data Logger debugger inspector.  The data structure
   // is maintained by the PPU core as it performs fetches, reads,
   // and writes to/from its managed RAM.  The
   // Code/Data Logger displays the collected information graphically.
   static CCodeDataLogger* m_logger;

   // The database for PPU core registers.  Declaration
   // is in source file.
   static CRegisterDatabase* m_dbRegisters;
   static CRegisterDatabase* m_dbOamRegisters;

   static CMemoryDatabase* m_dbPaletteMemory;
   static CMemoryDatabase* m_dbNameTableMemory;

   // The database for PPU core breakpoint events.  Declaration
   // is in source file.
   static CBreakpointEventInfo** m_tblBreakpointEvents;
   static int32_t                    m_numBreakpointEvents;
};

#endif
