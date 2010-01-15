#ifndef BREAKPOINTWATCHERTHREAD_H
#define BREAKPOINTWATCHERTHREAD_H

#include <QThread>
#include <QDialog>

class BreakpointWatcherThread : public QThread
{
   Q_OBJECT
public:
   BreakpointWatcherThread ( QObject* parent = 0 );
   virtual ~BreakpointWatcherThread () {}

   void setDialog(QDialog* dialog);

signals:
   void breakpointHit();

protected:
   virtual void run ();
};

#endif // BREAKPOINTWATCHERTHREAD_H
