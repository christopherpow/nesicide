#ifndef CPROJECTPALETTES_H
#define CPROJECTPALETTES_H

#include "iprojecttreeviewitem.h"
#include "cprojectpaletteitem.h"
#include "cprojecttreeviewmodel.h"

class CProjectPalettes : public IProjectTreeViewItem
{

public:
    CProjectPalettes();
    void newPaletteEvent();

    // IProjectTreeViewItem Interface Implmentation
    QString caption() const;
    virtual void contextMenuEvent(QContextMenuEvent *event, QTreeView *parent);
    virtual void openItemEvent(QTabWidget *tabWidget) {};

};

#endif // CPROJECTPALETTES_H
