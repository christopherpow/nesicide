#ifndef CSOURCEITEM_H
#define CSOURCEITEM_H

#include "cprojectbase.h"
#include "codeeditorform.h"

#include <QMessageBox>
#include <QTabWidget>

class CSourceItem : public CProjectBase
{
public:
   CSourceItem(IProjectTreeViewItem* parent);
   virtual ~CSourceItem();

   // Member Getters
   QString get_sourceCode();
   CodeEditorForm* getEditor() { return m_editor; }

   // Member Setters
   void set_sourceCode(QString sourceCode);

   // IXMLSerializable Interface Implementation
   virtual bool serialize(QDomDocument& doc, QDomNode& node);
   virtual bool serializeContent();
   virtual bool deserialize(QDomDocument& doc, QDomNode& node, QString& errors);
   virtual bool deserializeContent();

   // IProjectTreeViewItem Interface Implmentation
   QString caption() const;
   virtual void contextMenuEvent(QContextMenuEvent* event, QTreeView* parent);
   virtual void openItemEvent(QTabWidget* parent);
   virtual bool onCloseQuery();
   virtual void onClose();
   virtual bool isDocumentSaveable();
   virtual void onSaveDocument();
   virtual bool canChangeName();
   virtual bool onNameChanged(QString newName);
   virtual QWidget* tab() { return dynamic_cast<QWidget*>(m_editor); }

private:
   // Attributes
   QString m_sourceCode;

   // Designer
   CodeEditorForm* m_editor;
};

#endif // CSOURCEITEM_H
