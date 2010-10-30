#include "cgraphicsassembler.h"
#include "cnesicideproject.h"

CGraphicsAssembler::CGraphicsAssembler()
{
}

bool CGraphicsAssembler::assemble()
{
   buildTextLogger.write("<b>Building CHR-ROM Banks:</b>");
   CGraphicsBanks *gfxBanks = nesicideProject->getProject()->getGraphics()->getGraphicsBanks();
   CCHRROMBanks *chrRomBanks = nesicideProject->get_pointerToCartridge()->getPointerToChrRomBanks();

   int oldChrRomBanks = chrRomBanks->banks.count();

   for (int gfxBankIdx = 0; gfxBankIdx < gfxBanks->getGraphicsBankArray()->count(); gfxBankIdx++)
   {
      CGraphicsBank *curGfxBank = gfxBanks->getGraphicsBankArray()->at(gfxBankIdx);

      bool appendBank = (--oldChrRomBanks < 0);
      CCHRROMBank* chrRomBank;

      if (appendBank)
      {
         chrRomBank = new CCHRROMBank();
         chrRomBank->data = new qint8[0x2000];
         chrRomBank->bankID = gfxBankIdx;
         chrRomBank->InitTreeItem(chrRomBanks);
         chrRomBanks->appendChild(chrRomBank);
         chrRomBanks->banks.append(chrRomBank);
     } else {
         chrRomBank = chrRomBanks->banks.at(gfxBankIdx);
     }

      memset(chrRomBank->data, 0, sizeof(qint8)*0x2000);
      buildTextLogger.write("&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Constructing '" + curGfxBank->getBankName() + "':");

      int dataOffset = 0;

      for (int bankItemIdx = 0; bankItemIdx < curGfxBank->bankItems.count(); bankItemIdx++)
      {
         IChrRomBankItem *bankItem = curGfxBank->bankItems.at(bankItemIdx);
         IProjectTreeViewItem *ptvi = dynamic_cast<IProjectTreeViewItem*>(bankItem);
         buildTextLogger.write("&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;" +
                                 ptvi->caption() + "...");

         memcpy(chrRomBank->data + dataOffset, bankItem->getChrRomBankItemData()->data(), bankItem->getChrRomBankItemSize());
         dataOffset += bankItem->getChrRomBankItemSize();
      }


   }

   return true;
}
