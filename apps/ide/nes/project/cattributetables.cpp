#include <stdint.h>

#include "cattributetables.h"

CAttributeTables::CAttributeTables(IProjectTreeViewItem* parent)
{
   // Add node to tree
   InitTreeItem(":/resources/folder_closed.png",parent);
}

CAttributeTables::~CAttributeTables()
{
   // Remove any allocated children
   for ( int i = 0; i < m_attributeTables.count(); i++ )
   {
      delete m_attributeTables.at(i);
   }

   // Initialize this node's attributes
   m_attributeTables.clear();
}

void CAttributeTables::initializeProject()
{
   // Remove any allocated children
   for ( int i = 0; i < m_attributeTables.count(); i++ )
   {
      removeChild(m_attributeTables.at(i));
      delete m_attributeTables.at(i);
   }

   // Initialize this node's attributes
   m_attributeTables.clear();
}

void CAttributeTables::terminateProject()
{
   // Remove any allocated children
   for ( int i = 0; i < m_attributeTables.count(); i++ )
   {
      removeChild(m_attributeTables.at(i));
      delete m_attributeTables.at(i);
   }

   // Initialize this node's attributes
   m_attributeTables.clear();
}

bool CAttributeTables::serialize(QDomDocument& doc, QDomNode& node)
{
   QDomElement attributeTablesElement = addElement( doc, node, "attributetables" );

   for (int i = 0; i < m_attributeTables.count(); i++)
   {
      if (!m_attributeTables.at(i)->serialize(doc, attributeTablesElement))
      {
         return false;
      }
   }

   return true;
}

bool CAttributeTables::deserialize(QDomDocument& doc, QDomNode& node, QString& errors)
{
   QDomNode childNode = node.firstChild();

   if (!childNode.isNull())
   {
      do
      {
         if (childNode.nodeName() == "attributetable")
         {
            CAttributeTable* pNewAttributeTable = new CAttributeTable(this);
            m_attributeTables.append(pNewAttributeTable);
            appendChild(pNewAttributeTable);

            if (!pNewAttributeTable->deserialize(doc,childNode,errors))
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

QString CAttributeTables::caption() const
{
   return QString("Palettes");
}
