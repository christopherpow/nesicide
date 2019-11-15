#include "cobjectregistry.h"

QHash<QString,QObject*> CObjectRegistry::objects;
QMutex CObjectRegistry::mutex;

QObject* CObjectRegistry::getObject(const QString& name)
{
   QObject* object = NULL;

   mutex.lock();
   if ( objects.contains(name) )
   {
      object = objects[name];
   }
   mutex.lock();

   return object;
}

void CObjectRegistry::addObject(const QString& name, QObject* object)
{
   mutex.lock();
   objects.insert ( name, object );
   mutex.unlock();
}

void CObjectRegistry::removeObject(const QString &name)
{
   mutex.lock();
   objects.remove(name);
   mutex.unlock();
}
