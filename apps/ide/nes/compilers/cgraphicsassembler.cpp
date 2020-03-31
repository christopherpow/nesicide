#include "cgraphicsassembler.h"
#include "cnesicideproject.h"

#include "main.h"

static const char emptyBank[MEM_8KB] = { 0, };

CGraphicsAssembler::CGraphicsAssembler()
{
}

void CGraphicsAssembler::clean()
{
   return;
}

bool CGraphicsAssembler::assemble()
{
   CGraphicsBanks* gfxBanks = nesicideProject->getProject()->getGraphicsBanks();
   CCHRROMBanks* chrBanks = nesicideProject->getCartridge()->getChrRomBanks();
   QDir outputDir(nesicideProject->getProjectCHRROMOutputBasePath());
   QString outputName;
   QFile chrRomFile;
   int gfxBankSize;
   int currentChrSize;
   int currentGfxSize;

   // Make sure directory exists...
   if ( !outputDir.exists() )
   {
      outputDir.mkpath(outputDir.path());
   }

   if ( nesicideProject->getProjectCHRROMOutputName().isEmpty() )
   {
      outputName = outputDir.fromNativeSeparators(outputDir.filePath(nesicideProject->getProjectOutputName()+".chr"));
   }
   else
   {
      outputName = outputDir.fromNativeSeparators(outputDir.filePath(nesicideProject->getProjectCHRROMOutputName()));
   }

   if ( gfxBanks->getGraphicsBanks().count() )
   {
      buildTextLogger->write("<b>Building: "+outputName+"</b>");

      chrRomFile.setFileName(outputName);
      chrRomFile.open(QIODevice::ReadWrite|QIODevice::Truncate);
      if ( chrRomFile.isOpen() )
      {
         currentChrSize = 0;

         for (int chrBankIdx = 0, gfxBankIdx = 0; gfxBankIdx < gfxBanks->getGraphicsBanks().count(); gfxBankIdx++)
         {
            CGraphicsBank *curGfxBank = gfxBanks->getGraphicsBanks().at(gfxBankIdx);
            CCHRROMBank *curChrBank = chrBanks->getChrRomBanks().at(chrBankIdx);
            uint8_t *pChrBankData = curChrBank->getBankData()+currentChrSize;

            gfxBankSize = curGfxBank->getSize();
            currentGfxSize = 0;

            buildTextLogger->write("Constructing '" + curGfxBank->caption() + "':");

            if ( curGfxBank->getGraphics().count() )
            {
               for (int bankItemIdx = 0; bankItemIdx < curGfxBank->getGraphics().count(); bankItemIdx++)
               {
                  IChrRomBankItem* bankItem = curGfxBank->getGraphics().at(bankItemIdx);
                  IProjectTreeViewItem* ptvi = dynamic_cast<IProjectTreeViewItem*>(bankItem);
                  buildTextLogger->write("&nbsp;&nbsp;&nbsp;Adding: "+ptvi->caption()+"("+QString::number(bankItem->getChrRomBankItemSize())+" bytes)");

                  QByteArray bankItemData = bankItem->getChrRomBankItemData();
                  currentGfxSize += bankItemData.count();
                  currentChrSize += bankItemData.count();
                  if ( currentGfxSize > gfxBankSize )
                  {
                     buildTextLogger->write("<font color='red'>"+curGfxBank->caption()+"("+QString::number(gfxBankSize)+"): Warning: too much data for specified size</font>");
                     bankItemData.truncate(gfxBankSize);
                  }
                  chrRomFile.write(bankItemData.data(), bankItemData.count());
                  memcpy(pChrBankData,bankItemData.data(),bankItemData.count());
                  pChrBankData += bankItemData.count();
               }
            }
            else
            {
               // 1,2,4, or 8KB of empty space
               chrRomFile.write(emptyBank,curGfxBank->getSize());
               memset(pChrBankData,0,curGfxBank->getSize());
               pChrBankData += curGfxBank->getSize();
            }

            if ( (gfxBankIdx < gfxBanks->getGraphicsBanks().count()-1) &&
                 (currentChrSize >= MEM_8KB) )
            {
               currentChrSize = 0;
               chrBankIdx++;

               CCHRROMBanks *chrRomBanks = nesicideProject->getCartridge()->getChrRomBanks();
               if ( chrBankIdx >= chrRomBanks->getChrRomBanks().count() )
               {
                  CCHRROMBank *curBank = new CCHRROMBank(nesicideProject->getCartridge()->getChrRomBanks());
                  // This is a new bank
                  curBank->setBankIndex(chrRomBanks->getChrRomBanks().count());
                  chrRomBanks->appendChild(curBank);
                  chrRomBanks->getChrRomBanks().append(curBank);
               }
            }
         }

         chrRomFile.close();

         return true;
      }
   }
   else
   {
      return true;
   }

   return false;
}
