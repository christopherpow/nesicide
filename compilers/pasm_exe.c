#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

#include "pasm_types.h"

extern FILE* asmin;

extern char currentFile [];

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

   if ( argc == 1 )
   {
      strcpy ( currentFile, "stdin" );
   }
   else
   {
      strcpy ( currentFile, argv[1] );
   }
   
   initialize ();
   
   preprocess ( NULL, &buffer, &length );

   add_binary_bank ( text_segment, NULL );

   asm_scan_string ( buffer );
   asmin = NULL;

   // Parse language to intermediate representation...
   asmparse();

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

   initialize ();
   return 0;
}

