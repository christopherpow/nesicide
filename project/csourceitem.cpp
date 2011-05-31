#include "csourceitem.h"
#include "cnesicideproject.h"

#include "main.h"

CSourceItem::CSourceItem(IProjectTreeViewItem* parent)
{
   // Add node to tree
   InitTreeItem(parent);

   // Allocate attributes
}

CSourceItem::~CSourceItem()
{
}

QString CSourceItem::sourceCode()
{
   if (m_editor)
   {
      m_sourceCode = editor()->sourceCode();
   }

   return m_sourceCode;
}

void CSourceItem::setSourceCode(QString sourceCode)
{
   m_sourceCode = sourceCode;

   if (m_editor)
   {
      editor()->setSourceCode(sourceCode);
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
   if ( m_editor && m_editor->isModified() )
   {
      QDir dir(QDir::currentPath());
      QFile fileOut(dir.relativeFilePath(m_path));

      if ( fileOut.open(QIODevice::ReadWrite|QIODevice::Truncate|QIODevice::Text) )
      {
         fileOut.write(sourceCode().toAscii());
      }

      m_editor->setModified(false);
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
   QDir dir(QDir::currentPath());
   QFile fileIn(dir.relativeFilePath(m_path));

   if ( fileIn.exists() && fileIn.open(QIODevice::ReadOnly|QIODevice::Text) )
   {
      setSourceCode(QString(fileIn.readAll()));
      fileIn.close();
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
   const QString REMOVE_FROM_PROJECT = "&Remove from Project";

   QMenu menu(parent);
   menu.addAction( REMOVE_FROM_PROJECT );

   QAction* ret = menu.exec(event->globalPos());

   if (ret)
   {
      if (ret->text() == REMOVE_FROM_PROJECT )
      {
         if (QMessageBox::question(parent, "Remove from Project", "Are you sure you want to remove " + name() + " from the project?",
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
         nesicideProject->getProject()->getSources()->removeChild(this);
         nesicideProject->getProject()->getSources()->getSourceItems().removeAll(this);
         ((CProjectTreeViewModel*)parent->model())->layoutChangedEvent();
      }
   }
}

void CSourceItem::openItemEvent(CProjectTabWidget* tabWidget)
{
   if (m_editor)
   {
      if (m_editor->isVisible())
      {
         tabWidget->setCurrentWidget(m_editor);
      }
      else
      {
         tabWidget->addTab(m_editor, caption());
         tabWidget->setCurrentWidget(m_editor);
      }
   }
   else
   {
      m_editor = new CodeEditorForm(path(),m_sourceCode,this);
      tabWidget->addTab(m_editor, path());
      tabWidget->setCurrentWidget(m_editor);
   }
}

void CSourceItem::saveItemEvent()
{
   m_sourceCode = editor()->sourceCode();
   serializeContent();
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
