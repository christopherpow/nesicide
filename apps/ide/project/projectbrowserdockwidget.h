#ifndef PROJECTBROWSERDOCKWIDGET_H
#define PROJECTBROWSERDOCKWIDGET_H

#include <QDockWidget>
#include <QItemDelegate>

#include "cprojecttabwidget.h"
#include "cprojecttreewidget.h"

class CProjectModel;

namespace Ui {
    class ProjectBrowserDockWidget;
}

// Item delegate for row height spec
class ItemDelegate : public QItemDelegate
{
private:
    int m_iHeight;
public:
    ItemDelegate(QObject *poParent = Q_NULLPTR, int iHeight = -1) :
        QItemDelegate(poParent), m_iHeight(iHeight)
    {
    }

    void SetHeight(int iHeight)
    {
        m_iHeight = iHeight;
    }

    // Use this for setting tree item height.
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
    {
        QSize oSize = QItemDelegate::sizeHint(option, index);

        if (m_iHeight != -1)
        {
            // Set tree item height.
            oSize.setHeight(m_iHeight);
        }

        return oSize;
    }
};

class ProjectBrowserDockWidget : public QDockWidget
{
   Q_OBJECT
signals:
   void openUuidRequest(const QUuid& uuid);
   void closeUuidRequest(const QUuid& uuid);

public:
   explicit ProjectBrowserDockWidget(CProjectTabWidget* pTarget, QWidget *parent = 0);
   virtual ~ProjectBrowserDockWidget();

   void setProjectModel(CProjectModel* model);

   void layoutChangedEvent();
   void enableNavigation();
   void disableNavigation();

protected:
   bool eventFilter(QObject *watched, QEvent *event);

public slots:
   void itemOpened(QUuid uuid);
   void itemClosed(QUuid uuid);

   void itemOpened(int tabId);
   void itemSelected(int tabId);
   void itemClosed(int tabId);
   void itemModified(int tabId, bool modified);
   void itemSelectionChanged();

   void openItems_itemEntered(QTreeWidgetItem* item,int column);
   void openItems_itemClicked(QTreeWidgetItem* item,int column);

   void projectTreeChanged(QUuid uuid);

private:
   Ui::ProjectBrowserDockWidget *ui;
   CProjectTabWidget* m_pTarget;
   CProjectModel* m_pProjectModel;
   ItemDelegate* m_pItemDelegate;

   void buildProjectTree();
   void rebuildProjectTree();
   QList<QUuid> saveProjectTreeExpansionState();
   void restoreProjectTreeExpansionState(const QList<QUuid> &itemsToExpand);

private slots:
   void openItemRequested( QTreeWidgetItem * item, int column );
   void treeWidgetContextMenuRequested( QPoint pos );

   void openNewProjectItem(QUuid uuid);
};

#endif // PROJECTBROWSERDOCKWIDGET_H
