#include "sourcenavigator.h"
#include "ui_sourcenavigator.h"

#include "main.h"

#include "ccc65interface.h"

#include <QCompleter>

SourceNavigator::SourceNavigator(CProjectTabWidget* pTarget,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SourceNavigator)
{
    ui->setupUi(this);

    ui->files->setEnabled(false);
    ui->symbols->setEnabled(false);
    ui->symbols->completer()->setCompletionMode(QCompleter::PopupCompletion);

    QObject::connect(compiler,SIGNAL(compileDone(bool)),this,SLOT(compiler_compileDone(bool)));
    QObject::connect(emulator,SIGNAL(cartridgeLoaded()),this,SLOT(emulator_cartridgeLoaded()));
    QObject::connect(emulator,SIGNAL(emulatorPaused(bool)),this,SLOT(emulator_emulatorPaused(bool)));
    QObject::connect(emulator,SIGNAL(emulatorReset()),this,SLOT(emulator_emulatorPaused()));
    QObject::connect(breakpointWatcher,SIGNAL(breakpointHit()),this,SLOT(emulator_emulatorPaused()));

    m_pTarget = pTarget;
}

SourceNavigator::~SourceNavigator()
{
    delete ui;
}

void SourceNavigator::shutdown()
{
   ui->files->clear();
   ui->symbols->clear();
   ui->files->setEnabled(false);
   ui->symbols->setEnabled(false);
}

void SourceNavigator::updateFiles(bool doIt)
{
   ui->files->clear();
   ui->symbols->clear();

   if ( doIt )
   {
      QStringList files = CCC65Interface::getSourceFiles();
      ui->files->addItems(files);
      if ( files.count() )
      {
         updateSymbolsForFile(files.at(0));
      }
   }
   else
   {
      ui->files->clear();
      ui->symbols->clear();
   }

   ui->files->setEnabled(doIt);
   ui->symbols->setEnabled(doIt);
}

void SourceNavigator::updateSymbolsForFile(QString file)
{
   QStringList   symbols;

   blockSignals(true);
   ui->symbols->clear();

   symbols = CCC65Interface::getSymbolsForSourceFile(file);
   ui->symbols->addItems(symbols);

   blockSignals(false);
}

void SourceNavigator::emulator_cartridgeLoaded()
{
   blockSignals(true);
   updateFiles(true);
   blockSignals(false);
}

void SourceNavigator::emulator_emulatorPaused(bool show)
{
   IProjectTreeViewItemIterator iter(nesicideProject->getProject()->getSources());
   CSourceItem* pSource;
   QString file;
   int   linenumber;
   unsigned int addr;
   unsigned int absAddr;
   bool found = false;

   if ( show )
   {
      blockSignals(true);
      addr = nesGetCPUProgramCounterOfLastSync();
      absAddr = nesGetAbsoluteAddressFromAddress(addr);
      file = CCC65Interface::getSourceFileFromAbsoluteAddress(addr,absAddr);
      if ( !file.isEmpty() )
      {
         linenumber = CCC65Interface::getSourceLineFromAbsoluteAddress(addr,absAddr);
         ui->files->setCurrentIndex(ui->files->findText(file));
         on_files_activated(file);

         while ( iter.current() )
         {
            pSource = dynamic_cast<CSourceItem*>(iter.current());
            if ( pSource &&
                 (pSource->path() == file) )
            {
               pSource->editor()->showExecutionLine(linenumber);
               found = true;
            }
            else if ( pSource && pSource->editor() )
            {
               pSource->editor()->showExecutionLine(-1);
            }
            iter.next();
         }

         // Now search through open files that are not part of the project.
         for ( int tab = 0; tab < m_pTarget->count(); tab++ )
         {
            CodeEditorForm* editor = dynamic_cast<CodeEditorForm*>(m_pTarget->widget(tab));
            if ( editor &&
                 (editor->fileName() == file) )
            {
               editor->showExecutionLine(linenumber);
            }
            else if ( editor )
            {
               editor->showExecutionLine(-1);
            }
         }
      }
      blockSignals(false);
   }
}

void SourceNavigator::compiler_compileDone(bool bOk)
{
   blockSignals(true);
   updateFiles(bOk);
   blockSignals(false);
}

void SourceNavigator::on_files_activated(QString file)
{
   IProjectTreeViewItemIterator iter(nesicideProject->getProject()->getSources());
   CSourceItem* pSource;
   bool found = false;
   QDir projectDir = QDir::currentPath();
   QDir dir;
   QString fileName;
   QFile fileIn;

   // First check if the file is already open.
   int foundIdx = -1;
   for ( int tab = 0; tab < m_pTarget->count(); tab++ )
   {
      CodeEditorForm* editor = dynamic_cast<CodeEditorForm*>(m_pTarget->widget(tab));
      if ( editor &&
           editor->fileName() == file )
      {
         found = true;
         foundIdx = tab;
         m_pTarget->setCurrentWidget(editor);
         break;
      }
   }

   // File is not open, search the project.
   if ( !found )
   {
      while ( iter.current() )
      {
         pSource = dynamic_cast<CSourceItem*>(iter.current());
         if ( pSource )
         {
            if ( pSource->path() == file )
            {
               pSource->openItemEvent(m_pTarget);
               found = true;
               break;
            }
         }
         iter.next();
      }
   }

   if ( found )
   {
      // Nothing more to do, get outta here...
      return;
   }

   // If we got here we can't find the file in the project, search the project
   // directory.
   if ( !found )
   {
      dir = QDir::currentPath();
      fileName = dir.relativeFilePath(file);
      fileIn.setFileName(fileName);

      if ( fileIn.exists() )
      {
         found = true;
      }
   }

   // If we got here we might be looking for a source file that's part of a library.
   // Search the source paths...
   if ( !found )
   {
      QStringList sourcePaths = nesicideProject->getSourceSearchPaths();

      foreach ( QString searchDir, sourcePaths )
      {
         dir = searchDir;
         fileName = dir.filePath(file);
         fileIn.setFileName(fileName);

         if ( fileIn.exists() )
         {
            found = true;
            break;
         }
      }
   }

   // If we got here we can't find the damn thing, ask the user to help.
   if ( !found )
   {
      QString str;
      str.sprintf("Locate %s...",file.toAscii().constData());
      QString newDir = QFileDialog::getOpenFileName(0,str,QDir::currentPath());
      if ( !newDir.isEmpty() )
      {
         QFileInfo fileInfo(newDir);
         dir = projectDir.relativeFilePath(fileInfo.path());
         nesicideProject->addSourceSearchPath(dir.path());
         fileName = dir.filePath(file);
         fileIn.setFileName(fileName);

         if ( fileIn.exists() )
         {
            found = true;
         }
      }
   }

   // Try to open the file.
   if ( found && fileIn.open(QIODevice::ReadOnly|QIODevice::Text) )
   {
      CodeEditorForm* editor = new CodeEditorForm(fileIn.fileName(),QString(fileIn.readAll()));

      fileIn.close();

      m_pTarget->addTab(editor, fileIn.fileName());
      m_pTarget->setCurrentWidget(editor);
   }
}

void SourceNavigator::on_symbols_activated(QString symbol)
{
   QString file = CCC65Interface::getSourceFileFromSymbol(symbol);
   on_files_activated(file);
   emit snapTo("SourceNavigatorSymbol:"+symbol);
}
