#ifndef CCHRROMBANKS_H
#define CCHRROMBANKS_H

#include "cprojectbase.h"
#include "cchrrombank.h"

#include <QList>

class CCHRROMBanks : public CProjectBase
{
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
   virtual void contextMenuEvent(QContextMenuEvent*, QTreeView*) {}
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
   QList<CCHRROMBank*> m_chrRomBanks;
};

#endif // CCHRROMBANKS_H
