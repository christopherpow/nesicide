#include "debuggerupdatethread.h"

#include "dbg_cnesppu.h"

#include "main.h"

QThread *DebuggerUpdateThread::pThread = NULL;
int      DebuggerUpdateThread::resourceCount = -1;
QMutex  *DebuggerUpdateThread::pMutex = NULL;
bool     DebuggerUpdateThread::silenced = true;

DebuggerUpdateWorker::DebuggerUpdateWorker(void (*func)(),QObject */*parent*/) :
    _func(func)
{
}

DebuggerUpdateWorker::~DebuggerUpdateWorker()
{
}

void DebuggerUpdateWorker::updateDebuggers()
{
   if ( _func )
   {
      _func();
      emit updateComplete();
   }
}

DebuggerUpdateThread::DebuggerUpdateThread(void (*func)(),QObject */*parent*/)
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

   pWorker = new DebuggerUpdateWorker(func);

   QObject::connect(pWorker,SIGNAL(updateComplete()),this,SIGNAL(updateComplete()));

   pWorker->moveToThread(pThread);
}

DebuggerUpdateThread::~DebuggerUpdateThread()
{
   delete pWorker;
   pWorker = NULL;

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
   if ( !silenced )
   {
      pWorker->updateDebuggers();
   }
}
