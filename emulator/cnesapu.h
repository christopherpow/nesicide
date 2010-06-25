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
      m_clockLengthCounter = true;
   }

   // These are called directly for use in debugger inspectors.
   // Returns the channels' current length counter value.
   unsigned short LENGTHCOUNTER(void) const { return m_lengthCounter; }

   // Returns the channels' current linear counter value.
   unsigned char LINEARCOUNTER(void) const { return m_linearCounter; }

protected:
   // Current linear counter value.
   unsigned char  m_linearCounter;

   // Value to be loaded into linear counter when it needs reloading.
   unsigned char  m_linearCounterReload;

   // Current length counter value.
   unsigned short m_lengthCounter;

   // Whether or not to clock the length counter at next sequencer step
   // where the length counter should be clocked.  Length counter clocking
   // is choked if a write to the length counter occurs at specific points
   // within the APU frame.
   bool m_clockLengthCounter;

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

// The CAPUSquare class extends the base CAPUOscillator class to provide
// the mechanics of generating a square waveform sound channel.  It provides
// a divide-by-two clock, settable duty-cycle for waveform transitions, and an
// internal sequencer that drives the duty-cycling.  This class handles the
// APU cycle ticks passed to it by the main APU object (see CAPU).
// It also overloads the register-write method so that it can update its
// internal state based on the register settings provided.  No register-read
// method is provided; the channel is write-only registers.  Finally, it
// overloads the reset method so that the channel's internal state may be
// cleared upon a NES reset.
class CAPUSquare : public CAPUOscillator
{
public:
   CAPUSquare() :
      CAPUOscillator(),
      m_timerClk(0),
      m_seqTick(0),
      m_duty(0){};
   virtual ~CAPUSquare() {};

   // This method sets data internal to the APU channel.  There is no
   // get method because the APU channel's registers are write-only.
   inline void APU ( UINT addr, unsigned char data );

   // This method handles the channel mechanics of generating the
   // appropriate square waveform given the register settings.  It
   // is invoked by the main APU object at each APU cycle.  When necessary
   // it updates the DAC value of the channel.
   inline void TIMERTICK ( void );

   // This routine returns the channels' internal state to
   // what it should be at NES reset.
   inline void RESET ( void )
   { CAPUOscillator::RESET(); m_duty = 0; m_seqTick = 0; m_timerClk = 0; }

protected:
   // The square waveform channel has an internal divide-by-two clock.
   int           m_timerClk;

   // The square waveform channel has an internal 8-step sequencer for
   // generating the appropriate duty-cycle.
   int           m_seqTick;

   // The square waveform channel has four selectable duty-cycle modes.
   int           m_duty;
};

// The CAPUTriangle class extends the base CAPUOscillator class to provide
// the mechanics of generating a triangle waveform sound channel.  It provides
// a 32-step sequencer that steps through predefined DAC levels to generate
// a triangle waveform.  This class handles the
// APU cycle ticks passed to it by the main APU object (see CAPU).
// It also overloads the register-write method so that it can update its
// internal state based on the register settings provided.  No register-read
// method is provided; the channel is write-only registers.  Finally, it
// overloads the reset method so that the channel's internal state may be
// cleared upon a NES reset.
class CAPUTriangle : public CAPUOscillator
{
public:
    CAPUTriangle() :
      CAPUOscillator(),
      m_seqTick(0) {};
   virtual ~CAPUTriangle() {};

   // This method sets data internal to the APU channel.  There is no
   // get method because the APU channel's registers are write-only.
   inline void APU ( UINT addr, unsigned char data );

   // This method handles the channel mechanics of generating the
   // appropriate triangle waveform given the register settings.  It
   // is invoked by the main APU object at each APU cycle.  When necessary
   // it updates the DAC value of the channel.
   inline void TIMERTICK ( void );

   // This routine returns the channels' internal state to
   // what it should be at NES reset.
   inline void RESET ( void )
   { CAPUOscillator::RESET(); m_seqTick = 0; }

protected:
   // The triangle waveform channel has an internal 32-step sequencer
   // for generating the appropriate triangular waveform.
   int           m_seqTick;
};

// The CAPUNoise class extends the base CAPUOscillator class to provide
// the mechanics of generating a pseudo-random waveform sound channel.  It provides
// a 15-bit shift register, and a settable mode.  This class handles the
// APU cycle ticks passed to it by the main APU object (see CAPU).
// It also overloads the register-write method so that it can update its
// internal state based on the register settings provided.  No register-read
// method is provided; the channel is write-only registers.  Finally, it
// overloads the reset method so that the channel's internal state may be
// cleared upon a NES reset.
//
// Note:  The noise channel implementation generates a look-up table of
// shift register values using the algorithm that the NES uses.  It is
// arguable whether or not this is faster than just generating the shift
// register content in real-time.
class CAPUNoise : public CAPUOscillator
{
public:
    CAPUNoise();
   virtual ~CAPUNoise() {};

   // This method sets data internal to the APU channel.  There is no
   // get method because the APU channel's registers are write-only.
   inline void APU ( UINT addr, unsigned char data );

   // This method handles the channel mechanics of generating the
   // appropriate noise waveform given the register settings.  It
   // is invoked by the main APU object at each APU cycle.  When necessary
   // it updates the DAC value of the channel.
   inline void TIMERTICK ( void );

   // This routine returns the channels' internal state to
   // what it should be at NES reset.
   inline void RESET ( void )
   { CAPUOscillator::RESET(); m_mode = 0; m_shortTableIdx = 0; m_longTableIdx = 0; }

protected:
   // The noise channel has two different modes:
   // 93-cycle pattern
   // 32767-cycle pattern
   int            m_mode;

   // The noise channel generates a look-up table of
   // shift register values during program initialization.
   unsigned char  m_shortTable [ 93 ];
   unsigned char  m_longTable [ 32767 ];

   // The noise channel keeps track of where it is in the
   // table of shift register values.
   unsigned short m_shortTableIdx;
   unsigned short m_longTableIdx;
};

// The CAPUDMC class extends the base CAPUOscillator class to provide
// the mechanics of generating a delta-modulation sound channel.  It provides
// a sample buffer, an output shift register, a DMA unit, an interrupt generator,
// a loop flag, and internal memory for sample memory location and size.  This
// class handles the APU cycle ticks passed to it by the main APU object (see CAPU).
// It also overloads the register-write method so that it can update its
// internal state based on the register settings provided.  No register-read
// method is provided; the channel is write-only registers.  Finally, it
// overloads the reset method so that the channel's internal state may be
// cleared upon a NES reset.
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

   // This method sets data internal to the APU channel.  There is no
   // get method because the APU channel's registers are write-only.
   inline void APU ( UINT addr, unsigned char data );

   // This method handles the channel mechanics of generating the
   // appropriate delta-modulation waveform given the register settings.  It
   // is invoked by the main APU object at each APU cycle.  When necessary
   // it updates the DAC value of the channel.  It also drives the other
   // channel elements to fetch sample data using DMA.
   inline void TIMERTICK ( void );

   // The delta-modulation channel requires special handling of writes
   // to $4015 because it has an internal interrupt generator that is
   // acknowledged and it restarts sample playback if appropriate to do so.
   inline void ENABLE ( bool enabled );

   // The delta-modulation channel contains a DMA unit that fetches
   // sample data for playback when its internal sample buffer empties
   // into the output shift register.
   void DMAREADER ( void );

   // The source of DMA can be set to the emulation engine or to the
   // music designer.  In the latter case the APU is used by the music
   // designer to play music being created in the designer.
   void DMASOURCE ( unsigned char* source ) { m_dmaSource = source; m_dmaSourcePtr = source; }

   // These methods deal with the delta-modulation channel's interrupt flag.
   bool IRQASSERTED ( void ) const { return m_dmcIrqAsserted; }
   void IRQASSERTED ( bool asserted ) { m_dmcIrqAsserted = asserted; }
   bool IRQENABLED ( void ) const { return m_dmcIrqEnabled; }

   // These methods provide internal channel detail to the debuggers.
   unsigned short SAMPLEADDR ( void ) const { return m_sampleAddr; }
   unsigned short SAMPLELENGTH ( void ) const { return m_sampleLength; }
   unsigned short SAMPLEPOS ( void ) const { return m_dmaReaderAddrPtr; }
   unsigned char SAMPLEBUFFER ( void ) const { return m_sampleBuffer; }
   bool SAMPLEBUFFERFULL ( void ) const { return m_sampleBufferFull; }

   // This routine returns the channels' internal state to
   // what it should be at NES reset.
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
   // Current address within NES system memory where
   // sample data is being fetched from.
   unsigned short m_dmaReaderAddrPtr;

   // Whether or not the channel should generate an interrupt.
   bool          m_dmcIrqEnabled;

   // Whether or not the channel has generated an unacknowleged interrupt.
   bool          m_dmcIrqAsserted;

   // The channel's internal sample buffer.  It is filled
   // by the DMA reader component whenever it empties into
   // the output shift register.
   unsigned char m_sampleBuffer;

   // Internal flag indicating whether or not the DMA reader
   // component needs to fetch more sample data.
   bool          m_sampleBufferFull;

   // Flag indicating whether the sample should be looped or not.
   bool          m_loop;

   // Internal storage for sample address and length.  These are
   // calculated on writes to $4012 and $4013.
   unsigned short m_sampleAddr;
   unsigned short m_sampleLength;

   // The channel's internal output shift register.  At each APU
   // cycle where there is data in the output shift register it is
   // shifted one bit and the dropped bit is used to drive the DAC
   // either up or down (and thus generating a delta-modulation).
   unsigned char m_outputShift;

   // How many bits are left in the output shift register before it
   // needs to be filled from the sample buffer again in order to
   // keep generating the delta-modulation?
   unsigned char m_outputShiftCounter;

   // If no data is in the sample buffer the output shift register
   // will generate 'silence'.
   bool          m_silence;

   // Current memory location for fetching samples from the music
   // designer.  If this address is being used the m_dmaReaderAddrPtr
   // is ignored.
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
   static void LENGTHCOUNTERS ( unsigned short* sq1, unsigned short* sq2, unsigned short* triangle, unsigned short* noise, unsigned short* dmc )
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
   static void DMCIRQ ( bool* enabled, bool* asserted )
   {
      (*enabled) = m_dmc.IRQENABLED();
      (*asserted) = m_dmc.IRQASSERTED();
   }
   static void SAMPLEINFO ( unsigned short* addr, unsigned short* length, unsigned short* pos )
   {
      (*addr) = m_dmc.SAMPLEADDR();
      (*length) = m_dmc.SAMPLELENGTH();
      (*pos) = m_dmc.SAMPLEPOS();
   }
   static void DMAINFO ( unsigned char* buffer, bool* full )
   {
      (*buffer) = m_dmc.SAMPLEBUFFER();
      (*full) = m_dmc.SAMPLEBUFFERFULL();
   }
   static int SEQUENCERMODE ( void ) { return m_sequencerMode; }

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
