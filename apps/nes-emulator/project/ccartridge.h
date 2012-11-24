#ifndef CCARTRIDGE_H
#define CCARTRIDGE_H

#include <QString>

#include "nes_emulator_core.h"

typedef enum
{
   HorizontalMirroring = 0,
   VerticalMirroring = 1
} eMirrorMode;

class CCartridge
{
public:
   CCartridge();
   virtual ~CCartridge();

   // Member Getters
   eMirrorMode getMirrorMode()
   {
      return m_mirrorMode;
   }
   int getMapperNumber()
   {
      return m_mapperNumber;
   }
   bool isBatteryBackedRam()
   {
      return m_hasBatteryBackedRam;
   }
   bool getFourScreen()
   {
      return m_fourScreen;
   }
   int getNumPrgRomBanks()
   {
      return m_numPrgBanks;
   }
   char* getPointerToPrgRomBank(int bank)
   {
      return m_prgBank[bank];
   }
   int getNumChrRomBanks()
   {
      return m_numChrBanks;
   }
   char* getPointerToChrRomBank(int bank)
   {
      return m_chrBank[bank];
   }
   QString getSaveStateFile()
   {
      return saveStateFile;
   }

   // Member Setters
   void setNumChrRomBanks(int banks)
   {
      m_numChrBanks = banks;
   }
   void setNumPrgRomBanks(int banks)
   {
      m_numPrgBanks = banks;
   }
   void setMirrorMode(eMirrorMode mirrorMode)
   {
      m_mirrorMode = mirrorMode;
   }
   void setMapperNumber(int mapperNumber)
   {
      m_mapperNumber = mapperNumber;
   }
   void setBatteryBackedRam(bool batteryBackedRam)
   {
      m_hasBatteryBackedRam = batteryBackedRam;
   }
   void setFourScreen(bool fourScreen)
   {
      m_fourScreen = fourScreen;
   }
   void setSaveStateFile(QString file)
   {
      saveStateFile = file;
   }

private:
   int m_numPrgBanks;
   char m_prgBank [ NUM_ROM_BANKS ][ MEM_8KB ];
   int m_numChrBanks;
   char m_chrBank [ NUM_ROM_BANKS ][ MEM_8KB ];
   eMirrorMode m_mirrorMode;                      // Mirror mode used in the emulator
   bool m_hasBatteryBackedRam;                        // Memory can be saved via RAM kept valid with a battery
   bool m_fourScreen;
   int  m_mapperNumber;                              // Numeric ID of the cartridge mapper
   QString saveStateFile;
};

#endif // CCARTRIDGE_H
