#include "outputdialog.h"
#include "ui_outputdialog.h"

#include "cbuildertextlogger.h"

#include "main.h"

#include <QMenu>

OutputDialog::OutputDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::OutputDialog)
{
   ui->setupUi(this);
   ui->outputTabWidget->setCurrentIndex(0);
   generalTextLogger.setTextEditControl(ui->generalOutputTextEdit);
   buildTextLogger.setTextEditControl(ui->compilerOutputTextEdit);
   debugTextLogger.setTextEditControl(ui->debuggerOutputTextEdit);

   QObject::connect(breakpointWatcher, SIGNAL(breakpointHit()), this, SLOT(updateData()));
}

OutputDialog::~OutputDialog()
{
    delete ui;
}

void OutputDialog::setCurrentOutputTab(int tab)
{
   ui->outputTabWidget->setCurrentIndex(tab);
}

void OutputDialog::contextMenuEvent ( QContextMenuEvent *event )
{
    QMenu menu;
    QAction action("Clear",0);
    QAction* chosen;

    menu.addAction(&action);
    chosen = menu.exec(event->globalPos());
    if ( chosen == &action )
    {
      switch ( ui->outputTabWidget->currentIndex() )
      {
         case 0:
            generalTextLogger.clear();
         break;
         case 1:
            buildTextLogger.clear();
         break;
         case 2:
            debugTextLogger.clear();
         break;
      }
    }
}

void OutputDialog::updateData()
{
   debugTextLogger.update();
}
