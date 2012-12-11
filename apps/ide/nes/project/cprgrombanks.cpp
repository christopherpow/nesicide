#include "cprgrombanks.h"

CPRGROMBanks::CPRGROMBanks(IProjectTreeViewItem* parent)
{
   // Add node to tree
   InitTreeItem(":/resources/folder_closed.png",parent);
}

CPRGROMBanks::~CPRGROMBanks()
{
   // Remove any allocated children
   for ( int i = 0; i < m_prgRomBanks.count(); i++ )
   {
      delete m_prgRomBanks.at(i);
   }

   // Initialize this node's attributes
   m_prgRomBanks.clear();
}

void CPRGROMBanks::initializeProject()
{
   // Remove any allocated children
   for ( int i = 0; i < m_prgRomBanks.count(); i++ )
   {
      removeChild(m_prgRomBanks.at(i));
      delete m_prgRomBanks.at(i);
   }

   // Initialize this node's attributes
   m_prgRomBanks.clear();
}

void CPRGROMBanks::terminateProject()
{
   // Remove any allocated children
   for ( int i = 0; i < m_prgRomBanks.count(); i++ )
   {
      removeChild(m_prgRomBanks.at(i));
      delete m_prgRomBanks.at(i);
   }

   // Initialize this node's attributes
   m_prgRomBanks.clear();
}

bool CPRGROMBanks::serialize(QDomDocument& /*doc*/, QDomNode& /*node*/)
{
   // Don't carry the ROM data around with the project.
   return true;
}

bool CPRGROMBanks::deserialize(QDomDocument&, QDomNode&, QString& /*errors*/)
{
   // Don't carry the ROM data around with the project.
   return true;
}

QString CPRGROMBanks::caption() const
{
   return "PRG-ROM Banks";
}
