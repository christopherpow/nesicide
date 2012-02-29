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

private:
   CProjectTreeViewModel* m_pTreeViewModel;
   CProjectTabWidget* m_pTarget;
protected:
   virtual void contextMenuEvent(QContextMenuEvent* event);
   void keyPressEvent ( QKeyEvent* e );

signals:
   void projectTreeView_openItem(IProjectTreeViewItem* item);
};

#endif // CPROJECTTREEVIEW_H
