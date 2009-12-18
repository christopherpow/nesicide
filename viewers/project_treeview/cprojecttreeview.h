#ifndef CPROJECTTREEVIEW_H
#define CPROJECTTREEVIEW_H

#include <QTreeView>
#include <QContextMenuEvent>
#include "cprojecttreeviewmodel.h"

class CProjectTreeView : public QTreeView
{
public:
    CProjectTreeView(QWidget *parent = 0);
    QTabWidget *mdiTabWidget;
private:
    CProjectTreeViewModel *treeViewModel;
protected:
    virtual void contextMenuEvent(QContextMenuEvent *event);
    virtual void mouseDoubleClickEvent (QMouseEvent * event);
    void keyPressEvent ( QKeyEvent * e );
};

#endif // CPROJECTTREEVIEW_H
