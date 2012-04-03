#ifndef MEMORYINSPECTORDOCKWIDGET_H
#define MEMORYINSPECTORDOCKWIDGET_H

#include "cdebuggerbase.h"

#include "cdebuggermemorydisplaymodel.h"
#include "cdebuggernumericitemdelegate.h"
#include "cmemorydata.h"

namespace Ui {
    class MemoryInspectorDockWidget;
}

class MemoryInspectorDockWidget : public CDebuggerBase
{
   Q_OBJECT

public:
   explicit MemoryInspectorDockWidget(memDBFunc memDB, QWidget *parent = 0);
   virtual ~MemoryInspectorDockWidget();

protected:
   void showEvent(QShowEvent* e);
   void hideEvent(QHideEvent* e);
   void contextMenuEvent(QContextMenuEvent* e);
   void changeEvent(QEvent* e);

public slots:
   void updateMemory();
   void updateTargetMachine(QString target);

signals:
   void breakpointsChanged();

private:
   Ui::MemoryInspectorDockWidget *ui;
   CDebuggerMemoryDisplayModel* model;
   CDebuggerNumericItemDelegate* delegate;
   memDBFunc m_memDB;

private slots:
   void on_actionBreak_on_CPU_write_here_triggered();
   void on_actionBreak_on_CPU_read_here_triggered();
   void on_actionBreak_on_CPU_access_here_triggered();
};

#endif // MEMORYINSPECTORDOCKWIDGET_H
