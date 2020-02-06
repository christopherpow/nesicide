#ifndef DEBUGGERUPDATETHREAD_H
#define DEBUGGERUPDATETHREAD_H

#include <QThread>
#include <QMutex>

class DebuggerUpdateThread : public QObject
{
   Q_OBJECT
public:
   explicit DebuggerUpdateThread(void (*func)(),QObject *parent = 0);
   ~DebuggerUpdateThread();

   void changeFunction(void (*func)()) { _func = func; }

signals:
   void updateComplete();

public slots:
   void updateDebuggers();

private:
   void (*_func)();
   static QThread *pThread;
   static QMutex *pMutex;
   static int resourceCount;
};

#endif // DEBUGGERUPDATETHREAD_H
