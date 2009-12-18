#ifndef CPROJECT_H
#define CPROJECT_H

#include "iprojecttreeviewitem.h"
#include "ixmlserializable.h"
#include "csources.h"
#include "csourceitem.h"

class CProject : public IXMLSerializable, public IProjectTreeViewItem
{
public:
    CProject();
    ~CProject();

    CSourceItem *getMainSource();
    void setMainSource(CSourceItem *newSource);
    CSources *getSources();
    void setSources(CSources *newSources);

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
    CSources *m_pointerToSources;
    CSourceItem *m_mainSource;
};

#endif // CPROJECT_H
