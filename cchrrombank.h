#ifndef CCHRROMBANK_H
#define CCHRROMBANK_H

#include "ixmlserializable.h"
#include "iprojecttreeviewitem.h"
#include "chrromdisplaydialog.h"

class CCHRROMBank : public IXMLSerializable, public IProjectTreeViewItem
{
public:
    CCHRROMBank();

    qint8 bankID;
    qint8 *data;
    CHRROMDisplayDialog *editor;
    qint8 tabId;


    // IXMLSerializable Interface Implementation
    bool serialize(QDomDocument &doc, QDomNode &node);

    // IProjectTreeViewItem Interface Implmentation
    QString caption() const;
    virtual void contextMenuEvent(QContextMenuEvent *event, QTreeView *parent);
    virtual void openItemEvent(QTabWidget *tabWidget);

};

#endif // CCHRROMBANK_H
