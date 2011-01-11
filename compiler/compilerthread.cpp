#include "compilerthread.h"

#include "ccartridgebuilder.h"

#include "main.h"

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
}

void CompilerThread::run ()
{
   CCartridgeBuilder cartridgeBuilder;

   m_assembledOk = cartridgeBuilder.build();

   return;
}
