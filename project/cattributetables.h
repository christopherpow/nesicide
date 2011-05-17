#ifndef CATTRIBUTETABLES_H
#define CATTRIBUTETABLES_H

#include "cattributetable.h"
#include "cprojecttreeviewmodel.h"

#include <QInputDialog>
#include <QList>

class CAttributeTables : public CProjectBase
{
public:
   CAttributeTables(IProjectTreeViewItem* parent);
   virtual ~CAttributeTables();

   // Helper functions
   void initializeProject();
   void terminateProject();

   QList<CAttributeTable*>& getAttributeTableList() { return m_attributeTables; }

   // IXMLSerializable Interface Implementation
   virtual bool serialize(QDomDocument& doc, QDomNode& node);
   virtual bool deserialize(QDomDocument& doc, QDomNode& node, QString& errors);

   // IProjectTreeViewItem Interface Implmentation
   QString caption() const;
   virtual void contextMenuEvent(QContextMenuEvent* event, QTreeView* parent);
   virtual void openItemEvent(QTabWidget*) {}
   virtual bool onCloseQuery()
   {
      return true;
   }
   virtual void onClose() {}
   virtual bool isDocumentSaveable()
   {
      return false;
   }
   virtual void saveItemEvent() {}
   virtual bool canChangeName()
   {
      return false;
   }
   virtual bool onNameChanged(QString)
   {
      return true;
   }

private:
   // Contained children
   QList<CAttributeTable*> m_attributeTables;
};

#endif // CATTRIBUTETABLES_H
