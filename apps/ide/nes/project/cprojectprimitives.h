#ifndef CPROJECTPRIMITIVES_H
#define CPROJECTPRIMITIVES_H

#include "cprojectbase.h"
#include "cattributetables.h"
#include "ctilestamps.h"

#include <QString>

class CProjectPrimitives : public CProjectBase
{
   Q_OBJECT
public:
   CProjectPrimitives(IProjectTreeViewItem* parent);
   virtual ~CProjectPrimitives();

   // Helper functions
   void initializeProject();
   void terminateProject();

   CAttributeTables* getAttributeTables() { return m_pAttributeTables; }
   CTileStamps* getTileStamps() { return m_pTileStamps; }

   // IXMLSerializable Interface Implementation
   virtual bool serialize(QDomDocument& doc, QDomNode& node);
   virtual bool deserialize(QDomDocument& doc, QDomNode& node, QString& errors);

   // IProjectTreeViewItem Interface Implmentation
   QString caption() const;
   virtual void openItemEvent(CProjectTabWidget*) {}

   // ICenterWidgetItem Interface Implementation

private:
   // Contained children
   CAttributeTables* m_pAttributeTables;
   CTileStamps* m_pTileStamps;
};

#endif // CPROJECTPRIMITIVES_H
