#include <stdint.h>

#include "ctilestamps.h"

CTileStamps::CTileStamps(IProjectTreeViewItem* parent)
{
   // Add node to tree
   InitTreeItem(":/resources/folder_closed.png",parent);
}

CTileStamps::~CTileStamps()
{
   // Remove any allocated children
   for ( int i = 0; i < m_tileStamps.count(); i++ )
   {
      delete m_tileStamps.at(i);
   }

   // Initialize this node's attributes
   m_tileStamps.clear();
}

void CTileStamps::initializeProject()
{
   // Remove any allocated children
   for ( int i = 0; i < m_tileStamps.count(); i++ )
   {
      removeChild(m_tileStamps.at(i));
      delete m_tileStamps.at(i);
   }

   // Initialize this node's attributes
   m_tileStamps.clear();
}

void CTileStamps::terminateProject()
{
   // Remove any allocated children
   for ( int i = 0; i < m_tileStamps.count(); i++ )
   {
      removeChild(m_tileStamps.at(i));
      delete m_tileStamps.at(i);
   }

   // Initialize this node's attributes
   m_tileStamps.clear();
}

bool CTileStamps::serialize(QDomDocument& doc, QDomNode& node)
{
   QDomElement tileStampsElement = addElement( doc, node, "tiles" );

   for (int i = 0; i < m_tileStamps.count(); i++)
   {
      if (!m_tileStamps.at(i)->serialize(doc, tileStampsElement))
      {
         return false;
      }
   }

   return true;
}

bool CTileStamps::deserialize(QDomDocument& doc, QDomNode& node, QString& errors)
{
   QDomNode childNode = node.firstChild();

   if (!childNode.isNull())
   {
      do
      {
         if (childNode.nodeName() == "tile")
         {
            CTileStamp* pNewTileStamp = new CTileStamp(this);
            m_tileStamps.append(pNewTileStamp);
            appendChild(pNewTileStamp);

            if (!pNewTileStamp->deserialize(doc,childNode,errors))
            {
               return false;
            }
         }
         else
         {
            return false;
         }
      }
      while (!(childNode = childNode.nextSibling()).isNull());
   }

   return true;
}

QString CTileStamps::caption() const
{
   return QString("Tiles & Screens");
}
