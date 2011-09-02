#include "ctilestamp.h"
#include "cnesicideproject.h"

#include "main.h"

CTileStamp::CTileStamp(IProjectTreeViewItem* parent)
{
   // Add node to tree
   InitTreeItem("",parent);

   // Allocate attributes
}

CTileStamp::~CTileStamp()
{
}

QByteArray CTileStamp::getTile()
{
   if (m_editor)
   {
      m_tile = editor()->tile();
   }

   return m_tile;
}

bool CTileStamp::serialize(QDomDocument& doc, QDomNode& node)
{
   QDomElement element = addElement( doc, node, "tile" );
   element.setAttribute("name", m_name);
   element.setAttribute("uuid", uuid());

   if ( m_editor && m_editor->isModified() )
   {
      getTile();

      m_editor->setModified(false);
   }

   return true;
}

bool CTileStamp::deserialize(QDomDocument&, QDomNode& node, QString& errors)
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

   return true;
}

void CTileStamp::contextMenuEvent(QContextMenuEvent* event, QTreeView* parent)
{
   QMenu menu(parent);
   menu.addAction("&Delete");

   QAction* ret = menu.exec(event->globalPos());

   if (ret)
   {
      if (ret->text() == "&Delete")
      {
         if (QMessageBox::question(parent, "Delete Tile", "Are you sure you want to delete " + name(),
                                   QMessageBox::Yes, QMessageBox::No) != QMessageBox::Yes)
         {
            return;
         }

         if (m_editor)
         {
            QTabWidget* tabWidget = (QTabWidget*)m_editor->parentWidget()->parentWidget();
            tabWidget->removeTab(tabWidget->indexOf(m_editor));
         }

         // TODO: Fix this logic so the memory doesn't get lost.
//         nesicideProject->getProject()->getSources()->removeChild(this);
//         nesicideProject->getProject()->getSources()->getSourceItems().removeAll(this);
         ((CProjectTreeViewModel*)parent->model())->layoutChangedEvent();
      }
   }
}

void CTileStamp::openItemEvent(CProjectTabWidget* tabWidget)
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
      m_editor = new TileStampEditorForm(m_tile,this);
      tabWidget->addTab(m_editor, this->caption());
      tabWidget->setCurrentWidget(m_editor);
   }
}

void CTileStamp::saveItemEvent()
{
   m_tile = editor()->tile();

   if ( m_editor )
   {
      m_editor->setModified(false);
   }
}

bool CTileStamp::canChangeName()
{
   return true;
}

bool CTileStamp::onNameChanged(QString newName)
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
