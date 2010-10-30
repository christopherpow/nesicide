#ifndef EXECUTIONVISUALIZER_H
#define EXECUTIONVISUALIZER_H

#include <QDockWidget>
#include <QFrame>
#include <QGridLayout>

#include "executionvisualizerdialog.h"

class ExecutionVisualizer : public QDockWidget
{
   Q_OBJECT
public:
   ExecutionVisualizer();
   virtual ~ExecutionVisualizer();

public slots:
   void showMe();

protected:
   ExecutionVisualizerDialog* dialog;
   QFrame* frame;
};

#endif // EXECUTIONVISUALIZER_H
