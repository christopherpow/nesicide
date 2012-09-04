#ifndef BREAKPOINTWATCHERTHREAD_H
#define BREAKPOINTWATCHERTHREAD_H

#include <QThread>
#include <QSemaphore>

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
   void breakpoint();

protected:
   QThread* pThread;
};

#endif // BREAKPOINTWATCHERTHREAD_H
