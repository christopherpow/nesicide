#ifndef OAMVISUALIZERDOCKWIDGET_H
#define OAMVISUALIZERDOCKWIDGET_H

#include "cdebuggerbase.h"

#include "panzoomrenderer.h"

#include "debuggerupdatethread.h"

namespace Ui {
   class OAMVisualizerDockWidget;
}

class OAMVisualizerDockWidget : public CDebuggerBase
{
   Q_OBJECT

public:
   explicit OAMVisualizerDockWidget(QWidget *parent = 0);
   virtual ~OAMVisualizerDockWidget();

protected:
   void showEvent(QShowEvent* event);
   void hideEvent(QHideEvent* event);
   void changeEvent(QEvent* e);

public slots:
   void renderData();
   void updateTargetMachine(QString target);

private slots:
   void on_showVisible_toggled(bool checked);
   void on_updateScanline_editingFinished();

private:
   Ui::OAMVisualizerDockWidget *ui;
   PanZoomRenderer* renderer;
   DebuggerUpdateThread* pThread;
   QPoint pressPos;
};

#endif // OAMVISUALIZERDOCKWIDGET_H
