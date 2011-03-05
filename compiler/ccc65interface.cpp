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
   IProjectTreeViewItem*        item;
   QProcess    cc65;
   QString     invocationStr;
   QString     stderrStr;
   QStringList stderrList;
   int         exitCode;
   bool        ok = true;
   
   while ( iter.current() )
   {
      item = dynamic_cast<CSourceItem*>(iter.current());
      if ( item )
      {
         invocationStr = "ca65.exe ";
         invocationStr += nesicideProject->getCompilerDefinedSymbols();
         invocationStr += " ";
         invocationStr += nesicideProject->getCompilerUndefinedSymbols();
         invocationStr += " ";
         invocationStr += nesicideProject->getCompilerIncludePaths();
         invocationStr += " ";
         invocationStr += nesicideProject->getProjectSourceBasePath();
         invocationStr += "/";
         invocationStr += item->caption();
         invocationStr += " -o ";
         invocationStr += nesicideProject->getProjectOutputBasePath();
         invocationStr += "/";
         invocationStr += item->caption();
//  CPTODO       add source base name to source item so that it can be used to generate .s or .o postfix for this
         
         buildTextLogger->write(invocationStr);
         cc65.start(invocationStr); 
         cc65.waitForFinished();
         cc65.waitForReadyRead();
         exitCode = cc65.exitCode();
         stderrStr = QString(cc65.readAllStandardError());
         stderrList = stderrStr.split(QRegExp("[\r\n]"),QString::SkipEmptyParts);
         foreach ( const QString str, stderrList )
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
   
   return ok;
}
