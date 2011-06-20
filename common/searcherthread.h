#ifndef SEARCHERTHREAD_H
#define SEARCHERTHREAD_H

#include <QThread>
#include <QDir>

class SearcherThread : public QThread
{
   Q_OBJECT
public:
   SearcherThread ( QObject* parent = 0 );
   virtual ~SearcherThread ();
   void kill();

   void search(QDir dir, QString searchText, QString pattern, bool useRegex, bool caseSensitive);

signals:
   void searchDone(int found);

protected:
   virtual void run ();
   void doSearch(QDir dir,QString searchText,QString pattern,bool useRegex,bool caseSensitive,int* finds);
   bool m_isTerminating;
   QDir m_dir;
   QString m_searchText;
   QString m_pattern;
   bool m_useRegex;
   bool m_caseSensitive;
   int m_found;
};

#endif // SEARCHERTHREAD_H
