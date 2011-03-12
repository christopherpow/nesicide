#include "ccc65interface.h"

#include "cnesicideproject.h"
#include "iprojecttreeviewitem.h"

#include "main.h"

CCC65Interface::CCC65Interface()
{
}

CCC65Interface::~CCC65Interface()
{
}

bool CCC65Interface::assemble()
{
   IProjectTreeViewItemIterator iter(nesicideProject->getProject()->getSources());
   CSourceItem*                 source;
   QProcess                     cc65;
   QProcessEnvironment          env = QProcessEnvironment::systemEnvironment();
   QString                      invocationStr;
   QString                      stderrStr;
   QStringList                  stderrList;
   QStringList                  objList;
   QFileInfo                    fileInfo;
   int                          exitCode;
   bool                         ok = true;

   // Copy the system environment to the child process.
   cc65.setProcessEnvironment(env);

   // For each source code object, compile it.
   while ( iter.current() )
   {
      source = dynamic_cast<CSourceItem*>(iter.current());
      if ( source )
      {
         invocationStr = "ca65.exe ";
         invocationStr += nesicideProject->getCompilerDefinedSymbols();
         invocationStr += " ";
         invocationStr += nesicideProject->getCompilerUndefinedSymbols();
         invocationStr += " ";
         invocationStr += nesicideProject->getCompilerIncludePaths();
         invocationStr += " ";
         invocationStr += source->path();
         invocationStr += " -g -v --debug-info -o ";
         fileInfo.setFile(source->path());
         objList << nesicideProject->getProjectOutputBasePath()+"/"+fileInfo.completeBaseName()+".o";
         invocationStr += objList.last();

         buildTextLogger->write(invocationStr);

         cc65.start(invocationStr);
         cc65.waitForFinished();
         cc65.waitForReadyRead();
         exitCode = cc65.exitCode();
         stderrStr = QString(cc65.readAllStandardError());
         stderrList = stderrStr.split(QRegExp("[\r\n]"),QString::SkipEmptyParts);
         foreach ( const QString& str, stderrList )
         {
            buildTextLogger->write("<font color='red'>&nbsp;&nbsp;&nbsp;"+str+"</font>");
         }
         if ( exitCode )
         {
            buildTextLogger->write("<font color='red'>ca65.exe: exited with code "+QString::number(exitCode)+"</font>");
            ok = false;
         }
      }
      iter.next();
   }
   invocationStr = "ld65.exe ";
   invocationStr += nesicideProject->getCompilerDefinedSymbols();
   invocationStr += nesicideProject->getCompilerUndefinedSymbols();
   invocationStr += " -o ";
   invocationStr += nesicideProject->getProjectOutputBasePath()+"/"+nesicideProject->getProjectOutputName()+".prg";
   invocationStr += " -v -C C:/CSP/Projects/NES/Homebrew/russian_roulette/nes.ini";
   invocationStr += " --dbgfile ";
   invocationStr += nesicideProject->getProjectOutputBasePath()+"/"+nesicideProject->getProjectOutputName()+".dbg";
   foreach ( const QString& str, objList )
   {
      invocationStr += " ";
      invocationStr += str;
   }

   buildTextLogger->write(invocationStr);

   cc65.start(invocationStr);
   cc65.waitForFinished();
   cc65.waitForReadyRead();
   exitCode = cc65.exitCode();
   stderrStr = QString(cc65.readAllStandardError());
   stderrList = stderrStr.split(QRegExp("[\r\n]"),QString::SkipEmptyParts);
   foreach ( const QString& str, stderrList )
   {
      buildTextLogger->write("<font color='red'>&nbsp;&nbsp;&nbsp;"+str+"</font>");
   }
   if ( exitCode )
   {
      buildTextLogger->write("<font color='red'>ld65.exe: exited with code "+QString::number(exitCode)+"</font>");
      ok = false;
   }

   return ok;
}
