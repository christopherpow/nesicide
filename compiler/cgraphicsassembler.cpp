#include "cgraphicsassembler.h"
#include "cnesicideproject.h"

#include "main.h"

static const char emptyBank[MEM_8KB] = { 0, };

CGraphicsAssembler::CGraphicsAssembler()
{
}

bool CGraphicsAssembler::assemble()
{
   CGraphicsBanks* gfxBanks = nesicideProject->getProject()->getGraphicsBanks();
   QDir outputDir(nesicideProject->getProjectOutputBasePath());
   QString outputName;
   QFile chrRomFile;

   if ( nesicideProject->getProjectCHRROMOutputName().isEmpty() )
   {
      outputName = outputDir.toNativeSeparators(outputDir.filePath(nesicideProject->getProjectOutputName()+".chr"));
   }
   else
   {
      outputName = outputDir.toNativeSeparators(outputDir.filePath(nesicideProject->getProjectCHRROMOutputName()));
   }

   if ( gfxBanks->getGraphicsBanks().count() )
   {
      buildTextLogger->write("<b>Building: "+outputName+"</b>");

      chrRomFile.setFileName(outputName);
      chrRomFile.open(QIODevice::ReadWrite|QIODevice::Truncate);
      if ( chrRomFile.isOpen() )
      {
         for (int gfxBankIdx = 0; gfxBankIdx < gfxBanks->getGraphicsBanks().count(); gfxBankIdx++)
         {
            CGraphicsBank* curGfxBank = gfxBanks->getGraphicsBanks().at(gfxBankIdx);

            buildTextLogger->write("Constructing '" + curGfxBank->name() + "':");

            if ( curGfxBank->getGraphics().count() )
            {
               for (int bankItemIdx = 0; bankItemIdx < curGfxBank->getGraphics().count(); bankItemIdx++)
               {
                  IChrRomBankItem* bankItem = curGfxBank->getGraphics().at(bankItemIdx);
                  IProjectTreeViewItem* ptvi = dynamic_cast<IProjectTreeViewItem*>(bankItem);
                  buildTextLogger->write("&nbsp;&nbsp;&nbsp;Writing "+ptvi->caption()+"("+QString::number(bankItem->getChrRomBankItemSize())+" bytes)");

                  chrRomFile.write(bankItem->getChrRomBankItemData().data(), bankItem->getChrRomBankItemSize());
               }
            }
            else
            {
               // 8KB of empty space
               chrRomFile.write(emptyBank,MEM_8KB);
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
