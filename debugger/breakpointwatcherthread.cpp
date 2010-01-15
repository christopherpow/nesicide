#include "breakpointwatcherthread.h"

#include <QSemaphore>

QSemaphore breakpointWatcherSemaphore(0);

BreakpointWatcherThread::BreakpointWatcherThread(QObject *parent)
{
}

void BreakpointWatcherThread::setDialog(QDialog* dialog)
{
}

void BreakpointWatcherThread::run ()
{
   while ( 1 )
   {
      // Acquire the semaphore...which will block us until a breakpoint is hit...
      breakpointWatcherSemaphore.acquire();

      // A breakpoint has occurred...
      emit breakpointHit();
   }

   return;
}
