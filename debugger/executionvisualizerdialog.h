#ifndef EXECUTIONVISUALIZERDIALOG_H
#define EXECUTIONVISUALIZERDIALOG_H

#include "cexecutionvisualizerrenderer.h"

#include <QDialog>

namespace Ui {
   class ExecutionVisualizerDialog;
}

class ExecutionVisualizerDialog : public QDialog {
   Q_OBJECT
public:
   ExecutionVisualizerDialog(QWidget *parent = 0);
   ~ExecutionVisualizerDialog();
   void updateScrollbars();

protected:
   CExecutionVisualizerRenderer *renderer;

protected:
   void showEvent(QShowEvent *event);
   void hideEvent(QHideEvent *event);
   void changeEvent(QEvent *e);
   void resizeEvent(QResizeEvent *event);
   void updateDisplay();

private:
   Ui::ExecutionVisualizerDialog *ui;
   char *imgData;

public slots:
   void renderData();

private slots:
   void on_verticalScrollBar_valueChanged(int value);
   void on_horizontalScrollBar_valueChanged(int value);
   void on_zoomSlider_valueChanged(int value);
};

#endif // EXECUTIONVISUALIZERDIALOG_H
