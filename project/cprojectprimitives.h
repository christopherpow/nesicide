#ifndef CPROJECTPRIMITIVES_H
#define CPROJECTPRIMITIVES_H

#include "cprojectbase.h"
#include "cattributetables.h"

#include <QString>

class CProjectPrimitives : public CProjectBase
{
public:
   CProjectPrimitives(IProjectTreeViewItem* parent);
   virtual ~CProjectPrimitives();

   // Helper functions
   void initializeProject();
   void terminateProject();

   // IXMLSerializable Interface Implementation
   virtual bool serialize(QDomDocument& doc, QDomNode& node);
   virtual bool deserialize(QDomDocument& doc, QDomNode& node);

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
   virtual void onSaveDocument() {}
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
   CAttributeTables* m_pAttributeTables;
};

#endif // CPROJECTPRIMITIVES_H
