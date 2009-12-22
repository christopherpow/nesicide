#ifndef CSOURCES_H
#define CSOURCES_H

#include <QInputDialog>
#include <QList>
#include "iprojecttreeviewitem.h"
#include "ixmlserializable.h"
#include "csourceitem.h"
#include "cprojecttreeviewmodel.h"

class CSources : public IXMLSerializable, public IProjectTreeViewItem
{
public:
    CSources();
    virtual ~CSources();

    // Member Getters
    QList<CSourceItem *> *get_pointerToArrayOfSourceItems();

    // Member Setters
    void set_pointerToArrayOfSourceItems(QList<CSourceItem *> *pointerToArrayOfSourceItems);

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

private:
    QList<CSourceItem *> *m_pointerToArrayOfSourceItems;
};

#endif // CSOURCES_H
