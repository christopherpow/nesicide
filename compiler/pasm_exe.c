#include <stdio.h>

#include "pasm_types.h"

// prototype of bison-generated parser function

extern unsigned char check_fixup ( void );

char* errorStorage = NULL;
//extern char currentFile[];

int errorCount = 0;

int yyparse();

int main(int argc, char **argv)
{
   unsigned char error;
   char* buffer = NULL;
   int length;
   FILE* output;

   if ((argc > 1) && (freopen(argv[1], "r", stdin) == NULL))
   {
      fprintf ( stderr, "error: cannot open input file: %s\n", argv[1] );
      exit( 1 );
   }
   
//   strcpy ( currentFile, "" );

   add_binary_bank ( data_segment, ANONYMOUS_BANK );
   add_binary_bank ( text_segment, ANONYMOUS_BANK );

   yyparse();

   check_fixup ();
   output_binary_direct ( &buffer, &length );

   output = fopen ( "a.out", "wb" );
   if ( output )
   {
      fwrite ( buffer, 1, length, output );
      fclose ( output );
   }

//   dump_symbol_table ();
//   dump_fixup_table ();

   return 0;
}

