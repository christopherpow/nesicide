#if !defined ( PASM_LIB_H )
#define PASM_LIB_H

#include "pasm_types.h"

#if defined ( __cplusplus )
extern "C" {
#endif

int pasm_assemble( const char* buffer_in, char** buffer_out, int* size, incobj_callback_fn incobj );
void pasm_get_errors ( char** errors );
int pasm_get_num_errors ( void );
int pasm_get_num_symbols ( void );
char* pasm_get_symbol ( int symbol );
int pasm_get_symbol_data ( int symbol, char* data, int size );
int pasm_get_symbol_value ( int symbol );
symbol_type pasm_get_symbol_type ( int symbol );
int pasm_get_symbol_linenum ( int symbol );
int pasm_get_source_linenum ( unsigned int bank, unsigned int addr );

#if defined ( __cplusplus )
}
#endif

#endif 
