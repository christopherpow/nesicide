#include "cmusic.h"
#include "cnesicideproject.h"

#include "main.h"

CMusic::CMusic(IProjectTreeViewItem* parent)
{
   // Add node to tree
   InitTreeItem("",parent);
}

CMusic::~CMusic()
{
}

QList<uint8_t>& CMusic::getMusic()
{
   return m_music;
}

bool CMusic::serialize(QDomDocument& doc, QDomNode& node)
{
   QDomElement element = addElement( doc, node, "music" );
   element.setAttribute("name", m_name);
   element.setAttribute("uuid", uuid());

   if ( m_editor && m_editor->isModified() )
   {
      editor()->onSave();
   }

   return true;
}

bool CMusic::deserialize(QDomDocument&, QDomNode& node, QString& errors)
{
   QDomElement element = node.toElement();

   if (element.isNull())
   {
      return false;
   }

   if (!element.hasAttribute("name"))
   {
      errors.append("Missing required attribute 'name' of element <music name='?'>\n");
      return false;
   }

   if (!element.hasAttribute("uuid"))
   {
      errors.append("Missing required attribute 'uuid' of element <music name='"+element.attribute("name")+"'>\n");
      return false;
   }

   m_name = element.attribute("name");

   setUuid(element.attribute("uuid"));

   m_music.clear();

   return true;
}

void CMusic::contextMenuEvent(QContextMenuEvent* event, QTreeView* parent)
{
}

void CMusic::openItemEvent(CProjectTabWidget* tabWidget)
{
   if (m_editor)
   {
      tabWidget->setCurrentWidget(m_editor);
   }
   else
   {
      m_editor = new MusicEditorForm(/*m_music,*/this);
      tabWidget->addTab(m_editor, this->caption());
      tabWidget->setCurrentWidget(m_editor);
   }
}

void CMusic::saveItemEvent()
{
   // CPTODO: do
//   m_music = editor()->music();

   if ( m_editor )
   {
      m_editor->setModified(false);
   }
}

bool CMusic::onNameChanged(QString newName)
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
