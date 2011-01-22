#include "csourceassembler.h"
#include "cnesicideproject.h"
#include "cbuildertextlogger.h"

#include "pasm_lib.h"

#include "inspectorregistry.h"

#include "dbg_cnes6502.h"

#include "emulator_core.h"

#include "cmarker.h"

#include "main.h"

extern "C" int PASM_include ( char* objname, char** objdata, int* size );

CSourceAssembler::CSourceAssembler()
{
}

bool CSourceAssembler::assemble()
{
   CSourceItem* rootSource = nesicideProject->getProject()->getMainSource();
   CPRGROMBanks* prgRomBanks = nesicideProject->getCartridge()->getPrgRomBanks();
   char* romData = NULL;
   int romLength = 0;
   QString strBuffer1;
   QString strBuffer2;
   char* errors;
   int numErrors;
   int marker;

   if (!rootSource)
   {
      buildTextLogger.write("Error: No main source has been defined.");
      buildTextLogger.write("<i>You can select the main source to compile in the project properties dialog.</i>");
      return false;
   }

   buildTextLogger.write("&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Assembling '" + rootSource->name() + "'...");

   pasm_assemble ( rootSource->name().toAscii().constData(),
                   rootSource->get_sourceCode().toAscii().constData(),
                   &romData,
                   &romLength,
                   PASM_include );

   // Dump errors...
   strBuffer1.sprintf ( "%d (0x%x)", romLength, romLength );
   numErrors = pasm_get_num_errors ();

   if ( numErrors )
   {
      strBuffer2.sprintf ( "%d", numErrors );
      buildTextLogger.write("&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Assembled " + strBuffer1 + " bytes with " + strBuffer2 + " errors...<br />");
      pasm_get_errors ( &errors );
      strBuffer1.sprintf ( "%s", errors );
      buildTextLogger.write("<font color='red'><pre>" + strBuffer1 + "</pre></font>");
   }
   else
   {
      buildTextLogger.write("&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Assembled " + strBuffer1 + " bytes with no errors...<br />");

      int oldBanks = prgRomBanks->getPrgRomBanks().count();
      int bankIdx = 0;

      // Set up PRG-ROM banks...
      for ( ; romLength > 0; romLength -= MEM_16KB, romData += MEM_16KB )
      {
         // Grab either a previously used bank, or a new one
         CPRGROMBank* curBank;
         bool doAppend = (--oldBanks < 0);

         // Initialize the bank into the project banks
         if (doAppend)
         {
            curBank = new CPRGROMBank(nesicideProject->getCartridge()->getPrgRomBanks());
            // This is a new bank
            curBank->setBankIndex(prgRomBanks->getPrgRomBanks().count());
            prgRomBanks->appendChild(curBank);
            prgRomBanks->getPrgRomBanks().append(curBank);
         }
         else
         {
            curBank = prgRomBanks->getPrgRomBanks().at(bankIdx++);
         }

         curBank->setBankData ( (quint8*)romData );
      }

      // Add PermanentMarkers if any were found...
      if ( pasm_get_num_permanent_markers() )
      {
         for ( marker = 0; marker < NUM_PERMANENT_MARKERS; marker++ )
         {
            if ( pasm_is_permanent_marker_set(marker) )
            {
               nesGetExecutionMarkerDatabase()->AddSpecificMarker(marker,pasm_get_permanent_marker_start_address(marker));
               nesGetExecutionMarkerDatabase()->CompleteMarker(marker,pasm_get_permanent_marker_end_address(marker));
            }
         }
      }
   }

   return numErrors?false:true;
}
