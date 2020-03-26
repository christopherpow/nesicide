#ifndef DEBUGGERUPDATETHREAD_H
#define DEBUGGERUPDATETHREAD_H

#include <QThread>
#include <QMutex>

class DebuggerUpdateWorker : public QObject
{
   Q_OBJECT
public:
   explicit DebuggerUpdateWorker(void (*func)(),QObject *parent = 0);
   ~DebuggerUpdateWorker();

   void changeFunction(void (*func)()) { _func = func; }
   void updateDebuggers();

signals:
   void updateComplete();

private:
   void        (*_func)();
};

class DebuggerUpdateThread : public QObject
{
   Q_OBJECT
public:
   explicit DebuggerUpdateThread(void (*func)(),QObject *parent = 0);
   ~DebuggerUpdateThread();

   void changeFunction(void (*func)()) { pWorker->changeFunction(func); }

   static void silence(bool silence) { silenced = silence; }
   static bool isSilenced() { return silenced; }

public slots:
   void updateDebuggers();

signals:
   void updateComplete();

private:
   DebuggerUpdateWorker *pWorker;
   static QThread       *pThread;
   static QMutex        *pMutex;
   static int            resourceCount;
   static bool           silenced;
};

#endif // DEBUGGERUPDATETHREAD_H
