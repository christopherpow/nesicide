// cnesrommapper004.h: interface for the CROMMapper004 class.
//
//////////////////////////////////////////////////////////////////////

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
   static bool SYNCH ( int scanline );
   static void SETCPU ( void );
   static void SETPPU ( void );
   static void LOAD ( MapperState* data );
   static void SAVE ( MapperState* data );
   static void DISPLAY ( char* sz );

protected:
   // MMC3
   static unsigned char  m_reg [ 8 ];
   static unsigned char  m_irqCounter;
   static unsigned char  m_irqLatch;
   static bool           m_irqEnable;
   static bool           m_irqReload;
   static unsigned char  m_prg [ 2 ];
   static unsigned char  m_chr [ 8 ];
};

#endif // !defined(AFX_ROMMAPPER004_H__12F44ED8_2AE6_4358_976A_AA05DBAED62D__INCLUDED_)
