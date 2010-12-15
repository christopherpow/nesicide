#ifndef CPRGROMBANKS_H
#define CPRGROMBANKS_H

#include "cprojectbase.h"
#include "cprgrombank.h"

#include <QList>

class CPRGROMBanks : public CProjectBase
{
public:
    CPRGROMBanks();
    virtual ~CPRGROMBanks();

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
    virtual int getTabIndex() { return -1; }
    virtual bool isDocumentSaveable() { return false; }
    virtual void onSaveDocument() {}
    virtual bool canChangeName() { return false; }
    virtual bool onNameChanged(QString) { return true; }

private:
    QList<CPRGROMBank *> *m_pointerToArrayOfBanks;

};

#endif // CPRGROMBANKS_H
