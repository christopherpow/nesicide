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
   generalTextLogger.setTextEditControl(ui->generalOutputTextEdit);
   buildTextLogger.setTextEditControl(ui->compilerOutputTextEdit);
   debugTextLogger.setTextEditControl(ui->debuggerOutputTextEdit);

   QObject::connect(breakpointWatcher, SIGNAL(breakpointHit()), this, SLOT(updateData()));
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
   generalTextLogger.clear();
   buildTextLogger.clear();
   debugTextLogger.clear();
}

void OutputPaneDockWidget::clearPane(int tab)
{
   switch ( tab )
   {
      case Output_General:
         generalTextLogger.clear();
         break;
      case Output_Build:
         buildTextLogger.clear();
         break;
      case Output_Debug:
         debugTextLogger.clear();
         break;
   }
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
            generalTextLogger.clear();
            break;
         case Output_Build:
            buildTextLogger.clear();
            break;
         case Output_Debug:
            debugTextLogger.clear();
            break;
      }
   }

   if ( chosen == &logToFile )
   {
      QString fileName = QFileDialog::getSaveFileName(this, 0, 0, "Text Document (*.txt)");

      if (fileName.isEmpty())
      {
         return;
      }

      switch ( ui->outputTabWidget->currentIndex() )
      {
         case Output_General:
            generalTextLogger.clear();
            break;
         case Output_Build:
            buildTextLogger.clear();
            break;
         case Output_Debug:
            debugTextLogger.clear();
            break;
      }
   }
}

void OutputPaneDockWidget::updateData()
{
   debugTextLogger.update();
}
