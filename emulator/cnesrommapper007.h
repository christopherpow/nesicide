// cnesrommapper007.h: interface for the CROMMapper007 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined ( ROM_MAPPER007_H )
#define ROM_MAPPER007_H

#include "cnesrom.h"

class CROMMapper007 : public CROM  
{
public:
	CROMMapper007();
	virtual ~CROMMapper007();

   static void RESET ();
   static UINT MAPPER ( UINT addr );
   static void MAPPER ( UINT addr, unsigned char data );
   static void LOAD ( MapperState* data );
   static void SAVE ( MapperState* data );

protected:
   static unsigned char  m_reg;
};

#endif
