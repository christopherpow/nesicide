#ifndef DEBUGGERUPDATETHREAD_H
#define DEBUGGERUPDATETHREAD_H

#include <QThread>

class DebuggerUpdateThread : public QThread
{
   Q_OBJECT
public:
   explicit DebuggerUpdateThread(void (*func)(),QObject *parent = 0);

   virtual void run();

   void changeFunction(void (*func)()) { _func = func; }

signals:
   void updateComplete();

public slots:
   void updateDebuggers();

private:
   void (*_func)();
};

#endif // DEBUGGERUPDATETHREAD_H
