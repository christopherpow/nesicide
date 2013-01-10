#ifndef CCHRROMBANKS_H
#define CCHRROMBANKS_H

#include "cprojectbase.h"
#include "cchrrombank.h"

#include <QList>

class CCHRROMBanks : public CProjectBase
{
   Q_OBJECT
public:
   CCHRROMBanks(IProjectTreeViewItem* parent);
   virtual ~CCHRROMBanks();

   // Helper functions
   void initializeProject();
   void terminateProject();

   // Member Getters
   QList<CCHRROMBank*>& getChrRomBanks() { return m_chrRomBanks; }

   // IXMLSerializable Interface Implementation
   virtual bool serialize(QDomDocument& doc, QDomNode& node);
   virtual bool deserialize(QDomDocument& doc, QDomNode& node, QString& errors);

   // IProjectTreeViewItem Interface Implmentation
   QString caption() const;
   virtual void openItemEvent(CProjectTabWidget*) {}

   // ICenterWidgetItem Interface Implementation

private:
   // Contained children
   QList<CCHRROMBank*> m_chrRomBanks;
};

#endif // CCHRROMBANKS_H
