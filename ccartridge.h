#ifndef CCARTRIDGE_H
#define CCARTRIDGE_H

#include "ixmlserializable.h"
#include "iprojecttreeviewitem.h"
#include "cprgrombanks.h"
#include "cchrrombanks.h"

class CCartridge : public IXMLSerializable, public IProjectTreeViewItem
{
public:
    CCartridge();
    ~CCartridge();

    // Member Getters
    CPRGROMBanks *get_pointerToPrgRomBanks();
    CCHRROMBanks *get_pointerToChrRomBanks();

    // IXMLSerializable Interface Implementation
    virtual bool serialize(QDomDocument &doc, QDomNode &node);
    virtual bool deserialize(QDomDocument &doc, QDomNode &node);

    // IProjectTreeViewItem Interface Implmentation
    QString caption() const;
    virtual void contextMenuEvent(QContextMenuEvent*, QTreeView*) {}
    virtual void openItemEvent(QTabWidget*) {}

private:
    CPRGROMBanks *m_pointerToPrgRomBanks;
    CCHRROMBanks *m_pointerToChrRomBanks;
};

#endif // CCARTRIDGE_H
