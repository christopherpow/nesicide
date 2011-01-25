#include "breakpointwatcherthread.h"

#include <QSemaphore>
#include <stdio.h>

#include "dbg_cnes.h"

#include "main.h"

QSemaphore breakpointWatcherSemaphore(0);

BreakpointWatcherThread::BreakpointWatcherThread(QObject*)
{
   m_isTerminating = false;
}

BreakpointWatcherThread::~BreakpointWatcherThread()
{

}

void BreakpointWatcherThread::kill()
{
   m_isTerminating = true;
   breakpointWatcherSemaphore.release();
}

void BreakpointWatcherThread::run ()
{
   CBreakpointInfo* pBreakpoints = nesGetBreakpointDatabase();
   int idx;
   char hitMsg [ 256 ];

   for ( ; ; )
   {
      if ( m_isTerminating )
      {
         break;
      }

      // Acquire the semaphore...which will block us until a breakpoint is hit...
      breakpointWatcherSemaphore.acquire();

      for ( idx = 0; idx < pBreakpoints->GetNumBreakpoints(); idx++ )
      {
         BreakpointInfo* pBreakpoint = pBreakpoints->GetBreakpoint(idx);

         if ( pBreakpoint->hit )
         {
            pBreakpoints->GetHitPrintable(idx,hitMsg);

            debugTextLogger->write ( hitMsg );

            emit showPane(OutputPaneDockWidget::Output_Debug);
         }
      }

      nesClearAudioSamplesAvailable();

      // A breakpoint has occurred...

      emit breakpointHit();
   }

   return;
}
