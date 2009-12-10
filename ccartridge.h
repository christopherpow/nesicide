#ifndef CCARTRIDGE_H
#define CCARTRIDGE_H

#include "ixmlserializable.h"
#include "iprojecttreeviewitem.h"
#include "cprgrombanks.h"

class CCartridge : public IXMLSerializable, public IProjectTreeViewItem
{
public:
    CCartridge();
    CPRGROMBanks *prgromBanks;

    // IXMLSerializable Interface Implementation
    bool serialize(QDomDocument &doc, QDomNode &node);

    // IProjectTreeViewItem Interface Implmentation
    QString caption() const;
    virtual void contextMenuEvent(QContextMenuEvent *event, QTreeView *parent) {};
    virtual void openItemEvent(QTabWidget *tabWidget) {};
};

#endif // CCARTRIDGE_H
