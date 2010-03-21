#include <stdio.h>
#include <strings.h>

#include "pasm_types.h"

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

char* pasm_get_symbol ( int symbol )
{
   return stab[symbol].symbol;
}

int pasm_get_symbol_linenum ( int symbol )
{
   return stab[symbol].ir->source_linenum;
}

int pasm_get_symbol_data ( int symbol, char* data, int size )
{
   ir_table* ptr = stab[symbol].ir;
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

symbol_type pasm_get_symbol_type ( int symbol )
{
   symbol_type value;
   if ( stab[symbol].expr )
   {
      value = symbol_global;
   }
   else
   {
      value = symbol_label;
   }
   return value;
}

int pasm_get_symbol_value ( int symbol )
{
   int value;
   if ( stab[symbol].expr )
   {
      value = evaluate_expression ( stab[symbol].expr );
   }
   else
   {
      value = stab[symbol].ir->addr;
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

