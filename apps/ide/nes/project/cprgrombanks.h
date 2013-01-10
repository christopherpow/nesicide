#ifndef CPRGROMBANKS_H
#define CPRGROMBANKS_H

#include "cprojectbase.h"
#include "cprgrombank.h"

#include <QList>

class CPRGROMBanks : public CProjectBase
{
   Q_OBJECT
public:
   CPRGROMBanks(IProjectTreeViewItem* parent);
   virtual ~CPRGROMBanks();

   // Helper functions
   void initializeProject();
   void terminateProject();

   // Member Getters
   QList<CPRGROMBank*>& getPrgRomBanks() { return m_prgRomBanks; }

   // IXMLSerializable Interface Implementation
   virtual bool serialize(QDomDocument& doc, QDomNode& node);
   virtual bool deserialize(QDomDocument& doc, QDomNode& node, QString& errors);

   // IProjectTreeViewItem Interface Implmentation
   QString caption() const;
   virtual void openItemEvent(CProjectTabWidget*) {}

   // ICenterWidgetItem Interface Implementation

private:
   // Contained children
   QList<CPRGROMBank*> m_prgRomBanks;
};

#endif // CPRGROMBANKS_H
