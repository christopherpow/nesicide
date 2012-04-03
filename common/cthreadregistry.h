#ifndef CTHREADREGISTRY_H
#define CTHREADREGISTRY_H

#include <QHash>
#include <QString>
#include <QThread>

class CThreadRegistry
{
public:
   typedef struct
   {
      QThread* thread;
   } CThreadManager;
   CThreadRegistry() {};
   static QThread* getThread(const QString& name);
   static void     addThread(const QString& name,
                             QThread* thread);
   static void     removeThread(const QString& name);
private:
   static QHash<QString,CThreadManager*> threads;
};

#endif // CTHREADREGISTRY_H
