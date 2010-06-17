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

protected:
   static unsigned char  m_reg;
};

#endif
