// cnesrommapper065.h: interface for the CROMMapper065 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined ( ROM_MAPPER065_H )
#define ROM_MAPPER065_H

#include "cnesrom.h"

class CROMMapper065 : public CROM  
{
public:
	CROMMapper065();
	virtual ~CROMMapper065();

   static void RESET ();
   static UINT MAPPER ( UINT addr );
   static void MAPPER ( UINT addr, unsigned char data );
   static void SYNCH ( UINT ppuCycle, UINT ppuAddr );
   static void LOAD ( MapperState* data );
   static void SAVE ( MapperState* data );

protected:
   // Irem H-3001
   static unsigned char  m_reg [ 17 ];
   static unsigned char  m_irqCounter;
   static bool           m_irqEnable;
};

#endif
