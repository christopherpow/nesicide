#ifndef CATTRIBUTETABLE_H
#define CATTRIBUTETABLE_H

#include "cprojectbase.h"
#include "attributetableeditorform.h"

#include <QMessageBox>
#include <QTabWidget>

class CAttributeTable : public CProjectBase
{
public:
   CAttributeTable(IProjectTreeViewItem* parent);
   virtual ~CAttributeTable();

   // IXMLSerializable Interface Implementation
   virtual bool serialize(QDomDocument& doc, QDomNode& node);
   virtual bool deserialize(QDomDocument& doc, QDomNode& node, QString& errors);

   // IProjectTreeViewItem Interface Implmentation
   virtual void contextMenuEvent(QContextMenuEvent* event, QTreeView* parent);
   virtual void openItemEvent(QTabWidget* parent);
   virtual bool onCloseQuery();
   virtual void onClose();
   virtual bool isDocumentSaveable();
   virtual void onSaveDocument();
   virtual bool canChangeName();
   virtual bool onNameChanged(QString newName);
   virtual QWidget* tab() { return m_editor; }
private:
   // Designer
   AttributeTableEditorForm* m_editor;
};

#endif // CATTRIBUTETABLE_H
