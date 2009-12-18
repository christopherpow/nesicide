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
   static bool SYNCH ( int scanline );
   static void LOAD ( MapperState* data );
   static void SAVE ( MapperState* data );
   static void DISPLAY ( char* sz );

protected:
   // Irem H-3001
   static unsigned char  m_reg [ 17 ];
   static unsigned char  m_irqCounter;
   static bool           m_irqEnable;
};

#endif // !defined(AFX_ROMMAPPER065_H__128A8157_B38D_42BE_8549_05396E0D1387__INCLUDED_)
