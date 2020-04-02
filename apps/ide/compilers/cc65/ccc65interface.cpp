#include "ccc65interface.h"

#include "cnesicideproject.h"
#include "iprojecttreeviewitem.h"

#include "dbg_cnes6502.h"

#include "model/projectsearcher.h"

#include "cbuildertextlogger.h"

#include "environmentsettingsdialog.h"

CCC65Interface *CCC65Interface::_instance = NULL;

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
   : dbgInfo(NULL),
     targetMachine("none"),
     process(NULL)
{
   qRegisterMetaType<QProcess::ExitStatus>("QProcess::ExitStatus");
   qRegisterMetaType<QProcess::ProcessError>("QProcess::ProcessError");
   qRegisterMetaType<QProcess::ProcessState>("QProcess::ProcessState");
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
   QDir                baseDir(QDir::currentPath());
   QStringList         extensions = EnvironmentSettingsDialog::sourceExtensionsForAssembly().split(" ", QString::SkipEmptyParts);
   QList<CSourceItem*> projectSources = ProjectSearcher::findItemsOfType<CSourceItem>(CNesicideProject::instance());
   QStringList         includedSources;

   // For each source code object, compile it.
   foreach ( CSourceItem* source, projectSources )
   {
      foreach ( QString extension, extensions )
      {
         if ( source->includeInBuild() &&
              source->path().endsWith(extension,Qt::CaseInsensitive) )
         {
            includedSources.append(baseDir.fromNativeSeparators(baseDir.relativeFilePath(source->path())));
         }
      }
   }

   return includedSources;
}

QStringList CCC65Interface::getCLanguageSourcesFromProject()
{
   QDir                baseDir(QDir::currentPath());
   QStringList         extensions = EnvironmentSettingsDialog::sourceExtensionsForC().split(" ", QString::SkipEmptyParts);
   QStringList         headerExtensions = EnvironmentSettingsDialog::headerExtensions().split(" ", QString::SkipEmptyParts);
   bool                add;
   QList<CSourceItem*> projectSources = ProjectSearcher::findItemsOfType<CSourceItem>(CNesicideProject::instance());
   QStringList         includedSources;

   // For each source code object, compile it.
   foreach ( CSourceItem* source, projectSources )
   {
      add = true;
      foreach ( QString extension, extensions )
      {
         if ( source->includeInBuild() &&
              source->path().endsWith(extension,Qt::CaseInsensitive) )
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
               includedSources.append(baseDir.fromNativeSeparators(baseDir.relativeFilePath(source->path())));
            }
         }
      }
   }

   return includedSources;
}

QStringList CCC65Interface::getCustomSourcesFromProject()
{
   QDir                baseDir(QDir::currentPath());
   QStringList         extensions = EnvironmentSettingsDialog::customExtensions().split(" ", QString::SkipEmptyParts);
   QStringList         headerExtensions = EnvironmentSettingsDialog::headerExtensions().split(" ", QString::SkipEmptyParts);
   bool                add;
   QList<CSourceItem*> projectSources = ProjectSearcher::findItemsOfType<CSourceItem>(CNesicideProject::instance());
   QStringList         includedSources;

   // For each source code object, compile it.
   foreach ( CSourceItem* source, projectSources )
   {
      add = true;
      foreach ( QString extension, extensions )
      {
         if ( source->includeInBuild() &&
              source->path().endsWith(extension,Qt::CaseInsensitive) )
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
               includedSources.append(baseDir.fromNativeSeparators(baseDir.relativeFilePath(source->path())));
            }
         }
      }
   }

   return includedSources;
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
      makeFileContent.replace("<!project-name!>",CNesicideProject::instance()->getProjectOutputName());
      makeFileContent.replace("<!prg-rom-name!>",CNesicideProject::instance()->getProjectLinkerOutputName());
      makeFileContent.replace("<!linker-config!>",CNesicideProject::instance()->getLinkerConfigFile());
      makeFileContent.replace("<!compiler-flags!>",CNesicideProject::instance()->getCompilerAdditionalOptions());
      makeFileContent.replace("<!compiler-include-paths!>",CNesicideProject::instance()->getCompilerIncludePaths());
      makeFileContent.replace("<!compiler-defines!>",CNesicideProject::instance()->getCompilerDefinedSymbols());
      makeFileContent.replace("<!assembler-flags!>",CNesicideProject::instance()->getAssemblerAdditionalOptions());
      makeFileContent.replace("<!assembler-include-paths!>",CNesicideProject::instance()->getAssemblerIncludePaths());
      makeFileContent.replace("<!assembler-defines!>",CNesicideProject::instance()->getAssemblerDefinedSymbols());
      makeFileContent.replace("<!debug-file!>",CNesicideProject::instance()->getProjectDebugInfoName());
      makeFileContent.replace("<!linker-flags!>",CNesicideProject::instance()->getLinkerAdditionalOptions());
      makeFileContent.replace("<!source-dir!>",QDir::currentPath());
      makeFileContent.replace("<!object-dir!>",CNesicideProject::instance()->getProjectOutputBasePath());
      makeFileContent.replace("<!prg-dir!>",CNesicideProject::instance()->getProjectLinkerOutputBasePath());
      makeFileContent.replace("<!chr-dir!>",CNesicideProject::instance()->getProjectCHRROMOutputBasePath());
      makeFileContent.replace("<!clang-sources!>",getCLanguageSourcesFromProject().join(" "));
      makeFileContent.replace("<!asm-sources!>",getAssemblerSourcesFromProject().join(" "));
      makeFileContent.replace("<!custom-sources!>",getCustomSourcesFromProject().join(" "));
      makeFileContent.replace("<!target-rules!>",targetRules);
      makeFileContent.replace("<!linker-dependencies!>",CNesicideProject::instance()->getLinkerAdditionalDependencies());

      QString customRulesFiles;
      foreach ( QString customRuleFile,CNesicideProject::instance()->getMakefileCustomRuleFiles() )
      {
         customRulesFiles += "-include "+customRuleFile+"\r\n";
      }
      makeFileContent.replace("<!custom-rules!>",customRulesFiles);

      // Write the file to disk.
      makeFile.write(makeFileContent.toLatin1());

      makeFile.close();
      res.close();

      return true;
   }

   return false;
}

void CCC65Interface::process_errorOccurred(QProcess::ProcessError error)
{
   //buildTextLogger->write("<b><font color='gray' style='bold'>ENV::ERROR: "+QString::number(error)+"</font></b>");
}

void CCC65Interface::process_finished(int exitCode, QProcess::ExitStatus exitStatus)
{
   QString                      stdioStr;
   QStringList                  stdioList;

   //buildTextLogger->write("<b><font color='gray' style='bold'>ENV::EXIT "+QString::number(exitCode)+", "+QString::number(exitStatus)+"</font></b>");

   stdioStr = QString(process.readAllStandardError());
   stdioList = stdioStr.split(QRegExp("[\r\n]"),QString::SkipEmptyParts);
   errors.append(stdioList);
   foreach ( const QString& str, stdioList )
   {
      buildTextLogger->write("<font color='red'>"+str+"</font>");
   }
}

void CCC65Interface::process_readyReadStandardError()
{
   QString                      stdioStr;
   QStringList                  stdioList;

   if ( process.state() == QProcess::ProcessState::Running )
   {
      //buildTextLogger->write("<b><font color='gray' style='bold'>ENV::READ STDERR</font></b>");

      stdioStr = QString(process.readAllStandardError());
      stdioList = stdioStr.split(QRegExp("[\r\n]"),QString::SkipEmptyParts);
      errors.append(stdioList);
      foreach ( const QString& str, stdioList )
      {
         buildTextLogger->write("<font color='red'>"+str+"</font>");
      }
   }
}

void CCC65Interface::process_readyReadStandardOutput()
{
   QString     stdioStr;
   QStringList stdioList;

   if ( process.state() == QProcess::ProcessState::Running )
   {
      //buildTextLogger->write("<b><font color='gray' style='bold'>ENV::READ STDOUT</font></b>");

      stdioStr = QString(process.readAllStandardOutput());
      stdioList = stdioStr.split(QRegExp("[\r\n]"),QString::SkipEmptyParts);
      foreach ( const QString& str, stdioList )
      {
         buildTextLogger->write("<font color='blue'>"+str+"</font>");
      }
   }
}

void CCC65Interface::process_started()
{
   QString     stdioStr;
   QStringList stdioList;

   //buildTextLogger->write("<b><font color='gray' style='bold'>ENV::STARTED</font></b>");

   stdioStr = QString(process.readAllStandardOutput());
   stdioList = stdioStr.split(QRegExp("[\r\n]"),QString::SkipEmptyParts);
   foreach ( const QString& str, stdioList )
   {
      buildTextLogger->write("<font color='blue'>"+str+"</font>");
   }
}

void CCC65Interface::process_stateChanged(QProcess::ProcessState newState)
{
   //buildTextLogger->write("<b><font color='gray' style='bold'>ENV::STATE: "+QString::number(newState)+"</font></b>");
}

void CCC65Interface::clean()
{
   QStringList                  env = QProcess::systemEnvironment();
   QString                      invocationStr;
   QString                      stdioStr;
   QStringList                  stdioList;
   int                          exitCode;

   // Prevent overbuild
   protect.lock();

   process.moveToThread(QThread::currentThread());

   // Copy the system environment to the child process.
   process.setEnvironment(env);
   process.setWorkingDirectory(QDir::currentPath());
   QObject::connect(&process,SIGNAL(started()),this,SLOT(process_started()));
   QObject::connect(&process,SIGNAL(finished(int,QProcess::ExitStatus)),this,SLOT(process_finished(int,QProcess::ExitStatus)));
   QObject::connect(&process,SIGNAL(errorOccurred(QProcess::ProcessError)),this,SLOT(process_errorOccurred(QProcess::ProcessError)));
   QObject::connect(&process,SIGNAL(readyReadStandardError()),this,SLOT(process_readyReadStandardError()));
   QObject::connect(&process,SIGNAL(readyReadStandardOutput()),this,SLOT(process_readyReadStandardOutput()));
   QObject::connect(&process,SIGNAL(stateChanged(QProcess::ProcessState)),this,SLOT(process_stateChanged(QProcess::ProcessState)));

   // Clear the error storage.
   errors.clear();

   createMakefile();

   invocationStr = "make -f nesicide.mk clean";

   buildTextLogger->write(invocationStr);

   process.start(invocationStr);
   process.waitForFinished();

   QObject::disconnect(&process,SIGNAL(started()),this,SLOT(process_started()));
   QObject::disconnect(&process,SIGNAL(finished(int,QProcess::ExitStatus)),this,SLOT(process_finished(int,QProcess::ExitStatus)));
   QObject::disconnect(&process,SIGNAL(errorOccurred(QProcess::ProcessError)),this,SLOT(process_errorOccurred(QProcess::ProcessError)));
   QObject::disconnect(&process,SIGNAL(readyReadStandardError()),this,SLOT(process_readyReadStandardError()));
   QObject::disconnect(&process,SIGNAL(readyReadStandardOutput()),this,SLOT(process_readyReadStandardOutput()));
   QObject::disconnect(&process,SIGNAL(stateChanged(QProcess::ProcessState)),this,SLOT(process_stateChanged(QProcess::ProcessState)));

   // Prevent overbuild
   protect.unlock();

   return;
}

bool CCC65Interface::assemble()
{
   QStringList                  env = QProcess::systemEnvironment();
   QString                      invocationStr;
   QString                      stdioStr;
   QStringList                  stdioList;
   QDir                         outputDir(CNesicideProject::instance()->getProjectLinkerOutputBasePath());
   QString                      outputName;
   int                          exitCode;
   bool                         ok = true;

   // Prevent overbuild
   protect.lock();

   if ( CNesicideProject::instance()->getProjectLinkerOutputName().isEmpty() )
   {
      outputName = outputDir.fromNativeSeparators(outputDir.filePath(CNesicideProject::instance()->getProjectOutputName()+".prg"));
   }
   else
   {
      outputName = outputDir.fromNativeSeparators(outputDir.filePath(CNesicideProject::instance()->getProjectLinkerOutputName()));
   }
   buildTextLogger->write("<b>Building: "+outputName+"</b>");

   // Copy the system environment to the child process.
   process.setEnvironment(env);
   process.setWorkingDirectory(QDir::currentPath());
   QObject::connect(&process,SIGNAL(started()),this,SLOT(process_started()));
   QObject::connect(&process,SIGNAL(finished(int,QProcess::ExitStatus)),this,SLOT(process_finished(int,QProcess::ExitStatus)));
   QObject::connect(&process,SIGNAL(errorOccurred(QProcess::ProcessError)),this,SLOT(process_errorOccurred(QProcess::ProcessError)));
   QObject::connect(&process,SIGNAL(readyReadStandardError()),this,SLOT(process_readyReadStandardError()));
   QObject::connect(&process,SIGNAL(readyReadStandardOutput()),this,SLOT(process_readyReadStandardOutput()));
   QObject::connect(&process,SIGNAL(stateChanged(QProcess::ProcessState)),this,SLOT(process_stateChanged(QProcess::ProcessState)));

   // Clear the error storage.
   errors.clear();

   createMakefile();

   invocationStr = "make -f nesicide.mk all";

   buildTextLogger->write(invocationStr);

   process.start(invocationStr);
   process.waitForFinished();
   exitCode = process.exitCode();
   if ( exitCode )
   {
      ok = false;
   }

   QObject::disconnect(&process,SIGNAL(started()),this,SLOT(process_started()));
   QObject::disconnect(&process,SIGNAL(finished(int,QProcess::ExitStatus)),this,SLOT(process_finished(int,QProcess::ExitStatus)));
   QObject::disconnect(&process,SIGNAL(errorOccurred(QProcess::ProcessError)),this,SLOT(process_errorOccurred(QProcess::ProcessError)));
   QObject::disconnect(&process,SIGNAL(readyReadStandardError()),this,SLOT(process_readyReadStandardError()));
   QObject::disconnect(&process,SIGNAL(readyReadStandardOutput()),this,SLOT(process_readyReadStandardOutput()));
   QObject::disconnect(&process,SIGNAL(stateChanged(QProcess::ProcessState)),this,SLOT(process_stateChanged(QProcess::ProcessState)));

   // Prevent overbuild
   protect.unlock();

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

   if ( CNesicideProject::instance()->getProjectDebugInfoName().isEmpty() )
   {
      dbgInfoFile = dir.fromNativeSeparators(dir.relativeFilePath(CNesicideProject::instance()->getProjectOutputName()+".dbg"));
   }
   else
   {
      dbgInfoFile = dir.fromNativeSeparators(dir.relativeFilePath(CNesicideProject::instance()->getProjectDebugInfoName()));
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
   QProcessEnvironment          env = QProcessEnvironment::systemEnvironment();
   QString                      invocationStr;
   QString                      stdioStr;
   QStringList                  stdioList;
   int                          exitCode;
   QString outdated = "The NES ROM image is older than one or more of its source files.\n"
                      "Debuggers may not display correct information unless the NES ROM\n"
                      "is rebuilt.\n\n";
   bool ok = true;

   // Prevent overbuild
   protect.lock();

   // 'Build' is up-to-date if no sources present.
   if ( (getCLanguageSourcesFromProject().count() ||
        (getAssemblerSourcesFromProject().count())) )
   {
      // Copy the system environment to the child process.
      process.setProcessEnvironment(env);
      process.setWorkingDirectory(QDir::currentPath());
      QObject::connect(&process,SIGNAL(started()),this,SLOT(process_started()));
      QObject::connect(&process,SIGNAL(finished(int,QProcess::ExitStatus)),this,SLOT(process_finished(int,QProcess::ExitStatus)));
      QObject::connect(&process,SIGNAL(errorOccurred(QProcess::ProcessError)),this,SLOT(process_errorOccurred(QProcess::ProcessError)));
      QObject::connect(&process,SIGNAL(readyReadStandardError()),this,SLOT(process_readyReadStandardError()));
      QObject::connect(&process,SIGNAL(readyReadStandardOutput()),this,SLOT(process_readyReadStandardOutput()));
      QObject::connect(&process,SIGNAL(stateChanged(QProcess::ProcessState)),this,SLOT(process_stateChanged(QProcess::ProcessState)));

      // Clear the error storage.
      errors.clear();

      createMakefile();

      invocationStr = "make -f nesicide.mk -q all";

      process.start(invocationStr);
      process.waitForFinished();
      exitCode = process.exitCode();

      if ( exitCode == 1 )
      {
         QMessageBox::warning(NULL,"Consistency problem!",outdated);
         ok = false;
      }

      QObject::disconnect(&process,SIGNAL(started()),this,SLOT(process_started()));
      QObject::disconnect(&process,SIGNAL(finished(int,QProcess::ExitStatus)),this,SLOT(process_finished(int,QProcess::ExitStatus)));
      QObject::disconnect(&process,SIGNAL(errorOccurred(QProcess::ProcessError)),this,SLOT(process_errorOccurred(QProcess::ProcessError)));
      QObject::disconnect(&process,SIGNAL(readyReadStandardError()),this,SLOT(process_readyReadStandardError()));
      QObject::disconnect(&process,SIGNAL(readyReadStandardOutput()),this,SLOT(process_readyReadStandardOutput()));
      QObject::disconnect(&process,SIGNAL(stateChanged(QProcess::ProcessState)),this,SLOT(process_stateChanged(QProcess::ProcessState)));
   }

   // Prevent overbuild
   protect.unlock();

   return ok;
}

bool CCC65Interface::captureINESImage()
{
   QDir dir(QDir::currentPath());
   QString nesName;

   if ( CNesicideProject::instance()->getProjectCartridgeOutputName().isEmpty() )
   {
      nesName = dir.fromNativeSeparators(dir.relativeFilePath(CNesicideProject::instance()->getProjectOutputName()+".nes"));
   }
   else
   {
      nesName = dir.fromNativeSeparators(dir.relativeFilePath(CNesicideProject::instance()->getProjectCartridgeOutputName()));
   }

   buildTextLogger->write("<font color='black'><b>Reading NES executable from: "+nesName+"</b></font>");

   return CNesicideProject::instance()->createProjectFromRom(nesName,true);
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

unsigned int CCC65Interface::getSymbolPhysicalAddress(QString symbol, int index)
{
   // Dispatch to appropriate target machine handler.
   if ( !targetMachine.compare("nes",Qt::CaseInsensitive) )
   {
      return nesGetSymbolPhysicalAddress(symbol,index);
   }
   else if ( !targetMachine.compare("c64",Qt::CaseInsensitive) )
   {
      return c64GetSymbolPhysicalAddress(symbol,index);
   }
}

unsigned int CCC65Interface::nesGetSymbolPhysicalAddress(QString symbol, int index)
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

unsigned int CCC65Interface::c64GetSymbolPhysicalAddress(QString symbol, int index)
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

QString CCC65Interface::getSourceFileFromPhysicalAddress(uint32_t addr,uint32_t absAddr)
{
   // Dispatch to appropriate target machine handler.
   if ( !targetMachine.compare("nes",Qt::CaseInsensitive) )
   {
      return QDir::fromNativeSeparators(nesGetSourceFileFromPhysicalAddress(addr,absAddr));
   }
   else if ( !targetMachine.compare("c64",Qt::CaseInsensitive) )
   {
      return QDir::fromNativeSeparators(c64GetSourceFileFromPhysicalAddress(addr,absAddr));
   }
}

QString CCC65Interface::nesGetSourceFileFromPhysicalAddress(uint32_t addr,uint32_t absAddr)
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

QString CCC65Interface::c64GetSourceFileFromPhysicalAddress(uint32_t addr,uint32_t absAddr)
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

int CCC65Interface::getSourceLineFromPhysicalAddress(uint32_t addr,uint32_t absAddr)
{
   // Dispatch to appropriate target machine handler.
   if ( !targetMachine.compare("nes",Qt::CaseInsensitive) )
   {
      return nesGetSourceLineFromPhysicalAddress(addr,absAddr);
   }
   else if ( !targetMachine.compare("c64",Qt::CaseInsensitive) )
   {
      return c64GetSourceLineFromPhysicalAddress(addr,absAddr);
   }
}

int CCC65Interface::nesGetSourceLineFromPhysicalAddress(uint32_t addr,uint32_t absAddr)
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

int CCC65Interface::c64GetSourceLineFromPhysicalAddress(uint32_t addr,uint32_t absAddr)
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

unsigned int CCC65Interface::getPhysicalAddressFromFileAndLine(QString file,int source_line,int entry)
{
   // Dispatch to appropriate target machine handler.
   if ( !targetMachine.compare("nes",Qt::CaseInsensitive) )
   {
      return nesGetPhysicalAddressFromFileAndLine(file,source_line,entry);
   }
   else if ( !targetMachine.compare("c64",Qt::CaseInsensitive) )
   {
      return c64GetPhysicalAddressFromFileAndLine(file,source_line,entry);
   }
}

unsigned int CCC65Interface::nesGetPhysicalAddressFromFileAndLine(QString file,int source_line,int entry)
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

unsigned int CCC65Interface::c64GetPhysicalAddressFromFileAndLine(QString file,int source_line,int entry)
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

unsigned int CCC65Interface::getEndAddressFromPhysicalAddress(uint32_t addr,uint32_t absAddr)
{
   // Dispatch to appropriate target machine handler.
   if ( !targetMachine.compare("nes",Qt::CaseInsensitive) )
   {
      return nesGetEndAddressFromPhysicalAddress(addr,absAddr);
   }
   else if ( !targetMachine.compare("c64",Qt::CaseInsensitive) )
   {
      return c64GetEndAddressFromPhysicalAddress(addr,absAddr);
   }
}

unsigned int CCC65Interface::nesGetEndAddressFromPhysicalAddress(uint32_t addr,uint32_t absAddr)
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

unsigned int CCC65Interface::c64GetEndAddressFromPhysicalAddress(uint32_t addr,uint32_t absAddr)
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

bool CCC65Interface::isPhysicalAddressAnOpcode(uint32_t absAddr)
{
   // Dispatch to appropriate target machine handler.
   if ( !targetMachine.compare("nes",Qt::CaseInsensitive) )
   {
      return nesIsPhysicalAddressAnOpcode(absAddr);
   }
   else if ( !targetMachine.compare("c64",Qt::CaseInsensitive) )
   {
      return c64IsPhysicalAddressAnOpcode(absAddr);
   }
   return false;
}

bool CCC65Interface::nesIsPhysicalAddressAnOpcode(uint32_t absAddr)
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

bool CCC65Interface::c64IsPhysicalAddressAnOpcode(uint32_t absAddr)
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
