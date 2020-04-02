#include "cmusicitem.h"
#include "cnesicideproject.h"

CMusicItem::CMusicItem(IProjectTreeViewItem* parent)
{
   // Add node to tree
   InitTreeItem("",parent);
}

CMusicItem::~CMusicItem()
{
   // This is a singleton so the instance will be deleted elsewhere.
   // Prevent the base class from deleting anything.
   m_editor = NULL;
}

QByteArray CMusicItem::musicData()
{
   if (m_editor)
   {
      // We need to get the music data from the editor
      // for music files because music files
      // are stored external to the project.
      // We don't want to rely on the filesystem to give us
      // the latest copy of the file we *just* saved.
//      m_musicData = editor()->musicData();
   }

   return m_musicData;
}

void CMusicItem::setMusicData(QByteArray musicData)
{
   m_musicData = musicData;

   if (m_editor)
   {
//      editor()->setMusicData(musicData);
   }
}

bool CMusicItem::serialize(QDomDocument& doc, QDomNode& node)
{
   QDomElement element = addElement( doc, node, "music" );
   element.setAttribute("name", m_name);
   element.setAttribute("uuid", uuid());

   if ( m_editor && m_editor->isModified() )
   {
      editor()->onSave();
   }

   return serializeContent();
}

bool CMusicItem::serializeContent()
{
   if ( m_editor && m_editor->isModified() )
   {
      QDir dir(QDir::currentPath());
      QFile fileOut(dir.relativeFilePath(m_path));

      if ( fileOut.open(QIODevice::ReadWrite|QIODevice::Truncate|QIODevice::Text) )
      {
         fileOut.write(musicData());
      }

      m_editor->setModified(false);
   }

   return true;
}

bool CMusicItem::deserialize(QDomDocument&, QDomNode& node, QString& errors)
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

   m_musicData.clear();

   return deserializeContent();
}

bool CMusicItem::deserializeContent()
{
//   QDir dir(QDir::currentPath());
//   QFile fileIn(dir.relativeFilePath(m_path));

//   if ( fileIn.exists() && fileIn.open(QIODevice::ReadOnly|QIODevice::Text) )
//   {
//      setMusicData(fileIn.readAll());
//      fileIn.close();
//   }
//   else
//   {
//      // CPTODO: provide a file dialog for finding the source
//   }

   return true;
}

void CMusicItem::openItemEvent(CProjectTabWidget* tabWidget)
{
   if (m_editor)
   {
      tabWidget->setCurrentWidget(m_editor);
   }
   else
   {
      MusicEditorForm* editor = MusicEditorForm::instance();

      // Music Item must know their editor widget due to current architecture.
      openFile(this->caption());
      editor->setFileName(this->caption());
      editor->setTreeLink(this);
      m_editor = editor;
      tabWidget->addTab(m_editor, this->caption());
      tabWidget->setCurrentWidget(m_editor);
   }
}

bool CMusicItem::exportData()
{
   MusicEditorForm* editor = MusicEditorForm::instance();

   // Music Item must know their editor widget due to current architecture.
   openFile(this->caption());
   editor->setFileName(this->caption());
   editor->setTreeLink(this);
   m_editor = editor;

   bool ok = editor->exportData();

   openFile(NULL);

   return ok;
}

bool CMusicItem::onNameChanged(QString newName)
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
