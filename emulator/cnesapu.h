#if !defined ( APU_H )
#define APU_H

#include "cnesicidecommon.h"
#include "cregisterdata.h"

#define _SDL_main_h
#include "SDL.h"

#include "cbreakpointinfo.h"

#define NUM_APU_BUFS 16

// Breakpoint event identifiers.
// These event identifiers must match the ordering
// of breakpoint events defined in the source module.
enum
{
   APU_EVENT_IRQ = 0,
   APU_EVENT_DMC_DMA,
   APU_EVENT_LENGTH_COUNTER_CLOCKED,
   NUM_APU_EVENTS
};

// APU mask register ($4017) bit definitions.
#define APUSTATUS_FIVEFRAMES 0x80
#define APUSTATUS_IRQDISABLE 0x40

// Samples per video frame for 44.1KHz audio output.
// NTSC is 60Hz, PAL is 50Hz.  The number of samples
// drives the rate at which the SDL library will invoke
// the callback method to retrieve more audio samples to
// play.
#define APU_SAMPLES_NTSC 735
#define APU_SAMPLES_PAL  882

// The CAPUOscillator class is the base class of all of the
// sound channels within the APU.  There are five sound channels.
// Each sound channel has behavior that is identical to all
// other channels and behavior that is specific to its channel
// type.  The identical behavior is implemented in the CAPUOscillator
// base class.
//
// CAPUOscillator contains logic to drive the following
// sound channel components:
//
// Length Counter [Square, Triangle, Noise]
// Linear Counter [Triangle]
// Sweep Unit [Square]
// Envelope [Square, Noise]
// Enabled/disabled [Square, Triangle, Noise, DMC]
// Timer Divider (period) [Square, Triangle, Noise, DMC]
// DAC [Square, Triangle, Noise, DMC]
//
// While the Linear Counter should really be considered a specific
// aspect of the Triangle channel only, it made sense to include it
// in CAPUOscillator as it is possible that other channels on some
// mapper hardware might be triangle-like and might need to use it.
// (Still looking into this).
//
// The same could be said for the Sweep Unit which is specific to
// the Square channel.  Oh well...
class CAPUOscillator
{
public:
   CAPUOscillator();
   virtual ~CAPUOscillator() {};

   // An APU channel should know what channel ID it has.  This isn't
   // useful in the APU itself but is needed by the debugger inspectors
   // and designers that need to do specific things to specific APU channels.
   void SetChannel ( int channel ) { m_channel = channel; }

   // This method sets data internal to the APU channel.  There is no
   // get method because the APU channels' registers are write-only.
   inline void APU ( UINT addr, unsigned char data );

   // This method turns the channel on or off.  The length counter
   // disables the channel when it expires.  Writes to APU register
   // $4015 change the enabled state of the channels.
   inline void ENABLE ( bool enabled ) { m_enabled = enabled; if ( !m_enabled ) m_lengthCounter = 0; }

   // This method returns the length counter value.  Reads from APU
   // register $4015 return whether or not any length counters have expired.
   inline unsigned short LENGTH ( void ) const { return m_lengthCounter; }

   // The APU 240Hz sequencer clocks the length counter of each
   // channel that has one at pre-determined times in the APU frame.
   inline bool CLKLENGTHCOUNTER ( void );

   // The APU 240Hz sequencer clocks the sweep unit of each
   // channel that has one at pre-determined times in the APU frame.
   inline void CLKSWEEPUNIT ( void );

   // The APU 240Hz sequencer clocks the envelope of each
   // channel that has one at pre-determined times in the APU frame.
   inline void CLKENVELOPE ( void );

   // The APU 240Hz sequencer clocks the linear counter of each
   // channel that has one at pre-determined times in the APU frame.
   inline bool CLKLINEARCOUNTER ( void );

   // At each APU cycle (1,789,772Hz) the channels' divider is
   // clocked.  This divider provides an overall frequency for
   // updates to take place to the channels' DACs to generate the
   // appropriate waveform.
   inline UINT CLKDIVIDER ( void );

   // These routines set/get the channels' DAC value.
   inline void SETDAC ( unsigned char dac ) { m_dac = dac; }
   inline unsigned char GETDAC ( void ) { return m_dac; }

   // This routine returns the channels' internal state to
   // what it should be at NES reset.
   inline void RESET ( void )
   {
      m_linearCounter = 0;
      m_linearCounterReload = 0;
      m_lengthCounter = 0;
      m_period = 0;
      m_periodCounter = 0;
      m_envelope = 0;
      m_envelopeCounter = 0;
      m_envelopeDivider = 0;
      m_sweepShift = 0;
      m_sweepDivider = 0;
      m_sweep = 0;
      m_volume = 0;
      m_volumeSet = 0;
      m_enabled = false;
      m_newHalted = false;
      m_halted = false;
      m_linearCounterHalted = false;
      m_envelopeEnabled = false;
      m_sweepEnabled = false;
      m_sweepNegate = false;
      m_dac = 0;
      m_reg1Wrote = false;
      m_reg3Wrote = false;
   }

   // These are called directly for use in debugger inspectors.
   // Returns the channels' current length counter value.
   unsigned char LENGTHCOUNTER(void) const { return m_lengthCounter; }

   // Returns the channels' current linear counter value.
   unsigned char LINEARCOUNTER(void) const { return m_linearCounter; }

protected:
   // Current linear counter value.
   unsigned char  m_linearCounter;

   // Value to be loaded into linear counter when it needs reloading.
   unsigned char  m_linearCounterReload;

   // Current length counter value.
   unsigned short m_lengthCounter;

   // The number of APU cycles before the channels' internal
   // divider will emit a clock edge.
   unsigned short m_period;

   // The current number of cycles into the counting of the
   // current period.  Once this counter hits m_period the
   // timer divider emits a clock edge that drives the rest of
   // the channels' circuitry.
   unsigned short m_periodCounter;


   // These values define the envelope configuration currently
   // being used by the channel.  An envelope can either be a constant
   // volume level or a sawtooth wave.  In the wave case, a divider
   // deals out clock edges at a lower frequency than the period divider
   // governing the channels' pitch.  The envelope divider is used to
   // decrease the envelope's value down to zero and then immediately
   // back to the maximum possible, thus creating a sawtooth waveform.
   unsigned char  m_envelope;
   unsigned char  m_envelopeCounter;
   unsigned char  m_envelopeDivider;

   // These values define the sweep configuration currently being
   // used by the channel.  A divider deals out clock edges at a lower
   // frequency than the period divider governing the channels' pitch.
   // The sweep divider is used to periodically cause shifts in a feedback
   // loop to the channels' period divider, thus causing a change in the
   // channels' pitch either up or down.
   unsigned char  m_sweepShift;
   unsigned char  m_sweepDivider;
   unsigned char  m_sweep;

   // The channels' current volume that may eventually make
   // its way into the DAC as the current DAC setting if the
   // channel internals cause a write to the DAC.
   unsigned char  m_volume;
   unsigned char  m_volumeSet;

   // Is the channel enabled (via write to $4015)?
   bool           m_enabled;


   // Has the channel been halted entirely?
   bool           m_halted;

   // Halt flag changes take effect only on even APU
   // cycles which means the new setting must be kept
   // around until the next even APU cycle.
   bool           m_newHalted;

   // Has the channels' linear counter been halted?
   bool           m_linearCounterHalted;

   // Is the channels' envelope enabled or not.  Envelopes
   // can be either a set value or a sawtooth wave.
   bool           m_envelopeEnabled;

   // Is the channels' sweep unit enabled or not?
   bool           m_sweepEnabled;

   // Sweeping positively or negatively in pitch?
   bool           m_sweepNegate;

   // The channels' current DAC value.  This ranges
   // from 0-15 for Square, Triangle, and Noise channels
   // and from 0-127 for DMC.
   unsigned char  m_dac;

   // Flags indicating whether or not certain channel
   // registers were written since the last channel activity.
   bool           m_reg1Wrote;
   bool           m_reg3Wrote;

   // Channels' index:
   // 0 - Square 1
   // 1 - Square 2
   // 2 - Triangle
   // 3 - Noise
   // 4 - DMC
   int            m_channel;

   // Each APU channel has up to four registers visible to the CPU in the CPU's
   // memory map at address $4000 through $4013.  These are further
   // mirrored throughout the CPU's memory map up to address $5BFF (confirm?).
   // The APU keeps track internally of values written to the register
   // ports so that the debugger inspectors can return actual data
   // written to the registers instead of "open bus".  If the debugger
   // inspectors want to, they can implement a mode where the inspector
   // does infact reflect the actual state of the APU as seen by the CPU.
   // Typically, however, a user likes to see what has been written.
   // I'll keep the "open bus" in mind though to provide accurate feedback
   // as if the user were staring at an actual APU.
   unsigned char  m_reg [ 4 ];
};

class CAPUSquare : public CAPUOscillator
{
public:
   CAPUSquare() :
      CAPUOscillator(),
      m_timerClk(0),
      m_seqTick(0),
      m_duty(0){};
   virtual ~CAPUSquare() {};

   inline void APU ( UINT addr, unsigned char data );
   inline void TIMERTICK ( void );
   inline void RESET ( void ) { CAPUOscillator::RESET(); m_duty = 0; m_seqTick = 0; m_timerClk = 0; }

protected:
   int           m_timerClk;
   int           m_seqTick;
   int           m_duty;
};

class CAPUTriangle : public CAPUOscillator
{
public:
    CAPUTriangle() :
      CAPUOscillator(),
      m_seqTick(0) {};
   virtual ~CAPUTriangle() {};

   inline void APU ( UINT addr, unsigned char data );
   inline void TIMERTICK ( void );
   inline void RESET ( void ) { CAPUOscillator::RESET(); m_seqTick = 0; }

protected:
   int           m_seqTick;
};

class CAPUNoise : public CAPUOscillator
{
public:
    CAPUNoise();
   virtual ~CAPUNoise() {};

   inline void APU ( UINT addr, unsigned char data );
   inline void TIMERTICK ( void );
   inline void RESET ( void )
   { CAPUOscillator::RESET(); m_shift = 0x1; m_mode = 0; m_shortTableIdx = 0; m_longTableIdx = 0; }

protected:
   unsigned short m_shift;
   int            m_mode;
   unsigned char  m_shortTable [ 93 ];
   unsigned char  m_longTable [ 32767 ];
   unsigned short m_shortTableIdx;
   unsigned short m_longTableIdx;
};
class CAPUDMC : public CAPUOscillator
{
public:
    CAPUDMC() : CAPUOscillator(),
               m_dmaReaderAddrPtr(0x0000),
               m_dmcIrqEnabled(false),
               m_dmcIrqAsserted(false),
               m_sampleBuffer(0x00),
               m_sampleBufferFull(false),
               m_loop(false),
               m_sampleAddr(0x0000),
               m_sampleLength(0x0000),
               m_outputShift(0x00),
               m_outputShiftCounter(0),
               m_silence(false),
               m_dmaSource(NULL),
               m_dmaSourcePtr(NULL)
   {};
   virtual ~CAPUDMC() {};

   inline void APU ( UINT addr, unsigned char data );
   inline void TIMERTICK ( void );
   inline void ENABLE ( bool enabled );
   void DMAREADER ( void );
   void DMASOURCE ( unsigned char* source ) { m_dmaSource = source; m_dmaSourcePtr = source; }
   bool IRQASSERTED ( void ) const { return m_dmcIrqAsserted; }
   void IRQASSERTED ( bool asserted ) { m_dmcIrqAsserted = asserted; }

   inline void RESET ( void )
   {
      CAPUOscillator::RESET();
      m_loop = false;
      m_sampleAddr = 0x0000;
      m_sampleLength = 0x0000;
      m_dmaReaderAddrPtr = 0x0000;
      m_dmcIrqEnabled = false;
      m_dmcIrqAsserted = false;
      m_sampleBuffer = 0x00;
      m_sampleBufferFull = false;
      m_outputShift = 0x00;
      m_outputShiftCounter = 0;
      m_silence = false;
      m_dmaSource = NULL;
      m_dmaSourcePtr = NULL;
   }

protected:
   unsigned short m_dmaReaderAddrPtr;
   bool          m_dmcIrqEnabled;
   bool          m_dmcIrqAsserted;
   unsigned char m_sampleBuffer;
   bool          m_sampleBufferFull;
   bool          m_loop;
   unsigned short m_sampleAddr;
   unsigned short m_sampleLength;
   unsigned char m_outputShift;
   unsigned char m_outputShiftCounter;
   bool          m_silence;
   unsigned char* m_dmaSource;
   unsigned char* m_dmaSourcePtr;
};

class CAPU
{
public:
    CAPU();
    virtual ~CAPU();

   static void RESET ( void );
   static UINT APU ( UINT addr );
   static void APU ( UINT addr, unsigned char data );

   static void DMASOURCE ( unsigned char* source ) { m_dmc.DMASOURCE ( source ); }

   static unsigned char GET4015MASK ( void ) { return m_APUreg4015mask; }
   static void SET4015MASK ( unsigned char mask ) { m_APUreg4015mask = mask; CAPU::APU(0x4015,mask); }

   static UINT _APU ( UINT addr ) { return *(m_APUreg+(addr&0x1F)); }
   static void _APU ( UINT addr, unsigned char data ) { *(m_APUreg+(addr&0x1F)) = data; }
   static inline unsigned char DIRTY ( UINT addr )
   {
      unsigned char updated = *(m_APUregDirty+(addr&0x1F));
      *(m_APUregDirty+(addr&0x1F))=0;
      return updated;
   }

   static void RELEASEIRQ ( void );
   static inline void SEQTICK ( int sequence );
   static inline unsigned short AMPLITUDE ( void );

   // INTERNAL ACCESSOR FUNCTIONS
   // These are called directly.
   static void LENGTHCOUNTERS ( unsigned char* sq1, unsigned char* sq2, unsigned char* triangle, unsigned char* noise, unsigned char* dmc )
   {
      (*sq1) = m_square[0].LENGTHCOUNTER();
      (*sq2) = m_square[1].LENGTHCOUNTER();
      (*triangle) = m_triangle.LENGTHCOUNTER();
      (*noise) = m_noise.LENGTHCOUNTER();
      (*dmc) = m_dmc.LENGTHCOUNTER();
   }
   static void LINEARCOUNTER ( unsigned char* triangle )
   {
      (*triangle) = m_triangle.LINEARCOUNTER();
   }
   static void GETDACS ( unsigned char* square1,
                         unsigned char* square2,
                         unsigned char* triangle,
                         unsigned char* noise,
                         unsigned char* dmc );

   static void OPEN ( void );
   static void EMULATE ( int cycles );
   static void PLAY ( Uint8 *stream, int len );
   static void CLOSE ( void );

   static CRegisterData** REGISTERS() { return m_tblRegisters; }
   static int NUMREGISTERS() { return m_numRegisters; }

   static CBreakpointEventInfo** BREAKPOINTEVENTS() { return m_tblBreakpointEvents; }
   static int NUMBREAKPOINTEVENTS() { return m_numBreakpointEvents; }

   static inline void RESETCYCLECOUNTER ( unsigned int cycle ) { m_cycles = cycle; }
   static inline unsigned int CYCLES ( void ) { return m_cycles; }

protected:
   static unsigned char m_APUreg [ 32 ];
   static unsigned char m_APUregDirty [ 32 ];
   static unsigned char m_APUreg4015mask;
   static bool m_irqEnabled;
   static bool m_irqAsserted;

   static int m_sequencerMode;
   static int m_newSequencerMode;
   static int m_changeModes;
   static int m_sequenceStep;

   static CAPUSquare m_square[2];
   static CAPUTriangle m_triangle;
   static CAPUNoise m_noise;
   static CAPUDMC m_dmc;

   static SDL_AudioSpec m_sdlAudioSpec;
   static unsigned short m_waveBuf [ NUM_APU_BUFS ][ 1000 ];
   static int m_waveBufDepth [ NUM_APU_BUFS ];
   static int m_waveBufProduce;
   static int m_waveBufConsume;

   static CRegisterData** m_tblRegisters;
   static int             m_numRegisters;

   static CBreakpointEventInfo** m_tblBreakpointEvents;
   static int                    m_numBreakpointEvents;

   static unsigned int m_sampleRate;

   static unsigned int   m_cycles;
};

#endif
