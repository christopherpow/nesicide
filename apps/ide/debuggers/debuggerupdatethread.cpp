#include "debuggerupdatethread.h"

#include "dbg_cnesppu.h"

#include "main.h"

DebuggerUpdateThread::DebuggerUpdateThread(void (*func)(),QObject *parent) :
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
   delete pThread;
}

void DebuggerUpdateThread::updateDebuggers()
{
   _func();

   emit updateComplete();
}
