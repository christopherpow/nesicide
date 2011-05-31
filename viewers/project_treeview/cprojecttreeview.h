#ifndef CPROJECTTREEVIEW_H
#define CPROJECTTREEVIEW_H

#include <QTreeView>
#include <QContextMenuEvent>

#include "cprojecttreeviewmodel.h"
#include "cprojecttabwidget.h"

class CProjectTreeView : public QTreeView
{
   Q_OBJECT
public:
   CProjectTreeView(QWidget* parent = 0);
   void setTarget(CProjectTabWidget* pTarget) { m_pTarget = pTarget; }
private:
   CProjectTreeViewModel* m_pTreeViewModel;
   CProjectTabWidget* m_pTarget;
protected:
   virtual void contextMenuEvent(QContextMenuEvent* event);
   virtual void mouseDoubleClickEvent (QMouseEvent* event);
   void keyPressEvent ( QKeyEvent* e );

signals:
   void projectTreeView_openItem(IProjectTreeViewItem* item);
};

#endif // CPROJECTTREEVIEW_H
