#if !defined ( PASM_SHARED_TYPES_H )
#define PASM_SHARED_TYPES_H

typedef struct _symbol_table
{
	char*          symbol;
   struct _ir_table* ir;
   unsigned int  btab_ent;
	unsigned short addr;
   unsigned char global;
   int value;
} symbol_table;

typedef struct _ir_table
{
   unsigned char data[3];
   unsigned int  btab_ent;
   unsigned int  addr;
   unsigned int  len;
   unsigned char emitted;
   unsigned char multi;
   unsigned char fixed;
   unsigned char label;
   struct _ir_table* ref;
   struct _ir_table* next;
   struct _ir_table* prev;
} ir_table;

#endif 
