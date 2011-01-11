#include "cgraphicsassembler.h"
#include "cnesicideproject.h"

#include "main.h"

CGraphicsAssembler::CGraphicsAssembler()
{
}

bool CGraphicsAssembler::assemble()
{
   buildTextLogger.write("<b>Building CHR-ROM Banks:</b>");
   CGraphicsBanks* gfxBanks = nesicideProject->getProject()->getGraphicsBanks();
   CCHRROMBanks* chrRomBanks = nesicideProject->getCartridge()->getChrRomBanks();

   int oldChrRomBanks = chrRomBanks->getChrRomBanks().count();

   for (int gfxBankIdx = 0; gfxBankIdx < gfxBanks->getGraphicsBanks().count(); gfxBankIdx++)
   {
      CGraphicsBank* curGfxBank = gfxBanks->getGraphicsBanks().at(gfxBankIdx);

      bool appendBank = (--oldChrRomBanks < 0);
      CCHRROMBank* chrRomBank;

      if (appendBank)
      {
         chrRomBank = new CCHRROMBank(nesicideProject->getCartridge()->getChrRomBanks());
         chrRomBank->setBankIndex(gfxBankIdx);
         chrRomBanks->appendChild(chrRomBank);
         chrRomBanks->getChrRomBanks().append(chrRomBank);
      }
      else
      {
         chrRomBank = chrRomBanks->getChrRomBanks().at(gfxBankIdx);
      }

      chrRomBank->clearBankData();
      buildTextLogger.write("&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Constructing '" + curGfxBank->name() + "':");

      int dataOffset = 0;

      for (int bankItemIdx = 0; bankItemIdx < curGfxBank->getGraphics().count(); bankItemIdx++)
      {
         IChrRomBankItem* bankItem = curGfxBank->getGraphics().at(bankItemIdx);
         IProjectTreeViewItem* ptvi = dynamic_cast<IProjectTreeViewItem*>(bankItem);
         buildTextLogger.write("&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;" +
                               ptvi->caption() + "...");

         memcpy(chrRomBank->getBankData() + dataOffset, bankItem->getChrRomBankItemData()->data(), bankItem->getChrRomBankItemSize());
         dataOffset += bankItem->getChrRomBankItemSize();
      }


   }

   return true;
}
