#ifndef BREAKPOINTWATCHERTHREAD_H
#define BREAKPOINTWATCHERTHREAD_H

#include <QThread>
#include <QSemaphore>

class BreakpointWatcherThread : public QThread
{
   Q_OBJECT
public:
   BreakpointWatcherThread ( QObject* parent = 0 );
   virtual ~BreakpointWatcherThread ();
   void kill();

   QSemaphore* breakpointWatcherSemaphore() { return semaphore; }

signals:
   void breakpointHit();
   void showPane(int pane);

protected:
   QSemaphore* semaphore;

   virtual void run ();
   bool m_isTerminating;
};

#endif // BREAKPOINTWATCHERTHREAD_H
