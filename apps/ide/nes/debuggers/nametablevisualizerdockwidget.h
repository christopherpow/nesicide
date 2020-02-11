#ifndef NAMETABLEVISUALIZERDOCKWIDGET_H
#define NAMETABLEVISUALIZERDOCKWIDGET_H

#include "cdebuggerbase.h"

#include "panzoomrenderer.h"

#include "debuggerupdatethread.h"

namespace Ui {
   class NameTableVisualizerDockWidget;
}

class NameTableVisualizerDockWidget : public CDebuggerBase
{
   Q_OBJECT

public:
   explicit NameTableVisualizerDockWidget(QWidget *parent = 0);
   virtual ~NameTableVisualizerDockWidget();

protected:
   void showEvent(QShowEvent* event);
   void hideEvent(QHideEvent* event);
   void changeEvent(QEvent* e);
   void updateInfoText(int x=-1,int y=-1);
   bool eventFilter(QObject *obj, QEvent *event);
   void renderer_enterEvent(QEvent* event);
   void renderer_leaveEvent(QEvent* event);
   void renderer_mouseMoveEvent(QMouseEvent *event);

public slots:
   void renderData();
   void updateTargetMachine(QString target);

private slots:
   void on_showVisible_toggled(bool checked);

private:
   Ui::NameTableVisualizerDockWidget *ui;
   PanZoomRenderer* renderer;
   DebuggerUpdateThread* pThread;
};

#endif // NAMETABLEVISUALIZERDOCKWIDGET_H


