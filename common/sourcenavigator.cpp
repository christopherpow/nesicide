#include "sourcenavigator.h"
#include "ui_sourcenavigator.h"

#include "main.h"

#include "ccc65interface.h"
#include "cpasminterface.h"

#include "pasm_lib.h"

SourceNavigator::SourceNavigator(QTabWidget* pTarget,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SourceNavigator)
{
    ui->setupUi(this);

    ui->files->setEnabled(false);
    ui->symbols->setEnabled(false);

    QObject::connect(compiler,SIGNAL(compileDone(bool)),this,SLOT(compiler_compileDone(bool)));
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

void SourceNavigator::updateSymbolsForFile(int file)
{
   file_table*   pFile;
   symbol_table* pSymbol;
   int           symbol;

   blockSignals(true);
   ui->symbols->clear();
   pFile = pasm_get_source_file_by_index(file);
   for ( symbol = 0; symbol < pasm_get_num_symbols(); symbol++ )
   {
      if ( pasm_get_symbol_type_by_index(symbol) == symbol_label )
      {
         pSymbol = pasm_get_symbol_by_index(symbol);
         if ( pSymbol->ir &&
              (pSymbol->ir->file == pFile) )
         {
            ui->symbols->addItem(pasm_get_symbol_name_by_index(symbol));
         }
      }
   }
   blockSignals(false);
}

void SourceNavigator::emulator_emulatorPaused(bool show)
{
   IProjectTreeViewItemIterator iter(nesicideProject->getProject()->getSources());
   CSourceItem* pSource;
   char* file;
   int   linenumber;
   unsigned int absAddr;

   if ( show )
   {
      blockSignals(true);
      absAddr = nesGetAbsoluteAddressFromAddress(nesGetCPUProgramCounterOfLastSync());
      file = pasm_get_source_file_name_by_addr(absAddr);
      if ( file )
      {
         linenumber = pasm_get_source_linenum_by_absolute_addr(absAddr);
         on_files_activated(QString(file));

         while ( iter.current() )
         {
            pSource = dynamic_cast<CSourceItem*>(iter.current());
            if ( pSource &&
                 (pSource->caption() == file) )
            {
               pSource->getEditor()->selectLine(linenumber);
            }
            else if ( pSource && pSource->getEditor() )
            {
               pSource->getEditor()->selectLine(-1);
            }
            iter.next();
         }
      }
      blockSignals(false);
   }
}

void SourceNavigator::compiler_compileDone(bool bOk)
{
   QFileInfo fileInfo;

   blockSignals(true);
   ui->files->clear();
   ui->symbols->clear();

   if ( bOk )
   {
      QStringList files = CCC65Interface::getSourceFiles();
      foreach ( const QString& str, files )
      {
         fileInfo.setFile(str);
         ui->files->addItem(fileInfo.fileName());
      }
      updateSymbolsForFile(0);
   }
   else
   {
      ui->files->clear();
      ui->symbols->clear();
   }

   ui->files->setEnabled(bOk);
   ui->symbols->setEnabled(bOk);

   blockSignals(false);
}

void SourceNavigator::projectTreeView_openItem(QString item)
{
   int file = pasm_get_source_file_index_by_name(item.toAscii().constData());
   blockSignals(true);

   ui->files->setCurrentIndex(file);
   ui->symbols->clear();

   updateSymbolsForFile(file);

   blockSignals(false);
}

void SourceNavigator::on_files_activated(QString file)
{
   IProjectTreeViewItemIterator iter(nesicideProject->getProject()->getSources());
   CSourceItem* pSource;

   while ( iter.current() )
   {
      pSource = dynamic_cast<CSourceItem*>(iter.current());
      if ( pSource )
      {
         if ( pSource->name() == file )
         {
            pSource->openItemEvent(m_pTarget);
            updateSymbolsForFile(ui->files->currentIndex());
            emit fileNavigator_fileChanged(ui->files->currentText());
            break;
         }
      }
      iter.next();
   }
}

void SourceNavigator::on_symbols_activated(QString symbol)
{
   IProjectTreeViewItemIterator iter(nesicideProject->getProject()->getSources());
   CSourceItem* pSource;
   int          linenumber = pasm_get_symbol_linenum_by_name(symbol.toAscii().constData());

   while ( iter.current() )
   {
      pSource = dynamic_cast<CSourceItem*>(iter.current());
      if ( pSource &&
           (pSource->path() == ui->files->currentText()) )
      {
         pSource->openItemEvent(m_pTarget);
         pSource->getEditor()->selectLine(linenumber);
         emit fileNavigator_symbolChanged(ui->symbols->currentText(),ui->symbols->currentText(),linenumber);
         break;
      }
      iter.next();
   }
}
