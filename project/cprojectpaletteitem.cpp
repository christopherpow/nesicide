#include "cprojectpaletteitem.h"

CProjectPaletteItem::CProjectPaletteItem()
{
    this->name = QString("palette0");
}

CProjectPaletteItem::~CProjectPaletteItem()
{

}

QString CProjectPaletteItem::caption() const
{
    return name;
}

void CProjectPaletteItem::contextMenuEvent(QContextMenuEvent *event, QTreeView *parent)
{

}

void CProjectPaletteItem::openItemEvent(QTabWidget *tabWidget)
{
    PaletteEditorWindow *wnd = new PaletteEditorWindow();
    //wnd->paletteItem = this;
    int tabId = tabWidget->addTab(wnd, this->name);

}
