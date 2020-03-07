#ifndef COBJECTREGISTRY_H
#define COBJECTREGISTRY_H

#include <QHash>
#include <QString>
#include <QObject>
#include <QMutex>

class CObjectRegistry
{
public:
   static CObjectRegistry *_instance;
   static CObjectRegistry *instance()
   {
      if ( !_instance )
      {
         _instance = new CObjectRegistry();
      }
      return _instance;
   }
   QObject* getObject(const QString& name);
   void     addObject(const QString& name,
                             QObject* object);
   void     removeObject(const QString& name);
private:
   CObjectRegistry();
   QHash<QString,QObject*> objects;
   QMutex *mutex;
};

#endif // COBJECTREGISTRY_H
