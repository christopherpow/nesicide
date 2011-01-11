#ifndef CPROJECTTREEVIEW_H
#define CPROJECTTREEVIEW_H

#include <QTreeView>
#include <QContextMenuEvent>

#include "cprojecttreeviewmodel.h"
#include "main.h"

class CProjectTreeView : public QTreeView
{
   Q_OBJECT
public:
   CProjectTreeView(QWidget* parent = 0);
   void setTarget(QTabWidget* pTarget) { m_pTarget = pTarget; }
   QTabWidget* m_pTarget;
private:
   CProjectTreeViewModel* treeViewModel;
protected:
   virtual void contextMenuEvent(QContextMenuEvent* event);
   virtual void mouseDoubleClickEvent (QMouseEvent* event);
   void keyPressEvent ( QKeyEvent* e );

signals:
   void projectTreeView_openItem(QString item);
   
public slots:
   void fileNavigator_fileChanged(QString file);
   void fileNavigator_symbolChanged(QString file, QString symbol, int linenumber);
};

#endif // CPROJECTTREEVIEW_H
