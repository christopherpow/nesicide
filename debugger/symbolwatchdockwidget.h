#ifndef SYMBOLWATCHDOCKWIDGET_H
#define SYMBOLWATCHDOCKWIDGET_H

#include <QDockWidget>

#include "csymbolwatchmodel.h"
#include "cdebuggernumericitemdelegate.h"

namespace Ui {
    class SymbolWatchDockWidget;
}

class SymbolWatchDockWidget : public QDockWidget
{
    Q_OBJECT

public:
    explicit SymbolWatchDockWidget(QWidget *parent = 0);
    ~SymbolWatchDockWidget();

private:
    Ui::SymbolWatchDockWidget *ui;
    CSymbolWatchModel* model;
    CDebuggerNumericItemDelegate* delegate;
};

#endif // SYMBOLWATCHDOCKWIDGET_H
