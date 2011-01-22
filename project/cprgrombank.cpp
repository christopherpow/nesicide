#include "cprgrombank.h"

CPRGROMBank::CPRGROMBank(IProjectTreeViewItem* parent)
{
   // Add node to tree
   InitTreeItem(parent);
   
   // Allocate attributes
   m_bankIndex = -1;

   m_editor = (PRGROMDisplayDialog*)0;
}

CPRGROMBank::~CPRGROMBank()
{
   if (m_editor)
   {
      delete m_editor;
   }
}

PRGROMDisplayDialog* CPRGROMBank::getEditor()
{
   return m_editor;
}

bool CPRGROMBank::serialize(QDomDocument& doc, QDomNode& node)
{
   // Create the root element for the CHR-ROM object
   QDomElement prgromElement = addElement( doc, node, "prgrom" );
   QDomCDATASection dataSect = doc.createCDATASection(QByteArray::fromRawData((const char*)m_bankData,
                               MEM_16KB).toBase64());
   prgromElement.appendChild(dataSect);

   return true;
}

bool CPRGROMBank::deserialize(QDomDocument& doc, QDomNode& node)
{
   // Read in the DOM element
   QDomElement prgromElement = doc.documentElement();

   return true;
}

QString CPRGROMBank::caption() const
{
   return "PRG Bank " + QString::number(m_bankIndex, 10);
}

void CPRGROMBank::contextMenuEvent(QContextMenuEvent*, QTreeView*)
{
}

void CPRGROMBank::openItemEvent(QTabWidget* tabWidget)
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

      return;
   }
   else
   {
      m_editor = new PRGROMDisplayDialog();
      m_editor->setRomData(m_bankData);
      tabWidget->addTab(m_editor, this->caption());
      tabWidget->setCurrentWidget(m_editor);
   }
}


