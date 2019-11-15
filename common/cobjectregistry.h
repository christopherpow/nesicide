#ifndef COBJECTREGISTRY_H
#define COBJECTREGISTRY_H

#include <QHash>
#include <QString>
#include <QObject>
#include <QMutex>

class CObjectRegistry
{
public:
   CObjectRegistry() {};
   static QObject* getObject(const QString& name);
   static void     addObject(const QString& name,
                             QObject* object);
   static void     removeObject(const QString& name);
private:
   static QHash<QString,QObject*> objects;
   static QMutex mutex;
};

#endif // COBJECTREGISTRY_H
