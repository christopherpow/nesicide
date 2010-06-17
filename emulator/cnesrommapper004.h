#if !defined ( ROM_MAPPER004_H )
#define ROM_MAPPER004_H

#include "cnesrom.h"

class CROMMapper004 : public CROM  
{
public:
	CROMMapper004();
	virtual ~CROMMapper004();

   static void RESET ();
   static UINT MAPPER ( UINT addr );
   static void MAPPER ( UINT addr, unsigned char data );
   static void SYNCH ( UINT ppuCycle, UINT ppuAddr );
   static void SETCPU ( void );
   static void SETPPU ( void );
   static void LOAD ( MapperState* data );
   static void SAVE ( MapperState* data );

   // Internal accessors for mapper information inspector...
   // Note: called directly!
   static UINT IRQENABLED ( void ) { return m_irqEnable; }
   static UINT IRQASSERTED ( void ) { return m_irqAsserted; }
   static UINT IRQRELOAD ( void ) { return m_irqReload; }
   static UINT IRQCOUNTER ( void ) { return m_irqCounter; }
   static UINT PPUADDRA12 ( void ) { return m_lastPPUAddrA12; }
   static UINT PPUCYCLE ( void ) { return m_lastPPUCycle; }

protected:
   // MMC3
   static unsigned char  m_reg [ 8 ];
   static bool           m_irqAsserted;
   static unsigned char  m_irqCounter;
   static unsigned char  m_irqLatch;
   static bool           m_irqEnable;
   static bool           m_irqReload;
   static unsigned char  m_prg [ 2 ];
   static unsigned char  m_chr [ 8 ];

   static unsigned int   m_lastPPUAddrA12;
   static unsigned int   m_lastPPUCycle;
};

#endif
