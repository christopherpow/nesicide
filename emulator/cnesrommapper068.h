// cnesrommapper068.h: interface for the CROMMapper068 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined ( ROM_MAPPER068_H )
#define ROM_MAPPER068_H

#include "cnesrom.h"

class CROMMapper068 : public CROM  
{
public:
	CROMMapper068();
	virtual ~CROMMapper068();

   static void RESET ();
   static UINT MAPPER ( UINT addr );
   static void MAPPER ( UINT addr, unsigned char data );
   static void LOAD ( MapperState* data );
   static void SAVE ( MapperState* data );

protected:
   // Sunsoft Mapper #4
   static unsigned char  m_reg [ 4 ];
};

#endif
