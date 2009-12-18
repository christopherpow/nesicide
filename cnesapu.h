// cnesapu.h: interface for the CAPU class.
//
//////////////////////////////////////////////////////////////////////

#if !defined ( APU_H )
#define APU_H

#include "cnesicidecommon.h"

// CPTODO: use phonon instead of wavOut...
//#include "mmsystem.h"

#define APUSTATUS_FIVEFRAMES 0x80
#define APUSTATUS_IRQDISABLE 0x40

class CAPUOscillator
{
public:
	CAPUOscillator();
   virtual ~CAPUOscillator() {};
   
   void SetChannel ( int channel ) { m_channel = channel; }

   inline void APU ( UINT addr, unsigned char data );
   inline void ENABLE ( bool enabled ) { m_enabled = enabled; if ( !m_enabled ) m_lengthCounter = 0; }
   inline unsigned short LENGTH ( void ) const { return m_lengthCounter; }
   inline void CLKLENGTHCOUNTER ( void );
   inline void CLKSWEEPUNIT ( void );
   inline void CLKENVELOPE ( void );
   inline void CLKLINEARCOUNTER ( void );
   inline UINT CLKDIVIDER ( UINT sampleTicks );
   inline void SETDAC ( unsigned char dac ) { m_dac = dac; }
   inline unsigned char GETDAC ( void ) { return m_dac; }
   inline bool IsEnabled ( void ) const { return m_enabled; }
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
      m_halted = false;
      m_linearCounterHalted = false;
      m_envelopeEnabled = false;
      m_sweepEnabled = false;
      m_sweepNegate = false;
      m_dac = 0;
      m_reg1Wrote = false;
      m_reg3Wrote = false;
   }

protected:
   unsigned char  m_linearCounter;
   unsigned char  m_linearCounterReload;
   unsigned short m_lengthCounter;
   unsigned short m_period;
   unsigned short m_periodCounter;
   unsigned char  m_envelope;
   unsigned char  m_envelopeCounter;
   unsigned char  m_envelopeDivider;
   unsigned char  m_sweepShift;
   unsigned char  m_sweepDivider;
   unsigned char  m_sweep;
   unsigned char  m_volume;
   unsigned char  m_volumeSet;
   bool           m_enabled;
   bool           m_halted;
   bool           m_linearCounterHalted;
   bool           m_envelopeEnabled;
   bool           m_sweepEnabled;
   bool           m_sweepNegate;
   unsigned char  m_dac;
   bool           m_reg1Wrote;
   bool           m_reg3Wrote;
   int            m_channel;
   unsigned char  m_reg [ 4 ];
};

static int m_squareSeq [ 4 ] [ 8 ] = 
{
   { 0, 1, 0, 0, 0, 0, 0, 0 },
   { 0, 1, 1, 0, 0, 0, 0, 0 },
   { 0, 1, 1, 1, 1, 0, 0, 0 },
   { 1, 0, 0, 1, 1, 1, 1, 1 }
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
   inline void TIMERTICK ( UINT sampleTicks );
   inline void RESET ( void ) { CAPUOscillator::RESET(); m_duty = 0; m_seqTick = 0; m_timerClk = 0; }

protected:
   int           m_timerClk;
   int           m_seqTick;
   int           m_duty;
};

static int m_triangleSeq [ 32 ] = 
{
   0xF, 0xE, 0xD, 0xC, 0xB, 0xA, 0x9, 0x8,
   0x7, 0x6, 0x5, 0x4, 0x3, 0x2, 0x1, 0x0,
   0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7,
   0x8, 0x9, 0xA, 0xB, 0xC, 0xD, 0xE, 0xF
};

class CAPUTriangle : public CAPUOscillator
{
public:
	CAPUTriangle() : 
      CAPUOscillator(),
      m_seqTick(0) {};
   virtual ~CAPUTriangle() {};

   inline void APU ( UINT addr, unsigned char data );
   inline void TIMERTICK ( UINT sampleTicks );
   inline void RESET ( void ) { CAPUOscillator::RESET(); m_seqTick = 0; }

protected:
   int           m_seqTick;
};

static unsigned short m_noisePeriod [ 16 ] = 
{
   0x004,
   0x008,
   0x010,
   0x020,
   0x040,
   0x060,
   0x080,
   0x0A0,
   0x0CA,
   0x0FE,
   0x17C,
   0x1FC,
   0x2FA,
   0x3F8,
   0x7F2,
   0xFE4
};

class CAPUNoise : public CAPUOscillator
{
public:
	CAPUNoise();
   virtual ~CAPUNoise() {};

   inline void APU ( UINT addr, unsigned char data );
   inline void TIMERTICK ( UINT sampleTicks );
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

static unsigned char m_lengthLUT [ 32 ] = 
{
   0x0A,
   0xFE,
   0x14, 
   0x02,
   0x28, 
   0x04,
   0x50, 
   0x06,
   0xA0, 
   0x08,
   0x3C, 
   0x0A,
   0x0E, 
   0x0C,
   0x1A, 
   0x0E,
   0x0C, 
   0x10,
   0x18, 
   0x12,
   0x30, 
   0x14,
   0x60, 
   0x16,
   0xC0, 
   0x18,
   0x48, 
   0x1A,
   0x10, 
   0x1C,
   0x20, 
   0x1E
};

static unsigned short m_dmcPeriod [ 16 ] = 
{
   0x1AC,
   0x17C,
   0x154,
   0x140,
   0x11E,
   0x0FE,
   0x0E2,
   0x0D6,
   0x0BE,
   0x0A0,
   0x08E,
   0x080,
   0x06A,
   0x054,
   0x048,
   0x036
};

class CAPUDMC : public CAPUOscillator
{
public:
	CAPUDMC() : CAPUOscillator(), 
               m_loop(false),
               m_sampleAddr(0x0000), 
               m_sampleLength(0x0000),
               m_dmaReaderAddrPtr(0x0000),
               m_irqEnabled(false), 
               m_irqAsserted(false), 
               m_sampleBuffer(0x00),
               m_sampleBufferFull(false),
               m_outputShift(0x00),
               m_outputShiftCounter(0),
               m_silence(false),
               m_dmaSource(NULL),
               m_dmaSourcePtr(NULL)
   {};
   virtual ~CAPUDMC() {};

   inline void APU ( UINT addr, unsigned char data );
   inline void TIMERTICK ( UINT sampleTicks );
   inline void ENABLE ( bool enabled );
   UINT DMAREADER ( void );
   void DMASOURCE ( unsigned char* source ) { m_dmaSource = source; m_dmaSourcePtr = source; }
   bool IRQASSERTED ( void ) const { return m_irqAsserted; }
   inline void RESET ( void ) 
   { 
      CAPUOscillator::RESET(); 
      m_loop = false;
      m_sampleAddr = 0x0000; 
      m_sampleLength = 0x0000; 
      m_dmaReaderAddrPtr = 0x0000;
      m_irqEnabled = false; 
      m_irqAsserted = false; 
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
   bool          m_irqEnabled;
   bool          m_irqAsserted;
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
   static void SETFREQ ( int iFreq ) { m_iFreq = iFreq; }
   static void SETFACTORINDEX ( int factorIdx ) { CAPU::CLOSE(); m_iFactorIdx = factorIdx; CAPU::OPEN(); }

   static UINT _APU ( UINT addr ) { return *(m_APUreg+(addr&0x1F)); }
   static void _APU ( UINT addr, unsigned char data ) { *(m_APUreg+(addr&0x1F)) = data; }
   static inline unsigned char DIRTY ( UINT addr ) 
   { 
      unsigned char updated = *(m_APUregDirty+(addr&0x1F));
      *(m_APUregDirty+(addr&0x1F))=0; 
      return updated; 
   }
   
   static inline unsigned char LENGTHLUT ( int idx ) { return *(m_lengthLUT+idx); }

   static inline void SEQTICK ( void );
   static void GETDACS ( unsigned char* square1,
                         unsigned char* square2,
                         unsigned char* triangle,
                         unsigned char* noise,
                         unsigned char* dmc );
   static inline unsigned short AMPLITUDE ( void );

   static void OPEN ( void );
   static void RUN ( void );
   static void PLAY ( void );
   static void CLOSE ( void );

protected:
   static unsigned char m_APUreg [ 32 ];
   static unsigned char m_APUregDirty [ 32 ];
   static unsigned char m_APUreg4015mask;
   static int m_sequencerMode;
   static int m_sequence;
   static bool m_irqEnabled;
   static bool m_irqAsserted;

   static int m_iFreq;
   static int m_iFactorIdx;

   static CAPUSquare m_square[2];
   static CAPUTriangle m_triangle;
   static CAPUNoise m_noise;
   static CAPUDMC m_dmc;

   // CPTODO: use phonon instead of wavOut...
//   static HWAVEOUT m_hWaveDevice;
//   static WAVEHDR m_waveBufQueue [ NUM_APU_BUFS ];
   static bool m_waveBufsQueued;
   static unsigned short m_waveBuf [ NUM_APU_BUFS ][ 2000 ];
   static int m_waveBufDepth [ NUM_APU_BUFS ];
   static int m_waveBufProduce;
   static int m_waveBufConsume;
   static bool m_waveBufWritten [ NUM_APU_BUFS ];
};

#endif // !defined(AFX_APU_H__AE53BDB6_796A_4B67_BC4D_CC86C1F0191C__INCLUDED_)
