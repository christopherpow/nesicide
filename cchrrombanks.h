#ifndef CCHRROMBANKS_H
#define CCHRROMBANKS_H

#include <QList>
#include "ixmlserializable.h"
#include "iprojecttreeviewitem.h"
#include "cchrrombank.h"

class CCHRROMBanks : public IXMLSerializable, public IProjectTreeViewItem
{
public:
    CCHRROMBanks();
    QList<CCHRROMBank *> banks;

    // IXMLSerializable Interface Implementation
    virtual bool serialize(QDomDocument &doc, QDomNode &node);
    virtual bool deserialize(QDomDocument &doc, QDomNode &node);

    // IProjectTreeViewItem Interface Implmentation
    QString caption() const;
    virtual void contextMenuEvent(QContextMenuEvent*, QTreeView*) {}
    virtual void openItemEvent(QTabWidget*) {}
};

#endif // CCHRROMBANKS_H
