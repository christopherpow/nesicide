// cnes.h: interface for the CNES class.
//
//////////////////////////////////////////////////////////////////////

#if !defined ( NES_H )
#define NES_H

#include "cbreakpointinfo.h"

class CNES
{
public:
	CNES();
	virtual ~CNES();

   static void RUN ( unsigned char* joy );
   static void REPLAY ( bool enable ) { m_bReplay = enable; }
   static bool REPLAY () { return m_bReplay; }
   static unsigned int FRAME () { return m_frame; }

   static void RESET ( void );
   static CBreakpointInfo* BREAKPOINTS ( void ) { return &m_breakpoints; }
   static void CHECKBREAKPOINT ( eBreakpointTarget target );
   static void FORCEBREAKPOINT ( void );
   static bool ATBREAKPOINT ( void ) { return m_bAtBreakpoint; }
   static void CLEARBREAKPOINT ( void ) { m_bAtBreakpoint = false; }

protected:
   static bool         m_bReplay;
   static unsigned int m_frame;
   static CBreakpointInfo m_breakpoints;
   static bool            m_bAtBreakpoint;
};

#endif // !defined(AFX_NES_H__075DAA92_13A4_4462_9903_FBD07E97E908__INCLUDED_)
