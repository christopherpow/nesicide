#ifndef CSOURCEITEM_H
#define CSOURCEITEM_H

#include "iprojecttreeviewitem.h"
#include "ixmlserializable.h"

class CSourceItem : public IXMLSerializable, public IProjectTreeViewItem
{
public:
    CSourceItem();
    ~CSourceItem();

    // Member Getters
    QString get_sourceName();

    // Member Setters
    void set_sourceName(QString sourceName);

    // IXMLSerializable Interface Implementation
    virtual bool serialize(QDomDocument &doc, QDomNode &node);
    virtual bool deserialize(QDomDocument &doc, QDomNode &node);

    // IProjectTreeViewItem Interface Implmentation
    QString caption() const;
    virtual void contextMenuEvent(QContextMenuEvent* event, QTreeView* parent);
    virtual void openItemEvent(QTabWidget* parent);

private:
    QString m_sourceName;
    qint8 m_indexOfTab;
};

#endif // CSOURCEITEM_H
