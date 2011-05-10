#include "ccartridgebuilder.h"

#include "ccc65interface.h"
#include "cnesicideproject.h"
#include "main.h"

CCartridgeBuilder::CCartridgeBuilder()
{
}

bool CCartridgeBuilder::build()
{
   CSourceAssembler sourceAssembler;
   CGraphicsAssembler graphicsAssembler;
   QDir outputDir(nesicideProject->getProjectOutputBasePath());
   QString prgName;
   QString chrName;
   QString nesName;
   QFile prgFile;
   QFile chrFile;
   QFile nesFile;

   if ( nesicideProject->getProjectLinkerOutputName().isEmpty() )
   {
      prgName = outputDir.toNativeSeparators(outputDir.filePath(nesicideProject->getProjectOutputName()+".prg"));
   }
   else
   {
      prgName = outputDir.toNativeSeparators(outputDir.filePath(nesicideProject->getProjectLinkerOutputName()));
   }
   if ( nesicideProject->getProjectCHRROMOutputName().isEmpty() )
   {
      chrName = outputDir.toNativeSeparators(outputDir.filePath(nesicideProject->getProjectOutputName()+".chr"));
   }
   else
   {
      chrName = outputDir.toNativeSeparators(outputDir.filePath(nesicideProject->getProjectCHRROMOutputName()));
   }
   if ( nesicideProject->getProjectCartridgeOutputName().isEmpty() )
   {
      nesName = outputDir.toNativeSeparators(outputDir.filePath(nesicideProject->getProjectOutputName()+".nes"));
   }
   else
   {
      nesName = outputDir.toNativeSeparators(outputDir.filePath(nesicideProject->getProjectCartridgeOutputName()));
   }

   prgFile.setFileName(prgName);
   chrFile.setFileName(chrName);
   nesFile.setFileName(nesName);

   buildTextLogger->erase();
   buildTextLogger->write("<b>Project build started.</b>");

   if (!(sourceAssembler.assemble() && graphicsAssembler.assemble()))
   {
      buildTextLogger->write("<font color='red'><b>Build failed.</b></font>");
      return false;
   }

   prgFile.open(QIODevice::ReadOnly);
   chrFile.open(QIODevice::ReadOnly);
   nesFile.open(QIODevice::ReadWrite|QIODevice::Truncate);

   if ( prgFile.isOpen() &&
        chrFile.isOpen() &&
        nesFile.isOpen() )
   {
      QByteArray prgBytes = prgFile.readAll();
      QByteArray chrBytes = chrFile.readAll();

      nesFile.write(prgBytes);
      nesFile.write(chrBytes);

      buildTextLogger->write("<b>Writing "+nesName+"</b>");

      prgFile.close();
      chrFile.close();
      nesFile.close();
   }

   buildTextLogger->write("<b>Build completed successfully.</b>");
   return true;
}
