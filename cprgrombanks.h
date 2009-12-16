#ifndef CPRGROMBANKS_H
#define CPRGROMBANKS_H

#include <QList>

#include "ixmlserializable.h"
#include "iprojecttreeviewitem.h"
#include "cprgrombank.h"

class CPRGROMBanks : public IXMLSerializable, public IProjectTreeViewItem
{
public:
    CPRGROMBanks();
    ~CPRGROMBanks();

    // Member Getters
    QList<CPRGROMBank *> *get_pointerToArrayOfBanks();

    // Member Setters
    void set_pointerToArrayOfBanks(QList<CPRGROMBank *> *pointerToArrayOfBanks);

    // IXMLSerializable Interface Implementation
    virtual bool serialize(QDomDocument &doc, QDomNode &node);
    virtual bool deserialize(QDomDocument &doc, QDomNode &node);

    // IProjectTreeViewItem Interface Implmentation
    QString caption() const;
    virtual void contextMenuEvent(QContextMenuEvent*, QTreeView*) {}
    virtual void openItemEvent(QTabWidget*) {}
    virtual bool onCloseQuery() { return true; }
    virtual void onClose() {}

private:
    QList<CPRGROMBank *> *m_pointerToArrayOfBanks;

};

#endif // CPRGROMBANKS_H
