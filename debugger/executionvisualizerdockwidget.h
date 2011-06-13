#ifndef EXECUTIONVISUALIZERDOCKWIDGET_H
#define EXECUTIONVISUALIZERDOCKWIDGET_H

#include "cdebuggerbase.h"

#include "cexecutionvisualizerrenderer.h"

namespace Ui {
   class ExecutionVisualizerDockWidget;
}

class ExecutionVisualizerDockWidget : public CDebuggerBase
{
   Q_OBJECT

public:
   explicit ExecutionVisualizerDockWidget(QWidget *parent = 0);
   ~ExecutionVisualizerDockWidget();

protected:
   void showEvent(QShowEvent* event);
   void hideEvent(QHideEvent* event);
   void changeEvent(QEvent* e);
   void resizeEvent(QResizeEvent* event);
   CExecutionVisualizerRenderer* renderer;
   void updateScrollbars();

public slots:
   void renderData();

private slots:
   void on_verticalScrollBar_valueChanged(int value);
   void on_horizontalScrollBar_valueChanged(int value);
   void on_zoomSlider_valueChanged(int value);

private:
   Ui::ExecutionVisualizerDockWidget *ui;
   char* imgData;
};

#endif // EXECUTIONVISUALIZERDOCKWIDGET_H
