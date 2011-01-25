#include "csourceitem.h"
#include "cnesicideproject.h"

#include "main.h"

CSourceItem::CSourceItem(IProjectTreeViewItem* parent)
{
   // Add node to tree
   InitTreeItem(parent);
   
   // Allocate attributes
   m_editor = (CodeEditorForm*)NULL;
}

CSourceItem::~CSourceItem()
{
   if (m_editor)
   {
      delete m_editor;
   }
}

QString CSourceItem::get_sourceCode()
{
   if (m_editor)
   {
      m_sourceCode = m_editor->get_sourceCode();
   }

   return m_sourceCode;
}

void CSourceItem::set_sourceCode(QString sourceCode)
{
   m_sourceCode = sourceCode;

   if (m_editor)
   {
      m_editor->set_sourceCode(sourceCode);
   }
}

bool CSourceItem::serialize(QDomDocument& doc, QDomNode& node)
{
   QDomElement element = addElement( doc, node, "source" );
   element.setAttribute("name", m_name);
   element.setAttribute("uuid", uuid());
   QDomCDATASection dataSect = doc.createCDATASection(get_sourceCode());
   element.appendChild(dataSect);
   return true;
}

bool CSourceItem::deserialize(QDomDocument&, QDomNode& node)
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

   QDomCDATASection cdata = element.firstChild().toCDATASection();

   if (cdata.isNull())
   {
      return false;
   }

   m_sourceCode = cdata.data();

   return true;
}

QString CSourceItem::caption() const
{
   return m_name;
}

void CSourceItem::contextMenuEvent(QContextMenuEvent* event, QTreeView* parent)
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

         if (nesicideProject->getProject()->getMainSource() == this)
         {
            nesicideProject->getProject()->setMainSource((CSourceItem*)NULL);
         }

         if (m_editor)
         {
            QTabWidget* tabWidget = (QTabWidget*)m_editor->parentWidget()->parentWidget();
            tabWidget->removeTab(tabWidget->indexOf(m_editor));
         }

         // TODO: Fix this logic so the memory doesn't get lost.
         nesicideProject->getProject()->getSources()->removeChild(this);
         nesicideProject->getProject()->getSources()->getSourceItems().removeAll(this);
         ((CProjectTreeViewModel*)parent->model())->layoutChangedEvent();
      }
   }
}

void CSourceItem::openItemEvent(QTabWidget* tabWidget)
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
      m_editor = new CodeEditorForm(this->caption());
      m_editor->set_sourceCode(m_sourceCode);
      tabWidget->addTab(m_editor, this->caption());
      tabWidget->setCurrentWidget(m_editor);
   }
}

bool CSourceItem::onCloseQuery()
{
   if (m_sourceCode != m_editor->get_sourceCode())
   {
      return (QMessageBox::question(0, QString("Confirm Close"),
                                    QString("This file has unsaved changes that\n"
                                            "will be lost if closed. Close anyway?"),
                                    QMessageBox::Yes, QMessageBox::Cancel) == QMessageBox::Yes);
   }
   else
   {
      return true;
   }

}

void CSourceItem::onClose()
{
   if (m_editor)
   {
      delete m_editor;
      m_editor = (CodeEditorForm*)NULL;
   }
}

bool CSourceItem::isDocumentSaveable()
{
   return true;
}

void CSourceItem::onSaveDocument()
{
   m_sourceCode = m_editor->get_sourceCode();
}

bool CSourceItem::canChangeName()
{
   return true;
}

bool CSourceItem::onNameChanged(QString newName)
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
