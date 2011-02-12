#ifndef MEMORYINSPECTORDOCKWIDGET_H
#define MEMORYINSPECTORDOCKWIDGET_H

#include "cdebuggermemorydisplaymodel.h"
#include "cregisterdata.h"

#include <QDockWidget>

namespace Ui {
    class MemoryInspectorDockWidget;
}

class MemoryInspectorDockWidget : public QDockWidget
{
   Q_OBJECT

public:
   explicit MemoryInspectorDockWidget(eMemoryType display = eMemory_CPU, QWidget *parent = 0);
   ~MemoryInspectorDockWidget();

protected:
   void showEvent(QShowEvent* e);
   void contextMenuEvent(QContextMenuEvent* e);
   void changeEvent(QEvent* e);

public slots:
   void updateMemory();

signals:
   void breakpointsChanged();

private:
   Ui::MemoryInspectorDockWidget *ui;
   CDebuggerMemoryDisplayModel* model;
   eMemoryType m_display;

private slots:
   void on_actionBreak_on_CPU_write_here_triggered();
   void on_actionBreak_on_CPU_read_here_triggered();
   void on_actionBreak_on_CPU_access_here_triggered();
};

#endif // MEMORYINSPECTORDOCKWIDGET_H
