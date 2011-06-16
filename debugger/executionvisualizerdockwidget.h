#ifndef EXECUTIONVISUALIZERDOCKWIDGET_H
#define EXECUTIONVISUALIZERDOCKWIDGET_H

#include "cdebuggerbase.h"

#include "cexecutionvisualizerrenderer.h"

#include "cexecutionmarkerdisplaymodel.h"

#include "ixmlserializable.h"

namespace Ui {
   class ExecutionVisualizerDockWidget;
}

class ExecutionVisualizerDockWidget : public CDebuggerBase, public IXMLSerializable
{
   Q_OBJECT

public:
   explicit ExecutionVisualizerDockWidget(QWidget *parent = 0);
   ~ExecutionVisualizerDockWidget();

   // IXMLSerializable interface
   virtual bool serialize(QDomDocument& doc, QDomNode& node);
   virtual bool deserialize(QDomDocument& doc, QDomNode& node, QString& errors);

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
   void on_tableView_activated(QModelIndex index);
   void on_tableView_clicked(QModelIndex index);
   void on_tableView_entered(QModelIndex index);
   void on_tableView_pressed(QModelIndex index);
   void on_verticalScrollBar_valueChanged(int value);
   void on_horizontalScrollBar_valueChanged(int value);
   void on_zoomSlider_valueChanged(int value);

private:
   Ui::ExecutionVisualizerDockWidget *ui;
   CExecutionMarkerDisplayModel *model;
   char* imgData;
};

#endif // EXECUTIONVISUALIZERDOCKWIDGET_H
