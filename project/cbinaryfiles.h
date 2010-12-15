#ifndef CBINARYFILES_H
#define CBINARYFILES_H

#include "cprojectbase.h"
#include "cbinaryfile.h"
#include "cprojecttreeviewmodel.h"

#include <QList>
#include <QMenu>
#include <QFileDialog>

class CBinaryFiles : public CProjectBase
{
public:
    CBinaryFiles();
    virtual ~CBinaryFiles();

    QList<CBinaryFile *> *getBinaryFileList();
    void setBinaryFileList(QList<CBinaryFile *> * newBinaryFileList);

    // IXMLSerializable Interface Implementation
    virtual bool serialize(QDomDocument &doc, QDomNode &node);
    virtual bool deserialize(QDomDocument &doc, QDomNode &node);

    // IProjectTreeViewItem Interface Implmentation
    QString caption() const;
    virtual void contextMenuEvent(QContextMenuEvent* event, QTreeView* parent);
    virtual void openItemEvent(QTabWidget*) {}
    virtual bool onCloseQuery() { return true; }
    virtual void onClose() {}
    virtual int getTabIndex() { return -1; }
    virtual bool isDocumentSaveable() { return false; }
    virtual void onSaveDocument() {}
    virtual bool canChangeName() { return false; }
    virtual bool onNameChanged(QString) { return true; }

private:
    QList<CBinaryFile *> *m_pBinaryFileList;
};

#endif // CBINARYFILES_H
