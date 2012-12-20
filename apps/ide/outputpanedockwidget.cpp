#include "outputpanedockwidget.h"
#include "ui_outputpanedockwidget.h"

#include "cbuildertextlogger.h"
#include "cdockwidgetregistry.h"

#include "main.h"

#include <QMenu>
#include <QFileDialog>

OutputPaneDockWidget::OutputPaneDockWidget(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::OutputPaneDockWidget)
{
   QDockWidget* search = dynamic_cast<QDockWidget*>(CDockWidgetRegistry::getWidget("Search"));

   ui->setupUi(this);

   ui->outputStackedWidget->setCurrentIndex(Output_General);

   // Capture mouse events for the sub edits.
   ui->compilerOutputTextEdit->viewport()->installEventFilter(this);
   ui->searchOutputTextEdit->viewport()->installEventFilter(this);

   // Create the status-bar widget.
   general = new QPushButton("General Information");
   searchResults = new QPushButton("Search Results");
   buildResults = new QPushButton("Compile Output");
   debugInfo = new QPushButton("Debug Information");

   general->setStyleSheet("QPushButton { background: #A0A0A0 }");
   searchResults->setStyleSheet("QPushButton { background: #A0A0A0 }");
   buildResults->setStyleSheet("QPushButton { background: #A0A0A0 }");
   debugInfo->setStyleSheet("QPushButton { background: #A0A0A0 }");

   general->setCheckable(true);
   searchResults->setCheckable(true);
   buildResults->setCheckable(true);
   debugInfo->setCheckable(true);

   general->setMinimumWidth(150);
   searchResults->setMinimumWidth(150);
   buildResults->setMinimumWidth(150);
   debugInfo->setMinimumWidth(150);

   general->setMaximumWidth(150);
   searchResults->setMaximumWidth(150);
   buildResults->setMaximumWidth(150);
   debugInfo->setMaximumWidth(150);

   ui->searchInput->layout()->addWidget(search->widget());

   QObject::connect(general,SIGNAL(clicked()),this,SLOT(showGeneralPane()));
   QObject::connect(searchResults,SIGNAL(clicked()),this,SLOT(showSearchPane()));
   QObject::connect(buildResults,SIGNAL(clicked()),this,SLOT(showBuildPane()));
   QObject::connect(debugInfo,SIGNAL(clicked()),this,SLOT(showDebugPane()));
   QObject::connect(this,SIGNAL(visibilityChanged(bool)),this,SLOT(handleVisibilityChanged(bool)));
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

void OutputPaneDockWidget::initialize()
{
   emit addPermanentStatusBarWidget(general);
   emit addPermanentStatusBarWidget(buildResults);
   emit addPermanentStatusBarWidget(debugInfo);
   emit addPermanentStatusBarWidget(searchResults);
   general->setChecked(false);
   buildResults->setChecked(false);
   debugInfo->setChecked(false);
   searchResults->setChecked(false);
   ui->searchStackedWidget->setCurrentWidget(ui->searchInput);
}

void OutputPaneDockWidget::searcher_searchDone(int results)
{
   if ( results )
   {
      QString text = "Search Results: ";
      text += QString::number(results);
      searchResults->setStyleSheet("QPushButton { background: #80FF80 }");
      searchResults->setText(text);
   }
   else
   {
      searchResults->setStyleSheet("QPushButton { background: #A0A0A0 }");
      searchResults->setText("Search Results");
   }
   ui->searchStackedWidget->setCurrentWidget(ui->searchOutput);
}

void OutputPaneDockWidget::compiler_compileStarted()
{
   buildResults->setText("Compiling...");
   buildResults->setStyleSheet("QPushButton { background: #A0A0A0 }");
}

void OutputPaneDockWidget::compiler_compileDone(bool ok)
{
   buildResults->setText("Compile Results");
   if ( ok )
   {
      buildResults->setStyleSheet("QPushButton { background: #80FF80 }");
   }
   else
   {
      buildResults->setStyleSheet("QPushButton { background: #FF8080 }");
   }
}

void OutputPaneDockWidget::compiler_cleanStarted()
{
   buildResults->setText("Cleaning...");
   buildResults->setStyleSheet("QPushButton { background: #A0A0A0 }");
}

void OutputPaneDockWidget::compiler_cleanDone(bool ok)
{
   buildResults->setText("Compile Results");
   buildResults->setStyleSheet("QPushButton { background: #A0A0A0 }");
}

void OutputPaneDockWidget::showGeneralPane()
{
   if ( general->isChecked() )
   {
      show();
      ui->outputStackedWidget->setCurrentIndex(Output_General);
   }
   else
   {
      hide();
   }
   buildResults->setChecked(false);
   searchResults->setChecked(false);
   debugInfo->setChecked(false);
}

void OutputPaneDockWidget::showBuildPane()
{
   if ( buildResults->isChecked() )
   {
      show();
      ui->outputStackedWidget->setCurrentIndex(Output_Build);
   }
   else
   {
      hide();
   }
   buildResults->setStyleSheet("QPushButton { background: #A0A0A0 }");
   general->setChecked(false);
   searchResults->setChecked(false);
   debugInfo->setChecked(false);
}

void OutputPaneDockWidget::showDebugPane()
{
   if ( debugInfo->isChecked() )
   {
      show();
      ui->outputStackedWidget->setCurrentIndex(Output_Debug);
   }
   else
   {
      hide();
   }
   general->setChecked(false);
   searchResults->setChecked(false);
   buildResults->setChecked(false);
}

void OutputPaneDockWidget::showSearchPane()
{
   if ( searchResults->isChecked() )
   {
      show();
      ui->outputStackedWidget->setCurrentIndex(Output_Search);
   }
   else
   {
      hide();
   }
   general->setChecked(false);
   debugInfo->setChecked(false);
   buildResults->setChecked(false);
}

void OutputPaneDockWidget::resetPane(int tab)
{
   switch ( tab )
   {
   case Output_General:
      // Nothing to do.
      break;
   case Output_Build:
      // Nothing to do.
      break;
   case Output_Debug:
      // Nothing to do.
      break;
   case Output_Search:
      searchResults->setStyleSheet("QPushButton { background: #A0A0A0 }");
      searchResults->setText("Search Results");
      ui->searchStackedWidget->setCurrentWidget(ui->searchInput);
      break;
   }
}

void OutputPaneDockWidget::showPane(int tab)
{
   ui->outputStackedWidget->setCurrentIndex(tab);
   if ( isVisible() )
   {
      switch ( tab )
      {
      case Output_General:
         general->setChecked(true);
         buildResults->setChecked(false);
         debugInfo->setChecked(false);
         searchResults->setChecked(false);
         break;
      case Output_Build:
         general->setChecked(false);
         buildResults->setChecked(true);
         debugInfo->setChecked(false);
         searchResults->setChecked(false);
         break;
      case Output_Debug:
         general->setChecked(false);
         buildResults->setChecked(false);
         debugInfo->setChecked(true);
         searchResults->setChecked(false);
         break;
      case Output_Search:
         general->setChecked(false);
         buildResults->setChecked(false);
         debugInfo->setChecked(false);
         searchResults->setChecked(true);
         break;
      }
   }
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
      switch ( ui->outputStackedWidget->currentIndex() )
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

      switch ( ui->outputStackedWidget->currentIndex() )
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

void OutputPaneDockWidget::handleVisibilityChanged(bool visible)
{
   if ( !visible )
   {
      general->setChecked(false);
      buildResults->setChecked(false);
      debugInfo->setChecked(false);
      searchResults->setChecked(false);
   }
}
