#if !defined ( PASM_TYPES_H )
#define PASM_TYPES_H

#if defined ( __cplusplus )
extern "C" {
#endif

#define MAX_FIXUP_TRIES 5
#define INVALID_INSTR 0xFF
#define SYMBOL_NOT_FOUND 0xFFFFFFFF

// Flags for instruction fixer
#define PROMOTE 0
#define FIX     1

   typedef struct _keyword
   {
      const char* directive;
      const char* dotdirective;
      int         token;
      void (*handler)();
   } keyword;

   typedef struct _macro_table
   {
      int idx; // used for self-reference and also for scoping variables
      char* symbol;
      struct _symbol_list* stab;
      struct _ir_table* ir_head;
      struct _ir_table* ir_tail;
   } macro_table;

   typedef struct _text_type
   {
      char* string;
      int   length;
   } text_type;

   typedef enum
   {
      symbol_global,
      symbol_label
   } symbol_type;

   typedef struct _symbol_table
   {
      int                   idx;
      char*                 symbol;
      struct _ir_table*     ir;
      struct _expr_type*    expr;
      unsigned int          btab_ent;
      struct _symbol_table* next;
      struct _symbol_table* prev;
      unsigned char         alive;
   } symbol_table;

   typedef struct _symbol_list
   {
      struct _symbol_list* up;
      struct _symbol_table* head;
      struct _symbol_table* tail;
   } symbol_list;

   typedef union _ref_union
   {
      char* symbol;
      struct _symbol_table* symtab;
      struct _text_type* text;
   } ref_union;

   typedef enum
   {
      reference_symbol,
      reference_symtab,
      reference_const_string
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

   typedef enum
   {
      value_is_int,
      value_is_string
   } value_type;

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
      value_type vtype;
      union
      {
         int ival;
         text_type* sval;
      } value;
   } expr_type;

   typedef struct _file_table
   {
      char* name;
      char* text;
      struct _file_table* next;
   } file_table;

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
      fixup_datab_lo,
      fixup_datab_hi,
      fixup_dataw,
      fixup_align,
      fixup_string
   } fixup_type;

   typedef struct _ir_table
   {
      unsigned char data[3];
      unsigned int  btab_ent;
      unsigned int  addr;
      unsigned int  absAddr;
      unsigned int  len;
      unsigned char emitted;
      unsigned char instr;
      unsigned char multi;
      unsigned char align;
      unsigned char fixed;
      unsigned char label;
      unsigned char string;
      fixup_type    fixup;
      int           source_linenum;
      file_table*   file;
      struct _ir_table* next;
      struct _ir_table* prev;
      struct _symbol_table* symtab;
      struct _expr_type* expr;
   } ir_table;

#define TEXT_BANK_NAME_STRING "_text%03d_"
#define DATA_BANK_NAME_STRING "_data%03d_"

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
      struct _symbol_list* stab;

      // Intermediate representation queue pointers.  The assembly
      // code we're parsing is first built as a linked list of
      // intermediate representation structures.  Once the assembly
      // code has been fully and completely parsed we run some
      // reduction/optimization algorithms to complete the
      // intermediate representation and make it ready for binary
      // output.
      struct _ir_table* ir_head;
      struct _ir_table* ir_tail;
   } binary_table;

// PermanentMarker colors.
   enum
   {
      PERMANENT_MARKER_RED = 0,
      PERMANENT_MARKER_GREEN,
      PERMANENT_MARKER_BLUE,
      PERMANENT_MARKER_YELLOW,
      PERMANENT_MARKER_MAGENTA,
      PERMANENT_MARKER_CYAN,
      PERMANENT_MARKER_WHITE,
      PERMANENT_MARKER_BLACK,
      NUM_PERMANENT_MARKERS
   };

// PermanentMarker information.
   typedef struct _permanent_marker_table
   {
      struct _ir_table* start;
      struct _ir_table* end;
   } permanent_marker_table;

   symbol_table* find_symbol ( const char* symbol );
   
   typedef struct _error_table
   {
      struct _file_table* file;
      int   line;
      char* error;
      struct _error_table* next;
      struct _error_table* prev;
   } error_table;
   
   typedef struct _error_list
   {
      struct _error_table* head;
      struct _error_table* tail;
   } error_list;

#if defined ( __cplusplus )
}
#endif

#endif
