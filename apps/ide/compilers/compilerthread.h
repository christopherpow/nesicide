#ifndef COMPILERTHREAD_H
#define COMPILERTHREAD_H

#include <QThread>
#include <QSemaphore>

enum
{
   DoCompile,
   DoClean
};

class CompilerWorker : public QObject
{
   Q_OBJECT
public:
   CompilerWorker ( QObject* parent = 0 );
   virtual ~CompilerWorker ();

   bool assembledOk() { return m_assembledOk; }
   void reset() { m_assembledOk = false; }

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
};

class CompilerThread : public QObject
{
   Q_OBJECT
public:
   CompilerThread ( QObject* parent = 0 );
   virtual ~CompilerThread ();

   bool assembledOk() { return pWorker->assembledOk(); }
   void reset() { pWorker->reset(); }

public slots:
   void compile() { pWorker->compile(); }
   void clean() { pWorker->clean(); }

signals:
   void compileStarted();
   void compileDone(bool bOk);
   void cleanStarted();
   void cleanDone(bool bOk);

protected:
   CompilerWorker *pWorker;
   QThread        *pThread;
};

#endif // COMPILERTHREAD_H
