#if !defined ( IO_H )
#define IO_H

#include "cjoypadlogger.h"
#include "nes_emulator_core.h"

class CIO
{
public:
   CIO();
   virtual ~CIO();

   static void IO ( uint32_t addr, uint8_t data );
   static uint32_t IO ( uint32_t addr );
   static void _IO ( uint32_t addr, uint8_t data );
   static uint32_t _IO ( uint32_t addr );
   static inline void JOY ( uint8_t joy, uint8_t data )
   {
      *(m_ioJoy+joy) = data;
   }

protected:
   static uint8_t  m_ioJoy [ NUM_CONTROLLERS ];
};

class CIOStandardJoypad : public CIO
{
public:
   CIOStandardJoypad();
   virtual ~CIOStandardJoypad();

   static void IO ( uint32_t addr, uint8_t data );
   static uint32_t IO ( uint32_t addr );
   static void _IO ( uint32_t addr, uint8_t data );
   static uint32_t _IO ( uint32_t addr );
   static inline CJoypadLogger* LOGGER ( int idx ) { return m_logger+idx; }

protected:
   static uint8_t   m_ioJoyLatch [ NUM_CONTROLLERS ];
   static uint8_t   m_last4016;
   static CJoypadLogger  m_logger [ NUM_CONTROLLERS ];
};

class CIOVaus : public CIO
{
public:
   CIOVaus();
   virtual ~CIOVaus();

   static void IO ( uint32_t addr, uint8_t data );
   static uint32_t IO ( uint32_t addr );
   static void _IO ( uint32_t addr, uint8_t data );
   static uint32_t _IO ( uint32_t addr );
   static void SPECIAL ( int32_t port, int32_t special );

protected:
   static uint8_t   m_ioPotLatch [ NUM_CONTROLLERS ];
   static uint8_t   m_last4016;
   static uint8_t   m_trimPot [ NUM_CONTROLLERS ];
};

class CIOZapper : public CIO
{
public:
   CIOZapper();
   virtual ~CIOZapper();

   static void IO ( uint32_t addr, uint8_t data );
   static uint32_t IO ( uint32_t addr );
   static void _IO ( uint32_t addr, uint8_t data );
   static uint32_t _IO ( uint32_t addr );
};

#endif
