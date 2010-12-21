#include "cattributetables.h"

CAttributeTables::CAttributeTables(IProjectTreeViewItem* parent)
{
   // Add node to tree
   InitTreeItem(parent);
}

CAttributeTables::~CAttributeTables()
{
}

void CAttributeTables::initializeProject()
{
}

void CAttributeTables::terminateProject()
{
}

QString CAttributeTables::caption() const
{
   return QString("Attribute Tables");
}

void CAttributeTables::contextMenuEvent(QContextMenuEvent* event, QTreeView* parent)
{
}

