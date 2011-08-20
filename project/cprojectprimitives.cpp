#include <stdint.h>

#include "cprojectprimitives.h"

CProjectPrimitives::CProjectPrimitives(IProjectTreeViewItem* parent)
{
   // Add node to tree
   InitTreeItem(":/resources/folder_closed.png",parent);

   // Allocate children
   m_pAttributeTables = new CAttributeTables(this);
}

CProjectPrimitives::~CProjectPrimitives()
{
   if (m_pAttributeTables)
   {
      delete m_pAttributeTables;
   }
}

void CProjectPrimitives::initializeProject()
{
   // Initialize child nodes
   m_pAttributeTables->initializeProject();

   // Add child nodes to tree
   appendChild(m_pAttributeTables);
}

void CProjectPrimitives::terminateProject()
{
   // Terminate child nodes
   m_pAttributeTables->terminateProject();

   // Remove child nodes from tree
   removeChild(m_pAttributeTables);
}

bool CProjectPrimitives::serialize(QDomDocument& doc, QDomNode& node)
{
   // Create the root element for the CHR-ROM banks
   QDomElement projectPrimitivesElement = addElement( doc, node, "primitives" );

   if (m_pAttributeTables)
   {
      if (!m_pAttributeTables->serialize(doc, projectPrimitivesElement))
      {
         return false;
      }
   }
   else
   {
      return false;
   }

   return true;
}

bool CProjectPrimitives::deserialize(QDomDocument& doc, QDomNode& node, QString& errors)
{
   QDomNode childNode;

   // Deserialization order is important but file order is not,
   // so we must take care of any possible XML ordering of items
   // here.  First, look for primitives.
   childNode = node.firstChild();
   do
   {
      if (childNode.nodeName() == "attributetables")
      {
         if (!m_pAttributeTables->deserialize(doc,childNode,errors))
         {
            return false;
         }
      }
   }
   while (!(childNode = childNode.nextSibling()).isNull());

   return true;
}

QString CProjectPrimitives::caption() const
{
   return QString("Primitives");
}

void CProjectPrimitives::contextMenuEvent(QContextMenuEvent* event, QTreeView* parent)
{
}
