#include "symbolwatchdockwidget.h"
#include "ui_symbolwatchdockwidget.h"

SymbolWatchDockWidget::SymbolWatchDockWidget(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::SymbolWatchDockWidget)
{
    ui->setupUi(this);
}

SymbolWatchDockWidget::~SymbolWatchDockWidget()
{
    delete ui;
}
