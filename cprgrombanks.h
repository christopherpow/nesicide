#ifndef CPRGROMBANKS_H
#define CPRGROMBANKS_H

#include <QList>

#include "IXMLSerializable.h"
#include "iprojecttreeviewitem.h"
#include "cprgrombank.h"

class CPRGROMBanks : public IXMLSerializable, public IProjectTreeViewItem
{
public:
    CPRGROMBanks();
    QList<CPRGROMBank *> banks;

    // IXMLSerializable Interface Implementation
    bool serialize(QDomDocument &doc, QDomNode &node);

    // IProjectTreeViewItem Interface Implmentation
    QString caption() const;
    virtual void contextMenuEvent(QContextMenuEvent *event, QTreeView *parent) {};
    virtual void openItemEvent(QTabWidget *tabWidget) {};
};

#endif // CPRGROMBANKS_H
