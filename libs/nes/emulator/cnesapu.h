#if !defined ( APU_H )
#define APU_H

#include "cnes.h"

#define NUM_APU_BUFS 16
#define APU_BUFFER_SIZE (NUM_APU_BUFS*APU_SAMPLES)

// APU mask register ($4017) bit definitions.
#define APUSTATUS_FIVEFRAMES 0x80
#define APUSTATUS_IRQDISABLE 0x40

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
   CAPUOscillator(uint8_t periodAdjust);

   // An APU channel should know what channel ID it has.  This isn't
   // useful in the APU itself but is needed by the debugger inspectors
   // and designers that need to do specific things to specific APU channels.
   void SetChannel ( int32_t channel )
   {
      m_channel = channel;
   }

   // This method mutes or unmutes this channel.
   void MUTE(bool mute) { m_muted = !!mute; }

   // This method returns the mute state of the channel.
   bool MUTED() { return !!m_muted; }

   // This method sets data internal to the APU channel.  There is no
   // get method because the APU channels' registers are write-only.
   inline void APU ( uint32_t addr, uint8_t data );

   // This method turns the channel on or off.  The length counter
   // disables the channel when it expires.  Writes to APU register
   // $4015 change the enabled state of the channels.
   inline void ENABLE ( bool enabled )
   {
      m_enabled = enabled;

      if ( !m_enabled )
      {
         m_lengthCounter = 0;
      }
   }

   // This method returns the length counter value.  Reads from APU
   // register $4015 return whether or not any length counters have expired.
   inline uint16_t LENGTH ( void ) const
   {
      return m_lengthCounter;
   }

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
   inline uint32_t CLKDIVIDER ( void );

   // These routines set/get the channels' DAC value.
   inline void SETDAC ( uint8_t dac )
   {
      uint8_t oldDac = m_dac;
      m_dacAverage[m_dacSamples] = dac;
      m_dac = dac;
      m_dacSamples++;
      if ( dac != oldDac )
      {
         CNES::NES()->CHECKBREAKPOINT(eBreakInAPU,eBreakOnAPUEvent,dac,APU_EVENT_SQUARE1_DAC_VALUE+m_channel);
      }
   }
   inline uint8_t GETDAC ( void )
   {
      return m_dac;
   }

   // These routines deal with averaging the DAC value over time.
   inline void CLEARDACAVG ( void )
   {
      m_dacSamples = 0;
   }
   inline uint8_t* GETDACSAMPLES ( void ) { return m_dacAverage; }
   inline uint8_t  GETDACSAMPLECOUNT ( void ) { return m_dacSamples; }

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
      m_sweepVolume = 0;
      m_volume = 0;
      m_volumeSet = 0;
      m_enabled = false;
      m_newHalted = false;
      m_envelopeLoop = false;
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
   uint16_t LENGTHCOUNTER(void) const
   {
      return m_lengthCounter;
   }

   // Returns the channels' current linear counter value.
   uint8_t LINEARCOUNTER(void) const
   {
      return m_linearCounter;
   }

protected:
   // Whether or not the UI has muted this channel.
   bool m_muted;

   // Current linear counter value.
   uint8_t  m_linearCounter;

   // Value to be loaded into linear counter when it needs reloading.
   uint8_t  m_linearCounterReload;

   // Current length counter value.
   uint16_t m_lengthCounter;

   // Whether or not to clock the length counter at next sequencer step
   // where the length counter should be clocked.  Length counter clocking
   // is choked if a write to the length counter occurs at specific points
   // within the APU frame.
   bool m_clockLengthCounter;

   // The number of APU cycles before the channels' internal
   // divider will emit a clock edge.
   uint16_t m_period;

   // The number of cycles to add to the period to adjust its divider.
   uint16_t m_periodAdjust;

   // The current number of cycles into the counting of the
   // current period.  Once this counter hits m_period the
   // timer divider emits a clock edge that drives the rest of
   // the channels' circuitry.
   uint16_t m_periodCounter;

   // These values define the envelope configuration currently
   // being used by the channel.  An envelope can either be a constant
   // volume level or a sawtooth wave.  In the wave case, a divider
   // deals out clock edges at a lower frequency than the period divider
   // governing the channels' pitch.  The envelope divider is used to
   // decrease the envelope's value down to zero and then immediately
   // back to the maximum possible, thus creating a sawtooth waveform.
   uint8_t  m_envelope;
   uint8_t  m_envelopeCounter;
   uint8_t  m_envelopeDivider;
   uint8_t  m_envelopeLoop;

   // These values define the sweep configuration currently being
   // used by the channel.  A divider deals out clock edges at a lower
   // frequency than the period divider governing the channels' pitch.
   // The sweep divider is used to periodically cause shifts in a feedback
   // loop to the channels' period divider, thus causing a change in the
   // channels' pitch either up or down.
   uint8_t  m_sweepShift;
   uint8_t  m_sweepDivider;
   uint8_t  m_sweep;
   uint8_t  m_sweepVolume;

   // The channels' current volume that may eventually make
   // its way into the DAC as the current DAC setting if the
   // channel internals cause a write to the DAC.
   uint8_t  m_volume;
   uint8_t  m_volumeSet;

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
   uint8_t m_dac;
   uint8_t m_dacAverage[100]; // 100 DAC samples should be more than enough to
                              // cover both NTSC and PAL sample-spacing throughout
                              // the frame.
   uint8_t m_dacSamples;

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
   int32_t            m_channel;

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
   uint8_t  m_reg [ 4 ];
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
      CAPUOscillator(2),
      m_seqTick(0),
      m_duty(0) {};

   // This method sets data internal to the APU channel.  There is no
   // get method because the APU channel's registers are write-only.
   void APU ( uint32_t addr, uint8_t data );

   // This method handles the channel mechanics of generating the
   // appropriate square waveform given the register settings.  It
   // is invoked by the main APU object at each APU cycle.  When necessary
   // it updates the DAC value of the channel.
   void TIMERTICK ( void );

   // This routine returns the channels' internal state to
   // what it should be at NES reset.
   void RESET ( void )
   {
      CAPUOscillator::RESET();
      m_duty = 0;
      m_seqTick = 0;
   }

protected:
   // The square waveform channel has an internal 8-step sequencer for
   // generating the appropriate duty-cycle.
   int32_t           m_seqTick;

   // The square waveform channel has four selectable duty-cycle modes.
   int32_t           m_duty;
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
      CAPUOscillator(1),
      m_seqTick(0) {};

   // This method sets data internal to the APU channel.  There is no
   // get method because the APU channel's registers are write-only.
   inline void APU ( uint32_t addr, uint8_t data );

   // This method handles the channel mechanics of generating the
   // appropriate triangle waveform given the register settings.  It
   // is invoked by the main APU object at each APU cycle.  When necessary
   // it updates the DAC value of the channel.
   inline void TIMERTICK ( void );

   // This routine returns the channels' internal state to
   // what it should be at NES reset.
   inline void RESET ( void )
   {
      CAPUOscillator::RESET();
      m_seqTick = 0;
   }

protected:
   // The triangle waveform channel has an internal 32-step sequencer
   // for generating the appropriate triangular waveform.
   int32_t           m_seqTick;
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
   CAPUNoise ()
      : CAPUOscillator(0), m_mode(0), m_shiftRegister(1)
   {
   }

   // This method sets data internal to the APU channel.  There is no
   // get method because the APU channel's registers are write-only.
   inline void APU ( uint32_t addr, uint8_t data );

   // This method handles the channel mechanics of generating the
   // appropriate noise waveform given the register settings.  It
   // is invoked by the main APU object at each APU cycle.  When necessary
   // it updates the DAC value of the channel.
   inline void TIMERTICK ( void );

   // This routine returns the channels' internal state to
   // what it should be at NES reset.
   inline void RESET ( void )
   {
      CAPUOscillator::RESET();
      m_mode = 0;
      m_shiftRegister = 1;
   }

protected:
   // The noise channel has two different modes:
   // 93-cycle pattern
   // 32767-cycle pattern
   int32_t            m_mode;

   uint16_t m_shiftRegister;
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
   CAPUDMC() : CAPUOscillator(0),
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
      m_silence(true),
      m_dmaSource(NULL),
      m_dmaSourcePtr(NULL)
   {};

   // This method sets data internal to the APU channel.  There is no
   // get method because the APU channel's registers are write-only.
   void APU ( uint32_t addr, uint8_t data );

   // This method handles the channel mechanics of generating the
   // appropriate delta-modulation waveform given the register settings.  It
   // is invoked by the main APU object at each APU cycle.  When necessary
   // it updates the DAC value of the channel.  It also drives the other
   // channel elements to fetch sample data using DMA.
   void TIMERTICK ( void );

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
   void DMASOURCE ( uint8_t* source )
   {
      m_dmaSource = source;
      m_dmaSourcePtr = source;
   }

   void DMASAMPLE ( uint8_t data );

   // These methods deal with the delta-modulation channel's interrupt flag.
   bool IRQASSERTED ( void ) const
   {
      return m_dmcIrqAsserted;
   }
   void IRQASSERTED ( bool asserted )
   {
      m_dmcIrqAsserted = asserted;
   }
   bool IRQENABLED ( void ) const
   {
      return m_dmcIrqEnabled;
   }

   // These methods provide internal channel detail to the debuggers.
   uint16_t SAMPLEADDR ( void ) const
   {
      return m_sampleAddr;
   }
   uint16_t SAMPLELENGTH ( void ) const
   {
      return m_sampleLength;
   }
   uint16_t SAMPLEPOS ( void ) const
   {
      return m_dmaReaderAddrPtr;
   }
   uint8_t SAMPLEBUFFER ( void ) const
   {
      return m_sampleBuffer;
   }
   bool SAMPLEBUFFERFULL ( void ) const
   {
      return m_sampleBufferFull;
   }

   // This routine returns the channels' internal state to
   // what it should be at NES reset.
   void RESET ( void );

protected:
   // Current address within NES system memory where
   // sample data is being fetched from.
   uint16_t m_dmaReaderAddrPtr;

   // Whether or not the channel should generate an interrupt.
   bool          m_dmcIrqEnabled;

   // Whether or not the channel has generated an unacknowleged interrupt.
   bool          m_dmcIrqAsserted;

   // The channel's internal sample buffer.  It is filled
   // by the DMA reader component whenever it empties into
   // the output shift register.
   uint8_t m_sampleBuffer;

   // Internal flag indicating whether or not the DMA reader
   // component needs to fetch more sample data.
   bool          m_sampleBufferFull;

   // Flag indicating whether the sample should be looped or not.
   bool          m_loop;

   // Internal storage for sample address and length.  These are
   // calculated on writes to $4012 and $4013.
   uint16_t m_sampleAddr;
   uint16_t m_sampleLength;

   // The channel's internal output shift register.  At each APU
   // cycle where there is data in the output shift register it is
   // shifted one bit and the dropped bit is used to drive the DAC
   // either up or down (and thus generating a delta-modulation).
   uint8_t m_outputShift;

   // How many bits are left in the output shift register before it
   // needs to be filled from the sample buffer again in order to
   // keep generating the delta-modulation?
   uint8_t m_outputShiftCounter;

   // If no data is in the sample buffer the output shift register
   // will generate 'silence'.
   bool          m_silence;

   // Current memory location for fetching samples from the music
   // designer.  If this address is being used the m_dmaReaderAddrPtr
   // is ignored.
   uint8_t* m_dmaSource;
   uint8_t* m_dmaSourcePtr;
};

class C6502;

class CAPU
{
public:
   CAPU();

   void RESET ( void );
   uint32_t APU ( uint32_t addr );
   void APU ( uint32_t addr, uint8_t data );
   void EMULATE ( void );
   uint8_t* PLAY ( uint16_t samples );

   void DMASOURCE ( uint8_t* source )
   {
      m_dmc.DMASOURCE ( source );
   }

   void DMASAMPLE ( uint8_t data )
   {
      m_dmc.DMASAMPLE ( data );
   }

   uint8_t MUTED ( void )
   {
      return ( (!m_square[0].MUTED())|
               ((!m_square[1].MUTED())<<1)|
               ((!m_triangle.MUTED())<<2)|
               ((!m_noise.MUTED())<<3)|
               ((!m_dmc.MUTED())<<4) );
   }
   void MUTE ( uint8_t mask )
   {
      m_square[0].MUTE(!(mask&0x01));
      m_square[1].MUTE(!(mask&0x02));
      m_triangle.MUTE(!(mask&0x04));
      m_noise.MUTE(!(mask&0x08));
      m_dmc.MUTE(!(mask&0x10));
   }

   uint32_t _APU ( uint32_t addr )
   {
      return *(m_APUreg+(addr&0x1F));
   }
   void _APU ( uint32_t addr, uint8_t data )
   {
      *(m_APUreg+(addr&0x1F)) = data;
   }
   inline uint8_t DIRTY ( uint32_t addr )
   {
      uint8_t updated = *(m_APUregDirty+(addr&0x1F));
      *(m_APUregDirty+(addr&0x1F))=0;
      return updated;
   }

   void RELEASEIRQ ( void );
   inline void SEQTICK ( int32_t sequence );
   inline uint16_t AMPLITUDE ( void );

   inline void RESETCYCLECOUNTER ( uint32_t cycle )
   {
      m_cycles = cycle;
   }
   inline uint32_t CYCLES ( void )
   {
      return m_cycles;
   }

   int32_t SEQUENCERMODE ( void )
   {
      return m_sequencerMode;
   }

   // INTERNAL ACCESSOR FUNCTIONS
   // These are called directly.
   void LENGTHCOUNTERS ( uint16_t* sq1, uint16_t* sq2, uint16_t* triangle, uint16_t* noise, uint16_t* dmc )
   {
      (*sq1) = m_square[0].LENGTHCOUNTER();
      (*sq2) = m_square[1].LENGTHCOUNTER();
      (*triangle) = m_triangle.LENGTHCOUNTER();
      (*noise) = m_noise.LENGTHCOUNTER();
      (*dmc) = m_dmc.LENGTHCOUNTER();
   }
   void LINEARCOUNTER ( uint8_t* triangle )
   {
      (*triangle) = m_triangle.LINEARCOUNTER();
   }
   void GETDACS ( uint8_t* square1,
                         uint8_t* square2,
                         uint8_t* triangle,
                         uint8_t* noise,
                         uint8_t* dmc )
   {
      (*square1) = m_square[0].GETDAC();
      (*square2) = m_square[1].GETDAC();
      (*triangle) = m_triangle.GETDAC();
      (*noise) = m_noise.GETDAC();
      (*dmc) = m_dmc.GETDAC();
   }
   void DMCIRQ ( bool* enabled, bool* asserted )
   {
      (*enabled) = m_dmc.IRQENABLED();
      (*asserted) = m_dmc.IRQASSERTED();
   }
   void SAMPLEINFO ( uint16_t* addr, uint16_t* length, uint16_t* pos )
   {
      (*addr) = m_dmc.SAMPLEADDR();
      (*length) = m_dmc.SAMPLELENGTH();
      (*pos) = m_dmc.SAMPLEPOS();
   }
   void DMAINFO ( uint8_t* buffer, bool* full )
   {
      (*buffer) = m_dmc.SAMPLEBUFFER();
      (*full) = m_dmc.SAMPLEBUFFERFULL();
   }

protected:
   uint8_t m_APUreg [ 32 ];
   uint8_t m_APUregDirty [ 32 ];
   bool m_irqEnabled;
   bool m_irqAsserted;

   int32_t m_sequencerMode;
   int32_t m_newSequencerMode;
   int32_t m_changeModes;
   int32_t m_sequenceStep;

   CAPUSquare m_square[2];
   CAPUTriangle m_triangle;
   CAPUNoise m_noise;
   CAPUDMC m_dmc;

   uint16_t* m_waveBuf;
   int32_t m_waveBufProduce;
   int32_t m_waveBufConsume;

   uint32_t   m_cycles;

   float m_sampleSpacer;
   
   int32_t m_sampleBufferSize;
};

#endif
