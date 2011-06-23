#include "cbinaryfile.h"
#include "cnesicideproject.h"

#include "main.h"

CBinaryFile::CBinaryFile(IProjectTreeViewItem* parent)
{
   // Add node to tree
   InitTreeItem("",parent);

   // Allocate attributes
}

CBinaryFile::~CBinaryFile()
{
}

QByteArray CBinaryFile::getBinaryData()
{
   return m_binaryData;
}

bool CBinaryFile::onNameChanged(QString newName)
{
   m_name = newName;
   return true;
}

void CBinaryFile::setBinaryData(const QByteArray& newBinaryData)
{
   m_binaryData = newBinaryData;
}

bool CBinaryFile::serialize(QDomDocument& doc, QDomNode& node)
{
   QDomElement element = addElement( doc, node, "binaryfile" );

   element.setAttribute("name", m_name);
   element.setAttribute("path",m_path);
   element.setAttribute("uuid", uuid());

   // No need to serialize the content of binary files because there's
   // no way to modify their content within NESICIDE.

   return true;
}

bool CBinaryFile::deserialize(QDomDocument&, QDomNode& node, QString& errors)
{
   QDomElement element = node.toElement();

   if (element.isNull())
   {
      return false;
   }

   if (!element.hasAttribute("name"))
   {
      errors.append("Missing required attribute 'name' of element <binaryfile name='?'>\n");
      return false;
   }

   if (!element.hasAttribute("path"))
   {
      errors.append("Missing required attribute 'path' of element <binaryfile name='"+element.attribute("name")+"'>\n");
      return false;
   }

   if (!element.hasAttribute("uuid"))
   {
      errors.append("Missing required attribute 'uuid' of element <binaryfile name='"+element.attribute("name")+"'>\n");
      return false;
   }

   m_name = element.attribute("name");

   m_path = element.attribute("path");

   setUuid(element.attribute("uuid"));

   return deserializeContent();
}

bool CBinaryFile::deserializeContent()
{
   QDir dir(QDir::currentPath());
   QFile fileIn(dir.relativeFilePath(m_path));

   if ( fileIn.exists() && fileIn.open(QIODevice::ReadOnly|QIODevice::Text) )
   {
      setBinaryData(fileIn.readAll());
      fileIn.close();
   }
   else
   {
      // CPTODO: provide a file dialog for finding the binary file
   }

   return true;
}

QString CBinaryFile::caption() const
{
   return m_name;
}

void CBinaryFile::contextMenuEvent(QContextMenuEvent* event, QTreeView* parent)
{
   QMenu menu(parent);
   menu.addAction("&Delete");

   QAction* ret = menu.exec(event->globalPos());

   if (ret)
   {
      if (ret->text() == "&Delete")
      {
         if (QMessageBox::question(parent, "Delete Binary File", "Are you sure you want to delete " + m_name,
                                   QMessageBox::Yes, QMessageBox::No) != QMessageBox::Yes)
         {
            return;
         }

         // TODO: Fix this logic so the memory doesn't get lost.
         nesicideProject->getProject()->getBinaryFiles()->removeChild(this);
         nesicideProject->getProject()->getBinaryFiles()->getBinaryFileList().removeAll(this);
         ((CProjectTreeViewModel*)parent->model())->layoutChangedEvent();
      }
   }
}

int CBinaryFile::getChrRomBankItemSize()
{
   return getBinaryData().size();
}

QByteArray CBinaryFile::getChrRomBankItemData()
{
   return getBinaryData();
}

QIcon CBinaryFile::getChrRomBankItemIcon()
{
   return QIcon(":/resources/22_binary_file.png");
}
