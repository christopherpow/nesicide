#ifndef CBINARYFILE_H
#define CBINARYFILE_H

#include "cprojectbase.h"
#include "ichrrombankitem.h"

#include <QMessageBox>

class CBinaryFile : public CProjectBase,
                    public IChrRomBankItem
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

    // IChrRomBankItem Interface Implementation
    virtual int getChrRomBankItemSize();
    virtual QByteArray* getChrRomBankItemData();
    virtual QIcon getChrRomBankItemIcon();
    virtual QString getItemType() { return "Binary File"; };

private:
    QByteArray *m_binaryData;
    QString m_name;
};

#endif // CBINARYFILE_H
