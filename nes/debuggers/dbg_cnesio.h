#if !defined ( IO_H )
#define IO_H

#include "cnesicidecommon.h"

#if defined ( IDE_BUILD )
#include "cjoypadlogger.h"
#endif

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

#if defined ( IDE_BUILD )
   static inline CJoypadLogger* LOGGER ( int32_t idx )
   {
      return m_logger+idx;
   }
#endif

protected:
   static uint8_t  m_ioJoy [ NUM_JOY ];
   static uint8_t  m_ioJoyLatch [ NUM_JOY ];

   static uint8_t  m_last4016;

#if defined ( IDE_BUILD )
   static CJoypadLogger  m_logger [ NUM_JOY ];
#endif
};

#endif
