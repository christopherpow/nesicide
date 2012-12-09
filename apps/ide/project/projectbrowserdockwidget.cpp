#include "projectbrowserdockwidget.h"
#include "ui_projectbrowserdockwidget.h"

#include "cnesicideproject.h"
#include "model/cprojectmodel.h"
#include "model/cfiltermodel.h"

#include "main.h"

//--------------------------------------------------------------------------------------
// Sorting functions
//--------------------------------------------------------------------------------------
struct SortTopLevelItems
{
   SortTopLevelItems(CProjectTreeWidget* widget, CProjectModel* model)
   {
      tree = widget;
      project = model;
   }

   bool operator()(const QTreeWidgetItem* i1, const QTreeWidgetItem* i2) const
   {
      // Sort order:
      // 1. Filters, sorted by internal filter id
      // 2. Items not in a filter, sorted by name.
      CFilterModel* filters = project->getFilterModel();

      // Get uuids.
      QUuid item1 = tree->getUuidOf(i1);
      QUuid item2 = tree->getUuidOf(i2);

      if (filters->isFilter(item1))
      {
         if (filters->isFilter(item2))
         {
            return filters->compare(item1, item2);
         }
         else
         {
            return true; // item1 < item2
         }
      }
      else if (filters->isFilter(item2))
      {
         return false; // item1 > item2
      }
      else
      {
         return i1->text(0) < i2->text(0);
      }
   }

   CProjectModel* project;
   CProjectTreeWidget* tree;
};

//--------------------------------------------------------------------------------------
// Class methods
//--------------------------------------------------------------------------------------
ProjectBrowserDockWidget::ProjectBrowserDockWidget(CProjectTabWidget* pTarget, QWidget *parent)
   : QDockWidget(parent), ui(new Ui::ProjectBrowserDockWidget), m_pProject(NULL)
{
   ui->setupUi(this);

   m_pTarget = pTarget;

   setProjectModel(NULL);

   // CPTODO: Implement listview.
   ui->projectListView->hide();

   // Respond to signals of our children.
   QObject::connect(ui->projectTreeWidget, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)),
                                     this, SLOT(openItemRequested(QTreeWidgetItem*,int)));

   QObject::connect(ui->projectTreeWidget, SIGNAL(customContextMenuRequested(QPoint)),
                                     this, SLOT(treeWidgetContextMenuRequested(QPoint)));
}

ProjectBrowserDockWidget::~ProjectBrowserDockWidget()
{
   delete ui;
}

void ProjectBrowserDockWidget::layoutChangedEvent()
{
   setProjectModel(m_pProject);
}

void ProjectBrowserDockWidget::enableNavigation()
{

}

void ProjectBrowserDockWidget::disableNavigation()
{

}

void ProjectBrowserDockWidget::on_projectTreeView_doubleClicked(const QModelIndex &index)
{
   //IProjectTreeViewItem* item = (IProjectTreeViewItem*)index.internalPointer();

   //item->openItemEvent(m_pTarget);
}


void ProjectBrowserDockWidget::setProjectModel(CProjectModel *model)
{
   if (m_pProject)
   {
      QObject::disconnect(m_pProject, SIGNAL(itemAdded(QUuid)), this, SLOT(projectTreeChanged(QUuid)));
      QObject::disconnect(m_pProject, SIGNAL(itemRemoved(QUuid)), this, SLOT(projectTreeChanged(QUuid)));
   }

   m_pProject = model;
   rebuildProjectTree();

   if (model)
   {
      QObject::connect(m_pProject, SIGNAL(itemAdded(QUuid)), this, SLOT(projectTreeChanged(QUuid)));
      QObject::connect(m_pProject, SIGNAL(itemRemoved(QUuid)), this, SLOT(projectTreeChanged(QUuid)));
   }
}

void ProjectBrowserDockWidget::itemOpened(QUuid uuid)
{
   // Add to list of open items
   //ui->openFilesTreeWidget->addItem(m_pProject, uuid, QUuid());
   setProjectModel(m_pProject);
}

void ProjectBrowserDockWidget::itemClosed(QUuid uuid)
{
   // Remove from list of open items
   //ui->openFilesTreeWidget->removeItem(uuid);
   setProjectModel(m_pProject);
}

void ProjectBrowserDockWidget::projectTreeChanged(QUuid uuid)
{
   rebuildProjectTree();
}

void ProjectBrowserDockWidget::openItemRequested(QTreeWidgetItem *item, int)
{
   // Translate event.
   QUuid uuid = ui->projectTreeWidget->getUuidOf(item);
   emit openUuidRequest(uuid);
}

void ProjectBrowserDockWidget::treeWidgetContextMenuRequested(QPoint pos)
{
   // Invoke context menu.
   //QUuid uuid = ui->projectTreeWidget->getUuidAt(pos);
   //QPoint screenPos = ui->projectTreeWidget->mapToGlobal(pos);
   //CProjectContextMenu contextMenu(this, screenPos, m_pProject);
   //m_pProject->visitDataItem(uuid, contextMenu);
}

void ProjectBrowserDockWidget::buildProjectTree()
{
   // Remove old tree.
   ui->projectTreeWidget->clear();

   // If no tree to construct, nothing more to do.
   if (m_pProject == NULL)
      return;

   // Create a node for each uuid.
   foreach(QUuid uuid, m_pProject->getUuids())
   {
      ui->projectTreeWidget->addItem(m_pProject, uuid, QUuid());
   }

   // Connect child items to CFilter parents.
   CFilterModel* pFilters = m_pProject->getFilterModel();
   foreach(QUuid filterUuid, pFilters->getUuids())
   {
      foreach(QUuid childUuid, pFilters->getFilteredItems(filterUuid))
      {
         ui->projectTreeWidget->setParent(childUuid, filterUuid);
      }
   }

   // Sort top level items: Take em, sort em, insert em in a stew.
   QList<QTreeWidgetItem*> topLevelItems;
   while(ui->projectTreeWidget->topLevelItemCount() > 0)
   {
      topLevelItems.append(ui->projectTreeWidget->takeTopLevelItem(0));
   }

   qSort(topLevelItems.begin(), topLevelItems.end(), SortTopLevelItems(ui->projectTreeWidget, m_pProject));

   ui->projectTreeWidget->addTopLevelItems(topLevelItems);

   // Recursively sort filter contents.
}

void ProjectBrowserDockWidget::rebuildProjectTree()
{
   QList<QUuid> expansionState = this->saveProjectTreeExpansionState();
   buildProjectTree();
   this->restoreProjectTreeExpansionState(expansionState);
}

QList<QUuid> ProjectBrowserDockWidget::saveProjectTreeExpansionState()
{
   QList<QUuid> expandedItems;

   QTreeWidgetItemIterator it(ui->projectTreeWidget);
   while (*it)
   {
      QTreeWidgetItem* item = *it;
      if (item->isExpanded())
      {
         expandedItems.append(ui->projectTreeWidget->getUuidOf(item));
      }
      ++it;
   }

   return expandedItems;
}

void ProjectBrowserDockWidget::restoreProjectTreeExpansionState(const QList<QUuid> &itemsToExpand)
{
   QTreeWidgetItemIterator it(ui->projectTreeWidget);
   while (*it)
   {
      QTreeWidgetItem* item = *it;
      QUuid uuid = ui->projectTreeWidget->getUuidOf(item);
      bool shouldBeExanded = itemsToExpand.indexOf(uuid) >= 0;
      item->setExpanded(shouldBeExanded);
      ++it;
   }
}

