#include "cprgrombank.h"

CPRGROMBank::CPRGROMBank(IProjectTreeViewItem* parent)
{
   // Add node to tree
   InitTreeItem(parent);
   
   // Allocate attributes
   m_bankIndex = -1;

   m_pointerToEditorDialog = (PRGROMDisplayDialog*)0;
}

CPRGROMBank::~CPRGROMBank()
{
   if (m_pointerToEditorDialog)
   {
      delete m_pointerToEditorDialog;
   }
}

PRGROMDisplayDialog* CPRGROMBank::get_pointerToEditorDialog()
{
   return m_pointerToEditorDialog;
}

void CPRGROMBank::set_pointerToEditorDialog(PRGROMDisplayDialog* pointerToEditorDialog)
{
   m_pointerToEditorDialog = pointerToEditorDialog;
}

int CPRGROMBank::get_indexOfEditorTab()
{
   return m_indexOfEditorTab;
}

void CPRGROMBank::set_indexOfEditorTab(int indexOfEditorTab)
{
   m_indexOfEditorTab = indexOfEditorTab;
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

   if (m_pointerToEditorDialog)
   {
      if (m_pointerToEditorDialog->isVisible())
      {
         tabWidget->setCurrentIndex(m_indexOfEditorTab);
      }
      else
      {
         m_indexOfEditorTab = tabWidget->addTab(m_pointerToEditorDialog, this->caption());
         tabWidget->setCurrentIndex(m_indexOfEditorTab);
      }

      return;
   }
   else
   {
      m_pointerToEditorDialog = new PRGROMDisplayDialog();
      m_pointerToEditorDialog->setRomData(m_bankData);
      m_indexOfEditorTab = tabWidget->addTab(m_pointerToEditorDialog, this->caption());
   }

   tabWidget->setCurrentIndex(m_indexOfEditorTab);
}


