#include "compilerthread.h"

#include "ccartridgebuilder.h"

#include "main.h"

CompilerThread::CompilerThread(QObject *)
{
   m_isTerminating = false;
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

   cartridgeBuilder.build();

   return;
}
