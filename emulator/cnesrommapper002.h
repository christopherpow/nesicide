// cnesrommapper002.h: interface for the CROMMapper002 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined ( ROM_MAPPER002_H )
#define ROM_MAPPER002_H

#include "cnesrom.h"

class CROMMapper002 : public CROM  
{
public:
	CROMMapper002();
	virtual ~CROMMapper002();

   static void RESET ();
   static UINT MAPPER ( UINT addr );
   static void MAPPER ( UINT addr, unsigned char data );
   static void LOAD ( MapperState* data );
   static void SAVE ( MapperState* data );
   static void DISPLAY ( char* sz );

protected:
   static unsigned char  m_reg;
};

#endif // !defined(AFX_ROMMAPPER002_H__DC9CDCE4_C2E6_445A_A41B_AAB22ECFCCBE__INCLUDED_)
