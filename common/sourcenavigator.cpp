#include "sourcenavigator.h"
#include "ui_sourcenavigator.h"

#include "main.h"

#include "pasm_lib.h"

SourceNavigator::SourceNavigator(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SourceNavigator)
{
    ui->setupUi(this);
    
    QObject::connect(compiler,SIGNAL(finished()),this,SLOT(compiler_compileDone()));
}

SourceNavigator::~SourceNavigator()
{
    delete ui;
}

void SourceNavigator::on_files_currentIndexChanged(QString file)
{
   if ( (!signalsBlocked()) && (ui->files->currentIndex() >= 0) )
   {
      updateSymbolsForFile(ui->files->currentIndex());
      emit fileNavigator_fileChanged(ui->files->currentText());
   }
   else
   {
      ui->symbols->clear();
   }
}

void SourceNavigator::on_symbols_currentIndexChanged(QString symbol)
{
   int linenumber;
   
   if ( (!signalsBlocked()) && (ui->symbols->currentIndex() >= 0) )
   {
      linenumber = pasm_get_symbol_linenum_by_name(symbol.toAscii().data());
      emit fileNavigator_symbolChanged(ui->files->currentText(), ui->symbols->currentText(), linenumber-1);
   }
}

void SourceNavigator::changeFile(QString fileName)
{
   if ( ui->files->currentText() != fileName )
   {
      int file = pasm_get_source_file_index_by_name(fileName.toAscii().data());
      blockSignals(true);
      
      ui->files->setCurrentIndex(file);
   
      updateSymbolsForFile(file);      
   
      blockSignals(false);
   }
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

void SourceNavigator::compiler_compileDone()
{
   int           file;
   
   blockSignals(true);
   ui->files->clear();
   ui->symbols->clear();
   if ( compiler->assembledOk() )
   {
      for ( file = 0; file < pasm_get_num_source_files(); file++ )
      {
         ui->files->addItem(pasm_get_source_file_name_by_index(file));
      }
      updateSymbolsForFile(0);      
   }
   blockSignals(false);
}

void SourceNavigator::projectTreeView_openItem(QString item)
{
   int file = pasm_get_source_file_index_by_name(item.toAscii().data());
   blockSignals(true);
   
   ui->files->setCurrentIndex(file);
   ui->symbols->clear();

   updateSymbolsForFile(file);      

   blockSignals(false);
}
