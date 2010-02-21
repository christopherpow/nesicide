#if !defined ( PASM_TYPES_H )
#define PASM_TYPES_H

#include "pasm_shared_types.h"

#define INVALID_INSTR 0xFF
#define SYMBOL_NOT_FOUND 0xFFFFFFFF

#define ANONYMOUS_BANK "_anonymous_"

typedef int (*incobj_callback_fn) ( char* objname, char** objdata, int* size );

typedef union _ref_union
{
   char* symbol;
   struct _symbol_table* symtab;
} ref_union;

typedef enum
{
   reference_symbol,
   reference_symtab,
   reference_global
} reference_type;

typedef enum
{
   no_modifier = 0,
   use_big_of = 1,
   use_little_of = 2
} modifier_type;

typedef struct _ref_type
{
   reference_type type;
   modifier_type modifier;
   union _ref_union ref;
} ref_type;

typedef struct _number_type
{
   int number;
   unsigned char zp_ok;
} number_type;

typedef enum
{
   number_only = 0,
   reference_only = 1,
   compound = 2
} compound_type;

typedef struct _number_ref_compound_type
{
   compound_type type;
   struct _ref_type ref;
   struct _number_type num;
} number_ref_compound_type;

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

typedef enum
{
   fixup_immediate,
   fixup_immediate_big,
   fixup_immediate_little,
   fixup_absolute,
   fixup_absolute_big,
   fixup_absolute_little,
   fixup_zeropage,
   fixup_zeropage_big,
   fixup_zeropage_little,
   fixup_abs_idx,
   fixup_abs_idx_big,
   fixup_abs_idx_little,
   fixup_zp_idx,
   fixup_zp_idx_big,
   fixup_zp_idx_little,
   fixup_indirect,
   fixup_indirect_big,
   fixup_indirect_little,
   fixup_pre_idx_ind,
   fixup_pre_idx_ind_big,
   fixup_pre_idx_ind_little,
   fixup_post_idx_ind,
   fixup_post_idx_ind_big,
   fixup_post_idx_ind_little,
   fixup_relative,
   fixup_datab,
   fixup_dataw
} fixup_type;

typedef struct _fixup_table
{
   fixup_type     type;
   char*          symbol;
   struct _ir_table* ir;
   int            lineno;
   unsigned char  fixed;
   int            modifier;
} fixup_table;

#endif 
