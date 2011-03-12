#ifndef CPRGROMBANKS_H
#define CPRGROMBANKS_H

#include "cprojectbase.h"
#include "cprgrombank.h"

#include <QList>

class CPRGROMBanks : public CProjectBase
{
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
   QList<CPRGROMBank*> m_prgRomBanks;
};

#endif // CPRGROMBANKS_H
