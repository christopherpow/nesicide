#ifndef CSOURCEITEM_H
#define CSOURCEITEM_H

#include "cprojectbase.h"
#include "codeeditorform.h"

#include <QMessageBox>
#include <QTabWidget>

class CSourceItem : public CProjectBase
{
public:
   CSourceItem();
   virtual ~CSourceItem();

   // Member Getters
   QString get_sourceName();
   QString get_sourceCode();

   // Member Setters
   void set_sourceCode(QString sourceCode);
   void set_sourceName(QString sourceName);

   // IXMLSerializable Interface Implementation
   virtual bool serialize(QDomDocument& doc, QDomNode& node);
   virtual bool deserialize(QDomDocument& doc, QDomNode& node);

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
   QString m_name;
   QString m_sourceCode;
   int m_indexOfTab;
   CodeEditorForm* m_codeEditorForm;
};

#endif // CSOURCEITEM_H
