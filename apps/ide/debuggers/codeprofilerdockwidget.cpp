#include "codeprofilerdockwidget.h"
#include "ui_codeprofilerdockwidget.h"

#include "nes_emulator_core.h"

#include "ccodedatalogger.h"

#include "cobjectregistry.h"
#include "main.h"

CodeProfilerDockWidget::CodeProfilerDockWidget(QWidget *parent) :
    CDebuggerBase(parent),
    ui(new Ui::CodeProfilerDockWidget)
{
   ui->setupUi(this);
   model = new CDebuggerCodeProfilerModel();

   ui->tableView->setModel(model);
   ui->tableView->resizeColumnsToContents();

   ui->tableView->sortByColumn(CodeProfilerCol_Calls,Qt::DescendingOrder);

#if defined(Q_OS_MAC) || defined(Q_OS_MACX) || defined(Q_OS_MAC64)
   ui->tableView->setFont(QFont("Monaco", 11));
#endif
#ifdef Q_OS_LINUX
   ui->tableView->setFont(QFont("Monospace", 10));
#endif
#ifdef Q_OS_WIN
   ui->tableView->setFont(QFont("Consolas", 11));
#endif

   QObject::connect(ui->tableView->horizontalHeader(),SIGNAL(sortIndicatorChanged(int,Qt::SortOrder)),model,SLOT(sort(int,Qt::SortOrder)));
   QObject::connect(model,SIGNAL(layoutChanged()),this,SLOT(updateUi()));
}

CodeProfilerDockWidget::~CodeProfilerDockWidget()
{
    delete ui;
}

void CodeProfilerDockWidget::updateTargetMachine(QString target)
{
   if ( target.compare("none") )
   {
      QObject* breakpointWatcher = CObjectRegistry::instance()->getObject("Breakpoint Watcher");
      QObject* emulator = CObjectRegistry::instance()->getObject("Emulator");

      QObject::connect(breakpointWatcher,SIGNAL(breakpointHit()),model,SLOT(update()));
      QObject::connect(emulator,SIGNAL(machineReady()),this,SLOT(on_clear_clicked()));
      QObject::connect(emulator,SIGNAL(emulatorReset()),model,SLOT(update()));
      QObject::connect(emulator,SIGNAL(emulatorPaused(bool)),model,SLOT(update()));
   }
}

void CodeProfilerDockWidget::showEvent(QShowEvent */*event*/)
{
   QObject* emulator = CObjectRegistry::instance()->getObject("Emulator");

   if ( emulator )
   {
      QObject::connect(emulator,SIGNAL(updateDebuggers()),model,SLOT(update()));
   }
   model->update();
}

void CodeProfilerDockWidget::hideEvent(QHideEvent */*event*/)
{
   QObject* emulator = CObjectRegistry::instance()->getObject("Emulator");

   if ( emulator )
   {
      QObject::disconnect(emulator,SIGNAL(updateDebuggers()),model,SLOT(update()));
   }
}

void CodeProfilerDockWidget::updateUi()
{
   ui->symbolsProfiled->setText(QString::number(model->getItems().count()));
}

void CodeProfilerDockWidget::on_tableView_doubleClicked(QModelIndex index)
{
   QString symbol = model->getItems().at(index.row()).symbol;
   QString file = model->getItems().at(index.row()).file;

   emit snapTo("SourceNavigatorFile,"+file);
   emit snapTo("SourceNavigatorSymbol,"+symbol);
}

void CodeProfilerDockWidget::on_clear_clicked()
{
   nesClearCodeDataLoggerDatabases();

   model->clear();
   model->update();
}
