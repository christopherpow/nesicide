#ifndef CCARTRIDGE_H
#define CCARTRIDGE_H

#include "cprojectbase.h"
#include "cprgrombanks.h"
#include "cchrrombanks.h"

#include <QtGlobal>

namespace GameMirrorMode
{
typedef enum
{
   NoMirroring = 0,
   HorizontalMirroring = 1,
   VerticalMirroring = 2,
   FourScreenMirroring = 3
} eGameMirrorMode;
}

class CCartridge : public CProjectBase
{
public:
   CCartridge(IProjectTreeViewItem* parent);
   virtual ~CCartridge();

   // Helper functions
   void initializeProject();
   void terminateProject();

   // Member Getters
   GameMirrorMode::eGameMirrorMode getMirrorMode();
   qint8 getMapperNumber();
   bool isBatteryBackedRam();

   CPRGROMBanks* getPrgRomBanks();
   CCHRROMBanks* getChrRomBanks();

   // Member Setters
   void setMirrorMode(GameMirrorMode::eGameMirrorMode enumValue);
   void setMapperNumber(qint8 indexOfValue);
   void setBatteryBackedRam(bool hasBatteryBackedRam);

   // IXMLSerializable Interface Implementation
   virtual bool serialize(QDomDocument& doc, QDomNode& node);
   virtual bool deserialize(QDomDocument& doc, QDomNode& node);

   // IProjectTreeViewItem Interface Implmentation
   QString caption() const;
   virtual void contextMenuEvent(QContextMenuEvent*, QTreeView*) {}
   virtual void openItemEvent(QTabWidget*) {}
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
   // Contained children
   CPRGROMBanks* m_pPrgRomBanks;
   CCHRROMBanks* m_pChrRomBanks;

   // Attributes
   GameMirrorMode::eGameMirrorMode m_mirrorMode;               // Mirror mode used in the emulator
   bool m_hasBatteryBackedRam;                                     // Memory can be saved via RAM kept valid with a battery
   qint8 m_mapperNumber;                                           // Numeric ID of the cartridge mapper
};

#endif // CCARTRIDGE_H
