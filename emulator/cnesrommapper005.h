// crommapper005.h: interface for the CROMMapper005 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined ( ROM_MAPPER005_H )
#define ROM_MAPPER005_H

#include "cnesrom.h"

class CROMMapper005 : public CROM  
{
public:
	CROMMapper005();
	virtual ~CROMMapper005();

   static void RESET ();
   static UINT MAPPER ( UINT addr );
   static void MAPPER ( UINT addr, unsigned char data );
   static UINT LMAPPER ( UINT addr );
   static void LMAPPER ( UINT addr, unsigned char data );
   static void SYNCH ( UINT ppuCycle, UINT ppuAddr );
   static void SETCPU ( void );
   static void SETPPU ( void );
   static void LOAD ( MapperState* data );
   static void SAVE ( MapperState* data );

protected:
   // MMC5
   static unsigned char m_prgMode;
   static unsigned char m_chrMode;
   static unsigned char m_irqScanline;
   static unsigned char m_irqEnabled;
   static unsigned char m_irqStatus;
   static bool          m_prgRAM [ 4 ];
   static bool          m_wp;
   static unsigned char m_wp1;
   static unsigned char m_wp2;
   static unsigned char  m_mult1;
   static unsigned char  m_mult2;
   static unsigned short m_prod;
   static unsigned char  m_fillTile;
   static unsigned char  m_fillAttr;
};

#endif
