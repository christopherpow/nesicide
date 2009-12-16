#ifndef CPROJECT_H
#define CPROJECT_H

#include "iprojecttreeviewitem.h"
#include "ixmlserializable.h"
#include "csources.h"

class CProject : public IXMLSerializable, public IProjectTreeViewItem
{
public:
    CProject();
    ~CProject();

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

private:
    CSources *m_pointerToSources;
};

#endif // CPROJECT_H
