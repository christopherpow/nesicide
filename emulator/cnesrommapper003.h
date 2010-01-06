// cnesrommapper003.h: interface for the CROMMapper003 class.
//
//////////////////////////////////////////////////////////////////////

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

#endif // !defined(AFX_ROMMAPPER003_H__10DD2663_653F_490B_8657_00D30FD93AB9__INCLUDED_)
