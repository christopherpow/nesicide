// cnesrommapper010.h: interface for the CROMMapper010 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined ( ROM_MAPPER010_H )
#define ROM_MAPPER010_H

#include "cnesrom.h"

class CROMMapper010 : public CROM  
{
public:
	CROMMapper010();
	virtual ~CROMMapper010();

   static void RESET ();
   static UINT MAPPER ( UINT addr );
   static void MAPPER ( UINT addr, unsigned char data );
   static void LATCH ( UINT addr );
   static void LOAD ( MapperState* data );
   static void SAVE ( MapperState* data );

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

#endif // !defined(AFX_ROMMAPPER010_H__24DA3023_3FC3_4210_9DD9_6E52F025E61A__INCLUDED_)
