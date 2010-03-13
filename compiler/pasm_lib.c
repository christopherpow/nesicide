#include <stdio.h>

#include "pasm_types.h"

// prototype of bison-generated parser function

extern FILE* yyin;
extern incobj_callback_fn incobj_fn;
extern symbol_table* stab;
extern int stab_ent;

extern char* errorStorage;
extern int errorCount;

//extern char currentFile[];

extern void initialize ( void );

int yyparse();

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

symbol_type pasm_get_symbol_type ( struct _symbol_table* symbol )
{
   symbol_type value;
   if ( symbol->expr )
   {
      value = symbol_global;
   }
   else
   {
      value = symbol_label;
   }
   return value;
}

int pasm_get_symbol_value ( symbol_table* symbol )
{
   int value;
   if ( symbol->expr )
   {
      value = evaluate_expression ( symbol->expr );
   }
   else
   {
      value = symbol->ir->addr;
   }
   return value;
}

int pasm_get_num_symbols ( void )
{
   return stab_ent;
}

int pasm_assemble( const char* buffer_in, char** buffer_out, int* size, incobj_callback_fn incobj )
{
   int tries = 0;
   int promoted;

   incobj_fn = incobj;

   initialize ();

   add_binary_bank ( data_segment, ANONYMOUS_BANK );
   add_binary_bank ( text_segment, ANONYMOUS_BANK );

   yy_delete_buffer ();

   yy_scan_string ( buffer_in );
   yyin = NULL;

   // Parse language to intermediate representation...
   yyparse();

   // Reduce all expressions that had symbol references that weren't reducible...
   reduce_expressions ();

   // Lather, rinse, and repeat fixing and reducing until we're DONE...
   do
   {
      // Promote to zeropage if possible...
      // But don't bother fixing relatives yet...
      promoted = promote_instructions ( 0 );
   } while ( (promoted > 0) && ((++tries) < MAX_FIXUP_TRIES) );

   // Now bother with the relatives...
   promoted = promote_instructions ( 1 );

   // There should not have been any promotions in the last
   // run...check for that?  Maybe just let fixup spit out the errors...

   // Provide errors for incomplete fixup (undefined references)...
   check_fixup ();

   // Output final binary representation to buffer...
   output_binary ( buffer_out, size );

   return 0;
}

