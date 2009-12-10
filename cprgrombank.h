#ifndef CPRGROMBANK_H
#define CPRGROMBANK_H

#include "ixmlserializable.h"
#include "iprojecttreeviewitem.h"
#include "prgromdisplaydialog.h"

class CPRGROMBank : public IXMLSerializable, public IProjectTreeViewItem
{
public:
    CPRGROMBank();
    qint8 bankID;
    qint8 data[0x4000];
    PRGROMDisplayDialog *editor;
    int tabId;

    // IXMLSerializable Interface Implementation
    bool serialize(QDomDocument &doc, QDomNode &node);

    // IProjectTreeViewItem Interface Implmentation
    QString caption() const;
    virtual void contextMenuEvent(QContextMenuEvent *event, QTreeView *parent);
    virtual void openItemEvent(QTabWidget *tabWidget);

};

#endif // CPRGROMBANK_H
