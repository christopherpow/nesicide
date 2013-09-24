#include "cobjectregistry.h"

QHash<QString,QObject*> CObjectRegistry::objects;

QObject* CObjectRegistry::getObject(const QString& name)
{
   if ( objects.contains(name) )
   {
      return objects[name];
   }

   return 0;
}

void CObjectRegistry::addObject(const QString& name, QObject* object)
{
   objects.insert ( name, object );
}

void CObjectRegistry::removeObject(const QString &name)
{
   objects.remove(name);
}
