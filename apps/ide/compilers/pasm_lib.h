#if !defined ( PASM_LIB_H )
#define PASM_LIB_H

#include "pasm_types.h"

#if defined ( __cplusplus )
extern "C" {
#endif

// Assembler initialization.
   void pasm_initialize ( void );
   
// Assembler main interface.
   int pasm_assemble( const char* name, const char* buffer_in, char** buffer_out, int* size, incobj_callback_fn incobj );

// Interfaces to retrieve errors generated
// during the most recent assemble.  Note this information
// is destroyed and re-created on subsequent calls to
// pasm_assemble, so no dependence should be placed on its
// permanent existence.  [If you want it later, copy it!]
   int pasm_get_num_errors ( void );
   error_table* pasm_get_error ( int error );
   
// Interface to retrieve file information generated during
// the most recent assemble.  Note this information
// is destroyed and re-created on subsequent calls to
// pasm_assemble, so no dependence should be placed on its
// permanent existence.  [If you want it later, copy it!]
   int pasm_get_num_source_files ( void );
   file_table* pasm_get_source_file_by_index ( int file );
   file_table* pasm_get_source_file_by_name ( const char* filename );
   char* pasm_get_source_file_text_by_addr ( unsigned int absAddr );
   char* pasm_get_source_file_text_by_index ( int file );
   char* pasm_get_source_file_name_by_addr ( unsigned int absAddr );
   char* pasm_get_source_file_name_by_index ( int file );
   int pasm_get_source_file_index_by_name ( const char* filename );
   
// Interfaces to retrieve symbol information generated
// during the most recent assemble.  Note this information
// is destroyed and re-created on subsequent calls to
// pasm_assemble, so no dependence should be placed on its
// permanent existence.  [If you want it later, copy it!]
   int pasm_get_num_symbols ( void );
   symbol_table* pasm_get_symbol_by_index ( int symbol );
   char* pasm_get_symbol_name_by_index ( int symbol );
   int pasm_get_symbol_data_by_index ( int symbol, char* data, int size );
   int pasm_get_symbol_value_by_index ( int symbol );
   symbol_type pasm_get_symbol_type_by_index ( int symbol );
   int pasm_get_symbol_linenum_by_index ( int symbol );
   int pasm_get_symbol_linenum_by_name ( const char* symbol );
   
// Interfaces to convert between source code line number and assembled address.
// Note this information is destroyed and re-created on subsequent calls to
// pasm_assemble, so no dependence should be placed on its
// permanent existence.  [If you want it later, copy it!]
   int pasm_get_source_linenum_by_absolute_addr ( unsigned int absAddr );
   unsigned int pasm_get_source_addr_by_linenum ( int linenum );
   unsigned int pasm_get_source_addr_by_linenum_and_file ( int linenum, const char* file );
   unsigned int pasm_get_source_absolute_addr_by_linenum_and_file ( int linenum, const char* file );
   int pasm_check_for_instruction_at_absolute_addr ( unsigned int absAddr );

// Interfaces to retrieve PermanentMarker information generated
// during the most recent assemble.  Note this information
// is destroyed and re-created on subsequent calls to
// pasm_assemble, so no dependence should be placed on its
// permanent existence.  [If you want it later, copy it!]
   int pasm_get_num_permanent_markers ( void );
   int pasm_is_permanent_marker_set ( int color );
   unsigned int pasm_get_permanent_marker_start_address ( int color );
   unsigned int pasm_get_permanent_marker_end_address ( int color );

#if defined ( __cplusplus )
}
#endif

#endif
