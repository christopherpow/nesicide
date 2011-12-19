#include "debuggerupdatethread.h"

#include "dbg_cnesppu.h"

#include "main.h"

DebuggerUpdateThread::DebuggerUpdateThread(void (*func)(),QObject *parent) :
    QThread(parent),_func(func)
{
}

void DebuggerUpdateThread::run()
{
   _func();

   emit updateComplete();
}

void DebuggerUpdateThread::updateDebuggers()
{
   start();
}
