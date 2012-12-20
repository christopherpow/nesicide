#ifndef COMPILERTHREAD_H
#define COMPILERTHREAD_H

#include <QThread>
#include <QSemaphore>

enum
{
   DoCompile,
   DoClean
};

class CompilerThread : public QObject
{
   Q_OBJECT
public:
   CompilerThread ( QObject* parent = 0 );
   virtual ~CompilerThread ();

   bool assembledOk() { return m_assembledOk; }
   void reset() { m_assembledOk = false; }

public slots:
   void compile();
   void clean();

signals:
   void compileStarted();
   void compileDone(bool bOk);
   void cleanStarted();
   void cleanDone(bool bOk);

protected:
   bool m_assembledOk;
   int  m_operation;

   QThread* pThread;
};

#endif // COMPILERTHREAD_H
