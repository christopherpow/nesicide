#include "cprgrombank.h"

CPRGROMBank::CPRGROMBank(IProjectTreeViewItem* parent)
{
   // Add node to tree
   InitTreeItem("",parent);

   // Allocate attributes
   m_bankIndex = -1;
}

CPRGROMBank::~CPRGROMBank()
{
}

bool CPRGROMBank::serialize(QDomDocument& /*doc*/, QDomNode& /*node*/)
{
   // Don't carry the ROM data around with the project.
   return true;
}

bool CPRGROMBank::deserialize(QDomDocument& /*doc*/, QDomNode& /*node*/, QString& /*errors*/)
{
   // Don't carry the ROM data around with the project.
   return true;
}

QString CPRGROMBank::caption() const
{
   return "PRG Bank " + QString("%1").arg(m_bankIndex,2,16,QChar('0'));
}

void CPRGROMBank::openItemEvent(CProjectTabWidget* tabWidget)
{
   if (m_editor)
   {
      tabWidget->setCurrentWidget(m_editor);
   }
   else
   {
      m_editor = new PRGROMDisplayDialog(m_bankData,this);
      tabWidget->addTab(m_editor, this->caption());
      tabWidget->setCurrentWidget(m_editor);
   }
}


