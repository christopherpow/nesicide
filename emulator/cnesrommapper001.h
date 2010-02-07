// cnesrommapper001.h: interface for the CROMMapper001 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined ( ROM_MAPPER001_H )
#define ROM_MAPPER001_H

#include "cnesrom.h"

class CROMMapper001 : public CROM  
{
public:
	CROMMapper001();
	virtual ~CROMMapper001();

   static void RESET ();
   static UINT MAPPER ( UINT addr );
   static void MAPPER ( UINT addr, unsigned char data );
   static void LOAD ( MapperState* data );
   static void SAVE ( MapperState* data );

protected:
   // MMC1
   static unsigned char  m_reg [ 4 ];
   static unsigned char  m_regdef [ 4 ];
   static unsigned char  m_sr;
   static unsigned char  m_sel;
   static unsigned char  m_srCount;
   static UINT           m_lastWriteAddr;
};

#endif
