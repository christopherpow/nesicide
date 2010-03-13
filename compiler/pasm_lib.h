#if !defined ( PASM_LIB_H )
#define PASM_LIB_H

#include "pasm_types.h"

#if defined ( __cplusplus )
extern "C" {
#endif

int pasm_assemble( const char* buffer_in, char** buffer_out, int* size, incobj_callback_fn incobj );
void pasm_get_errors ( char** errors );
int pasm_get_num_errors ( void );
void pasm_get_symbols ( struct _symbol_table** symbols );
int pasm_get_symbol_data ( struct _symbol_table* symbol, char* data, int size );
int pasm_get_num_symbols ( void );
int pasm_get_symbol_value ( struct _symbol_table* symbol );
symbol_type pasm_get_symbol_type ( struct _symbol_table* symbol );

#if defined ( __cplusplus )
}
#endif

#endif 
