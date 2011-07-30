#ifndef CODEPROFILERDOCKWIDGET_H
#define CODEPROFILERDOCKWIDGET_H

#include "cdebuggerbase.h"

#include "cdebuggercodeprofilermodel.h"
#include "ixmlserializable.h"

namespace Ui {
    class CodeProfilerDockWidget;
}

class CodeProfilerDockWidget : public CDebuggerBase
{
    Q_OBJECT

public:
    explicit CodeProfilerDockWidget(QWidget *parent = 0);
    ~CodeProfilerDockWidget();

private:
    Ui::CodeProfilerDockWidget *ui;
    CDebuggerCodeProfilerModel *model;

private slots:
   void updateUi();
};

#endif // CODEPROFILERDOCKWIDGET_H
