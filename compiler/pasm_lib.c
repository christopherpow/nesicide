#include <stdio.h>
#include <strings.h>

#include "pasm_lib.h"

extern FILE* asmin;
extern incobj_callback_fn incobj_fn;
extern int btab_ent;
extern binary_table* btab;
extern symbol_list global_stab;

extern char* errorStorage;
extern int errorCount;

//extern char currentFile[];

extern void initialize ( void );

int asmparse();

void pasm_get_errors ( char** errors )
{
   (*errors) = errorStorage;
}

int pasm_get_num_errors ( void )
{
   return errorCount;
}

int pasm_get_source_linenum ( unsigned int bank, unsigned int addr )
{
   int b, foundbank = -1;
   int linenum = -1;
   ir_table* ptr;

   // Start by assuming code is banked as if it were going to be in
   // a mapper-enabled cartridge.  This will allow us to be able to
   // trace code in the appropriate bank.
   for ( b = 0; b < btab_ent; b++ )
   {
      if ( btab[b].type == text_segment )
      {
         if ( foundbank == bank )
         {
            break;
         }
         foundbank++;
      }
   }
   // If we found a bank try to find the address within it...
   if ( foundbank >= 0 )
   {
      for ( ptr = btab[foundbank].ir_head; ptr != NULL; ptr = ptr->next )
      {
         if ( (ptr->instr) && (ptr->addr == addr) )
         {
            linenum = ptr->source_linenum;
            break;
         }
      }
   }
   // If we couldn't find the address within what we thought
   // was the appropriate bank to be looking in the code is not
   // banked as we thought.  It could be a no-mapper cartridge.
   // Search through each bank for the address...
   if ( linenum < 0 )
   {
      for ( b = 0; b < btab_ent; b++ )
      {
         if ( btab[b].type == text_segment )
         {
            for ( ptr = btab[b].ir_head; ptr != NULL; ptr = ptr->next )
            {
               if ( (ptr->instr) && (ptr->addr == addr) )
               {
                  linenum = ptr->source_linenum;
                  break;
               }
            }
         }
      }
   }

   return linenum;
}

symbol_table* pasm_get_symbol_entry ( int symbol )
{
   symbol_list* list;
   symbol_table* ptr;
   int bank = 0;
   int i = 0;

   list = &global_stab;
   ptr = list->head;

   while ( (ptr != NULL) && (i < symbol) )
   {
      i++;
      ptr = ptr->next;
      if ( ptr == NULL )
      {
         break;
      }
   }

   if ( (ptr == NULL) || (i < symbol) )
   {
      list = btab[bank].stab;
      ptr = list->head;

      while ( (ptr != NULL) && (i < symbol) )
      {
         i++;
         ptr = ptr->next;
         if ( ptr == NULL )
         {
            bank++;
            list = btab[bank].stab;
            ptr = list->head;
         }
      }
   }

   return ptr;
}

char* pasm_get_symbol ( int symbol )
{
   symbol_table* ptr = pasm_get_symbol_entry ( symbol );

   return ptr->symbol;
}

int pasm_get_symbol_linenum ( int symbol )
{
   symbol_table* ptr = pasm_get_symbol_entry ( symbol );

   if ( ptr->ir )
   {
      return ptr->ir->source_linenum;
   }
   else
   {
      return 0;
   }
}

int pasm_get_symbol_data ( int symbol, char* data, int size )
{
   int bytes = 0;
   symbol_table* ptr = pasm_get_symbol_entry ( symbol );

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
   symbol_table* ptr = pasm_get_symbol_entry ( symbol );

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
   symbol_table* ptr = pasm_get_symbol_entry ( symbol );

   int value;
   int evaluated = 1;
   if ( ptr->expr )
   {
      value = evaluate_expression ( ptr->ir, ptr->expr, &evaluated, 0, NULL );
   }
   else
   {
      if ( ptr->ir )
      {
         value = ptr->ir->addr;
      }
      else
      {
         value = 0;
      }
   }
   return value;
}

int pasm_get_num_symbols ( void )
{
   symbol_list* list;
   symbol_table* ptr;
   int bank;
   int i = 0;

   list = &global_stab;
   ptr = list->head;
   while ( ptr != NULL )
   {
      i++;
      ptr = ptr->next;
   }

   for ( bank = 0; bank < btab_ent; bank++ )
   {
      list = btab[bank].stab;
      ptr = list->head;
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
   char* buffer = NULL;
   int length = 0;

   incobj_fn = incobj;

//   asm_delete_buffer ();

   initialize ();

   preprocess ( buffer_in, &buffer, &length );

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
   output_binary ( buffer_out, size );

   return 0;
}

