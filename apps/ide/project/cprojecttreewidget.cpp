
#include "cprojecttreewidget.h"

#include "model/cattributemodel.h"
#include "model/cbinaryfilemodel.h"
#include "model/ccartridgemodel.h"
#include "model/cfiltermodel.h"
#include "model/cgraphicsbankmodel.h"
#include "model/cprojectmodel.h"
#include "model/csourcefilemodel.h"
#include "model/ctilestampmodel.h"
#include "model/cmusicmodel.h"
#include "model/iuuidvisitor.h"

//--------------------------------------------------------------------------------------
// Helper classes
//--------------------------------------------------------------------------------------

// Extracts relevant data for tree node display.
class NodeDataReader : public IUuidVisitor
{
public:
   NodeDataReader(CProjectModel* project)
      : project(project)
   { }

   CProjectModel* project;
   QString name;
   QIcon   icon;
   QString toolTip;

   // Unknown item. Display uuid and a question mark as icon.
   virtual void visit(CUuid &d)
   {
      name    = d.uuid.toString();
      toolTip = QString();
      icon    = QIcon(); // TODO Question mark?
   }

   virtual void visit(CAttributeUuid &d)
   {
      CAttributeModel* model = project->getAttributeModel();
      name    = model->getName(d.uuid);
      toolTip = QString();
      icon    = QIcon(":/resources/RR_page_white.png");
   }

   virtual void visit(CBinaryFileUuid &d)
   {
      CBinaryFileModel* model = project->getBinaryFileModel();
      name    = model->getFileName(d.uuid);
      toolTip = QString();
      icon    = QIcon(":/resources/RR_page_white.png");
   }

   virtual void visit(CFilterUuid &d)
   {
      CFilterModel* model = project->getFilterModel();
      name    = model->getFilterName(d.uuid);
      toolTip = QString();
      icon    = QIcon(":/resources/folder_closed.png");
   }

   virtual void visit(CGraphicsBankUuid &d)
   {
      CGraphicsBankModel* model = project->getGraphicsBankModel();
      name    = model->getName(d.uuid);
      toolTip = QString();
      icon    = QIcon(":/resources/RR_graphics_bank.png");
   }

   virtual void visit(CSourceFileUuid &d)
   {
      CSourceFileModel* model = project->getSourceFileModel();
      name    = model->getFileName(d.uuid);
      toolTip = model->getRelativePath(d.uuid);
      icon    = QIcon(":/resources/RR_page_asm.png");
   }

   virtual void visit(CTileStampUuid &d)
   {
      CTileStampModel* model = project->getTileStampModel();
      name    = model->getName(d.uuid);
      toolTip = QString();
      icon    = QIcon(":/resources/RR_page_white.png");
   }

   virtual void visit(CMusicFileUuid &d)
   {
      CMusicModel* model = project->getMusicModel();
      name    = model->getName(d.uuid);
      toolTip = QString();
      icon    = QIcon(":/resources/RR_page_white.png");
   }

   virtual void visit(CChrRomUuid &d)
   {
      CCartridgeModel* model = project->getCartridgeModel();
      name    = model->getName(d.uuid);
      toolTip = QString();
      icon    = QIcon(":/resources/RR_page_white.png");
   }

   virtual void visit(CPrgRomUuid &d)
   {
      CCartridgeModel* model = project->getCartridgeModel();
      name    = model->getName(d.uuid);
      icon    = QIcon(":/resources/RR_page_white.png");
   }

};

//--------------------------------------------------------------------------------------
// CProjectTreeWidget implementation
//--------------------------------------------------------------------------------------

CProjectTreeWidget::CProjectTreeWidget(QWidget *parent) :
   QTreeWidget(parent)
{

}

QUuid CProjectTreeWidget::getUuidAt(const QPoint &pos)
{
   return getUuidOf(this->itemAt(pos));
}

QUuid CProjectTreeWidget::getUuidOf(const QTreeWidgetItem *item)
{
   if (item == NULL)
      return QUuid();

   return item->data(0, Qt::UserRole).toString();
}

bool CProjectTreeWidget::containsUuid(const QUuid &uuid)
{
   return findTreeWidgetItem(uuid) != NULL;
}

void CProjectTreeWidget::setCurrentUuid(const QUuid &uuid)
{
   QTreeWidgetItem *item = findTreeWidgetItem(uuid);
   if (item != NULL)
   {
      this->setCurrentItem(item);
   }
}

void CProjectTreeWidget::addItem(CProjectModel *project, const QUuid &uuid, const QUuid &parentUuid)
{
   QTreeWidgetItem* item = buildNodeFromUuid(project, uuid);

   QTreeWidgetItem* parentItem = findTreeWidgetItem(parentUuid);
   if (parentItem == NULL)
      this->addTopLevelItem(item);
   else
      parentItem->addChild(item);
}

void CProjectTreeWidget::setParent(const QUuid &child, const QUuid &parent)
{
   QTreeWidgetItem *childItem  = findTreeWidgetItem(child);
   QTreeWidgetItem *parentItem = findTreeWidgetItem(parent);

   if (parentItem == NULL || childItem == NULL)
      return;

   if (childItem->parent())
   {
      childItem->parent()->removeChild(childItem);
   }
   else
   {
      this->takeTopLevelItem(this->indexOfTopLevelItem(childItem));
   }

   parentItem->addChild(childItem);
}

void CProjectTreeWidget::removeItem(const QUuid &uuid)
{
   delete findTreeWidgetItem(uuid);
}

QTreeWidgetItem * CProjectTreeWidget::buildNodeFromUuid(CProjectModel *project, const QUuid &uuid)
{
   NodeDataReader visitor(project);
   project->visitDataItem(uuid, visitor);

   QTreeWidgetItem* item = new QTreeWidgetItem();
   item->setText(0, visitor.name);
   item->setIcon(0, visitor.icon);
   item->setToolTip(0, visitor.toolTip);
   item->setData(0, Qt::UserRole, QVariant(uuid));

   return item;
}

QTreeWidgetItem *CProjectTreeWidget::findTreeWidgetItem(const QUuid &uuid)
{
   QTreeWidgetItemIterator it(this);
   while (*it)
   {
      if (getUuidOf(*it) == uuid)
         return *it;
      ++it;
   }

   return NULL;
}
