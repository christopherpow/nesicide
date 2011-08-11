#include "sourcenavigator.h"
#include "ui_sourcenavigator.h"

#include "main.h"

#include "ccc65interface.h"

#include <QCompleter>

SourceNavigator::SourceNavigator(QWidget *parent) :
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
   QString file;
   int   linenumber;
   unsigned int addr;
   unsigned int absAddr;

   if ( show )
   {
      addr = nesGetCPUProgramCounterOfLastSync();
      absAddr = nesGetAbsoluteAddressFromAddress(addr);
      file = CCC65Interface::getSourceFileFromAbsoluteAddress(addr,absAddr);
      if ( !file.isEmpty() )
      {
         linenumber = CCC65Interface::getSourceLineFromAbsoluteAddress(addr,absAddr);
         emit snapTo("SourceNavigatorFile:"+file+","+QString::number(linenumber));
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
   emit snapTo("SourceNavigatorFile:"+file);
}

void SourceNavigator::on_symbols_activated(QString symbol)
{
   QString file = CCC65Interface::getSourceFileFromSymbol(symbol);
   emit snapTo("SourceNavigatorFile"+file);
   emit snapTo("SourceNavigatorSymbol:"+symbol);
}
