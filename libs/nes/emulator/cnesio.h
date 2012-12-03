#if !defined ( IO_H )
#define IO_H

#include "cjoypadlogger.h"
#include "nes_emulator_core.h"

class CIO
{
public:
   CIO() {}

   static void IO ( uint32_t addr, uint8_t data );
   static uint32_t IO ( uint32_t addr );
   static void _IO ( uint32_t addr, uint8_t data );
   static uint32_t _IO ( uint32_t addr );
   static inline void JOY ( uint8_t joy, uint32_t data )
   {
      *(m_ioJoy+joy) = data;
   }

protected:
   static uint32_t  m_ioJoy [ NUM_CONTROLLERS ];
};

class CIOStandardJoypad : public CIO
{
public:
   CIOStandardJoypad() {}

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

class CIOTurboJoypad : public CIOStandardJoypad
{
public:
   CIOTurboJoypad() {}

   static void IO ( uint32_t addr, uint8_t data );
   static void _IO ( uint32_t addr, uint8_t data );
   static uint32_t _IO ( uint32_t addr );

protected:
   static uint32_t m_lastFrame;
   static uint8_t m_alternator [ NUM_CONTROLLERS ][ 2 ];
};

class CIOVaus : public CIO
{
public:
   CIOVaus() {}

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
   CIOZapper() {}

   static void IO ( uint32_t addr, uint8_t data );
   static uint32_t IO ( uint32_t addr );
   static void _IO ( uint32_t addr, uint8_t data );
   static uint32_t _IO ( uint32_t addr );
};

#endif
