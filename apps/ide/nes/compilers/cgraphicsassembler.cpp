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
   QDir outputDir(nesicideProject->getProjectCHRROMOutputBasePath());
   QString outputName;
   QFile chrRomFile;

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
         for (int gfxBankIdx = 0; gfxBankIdx < gfxBanks->getGraphicsBanks().count(); gfxBankIdx++)
         {
            CGraphicsBank* curGfxBank = gfxBanks->getGraphicsBanks().at(gfxBankIdx);

            buildTextLogger->write("Constructing '" + curGfxBank->caption() + "':");

            if ( curGfxBank->getGraphics().count() )
            {
               for (int bankItemIdx = 0; bankItemIdx < curGfxBank->getGraphics().count(); bankItemIdx++)
               {
                  IChrRomBankItem* bankItem = curGfxBank->getGraphics().at(bankItemIdx);
                  IProjectTreeViewItem* ptvi = dynamic_cast<IProjectTreeViewItem*>(bankItem);
                  buildTextLogger->write("&nbsp;&nbsp;&nbsp;Adding: "+ptvi->caption()+"("+QString::number(bankItem->getChrRomBankItemSize())+" bytes)");

                  chrRomFile.write(bankItem->getChrRomBankItemData().data(), bankItem->getChrRomBankItemSize());
               }
            }
            else
            {
               // 1,2,4, or 8KB of empty space
               chrRomFile.write(emptyBank,curGfxBank->getSize());
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
