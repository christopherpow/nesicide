// cnesrommapper009.h: interface for the CROMMapper009 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined ( ROM_MAPPER009_H )
#define ROM_MAPPER009_H

#include "cnesrom.h"

class CROMMapper009 : public CROM  
{
public:
	CROMMapper009();
	virtual ~CROMMapper009();

   static void RESET ();
   static UINT MAPPER ( UINT addr );
   static void MAPPER ( UINT addr, unsigned char data );
   static void LATCH ( UINT addr );
   static void LOAD ( MapperState* data );
   static void SAVE ( MapperState* data );
   static void DISPLAY ( char* sz );

protected:
   // MMC3
   static unsigned char  m_reg [ 6 ];
   static unsigned char  m_latch0;
   static unsigned char  m_latch1;
   static unsigned char  m_latch0FD;
   static unsigned char  m_latch0FE;
   static unsigned char  m_latch1FD;
   static unsigned char  m_latch1FE;
};

#endif // !defined(AFX_ROMMAPPER009_H__26330150_5EA0_47C0_8B1F_545A3E7DC97B__INCLUDED_)
