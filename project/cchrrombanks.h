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
    virtual ~CCHRROMBanks();
    QList<CCHRROMBank *> banks;

    // IXMLSerializable Interface Implementation
    virtual bool serialize(QDomDocument &doc, QDomNode &node);
    virtual bool deserialize(QDomDocument &doc, QDomNode &node);

    // IProjectTreeViewItem Interface Implmentation
    QString caption() const;
    virtual void contextMenuEvent(QContextMenuEvent*, QTreeView*) {}
    virtual void openItemEvent(QTabWidget*) {}
    virtual bool onCloseQuery() { return true; }
    virtual void onClose() {}
    virtual int getTabIndex() { return -1; }
    virtual bool isDocumentSaveable() { return false; }
    virtual void onSaveDocument() {}
    virtual bool canChangeName() { return false; }
    virtual bool onNameChanged(QString) { return true; }
};

#endif // CCHRROMBANKS_H
