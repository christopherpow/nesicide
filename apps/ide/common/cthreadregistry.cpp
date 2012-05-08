#include "cthreadregistry.h"

QHash<QString,CThreadRegistry::CThreadManager*> CThreadRegistry::threads;

QThread* CThreadRegistry::getThread(const QString& name)
{
   if ( threads.contains(name) )
   {
      return threads[name]->thread;
   }

   return 0;
}

void CThreadRegistry::addThread(const QString& name, QThread* thread)
{
   CThreadManager* pThreadManager = new CThreadManager;
   pThreadManager->thread = thread;

   threads.insert ( name, pThreadManager );
}

void CThreadRegistry::removeThread(const QString &name)
{
   threads.remove(name);
}
