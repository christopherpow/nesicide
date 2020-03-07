#ifndef SEARCHERTHREAD_H
#define SEARCHERTHREAD_H

#include <QThread>
#include <QDir>
#include <QSemaphore>

class SearcherWorker : public QObject
{
   Q_OBJECT
public:
   SearcherWorker ( QObject* parent = 0 );
   virtual ~SearcherWorker ();

   void search(QDir dir, QString searchText, QString pattern, bool subfolders, bool sourceSearchPaths, bool useRegex, bool caseSensitive);

signals:
   void searchDone(int found);

protected:
   void doSearch(QDir dir,int* finds);
   bool m_isTerminating;
   QDir m_dir;
   QString m_searchText;
   QString m_pattern;
   bool m_subfolders;
   bool m_sourceSearchPaths;
   bool m_useRegex;
   bool m_caseSensitive;
   int m_found;
};

class SearcherThread : public QObject
{
   Q_OBJECT
public:
   SearcherThread ( QObject* parent = 0 );
   virtual ~SearcherThread ();

public slots:
   void search(QDir dir, QString searchText, QString pattern, bool subfolders, bool sourceSearchPaths, bool useRegex, bool caseSensitive)
   {
      pWorker->search(dir,searchText,pattern,subfolders,sourceSearchPaths,useRegex,caseSensitive);
   }

signals:
   void searchDone(int found);

protected:
   SearcherWorker *pWorker;
   QThread        *pThread;
};

#endif // SEARCHERTHREAD_H
