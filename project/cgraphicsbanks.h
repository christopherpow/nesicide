#ifndef CGRAPHICSBANKS_H
#define CGRAPHICSBANKS_H

#include "cprojectbase.h"
#include "cgraphicsbank.h"
#include "cprojecttreeviewmodel.h"

#include <QList>
#include <QInputDialog>

class CGraphicsBanks : public CProjectBase
{
public:
   CGraphicsBanks(IProjectTreeViewItem* parent);
   virtual ~CGraphicsBanks();

   // Helper functions
   void initializeProject();
   void terminateProject();

   QList<CGraphicsBank*>& getGraphicsBanks() { return m_graphicsBanks; }

   // IXMLSerializable Interface Implementation
   virtual bool serialize(QDomDocument& doc, QDomNode& node);
   virtual bool deserialize(QDomDocument& doc, QDomNode& node, QString& errors);

   // IProjectTreeViewItem Interface Implmentation
   QString caption() const
   {
      return QString("Graphics Banks");
   }
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
   QList<CGraphicsBank*> m_graphicsBanks;
};

#endif // CGRAPHICSBANKS_H
