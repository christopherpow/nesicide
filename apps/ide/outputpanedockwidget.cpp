#include "outputpanedockwidget.h"
#include "ui_outputpanedockwidget.h"

#include "cbuildertextlogger.h"

#include "main.h"

#include <QMenu>
#include <QFileDialog>

OutputPaneDockWidget::OutputPaneDockWidget(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::OutputPaneDockWidget)
{
   ui->setupUi(this);

   ui->outputTabWidget->setCurrentIndex(Output_General);

   // Capture mouse events for the sub edits.
   ui->compilerOutputTextEdit->viewport()->installEventFilter(this);
   ui->searchOutputTextEdit->viewport()->installEventFilter(this);
}

OutputPaneDockWidget::~OutputPaneDockWidget()
{
   delete ui;
}

bool OutputPaneDockWidget::eventFilter(QObject* object,QEvent* event)
{
   if ( object == ui->compilerOutputTextEdit->viewport() )
   {
      if ( event->type() == QEvent::MouseButtonDblClick )
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

            if ( !selection.contains(".exe") && // Get rid of pesky ld65.exe: Error: and ld65.exe: Warning: matches...
                 (selection.contains(": Error:") ||
                  selection.contains(": Warning:")) )
            {
               // Parse the error file and line number.
               errorParts = selection.split(":");
               file = errorParts.at(0);
               file = file.left(errorParts.at(0).indexOf('('));
               line = errorParts.at(0);
               line = line.right(errorParts.at(0).length()-file.length());
               line = line.mid(1,line.length()-2);

               emit snapTo("OutputPaneFile,"+file+","+line);
            }
         }
         return true;
      }
   }
   else if ( object == ui->searchOutputTextEdit->viewport() )
   {
      if ( event->type() == QEvent::MouseButtonDblClick )
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

               emit snapTo("OutputPaneFile,"+file+","+line);
            }
         }
         return true;
      }
   }
   return false;
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
