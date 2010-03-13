#if !defined ( PASM_SHARED_TYPES_H )
#define PASM_SHARED_TYPES_H

#define MAX_FIXUP_TRIES 5

typedef struct _symbol_table
{
	char*          symbol;
   struct _ir_table* ir;
   struct _expr_type* expr;
   unsigned int  btab_ent;
   unsigned char global;
} symbol_table;

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

typedef struct _ref_type
{
   reference_type type;
   union _ref_union ref;
} ref_type;

typedef struct _number_type
{
   int number;
   unsigned char zp_ok;
} number_type;

typedef enum
{
   expression_operator,
   expression_number,
   expression_reference
} expression_type;

typedef struct _expr_type
{
   expression_type type;
   union
   {
      number_type* num;
      ref_type*    ref;
      char         op;
   } node;
   struct _expr_type* left;
   struct _expr_type* right;
   struct _expr_type* parent;
} expr_type;

typedef enum
{
   fixup_fixed,
   fixup_immediate,
   fixup_absolute,
   fixup_zeropage,
   fixup_abs_idx_x,
   fixup_abs_idx_y,
   fixup_zp_idx,
   fixup_indirect,
   fixup_pre_idx_ind,
   fixup_post_idx_ind,
   fixup_relative,
   fixup_datab,
   fixup_dataw,
   fixup_align
} fixup_type;

typedef struct _ir_table
{
   unsigned char data[3];
   unsigned int  btab_ent;
   unsigned int  addr;
   unsigned int  len;
   unsigned char emitted;
   unsigned char multi;
   unsigned char align;
   unsigned char fixed;
   unsigned char label;
   fixup_type    fixup;
   struct _ir_table* next;
   struct _ir_table* prev;
   struct _expr_type* expr;
} ir_table;

#endif
