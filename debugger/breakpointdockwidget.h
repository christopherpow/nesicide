#ifndef BREAKPOINTDOCKWIDGET_H
#define BREAKPOINTDOCKWIDGET_H

#include <QDockWidget>

#include "cregisterdata.h"
#include "cbreakpointinfo.h"
#include "cbreakpointdisplaymodel.h"

namespace Ui {
   class BreakpointDockWidget;
}

class BreakpointDockWidget : public QDockWidget
{
   Q_OBJECT
   
public:
   explicit BreakpointDockWidget(QWidget *parent = 0);
   ~BreakpointDockWidget();

protected:
   void changeEvent(QEvent* e);
   void showEvent(QShowEvent* e);
   void contextMenuEvent(QContextMenuEvent* e);
   CBreakpointDisplayModel* model;

signals:
   void breakpointsChanged();

private:
   Ui::BreakpointDockWidget *ui;
   
public slots:
   void updateData();

private slots:
    void on_actionDisable_All_Breakpoints_triggered();
    void on_actionEnable_All_Breakpoints_triggered();
    void on_actionDisable_Breakpoint_triggered();
    void on_actionEnable_Breakpoint_triggered();
    void on_actionEdit_Breakpoint_triggered();
    void on_actionRemove_Breakpoint_triggered();
    void on_actionAdd_Breakpoint_triggered();
    void on_tableView_doubleClicked(QModelIndex index);
};

#endif // BREAKPOINTDOCKWIDGET_H
