#include "breakpointwatcherthread.h"

#include <QSemaphore>
#include <stdio.h>

#include "main.h"

BreakpointWatcherWorker::BreakpointWatcherWorker(QObject*)
{
}

BreakpointWatcherWorker::~BreakpointWatcherWorker()
{
}

void BreakpointWatcherWorker::breakpoint()
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

BreakpointWatcherThread::BreakpointWatcherThread(QObject*)
{
   pWorker = new BreakpointWatcherWorker();

   QObject::connect(pWorker,SIGNAL(breakpointHit()),this,SIGNAL(breakpointHit()));
   QObject::connect(pWorker,SIGNAL(showPane(int)),this,SIGNAL(showPane(int)));

   pThread = new QThread();

   pWorker->moveToThread(pThread);

   pThread->start();
}

BreakpointWatcherThread::~BreakpointWatcherThread()
{
   pThread->exit(0);
   pThread->wait();
   delete pThread;
   delete pWorker;
}
