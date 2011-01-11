#include "cbinaryfile.h"
#include "cnesicideproject.h"

#include "main.h"

CBinaryFile::CBinaryFile(IProjectTreeViewItem* parent)
{
   // Add node to tree
   InitTreeItem(parent);
   
   // Allocate attributes
   m_binaryData = new QByteArray();
}

CBinaryFile::~CBinaryFile()
{
   if (m_binaryData)
   {
      delete m_binaryData;
   }
}


QByteArray* CBinaryFile::getBinaryData()
{
   return m_binaryData;
}

bool CBinaryFile::onNameChanged(QString newName)
{
   m_name = newName;
   return true;
}

void CBinaryFile::setBinaryData(QByteArray* newBinaryData)
{
   m_binaryData = newBinaryData;
}

bool CBinaryFile::serialize(QDomDocument& doc, QDomNode& node)
{
   QDomElement element = addElement( doc, node, "binaryfile" );
   element.setAttribute("name", m_name);
   element.setAttribute("uuid", uuid());
   QDomCDATASection dataSect = doc.createCDATASection(m_binaryData->toBase64());
   element.appendChild(dataSect);
   return true;
}

bool CBinaryFile::deserialize(QDomDocument& doc, QDomNode& node)
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
   QDomCDATASection cdata = element.firstChild().toCDATASection();

   if (cdata.isNull())
   {
      return false;
   }

   setUuid(element.attribute("uuid"));

   m_binaryData = new QByteArray(QByteArray::fromBase64(cdata.data().toUtf8()));

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
   return this->getBinaryData()->size();
}

QByteArray* CBinaryFile::getChrRomBankItemData()
{
   return this->getBinaryData();
}

QIcon CBinaryFile::getChrRomBankItemIcon()
{
   return QIcon(":/resources/22_binary_file.png");
}
