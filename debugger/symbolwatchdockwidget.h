#ifndef SYMBOLWATCHDOCKWIDGET_H
#define SYMBOLWATCHDOCKWIDGET_H

#include <QDockWidget>

#include "csymbolwatchmodel.h"
#include "cdebuggernumericitemdelegate.h"
#include "cdebuggersymboldelegate.h"

namespace Ui {
   class SymbolWatchDockWidget;
}

class SymbolWatchDockWidget : public QDockWidget
{
   Q_OBJECT

public:
   explicit SymbolWatchDockWidget(QWidget *parent = 0);
   ~SymbolWatchDockWidget();

public slots:
   void updateSymbols();

private:
   Ui::SymbolWatchDockWidget *ui;
   CSymbolWatchModel* model;
   CDebuggerSymbolDelegate* symbolDelegate;
   CDebuggerNumericItemDelegate* valueDelegate;
};

#endif // SYMBOLWATCHDOCKWIDGET_H
