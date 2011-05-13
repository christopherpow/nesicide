#include "outputpanedockwidget.h"
#include "ui_outputpanedockwidget.h"

#include "cbuildertextlogger.h"

#include "main.h"

#include <QMenu>
#include <QFileDialog>

OutputPaneDockWidget::OutputPaneDockWidget(QTabWidget* pTarget, QWidget *parent) :
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

   if ( ui->compilerOutputTextEdit->blockCount() > 1 )
   {
      textCursor.select(QTextCursor::LineUnderCursor);
      selection = textCursor.selectedText();
      ui->compilerOutputTextEdit->setTextCursor(textCursor);

      if ( selection.contains("error:") )
      {
         errorParts = selection.split(":");

         while ( iter.current() )
         {
            pSource = dynamic_cast<CSourceItem*>(iter.current());
            if ( pSource &&
                 (pSource->caption() == errorParts.at(0)) )
            {
               pSource->openItemEvent(m_pTarget);
               pSource->editor()->selectLine(errorParts.at(1).toInt(0,10));
               break;
            }
            iter.next();
         }
      }
   }
}
