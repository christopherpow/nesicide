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
   
   bool assembledOk() { return m_assembledOk; }

signals:
   void compileStep(char* message);

protected:
   virtual void run ();
   bool m_isTerminating;
   bool m_assembledOk;
};

#endif // COMPILERTHREAD_H
