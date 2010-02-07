// cnesio.h: interface for the CIO class.
//
//////////////////////////////////////////////////////////////////////

#if !defined ( IO_H )
#define IO_H

#include "cnesicidecommon.h"

#include "cjoypadlogger.h"

class CIO  
{
public:
	CIO();
	virtual ~CIO();

   static void IO ( UINT addr, unsigned char data );
   static UINT IO ( UINT addr );
   static void _IO ( UINT addr, unsigned char data );
   static UINT _IO ( UINT addr );
   static inline void JOY ( unsigned char joy, unsigned char data ) { *(m_ioJoy+joy) = data; }

   static inline CJoypadLogger* LOGGER ( int idx ) { return m_logger+idx; }

protected:
   static unsigned char  m_ioJoy [ NUM_JOY ];
   static unsigned char  m_ioJoyLatch [ NUM_JOY ];

   static CJoypadLogger  m_logger [ NUM_JOY ];
};

#endif
