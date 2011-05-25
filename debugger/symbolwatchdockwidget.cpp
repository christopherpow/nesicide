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
    symbolDelegate = new CDebuggerSymbolDelegate();
    valueDelegate = new CDebuggerNumericItemDelegate();

    ui->tableView->setModel(model);
    ui->tableView->setItemDelegateForColumn(0,symbolDelegate);
    ui->tableView->setItemDelegateForColumn(1,valueDelegate);
}

SymbolWatchDockWidget::~SymbolWatchDockWidget()
{
    delete ui;
    delete model;
    delete valueDelegate;
    delete symbolDelegate;
}
