#ifndef CPRGROMBANK_H
#define CPRGROMBANK_H

#include "cprojectbase.h"
#include "prgromdisplaydialog.h"

#include "nes_emulator_core.h"

class CPRGROMBank : public CProjectBase
{
   Q_OBJECT
public:
   CPRGROMBank(IProjectTreeViewItem* parent);
   virtual ~CPRGROMBank();

   // Member Getters
   uint32_t getBankIndex() { return m_bankIndex; }
   uint8_t* getBankData() { return m_bankData; }
   PRGROMDisplayDialog* editor() { return dynamic_cast<PRGROMDisplayDialog*>(m_editor); }

   // Member Setters
   void setBankIndex(uint32_t bankIndex) { m_bankIndex = bankIndex; }
   void setBankData(uint8_t* bankData) { memcpy(m_bankData,bankData,MEM_8KB); }
   void clearBankData() { memset(m_bankData,0,MEM_8KB); }

   // IXMLSerializable Interface Implementation
   virtual bool serialize(QDomDocument& doc, QDomNode& node);
   virtual bool deserialize(QDomDocument& doc, QDomNode& node, QString& errors);

   // IProjectTreeViewItem Interface Implmentation
   QString caption() const;
   virtual void openItemEvent(CProjectTabWidget* tabWidget);

private:
   // Attributes
   uint32_t m_bankIndex;
   uint8_t  m_bankData [ MEM_8KB ];
};

#endif // CPRGROMBANK_H
