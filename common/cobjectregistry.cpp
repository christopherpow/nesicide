#include "cobjectregistry.h"

QHash<QString,CObjectRegistry::CObjectManager*> CObjectRegistry::objects;

QObject* CObjectRegistry::getObject(const QString& name)
{
   if ( objects.contains(name) )
   {
      return objects[name]->object;
   }

   return 0;
}

void CObjectRegistry::addObject(const QString& name, QObject* object)
{
   CObjectManager* pObjectManager = new CObjectManager;
   pObjectManager->object = object;

   objects.insert ( name, pObjectManager );
}

void CObjectRegistry::removeObject(const QString &name)
{
   objects.remove(name);
}
