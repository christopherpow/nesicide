#include <stdio.h>
#include <strings.h>

#include "pasm_lib.h"

extern FILE* asmin;
extern incobj_callback_fn incobj_fn;
extern int btab_ent;
extern binary_table* btab;
extern symbol_list global_stab;
extern symbol_table* current_label;
extern file_table* ftab;

extern error_list etab;
extern int error_count;

extern char currentFile [];

extern permanent_marker_table permanentMarker [];

extern void initialize ( void );

int asmparse();

int pasm_get_num_errors ( void )
{
   return error_count;
}

error_table* pasm_get_error ( int error )
{
   int e = 0;
   error_table* ptr = etab.head;
   for ( e = 0; e < error; e++ )
   {
      ptr = ptr->next;
   }
   return ptr;
}

int pasm_get_num_source_files ( void )
{
   file_table* ptr = ftab;
   int files = 0;
   
   while ( ptr )
   {
      files++;
      ptr = ptr->next;
   }
   
   return files;
}

file_table* pasm_get_source_file_by_name ( const char* filename )
{
   file_table* ptr = ftab;
   
   while ( ptr )
   {
      if ( strcmp(ptr->name,filename) == 0 )
      {
         break;
      }
      ptr = ptr->next;
   }
   
   return ptr;
}

file_table* pasm_get_source_file_by_index ( int file )
{
   file_table* ptr = ftab;
   int files = 0;
   
   while ( ptr && (files < file) )
   {
      files++;
      ptr = ptr->next;
   }
   
   return ptr;
}

int pasm_get_source_file_index_by_name ( const char* filename )
{
   file_table* ptr = ftab;
   int files = 0;
   
   while ( ptr )
   {
      if ( strcmp(ptr->name,filename) == 0 )
      {
         break;
      }
      files++;
      ptr = ptr->next;
   }
   
   return files;
}

char* pasm_get_source_file_name_by_index ( int file )
{
   file_table* lfile = pasm_get_source_file_by_index(file);
   
   return lfile->name;
}

char* pasm_get_source_file_name_by_addr ( unsigned int absAddr )
{
   int b, foundbank = -1;
   char* filename = NULL;
   ir_table* ptr;

   // If we couldn't find the address within what we thought
   // was the appropriate bank to be looking in the code is not
   // banked as we thought.  It could be a no-mapper cartridge.
   // Search through each bank for the address...
   if ( filename == NULL )
   {
      for ( b = 0; b < btab_ent; b++ )
      {
         if ( btab[b].type == text_segment )
         {
            for ( ptr = btab[b].ir_head; ptr != NULL; ptr = ptr->next )
            {
               if ( (ptr->instr) && 
                    ((absAddr >= ptr->absAddr) && 
                    (absAddr < (ptr->absAddr+ptr->len))) )
               {
                  if ( ptr->file )
                  {
                     filename = ptr->file->name;
                     break;
                  }
               }
            }
         }
      }
   }

   return filename;
}

char* pasm_get_source_file_text_by_index ( int file )
{
   file_table* lfile = pasm_get_source_file_by_index(file);
   
   return lfile->text;
}

char* pasm_get_source_file_text_by_addr ( unsigned int absAddr )
{
   int b, foundbank = -1;
   char* filetext = NULL;
   ir_table* ptr;

   // If we couldn't find the address within what we thought
   // was the appropriate bank to be looking in the code is not
   // banked as we thought.  It could be a no-mapper cartridge.
   // Search through each bank for the address...
   if ( filetext == NULL )
   {
      for ( b = 0; b < btab_ent; b++ )
      {
         if ( btab[b].type == text_segment )
         {
            for ( ptr = btab[b].ir_head; ptr != NULL; ptr = ptr->next )
            {
               if ( (ptr->instr) && 
                    ((absAddr >= ptr->absAddr) && 
                    (absAddr < (ptr->absAddr+ptr->len))) )
               {
                  if ( ptr->file )
                  {
                     filetext = ptr->file->text;
                     break;
                  }
               }
            }
         }
      }
   }

   return filetext;
}

int pasm_get_source_linenum_by_absolute_addr ( unsigned int absAddr )
{
   int b, foundbank = -1;
   int linenum = -1;
   ir_table* ptr;

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
               if ( (ptr->instr) && 
                    ((absAddr >= ptr->absAddr) && 
                    (absAddr < (ptr->absAddr+ptr->len))) )
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

unsigned int pasm_get_source_addr_by_linenum ( int linenum )
{
   int b;
   unsigned int addr = -1;
   ir_table* ptr;

   // Search through all banks looking for the appropriate source line number...
   // CPTODO: translate this to be smarter about source files also!
   for ( b = 0; b < btab_ent; b++ )
   {
      if ( btab[b].type == text_segment )
      {
         for ( ptr = btab[b].ir_head; ptr != NULL; ptr = ptr->next )
         {
            if ( (ptr->instr) && (ptr->source_linenum == linenum) )
            {
               addr = ptr->addr;
               break;
            }
         }
      }
   }

   return addr;
}

unsigned int pasm_get_source_addr_by_linenum_and_file ( int linenum, const char* file )
{
   int b;
   unsigned int addr = -1;
   ir_table* ptr;

   // Search through all banks looking for the appropriate source line number...
   // CPTODO: translate this to be smarter about source files also!
   for ( b = 0; b < btab_ent; b++ )
   {
      if ( btab[b].type == text_segment )
      {
         for ( ptr = btab[b].ir_head; ptr != NULL; ptr = ptr->next )
         {
            if ( (ptr->instr) && (ptr->source_linenum == linenum) &&
                 (ptr->file) && (strcmp(ptr->file->name,file) == 0) )
            {
               addr = ptr->addr;
               break;
            }
         }
      }
   }

   return addr;
}

unsigned int pasm_get_source_absolute_addr_by_linenum_and_file ( int linenum, const char* file )
{
   int b;
   unsigned int absAddr = -1;
   ir_table* ptr;

   // Search through all banks looking for the appropriate source line number...
   // CPTODO: translate this to be smarter about source files also!
   for ( b = 0; b < btab_ent; b++ )
   {
      if ( btab[b].type == text_segment )
      {
         for ( ptr = btab[b].ir_head; ptr != NULL; ptr = ptr->next )
         {
            if ( (ptr->instr) && (ptr->source_linenum == linenum) &&
                 (ptr->file) && (strcmp(ptr->file->name,file) == 0) )
            {
               absAddr = ptr->absAddr;
               break;
            }
         }
      }
   }

   return absAddr;
}

int pasm_check_for_instruction_at_absolute_addr ( unsigned int absAddr )
{
   int b;
   int instr = 0;
   ir_table* ptr;

   // Search through all banks looking for the appropriate source line number...
   // CPTODO: translate this to be smarter about source files also!
   for ( b = 0; b < btab_ent; b++ )
   {
      if ( btab[b].type == text_segment )
      {
         for ( ptr = btab[b].ir_head; ptr != NULL; ptr = ptr->next )
         {
            if ( (ptr->instr) && (ptr->absAddr == absAddr) )
            {
               instr = 1;
               break;
            }
         }
      }
   }

   return instr;
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

symbol_table* pasm_get_symbol_by_index ( int symbol )
{
   return pasm_get_symbol_entry ( symbol );
}

int pasm_get_symbol_linenum_by_name ( const char* symbol )
{
   symbol_table* ptr = find_symbol ( symbol );
   
   if ( ptr && ptr->ir )
   {
      return ptr->ir->source_linenum;
   }
   
   return -1;
}

char* pasm_get_symbol_name_by_index ( int symbol )
{
   symbol_table* ptr = pasm_get_symbol_entry ( symbol );

   return ptr->symbol;
}

int pasm_get_symbol_linenum_by_index ( int symbol )
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

int pasm_get_symbol_data_by_index ( int symbol, char* data, int size )
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

symbol_type pasm_get_symbol_type_by_index ( int symbol )
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

int pasm_get_symbol_value_by_index ( int symbol )
{
   symbol_table* ptr = pasm_get_symbol_entry ( symbol );

   int value;
   int evaluated = 1;
   if ( ptr->expr )
   {
      evaluate_expression ( ptr->ir, ptr->expr, &evaluated, 0, NULL );
      if ( evaluated && (ptr->expr->vtype == value_is_int) )
      {
         value = ptr->expr->value.ival;
      }
      else
      {
         value = 0;
      }
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

int pasm_get_num_permanent_markers ( void )
{
   int color;
   int markers = 0;

   for ( color = 0; color < NUM_PERMANENT_MARKERS; color++ )
   {
      if ( permanentMarker[color].start )
      {
         markers++;
      }
   }

   return markers;
}

int pasm_is_permanent_marker_set ( int color )
{
   int set = 0;
   if ( permanentMarker[color].start )
   {
      set = 1;
   }
   return set;
}

unsigned int pasm_get_permanent_marker_start_address ( int color )
{
   return permanentMarker[color].start->absAddr;
}

unsigned int pasm_get_permanent_marker_end_address ( int color )
{
   return permanentMarker[color].end->absAddr;
}

void pasm_initialize ( void )
{
   initialize ();
}

int pasm_assemble( const char* name, const char* buffer_in, char** buffer_out, int* size, incobj_callback_fn incobj )
{
   int tries = 0;
   int promoted;
   char* buffer = NULL;
   int length = 0;

   incobj_fn = incobj;

   strcpy ( currentFile, name );

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
   
   current_label = NULL;

   // Provide errors for incomplete fixup (undefined references)...
   check_fixup ();

   // Output final binary representation to buffer...
   output_binary ( buffer_out, size );

   return 0;
}
