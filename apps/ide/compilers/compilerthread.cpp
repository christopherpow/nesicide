#include "compilerthread.h"

#include "ccartridgebuilder.h"
#include "cmachineimagebuilder.h"

#include "main.h"

CompilerThread::CompilerThread(QObject*)
{
   m_isTerminating = false;
   m_assembledOk = false;

   compileSemaphore = new QSemaphore(0);
}

CompilerThread::~CompilerThread()
{
   delete compileSemaphore;
}

void CompilerThread::kill()
{
   m_isTerminating = true;
   compileSemaphore->release();
}

void CompilerThread::assemble()
{
   m_operation = DoCompile;
   compileSemaphore->release();
}

void CompilerThread::clean()
{
   m_operation = DoClean;
   compileSemaphore->release();
}

void CompilerThread::run ()
{
   CCartridgeBuilder cartridgeBuilder;
   CMachineImageBuilder machineImageBuilder;

   for ( ; ; )
   {
      // Acquire the compile semaphore to know when the main thread wants a compile done...
      compileSemaphore->acquire();

      if ( m_isTerminating )
      {
         break;
      }

      switch ( m_operation )
      {
      case DoCompile:
         emit compileStarted();
         if ( !nesicideProject->getProjectTarget().compare("nes",Qt::CaseInsensitive) )
         {
            m_assembledOk = cartridgeBuilder.build();
         }
         else if ( !nesicideProject->getProjectTarget().compare("c64",Qt::CaseInsensitive) )
         {
            m_assembledOk = machineImageBuilder.build();
         }
         emit compileDone(m_assembledOk);
         break;
      case DoClean:
         if ( !nesicideProject->getProjectTarget().compare("nes",Qt::CaseInsensitive) )
         {
            cartridgeBuilder.clean();
         }
         else if ( !nesicideProject->getProjectTarget().compare("c64",Qt::CaseInsensitive) )
         {
            machineImageBuilder.clean();
         }
         break;
      }
   }

   return;
}
