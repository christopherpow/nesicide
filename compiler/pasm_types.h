#if !defined ( PASM_TYPES_H )
#define PASM_TYPES_H

#include "pasm_shared_types.h"

#define INVALID_INSTR 0xFF
#define SYMBOL_NOT_FOUND 0xFFFFFFFF

#if defined ( __cplusplus )
extern "C" {
#endif

#define ANONYMOUS_BANK "_anonymous_"

typedef int (*incobj_callback_fn) ( char* objname, char** objdata, int* size );

typedef enum _segment_type
{
   text_segment,
   data_segment
} segment_type;

typedef struct _binary_table
{
   int            idx;
   segment_type   type;
   char*          symbol;
   unsigned int   addr;
} binary_table;

#if defined ( __cplusplus )
}
#endif

#endif
