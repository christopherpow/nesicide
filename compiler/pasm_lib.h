#if !defined ( PASM_LIB_H )
#define PASM_LIB_H

#include "pasm_types.h"

#if defined ( __cplusplus )
extern "C" {
#endif

// Assembler main interface.
int pasm_assemble( const char* name, const char* buffer_in, char** buffer_out, int* size, incobj_callback_fn incobj );

// Interfaces to retrieve errors generated
// during the most recent assemble.  Note this information
// is destroyed and re-created on subsequent calls to
// pasm_assemble, so no dependence should be placed on its
// permanent existence.  [If you want it later, copy it!]
void pasm_get_errors ( char** errors );
int pasm_get_num_errors ( void );

// Interfaces to retrieve symbol information generated
// during the most recent assemble.  Note this information
// is destroyed and re-created on subsequent calls to
// pasm_assemble, so no dependence should be placed on its
// permanent existence.  [If you want it later, copy it!]
int pasm_get_num_symbols ( void );
char* pasm_get_symbol ( int symbol );
int pasm_get_symbol_data ( int symbol, char* data, int size );
int pasm_get_symbol_value ( int symbol );
symbol_type pasm_get_symbol_type ( int symbol );
int pasm_get_symbol_linenum ( int symbol );
int pasm_get_source_linenum ( unsigned int absAddr );
unsigned int pasm_get_source_addr_from_linenum ( int linenum );

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
