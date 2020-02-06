#include "debuggerupdatethread.h"

#include "dbg_cnesppu.h"

#include "main.h"

QThread* DebuggerUpdateThread::pThread = NULL;
int      DebuggerUpdateThread::resourceCount = -1;
QMutex*  DebuggerUpdateThread::pMutex = NULL;

DebuggerUpdateThread::DebuggerUpdateThread(void (*func)(),QObject */*parent*/) :
    QObject(),
    _func(func)
{
   if ( resourceCount == -1 )
   {
      pThread = new QThread();
      pThread->start();
      pMutex = new QMutex();
      pMutex->lock();
      resourceCount = 1;
      pMutex->unlock();
   }
   else
   {
      pMutex->lock();
      resourceCount++;
      pMutex->unlock();
   }

   moveToThread(pThread);
}

DebuggerUpdateThread::~DebuggerUpdateThread()
{
   _func = NULL;
   pMutex->lock();
   resourceCount--;
   if ( resourceCount == 0 )
   {
      resourceCount = -1;
      pThread->exit(0);
      pThread->wait();
      delete pThread;
   }
   pMutex->unlock();
}

void DebuggerUpdateThread::updateDebuggers()
{
   if ( _func )
   {
      _func();
      emit updateComplete();
   }
}
