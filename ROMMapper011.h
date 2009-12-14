// ROMMapper011.h: interface for the CROMMapper011 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined ( ROM_MAPPER011_H )
#define ROM_MAPPER011_H

#include "ROM.h"

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
   static void DISPLAY ( char* sz );

protected:
   static unsigned char m_reg;
};

#endif // !defined(AFX_ROMMAPPER011_H__9D0C0DA1_20D4_444A_A7AA_7B32B637674B__INCLUDED_)
