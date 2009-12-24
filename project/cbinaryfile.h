#ifndef CBINARYFILE_H
#define CBINARYFILE_H

#include <QMessageBox>

#include "iprojecttreeviewitem.h"
#include "ixmlserializable.h"
#include "cprojecttreeviewmodel.h"

class CBinaryFile : public IXMLSerializable, public IProjectTreeViewItem
{
public:
    CBinaryFile();
    virtual ~CBinaryFile();

    QByteArray *getBinaryData();
    void setBinaryData(QByteArray *newBinaryData);
    void setBinaryName(QString newName);

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
    virtual bool canChangeName() { return true; }
    virtual bool onNameChanged(QString newName);

private:
    QByteArray *m_binaryData;
    QString m_binaryName;
};

#endif // CBINARYFILE_H
