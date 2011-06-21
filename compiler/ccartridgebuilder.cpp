#include "ccartridgebuilder.h"

#include "ccc65interface.h"
#include "cnesicideproject.h"
#include "main.h"

CCartridgeBuilder::CCartridgeBuilder()
{
}

void CCartridgeBuilder::clean()
{
   CSourceAssembler sourceAssembler;
   CGraphicsAssembler graphicsAssembler;

   buildTextLogger->erase();
   buildTextLogger->write("<b>Project build started.</b>");

   if ( (CCC65Interface::getCLanguageSourcesFromProject().count()) ||
        (CCC65Interface::getAssemblerSourcesFromProject().count()) )
   {
      sourceAssembler.clean();
      graphicsAssembler.clean();
   }
   else
   {
      buildTextLogger->write("<font color='red'>Nothing to clean.</font>");
   }

   buildTextLogger->write("<b>Build completed successfully.</b>");
}

bool CCartridgeBuilder::build()
{
   CSourceAssembler sourceAssembler;
   CGraphicsAssembler graphicsAssembler;
   CGraphicsBanks* gfxBanks = nesicideProject->getProject()->getGraphicsBanks();
   QDir baseDir(QDir::currentPath());
   QDir outputDir(nesicideProject->getProjectOutputBasePath());
   QString prgName;
   QString chrName;
   QString nesName;
   QFile prgFile;
   QFile chrFile;
   QFile nesFile;

   if ( nesicideProject->getProjectLinkerOutputName().isEmpty() )
   {
      prgName = outputDir.fromNativeSeparators(outputDir.filePath(nesicideProject->getProjectOutputName()+".prg"));
   }
   else
   {
      prgName = outputDir.fromNativeSeparators(outputDir.filePath(nesicideProject->getProjectLinkerOutputName()));
   }
   if ( nesicideProject->getProjectCHRROMOutputName().isEmpty() )
   {
      chrName = outputDir.fromNativeSeparators(outputDir.filePath(nesicideProject->getProjectOutputName()+".chr"));
   }
   else
   {
      chrName = outputDir.fromNativeSeparators(outputDir.filePath(nesicideProject->getProjectCHRROMOutputName()));
   }
   if ( nesicideProject->getProjectCartridgeOutputName().isEmpty() )
   {
      nesName = baseDir.fromNativeSeparators(baseDir.relativeFilePath(nesicideProject->getProjectOutputName()+".nes"));
   }
   else
   {
      nesName = baseDir.fromNativeSeparators(baseDir.relativeFilePath(nesicideProject->getProjectCartridgeOutputName()));
   }

   prgFile.setFileName(prgName);
   if ( gfxBanks->getGraphicsBanks().count() )
   {
      chrFile.setFileName(chrName);
   }
   nesFile.setFileName(nesName);

   buildTextLogger->erase();
   buildTextLogger->write("<b>Project build started.</b>");

   if ( (CCC65Interface::getCLanguageSourcesFromProject().count()) ||
        (CCC65Interface::getAssemblerSourcesFromProject().count()) )
   {
      if (!(sourceAssembler.assemble() && graphicsAssembler.assemble()))
      {
         buildTextLogger->write("<font color='red'><b>Build failed.</b></font>");
         return false;
      }

      prgFile.open(QIODevice::ReadOnly);
      if ( gfxBanks->getGraphicsBanks().count() )
      {
         chrFile.open(QIODevice::ReadOnly);
      }
      nesFile.open(QIODevice::ReadWrite|QIODevice::Truncate);

      if ( prgFile.isOpen() &&
           (((gfxBanks->getGraphicsBanks().count()) && (chrFile.isOpen())) ||
           (!(gfxBanks->getGraphicsBanks().count()))) &&
           nesFile.isOpen() )
      {
         QByteArray prgBytes = prgFile.readAll();
         QByteArray chrBytes;

         if ( gfxBanks->getGraphicsBanks().count() )
         {
            chrBytes = chrFile.readAll();
         }

         nesFile.write(prgBytes);

         if ( gfxBanks->getGraphicsBanks().count() )
         {
            nesFile.write(chrBytes);
         }

         buildTextLogger->write("<b>Writing: "+nesName+"</b>");

         prgFile.close();
         chrFile.close();
         nesFile.close();
      }
      else
      {
         buildTextLogger->write("<font color='red'><b>Build failed.</b></font>");
         return false;
      }
   }
   else
   {
      buildTextLogger->write("<font color='red'>Nothing to build.</font>");
   }
   buildTextLogger->write("<b>Build completed successfully.</b>");
   return true;
}
