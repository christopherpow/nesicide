#ifndef BREAKPOINTDOCKWIDGET_H
#define BREAKPOINTDOCKWIDGET_H

#include "cdebuggerbase.h"

#include "cregisterdata.h"
#include "cbreakpointinfo.h"
#include "cbreakpointdisplaymodel.h"

#include "ixmlserializable.h"

namespace Ui {
   class BreakpointDockWidget;
}

class BreakpointDockWidget : public CDebuggerBase, public IXMLSerializable
{
   Q_OBJECT

public:
   BreakpointDockWidget(CBreakpointInfo* pBreakpoints,QWidget *parent = 0);
   virtual ~BreakpointDockWidget();

   // IXMLSerializable interface
   virtual bool serialize(QDomDocument& doc, QDomNode& node);
   virtual bool deserialize(QDomDocument& doc, QDomNode& node, QString& errors);

protected:
   void changeEvent(QEvent* e);
   void showEvent(QShowEvent* e);
   void contextMenuEvent(QContextMenuEvent* e);
   void dragEnterEvent(QDragEnterEvent *event);
   void dragMoveEvent(QDragMoveEvent *event);
   void dropEvent(QDropEvent *event);
   void keyPressEvent(QKeyEvent *event);

signals:
   void breakpointsChanged();

private:
   Ui::BreakpointDockWidget *ui;
   CBreakpointInfo* m_pBreakpoints;
   CBreakpointDisplayModel* model;
   bool eventFilter(QObject *obj, QEvent *event);

public slots:
   void updateData();
   void updateTargetMachine(QString target);

private slots:
    void on_tableView_pressed(QModelIndex index);
    void on_tableView_doubleClicked(QModelIndex index);
    void on_actionDisable_All_Breakpoints_triggered();
    void on_actionEnable_All_Breakpoints_triggered();
    void on_actionDisable_Breakpoint_triggered();
    void on_actionEnable_Breakpoint_triggered();
    void on_actionEdit_Breakpoint_triggered();
    void on_actionRemove_Breakpoint_triggered();
    void on_actionRemove_All_Breakpoints_triggered();
    void on_actionAdd_Breakpoint_triggered();
};

#endif // BREAKPOINTDOCKWIDGET_H
