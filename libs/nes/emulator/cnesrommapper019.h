#if !defined ( ROM_MAPPER019_H )
#define ROM_MAPPER019_H

#include "cnesrom.h"

struct N106WaveChannel
{
   uint8_t  volume;
   uint32_t period;
   uint32_t periodCounter;
   uint8_t  instrumentLength;
   uint8_t  instrumentAddress;
   uint8_t  instrumentStep;
   uint8_t  dacAverage[100];
   uint8_t  dac;
   uint8_t  dacSamples;
   bool     muted;
   uint8_t* pSoundRAM;

   N106WaveChannel()
   {
      muted = false;
   }

   void SOUNDRAM(uint8_t* ram)
   {
      pSoundRAM = ram;
   }

   void RESET()
   {
      dacSamples = 0;
      period = 0;
      periodCounter = 0;
      instrumentLength = 0;
      instrumentAddress = 0;
      instrumentStep = 0;
      volume = 0;
   }
   void REG(uint32_t addr, uint8_t data)
   {
      switch ( addr&0x07 )
      {
      case 0:
         period &= 0x0003FF00;
         period |= data;
         break;
      case 2:
         period &= 0x000300FF;
         period |= (data<<8);
         break;
      case 4:
         period &= 0x0000FFFF;
         period |= ((data&0x03)<<16);
         instrumentLength = (4*(8-((data&0x1C)>>2)));
         break;
      case 6:
         instrumentAddress = data;
         break;
      case 7:
         volume = data&0xF;
         break;
      }
   }
   void TIMERTICK(uint8_t enabled);
   void SETDAC(uint8_t value)
   {
      dacAverage[dacSamples] = value;
      dac = value;
      dacSamples++;
   }
   uint8_t* GETDACSAMPLES()
   {
      return dacAverage;
   }
   uint8_t GETDACSAMPLECOUNT()
   {
      return dacSamples;
   }
   void CLEARDACAVG()
   {
      dacSamples = 0;
   }
};

class CROMMapper019 : public CROM
{
private:
   CROMMapper019();
public:
   static inline CROMMapper019* CARTFACTORY() { return new CROMMapper019(); }
   ~CROMMapper019();

   void RESET ( bool soft );
   uint32_t LMAPPER ( uint32_t addr );
   void LMAPPER ( uint32_t addr, uint8_t data );
   void HMAPPER ( uint32_t addr, uint8_t data );
   void SYNCCPU ( void );
   uint32_t DEBUGINFO ( uint32_t addr );
   uint16_t AMPLITUDE ( void );
   static void SOUNDENABLE ( uint32_t mask ) { m_soundEnableMask = mask; }

protected:
   // N106
   uint8_t  m_reg [ 19 ];
   uint8_t  m_chr [ 8 ];
   uint16_t m_irqCounter;
   bool     m_irqEnabled;

   // N106 sound
   N106WaveChannel m_wave[8];

   static uint32_t  m_soundEnableMask;

   uint8_t m_soundRAM[128];
   uint8_t m_soundRAMAddr;
   uint8_t m_soundChansEnabled;
};

#endif
