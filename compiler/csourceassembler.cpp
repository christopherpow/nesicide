#include "csourceassembler.h"
#include "cnesicideproject.h"
#include "cbuildertextlogger.h"

#include "pasm_lib.h"

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
   int bank;
   int s;

   if (!rootSource)
   {
      builderTextLogger.write("Error: No main source has been defined.");
      builderTextLogger.write("<i>You can select the main source to compile in the project properties dialog.</i>");
      return false;
   }

   builderTextLogger.write("&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Assembling '" + rootSource->get_sourceName() + "'...");

   pasm_assemble ( rootSource->get_sourceName().toLatin1().data(), rootSource->get_sourceCode().toLatin1().data(), &romData, &romLength, NULL );

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
      numSymbols = pasm_get_num_symbols();
      strBuffer.sprintf("<b>Symbol Table (%d symbols defined)</b>", numSymbols);
      builderTextLogger.write(strBuffer);
      for ( s = 0; s < numSymbols; s++ )
      {
         strBuffer.sprintf("&nbsp;&nbsp;&nbsp;%s&nbsp;&nbsp;&nbsp;%s&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;$%04X at line %d",
                           (pasm_get_symbol_type(s)==symbol_global)?"GLOBAL":"LABEL",
                           pasm_get_symbol(s),
                           pasm_get_symbol_value(s),
                           pasm_get_symbol_linenum(s));
         builderTextLogger.write(strBuffer);
      }
   }

   // Remove all banks...

   // Set up PRG-ROM banks...
   for ( ; romLength > 0; romLength -= 0x4000, romData += 0x4000 )
   {
      CPRGROMBank *curBank = new CPRGROMBank();
      curBank->set_indexOfPrgRomBank(prgRomBanks->get_pointerToArrayOfBanks()->count());
      memset(curBank->get_pointerToBankData(), 0, 0x4000);
      curBank->InitTreeItem(prgRomBanks);
      prgRomBanks->appendChild(curBank);
      prgRomBanks->get_pointerToArrayOfBanks()->append(curBank);

      curBank->set_pointerToBankData ( (quint8*)romData );
   }

   return numErrors?false:true;
}
