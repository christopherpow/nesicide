#include "cprojectprimitives.h"

CProjectPrimitives::CProjectPrimitives()
{
    projectPalettes = new CProjectPalettes();
    projectPalettes->InitTreeItem(this);
    this->appendChild(projectPalettes);
}

CProjectPrimitives::~CProjectPrimitives()
{
    if (projectPalettes)
        delete projectPalettes;
}

QString CProjectPrimitives::caption() const
{
    return QString("Primitives");
}

void CProjectPrimitives::contextMenuEvent(QContextMenuEvent *event, QTreeView *parent)
{
}

