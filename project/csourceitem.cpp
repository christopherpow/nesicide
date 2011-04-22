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

   element.setAttribute("name",m_name);
   element.setAttribute("path",m_path);
   element.setAttribute("uuid",uuid());

   return serializeContent();
}

bool CSourceItem::serializeContent()
{
   QDir dir(nesicideProject->getProjectSourceBasePath());
   QFile fileOut(dir.absoluteFilePath(m_path));

   if ( fileOut.open(QIODevice::ReadWrite|QIODevice::Truncate|QIODevice::Text) )
   {
      fileOut.write(get_sourceCode().toAscii());
   }

   return true;
}

bool CSourceItem::deserialize(QDomDocument&, QDomNode& node, QString& errors)
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

   if (!element.hasAttribute("path"))
   {
      errors.append("Missing required attribute 'path' of element <source name='"+element.attribute("name")+"'>\n");
      return false;
   }

   if (!element.hasAttribute("uuid"))
   {
      errors.append("Missing required attribute 'uuid' of element <source name='"+element.attribute("name")+"'>\n");
      return false;
   }

   m_name = element.attribute("name");

   m_path = element.attribute("path");

   setUuid(element.attribute("uuid"));

   return deserializeContent();
}

bool CSourceItem::deserializeContent()
{
   QDir dir(nesicideProject->getProjectSourceBasePath());
   QFile fileIn(dir.absoluteFilePath(m_path));

   if ( fileIn.exists() && fileIn.open(QIODevice::ReadOnly|QIODevice::Text) )
   {
      set_sourceCode(QString(fileIn.readAll()));
   }
   else
   {
      // CPTODO: provide a file dialog for finding the source
   }

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
