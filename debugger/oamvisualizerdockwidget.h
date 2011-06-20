#ifndef OAMVISUALIZERDOCKWIDGET_H
#define OAMVISUALIZERDOCKWIDGET_H

#include "cdebuggerbase.h"

#include "coampreviewrenderer.h"

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
   void resizeEvent(QResizeEvent* event);
   COAMPreviewRenderer* renderer;
   void updateScrollbars();

public slots:
   void renderData();

private slots:
   void on_showVisible_toggled(bool checked);
   void on_updateScanline_editingFinished();
   void on_verticalScrollBar_valueChanged(int value);
   void on_horizontalScrollBar_valueChanged(int value);
   void on_zoomSlider_valueChanged(int value);

private:
   Ui::OAMVisualizerDockWidget *ui;
   char* imgData;
};

#endif // OAMVISUALIZERDOCKWIDGET_H
