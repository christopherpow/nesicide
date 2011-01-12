#ifndef CPROJECTTREEVIEW_H
#define CPROJECTTREEVIEW_H

#include <QTabWidget>
#include <QTreeView>
#include <QContextMenuEvent>

#include "cprojecttreeviewmodel.h"
//#include "main.h"

class CProjectTreeView : public QTreeView
{
   Q_OBJECT
public:
   CProjectTreeView(QWidget* parent = 0);
   void setTarget(QTabWidget* pTarget) { m_pTarget = pTarget; }
private:
   CProjectTreeViewModel* m_pTreeViewModel;
   QTabWidget* m_pTarget;
protected:
   virtual void contextMenuEvent(QContextMenuEvent* event);
   virtual void mouseDoubleClickEvent (QMouseEvent* event);
   void keyPressEvent ( QKeyEvent* e );

signals:
   void projectTreeView_openItem(QString item);
};

#endif // CPROJECTTREEVIEW_H
