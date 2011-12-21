#ifndef EXECUTIONVISUALIZERDOCKWIDGET_H
#define EXECUTIONVISUALIZERDOCKWIDGET_H

#include "cdebuggerbase.h"

#include "cexecutionvisualizerrenderer.h"

#include "cexecutionmarkerdisplaymodel.h"

#include "debuggerupdatethread.h"

#include "ixmlserializable.h"

namespace Ui {
   class ExecutionVisualizerDockWidget;
}

class ExecutionVisualizerDockWidget : public CDebuggerBase, public IXMLSerializable
{
   Q_OBJECT

public:
   explicit ExecutionVisualizerDockWidget(QWidget *parent = 0);
   virtual ~ExecutionVisualizerDockWidget();

   // IXMLSerializable interface
   virtual bool serialize(QDomDocument& doc, QDomNode& node);
   virtual bool deserialize(QDomDocument& doc, QDomNode& node, QString& errors);

protected:
   void showEvent(QShowEvent* event);
   void hideEvent(QHideEvent* event);
   void changeEvent(QEvent* event);
   void resizeEvent(QResizeEvent* event);
   void keyPressEvent(QKeyEvent* event);
   void mousePressEvent(QMouseEvent *event);
   void mouseMoveEvent(QMouseEvent *event);
   void wheelEvent(QWheelEvent *event);
   void contextMenuEvent(QContextMenuEvent *event);
   CExecutionVisualizerRenderer* renderer;
   void updateScrollbars();

public slots:
   void renderData();

private slots:
   void on_actionRemove_Marker_triggered();
   void on_actionReset_Marker_Data_triggered();
   void on_tableView_activated(QModelIndex index);
   void on_tableView_clicked(QModelIndex index);
   void on_tableView_entered(QModelIndex index);
   void on_tableView_pressed(QModelIndex index);
   void on_verticalScrollBar_valueChanged(int value);
   void on_horizontalScrollBar_valueChanged(int value);
   void on_zoomSlider_valueChanged(int value);

signals:
   void breakpointsChanged();

private:
   Ui::ExecutionVisualizerDockWidget *ui;
   CExecutionMarkerDisplayModel *model;
   char* imgData;
   DebuggerUpdateThread* pThread;
   QPoint pressPos;
};

#endif // EXECUTIONVISUALIZERDOCKWIDGET_H
