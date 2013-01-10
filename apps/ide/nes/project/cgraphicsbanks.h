#ifndef CGRAPHICSBANKS_H
#define CGRAPHICSBANKS_H

#include "cprojectbase.h"
#include "cgraphicsbank.h"

#include <QList>
#include <QInputDialog>

class CGraphicsBanks : public CProjectBase
{
   Q_OBJECT
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
   virtual void openItemEvent(CProjectTabWidget*) {}

   // ICenterWidgetItem Interface Implementation

private:
   // Contained children
   QList<CGraphicsBank*> m_graphicsBanks;
};

#endif // CGRAPHICSBANKS_H
