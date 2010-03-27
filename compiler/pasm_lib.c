#include <stdio.h>
#include <strings.h>

#include "pasm_types.h"

extern FILE* yyin;
extern incobj_callback_fn incobj_fn;
extern symbol_list* stab;

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
   symbol_table* ptr = stab->head;
   int i = 0;

   while ( i < symbol )
   {
      i++;
      ptr = ptr->next;
   }

   return ptr->symbol;
}

int pasm_get_symbol_linenum ( int symbol )
{
   symbol_table* ptr = stab->head;
   int i = 0;

   while ( i < symbol )
   {
      i++;
      ptr = ptr->next;
   }

   return ptr->ir->source_linenum;
}

int pasm_get_symbol_data ( int symbol, char* data, int size )
{
   int bytes = 0;
#if 0
// CPTODO: implement when needed
   ir_table* ptr = stab[symbol].ir;

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

#endif
   return bytes;
}

symbol_type pasm_get_symbol_type ( int symbol )
{
   symbol_table* ptr = stab->head;
   int i = 0;

   while ( i < symbol )
   {
      i++;
      ptr = ptr->next;
   }

   symbol_type value;
   if ( ptr->expr )
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
   symbol_table* ptr = stab->head;
   int i = 0;

   while ( i < symbol )
   {
      i++;
      ptr = ptr->next;
   }

   int value;
   int evaluated = 1;
   if ( ptr->expr )
   {
      value = evaluate_expression ( ptr->ir, ptr->expr, &evaluated, 0, NULL );
   }
   else
   {
      value = ptr->ir->addr;
   }
   return value;
}

int pasm_get_num_symbols ( void )
{
   symbol_table* ptr;
   int i = 0;

   if ( stab )
   {
      ptr = stab->head;
      while ( ptr != NULL )
      {
         i++;
         ptr = ptr->next;
      }
   }

   return i;
}

int pasm_assemble( const char* buffer_in, char** buffer_out, int* size, incobj_callback_fn incobj )
{
   int tries = 0;
   int promoted;

   incobj_fn = incobj;

   initialize ();

   add_binary_bank ( text_segment, NULL );

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
      promoted = promote_instructions ( PROMOTE );
   } while ( (promoted > 0) && ((++tries) < MAX_FIXUP_TRIES) );

   // Now bother with the relatives...
   promoted = promote_instructions ( FIX );

   // There should not have been any promotions in the last
   // run...check for that?  Maybe just let fixup spit out the errors...

   // Provide errors for incomplete fixup (undefined references)...
   check_fixup ();

   // Output final binary representation to buffer...
   output_binary ( buffer_out, size );

   return 0;
}

