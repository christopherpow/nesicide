#ifndef CGRAPHICSBANKS_H
#define CGRAPHICSBANKS_H

#include <QList>
#include <QInputDialog>

#include "iprojecttreeviewitem.h"
#include "ixmlserializable.h"
#include "cgraphicsbank.h"
#include "cprojecttreeviewmodel.h"

class CGraphicsBanks : public IXMLSerializable, public IProjectTreeViewItem
{
public:
    CGraphicsBanks();
    virtual ~CGraphicsBanks();

    QList<CGraphicsBank *> *getGraphicsBankArray();
    void setGraphicsBankArray(QList<CGraphicsBank *> *newGraphicsBankArray);

    // IXMLSerializable Interface Implementation
    virtual bool serialize(QDomDocument &doc, QDomNode &node);
    virtual bool deserialize(QDomDocument &doc, QDomNode &node);

    // IProjectTreeViewItem Interface Implmentation
    QString caption() const { return QString("Banks"); }
    virtual void contextMenuEvent(QContextMenuEvent *event, QTreeView *parent);
    virtual void openItemEvent(QTabWidget*) {}
    virtual bool onCloseQuery() { return true; }
    virtual void onClose() {}
    virtual int getTabIndex() { return -1; }
    virtual bool isDocumentSaveable() { return false; }
    virtual void onSaveDocument() {}
    virtual bool canChangeName() { return false; }
    virtual bool onNameChanged(QString) { return true; }

private:
    QList<CGraphicsBank *> *m_paGraphicsBank;
};

#endif // CGRAPHICSBANKS_H
