#include "outputpanedockwidget.h"
#include "ui_outputpanedockwidget.h"

#include "cbuildertextlogger.h"

#include "main.h"

#include <QMenu>
#include <QFileDialog>

OutputPaneDockWidget::OutputPaneDockWidget(CProjectTabWidget* pTarget, QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::OutputPaneDockWidget)
{
   ui->setupUi(this);

   m_pTarget = pTarget;

   ui->outputTabWidget->setCurrentIndex(Output_General);
}

OutputPaneDockWidget::~OutputPaneDockWidget()
{
   delete ui;
}

void OutputPaneDockWidget::showPane(int tab)
{
   ui->outputTabWidget->setCurrentIndex(tab);
}

void OutputPaneDockWidget::clearAllPanes()
{
   ui->generalOutputTextEdit->clear();
   ui->compilerOutputTextEdit->clear();
   ui->debuggerOutputTextEdit->clear();
   ui->searchOutputTextEdit->clear();
}

void OutputPaneDockWidget::clearPane(int tab)
{
   switch ( tab )
   {
   case Output_General:
      ui->generalOutputTextEdit->clear();
      break;
   case Output_Build:
      ui->compilerOutputTextEdit->clear();
      break;
   case Output_Debug:
      ui->debuggerOutputTextEdit->clear();
      break;
   case Output_Search:
      ui->searchOutputTextEdit->clear();
      break;
   }
}

void OutputPaneDockWidget::updateGeneralPane(QString text)
{
   ui->generalOutputTextEdit->appendHtml(text);
}

void OutputPaneDockWidget::updateBuildPane(QString text)
{
   ui->compilerOutputTextEdit->appendHtml(text);
}

void OutputPaneDockWidget::updateDebugPane(QString text)
{
   ui->debuggerOutputTextEdit->appendHtml(text);
}

void OutputPaneDockWidget::updateSearchPane(QString text)
{
   ui->searchOutputTextEdit->appendHtml(text);
}

void OutputPaneDockWidget::eraseGeneralPane()
{
   ui->generalOutputTextEdit->clear();
}

void OutputPaneDockWidget::eraseBuildPane()
{
   ui->compilerOutputTextEdit->clear();
}

void OutputPaneDockWidget::eraseDebugPane()
{
   ui->debuggerOutputTextEdit->clear();
}

void OutputPaneDockWidget::eraseSearchPane()
{
   ui->searchOutputTextEdit->clear();
}

void OutputPaneDockWidget::contextMenuEvent ( QContextMenuEvent* event )
{
   QMenu menu;
   QAction clear("Clear",0);
   QAction logToFile("Log to file",0);
   QAction* chosen;

   menu.addAction(&clear);
   menu.addAction(&logToFile);
   chosen = menu.exec(event->globalPos());

   if ( chosen == &clear )
   {
      switch ( ui->outputTabWidget->currentIndex() )
      {
      case Output_General:
         ui->generalOutputTextEdit->clear();
         break;
      case Output_Build:
         ui->compilerOutputTextEdit->clear();
         break;
      case Output_Debug:
         ui->debuggerOutputTextEdit->clear();
         break;
      case Output_Search:
         ui->searchOutputTextEdit->clear();
         break;
      }
   }

   if ( chosen == &logToFile )
   {
      QString fileName = QFileDialog::getSaveFileName(this, "Log Output", QDir::currentPath(), "Text Document (*.txt)");

      if (fileName.isEmpty())
      {
         return;
      }

      switch ( ui->outputTabWidget->currentIndex() )
      {
      case Output_General:
         ui->generalOutputTextEdit->clear();
         break;
      case Output_Build:
         ui->compilerOutputTextEdit->clear();
         break;
      case Output_Debug:
         ui->debuggerOutputTextEdit->clear();
         break;
      case Output_Search:
         ui->searchOutputTextEdit->clear();
         break;
      }
   }
}

void OutputPaneDockWidget::on_compilerOutputTextEdit_selectionChanged()
{
   QTextCursor textCursor = ui->compilerOutputTextEdit->textCursor();
   QString     selection;
   QStringList errorParts;
   QString     file;
   QString     line;

   if ( ui->compilerOutputTextEdit->blockCount() > 1 )
   {
      textCursor.select(QTextCursor::LineUnderCursor);
      selection = textCursor.selectedText();
      ui->compilerOutputTextEdit->setTextCursor(textCursor);

      if ( selection.contains(": Error:") )
      {
         // Parse the error file and line number.
         errorParts = selection.split(":");
         file = errorParts.at(0);
         file = file.left(errorParts.at(0).indexOf('('));
         line = errorParts.at(0);
         line = line.right(errorParts.at(0).length()-file.length());
         line = line.mid(1,line.length()-2);

         openFileSelectLine(file,line.toInt());
      }
   }
}

void OutputPaneDockWidget::on_searchOutputTextEdit_selectionChanged()
{
   QTextCursor textCursor = ui->searchOutputTextEdit->textCursor();
   QString     selection;
   QStringList searchParts;
   QString     file;
   QString     line;

   if ( ui->searchOutputTextEdit->blockCount() > 1 )
   {
      textCursor.select(QTextCursor::LineUnderCursor);
      selection = textCursor.selectedText();
      ui->searchOutputTextEdit->setTextCursor(textCursor);

      if ( selection.contains(QRegExp(":([1-9][0-9]*):")) )
      {
         // Parse the search file and line number.
         searchParts = selection.split(":");
         file = searchParts.at(0);
         line = searchParts.at(1);

         openFileSelectLine(file,line.toInt());
      }
   }
}

void OutputPaneDockWidget::openFileSelectLine(QString file,int line)
{
   IProjectTreeViewItemIterator iter(nesicideProject->getProject()->getSources());
   CSourceItem* pSource;
   QDir dir;
   QDir projectDir = QDir::currentPath();
   QString fileName;
   QFile fileIn;
   bool        found = false;

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
         editor->highlightLine(line);
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
               pSource->editor()->highlightLine(line);
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
      editor->highlightLine(line);
   }
}
