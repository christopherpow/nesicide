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
   void updateTargetMachine(QString target);

private slots:
   void on_actionRemove_Marker_triggered();
   void on_actionReset_Marker_Data_triggered();
   void tableView_currentChanged(QModelIndex index,QModelIndex);

signals:
   void breakpointsChanged();

private:
   Ui::ExecutionVisualizerDockWidget *ui;
   CExecutionMarkerDisplayModel *model;
   PanZoomRenderer* renderer;
   DebuggerUpdateThread* pThread;
};

#endif // EXECUTIONVISUALIZERDOCKWIDGET_H
