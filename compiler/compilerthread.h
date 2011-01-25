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
   void reset() { m_assembledOk = false; }
   void assemble();

signals:
   void compileStarted();
   void compileStep(char* message);
   void compileDone(bool bOk);

protected:
   virtual void run ();
   bool m_isTerminating;
   bool m_assembledOk;
};

#endif // COMPILERTHREAD_H
