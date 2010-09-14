#include "csourceassembler.h"
#include "cnesicideproject.h"
#include "cbuildertextlogger.h"

#include "pasm_lib.h"

#include "inspectorregistry.h"

#include "dbg_cnes6502.h"

#include "emulator_core.h"

#include "cmarker.h"

extern "C" int PASM_include ( char* objname, char** objdata, int* size );

CSourceAssembler::CSourceAssembler()
{
}

bool CSourceAssembler::assemble()
{
   CSourceItem *rootSource = nesicideProject->getProject()->getMainSource();
   CPRGROMBanks *prgRomBanks = nesicideProject->get_pointerToCartridge()->getPointerToPrgRomBanks();
   char* romData = NULL;
   int romLength = 0;
   QString strBuffer;
   char* errors;
   int numErrors;
   int numSymbols;
   int symbol;
   int marker;

   if (!rootSource)
   {
      builderTextLogger.write("Error: No main source has been defined.");
      builderTextLogger.write("<i>You can select the main source to compile in the project properties dialog.</i>");
      return false;
   }

   builderTextLogger.write("&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Assembling '" + rootSource->get_sourceName() + "'...");

   pasm_assemble ( rootSource->get_sourceName().toLatin1().data(),
                   rootSource->get_sourceCode().toLatin1().data(),
                   &romData,
                   &romLength,
                   PASM_include );

   // Dump errors...
   strBuffer.sprintf ( "%d (0x%x)", romLength, romLength );
   numErrors = pasm_get_num_errors ();
   if ( numErrors )
   {
      strBuffer.sprintf ( "%d", numErrors );
      builderTextLogger.write("&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Assembled " + strBuffer + " bytes with " + strBuffer + " errors...");
      pasm_get_errors ( &errors );
      strBuffer.sprintf ( "%s", errors );
      builderTextLogger.write("<font color='red'><pre>" + strBuffer + "</pre></font>");
   }
   else
   {
      builderTextLogger.write("&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Assembled " + strBuffer + " bytes with no errors...");
      /*
      numSymbols = pasm_get_num_symbols();
      strBuffer.sprintf("<b>Symbol Table (%d symbols defined)</b>", numSymbols);
      builderTextLogger.write(strBuffer);
      for ( symbol = 0; symbol < numSymbols; symbol++ )
      {
         strBuffer.sprintf("&nbsp;&nbsp;&nbsp;%s&nbsp;&nbsp;&nbsp;%s&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;$%04X at line %d",
                           (pasm_get_symbol_type(symbol)==symbol_global)?"GLOBAL":"LABEL",
                           pasm_get_symbol(symbol),
                           pasm_get_symbol_value(symbol),
                           pasm_get_symbol_linenum(symbol));
         builderTextLogger.write(strBuffer);
      }*/

      int oldBanks = prgRomBanks->get_pointerToArrayOfBanks()->count();
      int bankIdx = 0;

      // Set up PRG-ROM banks...
      for ( ; romLength > 0; romLength -= 0x4000, romData += 0x4000 )
      {
         // Grab either a previously used bank, or a new one
         CPRGROMBank *curBank;
         bool doAppend = (--oldBanks < 0);

         // Initialize the bank into the project banks
         if (doAppend) {
            curBank = new CPRGROMBank();
            // This is a new bank
            curBank->set_indexOfPrgRomBank(prgRomBanks->get_pointerToArrayOfBanks()->count());
            curBank->InitTreeItem(prgRomBanks);
            prgRomBanks->appendChild(curBank);
            prgRomBanks->get_pointerToArrayOfBanks()->append(curBank);
        } else {
            curBank = prgRomBanks->get_pointerToArrayOfBanks()->at(bankIdx++);
        }
         curBank->set_pointerToBankData ( (quint8*)romData );
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

   if ( !numErrors )
   {
      QDockWidget* pCodeBrowser = InspectorRegistry::getInspector ( "Code Browser" );
      if ( pCodeBrowser->isVisible() )
      {
         pCodeBrowser->hide();
         pCodeBrowser->show();
      }
   }

   return numErrors?false:true;
}
