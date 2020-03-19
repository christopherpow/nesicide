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
protected:
   CROMMapper024();
public:
   static inline CROMMapper024* CARTFACTORY() { return new CROMMapper024(); }
   virtual ~CROMMapper024();

   void RESET ( bool soft );
   void HMAPPER ( uint32_t addr, uint8_t data );
   void SYNCCPU ( bool write, uint16_t addr, uint8_t data );
   uint32_t DEBUGINFO ( uint32_t addr );
   uint16_t AMPLITUDE ( void );

   static void SOUNDENABLE(uint32_t mask) { m_soundEnableMask = mask; }

protected:
   // VRC6
   uint8_t  m_reg [ 23 ];
   uint8_t  m_chr [ 8 ];
   uint8_t  m_irqReload;
   uint8_t  m_irqCounter;
   uint8_t  m_irqPrescaler;
   uint8_t  m_irqPrescalerPhase;
   bool     m_irqEnabled;

   static uint32_t  m_soundEnableMask;

   // VRC6 sound
   VRC6PulseChannel m_pulse[2];
   VRC6SawtoothChannel m_sawtooth;
};

#endif
