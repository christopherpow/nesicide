#ifndef CATTRIBUTETABLES_H
#define CATTRIBUTETABLES_H

#include "iprojecttreeviewitem.h"
#include "ixmlserializable.h"
#include <QString>

class CAttributeTables : public IProjectTreeViewItem, public IXMLSerializable
{
public:
   CAttributeTables();
   virtual ~CAttributeTables();

   // IXMLSerializable Interface Implementation
   virtual bool serialize(QDomDocument &doc, QDomNode &node) { IXMLEMPTYSPEC return true; }
   virtual bool deserialize(QDomDocument &doc, QDomNode &node) { IXMLEMPTYSPEC return true; }

   // IProjectTreeViewItem Interface Implmentation
   QString caption() const;
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

#endif // CATTRIBUTETABLES_H
