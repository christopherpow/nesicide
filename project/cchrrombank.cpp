#include "cchrrombank.h"

CCHRROMBank::CCHRROMBank(IProjectTreeViewItem* parent)
{
   // Add node to tree
   InitTreeItem(parent);
   
   // Allocate attributes
   editor = (CHRROMDisplayDialog*)NULL;
}

CCHRROMBank::~ CCHRROMBank()
{
}

bool CCHRROMBank::serialize(QDomDocument& doc, QDomNode& node)
{
   // Create the root element for the CHR-ROM object
   QDomElement chrromElement = addElement( doc, node, "chrrom" );
   QDomCDATASection dataSect = doc.createCDATASection(QByteArray::fromRawData((const char*)m_bankData, MEM_8KB).toBase64());
   chrromElement.appendChild(dataSect);
   return true;
}

bool CCHRROMBank::deserialize(QDomDocument& doc, QDomNode& node)
{
   // Read in the DOM element
   QDomElement chrromElement = doc.documentElement();

   return true;
}

QString CCHRROMBank::caption() const
{
   return "CHR Bank " + QString::number(m_bankIndex, 10);
}

void CCHRROMBank::contextMenuEvent(QContextMenuEvent*, QTreeView*)
{

}

void CCHRROMBank::openItemEvent(QTabWidget* tabWidget)
{
   if (editor)
   {
      if (editor->isVisible())
      {
         tabWidget->setCurrentIndex(m_tabIndex);
      }
      else
      {
         m_tabIndex = tabWidget->addTab(editor, this->caption());
         tabWidget->setCurrentIndex(m_tabIndex);
      }

      return;
   }
   else
   {
      editor = new CHRROMDisplayDialog(0, false, (qint8*)m_bankData);
      m_tabIndex = tabWidget->addTab(editor, this->caption());
   }


   tabWidget->setCurrentIndex(m_tabIndex);
   editor->updateScrollbars();

}

bool CCHRROMBank::onCloseQuery()
{
   return true;
}

void CCHRROMBank::onClose()
{
   if (editor)
   {
      delete editor;
      editor = (CHRROMDisplayDialog*)NULL;
      m_tabIndex = -1;
   }
}

void CCHRROMBank::onSaveDocument()
{

}
