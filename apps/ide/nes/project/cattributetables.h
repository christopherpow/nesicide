#ifndef CATTRIBUTETABLES_H
#define CATTRIBUTETABLES_H

#include "cattributetable.h"

#include <QInputDialog>
#include <QList>

class CAttributeTables : public CProjectBase
{
   Q_OBJECT
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
   virtual void openItemEvent(CProjectTabWidget*) {}

   // ICenterWidgetItem Interface Implementation

private:
   // Contained children
   QList<CAttributeTable*> m_attributeTables;
};

#endif // CATTRIBUTETABLES_H
