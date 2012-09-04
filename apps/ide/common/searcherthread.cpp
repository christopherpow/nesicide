#include "searcherthread.h"

#include "main.h"

SearcherThread::SearcherThread(QObject*)
{
   m_found = 0;

   pThread = new QThread();

   moveToThread(pThread);

   pThread->start();
}

SearcherThread::~SearcherThread()
{
   pThread->terminate();
   pThread->wait();
   delete pThread;
}

void SearcherThread::search(QDir dir, QString searchText, QString pattern, bool subfolders, bool sourceSearchPaths, bool useRegex, bool caseSensitive)
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
      foreach ( QString searchPath, nesicideProject->getSourceSearchPaths() )
      {
         m_dir = searchPath;
         doSearch(m_dir,&m_found);
      }
   }
   emit searchDone(m_found);
}

void SearcherThread::doSearch(QDir dir,int* finds)
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
                  foundText.sprintf("%s:%d:%s",base.relativeFilePath(entries.at(entry).filePath()).toAscii().constData(),line+1,contentLines.at(line).toAscii().constData());
                  searchTextLogger->write(foundText);
                  (*finds)++;
               }
            }
            file.close();
         }
      }
   }
}
