#include "breakpointwatcherthread.h"

#include <QSemaphore>
#include <stdio.h>
QSemaphore breakpointWatcherSemaphore(0);

BreakpointWatcherThread::BreakpointWatcherThread(QObject *)
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
   for ( ; ; )
   {
      if ( m_isTerminating )
      {
         break;
      }

      // Acquire the semaphore...which will block us until a breakpoint is hit...
      breakpointWatcherSemaphore.acquire();

      // A breakpoint has occurred...
      emit breakpointHit();
   }

   return;
}
