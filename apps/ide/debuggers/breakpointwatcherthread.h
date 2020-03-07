#ifndef BREAKPOINTWATCHERTHREAD_H
#define BREAKPOINTWATCHERTHREAD_H

#include <QThread>
#include <QSemaphore>

class BreakpointWatcherWorker : public QObject
{
   Q_OBJECT
public:
   BreakpointWatcherWorker ( QObject* parent = 0 );
   virtual ~BreakpointWatcherWorker ();

   void breakpoint();

signals:
   void breakpointHit();
   void showPane(int pane);
};

class BreakpointWatcherThread : public QObject
{
   Q_OBJECT
public:
   BreakpointWatcherThread ( QObject* parent = 0 );
   virtual ~BreakpointWatcherThread ();

signals:
   void breakpointHit();
   void showPane(int pane);

public slots:
   void breakpoint() { pWorker->breakpoint(); }

protected:
   BreakpointWatcherWorker *pWorker;
   QThread                 *pThread;
};

#endif // BREAKPOINTWATCHERTHREAD_H
