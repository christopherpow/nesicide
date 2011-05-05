#include "ccc65interface.h"

#include "cnesicideproject.h"
#include "iprojecttreeviewitem.h"

#include "main.h"

cc65_dbginfo        CCC65Interface::dbgInfo = NULL;
cc65_sourceinfo*    CCC65Interface::dbgSources = NULL;
cc65_segmentinfo*   CCC65Interface::dbgSegments = NULL;
cc65_lineinfo*      CCC65Interface::dbgLines = NULL;
cc65_symbolinfo*    CCC65Interface::dbgSymbols = NULL;

QStringList         CCC65Interface::files;


CCC65Interface::CCC65Interface()
{
}

CCC65Interface::~CCC65Interface()
{
   cc65_free_sourceinfo(dbgInfo,dbgSources);
   cc65_free_segmentinfo(dbgInfo,dbgSegments);
   cc65_free_lineinfo(dbgInfo,dbgLines);
   cc65_free_symbolinfo(dbgInfo,dbgSymbols);
   cc65_free_dbginfo(dbgInfo);
}

bool CCC65Interface::assemble()
{
   IProjectTreeViewItemIterator iter(nesicideProject->getProject()->getSources());
   CSourceItem*                 source;
   QProcess                     cc65;
   QProcessEnvironment          env = QProcessEnvironment::systemEnvironment();
   QString                      invocationStr;
   QString                      stdioStr;
   QStringList                  stdioList;
   QStringList                  objList;
   QFileInfo                    fileInfo;
   QDir                         sourceDir(nesicideProject->getProjectSourceBasePath());
   QDir                         outputDir(nesicideProject->getProjectOutputBasePath());
   QDir                         linkerConfigDir(nesicideProject->getLinkerConfigFile());
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
         invocationStr = "ca65.exe -V -g -v --debug-info ";
         invocationStr += nesicideProject->getCompilerAdditionalOptions();
         invocationStr += " ";
         invocationStr += nesicideProject->getCompilerDefinedSymbols();
         invocationStr += " ";
         invocationStr += nesicideProject->getCompilerIncludePaths();
         invocationStr += " ";
         invocationStr += "\""+sourceDir.toNativeSeparators(sourceDir.absoluteFilePath(source->path()))+"\"";
         invocationStr += " -o ";
         fileInfo.setFile(source->path());
         objList << "\""+sourceDir.toNativeSeparators(outputDir.absoluteFilePath(fileInfo.completeBaseName()+".o"))+"\"";
         invocationStr += objList.last();

         buildTextLogger->write(invocationStr);

         cc65.start(invocationStr);
         cc65.waitForFinished();
         cc65.waitForReadyRead();
         exitCode = cc65.exitCode();
         stdioStr = QString(cc65.readAllStandardOutput());
         stdioList = stdioStr.split(QRegExp("[\r\n]"),QString::SkipEmptyParts);
         foreach ( const QString& str, stdioList )
         {
            buildTextLogger->write("<font color='blue'>&nbsp;&nbsp;&nbsp;"+str+"</font>");
         }
         stdioStr = QString(cc65.readAllStandardError());
         stdioList = stdioStr.split(QRegExp("[\r\n]"),QString::SkipEmptyParts);
         foreach ( const QString& str, stdioList )
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
   invocationStr += " -V -o ";
   invocationStr += "\""+outputDir.toNativeSeparators(outputDir.absoluteFilePath(nesicideProject->getProjectOutputName()+".prg"))+"\"";
   invocationStr += " -v";
   if ( !(nesicideProject->getLinkerConfigFile().isEmpty()) )
   {
      invocationStr += " -C ";
      invocationStr += "\""+QDir::toNativeSeparators(nesicideProject->getLinkerConfigFile())+"\"";
   }
   invocationStr += " --dbgfile ";
   invocationStr += "\""+outputDir.toNativeSeparators(outputDir.absoluteFilePath(nesicideProject->getProjectOutputName()+".dbg"))+"\"";
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
   stdioStr = QString(cc65.readAllStandardOutput());
   stdioList = stdioStr.split(QRegExp("[\r\n]"),QString::SkipEmptyParts);
   foreach ( const QString& str, stdioList )
   {
      buildTextLogger->write("<font color='blue'>&nbsp;&nbsp;&nbsp;"+str+"</font>");
   }
   stdioStr = QString(cc65.readAllStandardError());
   stdioList = stdioStr.split(QRegExp("[\r\n]"),QString::SkipEmptyParts);
   foreach ( const QString& str, stdioList )
   {
      buildTextLogger->write("<font color='red'>&nbsp;&nbsp;&nbsp;"+str+"</font>");
   }
   if ( exitCode )
   {
      buildTextLogger->write("<font color='red'>ld65.exe: exited with code "+QString::number(exitCode)+"</font>");
      ok = false;
   }

   if ( ok )
   {
      ok = captureDebugInfo();
      if ( ok )
      {
         ok = captureINESImage();
      }
   }

   return ok;
}

static void ErrorFunc (const struct cc65_parseerror* E)
/* Handle errors or warnings that occur while parsing a debug file */
{
   char errorBuffer[256];
   sprintf(errorBuffer,
           "%s:%s(%lu): %s\n",
           E->type? "Error" : "Warning",
           E->name,
           (unsigned long) E->line,
           E->errormsg);
   buildTextLogger->write(errorBuffer);
}

bool CCC65Interface::captureDebugInfo()
{
   QDir dir(nesicideProject->getProjectOutputBasePath());
   QString dbgInfoFile = dir.toNativeSeparators(dir.absoluteFilePath(nesicideProject->getProjectOutputName()+".dbg"));
   int idx;

   buildTextLogger->write("<font color='black'><b>Reading debug information from: "+dbgInfoFile+"</b></font>");

   dbgInfo = cc65_read_dbginfo(dbgInfoFile.toAscii().constData(), ErrorFunc);
   if (dbgInfo == 0)
   {
      return false;
   }

   dbgSources = cc65_get_sourcelist(dbgInfo);
   dbgSegments = cc65_get_segmentlist(dbgInfo);

   files.clear();
   for ( idx = 0; idx < dbgSources->count; idx++ )
   {
      files.append(dbgSources->data[idx].source_name);
   }

   return true;
}

bool CCC65Interface::captureINESImage()
{
   QDir dir(nesicideProject->getProjectOutputBasePath());
   QString inesFile = dir.toNativeSeparators(dir.absoluteFilePath(nesicideProject->getProjectOutputName()+".prg"));
   QFile ines(inesFile);
   CPRGROMBanks* prgRomBanks = nesicideProject->getCartridge()->getPrgRomBanks();
   QByteArray romData;
   int romLength = 0;
   int romOffset = 0;
   int oldBanks = prgRomBanks->getPrgRomBanks().count();
   int bankIdx = 0;

   buildTextLogger->write("<font color='black'><b>Reading NES executable from: "+inesFile+"</b></font>");

   return nesicideProject->createProjectFromRom(inesFile);
}

QStringList& CCC65Interface::getSourceFiles()
{
   return files;
}

QStringList& CCC65Interface::getSymbolsForSourceFile(QString& sourceFile)
{

}

QString CCC65Interface::getSourceFileFromAbsoluteAddress(uint32_t addr,uint32_t absAddr)
{
   int  line;

   dbgLines = cc65_lineinfo_byaddr(dbgInfo,addr);

   for ( line = 0; line < dbgLines->count; line++ )
   {
      if ( (dbgLines->count == 1) || (dbgLines->data[line].output_offs-0x10 == absAddr) )
      {
         return dbgLines->data[line].source_name;
      }
   }
   return "";
}

int CCC65Interface::getSourceLineFromAbsoluteAddress(uint32_t addr,uint32_t absAddr)
{
   int line;

   dbgLines = cc65_lineinfo_byaddr(dbgInfo,addr);

   for ( line = 0; line < dbgLines->count; line++ )
   {
      if ( (dbgLines->count == 1) || (dbgLines->data[line].output_offs-0x10 == absAddr) )
      {
         return dbgLines->data[line].source_line;
      }
   }
   return -1;
}
