#ifndef DEBUGGERUPDATETHREAD_H
#define DEBUGGERUPDATETHREAD_H

#include <QThread>

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
   QThread* pThread;
};

#endif // DEBUGGERUPDATETHREAD_H
