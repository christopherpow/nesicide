#include "cgraphicsbank.h"
#include "cnesicideproject.h"

#include "main.h"

CGraphicsBank::CGraphicsBank(IProjectTreeViewItem* parent)
{
   // Add node to tree
   InitTreeItem("",parent);

   // Allocate attributes
   m_bankItems.clear();
}

CGraphicsBank::~CGraphicsBank()
{
}

QList<IChrRomBankItem*>& CGraphicsBank::getGraphics()
{
   if (m_editor)
   {
      m_bankItems = editor()->bankItems();
   }

   return m_bankItems;
}

bool CGraphicsBank::serialize(QDomDocument& doc, QDomNode& node)
{
   QDomElement element = addElement( doc, node, "graphicsbank" );
   element.setAttribute("name", m_name);
   element.setAttribute("uuid", uuid());

   if ( m_editor && m_editor->isModified() )
   {
      getGraphics();

      m_editor->setModified(false);
   }

   for (int i=0; i < m_bankItems.count(); i++)
   {
      QDomElement graphicsItemElement = addElement( doc, element, "graphicitem" );
      IProjectTreeViewItem* projectItem = dynamic_cast<IProjectTreeViewItem*>(m_bankItems.at(i));
      graphicsItemElement.setAttribute("uuid", projectItem->uuid() );
   }

   return true;
}

bool CGraphicsBank::deserialize(QDomDocument& doc, QDomNode& node, QString& errors)
{
   QDomElement element = node.toElement();

   if (element.isNull())
   {
      return false;
   }

   if (!element.hasAttribute("name"))
   {
      errors.append("Missing required attribute 'name' of element <source name='?'>\n");
      return false;
   }

   if (!element.hasAttribute("uuid"))
   {
      errors.append("Missing required attribute 'uuid' of element <source name='"+element.attribute("name")+"'>\n");
      return false;
   }

   m_name = element.attribute("name");

   setUuid(element.attribute("uuid"));

   m_bankItems.clear();

   QDomNode childNode = node.firstChild();

   if (!childNode.isNull())
   {
      do
      {
         if (childNode.nodeName() == "graphicitem")
         {
            QDomElement graphicItem = childNode.toElement();

            IProjectTreeViewItem* projectItem = findProjectItem(graphicItem.attribute("uuid"));
            IChrRomBankItem* pItem = dynamic_cast<IChrRomBankItem*>(projectItem);

            if ( pItem )
            {
               m_bankItems.append(pItem);
            }

         }
         else
         {
            return false;
         }
      } while (!(childNode = childNode.nextSibling()).isNull());
   }

   return true;
}

QString CGraphicsBank::caption() const
{
   return m_name;
}

void CGraphicsBank::contextMenuEvent(QContextMenuEvent* event, QTreeView* parent)
{
   QMenu menu(parent);
   menu.addAction("&Delete");

   QAction* ret = menu.exec(event->globalPos());

   if (ret)
   {
      if (ret->text() == "&Delete")
      {
         if (QMessageBox::question(parent, "Delete Source", "Are you sure you want to delete " + name(),
                                   QMessageBox::Yes, QMessageBox::No) != QMessageBox::Yes)
         {
            return;
         }

         if (m_editor)
         {
            QTabWidget* tabWidget = (QTabWidget*)this->m_editor->parentWidget()->parentWidget();
            tabWidget->removeTab(tabWidget->indexOf(m_editor));
         }

         // TODO: Fix this logic so the memory doesn't get lost.
         nesicideProject->getProject()->getGraphicsBanks()->removeChild(this);
         nesicideProject->getProject()->getGraphicsBanks()->getGraphicsBanks().removeAll(this);
         ((CProjectTreeViewModel*)parent->model())->layoutChangedEvent();
      }
   }
}

void CGraphicsBank::openItemEvent(CProjectTabWidget* tabWidget)
{
   if (m_editor)
   {
      if (m_editor->isVisible())
      {
         tabWidget->setCurrentWidget(m_editor);
      }
      else
      {
         tabWidget->addTab(m_editor, this->caption());
         tabWidget->setCurrentWidget(m_editor);
      }
   }
   else
   {
      m_editor = new GraphicsBankEditorForm(m_bankItems,this);
      tabWidget->addTab(m_editor, this->caption());
      tabWidget->setCurrentWidget(m_editor);
   }
}

void CGraphicsBank::saveItemEvent()
{
   m_bankItems.clear();

   for (int i=0; i < editor()->bankItems().count(); i++)
   {
      m_bankItems.append(editor()->bankItems().at(i));
   }

   if ( m_editor )
   {
      m_editor->setModified(false);
   }
}

bool CGraphicsBank::onNameChanged(QString newName)
{
   if (m_name != newName)
   {
      m_name = newName;

      if ( m_editor )
      {
         QTabWidget* tabWidget = (QTabWidget*)m_editor->parentWidget()->parentWidget();
         tabWidget->setTabText(tabWidget->indexOf(m_editor), newName);
      }
   }

   return true;
}
