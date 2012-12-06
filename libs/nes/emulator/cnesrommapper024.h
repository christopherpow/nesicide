#if !defined ( ROM_MAPPER024_H )
#define ROM_MAPPER024_H

#include "cnesrom.h"

struct VRC6PulseChannel
{
   uint8_t  mode;
   uint8_t  dutyCycle;
   uint8_t  volume;
   uint16_t period;
   uint16_t periodCounter;
   uint8_t  sequencerStep;
   bool     enabled;
   uint8_t  dacAverage[100];
   uint8_t  dac;
   uint8_t  dacSamples;
   bool     muted;

   VRC6PulseChannel()
   {
      muted = false;
   }

   void RESET()
   {
      enabled = false;
      dacSamples = 0;
      period = 0;
      periodCounter = 0;
      sequencerStep = 0;
      volume = 0;
      dutyCycle = 0;
      mode = 0;
   }
   void REG(uint32_t addr, uint8_t data)
   {
      switch ( addr&0x03 )
      {
      case 0:
         mode = data&0x80;
         dutyCycle = (data&0x70)>>4;
         volume = data&0x0F;
         break;
      case 1:
         period &= 0xFF00;
         period |= data;
         break;
      case 2:
         period &= 0x00FF;
         period |= ((data&0x0F)<<8);
         enabled = !!(data&0x80);
         break;
      }
   }
   void TIMERTICK();
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

struct VRC6SawtoothChannel
{
   uint8_t  accumulator;
   uint8_t  accumulatorRate;
   uint8_t  accumulatorCount;
   uint8_t  accumulatorDivider;
   uint16_t period;
   uint16_t periodCounter;
   bool     enabled;
   uint8_t  dacAverage[100];
   uint8_t  dac;
   uint8_t  dacSamples;
   bool     muted;

   VRC6SawtoothChannel()
   {
      muted = false;
   }

   void RESET()
   {
      enabled = false;
      dacSamples = 0;
      period = 0;
      periodCounter = 0;
      accumulator = 0;
      accumulatorRate = 0;
      accumulatorCount = 0;
      accumulatorDivider = 0;
   }
   void REG(uint32_t addr, uint8_t data)
   {
      switch ( addr&0x03 )
      {
      case 0:
         accumulatorRate = data&0x3F;
         break;
      case 1:
         period &= 0xFF00;
         period |= data;
         break;
      case 2:
         period &= 0x00FF;
         period |= ((data&0x0F)<<8);
         enabled = !!(data&0x80);
         break;
      }
   }
   void TIMERTICK();
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

class CROMMapper024 : public CROM
{
public:
   CROMMapper024();
   ~CROMMapper024();

   static void RESET ( bool soft );
   static void HMAPPER ( uint32_t addr, uint8_t data );
   static void SYNCCPU ( void );
   static uint32_t DEBUGINFO ( uint32_t addr );
   static uint16_t AMPLITUDE ( void );
   static void SOUNDENABLE ( uint32_t mask )
   {
      m_pulse[0].muted = !(mask&0x01);
      m_pulse[1].muted = !(mask&0x02);
      m_sawtooth.muted = !(mask&0x04);
   }

protected:
   // VRC6
   static uint8_t  m_reg [ 23 ];
   static uint8_t  m_chr [ 8 ];
   static uint8_t  m_irqReload;
   static uint8_t  m_irqCounter;
   static uint8_t  m_irqPrescaler;
   static uint8_t  m_irqPrescalerPhase;
   static bool     m_irqEnabled;

   // VRC6 sound
   static VRC6PulseChannel m_pulse[2];
   static VRC6SawtoothChannel m_sawtooth;
};

#endif
