#include "ccartridgebuilder.h"

#include "ccc65interface.h"
#include "cnesicideproject.h"

CCartridgeBuilder::CCartridgeBuilder()
{
}

void CCartridgeBuilder::clean()
{
   CSourceAssembler sourceAssembler;
   CGraphicsAssembler graphicsAssembler;

   buildTextLogger->erase();
   buildTextLogger->write("<b>Project build started.</b>");

   if ( (CCC65Interface::instance()->getCLanguageSourcesFromProject().count()) ||
        (CCC65Interface::instance()->getAssemblerSourcesFromProject().count()) ||
        (CCC65Interface::instance()->getCustomSourcesFromProject().count()) )
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
   CGraphicsBanks* gfxBanks = CNesicideProject::instance()->getProject()->getGraphicsBanks();
   QDir baseDir(QDir::currentPath());
   QDir outputPRGDir(CNesicideProject::instance()->getProjectLinkerOutputBasePath());
   QDir outputCHRDir(CNesicideProject::instance()->getProjectCHRROMOutputBasePath());
   QString prgName;
   QString chrName;
   QString nesName;
   QFile prgFile;
   QFile chrFile;
   QFile nesFile;
   bool ok;

   if ( CNesicideProject::instance()->getProjectLinkerOutputName().isEmpty() )
   {
      prgName = outputPRGDir.fromNativeSeparators(outputPRGDir.filePath(CNesicideProject::instance()->getProjectOutputName()+".prg"));
   }
   else
   {
      prgName = outputPRGDir.fromNativeSeparators(outputPRGDir.filePath(CNesicideProject::instance()->getProjectLinkerOutputName()));
   }
   if ( CNesicideProject::instance()->getProjectCHRROMOutputName().isEmpty() )
   {
      chrName = outputCHRDir.fromNativeSeparators(outputCHRDir.filePath(CNesicideProject::instance()->getProjectOutputName()+".chr"));
   }
   else
   {
      chrName = outputCHRDir.fromNativeSeparators(outputCHRDir.filePath(CNesicideProject::instance()->getProjectCHRROMOutputName()));
   }
   if ( CNesicideProject::instance()->getProjectCartridgeOutputName().isEmpty() )
   {
      nesName = baseDir.fromNativeSeparators(baseDir.relativeFilePath(CNesicideProject::instance()->getProjectOutputName()+".nes"));
   }
   else
   {
      nesName = baseDir.fromNativeSeparators(baseDir.relativeFilePath(CNesicideProject::instance()->getProjectCartridgeOutputName()));
   }

   prgFile.setFileName(prgName);
   if ( gfxBanks->getGraphicsBanks().count() )
   {
      chrFile.setFileName(chrName);
   }
   nesFile.setFileName(nesName);

   buildTextLogger->erase();
   buildTextLogger->write("<b>Project build started.</b>");

   if ( (CCC65Interface::instance()->getCLanguageSourcesFromProject().count()) ||
        (CCC65Interface::instance()->getAssemblerSourcesFromProject().count()) ||
        (CCC65Interface::instance()->getCustomSourcesFromProject().count()) )
   {
      if ( !CNesicideProject::instance()->getLinkerConfigFile().isEmpty() )
      {
         ok = graphicsAssembler.assemble();
         if ( ok )
         {
            ok = sourceAssembler.assemble();
            if ( !ok )
            {
               buildTextLogger->write("<font color='red'><b>Build failed while processing Source.</b></font>");
               return false;
            }
         }
         else
         {
            buildTextLogger->write("<font color='red'><b>Build failed while processing Graphics Banks.</b></font>");
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

            if ( (CNesicideProject::instance()->getProjectUsesCHRROM()) &&
                 (gfxBanks->getGraphicsBanks().count()) )
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
         buildTextLogger->write("<font color='red'>Nothing to build, no linker config file specified.  Go to Project menu, select Project Properties, and switch to the Linker tab to add one.</font>");
      }
   }
   else
   {
      buildTextLogger->write("<font color='red'>Nothing to build.</font>");
   }
   buildTextLogger->write("<b>Build completed successfully.</b>");
   return true;
}
