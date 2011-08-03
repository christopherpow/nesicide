#include "codeprofilerdockwidget.h"
#include "ui_codeprofilerdockwidget.h"

#include "emulator_core.h"

#include "ccodedatalogger.h"

#include "main.h"

CodeProfilerDockWidget::CodeProfilerDockWidget(CProjectTabWidget* pTarget, QWidget *parent) :
    CDebuggerBase(parent),
    ui(new Ui::CodeProfilerDockWidget)
{
   ui->setupUi(this);
   model = new CDebuggerCodeProfilerModel();

   m_pTarget = pTarget;

   ui->tableView->setModel(model);
   ui->tableView->resizeColumnsToContents();

   ui->tableView->sortByColumn(2,Qt::DescendingOrder);

   QObject::connect(emulator,SIGNAL(cartridgeLoaded()),model,SLOT(update()));
   QObject::connect(emulator,SIGNAL(emulatorReset()),model,SLOT(update()));
   QObject::connect(emulator,SIGNAL(emulatorPaused(bool)),model,SLOT(update()));
   QObject::connect(emulator,SIGNAL(updateDebuggers()),model,SLOT(update()));
   QObject::connect(breakpointWatcher,SIGNAL(breakpointHit()),model,SLOT(update()));
   QObject::connect(ui->tableView->horizontalHeader(),SIGNAL(sortIndicatorChanged(int,Qt::SortOrder)),model,SLOT(sort(int,Qt::SortOrder)));

   QObject::connect(model,SIGNAL(layoutChanged()),this,SLOT(updateUi()));
}

CodeProfilerDockWidget::~CodeProfilerDockWidget()
{
    delete ui;
}

void CodeProfilerDockWidget::updateUi()
{
   ui->symbolsProfiled->setText(QString::number(model->getItems().count()));
}

void CodeProfilerDockWidget::on_tableView_doubleClicked(QModelIndex index)
{
   IProjectTreeViewItemIterator iter(nesicideProject->getProject()->getSources());
   CSourceItem* pSource;
   bool found = false;
   QString symbol = model->getItems().at(index.row()).symbol;
   QString file = model->getItems().at(index.row()).file;

   while ( iter.current() )
   {
      pSource = dynamic_cast<CSourceItem*>(iter.current());
      if ( pSource )
      {
         if ( pSource->path() == file )
         {
            pSource->openItemEvent(m_pTarget);
            emit snapTo("SourceNavigatorFile:"+file);
            found = true;
            break;
         }
      }
      iter.next();
   }

   if ( !found )
   {
      // If we got here the file is not part of the project...lets open it anyway,
      // if it's not already open.
      int foundIdx = -1;
      for ( int tab = 0; tab < m_pTarget->count(); tab++ )
      {
         CodeEditorForm* editor = dynamic_cast<CodeEditorForm*>(m_pTarget->widget(tab));
         if ( editor &&
              editor->fileName() == file )
         {
            found = true;
            foundIdx = tab;
            emit snapTo("SourceNavigatorFile:"+file);
            break;
         }
      }
      if ( !found )
      {
         QDir dir(QDir::currentPath());
         QString fileName = dir.fromNativeSeparators(dir.filePath(file));
         QFile fileIn(fileName);

         if ( fileIn.exists() && fileIn.open(QIODevice::ReadOnly|QIODevice::Text) )
         {
            CodeEditorForm* editor = new CodeEditorForm(file,QString(fileIn.readAll()));

            fileIn.close();

            m_pTarget->addTab(editor, file);

            emit snapTo("SourceNavigatorFile:"+file);
         }
      }
      else
      {
         emit snapTo("SourceNavigatorFile:"+file);
      }
   }

   emit snapTo("SourceNavigatorSymbol:"+symbol);
}

void CodeProfilerDockWidget::on_clear_clicked()
{
   nesClearCodeDataLoggerDatabases();

   model->update();
}
