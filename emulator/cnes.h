// cnes.h: interface for the CNES class.
//
//////////////////////////////////////////////////////////////////////

#if !defined ( NES_H )
#define NES_H

#include "cbreakpointinfo.h"
#include "ctracer.h"

class CNES
{
public:
	CNES();
	virtual ~CNES();

   static void RUN ( unsigned char* joy );
   static void REPLAY ( bool enable ) { m_bReplay = enable; }
   static bool REPLAY () { return m_bReplay; }
   static unsigned int FRAME () { return m_frame; }

   static inline CTracer* TRACER ( void ) { return &m_tracer; }

   static void RESET ( void );
   static CBreakpointInfo* BREAKPOINTS ( void ) { return &m_breakpoints; }
   static void CHECKBREAKPOINT ( eBreakpointTarget target, eBreakpointType type = (eBreakpointType)-1, int data = 0 );
   static void FORCEBREAKPOINT ( void );
   static bool ATBREAKPOINT ( void ) { return m_bAtBreakpoint; }
   static void CLEARBREAKPOINT ( void ) { m_bAtBreakpoint = false; }
   static void STEPCPUBREAKPOINT ( void ) { m_bStepCPUBreakpoint = true; }
   static void STEPPPUBREAKPOINT ( void ) { m_bStepPPUBreakpoint = true; }

protected:
   static bool         m_bReplay;
   static unsigned int m_frame;
   static CBreakpointInfo m_breakpoints;
   static bool            m_bAtBreakpoint;
   static bool            m_bStepCPUBreakpoint;
   static bool            m_bStepPPUBreakpoint;

   static CTracer         m_tracer;
};

#endif
