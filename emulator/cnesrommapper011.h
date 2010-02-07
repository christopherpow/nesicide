// cnesrommapper011.h: interface for the CROMMapper011 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined ( ROM_MAPPER011_H )
#define ROM_MAPPER011_H

#include "cnesrom.h"

class CROMMapper011 : public CROM  
{
public:
	CROMMapper011();
	virtual ~CROMMapper011();

   static void RESET ();
   static UINT MAPPER ( UINT addr );
   static void MAPPER ( UINT addr, unsigned char data );
   static void LOAD ( MapperState* data );
   static void SAVE ( MapperState* data );

protected:
   static unsigned char m_reg;
};

#endif
