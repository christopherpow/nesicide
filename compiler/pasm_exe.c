#include <stdio.h>
#include <stdlib.h>

#include "pasm_types.h"

extern unsigned char check_fixup ( void );

//extern char currentFile[];

int yyparse();

int main(int argc, char **argv)
{
   unsigned char error;
   char* buffer = NULL;
   int length = 0;
   int tries = 0;
   int promoted = -1;
   FILE* output;

   if ((argc > 1) && (freopen(argv[1], "r", stdin) == NULL))
   {
      fprintf ( stderr, "error: cannot open input file: %s\n", argv[1] );
      exit( 1 );
   }
   
//   strcpy ( currentFile, "" );

   add_binary_bank ( data_segment, ANONYMOUS_BANK );
   add_binary_bank ( text_segment, ANONYMOUS_BANK );

   // Parse language to intermediate representation...
   yyparse();

   // Reduce all expressions that had symbol references that weren't reducible...
   reduce_expressions ();

   // Lather, rinse, and repeat fixing and reducing until we're DONE...
   do
   {
      // Promote to zeropage if possible...
      // But don't bother fixing relatives yet...
      promoted = promote_instructions ( PROMOTE );
   } while ( (promoted > 0) && ((++tries) < MAX_FIXUP_TRIES) );

   // Now bother with the relatives...
   promoted = promote_instructions ( FIX );

   // There should not have been any promotions in the last
   // run...check for that?  Maybe just let fixup spit out the errors...

   // Provide errors for incomplete fixup (undefined references)...
   check_fixup ();

   // Output final binary representation to buffer...
   output_binary ( &buffer, &length );

   if ( length )
   {
      output = fopen ( "a.out", "wb" );
      if ( output )
      {
         fwrite ( buffer, 1, length, output );
         fclose ( output );
      }
   }

   return 0;
}

