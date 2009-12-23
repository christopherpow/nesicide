#ifndef CBINARYFILES_H
#define CBINARYFILES_H

#include "iprojecttreeviewitem.h"
#include "ixmlserializable.h"

class CBinaryFiles : public IXMLSerializable, public IProjectTreeViewItem
{
public:
    CBinaryFiles();
    virtual ~CBinaryFiles();

    // IXMLSerializable Interface Implementation
    virtual bool serialize(QDomDocument &doc, QDomNode &node);
    virtual bool deserialize(QDomDocument &doc, QDomNode &node);

    // IProjectTreeViewItem Interface Implmentation
    QString caption() const;
    virtual void contextMenuEvent(QContextMenuEvent*, QTreeView*);
    virtual void openItemEvent(QTabWidget*) {}
    virtual bool onCloseQuery() { return true; }
    virtual void onClose() {}
    virtual int getTabIndex() { return -1; }
    virtual bool isDocumentSaveable() { return false; }
    virtual void onSaveDocument() {}
    virtual bool canChangeName() { return false; }
    virtual bool onNameChanged(QString) { return true; }
};

#endif // CBINARYFILES_H
