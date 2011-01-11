#ifndef CCHRROMBANK_H
#define CCHRROMBANK_H

#include "cprojectbase.h"
#include "chrromdisplaydialog.h"

#include "emulator_core.h"

class CCHRROMBank : public CProjectBase
{
public:
   CCHRROMBank(IProjectTreeViewItem* parent);
   virtual ~CCHRROMBank();

   // Member Getters
   uint32_t getBankIndex() { return m_bankIndex; }
   uint8_t* getBankData() { return m_bankData; }
   
   // Member Setters
   void setBankIndex(uint32_t bankIndex) { m_bankIndex = bankIndex; }
   void clearBankData() { memset(m_bankData,0,MEM_8KB); }

   // IXMLSerializable Interface Implementation
   virtual bool serialize(QDomDocument& doc, QDomNode& node);
   virtual bool deserialize(QDomDocument& doc, QDomNode& node);

   // IProjectTreeViewItem Interface Implmentation
   QString caption() const;
   virtual void contextMenuEvent(QContextMenuEvent* event, QTreeView* parent);
   virtual void openItemEvent(QTabWidget* tabWidget);
   virtual bool onCloseQuery();
   virtual void onClose();
   virtual bool isDocumentSaveable()
   {
      return false;
   };
   virtual void onSaveDocument();
   virtual bool canChangeName()
   {
      return false;
   };
   virtual bool onNameChanged(QString)
   {
      return true;
   };
   
private:   
   // Attributes
   uint32_t m_bankIndex;
   uint8_t  m_bankData [ MEM_8KB ];
   
   // Designer
   CHRROMDisplayDialog* editor;
};

#endif // CCHRROMBANK_H
