#include "cattributetable.h"
#include "cnesicideproject.h"

#include "main.h"

CAttributeTable::CAttributeTable(IProjectTreeViewItem* parent)
{
   // Add node to tree
   InitTreeItem(parent);
   
   // Allocate attributes
   m_attributeTableEditorForm = (AttributeTableEditorForm*)NULL;
}

CAttributeTable::~CAttributeTable()
{
   if (m_attributeTableEditorForm)
   {
      delete m_attributeTableEditorForm;
   }
}

bool CAttributeTable::serialize(QDomDocument& doc, QDomNode& node)
{
   QDomElement element = addElement( doc, node, "attributetable" );
   element.setAttribute("name", m_name);
   element.setAttribute("uuid", uuid());
   return true;
}

bool CAttributeTable::deserialize(QDomDocument&, QDomNode& node)
{
   QDomElement element = node.toElement();

   if (element.isNull())
   {
      return false;
   }

   if (!element.hasAttribute("name"))
   {
      return false;
   }

   if (!element.hasAttribute("uuid"))
   {
      return false;
   }

   m_name = element.attribute("name");

   setUuid(element.attribute("uuid"));

   return true;
}

void CAttributeTable::contextMenuEvent(QContextMenuEvent* event, QTreeView* parent)
{
   QMenu menu(parent);
   menu.addAction("&Delete");

   QAction* ret = menu.exec(event->globalPos());

   if (ret)
   {
      if (ret->text() == "&Delete")
      {
         if (QMessageBox::question(parent, "Delete Attribute Table", "Are you sure you want to delete " + name(),
                                   QMessageBox::Yes, QMessageBox::No) != QMessageBox::Yes)
         {
            return;
         }

         if (this->m_attributeTableEditorForm)
         {
            QTabWidget* tabWidget = (QTabWidget*)this->m_attributeTableEditorForm->parentWidget()->parentWidget();
            tabWidget->removeTab(m_tabIndex);
         }

         // TODO: Fix this logic so the memory doesn't get lost.
//         nesicideProject->getProject()->getSources()->removeChild(this);
//         nesicideProject->getProject()->getSources()->getSourceItems().removeAll(this);
         ((CProjectTreeViewModel*)parent->model())->layoutChangedEvent();
      }
   }
}

void CAttributeTable::openItemEvent(QTabWidget* tabWidget)
{
   if (m_attributeTableEditorForm)
   {
      if (m_attributeTableEditorForm->isVisible())
      {
         tabWidget->setCurrentIndex(m_tabIndex);
      }
      else
      {
         m_tabIndex = tabWidget->addTab(m_attributeTableEditorForm, this->caption());
         tabWidget->setCurrentIndex(m_tabIndex);
      }

      return;
   }
   else
   {
      m_attributeTableEditorForm = new AttributeTableEditorForm();
      m_tabIndex = tabWidget->addTab(m_attributeTableEditorForm, this->caption());
   }

   tabWidget->setCurrentIndex(m_tabIndex);
}

bool CAttributeTable::onCloseQuery()
{
   return true;
}

void CAttributeTable::onClose()
{
   if (m_attributeTableEditorForm)
   {
      delete m_attributeTableEditorForm;
      m_attributeTableEditorForm = (AttributeTableEditorForm*)NULL;
      m_tabIndex = -1;
   }
}

bool CAttributeTable::isDocumentSaveable()
{
   return true;
}

void CAttributeTable::onSaveDocument()
{
}

bool CAttributeTable::canChangeName()
{
   return true;
}

bool CAttributeTable::onNameChanged(QString newName)
{
   if (m_name != newName)
   {
      m_name = newName;

      if (m_attributeTableEditorForm && (m_tabIndex != -1))
      {
         QTabWidget* tabWidget = (QTabWidget*)m_attributeTableEditorForm->parentWidget()->parentWidget();
         tabWidget->setTabText(m_tabIndex, newName);
      }
   }

   return true;
}
