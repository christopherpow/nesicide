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
   PRGROMDisplayDialog* getEditor();

   // Member Setters
   void setBankIndex(uint32_t bankIndex) { m_bankIndex = bankIndex; }
   void setBankData(uint8_t* bankData) { memcpy(m_bankData,bankData,MEM_8KB); }
   void clearBankData() { memset(m_bankData,0,MEM_8KB); }

   // IXMLSerializable Interface Implementation
   virtual bool serialize(QDomDocument& doc, QDomNode& node);
   virtual bool deserialize(QDomDocument& doc, QDomNode& node, QString& errors);

   // IProjectTreeViewItem Interface Implmentation
   QString caption() const;
   virtual void contextMenuEvent(QContextMenuEvent* event, QTreeView* parent);
   virtual void openItemEvent(QTabWidget* tabWidget);
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
   virtual QWidget* tab() { return m_editor; }

private:
   // Attributes
   uint32_t m_bankIndex;
   uint8_t  m_bankData [ MEM_8KB ];

   // Designer
   PRGROMDisplayDialog* m_editor;
};

#endif // CPRGROMBANK_H
