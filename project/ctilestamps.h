#ifndef CTILESTAMPS_H
#define CTILESTAMPS_H

#include "ctilestamp.h"
#include "cprojecttreeviewmodel.h"

#include <QList>

class CTileStamps : public CProjectBase
{
   Q_OBJECT
public:
   CTileStamps(IProjectTreeViewItem* parent);
   virtual ~CTileStamps();

   // Helper functions
   void initializeProject();
   void terminateProject();

   QList<CTileStamp*>& getTileStampList() { return m_tileStamps; }

   // IXMLSerializable Interface Implementation
   virtual bool serialize(QDomDocument& doc, QDomNode& node);
   virtual bool deserialize(QDomDocument& doc, QDomNode& node, QString& errors);

   // IProjectTreeViewItem Interface Implmentation
   QString caption() const;
   virtual void contextMenuEvent(QContextMenuEvent* event, QTreeView* parent);
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
   QList<CTileStamp*> m_tileStamps;
};

#endif // CTILESTAMPS_H
