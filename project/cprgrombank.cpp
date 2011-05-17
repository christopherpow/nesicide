#include "cprgrombank.h"

CPRGROMBank::CPRGROMBank(IProjectTreeViewItem* parent)
{
   // Add node to tree
   InitTreeItem(parent);

   // Allocate attributes
   m_bankIndex = -1;
}

CPRGROMBank::~CPRGROMBank()
{
}

bool CPRGROMBank::serialize(QDomDocument& doc, QDomNode& node)
{
   // Don't carry the ROM data around with the project.
   return true;
}

bool CPRGROMBank::deserialize(QDomDocument& doc, QDomNode& node, QString& errors)
{
   // Don't carry the ROM data around with the project.
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
   }
   else
   {
      m_editor = new PRGROMDisplayDialog(m_bankData,this);
      tabWidget->addTab(m_editor, this->caption());
      tabWidget->setCurrentWidget(m_editor);
   }
}


