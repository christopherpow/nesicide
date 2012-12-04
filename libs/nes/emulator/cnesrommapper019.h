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
public:
   CROMMapper019();
   virtual ~CROMMapper019();

   static void RESET ( bool soft );
   static uint32_t LMAPPER ( uint32_t addr );
   static void LMAPPER ( uint32_t addr, uint8_t data );
   static void HMAPPER ( uint32_t addr, uint8_t data );
   static void SYNCCPU ( void );
   static uint32_t DEBUGINFO ( uint32_t addr );
   static uint16_t AMPLITUDE ( void );
   static void SOUNDENABLE ( uint32_t mask )
   {
      uint8_t bit;
      for ( bit = 0; bit < 8; bit++ )
      {
         m_wave[bit].muted = !(mask&(0x01<<bit));
      }
   }

protected:
   // N106
   static uint8_t  m_reg [ 19 ];
   static uint8_t  m_chr [ 8 ];
   static uint16_t m_irqCounter;
   static bool     m_irqEnabled;

   // N106 sound
   static N106WaveChannel m_wave[8];

   static uint8_t m_soundRAM[128];
   static uint8_t m_soundRAMAddr;
   static uint8_t m_soundChansEnabled;
};

#endif
