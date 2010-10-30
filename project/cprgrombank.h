#ifndef CPRGROMBANK_H
#define CPRGROMBANK_H

#include "ixmlserializable.h"
#include "iprojecttreeviewitem.h"
#include "prgromdisplaydialog.h"

class CPRGROMBank : public IXMLSerializable, public IProjectTreeViewItem
{
public:
    CPRGROMBank();
    virtual ~CPRGROMBank();

    // Member Getters
    qint8 get_indexOfPrgRomBank();
    quint8 *get_pointerToBankData();
    PRGROMDisplayDialog *get_pointerToEditorDialog();
    int get_indexOfEditorTab();

    // Member Setters
    void set_indexOfPrgRomBank(qint8 indexOfPrgRomBank);
    void set_pointerToBankData(quint8 *pointerToBankData);
    void set_pointerToEditorDialog(PRGROMDisplayDialog *pointerToEditorDialog);
    void set_indexOfEditorTab(int indexOfEditorTab);

    // IXMLSerializable Interface Implementation
    virtual bool serialize(QDomDocument &doc, QDomNode &node);
    virtual bool deserialize(QDomDocument &doc, QDomNode &node);

    // IProjectTreeViewItem Interface Implmentation
    QString caption() const;
    virtual void contextMenuEvent(QContextMenuEvent *event, QTreeView *parent);
    virtual void openItemEvent(QTabWidget *tabWidget);
    virtual bool onCloseQuery() { return true; }
    virtual void onClose() {}
    virtual int getTabIndex() { return -1; }
    virtual bool isDocumentSaveable() { return false; }
    virtual void onSaveDocument() {}
    virtual bool canChangeName() { return false; }
    virtual bool onNameChanged(QString) { return true; }

private:
    qint8 m_indexOfPrgRomBank;
    quint8 *m_pointerToBankData;
    PRGROMDisplayDialog *m_pointerToEditorDialog;
    int m_indexOfEditorTab;

};

#endif // CPRGROMBANK_H
