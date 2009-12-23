#ifndef CPROJECTPRIMITIVES_H
#define CPROJECTPRIMITIVES_H



#include "iprojecttreeviewitem.h"
#include "cprojectpalettes.h"
#include <QString>

class CProjectPrimitives : public IProjectTreeViewItem
{
public:
    CProjectPrimitives();
    virtual ~CProjectPrimitives();
    QString caption() const;
    CProjectPalettes *projectPalettes;
    virtual void contextMenuEvent(QContextMenuEvent *event, QTreeView *parent);
    virtual void openItemEvent(QTabWidget*) {}
    virtual bool onCloseQuery() { return true; }
    virtual void onClose() {}
    virtual int getTabIndex() { return -1; }
    virtual bool isDocumentSaveable() { return false; }
    virtual void onSaveDocument() {}
    virtual bool canChangeName() { return false; }
    virtual bool onNameChanged(QString) { return true; }
};

#endif // CPROJECTPRIMITIVES_H
