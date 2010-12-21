#ifndef CPRGROMBANK_H
#define CPRGROMBANK_H

#include "cprojectbase.h"
#include "prgromdisplaydialog.h"

#include "emulator_core.h"

class CPRGROMBank : public CProjectBase
{
public:
   CPRGROMBank(IProjectTreeViewItem* parent);
   virtual ~CPRGROMBank();

   // Member Getters
   uint32_t getBankIndex() { return m_bankIndex; }
   uint8_t* getBankData() { return m_bankData; }
   PRGROMDisplayDialog* get_pointerToEditorDialog();
   int get_indexOfEditorTab();
   
   // Member Setters
   void setBankIndex(uint32_t bankIndex) { m_bankIndex = bankIndex; }
   void setBankData(uint8_t* bankData) { memcpy(m_bankData,bankData,MEM_16KB); }
   void clearBankData() { memset(m_bankData,0,MEM_16KB); }
   void set_pointerToEditorDialog(PRGROMDisplayDialog* pointerToEditorDialog);
   void set_indexOfEditorTab(int indexOfEditorTab);

   // IXMLSerializable Interface Implementation
   virtual bool serialize(QDomDocument& doc, QDomNode& node);
   virtual bool deserialize(QDomDocument& doc, QDomNode& node);

   // IProjectTreeViewItem Interface Implmentation
   QString caption() const;
   virtual void contextMenuEvent(QContextMenuEvent* event, QTreeView* parent);
   virtual void openItemEvent(QTabWidget* tabWidget);
   virtual bool onCloseQuery()
   {
      return true;
   }
   virtual void onClose() {}
   virtual int getTabIndex()
   {
      return -1;
   }
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
   int m_indexOfEditorTab;

   // Attributes
   uint32_t m_bankIndex;
   uint8_t  m_bankData [ MEM_16KB ];
   
   // Designer
   PRGROMDisplayDialog* m_pointerToEditorDialog;

};

#endif // CPRGROMBANK_H
