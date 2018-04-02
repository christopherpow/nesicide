#include "ccc65interface.h"

#include "cnesicideproject.h"
#include "iprojecttreeviewitem.h"

#include "dbg_cnes6502.h"

#include "main.h"

cc65_dbginfo        CCC65Interface::dbgInfo = NULL;
QStringList         CCC65Interface::errors;
QString             CCC65Interface::targetMachine = "none";

// This utility compares two file paths regardless of original slashery.
bool fileNamesAreIdentical(QString file1, QString file2)
{
   file1 = QDir::fromNativeSeparators(file1);
   file2 = QDir::fromNativeSeparators(file2);
   return ( file1 == file2 );
}

static const char* clangTargetRuleFmt =
      "vpath %<!extension!> $(foreach <!extension!>,$(SOURCES),$(dir $<!extension!>))\r\n\r\n"
      "$(OBJDIR)/%.o: %.<!extension!>\r\n"
      "\t$(COMPILE) --create-dep $(@:.o=.d) -S $(CFLAGS) -o $(@:.o=.s) $<\r\n"
      "\t$(ASSEMBLE) $(ASFLAGS) -o $@ $(@:.o=.s)\r\n\r\n"
      ;

static const char* asmTargetRuleFmt =
      "vpath %<!extension!> $(foreach <!extension!>,$(SOURCES),$(dir $<!extension!>))\r\n\r\n"
      "$(OBJDIR)/%.o: %.<!extension!>\r\n"
      "\t$(ASSEMBLE) $(ASFLAGS) -o $@ $<\r\n\r\n"
      ;

CCC65Interface::CCC65Interface()
{
}

CCC65Interface::~CCC65Interface()
{
   clear();
}

void CCC65Interface::updateTargetMachine(QString target)
{
   targetMachine = target;
}

void CCC65Interface::clear()
{
   cc65_free_dbginfo(dbgInfo);
   dbgInfo = 0;
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
   QStringList                  headerExtensions = EnvironmentSettingsDialog::headerExtensions().split(" ", QString::SkipEmptyParts);
   bool                         add;

   // For each source code object, compile it.
   while ( iter.current() )
   {
      source = dynamic_cast<CSourceItem*>(iter.current());
      add = true;
      foreach ( QString extension, extensions )
      {
         if ( source && source->path().endsWith(extension,Qt::CaseInsensitive) )
         {
            foreach ( QString headerExtension, headerExtensions )
            {
               if ( source->path().endsWith(headerExtension,Qt::CaseInsensitive) )
               {
                  add = false;
                  break;
               }
            }

            if ( add )
            {
               sources.append(baseDir.fromNativeSeparators(baseDir.relativeFilePath(source->path())));
            }
         }
      }
      iter.next();
   }

   return sources;
}

QStringList CCC65Interface::getCustomSourcesFromProject()
{
   IProjectTreeViewItemIterator iter(nesicideProject->getProject()->getSources());
   QDir                         baseDir(QDir::currentPath());
   CSourceItem*                 source;
   QStringList                  sources;
   QStringList                  extensions = EnvironmentSettingsDialog::customExtensions().split(" ", QString::SkipEmptyParts);
   QStringList                  headerExtensions = EnvironmentSettingsDialog::headerExtensions().split(" ", QString::SkipEmptyParts);
   bool                         add;

   // For each source code object, compile it.
   while ( iter.current() )
   {
      source = dynamic_cast<CSourceItem*>(iter.current());
      add = true;
      foreach ( QString extension, extensions )
      {
         if ( source && source->path().endsWith(extension,Qt::CaseInsensitive) )
         {
            foreach ( QString headerExtension, headerExtensions )
            {
               if ( source->path().endsWith(headerExtension,Qt::CaseInsensitive) )
               {
                  add = false;
                  break;
               }
            }

            if ( add )
            {
               sources.append(baseDir.fromNativeSeparators(baseDir.relativeFilePath(source->path())));
            }
         }
      }
      iter.next();
   }

   return sources;
}

bool CCC65Interface::createMakefile()
{
   QDir outputDir(QDir::currentPath());
   QString outputName = outputDir.fromNativeSeparators(outputDir.filePath("nesicide.mk"));
   QFile res(":/resources/NESICIDE.mk_in");
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
      makeFileContent.replace("<!target-machine!>",targetMachine);
      makeFileContent.replace("<!project-name!>",nesicideProject->getProjectOutputName());
      makeFileContent.replace("<!prg-rom-name!>",nesicideProject->getProjectLinkerOutputName());
      makeFileContent.replace("<!linker-config!>",nesicideProject->getLinkerConfigFile());
      makeFileContent.replace("<!compiler-flags!>",nesicideProject->getCompilerAdditionalOptions());
      makeFileContent.replace("<!compiler-include-paths!>",nesicideProject->getCompilerIncludePaths());
      makeFileContent.replace("<!compiler-defines!>",nesicideProject->getCompilerDefinedSymbols());
      makeFileContent.replace("<!assembler-flags!>",nesicideProject->getAssemblerAdditionalOptions());
      makeFileContent.replace("<!assembler-include-paths!>",nesicideProject->getAssemblerIncludePaths());
      makeFileContent.replace("<!assembler-defines!>",nesicideProject->getAssemblerDefinedSymbols());
      makeFileContent.replace("<!debug-file!>",nesicideProject->getProjectDebugInfoName());
      makeFileContent.replace("<!linker-flags!>",nesicideProject->getLinkerAdditionalOptions());
      makeFileContent.replace("<!source-dir!>",QDir::currentPath());
      makeFileContent.replace("<!object-dir!>",nesicideProject->getProjectOutputBasePath());
      makeFileContent.replace("<!prg-dir!>",nesicideProject->getProjectLinkerOutputBasePath());
      makeFileContent.replace("<!chr-dir!>",nesicideProject->getProjectCHRROMOutputBasePath());
      makeFileContent.replace("<!clang-sources!>",getCLanguageSourcesFromProject().join(" "));
      makeFileContent.replace("<!asm-sources!>",getAssemblerSourcesFromProject().join(" "));
      makeFileContent.replace("<!custom-sources!>",getCustomSourcesFromProject().join(" "));
      makeFileContent.replace("<!target-rules!>",targetRules);
      makeFileContent.replace("<!linker-dependencies!>",nesicideProject->getLinkerAdditionalDependencies());

      if ( !nesicideProject->getMakefileCustomRulesFile().isEmpty() )
      {
         makeFileContent.replace("<!custom-rules!>",QString("-include ")+nesicideProject->getMakefileCustomRulesFile());
      }
      else
      {
         makeFileContent.replace("<!custom-rules!>","");
      }

      // Write the file to disk.
      makeFile.write(makeFileContent.toLatin1());

      makeFile.close();
      res.close();

      return true;
   }

   return false;
}

void CCC65Interface::clean()
{
   QProcess                     make;
   QStringList                  env = QProcess::systemEnvironment();
   QString                      invocationStr;
   QString                      stdioStr;
   QStringList                  stdioList;
   int                          exitCode;

   // Copy the system environment to the child process.
   make.setEnvironment(env);
   make.setWorkingDirectory(QDir::currentPath());

   // Clear the error storage.
   errors.clear();

   createMakefile();

   invocationStr = "make -f nesicide.mk clean";

   buildTextLogger->write(invocationStr);

   make.start(invocationStr);
   make.waitForFinished();
   make.waitForReadyRead();
   exitCode = make.exitCode();
   stdioStr = QString(make.readAllStandardOutput());
   stdioList = stdioStr.split(QRegExp("[\r\n]"),QString::SkipEmptyParts);
   foreach ( const QString& str, stdioList )
   {
      buildTextLogger->write("<font color='blue'>"+str+"</font>");
   }
   stdioStr = QString(make.readAllStandardError());
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
   QProcess                     make;
   QStringList                  env = QProcess::systemEnvironment();
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
   make.setEnvironment(env);
   make.setWorkingDirectory(QDir::currentPath());

   // Clear the error storage.
   errors.clear();

   createMakefile();

   invocationStr = "make -f nesicide.mk all";

   buildTextLogger->write(invocationStr);

   make.start(invocationStr);
   make.waitForFinished();
   make.waitForReadyRead();
   exitCode = make.exitCode();
   stdioStr = QString(make.readAllStandardOutput());
   stdioList = stdioStr.split(QRegExp("[\r\n]"),QString::SkipEmptyParts);
   foreach ( const QString& str, stdioList )
   {
      buildTextLogger->write("<font color='blue'>"+str+"</font>");
   }
   stdioStr = QString(make.readAllStandardError());
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
           "<font color=red>%s(%lu):%s - %s</font>\n",
           E->name,
           (unsigned long) E->line,
           E->type? "Error" : "Warning",
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

   CCC65Interface::clear();

   dbgInfo = cc65_read_dbginfo(dbgInfoFile.toLatin1().constData(), ErrorFunc);

   if (dbgInfo == 0)
   {
      return false;
   }

   // Check consistency of debug information when it's loaded.
   CCC65Interface::isBuildUpToDate();

   return true;
}

bool CCC65Interface::isBuildUpToDate()
{
   QProcess                     make;
   QProcessEnvironment          env = QProcessEnvironment::systemEnvironment();
   QString                      invocationStr;
   QString                      stdioStr;
   QStringList                  stdioList;
   int                          exitCode;
   QString outdated = "The NES ROM image is older than one or more of its source files.\n"
                      "Debuggers may not display correct information unless the NES ROM\n"
                      "is rebuilt.\n\n";
   bool ok = true;

   // 'Build' is up-to-date if no sources present.
   if ( (getCLanguageSourcesFromProject().count() ||
        (getAssemblerSourcesFromProject().count())) )
   {
      // Copy the system environment to the child process.
      make.setProcessEnvironment(env);
      make.setWorkingDirectory(QDir::currentPath());

      // Clear the error storage.
      errors.clear();

      createMakefile();

      invocationStr = "make -f nesicide.mk -q all";

      make.start(invocationStr);
      make.waitForFinished();
      exitCode = make.exitCode();

      if ( exitCode == 1 )
      {
         QMessageBox::warning(NULL,"Consistency problem!",outdated);
         ok = false;
      }
   }

   return ok;
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
   const cc65_sourceinfo* dbgSources;
   QStringList files;
   int file;

   if ( dbgInfo )
   {
      dbgSources = cc65_get_sourcelist(dbgInfo);

      if ( dbgSources )
      {
         for ( file = 0; file < dbgSources->count; file++ )
         {
            files.append(QDir::fromNativeSeparators(dbgSources->data[file].source_name));
         }

         cc65_free_sourceinfo(dbgInfo,dbgSources);
      }
   }
   return files;
}

uint32_t CCC65Interface::getSegmentBase(QString segment)
{
   const cc65_segmentinfo* dbgSegments;
   int seg;
   int addr = -1;

   if ( dbgInfo )
   {
      dbgSegments = cc65_get_segmentlist(dbgInfo);
      if ( dbgSegments )
      {
         for ( seg = 0; seg < dbgSegments->count; seg++ )
         {
            if ( dbgSegments->data[seg].segment_name == segment )
            {
               addr = dbgSegments->data[seg].segment_start;
            }
         }

         cc65_free_segmentinfo(dbgInfo,dbgSegments);
      }
   }

   return addr;
}

unsigned int CCC65Interface::getSourceFileModificationTime(QString sourceFile)
{
   const cc65_sourceinfo* dbgSources;
   unsigned int mtime = -1;
   int file;

   if ( dbgInfo )
   {
      dbgSources = cc65_get_sourcelist(dbgInfo);

      if ( dbgSources )
      {
         for ( file = 0; file < dbgSources->count; file++ )
         {
            if ( fileNamesAreIdentical(dbgSources->data[file].source_name,sourceFile) )
            {
               mtime = dbgSources->data[file].source_mtime;
               break;
            }
         }

         cc65_free_sourceinfo(dbgInfo,dbgSources);
      }
   }
   return mtime;
}

QStringList CCC65Interface::getSymbolsForSourceFile(QString /*sourceFile*/)
{
   const cc65_symbolinfo* dbgSymbols;
   QStringList symbols;
   int sym;

   if ( dbgInfo )
   {
      dbgSymbols = cc65_symbol_inrange(dbgInfo,0,0xFFFF);

      if ( dbgSymbols )
      {
         for ( sym = 0; sym < dbgSymbols->count; sym++ )
         {
            if ( dbgSymbols->data[sym].export_id == CC65_INV_ID )
            {
               symbols.append(dbgSymbols->data[sym].symbol_name);
            }
         }

         cc65_free_symbolinfo(dbgInfo,dbgSymbols);
      }
   }
   return symbols;
}

cc65_symbol_type CCC65Interface::getSymbolType(QString symbol, int index)
{
   const cc65_symbolinfo* dbgSymbols;
   cc65_symbol_type type = (cc65_symbol_type)CC65_INV_ID;

   if ( dbgInfo )
   {
      dbgSymbols = cc65_symbol_byname(dbgInfo,symbol.toLatin1().constData());

      if ( dbgSymbols )
      {
         if ( dbgSymbols->count > index )
         {
            type = dbgSymbols->data[index].symbol_type;
         }

         cc65_free_symbolinfo(dbgInfo,dbgSymbols);
      }
   }
   return type;
}

unsigned int CCC65Interface::getSymbolAddress(QString symbol, int index)
{
   const cc65_symbolinfo* dbgSymbols;
   unsigned int addr = 0xFFFFFFFF;
   int sym;

   // Getting a symbol by name gets all the def and ref entries for the symbol, so
   // we need to ignore the symbol count.  Move the 'sym' reference variable to the
   // proper symbol in the returned pile.
   if ( dbgInfo )
   {
      dbgSymbols = cc65_symbol_byname(dbgInfo,symbol.toLatin1().constData());

      if ( dbgSymbols )
      {
         for ( sym = 0; sym < dbgSymbols->count; sym++ )
         {
            if ( dbgSymbols->data[sym].export_id == CC65_INV_ID )
            {
               if ( !index )
               {
                  break;
               }
               index--;
            }
         }
         if ( sym < dbgSymbols->count )
         {
            addr = dbgSymbols->data[sym].symbol_value;
         }

         cc65_free_symbolinfo(dbgInfo,dbgSymbols);
      }
   }
   return addr;
}

unsigned int CCC65Interface::getSymbolAbsoluteAddress(QString symbol, int index)
{
   // Dispatch to appropriate target machine handler.
   if ( !targetMachine.compare("nes",Qt::CaseInsensitive) )
   {
      return nesGetSymbolAbsoluteAddress(symbol,index);
   }
   else if ( !targetMachine.compare("c64",Qt::CaseInsensitive) )
   {
      return c64GetSymbolAbsoluteAddress(symbol,index);
   }
}

unsigned int CCC65Interface::nesGetSymbolAbsoluteAddress(QString symbol, int index)
{
   const cc65_symbolinfo* dbgSymbols;
   const cc65_segmentinfo* dbgSegments;
   unsigned int addr;
   unsigned int absAddr = 0xFFFFFFFF;
   unsigned int addrOffset;
   int sym;

   // Getting a symbol by name gets all the def and ref entries for the symbol, so
   // we need to ignore the symbol count.  Move the 'sym' reference variable to the
   // proper symbol in the returned pile.
   if ( dbgInfo )
   {
      dbgSymbols = cc65_symbol_byname(dbgInfo,symbol.toLatin1().constData());

      if ( dbgSymbols )
      {
         for ( sym = 0; sym < dbgSymbols->count; sym++ )
         {
            if ( dbgSymbols->data[sym].export_id == CC65_INV_ID )
            {
               if ( !index )
               {
                  break;
               }
               index--;
            }
         }

         if ( sym < dbgSymbols->count )
         {
            addr = dbgSymbols->data[sym].symbol_value;

            if ( dbgSymbols->data[sym].segment_id != CC65_INV_ID )
            {
               dbgSegments = cc65_segment_byid(dbgInfo,dbgSymbols->data[sym].segment_id);

               if ( dbgSegments )
               {
                  if ( dbgSegments->count == 1 )
                  {
                     addrOffset = addr-dbgSegments->data[0].segment_start;
                     absAddr = dbgSegments->data[0].output_offs+addrOffset;
                  }

                  cc65_free_segmentinfo(dbgInfo,dbgSegments);
               }
            }
         }

         cc65_free_symbolinfo(dbgInfo,dbgSymbols);
      }
   }
   return absAddr;
}

unsigned int CCC65Interface::c64GetSymbolAbsoluteAddress(QString symbol, int index)
{
   const cc65_symbolinfo* dbgSymbols;
   const cc65_segmentinfo* dbgSegments;
   unsigned int addr;
   unsigned int absAddr = 0xFFFFFFFF;
   unsigned int addrOffset;
   int sym;

   // Getting a symbol by name gets all the def and ref entries for the symbol, so
   // we need to ignore the symbol count.  Move the 'sym' reference variable to the
   // proper symbol in the returned pile.
   if ( dbgInfo )
   {
      dbgSymbols = cc65_symbol_byname(dbgInfo,symbol.toLatin1().constData());

      if ( dbgSymbols )
      {
         for ( sym = 0; sym < dbgSymbols->count; sym++ )
         {
            if ( dbgSymbols->data[sym].export_id == CC65_INV_ID )
            {
               if ( !index )
               {
                  break;
               }
               index--;
            }
         }

         if ( sym < dbgSymbols->count )
         {
            addr = dbgSymbols->data[sym].symbol_value;

            if ( dbgSymbols->data[sym].segment_id != CC65_INV_ID )
            {
               dbgSegments = cc65_segment_byid(dbgInfo,dbgSymbols->data[sym].segment_id);

               if ( dbgSegments )
               {
                  if ( dbgSegments->count == 1 )
                  {
                     addrOffset = addr-dbgSegments->data[0].segment_start;
                     absAddr = dbgSegments->data[0].segment_start+addrOffset;
                  }

                  cc65_free_segmentinfo(dbgInfo,dbgSegments);
               }
            }
         }

         cc65_free_symbolinfo(dbgInfo,dbgSymbols);
      }
   }
   return absAddr;
}

unsigned int CCC65Interface::getSymbolSegment(QString symbol, int index)
{
   const cc65_symbolinfo* dbgSymbols;
   unsigned int seg = 0;
   int sym;

   // Getting a symbol by name gets all the def and ref entries for the symbol, so
   // we need to ignore the symbol count.  Move the 'sym' reference variable to the
   // proper symbol in the returned pile.
   if ( dbgInfo )
   {
      dbgSymbols = cc65_symbol_byname(dbgInfo,symbol.toLatin1().constData());

      if ( dbgSymbols )
      {
         for ( sym = 0; sym < dbgSymbols->count; sym++ )
         {
            if ( dbgSymbols->data[sym].export_id == CC65_INV_ID )
            {
               if ( !index )
               {
                  break;
               }
               index--;
            }
         }
         if ( sym < dbgSymbols->count )
         {
            seg = dbgSymbols->data[sym].segment_id;
         }

         cc65_free_symbolinfo(dbgInfo,dbgSymbols);
      }
   }
   return seg;
}

QString CCC65Interface::getSymbolSegmentName(QString symbol, int index)
{
   const cc65_symbolinfo* dbgSymbols;
   const cc65_segmentinfo* dbgSegments;
   QString seg = "?";
   int sym;

   // Getting a symbol by name gets all the def and ref entries for the symbol, so
   // we need to ignore the symbol count.  Move the 'sym' reference variable to the
   // proper symbol in the returned pile.
   if ( dbgInfo )
   {
      dbgSymbols = cc65_symbol_byname(dbgInfo,symbol.toLatin1().constData());

      if ( dbgSymbols )
      {
         for ( sym = 0; sym < dbgSymbols->count; sym++ )
         {
            if ( dbgSymbols->data[sym].export_id == CC65_INV_ID )
            {
               if ( !index )
               {
                  break;
               }
               index--;
            }
         }
         if ( sym < dbgSymbols->count )
         {
            if ( dbgSymbols->data[sym].segment_id != CC65_INV_ID )
            {
               dbgSegments = cc65_segment_byid(dbgInfo,dbgSymbols->data[sym].segment_id);
               if ( dbgSegments )
               {
                  seg = dbgSegments->data[0].segment_name;

                  cc65_free_segmentinfo(dbgInfo,dbgSegments);
               }
            }
         }

         cc65_free_symbolinfo(dbgInfo,dbgSymbols);
      }
   }
   return seg;
}

unsigned int CCC65Interface::getSymbolIndexFromSegment(QString symbol, int segment)
{
   const cc65_symbolinfo* dbgSymbols;
   unsigned int index = 0;
   unsigned int idx;

   if ( dbgInfo )
   {
      dbgSymbols = cc65_symbol_byname(dbgInfo,symbol.toLatin1().constData());

      if ( dbgSymbols )
      {
         for ( idx = 0; idx < dbgSymbols->count; idx++ )
         {
            if ( /*(dbgSymbols->data[idx].export_id == CC65_INV_ID) &&*/
                 (dbgSymbols->data[idx].segment_id == segment) )
            {
               index = idx;
               break;
            }
         }

         cc65_free_symbolinfo(dbgInfo,dbgSymbols);
      }
   }
   return index;
}

unsigned int CCC65Interface::getSymbolSize(QString symbol, int index)
{
   const cc65_symbolinfo* dbgSymbols;
   unsigned int size = 0;
   int sym;

   // Getting a symbol by name gets all the def and ref entries for the symbol, so
   // we need to ignore the symbol count.  Move the 'sym' reference variable to the
   // proper symbol in the returned pile.
   if ( dbgInfo )
   {
      dbgSymbols = cc65_symbol_byname(dbgInfo,symbol.toLatin1().constData());

      if ( dbgSymbols )
      {
         for ( sym = 0; sym < dbgSymbols->count; sym++ )
         {
            if ( dbgSymbols->data[sym].export_id == CC65_INV_ID )
            {
               if ( !index )
               {
                  break;
               }
               index--;
            }
         }
         if ( sym < dbgSymbols->count )
         {
            size = dbgSymbols->data[sym].symbol_size;
         }

         cc65_free_symbolinfo(dbgInfo,dbgSymbols);
      }
   }
   return size;
}

int CCC65Interface::getSymbolMatchCount(QString symbol)
{
   const cc65_symbolinfo* dbgSymbols;
   int sym;
   int count = 0;

   if ( dbgInfo )
   {
      dbgSymbols = cc65_symbol_byname(dbgInfo,symbol.toLatin1().constData());

      if ( dbgSymbols )
      {
         for ( sym = 0; sym < dbgSymbols->count; sym++ )
         {
            if ( dbgSymbols->data[sym].export_id == CC65_INV_ID )
            {
               count++;
            }
         }

         cc65_free_symbolinfo(dbgInfo,dbgSymbols);
      }
   }
   return count;
}

QString CCC65Interface::getSourceFileFromAbsoluteAddress(uint32_t addr,uint32_t absAddr)
{
   // Dispatch to appropriate target machine handler.
   if ( !targetMachine.compare("nes",Qt::CaseInsensitive) )
   {
      return QDir::fromNativeSeparators(nesGetSourceFileFromAbsoluteAddress(addr,absAddr));
   }
   else if ( !targetMachine.compare("c64",Qt::CaseInsensitive) )
   {
      return QDir::fromNativeSeparators(c64GetSourceFileFromAbsoluteAddress(addr,absAddr));
   }
}

QString CCC65Interface::nesGetSourceFileFromAbsoluteAddress(uint32_t addr,uint32_t absAddr)
{
   const cc65_spaninfo* dbgSpans;
   const cc65_segmentinfo* dbgSegments;
   const cc65_lineinfo* dbgLines;
   const cc65_sourceinfo* dbgSources;
   int span;
   int line;
   int highestTypeMatch = 0;
   int indexOfHighestTypeMatch = -1;
   QString file = "";

   if ( dbgInfo )
   {
      // Get the span containing this virtual address.
      dbgSpans = cc65_span_byaddr(dbgInfo,addr);

      if ( dbgSpans )
      {
         for ( span = 0; span < dbgSpans->count; span++ )
         {
            if ( dbgSpans->data[span].line_count )
            {
               // Get the segment potentially containing this absolute address.
               dbgSegments = cc65_segment_byid(dbgInfo,dbgSpans->data[span].segment_id);

               // Get the line potentially containing this absolute address.
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
               if ( dbgSegments )
               {
                  cc65_free_segmentinfo(dbgInfo,dbgSegments);
               }
               if ( dbgLines )
               {
                  cc65_free_lineinfo(dbgInfo,dbgLines);
               }
            }
         }

         cc65_free_spaninfo(dbgInfo,dbgSpans);

         if ( indexOfHighestTypeMatch >= 0 )
         {
            // Get the picked line.
            dbgLines = cc65_line_byid(dbgInfo,indexOfHighestTypeMatch);

            // Get the source file of the picked line.
            dbgSources = cc65_source_byid(dbgInfo,dbgLines->data[0].source_id);
            file = dbgSources->data[0].source_name;

            cc65_free_sourceinfo(dbgInfo,dbgSources);
            cc65_free_lineinfo(dbgInfo,dbgLines);
         }
      }
   }
   return QDir::fromNativeSeparators(file);
}

QString CCC65Interface::c64GetSourceFileFromAbsoluteAddress(uint32_t addr,uint32_t absAddr)
{
   const cc65_spaninfo* dbgSpans;
   const cc65_segmentinfo* dbgSegments;
   const cc65_lineinfo* dbgLines;
   const cc65_sourceinfo* dbgSources;
   int span;
   int line;
   int highestTypeMatch = 0;
   int indexOfHighestTypeMatch = -1;
   QString file = "";

   if ( dbgInfo )
   {
      // Get the span containing this virtual address.
      dbgSpans = cc65_span_byaddr(dbgInfo,addr);

      if ( dbgSpans )
      {
         for ( span = 0; span < dbgSpans->count; span++ )
         {
            if ( dbgSpans->data[span].line_count )
            {
               // Get the segment potentially containing this absolute address.
               dbgSegments = cc65_segment_byid(dbgInfo,dbgSpans->data[span].segment_id);

               // Get the line potentially containing this absolute address.
               dbgLines = cc65_line_byspan(dbgInfo,dbgSpans->data[span].span_id);

               if ( dbgSegments && (dbgSegments->count == 1) &&
                    dbgLines && (dbgLines->count > 0) )
               {
                  if ( (absAddr >= dbgSegments->data[0].segment_start) &&
                       (absAddr < (dbgSegments->data[0].segment_start+dbgSegments->data[0].segment_size)) )
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
               if ( dbgSegments )
               {
                  cc65_free_segmentinfo(dbgInfo,dbgSegments);
               }
               if ( dbgLines )
               {
                  cc65_free_lineinfo(dbgInfo,dbgLines);
               }
            }
         }

         cc65_free_spaninfo(dbgInfo,dbgSpans);

         if ( indexOfHighestTypeMatch >= 0 )
         {
            // Get the picked line.
            dbgLines = cc65_line_byid(dbgInfo,indexOfHighestTypeMatch);

            // Get the source file of the picked line.
            dbgSources = cc65_source_byid(dbgInfo,dbgLines->data[0].source_id);
            file = dbgSources->data[0].source_name;

            cc65_free_sourceinfo(dbgInfo,dbgSources);
            cc65_free_lineinfo(dbgInfo,dbgLines);
         }
      }
   }
   return QDir::fromNativeSeparators(file);
}

int CCC65Interface::getSourceLineFromAbsoluteAddress(uint32_t addr,uint32_t absAddr)
{
   // Dispatch to appropriate target machine handler.
   if ( !targetMachine.compare("nes",Qt::CaseInsensitive) )
   {
      return nesGetSourceLineFromAbsoluteAddress(addr,absAddr);
   }
   else if ( !targetMachine.compare("c64",Qt::CaseInsensitive) )
   {
      return c64GetSourceLineFromAbsoluteAddress(addr,absAddr);
   }
}

int CCC65Interface::nesGetSourceLineFromAbsoluteAddress(uint32_t addr,uint32_t absAddr)
{
   const cc65_spaninfo* dbgSpans;
   const cc65_lineinfo* dbgLines;
   const cc65_segmentinfo* dbgSegments;
   int span;
   int line;
   int highestTypeMatch = 0;
   int indexOfHighestTypeMatch = -1;
   int source_line = -1;

   if ( dbgInfo )
   {
      // Get the span containing this virtual address.
      dbgSpans = cc65_span_byaddr(dbgInfo,addr);

      if ( dbgSpans )
      {
         for ( span = 0; span < dbgSpans->count; span++ )
         {
            if ( dbgSpans->data[span].line_count )
            {
               // Get the segment potentially containing this absolute address.
               dbgSegments = cc65_segment_byid(dbgInfo,dbgSpans->data[span].segment_id);

               // Get the line potentially containing this absolute address.
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
               if ( dbgSegments )
               {
                  cc65_free_segmentinfo(dbgInfo,dbgSegments);
               }
               if ( dbgLines )
               {
                  cc65_free_lineinfo(dbgInfo,dbgLines);
               }
            }
         }
         if ( indexOfHighestTypeMatch >= 0 )
         {
            // Get the picked line.
            dbgLines = cc65_line_byid(dbgInfo,indexOfHighestTypeMatch);
            source_line = dbgLines->data[0].source_line;
            cc65_free_lineinfo(dbgInfo,dbgLines);
         }

         cc65_free_spaninfo(dbgInfo,dbgSpans);
      }
   }
   return source_line;
}

int CCC65Interface::c64GetSourceLineFromAbsoluteAddress(uint32_t addr,uint32_t absAddr)
{
   const cc65_spaninfo* dbgSpans;
   const cc65_lineinfo* dbgLines;
   const cc65_segmentinfo* dbgSegments;
   int span;
   int line;
   int highestTypeMatch = 0;
   int indexOfHighestTypeMatch = -1;
   int source_line = -1;

   if ( dbgInfo )
   {
      // Get the span containing this virtual address.
      dbgSpans = cc65_span_byaddr(dbgInfo,addr);

      if ( dbgSpans )
      {
         for ( span = 0; span < dbgSpans->count; span++ )
         {
            if ( dbgSpans->data[span].line_count )
            {
               // Get the segment potentially containing this absolute address.
               dbgSegments = cc65_segment_byid(dbgInfo,dbgSpans->data[span].segment_id);

               // Get the line potentially containing this absolute address.
               dbgLines = cc65_line_byspan(dbgInfo,dbgSpans->data[span].span_id);

               if ( dbgSegments && (dbgSegments->count == 1) &&
                    dbgLines && (dbgLines->count > 0) )
               {
                  if ( (absAddr >= dbgSegments->data[0].segment_start) &&
                       (absAddr < (dbgSegments->data[0].segment_start+dbgSegments->data[0].segment_size)) )
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
               if ( dbgSegments )
               {
                  cc65_free_segmentinfo(dbgInfo,dbgSegments);
               }
               if ( dbgLines )
               {
                  cc65_free_lineinfo(dbgInfo,dbgLines);
               }
            }
         }
         if ( indexOfHighestTypeMatch >= 0 )
         {
            // Get the picked line.
            dbgLines = cc65_line_byid(dbgInfo,indexOfHighestTypeMatch);
            source_line = dbgLines->data[0].source_line;
            cc65_free_lineinfo(dbgInfo,dbgLines);
         }

         cc65_free_spaninfo(dbgInfo,dbgSpans);
      }
   }
   return source_line;
}

QString CCC65Interface::getSourceFileFromSymbol(QString symbol)
{
   const cc65_symbolinfo* dbgSymbols;
   const cc65_lineinfo* dbgLines;
   const cc65_sourceinfo* dbgSources;
   int sym;
   QString file = "";

   if ( dbgInfo )
   {
      dbgSymbols = cc65_symbol_byname(dbgInfo,symbol.toLatin1().constData());

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
            dbgLines = cc65_line_bysymdef(dbgInfo,dbgSymbols->data[sym].symbol_id);

            if ( dbgLines && (dbgLines->count == 1) )
            {
               dbgSources = cc65_source_byid(dbgInfo,dbgLines->data[0].source_id);

               if ( dbgSources && (dbgSources->count == 1) )
               {
                  file = dbgSources->data[0].source_name;
               }

               if ( dbgSources )
               {
                  cc65_free_sourceinfo(dbgInfo,dbgSources);
               }
            }

            if ( dbgLines )
            {
               cc65_free_lineinfo(dbgInfo,dbgLines);
            }
         }

         cc65_free_symbolinfo(dbgInfo,dbgSymbols);
      }
   }

   return QDir::fromNativeSeparators(file);
}

int CCC65Interface::getSourceLineFromFileAndSymbol(QString file,QString symbol)
{
   const cc65_symbolinfo* dbgSymbols;
   const cc65_lineinfo* dbgLines;
   const cc65_sourceinfo* dbgSources;
   int sym;
   int source_line = -1;

   if ( dbgInfo )
   {
      dbgSymbols = cc65_symbol_byname(dbgInfo,symbol.toLatin1().constData());

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
            dbgLines = cc65_line_bysymdef(dbgInfo,dbgSymbols->data[sym].symbol_id);

            if ( dbgLines && (dbgLines->count == 1) )
            {
               dbgSources = cc65_source_byid(dbgInfo,dbgLines->data[0].source_id);

               if ( dbgSources && (dbgSources->count == 1) )
               {
                  if ( fileNamesAreIdentical(dbgSources->data[0].source_name,file) )
                  {
                     source_line = dbgLines->data[0].source_line;
                  }
               }

               if ( dbgSources )
               {
                  cc65_free_sourceinfo(dbgInfo,dbgSources);
               }
            }

            if ( dbgLines )
            {
               cc65_free_lineinfo(dbgInfo,dbgLines);
            }
         }

         cc65_free_symbolinfo(dbgInfo,dbgSymbols);
      }
   }

   return source_line;
}

int CCC65Interface::getLineMatchCount(QString file, int source_line)
{
   const cc65_sourceinfo* dbgSources;
   const cc65_lineinfo* dbgLines;
   const cc65_spaninfo* dbgSpans;
   int fidx;
   int count = 0;

   if ( dbgInfo )
   {
      // Get source ID from file.
      dbgSources = cc65_get_sourcelist(dbgInfo);

      // Get the appropriate file.
      for ( fidx = 0; fidx < dbgSources->count; fidx++ )
      {
         if ( fileNamesAreIdentical(dbgSources->data[fidx].source_name,file) )
         {
            break;
         }
      }

      dbgLines = cc65_line_bynumber(dbgInfo,dbgSources->data[fidx].source_id,source_line);
      if ( dbgLines && (dbgLines->count == 1) )
      {
         // Get the spans for this line.
         dbgSpans = cc65_span_byline(dbgInfo,dbgLines->data[0].line_id);

         if ( dbgSpans )
         {
            count = dbgSpans->count;

            cc65_free_spaninfo(dbgInfo,dbgSpans);
         }
      }

      if ( dbgLines )
      {
         cc65_free_lineinfo(dbgInfo,dbgLines);
      }

      cc65_free_sourceinfo(dbgInfo,dbgSources);
   }

   return count;
}

unsigned int CCC65Interface::getAddressFromFileAndLine(QString file,int source_line,int entry)
{
   const cc65_sourceinfo* dbgSources;
   const cc65_lineinfo* dbgLines;
   const cc65_spaninfo* dbgSpans;
   int highestTypeMatch = 0;
   int indexOfHighestTypeMatch = -1;
   int span;
   int fidx;
   int count = 0;
   int addr = -1;

   if ( dbgInfo )
   {
      dbgSources = cc65_get_sourcelist(dbgInfo);

      if ( dbgSources )
      {
         // Get the appropriate file.
         for ( fidx = 0; fidx < dbgSources->count; fidx++ )
         {
            if ( fileNamesAreIdentical(dbgSources->data[fidx].source_name,file) )
            {
               break;
            }
         }

         // Get line information from source file.
         dbgLines = cc65_line_bynumber(dbgInfo,dbgSources->data[fidx].source_id,source_line);

         if ( dbgLines && (dbgLines->count == 1) )
         {
            // Get the spans for this line.
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
                     highestTypeMatch = dbgLines->data[0].line_type;
                     indexOfHighestTypeMatch = dbgSpans->data[span].span_id;
                     break;
                  }

                  // Keep track of when to return data if asked to break early.
                  count++;
               }

               cc65_free_spaninfo(dbgInfo,dbgSpans);
            }
         }

         if ( dbgLines )
         {
            cc65_free_lineinfo(dbgInfo,dbgLines);
         }

         cc65_free_sourceinfo(dbgInfo,dbgSources);

         if ( indexOfHighestTypeMatch >= 0 )
         {
            // Get span information to return.
            dbgSpans = cc65_span_byid(dbgInfo,indexOfHighestTypeMatch);

            addr = dbgSpans->data[0].span_start;

            cc65_free_spaninfo(dbgInfo,dbgSpans);
         }
      }
   }
   return addr;
}

unsigned int CCC65Interface::getAbsoluteAddressFromFileAndLine(QString file,int source_line,int entry)
{
   // Dispatch to appropriate target machine handler.
   if ( !targetMachine.compare("nes",Qt::CaseInsensitive) )
   {
      return nesGetAbsoluteAddressFromFileAndLine(file,source_line,entry);
   }
   else if ( !targetMachine.compare("c64",Qt::CaseInsensitive) )
   {
      return c64GetAbsoluteAddressFromFileAndLine(file,source_line,entry);
   }
}

unsigned int CCC65Interface::nesGetAbsoluteAddressFromFileAndLine(QString file,int source_line,int entry)
{
   const cc65_sourceinfo* dbgSources;
   const cc65_lineinfo* dbgLines;
   const cc65_spaninfo* dbgSpans;
   const cc65_segmentinfo* dbgSegments;
   int highestTypeMatch = 0;
   int indexOfHighestTypeMatch = -1;
   int span;
   int fidx;
   int count = 0;
   int absAddr = -1;
   
   if ( dbgInfo )
   {
      dbgSources = cc65_get_sourcelist(dbgInfo);

      if ( dbgSources )
      {
         // Get the appropriate file.
         for ( fidx = 0; fidx < dbgSources->count; fidx++ )
         {
            if ( fileNamesAreIdentical(dbgSources->data[fidx].source_name,file) )
            {
               break;
            }
         }

         // Get line information from source file.
         dbgLines = cc65_line_bynumber(dbgInfo,dbgSources->data[fidx].source_id,source_line);

         if ( dbgLines && (dbgLines->count == 1) )
         {
            // Get the spans for this line.
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
                     highestTypeMatch = dbgLines->data[0].line_type;
                     indexOfHighestTypeMatch = dbgSpans->data[span].span_id;
                     break;
                  }

                  // Keep track of when to return data if asked to break early.
                  count++;
               }

               cc65_free_spaninfo(dbgInfo,dbgSpans);
            }
         }

         if ( dbgLines )
         {
            cc65_free_lineinfo(dbgInfo,dbgLines);
         }

         cc65_free_sourceinfo(dbgInfo,dbgSources);

         if ( indexOfHighestTypeMatch >= 0 )
         {
            // Get span information.
            dbgSpans = cc65_span_byid(dbgInfo,indexOfHighestTypeMatch);

            // Get segment information from span.
            dbgSegments = cc65_segment_byid(dbgInfo,dbgSpans->data[0].segment_id);
            if ( dbgSegments->data[0].output_name )
            {
               absAddr = dbgSegments->data[0].output_offs+(dbgSpans->data[0].span_start-dbgSegments->data[0].segment_start)-0x10;
            }
            else
            {
               absAddr = dbgSegments->data[0].output_offs+(dbgSpans->data[0].span_start-dbgSegments->data[0].segment_start);
            }

            cc65_free_spaninfo(dbgInfo,dbgSpans);
            cc65_free_segmentinfo(dbgInfo,dbgSegments);
         }
      }
   }
   return absAddr;
}

unsigned int CCC65Interface::c64GetAbsoluteAddressFromFileAndLine(QString file,int source_line,int entry)
{
   const cc65_sourceinfo* dbgSources;
   const cc65_lineinfo* dbgLines;
   const cc65_spaninfo* dbgSpans;
   const cc65_segmentinfo* dbgSegments;
   int highestTypeMatch = 0;
   int indexOfHighestTypeMatch = -1;
   int span;
   int fidx;
   int count = 0;
   int absAddr = -1;

   if ( dbgInfo )
   {
      dbgSources = cc65_get_sourcelist(dbgInfo);

      if ( dbgSources )
      {
         // Get the appropriate file.
         for ( fidx = 0; fidx < dbgSources->count; fidx++ )
         {
            if ( fileNamesAreIdentical(dbgSources->data[fidx].source_name,file) )
            {
               break;
            }
         }

         // Get line information from source file.
         dbgLines = cc65_line_bynumber(dbgInfo,dbgSources->data[fidx].source_id,source_line);

         if ( dbgLines && (dbgLines->count == 1) )
         {
            // Get the spans for this line.
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
                     highestTypeMatch = dbgLines->data[0].line_type;
                     indexOfHighestTypeMatch = dbgSpans->data[span].span_id;
                     break;
                  }

                  // Keep track of when to return data if asked to break early.
                  count++;
               }

               cc65_free_spaninfo(dbgInfo,dbgSpans);
            }
         }

         if ( dbgLines )
         {
            cc65_free_lineinfo(dbgInfo,dbgLines);
         }

         cc65_free_sourceinfo(dbgInfo,dbgSources);

         if ( indexOfHighestTypeMatch >= 0 )
         {
            // Get span information.
            dbgSpans = cc65_span_byid(dbgInfo,indexOfHighestTypeMatch);

            // Get segment information from span.
            dbgSegments = cc65_segment_byid(dbgInfo,dbgSpans->data[0].segment_id);
            absAddr = dbgSegments->data[0].segment_start+(dbgSpans->data[0].span_start-dbgSegments->data[0].segment_start);

            cc65_free_spaninfo(dbgInfo,dbgSpans);
            cc65_free_segmentinfo(dbgInfo,dbgSegments);
         }
      }
   }
   return absAddr;
}

unsigned int CCC65Interface::getEndAddressFromAbsoluteAddress(uint32_t addr,uint32_t absAddr)
{
   // Dispatch to appropriate target machine handler.
   if ( !targetMachine.compare("nes",Qt::CaseInsensitive) )
   {
      return nesGetEndAddressFromAbsoluteAddress(addr,absAddr);
   }
   else if ( !targetMachine.compare("c64",Qt::CaseInsensitive) )
   {
      return c64GetEndAddressFromAbsoluteAddress(addr,absAddr);
   }
}

unsigned int CCC65Interface::nesGetEndAddressFromAbsoluteAddress(uint32_t addr,uint32_t absAddr)
{
   const cc65_lineinfo* dbgLines;
   const cc65_spaninfo* dbgSpans;
   const cc65_segmentinfo* dbgSegments;
   int      span;
   int      line;
   int highestTypeMatch = 0;
   int indexOfHighestTypeMatch = -1;
   int endAddr = -1;

   if ( dbgInfo )
   {
      dbgSpans = cc65_span_byaddr(dbgInfo,addr);

      if ( dbgSpans )
      {
         for ( span = 0; span < dbgSpans->count; span++ )
         {
            dbgSegments = cc65_segment_byid(dbgInfo,dbgSpans->data[span].segment_id);

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

            if ( dbgLines )
            {
               cc65_free_lineinfo(dbgInfo,dbgLines);
            }
            if ( dbgSegments )
            {
               cc65_free_segmentinfo(dbgInfo,dbgSegments);
            }
         }

         cc65_free_spaninfo(dbgInfo,dbgSpans);

         if ( indexOfHighestTypeMatch >= 0 )
         {
            dbgSpans = cc65_span_byid(dbgInfo,indexOfHighestTypeMatch);

            endAddr = dbgSpans->data[0].span_end;

            cc65_free_spaninfo(dbgInfo,dbgSpans);
         }
      }
   }

   return endAddr;
}

unsigned int CCC65Interface::c64GetEndAddressFromAbsoluteAddress(uint32_t addr,uint32_t absAddr)
{
   const cc65_lineinfo* dbgLines;
   const cc65_spaninfo* dbgSpans;
   const cc65_segmentinfo* dbgSegments;
   int      span;
   int      line;
   int highestTypeMatch = 0;
   int indexOfHighestTypeMatch = -1;
   int endAddr = -1;

   if ( dbgInfo )
   {
      dbgSpans = cc65_span_byaddr(dbgInfo,addr);

      if ( dbgSpans )
      {
         for ( span = 0; span < dbgSpans->count; span++ )
         {
            dbgSegments = cc65_segment_byid(dbgInfo,dbgSpans->data[span].segment_id);

            dbgLines = cc65_line_byspan(dbgInfo,dbgSpans->data[span].span_id);

            if ( dbgSegments && (dbgSegments->count == 1) &&
                 dbgLines && (dbgLines->count > 0) )
            {
               if ( (absAddr >= dbgSegments->data[0].segment_start+(dbgSpans->data[span].span_start-dbgSegments->data[0].segment_start)) &&
                    (absAddr <= dbgSegments->data[0].segment_start+(dbgSpans->data[span].span_end-dbgSegments->data[0].segment_start)) )
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

            if ( dbgLines )
            {
               cc65_free_lineinfo(dbgInfo,dbgLines);
            }
            if ( dbgSegments )
            {
               cc65_free_segmentinfo(dbgInfo,dbgSegments);
            }
         }

         cc65_free_spaninfo(dbgInfo,dbgSpans);

         if ( indexOfHighestTypeMatch >= 0 )
         {
            dbgSpans = cc65_span_byid(dbgInfo,indexOfHighestTypeMatch);

            endAddr = dbgSpans->data[0].span_end;

            cc65_free_spaninfo(dbgInfo,dbgSpans);
         }
      }
   }

   return endAddr;
}

bool CCC65Interface::isAbsoluteAddressAnOpcode(uint32_t absAddr)
{
   // Dispatch to appropriate target machine handler.
   if ( !targetMachine.compare("nes",Qt::CaseInsensitive) )
   {
      return nesIsAbsoluteAddressAnOpcode(absAddr);
   }
   else if ( !targetMachine.compare("c64",Qt::CaseInsensitive) )
   {
      return c64IsAbsoluteAddressAnOpcode(absAddr);
   }
   return false;
}

bool CCC65Interface::nesIsAbsoluteAddressAnOpcode(uint32_t absAddr)
{
   const cc65_spaninfo* dbgSpans;
   const cc65_segmentinfo* dbgSegments;
   uint32_t addr;
   int      span;
   bool     opcode = false;

   if ( dbgInfo )
   {
      // Make addresses for where code might be in PRG-ROM space.
      addr = (absAddr&MASK_8KB);
      for ( ; addr < MEM_64KB; addr += MEM_8KB )
      {
         dbgSpans = cc65_span_byaddr(dbgInfo,addr);

         if ( dbgSpans )
         {
            for ( span = 0; span < dbgSpans->count; span++ )
            {
               dbgSegments = cc65_segment_byid(dbgInfo,dbgSpans->data[span].segment_id);

               if ( dbgSegments && (dbgSegments->count == 1) )
               {
                  if ( dbgSegments->data[0].output_name )
                  {
                     if ( absAddr == dbgSegments->data[0].output_offs+(dbgSpans->data[span].span_start-dbgSegments->data[0].segment_start)-0x10 )
                     {
                        opcode = true;
                     }
                  }
                  else
                  {
                     if ( absAddr == dbgSegments->data[0].output_offs+(dbgSpans->data[span].span_start-dbgSegments->data[0].segment_start) )
                     {
                        opcode = true;
                     }
                  }
               }

               if ( dbgSegments )
               {
                  cc65_free_segmentinfo(dbgInfo,dbgSegments);
               }
            }

            cc65_free_spaninfo(dbgInfo,dbgSpans);
         }
      }
   }

   return opcode;
}

bool CCC65Interface::c64IsAbsoluteAddressAnOpcode(uint32_t absAddr)
{
   const cc65_spaninfo* dbgSpans;
   const cc65_segmentinfo* dbgSegments;
   uint32_t addr;
   int      span;
   bool     opcode = false;

   if ( dbgInfo )
   {
      // Make addresses for where code might be in PRG-ROM space.
      dbgSpans = cc65_span_byaddr(dbgInfo,absAddr);

      if ( dbgSpans )
      {
         for ( span = 0; span < dbgSpans->count; span++ )
         {
            dbgSegments = cc65_segment_byid(dbgInfo,dbgSpans->data[span].segment_id);

            if ( dbgSegments && (dbgSegments->count == 1) )
            {
               if ( absAddr == dbgSegments->data[0].segment_start+(dbgSpans->data[span].span_start-dbgSegments->data[0].segment_start) )
               {
                  opcode = true;
               }
            }

            if ( dbgSegments )
            {
               cc65_free_segmentinfo(dbgInfo,dbgSegments);
            }
         }

         cc65_free_spaninfo(dbgInfo,dbgSpans);
      }
   }

   return opcode;
}

bool CCC65Interface::isErrorOnLineOfFile(QString file,int source_line)
{
   QString errorLookup;
   bool    found = false;

   // Form error string key.
   errorLookup = QDir::fromNativeSeparators(file)+'('+QString::number(source_line)+"):";
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
   const cc65_symbolinfo* dbgSymbols;
   bool symbol = false;

   if ( dbgInfo )
   {
      dbgSymbols = cc65_symbol_byname(dbgInfo,string.toLatin1().constData());

      if ( dbgSymbols )
      {
         symbol = true;
      }

      cc65_free_symbolinfo(dbgInfo,dbgSymbols);
   }
   return symbol;
}
