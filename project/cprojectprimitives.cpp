#include "cprojectprimitives.h"

CProjectPrimitives::CProjectPrimitives()
{
   m_attributeTables = new CAttributeTables();
   m_attributeTables->InitTreeItem(this);
   this->appendChild(m_attributeTables);
}

CProjectPrimitives::~CProjectPrimitives()
{
   if (m_attributeTables)
   {
      delete m_attributeTables;
   }
}

QString CProjectPrimitives::caption() const
{
   return QString("Primitives");
}

void CProjectPrimitives::contextMenuEvent(QContextMenuEvent* event, QTreeView* parent)
{
}

