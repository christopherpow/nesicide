#if !defined ( PPU_H )
#define PPU_H

#include "nes_emulator_core.h"

#include "cregisterdata.h"
#include "cbreakpointinfo.h"

#include <QColor>

// The Execution Visualizer debugger inspector converts PPU cycles to
// (x,y) coordinates for visualization of the execution of code in the
// PPU frame.
#define CYCLE_TO_VISX(c) (c%PPU_CYCLES_PER_SCANLINE)
#define CYCLE_TO_VISY(c) (c/PPU_CYCLES_PER_SCANLINE)
#define VISY_VISX_TO_CYCLE(y,x) ((y*PPU_CYCLES_PER_SCANLINE)+x)

void RENDERPPUCODEDATALOGGER ();
int8_t* PPUCODEDATALOGGERTV ();
void CLEARPPUCODEDATALOGGER ();

void RENDERNAMETABLE ();
int8_t* NAMETABLETV ();
void CLEARNAMETABLETV ();

void RENDEROAM ();
int8_t* OAMTV ();
void CLEAROAMTV ();

// This accessor method sets the flag indicating whether or not
// visible or invisible sprites should be decorated by the OAM viewer.
void SetOAMViewerShowVisible ( bool visible );

// This accessor method sets the flag indicating whether or not
// visible or invisible regions of the TV screen should be decorated by
// the NameTable viewer.
void SetPPUViewerShowVisible ( bool visible );

// The CHR memory visualization inspector and the OAM visualization
// inspector are triggered on a user-defined scanline.  These accessor
// methods allow the UI to change the scanline-of-interest for the
// debugger inspectors.
void SetPPUViewerScanline ( uint32_t scanline );
uint32_t GetPPUViewerScanline ( void );
void SetOAMViewerScanline ( uint32_t scanline );
uint32_t GetOAMViewerScanline ( void );

void SetCHRMEMInspectorColor ( int32_t idx, QColor color );

// These functions are invoked at appropriate points in the PPU
// rendering frame to cause the PPU to render the current state
// of the CHR, OAM, and nametable memories.  For example, an emulated
// game might change the CHR memory map in mid PPU frame.  The CHR
// memory inspector allows visualization of the CHR memory at a
// specified scanline, thus showing the state of the CHR memory
// before or after the change.
void RENDERCHRMEM ( void );
int8_t* CHRMEMTV ();
void CLEARCHRMEMTV ();

// NTSC/PAL-dependent interfaces.
// NTSC: 262 scanlines
// PAL: 312 scanlines
// Dendy: 312 scanlines
uint32_t SCANLINES ( void );

#endif
