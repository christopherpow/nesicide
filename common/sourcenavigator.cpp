#include "sourcenavigator.h"
#include "ui_sourcenavigator.h"

#include "main.h"

#include "pasm_lib.h"

SourceNavigator::SourceNavigator(QTabWidget* pTarget,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SourceNavigator)
{
    ui->setupUi(this);
    
    ui->files->setEnabled(false);
    ui->symbols->setEnabled(false);
    
    QObject::connect(compiler,SIGNAL(finished()),this,SLOT(compiler_compileDone()));
    
    m_pTarget = pTarget;
}

SourceNavigator::~SourceNavigator()
{
    delete ui;
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
   else
   {
      ui->files->clear();
      ui->symbols->clear();
   }
   
   ui->files->setEnabled(compiler->assembledOk());
   ui->symbols->setEnabled(compiler->assembledOk());
   
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

void SourceNavigator::on_files_activated(QString file)
{
   IProjectTreeViewItemIterator iter(nesicideProject->getProject()->getSources());
   CSourceItem* pSource;

   while ( iter.current() )
   {
      pSource = dynamic_cast<CSourceItem*>(iter.current());
      if ( pSource && 
           (pSource->caption() == file) )
      {
         pSource->openItemEvent(m_pTarget);
         updateSymbolsForFile(ui->files->currentIndex());
         emit fileNavigator_fileChanged(ui->files->currentText());
         break;
      }
      iter.next();
   }
}

void SourceNavigator::on_symbols_activated(QString symbol)
{
   IProjectTreeViewItemIterator iter(nesicideProject->getProject()->getSources());
   CSourceItem* pSource;
   int linenumber = pasm_get_symbol_linenum_by_name(symbol.toAscii().data());

   while ( iter.current() )
   {
      pSource = dynamic_cast<CSourceItem*>(iter.current());
      if ( pSource &&
           (pSource->caption() == ui->files->currentText()) )
      {
         pSource->openItemEvent(m_pTarget);
         pSource->getEditor()->selectLine(linenumber);
         emit fileNavigator_symbolChanged(ui->symbols->currentText(),ui->symbols->currentText(),linenumber);
         break;
      }
      iter.next();
   }
}
