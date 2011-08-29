#include "ccc65interface.h"

#include "cnesicideproject.h"
#include "iprojecttreeviewitem.h"

#include "main.h"

cc65_dbginfo        CCC65Interface::dbgInfo = NULL;
const cc65_libraryinfo*   CCC65Interface::dbgLibraries = NULL;
const cc65_moduleinfo*    CCC65Interface::dbgModules = NULL;
const cc65_scopeinfo*     CCC65Interface::dbgScopes = NULL;
const cc65_spaninfo*      CCC65Interface::dbgSpans = NULL;
const cc65_sourceinfo*    CCC65Interface::dbgSources = NULL;
const cc65_segmentinfo*   CCC65Interface::dbgSegments = NULL;
const cc65_lineinfo*      CCC65Interface::dbgLines = NULL;
const cc65_symbolinfo*    CCC65Interface::dbgSymbols = NULL;
QStringList         CCC65Interface::errors;

static const char* clangTargetRuleFmt =
      "vpath %<!extension!> $(foreach <!extension!>,$(SOURCES),$(dir $<!extension!>))\r\n\r\n"
      "$(OBJDIR)/%.o: %.<!extension!>\r\n"
      "\t$(COMPILE) --create-dep $(@:.o=.d) -c $(CFLAGS) -o $@ $<\r\n\r\n"
      ;
// CPTODO: This compiles a .s from a .c but not sure I want it yet:
// "\t$(COMPILE) -S $(CFLAGS) $<\r\n\r\n"
// (otherwise the .s from a .c is thrown away by cl65)

static const char* asmTargetRuleFmt =
      "vpath %<!extension!> $(foreach <!extension!>,$(SOURCES),$(dir $<!extension!>))\r\n\r\n"
      "$(OBJDIR)/%.o: %.<!extension!>\r\n"
      "\t$(ASSEMBLE) --create-dep $(@:.o=.d) $(ASFLAGS) -o $@ $<\r\n\r\n"
      ;

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
      if ( dbgLibraries )
      {
         cc65_free_libraryinfo(dbgInfo,dbgLibraries);
         dbgLibraries = 0;
      }
      if ( dbgModules )
      {
         cc65_free_moduleinfo(dbgInfo,dbgModules);
         dbgModules = 0;
      }
      if ( dbgScopes )
      {
         cc65_free_scopeinfo(dbgInfo,dbgScopes);
         dbgScopes = 0;
      }
      if ( dbgSpans )
      {
         cc65_free_spaninfo(dbgInfo,dbgSpans);
         dbgSpans = 0;
      }
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

QStringList CCC65Interface::getAssemblerSourcesFromProject()
{
   IProjectTreeViewItemIterator iter(nesicideProject->getProject()->getSources());
   QDir                         baseDir(QDir::currentPath());
   CSourceItem*                 source;
   QStringList                  sources;
   QStringList                  extensions = EnvironmentSettingsDialog::sourceExtensionsForAssembly().split(" ", QString::SkipEmptyParts);

   // For each source code object, compile it.
   while ( iter.current() )
   {
      source = dynamic_cast<CSourceItem*>(iter.current());
      foreach ( QString extension, extensions )
      {
         if ( source && source->path().endsWith(extension,Qt::CaseInsensitive) )
         {
            sources.append(baseDir.fromNativeSeparators(baseDir.relativeFilePath(source->path())));
         }
      }
      iter.next();
   }

   return sources;
}

QStringList CCC65Interface::getCLanguageSourcesFromProject()
{
   IProjectTreeViewItemIterator iter(nesicideProject->getProject()->getSources());
   QDir                         baseDir(QDir::currentPath());
   CSourceItem*                 source;
   QStringList                  sources;
   QStringList                  extensions = EnvironmentSettingsDialog::sourceExtensionsForC().split(" ", QString::SkipEmptyParts);

   // For each source code object, compile it.
   while ( iter.current() )
   {
      source = dynamic_cast<CSourceItem*>(iter.current());
      foreach ( QString extension, extensions )
      {
         if ( source && source->path().endsWith(extension,Qt::CaseInsensitive) )
         {
            sources.append(baseDir.fromNativeSeparators(baseDir.relativeFilePath(source->path())));
         }
      }
      iter.next();
   }

   return sources;
}

bool CCC65Interface::createMakefile()
{
   QDir outputDir(QDir::currentPath());
   QString outputName = outputDir.fromNativeSeparators(outputDir.filePath("Makefile"));
   QFile res(":/resources/Makefile");
   QFile makeFile(outputName);
   QString targetRules;
   QString targetRule;
   QStringList extensions;

   // Get the embedded universal makefile...
   res.open(QIODevice::ReadOnly);

   // Create target rules section (based on user-specified extensions so it needs
   // to be configured it can't just be specified in the Makefile resource template.
   extensions = EnvironmentSettingsDialog::sourceExtensionsForC().split(" ",QString::SkipEmptyParts);
   foreach ( QString extension, extensions )
   {
      targetRule = clangTargetRuleFmt;
      targetRule.replace("<!extension!>",extension.right(extension.length()-1)); // Chop off the '.'
      targetRules += targetRule;
   }
   extensions = EnvironmentSettingsDialog::sourceExtensionsForAssembly().split(" ",QString::SkipEmptyParts);
   foreach ( QString extension, extensions )
   {
      targetRule = asmTargetRuleFmt;
      targetRule.replace("<!extension!>",extension.right(extension.length()-1)); // Chop off the '.'
      targetRules += targetRule;
   }

   // Create the project's makefile...
   makeFile.open(QIODevice::ReadWrite|QIODevice::Truncate);

   if ( res.isOpen() && makeFile.isOpen() )
   {
      QString makeFileContent;

      // Read the embedded Makefile resource.
      makeFileContent = res.readAll();

      // Replace stuff that needs to be replaced...
      makeFileContent.replace("<!project-name!>",nesicideProject->getProjectOutputName());
      makeFileContent.replace("<!prg-rom-name!>",nesicideProject->getProjectLinkerOutputName());
      makeFileContent.replace("<!linker-config!>",nesicideProject->getLinkerConfigFile());
      makeFileContent.replace("<!compiler-flags!>",nesicideProject->getCompilerAdditionalOptions());
      makeFileContent.replace("<!assembler-flags!>",nesicideProject->getAssemblerAdditionalOptions());
      makeFileContent.replace("<!debug-file!>",nesicideProject->getProjectDebugInfoName());
      makeFileContent.replace("<!linker-flags!>",nesicideProject->getLinkerAdditionalOptions());
      makeFileContent.replace("<!source-dir!>",QDir::currentPath());
      makeFileContent.replace("<!object-dir!>",nesicideProject->getProjectOutputBasePath());
      makeFileContent.replace("<!prg-dir!>",nesicideProject->getProjectLinkerOutputBasePath());
      makeFileContent.replace("<!chr-dir!>",nesicideProject->getProjectCHRROMOutputBasePath());
      makeFileContent.replace("<!clang-sources!>",getCLanguageSourcesFromProject().join(" "));
      makeFileContent.replace("<!asm-sources!>",getAssemblerSourcesFromProject().join(" "));
      makeFileContent.replace("<!target-rules!>",targetRules);
      makeFileContent.replace("<!linker-dependencies!>",nesicideProject->getLinkerAdditionalDependencies());

      // Write the file to disk.
      makeFile.write(makeFileContent.toAscii());

      makeFile.close();
      res.close();

      return true;
   }

   return false;
}

void CCC65Interface::clean()
{
   QProcess                     cc65;
   QProcessEnvironment          env = QProcessEnvironment::systemEnvironment();
   QString                      invocationStr;
   QString                      stdioStr;
   QStringList                  stdioList;
   int                          exitCode;

   // Copy the system environment to the child process.
   cc65.setProcessEnvironment(env);
   cc65.setWorkingDirectory(QDir::currentPath());

   // Clear the error storage.
   errors.clear();

   createMakefile();

   invocationStr = "make clean";

   buildTextLogger->write(invocationStr);

   cc65.start(invocationStr);
   cc65.waitForFinished();
   cc65.waitForReadyRead();
   exitCode = cc65.exitCode();
   stdioStr = QString(cc65.readAllStandardOutput());
   stdioList = stdioStr.split(QRegExp("[\r\n]"),QString::SkipEmptyParts);
   foreach ( const QString& str, stdioList )
   {
      buildTextLogger->write("<font color='blue'>"+str+"</font>");
   }
   stdioStr = QString(cc65.readAllStandardError());
   stdioList = stdioStr.split(QRegExp("[\r\n]"),QString::SkipEmptyParts);
   errors.append(stdioList);
   foreach ( const QString& str, stdioList )
   {
      buildTextLogger->write("<font color='red'>"+str+"</font>");
   }

   return;
}

bool CCC65Interface::assemble()
{
   QProcess                     cc65;
   QProcessEnvironment          env = QProcessEnvironment::systemEnvironment();
   QString                      invocationStr;
   QString                      stdioStr;
   QStringList                  stdioList;
   QDir                         outputDir(nesicideProject->getProjectLinkerOutputBasePath());
   QString                      outputName;
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
      buildTextLogger->write("<font color='blue'>"+str+"</font>");
   }
   stdioStr = QString(cc65.readAllStandardError());
   stdioList = stdioStr.split(QRegExp("[\r\n]"),QString::SkipEmptyParts);
   errors.append(stdioList);
   foreach ( const QString& str, stdioList )
   {
      buildTextLogger->write("<font color='red'>"+str+"</font>");
   }
   if ( exitCode )
   {
      ok = false;
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

   // Check consistency of debug information when it's loaded.
   CCC65Interface::checkDebugInfo();

   return true;
}

int CCC65Interface::checkDebugInfo()
{
   // Check for newer files than debug info.
   QStringList files;
   QDateTime mtimeInDbginfo;
   QDateTime mtimeOfFile;
   QFileInfo fileInfo;
   unsigned int mtimeRaw;
   QString outdated = "The following files have been modified since the last build\n"
                      "and may not display correct information in the debuggers.\n\n";
   int outdatedCount = 0;

   // Get the file list to do a consistency check.
   files = CCC65Interface::getSourceFiles();

   foreach ( QString file, files )
   {
      fileInfo.setFile(file);
      mtimeOfFile = fileInfo.lastModified();
      mtimeRaw = CCC65Interface::getSourceFileModificationTime(file);
      mtimeInDbginfo = QDateTime::fromTime_t(mtimeRaw);
      if ( mtimeOfFile > mtimeInDbginfo )
      {
         outdated += file;
         outdatedCount++;
      }
   }

   if ( outdatedCount )
   {
      QMessageBox::warning(NULL,"Consistency problem...",outdated);
   }

   return outdatedCount;
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

   return nesicideProject->createProjectFromRom(nesName,true);
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

unsigned int CCC65Interface::getSourceFileModificationTime(QString sourceFile)
{
   unsigned int mtime = -1;
   int file;

   if ( dbgInfo )
   {
      cc65_free_sourceinfo(dbgInfo,dbgSources);
      dbgSources = cc65_get_sourcelist(dbgInfo);

      if ( dbgSources )
      {
         for ( file = 0; file < dbgSources->count; file++ )
         {
            if ( sourceFile == dbgSources->data[file].source_name )
            {
               mtime = dbgSources->data[file].source_mtime;
               break;
            }
         }
      }
   }
   return mtime;
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

cc65_symbol_type CCC65Interface::getSymbolType(QString symbol, int index)
{
   cc65_symbol_type type = (cc65_symbol_type)CC65_INV_ID;

   if ( dbgInfo )
   {
      cc65_free_symbolinfo(dbgInfo,dbgSymbols);
      dbgSymbols = cc65_symbol_byname(dbgInfo,symbol.toAscii().constData());

      if ( dbgSymbols )
      {
         if ( dbgSymbols->count > index )
         {
            type = dbgSymbols->data[index].symbol_type;
         }
      }
   }
   return type;
}

unsigned int CCC65Interface::getSymbolAddress(QString symbol, int index)
{
   unsigned int addr = 0xFFFFFFFF;

   if ( dbgInfo )
   {
      cc65_free_symbolinfo(dbgInfo,dbgSymbols);
      dbgSymbols = cc65_symbol_byname(dbgInfo,symbol.toAscii().constData());

      if ( dbgSymbols )
      {
         if ( dbgSymbols->count > index )
         {
            addr = dbgSymbols->data[index].symbol_value;
         }
      }
   }
   return addr;
}

unsigned int CCC65Interface::getSymbolAbsoluteAddress(QString symbol, int index)
{
   unsigned int addr;
   unsigned int absAddr = 0xFFFFFFFF;
   unsigned int addrOffset;

   if ( dbgInfo )
   {
      cc65_free_symbolinfo(dbgInfo,dbgSymbols);
      dbgSymbols = cc65_symbol_byname(dbgInfo,symbol.toAscii().constData());

      if ( dbgSymbols )
      {
         if ( (dbgSymbols->count > index) /*&&
              (dbgSymbols->data[index].symbol_type == CC65_SYM_LABEL)*/ )
         {
            addr = dbgSymbols->data[index].symbol_value;
            dbgSegments = cc65_segment_byid(dbgInfo,dbgSymbols->data[index].segment_id);

            if ( dbgSegments )
            {
               if ( dbgSegments->count == 1 )
               {
                  addrOffset = addr-dbgSegments->data[0].segment_start;
                  absAddr = dbgSegments->data[0].output_offs+addrOffset;
               }
            }
         }
      }
   }
   return absAddr;
}

unsigned int CCC65Interface::getSymbolSegment(QString symbol, int index)
{
   unsigned int seg = 0;

   if ( dbgInfo )
   {
      cc65_free_symbolinfo(dbgInfo,dbgSymbols);
      dbgSymbols = cc65_symbol_byname(dbgInfo,symbol.toAscii().constData());

      if ( dbgSymbols )
      {
         if ( (dbgSymbols->count > index) /*&&
              (dbgSymbols->data[index].symbol_type == CC65_SYM_LABEL)*/ )
         {
            seg = dbgSymbols->data[index].segment_id;
         }
      }
   }
   return seg;
}

QString CCC65Interface::getSymbolSegmentName(QString symbol, int index)
{
   QString seg = "?";

   if ( dbgInfo )
   {
      cc65_free_symbolinfo(dbgInfo,dbgSymbols);
      dbgSymbols = cc65_symbol_byname(dbgInfo,symbol.toAscii().constData());

      if ( dbgSymbols )
      {
         if ( (dbgSymbols->count > index) /*&&
              (dbgSymbols->data[index].symbol_type == CC65_SYM_LABEL)*/ )
         {
            if ( dbgSymbols->data[index].segment_id >= 0 )
            {
               dbgSegments = cc65_segment_byid(dbgInfo,dbgSymbols->data[index].segment_id);
               if ( dbgSegments )
               {
                  seg = dbgSegments->data[0].segment_name;
               }
            }
         }
      }
   }
   return seg;
}

unsigned int CCC65Interface::getSymbolIndexFromSegment(QString symbol, int segment)
{
   unsigned int index = 0;
   unsigned int idx;

   if ( dbgInfo )
   {
      cc65_free_symbolinfo(dbgInfo,dbgSymbols);
      dbgSymbols = cc65_symbol_byname(dbgInfo,symbol.toAscii().constData());

      if ( dbgSymbols )
      {
         for ( idx = 0; idx < dbgSymbols->count; idx++ )
         {
            if ( dbgSymbols->data[idx].segment_id == segment )
            {
               index = idx;
               break;
            }
         }
      }
   }
   return index;
}

unsigned int CCC65Interface::getSymbolSize(QString symbol, int index)
{
   unsigned int size = 0;

   if ( dbgInfo )
   {
      cc65_free_symbolinfo(dbgInfo,dbgSymbols);
      dbgSymbols = cc65_symbol_byname(dbgInfo,symbol.toAscii().constData());

      if ( dbgSymbols )
      {
         if ( dbgSymbols->count > index )
         {
            size = dbgSymbols->data[index].symbol_size;
         }
      }
   }
   return size;
}

int CCC65Interface::getSymbolMatchCount(QString symbol)
{
   int sym;
   int count = 0;

   if ( dbgInfo )
   {
      cc65_free_symbolinfo(dbgInfo,dbgSymbols);
      dbgSymbols = cc65_symbol_byname(dbgInfo,symbol.toAscii().constData());

      if ( dbgSymbols )
      {
         for ( sym = 0; sym < dbgSymbols->count; sym++ )
         {
            if ( dbgSymbols->data[sym].export_id == CC65_INV_ID )
            {
               count++;
            }
         }
      }
   }
   return count;
}

QString CCC65Interface::getSourceFileFromAbsoluteAddress(uint32_t addr,uint32_t absAddr)
{
   int span;
   int line;
   int highestTypeMatch = 0;
   int indexOfHighestTypeMatch = -1;

   if ( dbgInfo )
   {
      // Get the span containing this virtual address.
      cc65_free_spaninfo(dbgInfo,dbgSpans);
      dbgSpans = cc65_span_byaddr(dbgInfo,addr);

      if ( dbgSpans )
      {
         for ( span = 0; span < dbgSpans->count; span++ )
         {
            if ( dbgSpans->data[span].line_count )
            {
               // Get the segment potentially containing this absolute address.
               cc65_free_segmentinfo(dbgInfo,dbgSegments);
               dbgSegments = cc65_segment_byid(dbgInfo,dbgSpans->data[span].segment_id);

               // Get the line potentially containing this absolute address.
               cc65_free_lineinfo(dbgInfo,dbgLines);
               dbgLines = cc65_line_byspan(dbgInfo,dbgSpans->data[span].span_id);

               if ( dbgSegments && (dbgSegments->count == 1) &&
                    dbgLines && (dbgLines->count > 0) )
               {
                  if ( dbgSegments->data[0].output_name )
                  {
                     if ( (absAddr >= dbgSegments->data[0].output_offs-0x10) &&
                          (absAddr < (dbgSegments->data[0].output_offs-0x10+dbgSegments->data[0].segment_size)) )
                     {
                        for ( line = 0; line < dbgLines->count; line++ )
                        {
                           if ( dbgLines->data[line].line_type >= highestTypeMatch )
                           {
                              highestTypeMatch = dbgLines->data[line].line_type;
                              indexOfHighestTypeMatch = dbgLines->data[line].line_id;
                           }
                        }
                     }
                  }
                  else
                  {
                     if ( (absAddr >= dbgSegments->data[0].output_offs) &&
                          (absAddr < (dbgSegments->data[0].output_offs+dbgSegments->data[0].segment_size)) )
                     {
                        for ( line = 0; line < dbgLines->count; line++ )
                        {
                           if ( dbgLines->data[line].line_type >= highestTypeMatch )
                           {
                              highestTypeMatch = dbgLines->data[line].line_type;
                              indexOfHighestTypeMatch = dbgLines->data[line].line_id;
                           }
                        }
                     }
                  }
               }
            }
         }
         if ( indexOfHighestTypeMatch >= 0 )
         {
            // Get the picked line.
            cc65_free_lineinfo(dbgInfo,dbgLines);
            dbgLines = cc65_line_byid(dbgInfo,indexOfHighestTypeMatch);

            // Get the source file of the picked line.
            cc65_free_sourceinfo(dbgInfo,dbgSources);
            dbgSources = cc65_source_byid(dbgInfo,dbgLines->data[0].source_id);
            return dbgSources->data[0].source_name;
         }
      }
   }
   return "";
}

int CCC65Interface::getSourceLineFromAbsoluteAddress(uint32_t addr,uint32_t absAddr)
{
   int span;
   int line;
   int highestTypeMatch = 0;
   int indexOfHighestTypeMatch = -1;

   if ( dbgInfo )
   {
      // Get the span containing this virtual address.
      cc65_free_spaninfo(dbgInfo,dbgSpans);
      dbgSpans = cc65_span_byaddr(dbgInfo,addr);

      if ( dbgSpans )
      {
         for ( span = 0; span < dbgSpans->count; span++ )
         {
            if ( dbgSpans->data[span].line_count )
            {
               // Get the segment potentially containing this absolute address.
               cc65_free_segmentinfo(dbgInfo,dbgSegments);
               dbgSegments = cc65_segment_byid(dbgInfo,dbgSpans->data[span].segment_id);

               // Get the line potentially containing this absolute address.
               cc65_free_lineinfo(dbgInfo,dbgLines);
               dbgLines = cc65_line_byspan(dbgInfo,dbgSpans->data[span].span_id);

               if ( dbgSegments && (dbgSegments->count == 1) &&
                    dbgLines && (dbgLines->count > 0) )
               {
                  if ( dbgSegments->data[0].output_name )
                  {
                     if ( (absAddr >= dbgSegments->data[0].output_offs-0x10) &&
                          (absAddr < (dbgSegments->data[0].output_offs-0x10+dbgSegments->data[0].segment_size)) )
                     {
                        for ( line = 0; line < dbgLines->count; line++ )
                        {
                           if ( dbgLines->data[line].line_type >= highestTypeMatch )
                           {
                              highestTypeMatch = dbgLines->data[line].line_type;
                              indexOfHighestTypeMatch = dbgLines->data[line].line_id;
                           }
                        }
                     }
                  }
                  else
                  {
                     if ( (absAddr >= dbgSegments->data[0].output_offs) &&
                          (absAddr < (dbgSegments->data[0].output_offs+dbgSegments->data[0].segment_size)) )
                     {
                        for ( line = 0; line < dbgLines->count; line++ )
                        {
                           if ( dbgLines->data[line].line_type >= highestTypeMatch )
                           {
                              highestTypeMatch = dbgLines->data[line].line_type;
                              indexOfHighestTypeMatch = dbgLines->data[line].line_id;
                           }
                        }
                     }
                  }
               }
            }
         }
         if ( indexOfHighestTypeMatch >= 0 )
         {
            // Get the picked line.
            cc65_free_lineinfo(dbgInfo,dbgLines);
            dbgLines = cc65_line_byid(dbgInfo,indexOfHighestTypeMatch);
            return dbgLines->data[0].source_line;
         }
      }
   }
   return -1;
}

QString CCC65Interface::getSourceFileFromSymbol(QString symbol)
{
   int sym;

   cc65_free_symbolinfo(dbgInfo,dbgSymbols);
   dbgSymbols = cc65_symbol_byname(dbgInfo,symbol.toAscii().constData());

   // Getting a symbol by name gets all the def and ref entries for the symbol, so
   // we need to ignore the symbol count.  Move the 'sym' reference variable to the
   // proper symbol in the returned pile.
   if ( dbgSymbols )
   {
      for ( sym = 0; sym < dbgSymbols->count; sym++ )
      {
         if ( dbgSymbols->data[sym].export_id == CC65_INV_ID )
         {
            break;
         }
      }
      if ( sym < dbgSymbols->count )
      {
         cc65_free_lineinfo(dbgInfo,dbgLines);
         dbgLines = cc65_line_bysymdef(dbgInfo,dbgSymbols->data[sym].symbol_id);

         if ( dbgLines && (dbgLines->count == 1) )
         {
            cc65_free_sourceinfo(dbgInfo,dbgSources);
            dbgSources = cc65_source_byid(dbgInfo,dbgLines->data[0].source_id);

            if ( dbgSources && (dbgSources->count == 1) )
            {
               return dbgSources->data[0].source_name;
            }
         }
      }
   }

   return "";
}

int CCC65Interface::getSourceLineFromFileAndSymbol(QString file,QString symbol)
{
   int sym;

   cc65_free_symbolinfo(dbgInfo,dbgSymbols);
   dbgSymbols = cc65_symbol_byname(dbgInfo,symbol.toAscii().constData());

   // Getting a symbol by name gets all the def and ref entries for the symbol, so
   // we need to ignore the symbol count.  Move the 'sym' reference variable to the
   // proper symbol in the returned pile.
   if ( dbgSymbols )
   {
      for ( sym = 0; sym < dbgSymbols->count; sym++ )
      {
         if ( dbgSymbols->data[sym].export_id == CC65_INV_ID )
         {
            break;
         }
      }
      if ( sym < dbgSymbols->count )
      {
         cc65_free_lineinfo(dbgInfo,dbgLines);
         dbgLines = cc65_line_bysymdef(dbgInfo,dbgSymbols->data[sym].symbol_id);

         if ( dbgLines && (dbgLines->count == 1) )
         {
            cc65_free_sourceinfo(dbgInfo,dbgSources);
            dbgSources = cc65_source_byid(dbgInfo,dbgLines->data[0].source_id);

            if ( dbgSources && (dbgSources->count == 1) )
            {
               if ( dbgSources->data[0].source_name == file )
               {
                  return dbgLines->data[0].source_line;
               }
            }
         }
      }
   }

   return -1;
}

int CCC65Interface::getLineMatchCount(QString file, int source_line)
{
   int fidx;
   int span;

   if ( dbgInfo )
   {
      // Get source ID from file.
      cc65_free_sourceinfo(dbgInfo,dbgSources);
      dbgSources = cc65_get_sourcelist(dbgInfo);

      for ( fidx = 0; fidx < dbgSources->count; fidx++ )
      {
         if ( dbgSources->data[fidx].source_name == file )
         {
            cc65_free_lineinfo(dbgInfo,dbgLines);
            dbgLines = cc65_line_bynumber(dbgInfo,dbgSources->data[fidx].source_id,source_line);
            if ( dbgLines && (dbgLines->count == 1) )
            {
               // Get the spans for this line.
               cc65_free_spaninfo(dbgInfo,dbgSpans);
               dbgSpans = cc65_span_byline(dbgInfo,dbgLines->data[0].line_id);

               if ( dbgSpans )
               {
                  return dbgSpans->count;
               }
            }
         }
      }
   }

   return 0;
}

unsigned int CCC65Interface::getAddressFromFileAndLine(QString file,int source_line,int entry)
{
   int highestTypeMatch = 0;
   int indexOfHighestTypeMatch = -1;
   int span;
   int fidx;
   int count = 0;

   if ( dbgInfo )
   {
      cc65_free_sourceinfo(dbgInfo,dbgSources);
      dbgSources = cc65_get_sourcelist(dbgInfo);

      if ( dbgSources )
      {
         for ( fidx = 0; fidx < dbgSources->count; fidx++ )
         {
            if ( dbgSources->data[fidx].source_name == file )
            {
               // Get line information from source file.
               cc65_free_lineinfo(dbgInfo,dbgLines);
               dbgLines = cc65_line_bynumber(dbgInfo,dbgSources->data[fidx].source_id,source_line);

               if ( dbgLines && (dbgLines->count == 1) )
               {
                  // Get the spans for this line.
                  cc65_free_spaninfo(dbgInfo,dbgSpans);
                  dbgSpans = cc65_span_byline(dbgInfo,dbgLines->data[0].line_id);

                  if ( dbgSpans )
                  {
                     for ( span = 0; span < dbgSpans->count; span++ )
                     {
                        // Inject preference for MACRO expansions over C language over assembly...
                        if ( dbgLines->data[0].line_type >= highestTypeMatch )
                        {
                           highestTypeMatch = dbgLines->data[0].line_type;
                           indexOfHighestTypeMatch = dbgSpans->data[span].span_id;
                        }
                        if ( count == entry )
                        {
                           return dbgSpans->data[span].span_start;
                        }

                        // Keep track of when to return data if asked to break early.
                        count++;
                     }
                  }
               }
            }
         }

         if ( indexOfHighestTypeMatch >= 0 )
         {
            // Get span information to return.
            cc65_free_spaninfo(dbgInfo,dbgSpans);
            dbgSpans = cc65_span_byid(dbgInfo,indexOfHighestTypeMatch);
            return dbgSpans->data[0].span_start;
         }
      }
   }
   return -1;
}

unsigned int CCC65Interface::getAbsoluteAddressFromFileAndLine(QString file,int source_line,int entry)
{
   int highestTypeMatch = 0;
   int indexOfHighestTypeMatch = -1;
   int span;
   int fidx;
   int count = 0;

   if ( dbgInfo )
   {
      cc65_free_sourceinfo(dbgInfo,dbgSources);
      dbgSources = cc65_get_sourcelist(dbgInfo);

      if ( dbgSources )
      {
         for ( fidx = 0; fidx < dbgSources->count; fidx++ )
         {
            if ( dbgSources->data[fidx].source_name == file )
            {
               // Get line information from source file.
               cc65_free_lineinfo(dbgInfo,dbgLines);
               dbgLines = cc65_line_bynumber(dbgInfo,dbgSources->data[fidx].source_id,source_line);

               if ( dbgLines && (dbgLines->count == 1) )
               {
                  // Get the spans for this line.
                  cc65_free_spaninfo(dbgInfo,dbgSpans);
                  dbgSpans = cc65_span_byline(dbgInfo,dbgLines->data[0].line_id);

                  if ( dbgSpans )
                  {
                     for ( span = 0; span < dbgSpans->count; span++ )
                     {
                        // Inject preference for MACRO expansions over C language over assembly...
                        if ( dbgLines->data[0].line_type >= highestTypeMatch )
                        {
                           highestTypeMatch = dbgLines->data[0].line_type;
                           indexOfHighestTypeMatch = dbgSpans->data[span].span_id;
                        }
                        if ( count == entry )
                        {
                           // Get segment information from span.
                           cc65_free_segmentinfo(dbgInfo,dbgSegments);
                           dbgSegments = cc65_segment_byid(dbgInfo,dbgSpans->data[0].segment_id);
                           if ( dbgSegments->data[0].output_name )
                           {
                              return dbgSegments->data[0].output_offs+(dbgSpans->data[span].span_start-dbgSegments->data[0].segment_start)-0x10;
                           }
                           else
                           {
                              return dbgSegments->data[0].output_offs+(dbgSpans->data[span].span_start-dbgSegments->data[0].segment_start);
                           }

                           // Keep track of when to return data if asked to break early.
                           count++;
                        }
                     }
                  }
               }
            }
         }

         if ( indexOfHighestTypeMatch >= 0 )
         {
            // Get span information.
            cc65_free_spaninfo(dbgInfo,dbgSpans);
            dbgSpans = cc65_span_byid(dbgInfo,indexOfHighestTypeMatch);

            // Get segment information from span.
            cc65_free_segmentinfo(dbgInfo,dbgSegments);
            dbgSegments = cc65_segment_byid(dbgInfo,dbgSpans->data[0].segment_id);
            if ( dbgSegments->data[0].output_name )
            {
               return dbgSegments->data[0].output_offs+(dbgSpans->data[0].span_start-dbgSegments->data[0].segment_start)-0x10;
            }
            else
            {
               return dbgSegments->data[0].output_offs+(dbgSpans->data[0].span_start-dbgSegments->data[0].segment_start);
            }
         }
      }
   }
   return -1;
}


unsigned int CCC65Interface::getEndAddressFromAbsoluteAddress(uint32_t addr,uint32_t absAddr)
{
   int      span;
   int      line;
   int highestTypeMatch = 0;
   int indexOfHighestTypeMatch = -1;

   if ( dbgInfo )
   {
      cc65_free_spaninfo(dbgInfo,dbgSpans);
      dbgSpans = cc65_span_byaddr(dbgInfo,addr);

      if ( dbgSpans )
      {
         for ( span = 0; span < dbgSpans->count; span++ )
         {
            cc65_free_segmentinfo(dbgInfo,dbgSegments);
            dbgSegments = cc65_segment_byid(dbgInfo,dbgSpans->data[span].segment_id);

            cc65_free_lineinfo(dbgInfo,dbgLines);
            dbgLines = cc65_line_byspan(dbgInfo,dbgSpans->data[span].span_id);

            if ( dbgSegments && (dbgSegments->count == 1) &&
                 dbgLines && (dbgLines->count > 0) )
            {
               if ( dbgSegments->data[0].output_name )
               {
                  if ( (absAddr >= dbgSegments->data[0].output_offs+(dbgSpans->data[span].span_start-dbgSegments->data[0].segment_start)-0x10) &&
                       (absAddr <= dbgSegments->data[0].output_offs+(dbgSpans->data[span].span_end-dbgSegments->data[0].segment_start)-0x10) )
                  {
                     for ( line = 0; line < dbgLines->count; line++ )
                     {
                        if ( dbgLines->data[line].line_type >= highestTypeMatch )
                        {
                           highestTypeMatch = dbgLines->data[line].line_type;
                           indexOfHighestTypeMatch = dbgSpans->data[span].span_id;
                        }
                     }
                  }
               }
               else
               {
                  if ( (absAddr >= dbgSegments->data[0].output_offs+(dbgSpans->data[span].span_start-dbgSegments->data[0].segment_start)) &&
                       (absAddr <= dbgSegments->data[0].output_offs+(dbgSpans->data[span].span_end-dbgSegments->data[0].segment_start)) )
                  {
                     for ( line = 0; line < dbgLines->count; line++ )
                     {
                        if ( dbgLines->data[line].line_type >= highestTypeMatch )
                        {
                           highestTypeMatch = dbgLines->data[line].line_type;
                           indexOfHighestTypeMatch = dbgSpans->data[span].span_id;
                        }
                     }
                  }
               }
            }
         }
         if ( indexOfHighestTypeMatch >= 0 )
         {
            cc65_free_spaninfo(dbgInfo,dbgSpans);
            dbgSpans = cc65_span_byid(dbgInfo,indexOfHighestTypeMatch);
            return dbgSpans->data[0].span_end;
         }
      }
   }

   return 0xFFFFFFFF;
}

bool CCC65Interface::isAbsoluteAddressAnOpcode(uint32_t absAddr)
{
   uint32_t addr;
   int      span;

   if ( dbgInfo )
   {
      // Make addresses for where code might be in PRG-ROM space.
      addr = (absAddr&MASK_8KB)+MEM_32KB;
      for ( ; addr < MEM_64KB; addr += MEM_8KB )
      {
         cc65_free_spaninfo(dbgInfo,dbgSpans);
         dbgSpans = cc65_span_byaddr(dbgInfo,addr);

         if ( dbgSpans )
         {
            for ( span = 0; span < dbgSpans->count; span++ )
            {
               cc65_free_segmentinfo(dbgInfo,dbgSegments);
               dbgSegments = cc65_segment_byid(dbgInfo,dbgSpans->data[span].segment_id);

               if ( dbgSegments && (dbgSegments->count == 1) )
               {
                  if ( dbgSegments->data[0].output_name )
                  {
                     if ( absAddr == dbgSegments->data[0].output_offs+(dbgSpans->data[span].span_start-dbgSegments->data[0].segment_start)-0x10 )
                     {
                        return true;
                     }
                  }
                  else
                  {
                     if ( absAddr == dbgSegments->data[0].output_offs+(dbgSpans->data[span].span_start-dbgSegments->data[0].segment_start) )
                     {
                        return true;
                     }
                  }
               }
            }
         }
      }
   }

   return false;
}

bool CCC65Interface::isErrorOnLineOfFile(QString file,int source_line)
{
   QString errorLookup;
   bool    found = false;

   // Form error string key.
   errorLookup = file+'('+QString::number(source_line)+"):";
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

bool CCC65Interface::isStringASymbol(QString string)
{
   if ( dbgInfo )
   {
      dbgSymbols = cc65_symbol_byname(dbgInfo,string.toAscii().constData());
      if ( dbgSymbols )
      {
         return true;
      }
   }
   return false;
}
