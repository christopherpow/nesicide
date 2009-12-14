#ifndef CPROJECTPRIMITIVES_H
#define CPROJECTPRIMITIVES_H

#include "iprojecttreeviewitem.h"
#include "cprojectpalettes.h"
#include <QString>

class CProjectPrimitives : public IProjectTreeViewItem
{
public:
    CProjectPrimitives();
    ~CProjectPrimitives();
    QString caption() const;
    CProjectPalettes *projectPalettes;
    virtual void contextMenuEvent(QContextMenuEvent *event, QTreeView *parent);
    virtual void openItemEvent(QTabWidget*) {}
};

#endif // CPROJECTPRIMITIVES_H
