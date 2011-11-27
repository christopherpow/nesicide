#include "breakpointwatcherthread.h"

#include <QSemaphore>
#include <stdio.h>

#include "dbg_cnes.h"

#include "main.h"

BreakpointWatcherThread::BreakpointWatcherThread(QObject*)
{
   m_isTerminating = false;

   semaphore = new QSemaphore(0);
}

BreakpointWatcherThread::~BreakpointWatcherThread()
{
   delete semaphore;
}

void BreakpointWatcherThread::kill()
{
   m_isTerminating = true;
   semaphore->release();
}

void BreakpointWatcherThread::run ()
{
   CBreakpointInfo* pBreakpoints = nesGetBreakpointDatabase();
   int idx;
   char hitMsg [ 256 ];

   for ( ; ; )
   {
      // Acquire the semaphore...which will block us until a breakpoint is hit...
      semaphore->acquire();

      if ( m_isTerminating )
      {
         break;
      }

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
