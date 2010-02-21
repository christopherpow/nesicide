#include <stdio.h>

#include "pasm_types.h"

// prototype of bison-generated parser function

int errorCount = 0;
extern FILE* yyin;
extern incobj_callback_fn incobj_fn;
extern symbol_table* stab;
extern int stab_ent;

//extern char currentFile[];

extern void initialize ( void );

int yyparse();

char* errorStorage = NULL;

void pasm_get_errors ( char** errors )
{
   (*errors) = errorStorage;
}

int pasm_get_num_errors ( void )
{
   return errorCount;
}

void pasm_get_symbols ( symbol_table** symbols )
{
   (*symbols) = stab;
}

int pasm_get_symbol_data ( symbol_table* symbol, char* data, int size )
{
   ir_table* ptr = symbol->ir;
   int bytes = 0;

   if ( ptr != NULL )
   {
      do
      {
         memcpy ( data, ptr->data, ptr->len );
         data += ptr->len;
         size -= ptr->len;
         bytes += ptr->len;
         ptr = ptr->next;
      } while ( (ptr != NULL) && (size > 0) );
   }

   return bytes;
}

int pasm_get_num_symbols ( void )
{
   return stab_ent;
}

int pasm_assemble( const char* buffer_in, char** buffer_out, int* size, incobj_callback_fn incobj )
{
   incobj_fn = incobj;

   initialize ();

   add_binary_bank ( data_segment, ANONYMOUS_BANK );
   add_binary_bank ( text_segment, ANONYMOUS_BANK );

   yy_delete_buffer ();

   yy_scan_string ( buffer_in );
   yyin = NULL;

   yyparse();

   check_fixup ();
   output_binary_direct ( buffer_out, size );

   return 0;
}

