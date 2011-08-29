#ifndef CSOURCES_H
#define CSOURCES_H

#include "cprojectbase.h"
#include "csourceitem.h"
#include "cprojecttreeviewmodel.h"

#include <QInputDialog>
#include <QList>

class CSources : public CProjectBase
{
   Q_OBJECT
public:
   CSources(IProjectTreeViewItem* parent);
   virtual ~CSources();

   // Helper functions
   void initializeProject();
   void terminateProject();

   void addSourceFile(QString fileName);

   // Member Getters
   QList<CSourceItem*>& getSourceItems() { return m_sourceItems; }

   // IXMLSerializable Interface Implementation
   virtual bool serialize(QDomDocument& doc, QDomNode& node);
   virtual bool deserialize(QDomDocument& doc, QDomNode& node, QString& errors);

   // IProjectTreeViewItem Interface Implmentation
   QString caption() const;
   virtual void contextMenuEvent(QContextMenuEvent*, QTreeView*);
   virtual void openItemEvent(CProjectTabWidget*) {}
   virtual bool onCloseQuery()
   {
      return true;
   }
   virtual void onClose() {}
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
   QList<CSourceItem*> m_sourceItems;
};

#endif // CSOURCES_H
