#include "compilerthread.h"

#include "ccartridgebuilder.h"
#include "cmachineimagebuilder.h"

#include "cnesicideproject.h"

CompilerWorker::CompilerWorker(QObject*)
{
   m_assembledOk = false;
}

CompilerWorker::~CompilerWorker()
{
}

void CompilerWorker::compile()
{
   CCartridgeBuilder cartridgeBuilder;
   CMachineImageBuilder machineImageBuilder;

   emit compileStarted();
   if ( !CNesicideProject::instance()->getProjectTarget().compare("nes",Qt::CaseInsensitive) )
   {
      m_assembledOk = cartridgeBuilder.build();
   }
   else if ( !CNesicideProject::instance()->getProjectTarget().compare("c64",Qt::CaseInsensitive) )
   {
      m_assembledOk = machineImageBuilder.build();
   }
   emit compileDone(m_assembledOk);
}

void CompilerWorker::clean()
{
   CCartridgeBuilder cartridgeBuilder;
   CMachineImageBuilder machineImageBuilder;

   emit cleanStarted();
   if ( !CNesicideProject::instance()->getProjectTarget().compare("nes",Qt::CaseInsensitive) )
   {
      cartridgeBuilder.clean();
   }
   else if ( !CNesicideProject::instance()->getProjectTarget().compare("c64",Qt::CaseInsensitive) )
   {
      machineImageBuilder.clean();
   }
   emit cleanDone(true);
}

CompilerThread::CompilerThread(QObject*)
{
   pWorker = new CompilerWorker();

   QObject::connect(pWorker,SIGNAL(cleanDone(bool)),this,SIGNAL(cleanDone(bool)));
   QObject::connect(pWorker,SIGNAL(cleanStarted()),this,SIGNAL(cleanStarted()));
   QObject::connect(pWorker,SIGNAL(compileDone(bool)),this,SIGNAL(compileDone(bool)));
   QObject::connect(pWorker,SIGNAL(compileStarted()),this,SIGNAL(compileStarted()));

   pThread = new QThread();

   pWorker->moveToThread(pThread);

   pThread->start();
}

CompilerThread::~CompilerThread()
{
   pThread->exit(0);
   pThread->wait();
   delete pThread;
   delete pWorker;
}
