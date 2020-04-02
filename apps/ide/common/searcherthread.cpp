#include "searcherthread.h"

#include "cbuildertextlogger.h"
#include "cnesicideproject.h"

SearcherWorker::SearcherWorker(QObject*)
{
   m_found = 0;
}

SearcherWorker::~SearcherWorker()
{
}

void SearcherWorker::search(QDir dir, QString searchText, QString pattern, bool subfolders, bool sourceSearchPaths, bool useRegex, bool caseSensitive)
{
   m_dir = dir;
   m_searchText = searchText;
   m_pattern = pattern;
   m_subfolders = subfolders;
   m_sourceSearchPaths = sourceSearchPaths;
   m_useRegex = useRegex;
   m_caseSensitive = caseSensitive;

   m_found = 0;
   doSearch(m_dir,&m_found);
   if ( m_sourceSearchPaths )
   {
      foreach ( QString searchPath, CNesicideProject::instance()->getSourceSearchPaths() )
      {
         m_dir = searchPath;
         doSearch(m_dir,&m_found);
      }
   }
   emit searchDone(m_found);
}

void SearcherWorker::doSearch(QDir dir,int* finds)
{
   QDir          base(QDir::currentPath());
   QFileInfoList entries = dir.entryInfoList(QDir::AllDirs|QDir::NoDotAndDotDot|QDir::NoSymLinks|QDir::Files);
   QFile         file;
   QString       content;
   QStringList   contentLines;
   QString       foundText;
   bool          found;
   int           entry;
   int           line;
   Qt::CaseSensitivity caseSensitivity = (m_caseSensitive)?Qt::CaseSensitive:Qt::CaseInsensitive;
   QRegExp       regex;

   regex = QRegExp(m_searchText);
   regex.setCaseSensitivity(caseSensitivity);

   for ( entry = 0; entry < entries.count(); entry++ )
   {
      if ( (m_subfolders) && (entries.at(entry).isDir()) )
      {
         doSearch(QDir(entries.at(entry).filePath()),finds);
      }
      else if ( entries.at(entry).isFile() )
      {
         file.setFileName(entries.at(entry).filePath());
         if ( file.open(QIODevice::ReadOnly) )
         {
            content.clear();
            content = file.readAll();
            contentLines.clear();
            contentLines = content.split(QRegExp("[\n]"));

            for ( line = 0; line < contentLines.count(); line++ )
            {
               if ( m_useRegex )
               {
                  found = contentLines.at(line).contains(regex);
               }
               else
               {
                  found = contentLines.at(line).contains(m_searchText,caseSensitivity);
               }
               if ( found )
               {
                  foundText.sprintf("%s:%d:%s",base.relativeFilePath(entries.at(entry).filePath()).toLatin1().constData(),line+1,contentLines.at(line).toLatin1().constData());
                  searchTextLogger->write(foundText);
                  (*finds)++;
               }
            }
            file.close();
         }
      }
   }
}

SearcherThread::SearcherThread(QObject*)
{
   pWorker = new SearcherWorker();

   QObject::connect(pWorker,SIGNAL(searchDone(int)),this,SIGNAL(searchDone(int)));

   pThread = new QThread();

   pWorker->moveToThread(pThread);

   pThread->start();
}

SearcherThread::~SearcherThread()
{
   pThread->exit(0);
   pThread->wait();
   delete pThread;
   delete pWorker;
}
