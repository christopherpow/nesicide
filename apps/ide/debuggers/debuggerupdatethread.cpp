#include "debuggerupdatethread.h"

#include "dbg_cnesppu.h"

#include "main.h"

DebuggerUpdateThread::DebuggerUpdateThread(void (*func)(),QObject */*parent*/) :
    QObject(),_func(func)
{
   pThread = new QThread();

   moveToThread(pThread);

   pThread->start();
}

DebuggerUpdateThread::~DebuggerUpdateThread()
{
   pThread->terminate();
   pThread->wait();
   _func = NULL;
   delete pThread;
}

void DebuggerUpdateThread::updateDebuggers()
{
   if ( _func )
      _func();

   emit updateComplete();
}
