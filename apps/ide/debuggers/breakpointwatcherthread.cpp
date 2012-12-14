#include "breakpointwatcherthread.h"

#include <QSemaphore>
#include <stdio.h>

#include "main.h"

BreakpointWatcherThread::BreakpointWatcherThread(QObject*)
{
   pThread = new QThread();

   moveToThread(pThread);

   pThread->start();
}

BreakpointWatcherThread::~BreakpointWatcherThread()
{
   pThread->terminate();
   pThread->wait();
   delete pThread;
}

void BreakpointWatcherThread::breakpoint()
{
   CBreakpointInfo* pBreakpoints = NULL;
   int idx;
   char hitMsg [ 512 ];

   if ( !nesicideProject->getProjectTarget().compare("nes",Qt::CaseInsensitive) )
   {
      pBreakpoints = nesGetBreakpointDatabase();
   }
   else if ( !nesicideProject->getProjectTarget().compare("c64",Qt::CaseInsensitive) )
   {
      pBreakpoints = c64GetBreakpointDatabase();
   }

   if ( pBreakpoints )
   {
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
   }

   // A breakpoint has occurred...
   emit breakpointHit();
}
