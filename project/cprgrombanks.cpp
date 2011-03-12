#include "cprgrombanks.h"

CPRGROMBanks::CPRGROMBanks(IProjectTreeViewItem* parent)
{
   // Add node to tree
   InitTreeItem(parent);
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

bool CPRGROMBanks::serialize(QDomDocument& doc, QDomNode& node)
{
   // Create the root element for the PRG-ROM banks
   QDomElement prgromElement = addElement( doc, node, "prgrombanks" );

   for (int i = 0; i < m_prgRomBanks.count(); i++)
   {
      if (!m_prgRomBanks.at(i)->serialize(doc, prgromElement))
      {
         return false;
      }
   }

   return true;
}

bool CPRGROMBanks::deserialize(QDomDocument&, QDomNode&, QString& errors)
{
   return true;
}

QString CPRGROMBanks::caption() const
{
   return "PRG-ROM Banks";
}
