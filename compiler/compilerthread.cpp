#include "compilerthread.h"

#include "ccartridgebuilder.h"

#include "main.h"

QSemaphore compileSemaphore(0);

CompilerThread::CompilerThread(QObject*)
{
   m_isTerminating = false;
   m_assembledOk = false;
}

CompilerThread::~CompilerThread()
{

}

void CompilerThread::kill()
{
   m_isTerminating = true;
   compileSemaphore.release();
}

void CompilerThread::assemble()
{
   compileSemaphore.release();
}

void CompilerThread::run ()
{
   CCartridgeBuilder cartridgeBuilder;

   for ( ; ; )
   {
      if ( m_isTerminating )
      {
         break;
      }
      
      // Acquire the compile semaphore to know when the main thread wants a compile done...
      compileSemaphore.acquire();
      
      emit compileStarted();
      
      m_assembledOk = cartridgeBuilder.build();
      
      emit compileDone(m_assembledOk);
   }

   return;
}
