#include "breakpointwatcherthread.h"

#include <QSemaphore>
#include <stdio.h>
QSemaphore breakpointWatcherSemaphore(0);

BreakpointWatcherThread::BreakpointWatcherThread(QObject *parent)
{
   m_isTerminating = false;
}

BreakpointWatcherThread::~BreakpointWatcherThread()
{

}

void BreakpointWatcherThread::kill()
{
   m_isTerminating = true;
   if (!breakpointWatcherSemaphore.available())
      breakpointWatcherSemaphore.release();
}

void BreakpointWatcherThread::setDialog(QDialog* dialog)
{
}

void BreakpointWatcherThread::run ()
{
   for ( ; ; )
   {
      if ( m_isTerminating )
      {
         if (breakpointWatcherSemaphore.available())
            breakpointWatcherSemaphore.release();
         return;
      }

      // Acquire the semaphore...which will block us until a breakpoint is hit...
      bool acquired = false;
      while (!acquired)
      {
         acquired = breakpointWatcherSemaphore.tryAcquire(1, 1);
         if (!acquired)
            msleep(250);

         // Terminate if necessary...
         if ( m_isTerminating )
         {
            if (breakpointWatcherSemaphore.available())
               breakpointWatcherSemaphore.release();
            return;
         }
      }

      // A breakpoint has occurred...
      emit breakpointHit();
   }

   return;
}
