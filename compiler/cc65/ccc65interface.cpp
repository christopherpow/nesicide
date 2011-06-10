#include "ccc65interface.h"

#include "cnesicideproject.h"
#include "iprojecttreeviewitem.h"

#include "main.h"

cc65_dbginfo        CCC65Interface::dbgInfo = NULL;
cc65_sourceinfo*    CCC65Interface::dbgSources = NULL;
cc65_segmentinfo*   CCC65Interface::dbgSegments = NULL;
cc65_lineinfo*      CCC65Interface::dbgLines = NULL;
cc65_symbolinfo*    CCC65Interface::dbgSymbols = NULL;
QStringList         CCC65Interface::errors;

CCC65Interface::CCC65Interface()
{
}

CCC65Interface::~CCC65Interface()
{
   clear();
}

void CCC65Interface::clear()
{
   if ( dbgInfo )
   {
      if ( dbgSources )
      {
         cc65_free_sourceinfo(dbgInfo,dbgSources);
         dbgSources = 0;
      }
      if (  dbgSegments )
      {
         cc65_free_segmentinfo(dbgInfo,dbgSegments);
         dbgSegments = 0;
      }
      if ( dbgLines )
      {
         cc65_free_lineinfo(dbgInfo,dbgLines);
         dbgLines = 0;
      }
      if ( dbgSymbols )
      {
         cc65_free_symbolinfo(dbgInfo,dbgSymbols);
         dbgSymbols = 0;
      }
      cc65_free_dbginfo(dbgInfo);
      dbgInfo = 0;
   }
}

QStringList CCC65Interface::getSourcesFromProject()
{
   IProjectTreeViewItemIterator iter(nesicideProject->getProject()->getSources());
   QDir                         baseDir(QDir::currentPath());
   CSourceItem*                 source;
   QStringList                  sources;

   // For each source code object, compile it.
   while ( iter.current() )
   {
      source = dynamic_cast<CSourceItem*>(iter.current());
      if ( source )
      {
         sources.append(baseDir.fromNativeSeparators(baseDir.relativeFilePath(source->path())));
      }
      iter.next();
   }

   return sources;
}

bool CCC65Interface::createMakefile()
{
   QDir outputDir(QDir::currentPath());
   QString outputName = outputDir.fromNativeSeparators(outputDir.filePath("Makefile"));
   QFile res(":Makefile");
   QFile makeFile(outputName);

   // Get the embedded universal makefile...
   res.open(QIODevice::ReadOnly);

   // Create the project's makefile...
   makeFile.open(QIODevice::ReadWrite|QIODevice::Truncate);

   if ( res.isOpen() && makeFile.isOpen() )
   {
      QString makeFileContent;

      buildTextLogger->write("<b>Creating: "+outputName+"</b>");

      // Read the embedded Makefile resource.
      makeFileContent = res.readAll();

      // Replace stuff that needs to be replaced...
      makeFileContent.replace("<!project-name!>",nesicideProject->getProjectOutputName());
      makeFileContent.replace("<!prg-rom-name!>",nesicideProject->getProjectLinkerOutputName());
      makeFileContent.replace("<!linker-config!>",nesicideProject->getLinkerConfigFile());
      makeFileContent.replace("<!compiler-flags!>","");
      makeFileContent.replace("<!assembler-flags!>",nesicideProject->getCompilerAdditionalOptions());
      makeFileContent.replace("<!debug-file!>",nesicideProject->getProjectDebugInfoName());
      makeFileContent.replace("<!linker-flags!>",nesicideProject->getLinkerAdditionalOptions());
      makeFileContent.replace("<!source-dir!>",QDir::currentPath());
      makeFileContent.replace("<!object-dir!>",nesicideProject->getProjectOutputBasePath());
      makeFileContent.replace("<!clang-sources!>","");
      makeFileContent.replace("<!asm-sources!>",getSourcesFromProject().join(" "));

      // Write the file to disk.
      makeFile.write(makeFileContent.toAscii());

      makeFile.close();
      res.close();

      return true;
   }

   return false;
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
   QDir                         baseDir(QDir::currentPath());
   QDir                         outputDir(nesicideProject->getProjectOutputBasePath());
   QString                      outputName;
   QDir                         linkerConfigDir(nesicideProject->getLinkerConfigFile());
   int                          exitCode;
   bool                         ok = true;

   if ( nesicideProject->getProjectLinkerOutputName().isEmpty() )
   {
      outputName = outputDir.fromNativeSeparators(outputDir.filePath(nesicideProject->getProjectOutputName()+".prg"));
   }
   else
   {
      outputName = outputDir.fromNativeSeparators(outputDir.filePath(nesicideProject->getProjectLinkerOutputName()));
   }
   buildTextLogger->write("<b>Building: "+outputName+"</b>");

   // Copy the system environment to the child process.
   cc65.setProcessEnvironment(env);
   cc65.setWorkingDirectory(QDir::currentPath());

   // Clear the error storage.
   errors.clear();

   createMakefile();

   invocationStr = "make all";

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
   errors.append(stdioList);
   foreach ( const QString& str, stdioList )
   {
      buildTextLogger->write("<font color='red'>&nbsp;&nbsp;&nbsp;"+str+"</font>");
   }

   return ok;
}

static void ErrorFunc (const struct cc65_parseerror* E)
/* Handle errors or warnings that occur while parsing a debug file */
{
   char errorBuffer[256];
   sprintf(errorBuffer,
           "<font color=red>%s:%s(%lu): %s</font>\n",
           E->type? "Error" : "Warning",
           E->name,
           (unsigned long) E->line,
           E->errormsg);
   buildTextLogger->write(errorBuffer);
}

bool CCC65Interface::captureDebugInfo()
{
   QDir dir(QDir::currentPath());
   QString dbgInfoFile;

   if ( nesicideProject->getProjectDebugInfoName().isEmpty() )
   {
      dbgInfoFile = dir.fromNativeSeparators(dir.relativeFilePath(nesicideProject->getProjectOutputName()+".dbg"));
   }
   else
   {
      dbgInfoFile = dir.fromNativeSeparators(dir.relativeFilePath(nesicideProject->getProjectDebugInfoName()));
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
   QDir dir(QDir::currentPath());
   QString nesName;

   if ( nesicideProject->getProjectCartridgeOutputName().isEmpty() )
   {
      nesName = dir.fromNativeSeparators(dir.relativeFilePath(nesicideProject->getProjectOutputName()+".nes"));
   }
   else
   {
      nesName = dir.fromNativeSeparators(dir.relativeFilePath(nesicideProject->getProjectCartridgeOutputName()));
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
      cc65_free_sourceinfo(dbgInfo,dbgSources);
      dbgSources = cc65_get_sourcelist(dbgInfo);

      if ( dbgSources )
      {
         for ( file = 0; file < dbgSources->count; file++ )
         {
            files.append(QDir::fromNativeSeparators(dbgSources->data[file].source_name));
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
      cc65_free_symbolinfo(dbgInfo,dbgSymbols);
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

unsigned int CCC65Interface::getSymbolAddress(QString symbol)
{
   unsigned int addr = 0xFFFFFFFF;

   if ( dbgInfo )
   {
      cc65_free_symbolinfo(dbgInfo,dbgSymbols);
      dbgSymbols = cc65_symbol_byname(dbgInfo,symbol.toAscii().constData());

      if ( dbgSymbols )
      {
         if ( (dbgSymbols->count == 1) &&
              (dbgSymbols->data[0].symbol_type == CC65_SYM_LABEL) )
         {
            addr = dbgSymbols->data[0].symbol_value;
         }
      }
   }
   return addr;
}

QString CCC65Interface::getSourceFileFromAbsoluteAddress(uint32_t addr,uint32_t absAddr)
{
   int  line;

   if ( dbgInfo )
   {
      cc65_free_lineinfo(dbgInfo,dbgLines);
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
      cc65_free_lineinfo(dbgInfo,dbgLines);
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
      cc65_free_lineinfo(dbgInfo,dbgLines);
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
      cc65_free_lineinfo(dbgInfo,dbgLines);
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
         cc65_free_lineinfo(dbgInfo,dbgLines);
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

bool CCC65Interface::isErrorOnLineOfFile(QString file,int line)
{
   QString errorLookup;
   bool    found = false;

   // Form error string key.
   errorLookup = file+'('+QString::number(line)+"):";
   foreach ( const QString error, errors )
   {
      if ( error.contains(errorLookup) )
      {
         found = true;
         break;
      }
   }
   return found;
}
