#ifndef COMPILERTHREAD_H
#define COMPILERTHREAD_H

#include <QThread>
#include <QSemaphore>

enum
{
   DoCompile,
   DoClean
};

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
   void clean();

signals:
   void compileStarted();
   void compileStep(char* message);
   void compileDone(bool bOk);

protected:
   QSemaphore* compileSemaphore;

   virtual void run ();
   bool m_isTerminating;
   bool m_assembledOk;
   int  m_operation;
};

#endif // COMPILERTHREAD_H
