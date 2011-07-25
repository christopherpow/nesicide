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
   IProjectTreeViewItemIterator iter(nesicideProject->getProject()->getSources());
   CSourceItem* pSource;
   QStringList errorParts;
   QString     errorFile;
   QString     errorLine;
   bool        found = false;

   if ( ui->compilerOutputTextEdit->blockCount() > 1 )
   {
      textCursor.select(QTextCursor::LineUnderCursor);
      selection = textCursor.selectedText();
      ui->compilerOutputTextEdit->setTextCursor(textCursor);

      if ( selection.contains(": Error:") )
      {
         // Parse the error file and line number.
         errorParts = selection.split(":");
         errorFile = errorParts.at(0);
         errorFile = errorFile.left(errorParts.at(0).indexOf('('));
         errorLine = errorParts.at(0);
         errorLine = errorLine.right(errorParts.at(0).length()-errorFile.length());
         errorLine = errorLine.mid(1,errorLine.length()-2);

         while ( iter.current() )
         {
            pSource = dynamic_cast<CSourceItem*>(iter.current());
            if ( pSource &&
                 (errorFile.contains(pSource->path())) )
            {
               pSource->openItemEvent(m_pTarget);
               pSource->editor()->highlightLine(errorLine.toInt());
               found = true;
               break;
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
                    editor->fileName() == errorFile )
               {
                  found = true;
                  foundIdx = tab;
                  editor->highlightLine(errorLine.toInt());
                  break;
               }
            }
            if ( !found )
            {
               QDir dir(QDir::currentPath());
               QString fileName = dir.fromNativeSeparators(dir.filePath(errorFile));
               QFile fileIn(fileName);

               if ( fileIn.exists() && fileIn.open(QIODevice::ReadOnly|QIODevice::Text) )
               {
                  CodeEditorForm* editor = new CodeEditorForm(errorFile,QString(fileIn.readAll()));

                  fileIn.close();

                  m_pTarget->addTab(editor, errorFile);
                  m_pTarget->setCurrentWidget(editor);
                  editor->highlightLine(errorLine.toInt());
               }
            }
            else
            {
               m_pTarget->setCurrentIndex(foundIdx);
            }
         }
      }
   }
}

void OutputPaneDockWidget::on_searchOutputTextEdit_selectionChanged()
{
   QTextCursor textCursor = ui->searchOutputTextEdit->textCursor();
   QString     selection;
   IProjectTreeViewItemIterator iter(nesicideProject->getProject()->getSources());
   CSourceItem* pSource;
   QStringList searchParts;
   QString     searchFile;
   QString     searchLine;
   bool        found = false;

   if ( ui->searchOutputTextEdit->blockCount() > 1 )
   {
      textCursor.select(QTextCursor::LineUnderCursor);
      selection = textCursor.selectedText();
      ui->searchOutputTextEdit->setTextCursor(textCursor);

      if ( selection.contains(QRegExp(":([1-9][0-9]*):")) )
      {
         // Parse the search file and line number.
         searchParts = selection.split(":");
         searchFile = searchParts.at(0);
         searchLine = searchParts.at(1);

         while ( iter.current() )
         {
            pSource = dynamic_cast<CSourceItem*>(iter.current());
            if ( pSource &&
                 (searchFile.contains(pSource->path())) )
            {
               pSource->openItemEvent(m_pTarget);
               pSource->editor()->highlightLine(searchLine.toInt());
               found = true;
               break;
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
                    editor->fileName() == searchFile )
               {
                  found = true;
                  foundIdx = tab;
                  editor->highlightLine(searchLine.toInt());
                  break;
               }
            }
            if ( !found )
            {
               QDir dir(QDir::currentPath());
               QString fileName = dir.fromNativeSeparators(dir.filePath(searchFile));
               QFile fileIn(fileName);

               if ( fileIn.exists() && fileIn.open(QIODevice::ReadOnly|QIODevice::Text) )
               {
                  CodeEditorForm* editor = new CodeEditorForm(searchFile,QString(fileIn.readAll()));

                  fileIn.close();

                  m_pTarget->addTab(editor, searchFile);
                  m_pTarget->setCurrentWidget(editor);
                  editor->highlightLine(searchLine.toInt());
               }
            }
            else
            {
               m_pTarget->setCurrentIndex(foundIdx);
            }
         }
      }
   }
}
