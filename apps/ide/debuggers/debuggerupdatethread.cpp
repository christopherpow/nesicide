#include "debuggerupdatethread.h"

#include "dbg_cnesppu.h"

#include "main.h"

DebuggerUpdateThread::DebuggerUpdateThread(void (*func)(),QObject */*parent*/) :
    QObject(),
    _func(func)
{
   pThread = new QThread();

   moveToThread(pThread);

   pThread->start();
}

DebuggerUpdateThread::~DebuggerUpdateThread()
{
   _func = NULL;
   pThread->exit(0);
   pThread->wait();
   delete pThread;
}

void DebuggerUpdateThread::updateDebuggers()
{
   if ( _func )
         _func();

   emit updateComplete();
}
