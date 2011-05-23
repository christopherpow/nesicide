#include "symbolwatchdockwidget.h"
#include "ui_symbolwatchdockwidget.h"

enum
{
   SymbolWatchCol_Name,
   SymbolWatchCol_Value
};

SymbolWatchDockWidget::SymbolWatchDockWidget(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::SymbolWatchDockWidget)
{
    ui->setupUi(this);
    model = new CSymbolWatchModel();
    delegate = new CDebuggerNumericItemDelegate();

    ui->tableView->setModel(model);
    ui->tableView->setItemDelegateForColumn(SymbolWatchCol_Value,delegate);
}

SymbolWatchDockWidget::~SymbolWatchDockWidget()
{
    delete ui;
}
