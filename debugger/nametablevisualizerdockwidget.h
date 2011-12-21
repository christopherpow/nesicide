#ifndef NAMETABLEVISUALIZERDOCKWIDGET_H
#define NAMETABLEVISUALIZERDOCKWIDGET_H

#include "cdebuggerbase.h"

#include "cnametablepreviewrenderer.h"

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
   void resizeEvent(QResizeEvent* event);
   void mousePressEvent(QMouseEvent *event);
   void mouseMoveEvent(QMouseEvent *event);
   void wheelEvent(QWheelEvent *event);
   CNameTablePreviewRenderer* renderer;
   void updateScrollbars();

public slots:
   void renderData();

private slots:
   void on_showVisible_toggled(bool checked);
   void on_verticalScrollBar_valueChanged(int value);
   void on_horizontalScrollBar_valueChanged(int value);
   void on_zoomSlider_valueChanged(int value);

private:
   Ui::NameTableVisualizerDockWidget *ui;
   char* imgData;
   DebuggerUpdateThread* pThread;
   QPoint pressPos;
};

#endif // NAMETABLEVISUALIZERDOCKWIDGET_H


