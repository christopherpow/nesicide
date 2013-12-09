#include "compilerthread.h"

#include "ccartridgebuilder.h"
#include "cmachineimagebuilder.h"

#include "main.h"

CompilerThread::CompilerThread(QObject*)
{
   m_assembledOk = false;

   pThread = new QThread();

   moveToThread(pThread);

   pThread->start();
}

CompilerThread::~CompilerThread()
{
   pThread->exit(0);
   pThread->wait();
   delete pThread;
}

void CompilerThread::compile()
{
   CCartridgeBuilder cartridgeBuilder;
   CMachineImageBuilder machineImageBuilder;

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
}

void CompilerThread::clean()
{
   CCartridgeBuilder cartridgeBuilder;
   CMachineImageBuilder machineImageBuilder;

   emit cleanStarted();
   if ( !nesicideProject->getProjectTarget().compare("nes",Qt::CaseInsensitive) )
   {
      cartridgeBuilder.clean();
   }
   else if ( !nesicideProject->getProjectTarget().compare("c64",Qt::CaseInsensitive) )
   {
      machineImageBuilder.clean();
   }
   emit cleanDone(true);
}
