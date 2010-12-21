#include "cprojectprimitives.h"

CProjectPrimitives::CProjectPrimitives(IProjectTreeViewItem* parent)
{
   // Add node to tree
   InitTreeItem(parent);

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

QString CProjectPrimitives::caption() const
{
   return QString("Primitives");
}

void CProjectPrimitives::contextMenuEvent(QContextMenuEvent* event, QTreeView* parent)
{
}
