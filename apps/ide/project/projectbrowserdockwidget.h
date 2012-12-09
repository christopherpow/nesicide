#ifndef PROJECTBROWSERDOCKWIDGET_H
#define PROJECTBROWSERDOCKWIDGET_H

#include <QDockWidget>

#include "cprojecttabwidget.h"

#include "cprojecttreeviewmodel.h"
#include "cprojecttreeview.h"

#include "cprojecttreewidget.h"

class CProjectModel;

namespace Ui {
    class ProjectBrowserDockWidget;
}

class ProjectBrowserDockWidget : public QDockWidget
{
   Q_OBJECT
signals:
   void openUuidRequest(const QUuid& uuid); // -> tabWidget

public:
   explicit ProjectBrowserDockWidget(CProjectTabWidget* pTarget, QWidget *parent = 0);
   virtual ~ProjectBrowserDockWidget();

   void setProjectModel(CProjectModel* model);

   void layoutChangedEvent();
   void enableNavigation();
   void disableNavigation();

public slots:
   void itemOpened(QUuid uuid); // <- tabWidget
   void itemClosed(QUuid uuid); // <- tabWidget

   void projectTreeChanged(QUuid uuid);

private:
   Ui::ProjectBrowserDockWidget *ui;
   CProjectTabWidget* m_pTarget;
   CProjectModel* m_pProject;

   void buildProjectTree();
   void rebuildProjectTree();
   QList<QUuid> saveProjectTreeExpansionState();
   void restoreProjectTreeExpansionState(const QList<QUuid> &itemsToExpand);

private slots:
   void openItemRequested( QTreeWidgetItem * item, int column );
   void treeWidgetContextMenuRequested( QPoint pos );
};

#endif // PROJECTBROWSERDOCKWIDGET_H
