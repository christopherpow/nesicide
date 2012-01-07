#ifndef EXECUTIONVISUALIZERDOCKWIDGET_H
#define EXECUTIONVISUALIZERDOCKWIDGET_H

#include "cdebuggerbase.h"

#include "panzoomrenderer.h"

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
   void keyPressEvent(QKeyEvent* event);
   void contextMenuEvent(QContextMenuEvent *event);

public slots:
   void renderData();

private slots:
   void on_actionRemove_Marker_triggered();
   void on_actionReset_Marker_Data_triggered();
   void on_tableView_activated(QModelIndex index);
   void on_tableView_clicked(QModelIndex index);
   void on_tableView_entered(QModelIndex index);
   void on_tableView_pressed(QModelIndex index);

signals:
   void breakpointsChanged();

private:
   Ui::ExecutionVisualizerDockWidget *ui;
   CExecutionMarkerDisplayModel *model;
   char* imgData;
   PanZoomRenderer* renderer;
   DebuggerUpdateThread* pThread;
   QPoint pressPos;
};

#endif // EXECUTIONVISUALIZERDOCKWIDGET_H
