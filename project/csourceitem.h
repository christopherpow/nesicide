#ifndef CSOURCEITEM_H
#define CSOURCEITEM_H

#include <QMessageBox>
#include <QTabWidget>
#include "iprojecttreeviewitem.h"
#include "ixmlserializable.h"
#include "codeeditorform.h"

class CSourceItem : public IXMLSerializable, public IProjectTreeViewItem
{
public:
    CSourceItem();
    ~CSourceItem();

    // Member Getters
    QString get_sourceName();
    QString get_sourceCode();

    // Member Setters
    void set_sourceCode(QString sourceCode);
    void set_sourceName(QString sourceName);

    // IXMLSerializable Interface Implementation
    virtual bool serialize(QDomDocument &doc, QDomNode &node);
    virtual bool deserialize(QDomDocument &doc, QDomNode &node);

    // IProjectTreeViewItem Interface Implmentation
    QString caption() const;
    virtual void contextMenuEvent(QContextMenuEvent* event, QTreeView* parent);
    virtual void openItemEvent(QTabWidget* parent);
    virtual bool onCloseQuery();
    virtual void onClose();
    virtual int getTabIndex();
    virtual bool isDocumentSaveable();
    virtual void onSaveDocument();
    virtual bool canChangeName();
    virtual bool onNameChanged(QString newName);

private:
    QString m_sourceName;
    QString m_sourceCode;
    int m_indexOfTab;
    CodeEditorForm *m_codeEditorForm;
};

#endif // CSOURCEITEM_H
