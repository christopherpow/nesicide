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
   m_operation = DoCompile;
   compileSemaphore.release();
}

void CompilerThread::clean()
{
   m_operation = DoClean;
   compileSemaphore.release();
}

void CompilerThread::run ()
{
   CCartridgeBuilder cartridgeBuilder;

   for ( ; ; )
   {
      // Acquire the compile semaphore to know when the main thread wants a compile done...
      compileSemaphore.acquire();

      if ( m_isTerminating )
      {
         break;
      }

      switch ( m_operation )
      {
      case DoCompile:
         emit compileStarted();
         m_assembledOk = cartridgeBuilder.build();
         emit compileDone(m_assembledOk);
         break;
      case DoClean:
         cartridgeBuilder.clean();
         break;
      }
   }

   return;
}
