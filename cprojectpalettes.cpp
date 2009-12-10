#include "cprojectpalettes.h"

CProjectPalettes::CProjectPalettes()
{

}

QString CProjectPalettes::caption() const
{
    return QString("Palettes");
}

void CProjectPalettes::contextMenuEvent(QContextMenuEvent *event, QTreeView *parent)
{
    QMenu menu(parent);
    menu.addAction(QIcon(":resources/style.svg"), "&New Palette");

    QAction *ret = menu.exec(event->globalPos());
    if (ret)
    {
        if (ret->text() == "&New Palette")
        {
            newPaletteEvent();
            ((CProjectTreeViewModel *)parent->model())->layoutChangedEvent();
        }
    }

}

void CProjectPalettes::newPaletteEvent()
{
    CProjectPaletteItem *palItem = new CProjectPaletteItem();
    palItem->InitTreeItem(this);
    this->appendChild(palItem);
}

