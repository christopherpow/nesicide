#include "ccc65interface.h"

#include "cnesicideproject.h"
#include "iprojecttreeviewitem.h"

#include "main.h"

cc65_dbginfo        CCC65Interface::dbgInfo = NULL;
cc65_sourceinfo*    CCC65Interface::dbgSources = NULL;
cc65_segmentinfo*   CCC65Interface::dbgSegments = NULL;
cc65_lineinfo*      CCC65Interface::dbgLines = NULL;
cc65_symbolinfo*    CCC65Interface::dbgSymbols = NULL;

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
   cc65.setWorkingDirectory(QDir::currentPath());

   // For each source code object, compile it.
   while ( iter.current() )
   {
      source = dynamic_cast<CSourceItem*>(iter.current());
      if ( source )
      {
         invocationStr = "ca65 -V -g -v --debug-info ";
         invocationStr += nesicideProject->getCompilerAdditionalOptions();
         invocationStr += " ";
         invocationStr += nesicideProject->getCompilerDefinedSymbols();
         invocationStr += " ";
         invocationStr += nesicideProject->getCompilerIncludePaths();
         invocationStr += " ";
         invocationStr += "\""+sourceDir.toNativeSeparators(sourceDir.filePath(source->path()))+"\"";
         invocationStr += " -o ";
         fileInfo.setFile(source->path());
         objList << "\""+outputDir.toNativeSeparators(outputDir.filePath(fileInfo.completeBaseName()+".o"))+"\"";
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
            buildTextLogger->write("<font color='red'>ca65: exited with code "+QString::number(exitCode)+"</font>");
            ok = false;
         }
      }
      iter.next();
   }
   invocationStr = "ld65 ";
   invocationStr += nesicideProject->getCompilerDefinedSymbols();
   invocationStr += nesicideProject->getLinkerAdditionalOptions();
   invocationStr += " ";
   invocationStr += " -V -o ";
   if ( nesicideProject->getProjectLinkerOutputName().isEmpty() )
   {
      invocationStr += "\""+outputDir.toNativeSeparators(outputDir.filePath(nesicideProject->getProjectOutputName()+".prg"))+"\"";
   }
   else
   {
      invocationStr += "\""+outputDir.toNativeSeparators(outputDir.filePath(nesicideProject->getProjectLinkerOutputName()))+"\"";
   }
   if ( !(nesicideProject->getLinkerConfigFile().isEmpty()) )
   {
      invocationStr += " -C ";
      invocationStr += "\""+QDir::toNativeSeparators(nesicideProject->getLinkerConfigFile())+"\"";
   }
   invocationStr += " --dbgfile ";
   if ( nesicideProject->getProjectDebugInfoName().isEmpty() )
   {
      invocationStr += "\""+outputDir.toNativeSeparators(outputDir.filePath(nesicideProject->getProjectOutputName()+".dbg"))+"\"";
   }
   else
   {
      invocationStr += "\""+outputDir.toNativeSeparators(outputDir.filePath(nesicideProject->getProjectDebugInfoName()))+"\"";
   }
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
      buildTextLogger->write("<font color='red'>ld65: exited with code "+QString::number(exitCode)+"</font>");
      ok = false;
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
   QString dbgInfoFile;

   if ( nesicideProject->getProjectDebugInfoName().isEmpty() )
   {
      dbgInfoFile = dir.toNativeSeparators(dir.filePath(nesicideProject->getProjectOutputName()+".dbg"));
   }
   else
   {
      dbgInfoFile = dir.toNativeSeparators(dir.filePath(nesicideProject->getProjectDebugInfoName()));
   }
   buildTextLogger->write("<font color='black'><b>Reading debug information from: "+dbgInfoFile+"</b></font>");

   dbgInfo = cc65_read_dbginfo(dbgInfoFile.toAscii().constData(), ErrorFunc);
   if (dbgInfo == 0)
   {
      return false;
   }

   return true;
}

bool CCC65Interface::captureINESImage()
{
   QDir outputDir(nesicideProject->getProjectOutputBasePath());
   QString nesName;

   if ( nesicideProject->getProjectCartridgeOutputName().isEmpty() )
   {
      nesName = outputDir.toNativeSeparators(outputDir.filePath(nesicideProject->getProjectOutputName()+".nes"));
   }
   else
   {
      nesName = outputDir.toNativeSeparators(outputDir.filePath(nesicideProject->getProjectCartridgeOutputName()));
   }

   buildTextLogger->write("<font color='black'><b>Reading NES executable from: "+nesName+"</b></font>");

   return nesicideProject->createProjectFromRom(nesName);
}

QStringList CCC65Interface::getSourceFiles()
{
   QStringList files;
   int file;

   if ( dbgInfo )
   {
      dbgSources = cc65_get_sourcelist(dbgInfo);

      if ( dbgSources )
      {
         for ( file = 0; file < dbgSources->count; file++ )
         {
            files.append(dbgSources->data[file].source_name);
         }
      }
   }
   return files;
}

QStringList CCC65Interface::getSymbolsForSourceFile(QString sourceFile)
{
   QStringList symbols;
   int symbol;

   if ( dbgInfo )
   {
      dbgSymbols = cc65_symbol_inrange(dbgInfo,0,0xFFFF);

      if ( dbgSymbols )
      {
         for ( symbol = 0; symbol < dbgSymbols->count; symbol++ )
         {
            symbols.append(dbgSymbols->data[symbol].symbol_name);
         }
      }
   }
   return symbols;
}

QString CCC65Interface::getSourceFileFromAbsoluteAddress(uint32_t addr,uint32_t absAddr)
{
   int  line;

   if ( dbgInfo )
   {
      dbgLines = cc65_lineinfo_byaddr(dbgInfo,addr);

      if ( dbgLines )
      {
         for ( line = 0; line < dbgLines->count; line++ )
         {
            if ( (dbgLines->count == 1) || (dbgLines->data[line].output_offs-0x10 == absAddr) )
            {
               return dbgLines->data[line].source_name;
            }
         }
      }
   }
   return "";
}

int CCC65Interface::getSourceLineFromAbsoluteAddress(uint32_t addr,uint32_t absAddr)
{
   int line;

   if ( dbgInfo )
   {
      dbgLines = cc65_lineinfo_byaddr(dbgInfo,addr);

      if ( dbgLines )
      {
         for ( line = 0; line < dbgLines->count; line++ )
         {
            if ( (dbgLines->count == 1) || (dbgLines->data[line].output_offs-0x10 == absAddr) )
            {
               return dbgLines->data[line].source_line;
            }
         }
      }
   }
   return -1;
}

unsigned int CCC65Interface::getAddressFromFileAndLine(QString file,int line)
{
   if ( dbgInfo )
   {
      dbgLines = cc65_lineinfo_byname(dbgInfo,file.toAscii().constData(),line);

      if ( dbgLines )
      {
         for ( line = 0; line < dbgLines->count; line++ )
         {
            if ( (dbgLines->count == 1) || (dbgLines->data[line].source_line == line) )
            {
               return dbgLines->data[line].line_start;
            }
         }
      }
   }
   return -1;
}

unsigned int CCC65Interface::getAbsoluteAddressFromFileAndLine(QString file,int line)
{
   if ( dbgInfo )
   {
      dbgLines = cc65_lineinfo_byname(dbgInfo,file.toAscii().constData(),line);

      if ( dbgLines )
      {
         for ( line = 0; line < dbgLines->count; line++ )
         {
            if ( (dbgLines->count == 1) || (dbgLines->data[line].source_line == line) )
            {
               return dbgLines->data[line].output_offs-0x10;
            }
         }
      }
   }
   return -1;
}

bool CCC65Interface::isAbsoluteAddressAnOpcode(uint32_t absAddr)
{
   uint32_t addr;
   int      line;

   if ( dbgInfo )
   {
      // Make addresses for where code might be in PRG-ROM space.
      addr = (absAddr&MASK_8KB)+MEM_32KB;
      for ( ; addr < MEM_64KB; addr += MEM_8KB )
      {
         dbgLines = cc65_lineinfo_byaddr(dbgInfo,addr);

         if ( dbgLines )
         {
            for ( line = 0; line < dbgLines->count; line++ )
            {
               if ( (dbgLines->data[line].line_start == addr) &&
                    (dbgLines->data[line].output_offs-0x10 == absAddr) )
               {
                  return true;
               }
            }
         }
      }
   }
   return false;
}
