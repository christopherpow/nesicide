#ifndef CCHRROMBANK_H
#define CCHRROMBANK_H

#include "cprojectbase.h"
#include "chrromdisplaydialog.h"

#include "nes_emulator_core.h"

class CCHRROMBank : public CProjectBase
{
   Q_OBJECT
public:
   CCHRROMBank(IProjectTreeViewItem* parent);
   virtual ~CCHRROMBank();
   void exportAsPNG();
   void importFromPNG();

   // Member Getters
   uint32_t getBankIndex() { return m_bankIndex; }
   uint8_t* getBankData() { return m_bankData; }

   // Member Setters
   void setBankIndex(uint32_t bankIndex) { m_bankIndex = bankIndex; }
   void clearBankData() { memset(m_bankData,0,MEM_8KB); }
   void setBankData(const char* bankData) { memcpy(m_bankData,bankData,MEM_8KB); }

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

#endif // CCHRROMBANK_H
