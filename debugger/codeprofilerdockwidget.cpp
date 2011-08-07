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
   QDir dir;
   QDir projectDir = QDir::currentPath();
   QString fileName;
   QFile fileIn;
   QString symbol = model->getItems().at(index.row()).symbol;
   QString file = model->getItems().at(index.row()).file;

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
         emit snapTo("SourceNavigatorSymbol:"+symbol);
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
               emit snapTo("SourceNavigatorSymbol:"+symbol);
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
      dir.setPath(QDir::currentPath());
      fileName = dir.filePath(file);
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

   emit snapTo("SourceNavigatorSymbol:"+symbol);
}

void CodeProfilerDockWidget::on_clear_clicked()
{
   nesClearCodeDataLoggerDatabases();

   model->update();
}
