#include "cchrrombanks.h"

CCHRROMBanks::CCHRROMBanks(IProjectTreeViewItem* parent)
{
   // Add node to tree
   InitTreeItem(parent);
}

CCHRROMBanks::~CCHRROMBanks()
{
   // Remove any allocated children
   for ( int i = 0; i < m_chrRomBanks.count(); i++ )
   {
      delete m_chrRomBanks.at(i);
   }

   // Initialize this node's attributes
   m_chrRomBanks.clear();
}

void CCHRROMBanks::initializeProject()
{
   // Remove any allocated children
   for ( int i = 0; i < m_chrRomBanks.count(); i++ )
   {
      removeChild(m_chrRomBanks.at(i));
      delete m_chrRomBanks.at(i);
   }

   // Initialize this node's attributes
   m_chrRomBanks.clear();
}

void CCHRROMBanks::terminateProject()
{
   // Remove any allocated children
   for ( int i = 0; i < m_chrRomBanks.count(); i++ )
   {
      removeChild(m_chrRomBanks.at(i));
      delete m_chrRomBanks.at(i);
   }

   // Initialize this node's attributes
   m_chrRomBanks.clear();
}

// IXMLSerializable Interface Implementation
bool CCHRROMBanks::serialize(QDomDocument& doc, QDomNode& node)
{
   // Create the root element for the CHR-ROM banks
   QDomElement chrromElement = addElement( doc, node, "chrrombanks" );

   for (int i = 0; i < m_chrRomBanks.count(); i++)
   {
      if (!m_chrRomBanks.at(i)->serialize(doc, chrromElement))
      {
         return false;
      }
   }
   
   return true;
}

bool CCHRROMBanks::deserialize(QDomDocument&, QDomNode&)
{
   return true;
}

// IProjectTreeViewItem Interface Implmentation
QString CCHRROMBanks::caption() const
{
   return "CHR-ROM Banks";
}
