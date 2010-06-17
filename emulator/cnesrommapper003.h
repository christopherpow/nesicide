#if !defined ( ROM_MAPPER003_H )
#define ROM_MAPPER003_H

#include "cnesrom.h"

class CROMMapper003 : public CROM  
{
public:
	CROMMapper003();
	virtual ~CROMMapper003();

   static void RESET ();
   static UINT MAPPER ( UINT addr );
   static void MAPPER ( UINT addr, unsigned char data );
   static void LOAD ( MapperState* data );
   static void SAVE ( MapperState* data );

protected:
   static unsigned char  m_reg;
};

#endif
