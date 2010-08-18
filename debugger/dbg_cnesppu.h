#if !defined ( PPU_H )
#define PPU_H

#include "emulator_core.h"

#include "cregisterdata.h"
#include "cbreakpointinfo.h"

// The Execution Visualizer debugger inspector converts PPU cycles to
// (x,y) coordinates for visualization of the execution of code in the
// PPU frame.
#define CYCLE_TO_VISX(c) (c%PPU_CYCLES_PER_SCANLINE)
#define CYCLE_TO_VISY(c) (c/PPU_CYCLES_PER_SCANLINE)
#define VISY_VISX_TO_CYCLE(y,x) ((y*PPU_CYCLES_PER_SCANLINE)+x)

// The CPPUDBG class is a container for all debug elements
// relevant to the NES' PPU.
class CPPUDBG
{
public:
    CPPUDBG();
    virtual ~CPPUDBG();

   // NTSC/PAL-dependent interfaces.
   // NTSC: 262 scanlines
   // PAL: 312 scanlines
   static inline uint32_t SCANLINES ( void ) { return nesGetSystemMode()==MODE_NTSC?SCANLINES_TOTAL_NTSC:SCANLINES_TOTAL_PAL; }

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
   static inline uint32_t _CYCLES ( void ) { return nesGetPPUCycle(); }

   // Read a PPU register without changing the PPU's internal state.
   // This routine is used by the debugger in order to retrieve information
   // about the internal state of the PPU without affecting said internal state.
   // It is also used by the emulation to determine what steps to perform as
   // part of the emulation process (ie. generate an NMI to the CPU core or not?)
   static inline uint32_t _PPU ( uint32_t addr ) { return nesGetPPURegister(addr); }

   // Write a PPU register.
   // This routine is used by the emuation in order to change the internal state
   // of the PPU from the perspective of the emulated machine, not because any
   // emulated code made it change.  For example, setting or clearing the VBLANK
   // flag are not necessarily dependent on executed code.
   static inline void _PPU ( uint32_t addr, uint8_t data ) { return nesSetPPURegister(addr,data); }

   // Read a byte from the PPU's internal OAM memory.
   static inline uint32_t _OAM ( uint32_t oam, uint32_t sprite ) { return nesGetPPUOAM(oam,sprite); }

   // Write a byte to the PPU's internal OAM memory.
   static inline void _OAM ( uint32_t oam, uint32_t sprite, uint8_t data ) { nesSetPPUOAM(oam,sprite,data); }

   // Silently read from a memory location visible to the PPU.
   // This routine is used by the debuggers to gather PPU information without
   // impacting the state of the emulation or the visual aspect of any inspectors.
   static inline uint32_t _MEM ( uint32_t addr ) { return nesGetPPUMemory(addr); }

   // Silently write to a memory location visible to the PPU.
   // This routine is used by the debuggers to change PPU information without
   // impacting the state of the emulation or the visual aspect of any inspectors.
   static inline void _MEM ( uint32_t addr, uint8_t data ) { nesSetPPUMemory(addr,data); }

   // Silently read from memory locations visible to the PPU.
   // These routines are used by the debuggers to gather PPU information without
   // impacting the state of the emulation.
   static inline uint8_t _NAMETABLE ( uint16_t addr ) { return nesGetPPUNameTableData(addr); }
   static inline uint8_t _ATTRTABLE ( uint16_t addr ) { return nesGetPPUAttributeTableData(addr); }
   static inline uint8_t _PATTERNDATA ( uint16_t addr ) { return nesGetPPUPatternData(addr); }
   static inline uint8_t _PALETTE ( uint8_t addr ) { return nesGetPPUPaletteData(addr); }

   // Return the internal state of the PPU's hardware such as the OAM address latch, the PPU
   // address latch, and the read-buffer.
   static inline uint16_t _OAMADDR ( void ) { return nesGetPPUOAMAddress(); }
   static inline uint16_t _PPUADDR ( void ) { return nesGetPPUAddress(); }
   static inline uint8_t _PPUREADLATCH ( void ) { return nesGetPPUReadLatch(); }
   static inline uint16_t _PPUADDRLATCH ( void ) { return nesGetPPUAddressLatch(); }
   static inline int32_t _PPUFLIPFLOP ( void ) { return nesGetPPUFlipFlop(); }

   // Accessor functions for the database of scroll values indexed by pixel
   // location.  At each PPU cycle that is rendering a visible pixel the scroll
   // register values are stored so that a representation of the visible portions
   // of the nametable may be overlaid upon the actual nametable in the nametable visual
   // inspector.
   static inline uint16_t _SCROLLX ( int32_t x, int32_t y ) { return *(*(m_2005x+x)+y); }
   static inline uint16_t _SCROLLY ( int32_t x, int32_t y ) { return *(*(m_2005y+x)+y); }

   // Accessor functions for the pixel coordinate of the last sprite-0 hit.
   static inline uint8_t _SPRITE0HITX ( void ) { return m_lastSprite0HitX; }
   static inline uint8_t _SPRITE0HITY ( void ) { return m_lastSprite0HitY; }

   // Accessor functions for the pixel coordinate of the PPU during rendering.
   static inline uint8_t _X ( void ) { return m_x; }
   static inline uint8_t _Y ( void ) { return m_y; }

   // Returns a running counter of the number of frames that have elapsed
   // during emulation.  At 60Hz this counter will not roll-over until
   // approximately 828 days of emulation have passed.  Note, however, that
   // even then, roll-over of this counter is not a significant event.
   static inline uint32_t _FRAME ( void ) { return nesGetPPUFrame(); }

   // The CHR memory rendering is performed by the PPU.  The CHR
   // memory is maintained by the ROM object (see CROM).  However,
   // the PPU is the portal through which access to this memory is made.
   // The CHR memory visualization inspector allocates the memory to be
   // rendered to, and passes it to the PPU.  If the CHR memory
   // visualization inspector is not visible the rendering is disabled
   // to save host CPU cycles.
   static inline void EnableCHRMEMInspector ( bool enable ) { m_bCHRMEMInspector = enable; }
   static inline void CHRMEMInspectorTV ( int8_t* pTV ) { m_pCHRMEMInspectorTV = pTV; }

   // The OAM memory rendering is performed by the PPU.  The OAM
   // visualization inspector allocates the memory to be rendered to,
   // and passes it to the PPU.  If the OAM visualization inspector is
   // not visible the rendering is disabled to save host CPU cycles.
   static inline void EnableOAMInspector ( bool enable ) { m_bOAMInspector = enable; }
   static inline void OAMInspectorTV ( int8_t* pTV ) { m_pOAMInspectorTV = pTV; }

   // The nametable memory rendering is performed by the PPU.  The nametable
   // visualization inspector allocates the memory to be rendered to,
   // and passes it to the PPU.  If the nametable visualization inspector
   // is not visible the rendering is disabled to save host CPU cycles.
   static inline void EnableNameTableInspector ( bool enable ) { m_bNameTableInspector = enable; }
   static inline void NameTableInspectorTV ( int8_t* pTV ) { m_pNameTableInspectorTV = pTV; }

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
   static void SetPPUViewerScanline ( uint32_t scanline ) { m_iPPUViewerScanline = scanline; }
   static uint32_t GetPPUViewerScanline ( void ) { return m_iPPUViewerScanline; }
   static void SetOAMViewerScanline ( uint32_t scanline ) { m_iOAMViewerScanline = scanline; }
   static uint32_t GetOAMViewerScanline ( void ) { return m_iOAMViewerScanline; }

   // This accessor method sets the flag indicating whether or not
   // visible or invisible sprites should be decorated by the OAM viewer.
   static void SetOAMViewerShowVisible ( bool visible ) { m_bOAMViewerShowVisible = visible; }

   // This accessor method sets the flag indicating whether or not
   // visible or invisible regions of the TV screen should be decorated by
   // the NameTable viewer.
   static void SetPPUViewerShowVisible ( bool visible ) { m_bPPUViewerShowVisible = visible; }

   // Interface to retrieve the database defining the registers
   // of the PPU core in a "human readable" form that is used by
   // the Register-type debugger inspector.  The hexadecimal world
   // of the PPU core registers is transformed into textual description
   // and manipulatable bitfield texts by means of this database.
   // PPU registers are declared in the source file.
   static CRegisterData** REGISTERS() { return m_tblRegisters; }
   static int32_t NUMREGISTERS() { return m_numRegisters; }

   // Interface to retrieve the database of PPU core-specific
   // breakpoint events.  A breakpoint event is an event that
   // is typically internal to the PPU core that the user would
   // like to trigger a program halt on for inspection of the emulated
   // machine.  PPU breakpoint events are declared in the source
   // file.
   static CBreakpointEventInfo** BREAKPOINTEVENTS() { return m_tblBreakpointEvents; }
   static int32_t NUMBREAKPOINTEVENTS() { return m_numBreakpointEvents; }

   // The PPU's Code/Data Logger display is generated by the PPU core
   // because the PPU core maintains all of the information necessary
   // to generate it.
   static inline void CodeDataLoggerInspectorTV ( int8_t* pTV ) { m_pCodeDataLoggerInspectorTV = pTV; }
   static void RENDERCODEDATALOGGER ( void );

   // These items are the database that keeps track of the status of the
   // x and y scroll values for each rendered pixel.  This information is
   // used by the nametable visualizer to highlight areas of the nametable
   // memory internal to the PPU that are being rendered to the screen.
   static uint8_t  m_last2005x;
   static uint8_t  m_last2005y;
   static uint16_t m_2005x [ 256 ][ 240 ];
   static uint16_t m_2005y [ 256 ][ 240 ];

   // These items are the position of the last sprite-0 hit event on the
   // last rendered PPU frame.  They are invalidated at the start of each
   // new frame.
   static uint8_t  m_lastSprite0HitX;
   static uint8_t  m_lastSprite0HitY;

   // These items are the position of the pixel being rendered by the
   // PPU on any given PPU cycle.  This information is used by the debugger.
   static uint8_t  m_x;
   static uint8_t  m_y;

   // Flags defining whether or not the various debugger inspectors
   // that are visualizers are enabled (visible) or not.
   static bool           m_bCHRMEMInspector;
   static bool           m_bOAMInspector;
   static bool           m_bNameTableInspector;

   // These are the rendering surfaces on which the PPU draws
   // the data to be visualized by the various debugger inspectors
   // that provide visual information on the state of the NES.
   static int8_t*          m_pCHRMEMInspectorTV;
   static int8_t*          m_pOAMInspectorTV;
   static int8_t*          m_pNameTableInspectorTV;
   static int8_t*          m_pCodeDataLoggerInspectorTV;

   // These are the current scanlines on which the visualizer
   // debugger inspectors that are scanline-triggered are to be updated.
   static uint32_t           m_iPPUViewerScanline;
   static uint32_t           m_iOAMViewerScanline;

   // Flag indicating whether or not to decorate invisible sprites.
   static bool           m_bOAMViewerShowVisible;

   // Flag indicating whether or not to decorate invisible TV region(s).
   static bool           m_bPPUViewerShowVisible;

   // The database for PPU core registers.  Declaration
   // is in source file.
   static CRegisterData** m_tblRegisters;
   static int32_t             m_numRegisters;

   // The database for PPU core breakpoint events.  Declaration
   // is in source file.
   static CBreakpointEventInfo** m_tblBreakpointEvents;
   static int32_t                    m_numBreakpointEvents;
};

#endif
