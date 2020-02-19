#include "sourcenavigator.h"
#include "ui_sourcenavigator.h"

#include "ccc65interface.h"

#include <QCompleter>

#include "cobjectregistry.h"

#include "nes_emulator_core.h"
#include "c64_emulator_core.h"

SourceNavigator::SourceNavigator(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SourceNavigator)
{
   ui->setupUi(this);

   ui->files->setEnabled(false);
   ui->symbols->setEnabled(false);
   ui->symbols->completer()->setCompletionMode(QCompleter::PopupCompletion);

   m_loadedTarget = "none";

   QObject* compiler = CObjectRegistry::getInstance()->getObject("Compiler");
   QObject* breakpointWatcher = CObjectRegistry::getInstance()->getObject("Breakpoint Watcher");
   QObject::connect(compiler,SIGNAL(compileDone(bool)),this,SLOT(compiler_compileDone(bool)));
   QObject::connect(breakpointWatcher,SIGNAL(breakpointHit()),this,SLOT(emulator_emulatorPaused()));
}

SourceNavigator::~SourceNavigator()
{
    delete ui;
}

void SourceNavigator::updateTargetMachine(QString target)
{
   QObject* emulator = CObjectRegistry::getInstance()->getObject("Emulator");

   m_loadedTarget = target;

   if ( emulator )
   {
      QObject::connect(emulator,SIGNAL(machineReady()),this,SLOT(emulator_machineReady()));
      QObject::connect(emulator,SIGNAL(emulatorPaused(bool)),this,SLOT(emulator_emulatorPaused(bool)));
      QObject::connect(emulator,SIGNAL(emulatorReset()),this,SLOT(emulator_emulatorPaused()));
   }
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

   QStringList files = CCC65Interface::getSourceFiles();
   ui->files->addItems(files);
   
   if ( doIt )
   {
      if ( files.count() )
      {
         updateSymbolsForFile(files.at(0));
      }
   }
   else
   {
      ui->symbols->clear();
   }

   ui->files->setEnabled(files.count());
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

void SourceNavigator::emulator_machineReady()
{
   blockSignals(true);
   updateFiles(true);
   blockSignals(false);
}

void SourceNavigator::emulator_emulatorPaused(bool show)
{
   QString file;
   int   linenumber;
   unsigned int addr;
   unsigned int absAddr;

   if ( show )
   {
      if ( !m_loadedTarget.compare("nes",Qt::CaseInsensitive) )
      {
         addr = nesGetCPUProgramCounterOfLastSync();
         absAddr = nesGetPhysicalAddressFromAddress(addr);
      }
      else if ( !m_loadedTarget.compare("c64",Qt::CaseInsensitive) )
      {
         addr = c64GetCPURegister(CPU_PC);
         absAddr = c64GetPhysicalAddressFromAddress(addr);
      }

      file = CCC65Interface::getSourceFileFromPhysicalAddress(addr,absAddr);
      if ( !file.isEmpty() )
      {
         linenumber = CCC65Interface::getSourceLineFromPhysicalAddress(addr,absAddr);
         emit snapTo("SourceNavigatorFile,"+file+","+QString::number(linenumber));
      }
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
   emit snapTo("SourceNavigatorFile,"+file);
}

void SourceNavigator::on_symbols_activated(QString symbol)
{
   QString file = CCC65Interface::getSourceFileFromSymbol(symbol);
   emit snapTo("SourceNavigatorFile,"+file);
   emit snapTo("SourceNavigatorSymbol,"+symbol);
}
