#ifndef COMPILERTHREAD_H
#define COMPILERTHREAD_H

#include <QThread>

class CompilerThread : public QThread
{
   Q_OBJECT
public:
   CompilerThread ( QObject* parent = 0 );
   virtual ~CompilerThread ();
   void kill();

signals:
   void compileStep(char* message);

protected:
   virtual void run ();
   bool m_isTerminating;
};

#endif // COMPILERTHREAD_H
