#include "breakpointwatcherthread.h"

#include <QSemaphore>

QSemaphore breakpointWatcherSemaphore(0);

BreakpointWatcherThread::BreakpointWatcherThread(QObject *parent)
{
   m_isTerminating = false;
}

BreakpointWatcherThread::~BreakpointWatcherThread()
{
   m_isTerminating = true;
   breakpointWatcherSemaphore.release();
   this->wait(ULONG_MAX);
}

void BreakpointWatcherThread::setDialog(QDialog* dialog)
{
}

void BreakpointWatcherThread::run ()
{
   for ( ; ; )
   {
      // Acquire the semaphore...which will block us until a breakpoint is hit...
      breakpointWatcherSemaphore.acquire();

      // Terminate if necessary...
      if ( m_isTerminating )
      {
         break;
      }

      // A breakpoint has occurred...
      emit breakpointHit();
   }

   return;
}
