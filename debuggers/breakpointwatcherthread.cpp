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
   CBreakpointInfo* pBreakpoints;
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

      if ( !nesicideProject->getProjectTarget().compare("nes",Qt::CaseInsensitive) )
      {
         pBreakpoints = nesGetBreakpointDatabase();
      }
      else if ( !nesicideProject->getProjectTarget().compare("c64",Qt::CaseInsensitive) )
      {
         pBreakpoints = c64GetBreakpointDatabase();
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

      if ( !nesicideProject->getProjectTarget().compare("nes",Qt::CaseInsensitive) )
      {
         nesClearAudioSamplesAvailable();
      }

      // A breakpoint has occurred...
      emit breakpointHit();
   }

   return;
}
