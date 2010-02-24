%{
#include <stdio.h>
#include "pasm_types.h"

#define YYERROR_VERBOSE

#define DEFAULT_DIRECT 0x4000
#define FTAB_ENT_INC 32
#define BTAB_ENT_INC 1

incobj_callback_fn incobj_fn = NULL;

int yyerror(char *s);
int yylex(void);
int yywrap(void);

//char currentFile [ 256 ];
extern symbol_table* stab;
fixup_table*  ftab = NULL;
binary_table* btab = NULL;
binary_table* cur = NULL;
ir_table* ir_head = NULL;
ir_table* ir_tail = NULL;
extern symbol_table* find_symbol ( char* symbol, int bank );

extern int stab_ent;
int ftab_ent = 0;
int ftab_max = 0;
int btab_ent = 0;
int btab_max = 0;

extern int yylineno;

extern number_type* get_next_numtype ( void );
extern number_ref_compound_type* get_next_compoundtype ( void );

void add_error ( char* s );

unsigned char valid_instr_amode ( int instr, int amode );
unsigned char update_symbol_addr ( symbol_table* stab, unsigned int addr );
unsigned char update_symbol_ir ( symbol_table* stab );
unsigned int find_symbol_addr ( char* sym, int bank );

unsigned char add_fixup ( char* symbol, fixup_type type, int modifier );
void fixup ( symbol_table* symtab );
void check_fixup ( void );
unsigned int distance ( ir_table* from, ir_table* to );

unsigned char add_binary_bank ( segment_type type, char* symbol );
unsigned char set_binary_bank ( segment_type type, char* bankname );
void set_binary_addr ( unsigned int addr );
void output_binary ( void );
void output_binary_direct ( char** buffer, int* size );

void dump_symbol_table ( void );
void dump_fixup_table ( void );
void dump_binary_table ( void );
void dump_ir_table ( void );

ir_table* emit_ir ( void );
void emit_fix ( void );
void emit_label ( void );
void emit_bin ( unsigned char data );
void emit_bin_ref ( ir_table* ref );
void emit_bin2 ( unsigned short data );
void emit_bin2_ref ( ir_table* ref );
void emit_binm ( unsigned char data, int m );
void emit_bin_implied ( C6502_opcode* opcode );
void emit_bin_accumulator ( C6502_opcode* opcode );
void emit_bin_immediate ( C6502_opcode* opcode, unsigned char data );
void emit_bin_immediate_ref ( C6502_opcode* opcode, ir_table* ref );
void emit_bin_absolute ( C6502_opcode* opcode, unsigned short data );
void emit_bin_absolute_ref ( C6502_opcode* opcode, ir_table* ref );
void emit_bin_zeropage ( C6502_opcode* opcode, unsigned char data );
void emit_bin_zeropage_ref ( C6502_opcode* opcode, ir_table* ref );
void emit_bin_relative ( C6502_opcode* opcode, char data );
void emit_bin_relative_ref ( C6502_opcode* opcode, ir_table* ref );
void emit_bin_indirect ( C6502_opcode* opcode, unsigned short data );
void emit_bin_indirect_ref ( C6502_opcode* opcode, ir_table* ref );
void emit_bin_abs_idx_x ( C6502_opcode* opcode, unsigned short data );
void emit_bin_abs_idx_x_ref ( C6502_opcode* opcode, ir_table* ref );
void emit_bin_abs_idx_y ( C6502_opcode* opcode, unsigned short data );
void emit_bin_abs_idx_y_ref ( C6502_opcode* opcode, ir_table* ref );
void emit_bin_zp_idx_x ( C6502_opcode* opcode, unsigned char data );
void emit_bin_zp_idx_x_ref ( C6502_opcode* opcode, ir_table* ref );
void emit_bin_zp_idx_y ( C6502_opcode* opcode, unsigned char data );
void emit_bin_zp_idx_y_ref ( C6502_opcode* opcode, ir_table* ref );
void emit_bin_pre_idx_ind ( C6502_opcode* opcode, unsigned char data );
void emit_bin_pre_idx_ind_ref ( C6502_opcode* opcode, ir_table* ref );
void emit_bin_post_idx_ind ( C6502_opcode* opcode, unsigned char data );
void emit_bin_post_idx_ind_ref ( C6502_opcode* opcode, ir_table* ref );

char e [ 256 ];
%}
%union{
	struct _number_ref_compound_type* com;
	struct _number_type* num;
	struct _number_type* expr;
	char* text;
	struct _ref_type* ref;
	int seg;
	int instr;
	unsigned char flag;
}
%token <text> QUOTEDSTRING
%token <instr> INSTR
%token <ref> LABEL LABELREF
%token <num> DIGITS
%token DATAB DATAW
%token ORIGN SPACE ADVANCE
%token TERM UMINUS
%token <seg> SEGMENT 
%token INCBIN INCOBJ INCLUDE
%type <com> expr
%nonassoc '(' ')'
%left '+' '-'
%left '*' '/'
%nonassoc '~' UMINUS
%nonassoc '>' '<'
%start program
//lines: statement
//          | lines statement
%%
program: lines

lines: /* empty */
          | lines statement
          ;

statement: identifier instruction TERM
           | identifier TERM 
			  | instruction TERM
			  | TERM
			  ;

identifier: LABEL {
	emit_label ();
	update_symbol_addr ( $1->ref.symtab, cur->addr );
	update_symbol_ir ( $1->ref.symtab );
	fixup ( $1->ref.symtab );
	//dump_symbol_table ();
}
			  ;

instruction: INCBIN QUOTEDSTRING {
	char* fn = strdup ( $2 );
	int c;
	fn += 1; // walk past "
	fn[strlen(fn)-1] = 0; // chop off trailing "
	FILE* fp = fopen ( fn, "rb" );
	if ( fp != NULL )
	{
		for ( c=fgetc(fp); c!=EOF; c=fgetc(fp) )
		{
			emit_bin ( c );
		}
	}
	else
	{
		sprintf ( e, "cannot open included file: %s", fn );
		yyerror ( e );
		fprintf ( stderr, "error: %d: cannot open included file: %s\n", yylineno, fn );
	}
}
			  | INCOBJ QUOTEDSTRING {
	char* data = NULL; 
	char* fn = strdup ( $2 );
	int c;
	fn += 1; // walk past "
	fn[strlen(fn)-1] = 0; // chop off trailing "
	int size = 0;
	int f;
	int buf, orig;
	if ( incobj_fn )
	{
		f = incobj_fn ( $2, &data, &size );
		if ( f != 0 )
		{
	//		strcpy ( currentFile, $2 );
			
			orig = get_current_buffer ();

			buf = yy_scan_string ( data );

			yyparse();

			yy_flush_buffer ( buf );

			yy_switch_to_buffer ( orig );

			yy_delete_buffer ( buf );
			free ( data );

			yyparse();

			yy_flush_buffer ( orig );

	//		strcpy ( currentFile, "" );
		}
		else
		{
			sprintf ( e, "object not found for .incobj: %s", $2 );
			yyerror ( e );
			fprintf ( stderr, "error: %d: object not found for .incobj: %s\n", yylineno, $2 );
		}
	}
	else
	{
		sprintf ( e, "use of .incobj directive not supported: %s", fn );
		yyerror ( e );
		fprintf ( stderr, "error: %d: use of .incobj directive not supported: %s\n", yylineno, fn );
	}
}
			  | INCLUDE QUOTEDSTRING {
	char* fn = strdup ( $2 );
	char* buffer;
	int c;
	int bytes;
	int buf, orig;
	fn += 1; // walk past "
	fn[strlen(fn)-1] = 0; // chop off trailing "
	FILE* fp = fopen ( fn, "r" );
	if ( fp != NULL )
	{
//		strcpy ( currentFile, $2 );

		fseek ( fp, 0, SEEK_END );
		bytes = ftell ( fp );
		printf ( "bytes found: %d\n", bytes );
		if ( bytes > 0 )
		{
			fseek ( fp, 0, SEEK_SET );
			buffer = (char*) malloc ( bytes+1 );
			memset ( buffer, 0, bytes+1 );
			if ( buffer != NULL )
			{
				fread ( buffer, 1, bytes, fp );

				orig = get_current_buffer ();

				buf = yy_scan_string ( buffer );

				yyparse();

				yy_flush_buffer ( buf );

				yy_switch_to_buffer ( orig );

				yy_delete_buffer ( buf );
				free ( buffer );

				yyparse();

				yy_flush_buffer ( orig );
			}
		}

//		strcpy ( currentFile, "" );
	}
	else
	{
		sprintf ( e, "cannot open included file: %s", fn );
		yyerror ( e );
		fprintf ( stderr, "error: %d: cannot open included file: %s\n", yylineno, fn );
	}
}
			  | LABELREF '=' expr {
	unsigned char f;
	symbol_table* p = NULL;
	if ( $1->type == reference_symbol )
	{
		f=add_symbol($1->ref.symbol,&p);
		if ( !f )
		{
			sprintf ( e, "multiple declarations of symbol: %s", $1->ref.symbol ); 
			yyerror ( e );
			fprintf ( stderr, "error: %d: multiple declarations of symbol: %s\n", yylineno, $1->ref.symbol );
		}
		if ( p )
		{
			p->global = 1;
			printf ( "getting addr %08x\n", $3 );
			p->addr = get_compound_value ( $3 );
			printf ( "got addr %08x\n", p->addr );
			// CSP ADD FIXUP FOR THIS!!
		}
	}
	else
	{
		sprintf ( e, "multiple declarations of symbol: %s", $1->ref.symtab->symbol ); 
		yyerror ( e );
		fprintf ( stderr, "error: %d: multiple declarations of symbol: %s\n", yylineno, $1->ref.symtab->symbol );
	}
//	dump_symbol_table ();
}
//			  | LABELREF {
//	sprintf ( e, "parse error, unrecognized token: %s", $1->type==reference_symbol?$1->ref.ref.symbol:$1->ref.ref.symtab->symbol );
//	yyerror ( e );
//	fprintf ( stderr, "error: %d: parse error, unrecognized token: %s\n", yylineno, $1->type==reference_symbol?$1->ref.ref.symbol:$1->ref.ref.symtab->symbol );
//}
			  | INSTR {
	unsigned char f;
   if ( (f=valid_instr_amode($1,AM_IMPLIED)) != INVALID_INSTR )
	{
		emit_bin_implied ( &(m_6502opcode[f]) );
	}
	else if ( (f=valid_instr_amode($1,AM_ACCUMULATOR)) != INVALID_INSTR )
	{
		emit_bin_accumulator ( &(m_6502opcode[f]) );
	}
	else
	{
		sprintf ( e, "invalid addressing mode for instruction: %s", $1 );
		yyerror ( e );
		fprintf ( stderr, "error: %d: invalid addressing mode for instruction: %s\n", yylineno, $1 );
	}
}
           | INSTR '#' expr { 
	unsigned char f;
	unsigned int  loc;
	unsigned int  d;

	if ( $3->type == number_only )
	{
		if ( ((f=valid_instr_amode($1,AM_IMMEDIATE)) != INVALID_INSTR) &&
			  ($3->num.zp_ok == 1) )
		{
			emit_bin_immediate ( &(m_6502opcode[f]), $3->num.number );
		}
		else if ( $3->num.zp_ok == 0 )
		{
			sprintf ( e, "immediate value too large: %d", $3->num.number );
			yyerror ( e );
			fprintf ( stderr, "error: %d: number out of range for instruction format\n", yylineno );
		}
		else
		{
			sprintf ( e, "invalid addressing mode for instruction: %s", $1 );
			yyerror ( e );
			fprintf ( stderr, "error: %d: invalid addressing mode for instruction: %s\n", yylineno, $1 );
		}
	}
	else
	{
		if ( $3->ref.type == reference_global )
		{
			/* backward (known) reference... */
			loc = $3->ref.ref.symtab->addr;
			if ( $3->type == compound )
			{
				loc += $3->num.number;
			}
			if ( $3->ref.modifier == use_big_of ) loc = (loc>>8)&0xFF;
			else if ( $3->ref.modifier == use_little_of ) loc &= 0xFF;
			if ( loc < 0x100 )
			{
				if ( (f=valid_instr_amode($1,AM_IMMEDIATE)) != INVALID_INSTR )
				{
					emit_bin_immediate ( &(m_6502opcode[f]), loc );
				}
				else
				{
					sprintf ( e, "invalid addressing mode for instruction: %s", $1 );
					yyerror ( e );
					fprintf ( stderr, "error: %d: invalid addressing mode for instruction: %s\n", yylineno, $1 );
				}
			}
			else
			{
				sprintf ( e, "value too large for immediate in instruction: %s", $1 );
				yyerror ( e );
				fprintf ( stderr, "error: %d: value too large for immediate in instruction: %s", yylineno, $1 );
			}
		}
		else if ( $3->ref.type == reference_symtab )
		{
			/* backward (known) reference... */
			loc = find_symbol_addr ( $3->ref.ref.symtab->symbol, cur->idx );
			if ( $3->type == compound )
			{
				loc += $3->num.number;
			}
			if ( $3->ref.modifier == use_big_of ) loc = (loc>>8)&0xFF;
			else if ( $3->ref.modifier == use_little_of ) loc &= 0xFF;
			if ( loc < 0x100 )
			{
				if ( (f=valid_instr_amode($1,AM_IMMEDIATE)) != INVALID_INSTR )
				{
					emit_bin_immediate_ref ( &(m_6502opcode[f]), $3->ref.ref.symtab->ir );
					add_fixup ( $3->ref.ref.symbol, fixup_immediate, 0 );
				}
				else
				{
					sprintf ( e, "invalid addressing mode for instruction: %s", $1 );
					yyerror ( e );
					fprintf ( stderr, "error: %d: invalid addressing mode for instruction: %s\n", yylineno, $1 );
				}
			}
			else
			{
				sprintf ( e, "value too large for immediate in instruction: %s", $1 );
				yyerror ( e );
				fprintf ( stderr, "error: %d: value too large for immediate in instruction: %s", yylineno, $1 );
			}
		}
		else
		{
			/* forward (unknown) reference... */
			if ( (f=valid_instr_amode($1,AM_IMMEDIATE)) != INVALID_INSTR )
			{
				emit_bin_immediate_ref ( &(m_6502opcode[f]), $3->ref.ref.symtab->ir );
				if ( $3->ref.modifier == use_big_of )
				{
					add_fixup ( $3->ref.ref.symbol, fixup_immediate_big, 0 );
				}
				else if ( $3->ref.modifier == use_little_of )
				{
					add_fixup ( $3->ref.ref.symbol, fixup_immediate_little, 0 );
				}
				else
				{
					add_fixup ( $3->ref.ref.symbol, fixup_immediate, 0 );
				}
			}
			else
			{
				sprintf ( e, "invalid addressing mode for instruction: %s", $1 );
				yyerror ( e );
				fprintf ( stderr, "error: %d: invalid addressing mode for instruction: %s\n", yylineno, $1 );
			}
		}
	}
}
			  | INSTR expr ',' 'x' { 
	unsigned char f;
	unsigned int  loc;

	if ( $2->type == number_only )
	{
		if ( ((f=valid_instr_amode($1,AM_ZEROPAGE_INDEXED_X)) != INVALID_INSTR) &&
			  ($2->num.zp_ok == 1) )
		{
			emit_bin_zp_idx_x ( &(m_6502opcode[f]), $2->num.number );
		}
		else if ( (f=valid_instr_amode($1,AM_ABSOLUTE_INDEXED_X)) != INVALID_INSTR )
		{
			emit_bin_abs_idx_x ( &(m_6502opcode[f]), $2->num.number );
		}
		else if ( ((f=valid_instr_amode($1,AM_ZEROPAGE_INDEXED_X)) != INVALID_INSTR) &&
					 ($2->num.zp_ok == 0) )
		{
			sprintf ( e, "address out of range for instruction format, cannot promote instruction" );
			yyerror ( e );
			fprintf ( stderr, "error: %d: number out of range for instruction format\n", yylineno );
		}
		else
		{
			sprintf ( e, "invalid addressing mode for instruction: %s", $1 );
			yyerror ( e );
			fprintf ( stderr, "error: %d: invalid addressing mode for instruction: %s\n", yylineno, $1 );
		}
	}
	else
	{
		if ( $2->ref.type == reference_global )
		{
			/* backward (known) reference... */
			loc = $2->ref.ref.symtab->addr;
			if ( $2->type == compound )
			{
				loc += $2->num.number;
			}
			if ( $2->ref.modifier == use_little_of ) loc &= 0xFF;
			else if ( $2->ref.modifier == use_big_of ) loc = (loc>>8)&0xFF;
			if ( ((f=valid_instr_amode($1,AM_ZEROPAGE_INDEXED_X)) != INVALID_INSTR) &&
				  (loc < 0x100) )
			{
				emit_bin_zp_idx_x ( &(m_6502opcode[f]), loc );
			}
			else if ( (f=valid_instr_amode($1,AM_ABSOLUTE_INDEXED_X)) != INVALID_INSTR )
			{
				emit_bin_abs_idx_x ( &(m_6502opcode[f]), loc );
			}
			else if ( ((f=valid_instr_amode($1,AM_ZEROPAGE_INDEXED_X)) != INVALID_INSTR) &&
						 (loc >= 0x100) )
			{
				sprintf ( e, "label out of range for instruction format, cannot promote instruction" );
				yyerror ( e );
				fprintf ( stderr, "error: %d: number out of range for instruction format\n", yylineno );
			}
			else
			{
				sprintf ( e, "invalid addressing mode for instruction: %s", $1 );
				yyerror ( e );
				fprintf ( stderr, "error: %d: invalid addressing mode for instruction: %s\n", yylineno, $1 );
			}
		}
		else if ( $2->ref.type == reference_symtab )
		{
			/* backward (known) reference... */
			loc = find_symbol_addr ( $2->ref.ref.symtab->symbol, cur->idx );
			if ( $2->type == compound )
			{
				loc += $2->num.number;
			}
			if ( $2->ref.modifier == use_little_of ) loc &= 0xFF;
			else if ( $2->ref.modifier == use_big_of ) loc = (loc>>8)&0xFF;
			if ( ((f=valid_instr_amode($1,AM_ZEROPAGE_INDEXED_X)) != INVALID_INSTR) &&
				  (loc < 0x100) )
			{
				emit_bin_zp_idx_x_ref ( &(m_6502opcode[f]), $2->ref.ref.symtab->ir );
				add_fixup ( $2->ref.ref.symtab->symbol, fixup_zp_idx, 0 );
			}
			else if ( (f=valid_instr_amode($1,AM_ABSOLUTE_INDEXED_X)) != INVALID_INSTR )
			{
				emit_bin_abs_idx_x_ref ( &(m_6502opcode[f]), $2->ref.ref.symtab->ir );
				add_fixup ( $2->ref.ref.symtab->symbol, fixup_abs_idx, 0 );
			}
			else if ( ((f=valid_instr_amode($1,AM_ZEROPAGE_INDEXED_X)) != INVALID_INSTR) &&
						 (loc >= 0x100) )
			{
				sprintf ( e, "label out of range for instruction format, cannot promote instruction" );
				yyerror ( e );
				fprintf ( stderr, "error: %d: number out of range for instruction format\n", yylineno );
			}
			else
			{
				sprintf ( e, "invalid addressing mode for instruction: %s", $1 );
				yyerror ( e );
				fprintf ( stderr, "error: %d: invalid addressing mode for instruction: %s\n", yylineno, $1 );
			}
		}
		else
		{
			/* forward (unknown) reference... */
			if ( (f=valid_instr_amode($1,AM_ZEROPAGE_INDEXED_X)) != INVALID_INSTR )
			{
				emit_bin_zp_idx_x_ref ( &(m_6502opcode[f]), NULL );
				if ( $2->ref.modifier == use_big_of )
				{
					add_fixup ( $2->ref.ref.symbol, fixup_zp_idx_big, 0 );
				}
				else if ( $2->ref.modifier == use_little_of )
				{
					add_fixup ( $2->ref.ref.symbol, fixup_zp_idx_little, 0 );
				}
				else
				{
					add_fixup ( $2->ref.ref.symbol, fixup_zp_idx, 0 );
				}
			}
			else if ( (f=valid_instr_amode($1,AM_ABSOLUTE_INDEXED_X)) != INVALID_INSTR )
			{
				emit_bin_abs_idx_x_ref ( &(m_6502opcode[f]), NULL );
				if ( $2->ref.modifier == use_big_of )
				{
					add_fixup ( $2->ref.ref.symbol, fixup_abs_idx_big, 0 );
				}
				else if ( $2->ref.modifier == use_little_of )
				{
					add_fixup ( $2->ref.ref.symbol, fixup_abs_idx_little, 0 );
				}
				else
				{
					add_fixup ( $2->ref.ref.symbol, fixup_abs_idx, 0 );
				}
			}
			else
			{
				sprintf ( e, "invalid addressing mode for instruction: %s", $1 );
				yyerror ( e );
				fprintf ( stderr, "error: %d: invalid addressing mode for instruction: %s\n", yylineno, $1 );
			}
		}
	}
}
			  | INSTR expr ',' 'y' { 
	unsigned char f;
	unsigned int  loc;

	if ( $2->type == number_only )
	{
		if ( ((f=valid_instr_amode($1,AM_ZEROPAGE_INDEXED_Y)) != INVALID_INSTR) &&
			  ($2->num.zp_ok == 1) )
		{
			emit_bin_zp_idx_y ( &(m_6502opcode[f]), $2->num.number );
		}
		else if ( (f=valid_instr_amode($1,AM_ABSOLUTE_INDEXED_Y)) != INVALID_INSTR )
		{
			emit_bin_abs_idx_y ( &(m_6502opcode[f]), $2->num.number );
		}
		else if ( ((f=valid_instr_amode($1,AM_ZEROPAGE_INDEXED_Y)) != INVALID_INSTR) &&
					 ($2->num.zp_ok == 0) )
		{
			sprintf ( e, "address out of range for instruction format, cannot promote instruction" );
			yyerror ( e );
			fprintf ( stderr, "error: %d: number out of range for instruction format\n", yylineno );
		}
		else
		{
			sprintf ( e, "invalid addressing mode for instruction: %s", $1 );
			yyerror ( e );
			fprintf ( stderr, "error: %d: invalid addressing mode for instruction: %s\n", yylineno, $1 );
		}
	}
	else
	{
		if ( $2->ref.type == reference_global )
		{
			/* backward (known) reference... */
			loc = $2->ref.ref.symtab->addr;
			if ( $2->type == compound )
			{
				loc += $2->num.number;
			}
			if ( $2->ref.modifier == use_little_of ) loc &= 0xFF;
			else if ( $2->ref.modifier == use_big_of ) loc = (loc>>8)&0xFF;
			if ( ((f=valid_instr_amode($1,AM_ZEROPAGE_INDEXED_Y)) != INVALID_INSTR) &&
				  (loc < 0x100) )
			{
				emit_bin_zp_idx_y ( &(m_6502opcode[f]), loc );
			}
			else if ( (f=valid_instr_amode($1,AM_ABSOLUTE_INDEXED_Y)) != INVALID_INSTR )
			{
				emit_bin_abs_idx_x ( &(m_6502opcode[f]), loc );
			}
			else if ( ((f=valid_instr_amode($1,AM_ZEROPAGE_INDEXED_Y)) != INVALID_INSTR) &&
						 (loc >= 0x100) )
			{
				sprintf ( e, "label out of range for instruction format, cannot promote instruction" );
				yyerror ( e );
				fprintf ( stderr, "error: %d: number out of range for instruction format\n", yylineno );
			}
			else
			{
				sprintf ( e, "invalid addressing mode for instruction: %s", $1 );
				yyerror ( e );
				fprintf ( stderr, "error: %d: invalid addressing mode for instruction: %s\n", yylineno, $1 );
			}
		}
		else if ( $2->ref.type == reference_symtab )
		{
			/* backward (known) reference... */
			loc = find_symbol_addr ( $2->ref.ref.symtab->symbol, cur->idx );
			if ( $2->type == compound )
			{
				loc += $2->num.number;
			}
			if ( $2->ref.modifier == use_little_of ) loc &= 0xFF;
			else if ( $2->ref.modifier == use_big_of ) loc = (loc>>8)&0xFF;
			if ( ((f=valid_instr_amode($1,AM_ZEROPAGE_INDEXED_Y)) != INVALID_INSTR) &&
				  (loc < 0x100) )
			{
				emit_bin_zp_idx_y_ref ( &(m_6502opcode[f]), $2->ref.ref.symtab->ir );
				add_fixup ( $2->ref.ref.symtab->symbol, fixup_zp_idx, 0 );
			}
			else if ( (f=valid_instr_amode($1,AM_ABSOLUTE_INDEXED_Y)) != INVALID_INSTR )
			{
				emit_bin_abs_idx_x_ref ( &(m_6502opcode[f]), $2->ref.ref.symtab->ir );
				add_fixup ( $2->ref.ref.symtab->symbol, fixup_abs_idx, 0 );
			}
			else if ( ((f=valid_instr_amode($1,AM_ZEROPAGE_INDEXED_Y)) != INVALID_INSTR) &&
						 (loc >= 0x100) )
			{
				sprintf ( e, "label out of range for instruction format, cannot promote instruction" );
				yyerror ( e );
				fprintf ( stderr, "error: %d: number out of range for instruction format\n", yylineno );
			}
			else
			{
				sprintf ( e, "invalid addressing mode for instruction: %s", $1 );
				yyerror ( e );
				fprintf ( stderr, "error: %d: invalid addressing mode for instruction: %s\n", yylineno, $1 );
			}
		}
		else
		{
			/* forward (unknown) reference... */
			if ( ((f=valid_instr_amode($1,AM_ZEROPAGE_INDEXED_Y)) != INVALID_INSTR) &&
				  (loc < 0x100) )
			{
				emit_bin_zp_idx_y_ref ( &(m_6502opcode[f]), NULL );
				if ( $2->ref.modifier == use_big_of )
				{
					add_fixup ( $2->ref.ref.symbol, fixup_zp_idx_big, 0 );
				}
				else if ( $2->ref.modifier == use_little_of )
				{
					add_fixup ( $2->ref.ref.symbol, fixup_zp_idx_little, 0 );
				}
				else
				{
					add_fixup ( $2->ref.ref.symbol, fixup_zp_idx, 0 );
				}
			}
			else if ( (f=valid_instr_amode($1,AM_ABSOLUTE_INDEXED_Y)) != INVALID_INSTR )
			{
				emit_bin_abs_idx_y_ref ( &(m_6502opcode[f]), NULL );
				if ( $2->ref.modifier == use_big_of )
				{
					add_fixup ( $2->ref.ref.symbol, fixup_abs_idx_big, 0 );
				}
				else if ( $2->ref.modifier == use_little_of )
				{
					add_fixup ( $2->ref.ref.symbol, fixup_abs_idx_little, 0 );
				}
				else
				{
					add_fixup ( $2->ref.ref.symbol, fixup_abs_idx, 0 );
				}
			}
			else
			{
				sprintf ( e, "invalid addressing mode for instruction: %s", $1 );
				yyerror ( e );
				fprintf ( stderr, "error: %d: invalid addressing mode for instruction: %s\n", yylineno, $1 );
			}
		}
	}
}
			  | INSTR '(' expr ')' { 
	unsigned char f;
	unsigned int  loc;

	if ( $3->type == number_only )
	{
		if ( (f=valid_instr_amode($1,AM_INDIRECT)) != INVALID_INSTR )
		{
			emit_bin_indirect ( &(m_6502opcode[f]), $3->num.number );
		}
		else
		{
			sprintf ( e, "invalid addressing mode for instruction: %s", $1 );
			yyerror ( e );
			fprintf ( stderr, "error: %d: invalid addressing mode for instruction: %s\n", yylineno, $1 );
		}
	}
	else
	{
		if ( $3->ref.type == reference_global )
		{
			/* backward (known) reference... */
			loc = $3->ref.ref.symtab->addr;
			if ( $3->type == compound )
			{
				loc += $3->num.number;
			}
			if ( $3->ref.modifier == use_little_of ) loc &= 0xFF;
			else if ( $3->ref.modifier == use_big_of ) loc = (loc>>8)&0xFF;
			if ( (f=valid_instr_amode($1,AM_INDIRECT)) != INVALID_INSTR )
			{
				emit_bin_indirect ( &(m_6502opcode[f]), loc );
			}
			else
			{
				sprintf ( e, "invalid addressing mode for instruction: %s", $1 );
				yyerror ( e );
				fprintf ( stderr, "error: %d: invalid addressing mode for instruction: %s\n", yylineno, $1 );
			}
		}
		else if ( $3->ref.type == reference_symtab )
		{
			/* backward (known) reference... */
			loc = find_symbol_addr ( $3->ref.ref.symtab->symbol, cur->idx );
			if ( $3->type == compound )
			{
				loc += $3->num.number;
			}
			if ( $3->ref.modifier == use_little_of ) loc &= 0xFF;
			else if ( $3->ref.modifier == use_big_of ) loc = (loc>>8)&0xFF;
			if ( (f=valid_instr_amode($1,AM_INDIRECT)) != INVALID_INSTR )
			{
				emit_bin_indirect_ref ( &(m_6502opcode[f]), $3->ref.ref.symtab->ir );
				add_fixup ( $3->ref.ref.symbol, fixup_indirect, 0 );
			}
			else
			{
				sprintf ( e, "invalid addressing mode for instruction: %s", $1 );
				yyerror ( e );
				fprintf ( stderr, "error: %d: invalid addressing mode for instruction: %s\n", yylineno, $1 );
			}
		}
		else
		{
			/* forward (unknown) reference... */
			if ( (f=valid_instr_amode($1,AM_INDIRECT)) != INVALID_INSTR )
			{
				emit_bin_indirect_ref ( &(m_6502opcode[f]), NULL );
				if ( $3->ref.modifier == use_big_of )
				{
					add_fixup ( $3->ref.ref.symbol, fixup_indirect_big, 0 );
				}
				else if ( $3->ref.modifier == use_little_of )
				{
					add_fixup ( $3->ref.ref.symbol, fixup_indirect_little, 0 );
				}
				else
				{
					add_fixup ( $3->ref.ref.symbol, fixup_indirect, 0 );
				}
			}
			else
			{
				sprintf ( e, "invalid addressing mode for instruction: %s", $1 );
				yyerror ( e );
				fprintf ( stderr, "error: %d: invalid addressing mode for instruction: %s\n", yylineno, $1 );
			}
		}
	}
}
			  | INSTR '(' expr ',' 'x' ')' { 
	unsigned char f;
	unsigned int  loc;

	if ( $3->type == number_only )
	{
		if ( ((f=valid_instr_amode($1,AM_PREINDEXED_INDIRECT)) != INVALID_INSTR) &&
			  ($3->num.zp_ok == 1) )
		{
			emit_bin_pre_idx_ind ( &(m_6502opcode[f]), $3->num.number );
		}
		else if ( ((f=valid_instr_amode($1,AM_PREINDEXED_INDIRECT)) != INVALID_INSTR) &&
					 ($3->num.zp_ok == 0) )
		{
			sprintf ( e, "address out of range for instruction format, cannot promote instruction" );
			yyerror ( e );
			fprintf ( stderr, "error: %d: number out of range for instruction format\n", yylineno );
		}
		else
		{
			sprintf ( e, "invalid addressing mode for instruction: %s", $1 );
			yyerror ( e );
			fprintf ( stderr, "error: %d: invalid addressing mode for instruction: %s\n", yylineno, $1 );
		}
	}
	else
	{
		if ( $3->ref.type == reference_global )
		{
			/* backward (known) reference... */
			loc = $3->ref.ref.symtab->addr;
			if ( $3->type == compound )
			{
				loc += $3->num.number;
			}
			if ( $3->ref.modifier == use_little_of ) loc &= 0xFF;
			else if ( $3->ref.modifier == use_big_of ) loc = (loc>>8)&0xFF;
			if ( ((f=valid_instr_amode($1,AM_PREINDEXED_INDIRECT)) != INVALID_INSTR) &&
				  (loc < 0x100) )
			{
				emit_bin_pre_idx_ind ( &(m_6502opcode[f]), loc );
			}
			else if ( ((f=valid_instr_amode($1,AM_PREINDEXED_INDIRECT)) != INVALID_INSTR) &&
						 (loc >= 0x100) )
			{
				sprintf ( e, "label out of range for instruction format, cannot promote instruction" );
				yyerror ( e );
				fprintf ( stderr, "error: %d: number out of range for instruction format\n", yylineno );
			}
			else
			{
				sprintf ( e, "invalid addressing mode for instruction: %s", $1 );
				yyerror ( e );
				fprintf ( stderr, "error: %d: invalid addressing mode for instruction: %s\n", yylineno, $1 );
			}
		}
		else if ( $3->ref.type == reference_symtab )
		{
			/* backward (known) reference... */
			loc = find_symbol_addr ( $3->ref.ref.symtab->symbol, cur->idx );
			if ( $3->type == compound )
			{
				loc += $3->num.number;
			}
			if ( $3->ref.modifier == use_little_of ) loc &= 0xFF;
			else if ( $3->ref.modifier == use_big_of ) loc = (loc>>8)&0xFF;
			if ( ((f=valid_instr_amode($1,AM_PREINDEXED_INDIRECT)) != INVALID_INSTR) &&
				  (loc < 0x100) )
			{
				emit_bin_pre_idx_ind_ref ( &(m_6502opcode[f]), $3->ref.ref.symtab->ir );
				add_fixup ( $3->ref.ref.symbol, fixup_pre_idx_ind, 0 );
			}
			else if ( ((f=valid_instr_amode($1,AM_PREINDEXED_INDIRECT)) != INVALID_INSTR) &&
						 (loc >= 0x100) )
			{
				sprintf ( e, "label out of range for instruction format, cannot promote instruction" );
				yyerror ( e );
				fprintf ( stderr, "error: %d: number out of range for instruction format\n", yylineno );
			}
			else
			{
				sprintf ( e, "invalid addressing mode for instruction: %s", $1 );
				yyerror ( e );
				fprintf ( stderr, "error: %d: invalid addressing mode for instruction: %s\n", yylineno, $1 );
			}
		}
		else
		{
			/* forward (unknown) reference... */
			if ( (f=valid_instr_amode($1,AM_PREINDEXED_INDIRECT)) != INVALID_INSTR )
			{
				emit_bin_pre_idx_ind_ref ( &(m_6502opcode[f]), NULL );
				if ( $3->ref.modifier == use_big_of )
				{
					add_fixup ( $3->ref.ref.symbol, fixup_pre_idx_ind_big, 0 );
				}
				else if ( $3->ref.modifier == use_little_of )
				{
					add_fixup ( $3->ref.ref.symbol, fixup_pre_idx_ind_little, 0 );
				}
				else
				{
					add_fixup ( $3->ref.ref.symbol, fixup_pre_idx_ind, 0 );
				}
			}
			else
			{
				sprintf ( e, "invalid addressing mode for instruction: %s", $1 );
				yyerror ( e );
				fprintf ( stderr, "error: %d: invalid addressing mode for instruction: %s\n", yylineno, $1 );
			}
		}
	}
}
			  | INSTR '(' expr ')' ',' 'y' { 
	unsigned char f;
	unsigned int  loc;

	if ( $3->type == number_only )
	{
		if ( ((f=valid_instr_amode($1,AM_POSTINDEXED_INDIRECT)) != INVALID_INSTR) &&
			  ($3->num.zp_ok == 1) )
		{
			emit_bin_post_idx_ind ( &(m_6502opcode[f]), $3->num.number );
		}
		else if ( ((f=valid_instr_amode($1,AM_POSTINDEXED_INDIRECT)) != INVALID_INSTR) &&
					 ($3->num.zp_ok == 0) )
		{
			sprintf ( e, "address out of range for instruction format, cannot promote instruction" );
			yyerror ( e );
			fprintf ( stderr, "error: %d: number out of range for instruction format\n", yylineno );
		}
		else
		{
			sprintf ( e, "invalid addressing mode for instruction: %s", $1 );
			yyerror ( e );
			fprintf ( stderr, "error: %d: invalid addressing mode for instruction: %s\n", yylineno, $1 );
		}
	}
	else
	{
		if ( $3->ref.type == reference_global )
		{
			/* backward (known) reference... */
			loc = $3->ref.ref.symtab->addr;
			if ( $3->type == compound )
			{
				loc += $3->num.number;
			}
			if ( $3->ref.modifier == use_little_of ) loc &= 0xFF;
			else if ( $3->ref.modifier == use_big_of ) loc = (loc>>8)&0xFF;
			if ( ((f=valid_instr_amode($1,AM_POSTINDEXED_INDIRECT)) != INVALID_INSTR) &&
				  (loc < 0x100) )
			{
				emit_bin_post_idx_ind ( &(m_6502opcode[f]), loc );
			}
			if ( ((f=valid_instr_amode($1,AM_POSTINDEXED_INDIRECT)) != INVALID_INSTR) &&
				  (loc < 0x100) )
			{
				sprintf ( e, "label out of range for instruction format, cannot promote instruction" );
				yyerror ( e );
				fprintf ( stderr, "error: %d: number out of range for instruction format\n", yylineno );
			}
			else
			{
				sprintf ( e, "invalid addressing mode for instruction: %s", $1 );
				yyerror ( e );
				fprintf ( stderr, "error: %d: invalid addressing mode for instruction: %s\n", yylineno, $1 );
			}
		}
		else if ( $3->ref.type == reference_symtab )
		{
			/* backward (known) reference... */
			loc = find_symbol_addr ( $3->ref.ref.symtab->symbol, cur->idx );
			if ( $3->type == compound )
			{
				loc += $3->num.number;
			}
			if ( $3->ref.modifier == use_little_of ) loc &= 0xFF;
			else if ( $3->ref.modifier == use_big_of ) loc = (loc>>8)&0xFF;
			if ( ((f=valid_instr_amode($1,AM_POSTINDEXED_INDIRECT)) != INVALID_INSTR) &&
				  (loc < 0x100) )
			{
				emit_bin_post_idx_ind_ref ( &(m_6502opcode[f]), $3->ref.ref.symtab->ir );
				add_fixup ( $3->ref.ref.symbol, fixup_post_idx_ind, 0 );
			}
			if ( ((f=valid_instr_amode($1,AM_POSTINDEXED_INDIRECT)) != INVALID_INSTR) &&
				  (loc < 0x100) )
			{
				sprintf ( e, "label out of range for instruction format, cannot promote instruction" );
				yyerror ( e );
				fprintf ( stderr, "error: %d: number out of range for instruction format\n", yylineno );
			}
			else
			{
				sprintf ( e, "invalid addressing mode for instruction: %s", $1 );
				yyerror ( e );
				fprintf ( stderr, "error: %d: invalid addressing mode for instruction: %s\n", yylineno, $1 );
			}
		}
		else
		{
			/* forward (unknown) reference... */
			if ( (f=valid_instr_amode($1,AM_POSTINDEXED_INDIRECT)) != INVALID_INSTR )
			{
				emit_bin_post_idx_ind_ref ( &(m_6502opcode[f]), NULL );
				if ( $3->ref.modifier == use_big_of )
				{
					add_fixup ( $3->ref.ref.symbol, fixup_post_idx_ind_big, 0 );
				}
				else if ( $3->ref.modifier == use_little_of )
				{
					add_fixup ( $3->ref.ref.symbol, fixup_post_idx_ind_little, 0 );
				}
				else
				{
					add_fixup ( $3->ref.ref.symbol, fixup_post_idx_ind, 0 );
				}
			}
			else
			{
				sprintf ( e, "invalid addressing mode for instruction: %s", $1 );
				yyerror ( e );
				fprintf ( stderr, "error: %d: invalid addressing mode for instruction: %s\n", yylineno, $1 );
			}
		}
	}
}
			  | INSTR expr {
	unsigned char f;
	unsigned int  loc;
	unsigned int  d;

	if ( $2->type == number_only )
	{
		if ( ((f=valid_instr_amode($1,AM_ZEROPAGE)) != INVALID_INSTR) &&
			  ($2->num.zp_ok == 1) )
		{
			emit_bin_zeropage ( &(m_6502opcode[f]), $2->num.number );
		}
		else if ( (f=valid_instr_amode($1,AM_ABSOLUTE)) != INVALID_INSTR )
		{
			emit_bin_absolute ( &(m_6502opcode[f]), $2->num.number );
		}
		else if ( (f=valid_instr_amode($1,AM_RELATIVE)) != INVALID_INSTR )
		{
			emit_bin_relative ( &(m_6502opcode[f]), $2->num.number );
		}
		else if ( ((f=valid_instr_amode($1,AM_ZEROPAGE)) != INVALID_INSTR) &&
					 ($2->num.zp_ok == 0) )
		{
			sprintf ( e, "address out of range for instruction format, cannot promote instruction" );
			yyerror ( e );
			fprintf ( stderr, "error: %d: number out of range for instruction format\n", yylineno );
		}
		else if ( ((f=valid_instr_amode($1,AM_RELATIVE)) != INVALID_INSTR) &&
					 ($2->num.zp_ok == 0) )
		{
			sprintf ( e, "address out of range for relative branch" );
			yyerror ( e );
			fprintf ( stderr, "error: %d: number out of range for instruction format\n", yylineno );
		}
		else
		{
			sprintf ( e, "invalid addressing mode for instruction: %s", $1 );
			yyerror ( e );
			fprintf ( stderr, "error: %d: invalid addressing mode for instruction: %s\n", yylineno, $1 );
		}
	}
	else
	{
		if ( $2->ref.type == reference_global )
		{
			/* backward (known) reference... */
			loc = $2->ref.ref.symtab->addr;
			if ( $2->type == compound )
			{
				loc += $2->num.number;
			}
			if ( $2->ref.modifier == use_little_of ) loc &= 0xFF;
			else if ( $2->ref.modifier == use_big_of ) loc = (loc>>8)&0xFF;
			if ( (f=valid_instr_amode($1,AM_RELATIVE)) != INVALID_INSTR )
			{
				sprintf ( e, "branches using global variables as offsets not allowed" );
				yyerror ( e );
				fprintf ( stderr, "error: %d: branches using global variables as offsets not allowed\n", yylineno );
			}
			else if ( ((f=valid_instr_amode($1,AM_ZEROPAGE)) != INVALID_INSTR) &&
						 (loc < 0x100) )
			{
				emit_bin_zeropage ( &(m_6502opcode[f]), loc );
			}
			else if ( (f=valid_instr_amode($1,AM_ABSOLUTE)) != INVALID_INSTR )
			{
				emit_bin_absolute ( &(m_6502opcode[f]), loc );
			}
			else if ( ((f=valid_instr_amode($1,AM_ZEROPAGE)) != INVALID_INSTR) &&
						 (loc >= 0x100) )
			{
				sprintf ( e, "label out of range for instruction format, cannot promote instruction" );
				yyerror ( e );
				fprintf ( stderr, "error: %d: number out of range for instruction format\n", yylineno );
			}
			else
			{
				sprintf ( e, "invalid addressing mode for instruction: %s", $1 );
				yyerror ( e );
				fprintf ( stderr, "error: %d: invalid addressing mode for instruction: %s\n", yylineno, $1 );
			}
		}
		else if ( $2->ref.type == reference_symtab )
		{
			/* backward (known) reference... */
			loc = find_symbol_addr ( $2->ref.ref.symtab->symbol, cur->idx );
			if ( $2->type == compound )
			{
				loc += $2->num.number;
			}
			if ( $2->ref.modifier == use_little_of ) loc &= 0xFF;
			else if ( $2->ref.modifier == use_big_of ) loc = (loc>>8)&0xFF;
			if ( (f=valid_instr_amode($1,AM_RELATIVE)) != INVALID_INSTR )
			{
				emit_bin_relative_ref ( &(m_6502opcode[f]), $2->ref.ref.symtab->ir );
				d = distance ( $2->ref.ref.symtab->ir, ir_tail );
				if ( d > 0x7F )
				{
					sprintf ( e, "label out of range for relative branch" );
					yyerror ( e );
					fprintf ( stderr, "error: %d: branch to label out of page range\n", yylineno );
				}
				add_fixup ( $2->ref.ref.symtab->symbol, fixup_relative, 0 );
			}
			else if ( ((f=valid_instr_amode($1,AM_ZEROPAGE)) != INVALID_INSTR) &&
						 (loc < 0x100) )
			{
				emit_bin_zeropage_ref ( &(m_6502opcode[f]), $2->ref.ref.symtab->ir );
				add_fixup ( $2->ref.ref.symtab->symbol, fixup_zeropage, 0 );
			}
			else if ( (f=valid_instr_amode($1,AM_ABSOLUTE)) != INVALID_INSTR )
			{
				emit_bin_absolute_ref ( &(m_6502opcode[f]), $2->ref.ref.symtab->ir );
				add_fixup ( $2->ref.ref.symtab->symbol, fixup_absolute, 0 );
			}
			else if ( ((f=valid_instr_amode($1,AM_ZEROPAGE)) != INVALID_INSTR) &&
						 (loc >= 0x100) )
			{
				sprintf ( e, "label out of range for instruction format, cannot promote instruction" );
				yyerror ( e );
				fprintf ( stderr, "error: %d: number out of range for instruction format\n", yylineno );
			}
			else
			{
				sprintf ( e, "invalid addressing mode for instruction: %s", $1 );
				yyerror ( e );
				fprintf ( stderr, "error: %d: invalid addressing mode for instruction: %s\n", yylineno, $1 );
			}
		}
		else
		{
			/* forward (unknown) reference... */
			if ( (f=valid_instr_amode($1,AM_RELATIVE)) != INVALID_INSTR )
			{
				emit_bin_relative_ref ( &(m_6502opcode[f]), NULL );
				if ( ($2->ref.modifier == use_big_of) ||
					  ($2->ref.modifier == use_little_of) )
				{
					sprintf ( e, "relative branching to byte-shifted address not allowed" );
					yyerror ( e );
					fprintf ( stderr, "error: %d: relative branching to byte-shifted address not allowed\n", yylineno );
				}
				else
				{
					add_fixup ( $2->ref.ref.symbol, fixup_relative, 0 );
				}
			}
			else if ( (f=valid_instr_amode($1,AM_ZEROPAGE)) != INVALID_INSTR )
			{
				emit_bin_zeropage_ref ( &(m_6502opcode[f]), NULL );
				if ( $2->ref.modifier == use_big_of )
				{
					add_fixup ( $2->ref.ref.symbol, fixup_zeropage_big, 0 );
				}
				else if ( $2->ref.modifier == use_little_of )
				{
					add_fixup ( $2->ref.ref.symbol, fixup_zeropage_little, 0 );
				}
				else
				{
					add_fixup ( $2->ref.ref.symbol, fixup_zeropage, 0 );
				}
			}
			else if ( (f=valid_instr_amode($1,AM_ABSOLUTE)) != INVALID_INSTR )
			{
				emit_bin_absolute_ref ( &(m_6502opcode[f]), NULL );
				if ( $2->ref.modifier == use_big_of )
				{
					add_fixup ( $2->ref.ref.symbol, fixup_absolute_big, 0 );
				}
				else if ( $2->ref.modifier == use_little_of )
				{
					add_fixup ( $2->ref.ref.symbol, fixup_absolute_little, 0 );
				}
				else
				{
					add_fixup ( $2->ref.ref.symbol, fixup_absolute, 0 );
				}
			}
			else
			{
				sprintf ( e, "invalid addressing mode for instruction: %s", $1 );
				yyerror ( e );
				fprintf ( stderr, "error: %d: invalid addressing mode for instruction: %s\n", yylineno, $1 );
			}
		}
	}
}
           | DATAB list
           | DATAW list {
}
           | SEGMENT QUOTEDSTRING {
   if ( set_binary_bank($1,$2) == 0 )
	{
		add_binary_bank ( $1, $2 );
	}
}
           | SEGMENT LABELREF {
   if ( set_binary_bank($1,$2->ref.symbol) == 0 )
   {
    add_binary_bank ( $1, $2->ref.symbol );
   }
}
           | SEGMENT {
	set_binary_bank ( $1, ANONYMOUS_BANK );
}
			  | ORIGN DIGITS {
	if ( $2->number >= 0 )
	{
		set_binary_addr ( $2->number );
		emit_fix ();
	}
	else
	{
		sprintf ( e, "negative value in .org directive" );
		yyerror ( e );
		fprintf ( stderr, "error: %d: negative origin not allowed\n", yylineno );
	}
}
			  | SPACE LABELREF DIGITS { 
	symbol_table* ptr;
	if ( $2->type == reference_symtab )
	{
		sprintf ( e, "multiple declarations of symbol: %s", $2->ref.symtab->symbol );
		yyerror ( e );
		fprintf ( stderr, "error: %d: multiple declarations of symbol: %s\n", yylineno, $2->ref.symtab->symbol );
	}
	else
	{
		/* forward (unknown) reference... */
		add_symbol ( $2->ref.symbol, &ptr );
		emit_label ();
		update_symbol_addr ( ptr, cur->addr );
		update_symbol_ir ( ptr );
   	fixup ( ptr );
		set_binary_addr ( cur->addr+$3->number );
		//dump_symbol_table ();
	}
}
			  | ADVANCE DIGITS { 
	int i, j;
	if ( cur->addr <= $2->number )
	{
		j = ($2->number-cur->addr);
		emit_binm ( 0x00, j );
		emit_fix ();
	}
	else
	{
		sprintf ( e, "negative offset in .advance directive based on current address" );
		yyerror ( e );
		fprintf ( stderr, "error: %d: negative offset in .advance directive based on current address\n", yylineno );
	}
}
         ;
list: DIGITS {
	if ( $1->zp_ok )
	{
		emit_bin ( $1->number );
	}
	else
	{
		emit_bin2 ( $1->number );
	}
}
	 | QUOTEDSTRING {
	char* str = strdup ( $1 );
	int c;
	str += 1; // walk past "
	str[strlen(str)-1] = 0; // chop off trailing "

	for ( c = 0; c < strlen(str); c++ )
	{
		emit_bin ( str[c]&0xFF );
	}
}
    | LABELREF {
	unsigned char f;
	unsigned int  loc;
	if ( $1->type == reference_global )
	{
		/* backward (known) reference... */
		loc = $1->ref.symtab->addr;
		if ( $1->modifier == use_little_of || loc < 0x100 )
		{
			loc &= 0xFF;
			emit_bin ( loc );
		}
		else if ( $1->modifier == use_big_of )
		{
			loc = (loc>>8)&0xFF;
			emit_bin ( loc );
		}
		else
		{
			emit_bin2 ( loc );
		}
	}
	else if ( $1->type == reference_symtab )
	{
		/* backward (known) reference... */
		loc = find_symbol_addr ( $1->ref.symtab->symbol, cur->idx );
		if ( $1->modifier == use_little_of )
		{
			loc &= 0xFF;
			emit_bin_ref ( $1->ref.symtab->ir );
			add_fixup ( $1->ref.symtab->symbol, fixup_datab, 0 );
		}
		else if ( $1->modifier == use_big_of )
		{
			loc = (loc>>8)&0xFF;
			emit_bin_ref ( $1->ref.symtab->ir );
			add_fixup ( $1->ref.symtab->symbol, fixup_datab, 0 );
		}
		else
		{
			emit_bin2_ref ( $1->ref.symtab->ir );
			add_fixup ( $1->ref.symtab->symbol, fixup_dataw, 0 );
		}
	}
	else
	{
		/* forward (unknown) reference... */
		emit_bin2_ref ( NULL );
		add_fixup ( $1->ref.symbol, fixup_dataw, 0 );
	}
}
	 | list ',' DIGITS {
	if ( $3->zp_ok )
	{
		emit_bin ( $3->number );
	}
	else
	{
		emit_bin2 ( $3->number );
	}
}
    | list ',' LABELREF {
	unsigned char f;
	unsigned int  loc;
	if ( $3->type == reference_global )
	{
		/* backward (known) reference... */
		loc = $3->ref.symtab->addr;
		if ( $3->modifier == use_little_of || loc < 0x100 )
		{
			loc &= 0xFF;
			emit_bin ( loc );
		}
		else if ( $3->modifier == use_big_of )
		{
			loc = (loc>>8)&0xFF;
			emit_bin ( loc );
		}
		else
		{
			emit_bin2 ( loc );
		}
	}
	else if ( $3->type == reference_symtab )
	{
		/* backward (known) reference... */
		loc = find_symbol_addr ( $3->ref.symtab->symbol, cur->idx );
		if ( $3->modifier == use_little_of )
		{
			loc &= 0xFF;
			emit_bin_ref ( $3->ref.symtab->ir );
			add_fixup ( $3->ref.symtab->symbol, fixup_datab, 0 );
		}
		else if ( $3->modifier == use_big_of )
		{
			loc = (loc>>8)&0xFF;
			emit_bin_ref ( $3->ref.symtab->ir );
			add_fixup ( $3->ref.symtab->symbol, fixup_datab, 0 );
		}
		else
		{
			emit_bin2_ref ( $3->ref.symtab->ir );
			add_fixup ( $3->ref.symtab->symbol, fixup_dataw, 0 );
		}
	}
	else
	{
		/* forward (unknown) reference... */
		emit_bin2_ref ( NULL );
		add_fixup ( $3->ref.symbol, fixup_dataw, 0 );
	}
}
	 | list ',' QUOTEDSTRING {
	char* str = strdup ( $3 );
	int c;
	str += 1; // walk past "
	str[strlen(str)-1] = 0; // chop off trailing "

	for ( c = 0; c < strlen(str); c++ )
	{
		emit_bin ( str[c]&0xFF );
	}
}
	 ;
expr : DIGITS {
	$$ = get_next_compoundtype ();
	$$->type = number_only;
	$$->num.number = $1->number;
	$$->num.zp_ok = 0;
	if ( ($$->num.number >= -128) &&
	     ($$->num.number < 256) )
	{
		$$->num.zp_ok = 1;
	}
}
      | LABELREF {
   $$ = get_next_compoundtype ();
   $$->type = reference_only;
   $$->ref.ref.symtab = $1->ref.symtab;
}
     | '(' expr ')' {
	$$ = get_next_compoundtype ();
	copy_compound_type ( $$, $2 );
	$$->num.number = $2->num.number;
	$$->num.zp_ok = 0;
	if ( ($$->num.number >= -128) &&
	     ($$->num.number < 256) )
	{
		$$->num.zp_ok = 1;
	}
}
     | '>' expr {
	$$ = get_next_compoundtype ();
	copy_compound_type ( $$, $2 );
	$$->num.number = $2->num.number&0xFF;
	$$->num.zp_ok = 1;
}
     | '<' expr {
	$$ = get_next_compoundtype ();
	copy_compound_type ( $$, $2 );
	$$->num.number = ($2->num.number>>8)&0xFF;
	$$->num.zp_ok = 1;
}
     | '-' expr {
	$$ = get_next_compoundtype ();
	copy_compound_type ( $$, $2 );
	$$->num.number = -$2->num.number;
	if ( ($$->num.number >= -128) &&
	     ($$->num.number < 256) )
	{
		$$->num.zp_ok = 1;
	}
}
     | '~' expr {
	$$ = get_next_compoundtype ();
	copy_compound_type ( $$, $2 );
	$$->num.number = ~$2->num.number;
	$$->num.zp_ok = 0;
	if ( ($$->num.number >= -128) &&
	     ($$->num.number < 256) )
	{
		$$->num.zp_ok = 1;
	}
}
     | expr '+' expr {
	$$ = get_next_compoundtype ();
	combine_compound_types ( $$, $1 );
	$$->num.number += $3->num.number;
	$$->num.zp_ok = 0;
	if ( ($$->num.number >= -128) &&
	     ($$->num.number < 256) )
	{
		$$->num.zp_ok = 1;
	}
}
     | expr '-' expr {
	$$ = get_next_compoundtype ();
	combine_compound_types ( $$, $1 );
	$$->num.number -= $3->num.number;
	$$->num.zp_ok = 0;
	if ( ($$->num.number >= -128) &&
	     ($$->num.number < 256) )
	{
		$$->num.zp_ok = 1;
	}
}
     | expr '*' expr {
	$$ = get_next_compoundtype ();
	combine_compound_types ( $$, $1 );
	$$->num.number *= $3->num.number;
	$$->num.zp_ok = 0;
	if ( ($$->num.number >= -128) &&
	     ($$->num.number < 256) )
	{
		$$->num.zp_ok = 1;
	}
}
     | expr '/' expr {
	$$ = get_next_compoundtype ();
	combine_compound_types ( $$, $1 );
	$$->num.number /= $3->num.number;
	$$->num.zp_ok = 0;
	if ( ($$->num.number >= -128) &&
	     ($$->num.number < 256) )
	{
		$$->num.zp_ok = 1;
	}
}
	;
%%

extern symbol_table* current_label;
extern int errorCount;
extern char* errorStorage;
extern int stab_ent;
extern int stab_max;
extern int ftab_ent;
extern int ftab_max;
extern int btab_ent;
extern int btab_max;
extern char* yytext;

int yyerror(char* s)
{
   char er [ 256 ];
   if ( yytext[0] != 0 )
   {
      sprintf ( er, "%s at '%s'", s, yytext );
   }
   else
   {
      sprintf ( er, "%s", s );
   }
   add_error ( er );
}

void initialize ( void )
{
	ir_table* ptr;
	ir_table* ptd = NULL;
	int idx;

	yylineno = 1;

	free ( errorStorage );
	errorStorage = NULL;
	errorCount = 0;

	for ( ptr = ir_head; ptr != NULL; ptr = ptr->next )
	{
		if ( ptd != NULL ) free ( ptd );
		ptd = ptr;
	}
	free ( ptd );
	ir_head = NULL;
	ir_tail = NULL;

	for ( idx = 0; idx < stab_ent; idx++ )
	{
		free ( stab[idx].symbol );
	}
	free ( stab );
	stab = NULL;
	stab_ent = 0;
	stab_max = 0;

	for ( idx = 0; idx < ftab_ent; idx++ )
	{
		free ( ftab[idx].symbol );
	}
	free ( ftab );
	ftab = NULL;
	ftab_ent = 0;
	ftab_max = 0;

	for ( idx = 0; idx < btab_ent; idx++ )
	{
		free ( btab[idx].symbol );
	}
	free ( btab );
	btab = NULL;
	btab_ent = 0;
	btab_max = 0;
}

void add_error(char *s)
{
	static char error_buffer [ 2048 ];

	errorCount++;
	if ( errorStorage == NULL )
	{
		if ( current_label == NULL )
		{
//			if ( strlen(currentFile) )
//			{
//				sprintf ( error_buffer, "error: %d: in included file %s: ", yylineno, currentFile );
//			}
//			else
//			{
				sprintf ( error_buffer, "error: %d: ", yylineno );
//			}
			errorStorage = (char*) malloc ( strlen(error_buffer)+1+strlen(s)+3 );
			strcpy ( errorStorage, error_buffer );
			strcat ( errorStorage, s );
			strcat ( errorStorage, "\r\n" );
		}
		else
		{
//			if ( strlen(currentFile) )
//			{
//				sprintf ( error_buffer, "error: %d: in included file %s: ", yylineno, currentFile );
//			}
//			else
//			{
				sprintf ( error_buffer, "error: %d: after %s: ", yylineno, current_label->symbol );
//			}
			errorStorage = (char*) malloc ( strlen(error_buffer)+1+strlen(s)+3 );
			strcpy ( errorStorage, error_buffer );
			strcat ( errorStorage, s );
			strcat ( errorStorage, "\r\n" );
		}
	}
	else
	{
		if ( current_label == NULL )
		{
			sprintf ( error_buffer, "error: %d: ", yylineno );
			errorStorage = (char*) realloc ( errorStorage, strlen(errorStorage)+1+strlen(error_buffer)+1+strlen(s)+3 );
			strcat ( errorStorage, error_buffer );
			strcat ( errorStorage, s );
			strcat ( errorStorage, "\r\n" );
		}
		else
		{
			sprintf ( error_buffer, "error: %d: after %s: ", yylineno, current_label->symbol );
			errorStorage = (char*) realloc ( errorStorage, strlen(errorStorage)+1+strlen(error_buffer)+1+strlen(s)+3 );
			strcat ( errorStorage, error_buffer );
			strcat ( errorStorage, s );
			strcat ( errorStorage, "\r\n" );
		}
	}
}

int yywrap(void)
{
  return -1;
}

void fixup ( symbol_table* symtab )
{
	int i, j;
	ir_table* ptr;
	ir_table* save_ptr;
	unsigned int d;
	unsigned int dc;
	unsigned char f;
	unsigned int loc;
	int di;

	for ( i = 0; i < ftab_ent; i++ )
	{
		if ( strcmp(ftab[i].symbol,symtab->symbol) == 0 )
		{
			ptr = ftab[i].ir;

			switch ( ftab[i].type )
			{
				case fixup_datab:
					if ( ptr->ref == NULL )
					{
						ptr->ref = ir_tail;
					}
					loc = symtab->addr&0xFF;
					if ( loc < 0x100 )
					{
						ptr->data[0] = loc;
						ftab[i].fixed = 1;
					}
					else
					{
						sprintf ( e, "unreferenceable label in .db" );
						yyerror ( e );
						fprintf ( stderr, "error: %d: unreferenceable label in .db\n", yylineno );
					}
				break;

				case fixup_dataw:
					if ( ptr->ref == NULL )
					{
						ptr->ref = ir_tail;
					}
					ptr->data[0] = symtab->addr&0xFF;
					ptr->data[1] = (symtab->addr>>8)&0xFF;
					ftab[i].fixed = 1;
				break;

				case fixup_absolute:
				case fixup_abs_idx:
				case fixup_indirect:
					if ( ptr->ref == NULL )
					{
						ptr->ref = ir_tail;
					}
					ptr->data[1] = ptr->ref->addr&0xFF;
					ptr->data[2] = (ptr->ref->addr>>8)&0xFF;
					ftab[i].fixed = 1;
				break;

				case fixup_indirect_big:
				case fixup_absolute_big:
				case fixup_abs_idx_big:
					if ( ptr->ref == NULL )
					{
						ptr->ref = ir_tail;
					}
					ptr->data[1] = (ptr->ref->addr>>8)&0xFF;
					ptr->data[2] = 0;
					ftab[i].fixed = 1;
				break;

				case fixup_indirect_little:
				case fixup_absolute_little:
				case fixup_abs_idx_little:
					if ( ptr->ref == NULL )
					{
						ptr->ref = ir_tail;
					}
					ptr->data[1] = ptr->ref->addr&0xFF;
					ptr->data[2] = 0;
					ftab[i].fixed = 1;
				break;

				case fixup_zeropage:
					if ( ptr->ref == NULL )
					{
						ptr->ref = ir_tail;
					}
					loc = symtab->addr;
					if ( loc < 0x100 )
					{
						ptr->data[1] = loc;
						ftab[i].fixed = 1;
					}
					else
					{
						// promote to absolute if possible...
						if ( (f=valid_instr_amode(m_6502opcode[ptr->data[0]].op,AM_ABSOLUTE)) != INVALID_INSTR )
						{
							// promote to absolute fixup so we keep track of it!
							ftab[i].type = fixup_absolute;

							ptr->data[0] = f&0xFF;
							ptr->len = 3;
							save_ptr = ptr;

							for ( ptr = save_ptr->next; (ptr != NULL) && (ptr->fixed == 0); ptr = ptr->next )
							{
								ptr->addr++;
								if ( ptr->multi == 1 ) ptr->len -= 1;
							}
							for ( ptr = save_ptr; (ptr != NULL) && (ptr->fixed == 0); ptr = ptr->next )
							{
								// move symbol addresses!!
								for ( j = 0; j < stab_ent; j++ )
								{
									if ( !stab[j].global && stab[j].ir == ptr )
									{
										stab[j].addr = ptr->addr;
									}
								}
							}
							save_ptr->data[1] = symtab->addr&0xFF;
							save_ptr->data[2] = (symtab->addr>>8)&0xFF;
							if ( save_ptr->btab_ent == cur->idx )
							{
								cur->addr++;
							}
							ftab[i].fixed = 1;
						}
						else
						{
							sprintf ( e, "unable to promote instruction" );
							yyerror ( e );
							fprintf ( stderr, "error: %d: unable to promote instruction\n", yylineno );
						}
					}
				break;

				case fixup_zeropage_big:
					if ( ptr->ref == NULL )
					{
						ptr->ref = ir_tail;
					}
					loc = (symtab->addr>>8)&0xFF;
					if ( loc < 0x100 )
					{
						ptr->data[1] = loc;
						ftab[i].fixed = 1;
					}
					else
					{
						// promote to absolute if possible...
						if ( (f=valid_instr_amode(m_6502opcode[ptr->data[0]].op,AM_ABSOLUTE)) != INVALID_INSTR )
						{
							// promote to absolute fixup so we keep track of it!
							ftab[i].type = fixup_absolute_big;

							ptr->data[0] = f&0xFF;
							ptr->len = 3;
							save_ptr = ptr;

							for ( ptr = save_ptr->next; (ptr != NULL) && (ptr->fixed == 0); ptr = ptr->next )
							{
								ptr->addr++;
								if ( ptr->multi == 1 ) ptr->len -= 1;
							}
							for ( ptr = save_ptr; (ptr != NULL) && (ptr->fixed == 0); ptr = ptr->next )
							{
								// move symbol addresses!!
								for ( j = 0; j < stab_ent; j++ )
								{
									if ( !stab[j].global && stab[j].ir == ptr )
									{
										stab[j].addr = ptr->addr;
									}
								}
							}
							save_ptr->data[1] = loc;
							save_ptr->data[2] = 0;
							if ( save_ptr->btab_ent == cur->idx )
							{
								cur->addr++;
							}
							ftab[i].fixed = 1;
						}
						else
						{
							sprintf ( e, "unable to promote instruction" );
							yyerror ( e );
							fprintf ( stderr, "error: %d: unable to promote instruction\n", yylineno );
						}
					}
				break;

				case fixup_zeropage_little:
					if ( ptr->ref == NULL )
					{
						ptr->ref = ir_tail;
					}
					loc = symtab->addr&0xFF;
					if ( loc < 0x100 )
					{
						ptr->data[1] = loc;
						ftab[i].fixed = 1;
					}
					else
					{
						// promote to absolute if possible...
						if ( (f=valid_instr_amode(m_6502opcode[ptr->data[0]].op,AM_ABSOLUTE)) != INVALID_INSTR )
						{
							// promote to absolute fixup so we keep track of it!
							ftab[i].type = fixup_absolute_little;

							ptr->data[0] = f&0xFF;
							ptr->len = 3;
							save_ptr = ptr;

							for ( ptr = save_ptr->next; (ptr != NULL) && (ptr->fixed == 0); ptr = ptr->next )
							{
								ptr->addr++;
								if ( ptr->multi == 1 ) ptr->len -= 1;
							}
							for ( ptr = save_ptr; (ptr != NULL) && (ptr->fixed == 0); ptr = ptr->next )
							{
								// move symbol addresses!!
								for ( j = 0; j < stab_ent; j++ )
								{
									if ( !stab[j].global && stab[j].ir == ptr )
									{
										stab[j].addr = ptr->addr;
									}
								}
							}
							save_ptr->data[1] = loc;
							save_ptr->data[2] = 0;
							if ( save_ptr->btab_ent == cur->idx )
							{
								cur->addr++;
							}
							ftab[i].fixed = 1;
						}
						else
						{
							sprintf ( e, "unable to promote instruction" );
							yyerror ( e );
							fprintf ( stderr, "error: %d: unable to promote instruction\n", yylineno );
						}
					}
				break;

				case fixup_zp_idx:
					if ( ptr->ref == NULL )
					{
						ptr->ref = ir_tail;
					}
					loc = symtab->addr;
					if ( loc < 0x100 )
					{
						ptr->data[1] = loc;
						ftab[i].fixed = 1;
					}
					else
					{
						// promote to absolute indexed if possible...
						if ( ((m_6502opcode[ptr->data[0]].amode == AM_ZEROPAGE_INDEXED_X) &&
						     ((f=valid_instr_amode(m_6502opcode[ptr->data[0]].op,AM_ABSOLUTE_INDEXED_X)) != INVALID_INSTR)) )
						{
							// promote to absolute fixup so we keep track of it!
							ftab[i].type = fixup_abs_idx;

							ptr->data[0] = f&0xFF;
							ptr->len = 3;
							save_ptr = ptr;

							for ( ptr = save_ptr->next; (ptr != NULL) && (ptr->fixed == 0); ptr = ptr->next )
							{
								ptr->addr++;
								if ( ptr->multi == 1 ) ptr->len -= 1;
							}
							for ( ptr = save_ptr; (ptr != NULL) && (ptr->fixed == 0); ptr = ptr->next )
							{
								// move symbol addresses!!
								for ( j = 0; j < stab_ent; j++ )
								{
									if ( !stab[j].global && stab[j].ir == ptr )
									{
										stab[j].addr = ptr->addr;
									}
								}
							}
							save_ptr->data[1] = symtab->addr&0xFF;
							save_ptr->data[2] = (symtab->addr>>8)&0xFF;
							if ( save_ptr->btab_ent == cur->idx )
							{
								cur->addr++;
							}
							ftab[i].fixed = 1;
						}
						else if ( ((m_6502opcode[ptr->data[0]].amode == AM_ZEROPAGE_INDEXED_Y) &&
						          ((f=valid_instr_amode(m_6502opcode[ptr->data[0]].op,AM_ABSOLUTE_INDEXED_Y)) != INVALID_INSTR)) )
						{
							// promote to absolute fixup so we keep track of it!
							ftab[i].type = fixup_abs_idx;

							ptr->data[0] = f&0xFF;
							ptr->len = 3;
							save_ptr = ptr;

							for ( ptr = save_ptr->next; (ptr != NULL) && (ptr->fixed == 0); ptr = ptr->next )
							{
								ptr->addr++;
								if ( ptr->multi == 1 ) ptr->len -= 1;
							}
							for ( ptr = save_ptr; (ptr != NULL) && (ptr->fixed == 0); ptr = ptr->next )
							{
								// move symbol addresses!!
								for ( j = 0; j < stab_ent; j++ )
								{
									if ( !stab[j].global && stab[j].ir == ptr )
									{
										stab[j].addr = ptr->addr;
									}
								}
							}
							save_ptr->data[1] = symtab->addr&0xFF;
							save_ptr->data[2] = (symtab->addr>>8)&0xFF;
							if ( save_ptr->btab_ent == cur->idx )
							{
								cur->addr++;
							}
							ftab[i].fixed = 1;
						}
						else
						{
							sprintf ( e, "unable to promote instruction" );
							yyerror ( e );
							fprintf ( stderr, "error: %d: unable to promote instruction\n", yylineno );
						}
					}
				break;

				case fixup_zp_idx_big:
					if ( ptr->ref == NULL )
					{
						ptr->ref = ir_tail;
					}
					loc = (symtab->addr>>8)&0xFF;
					if ( loc < 0x100 )
					{
						ptr->data[1] = loc;
						ftab[i].fixed = 1;
					}
					else
					{
						// promote to absolute indexed if possible...
						if ( ((m_6502opcode[ptr->data[0]].amode == AM_ZEROPAGE_INDEXED_X) &&
						     ((f=valid_instr_amode(m_6502opcode[ptr->data[0]].op,AM_ABSOLUTE_INDEXED_X)) != INVALID_INSTR)) )
						{
							// promote to absolute fixup so we keep track of it!
							ftab[i].type = fixup_abs_idx_big;

							ptr->data[0] = f&0xFF;
							ptr->len = 3;
							save_ptr = ptr;

							for ( ptr = save_ptr->next; (ptr != NULL) && (ptr->fixed == 0); ptr = ptr->next )
							{
								ptr->addr++;
								if ( ptr->multi == 1 ) ptr->len -= 1;
							}
							for ( ptr = save_ptr; (ptr != NULL) && (ptr->fixed == 0); ptr = ptr->next )
							{
								// move symbol addresses!!
								for ( j = 0; j < stab_ent; j++ )
								{
									if ( !stab[j].global && stab[j].ir == ptr )
									{
										stab[j].addr = ptr->addr;
									}
								}
							}
							save_ptr->data[1] = loc;
							save_ptr->data[2] = 0;
							if ( save_ptr->btab_ent == cur->idx )
							{
								cur->addr++;
							}
							ftab[i].fixed = 1;
						}
						else if ( ((m_6502opcode[ptr->data[0]].amode == AM_ZEROPAGE_INDEXED_Y) &&
						          ((f=valid_instr_amode(m_6502opcode[ptr->data[0]].op,AM_ABSOLUTE_INDEXED_Y)) != INVALID_INSTR)) )
						{
							// promote to absolute fixup so we keep track of it!
							ftab[i].type = fixup_abs_idx;

							ptr->data[0] = f&0xFF;
							ptr->len = 3;
							save_ptr = ptr;

							for ( ptr = save_ptr->next; (ptr != NULL) && (ptr->fixed == 0); ptr = ptr->next )
							{
								ptr->addr++;
								if ( ptr->multi == 1 ) ptr->len -= 1;
							}
							for ( ptr = save_ptr; (ptr != NULL) && (ptr->fixed == 0); ptr = ptr->next )
							{
								// move symbol addresses!!
								for ( j = 0; j < stab_ent; j++ )
								{
									if ( !stab[j].global && stab[j].ir == ptr )
									{
										stab[j].addr = ptr->addr;
									}
								}
							}
							save_ptr->data[1] = loc;
							save_ptr->data[2] = 0;
							if ( save_ptr->btab_ent == cur->idx )
							{
								cur->addr++;
							}
							ftab[i].fixed = 1;
						}
						else
						{
							sprintf ( e, "unable to promote instruction" );
							yyerror ( e );
							fprintf ( stderr, "error: %d: unable to promote instruction\n", yylineno );
						}
					}
				break;

				case fixup_zp_idx_little:
					if ( ptr->ref == NULL )
					{
						ptr->ref = ir_tail;
					}
					loc = symtab->addr&0xFF;
					if ( loc < 0x100 )
					{
						ptr->data[1] = loc;
						ftab[i].fixed = 1;
					}
					else
					{
						// promote to absolute indexed if possible...
						if ( ((m_6502opcode[ptr->data[0]].amode == AM_ZEROPAGE_INDEXED_X) &&
						     ((f=valid_instr_amode(m_6502opcode[ptr->data[0]].op,AM_ABSOLUTE_INDEXED_X)) != INVALID_INSTR)) )
						{
							// promote to absolute fixup so we keep track of it!
							ftab[i].type = fixup_abs_idx_little;

							ptr->data[0] = f&0xFF;
							ptr->len = 3;
							save_ptr = ptr;

							for ( ptr = save_ptr->next; (ptr != NULL) && (ptr->fixed == 0); ptr = ptr->next )
							{
								ptr->addr++;
								if ( ptr->multi == 1 ) ptr->len -= 1;
							}
							for ( ptr = save_ptr; (ptr != NULL) && (ptr->fixed == 0); ptr = ptr->next )
							{
								// move symbol addresses!!
								for ( j = 0; j < stab_ent; j++ )
								{
									if ( !stab[j].global && stab[j].ir == ptr )
									{
										stab[j].addr = ptr->addr;
									}
								}
							}
							save_ptr->data[1] = loc;
							save_ptr->data[2] = 0;
							if ( save_ptr->btab_ent == cur->idx )
							{
								cur->addr++;
							}
							ftab[i].fixed = 1;
						}
						else if ( ((m_6502opcode[ptr->data[0]].amode == AM_ZEROPAGE_INDEXED_Y) &&
						          ((f=valid_instr_amode(m_6502opcode[ptr->data[0]].op,AM_ABSOLUTE_INDEXED_Y)) != INVALID_INSTR)) )
						{
							// promote to absolute fixup so we keep track of it!
							ftab[i].type = fixup_abs_idx;

							ptr->data[0] = f&0xFF;
							ptr->len = 3;
							save_ptr = ptr;

							for ( ptr = save_ptr->next; (ptr != NULL) && (ptr->fixed == 0); ptr = ptr->next )
							{
								ptr->addr++;
								if ( ptr->multi == 1 ) ptr->len -= 1;
							}
							for ( ptr = save_ptr; (ptr != NULL) && (ptr->fixed == 0); ptr = ptr->next )
							{
								// move symbol addresses!!
								for ( j = 0; j < stab_ent; j++ )
								{
									if ( !stab[j].global && stab[j].ir == ptr )
									{
										stab[j].addr = ptr->addr;
									}
								}
							}
							save_ptr->data[1] = loc;
							save_ptr->data[2] = 0;
							if ( save_ptr->btab_ent == cur->idx )
							{
								cur->addr++;
							}
							ftab[i].fixed = 1;
						}
						else
						{
							sprintf ( e, "unable to promote instruction" );
							yyerror ( e );
							fprintf ( stderr, "error: %d: unable to promote instruction\n", yylineno );
						}
					}
				break;

				case fixup_pre_idx_ind:
				case fixup_post_idx_ind:
					if ( ptr->ref == NULL )
					{
						ptr->ref = ir_tail;
					}
					loc = symtab->addr;
					if ( loc < 0x100 )
					{
						ptr->data[1] = loc;
						ftab[i].fixed = 1;
					}
					else
					{
						sprintf ( e, "instruction operand unreachable, not on zero-page: %s", ftab[i].symbol );
						yyerror ( e );
						fprintf ( stderr, "error: %d: instruction operand unreachable, not on zero-page: %s\n", yylineno, ftab[i].symbol );
					}
				break;

				case fixup_pre_idx_ind_big:
				case fixup_post_idx_ind_big:
					if ( ptr->ref == NULL )
					{
						ptr->ref = ir_tail;
					}
					loc = (symtab->addr>>8)&0xFF;
					if ( loc < 0x100 )
					{
						ptr->data[1] = loc;
						ftab[i].fixed = 1;
					}
					else
					{
						sprintf ( e, "instruction operand unreachable, not on zero-page: %s", ftab[i].symbol );
						yyerror ( e );
						fprintf ( stderr, "error: %d: instruction operand unreachable, not on zero-page: %s\n", yylineno, ftab[i].symbol );
					}
				break;

				case fixup_pre_idx_ind_little:
				case fixup_post_idx_ind_little:
					if ( ptr->ref == NULL )
					{
						ptr->ref = ir_tail;
					}
					loc = symtab->addr&0xFF;
					if ( loc < 0x100 )
					{
						ptr->data[1] = loc;
						ftab[i].fixed = 1;
					}
					else
					{
						sprintf ( e, "instruction operand unreachable, not on zero-page: %s", ftab[i].symbol );
						yyerror ( e );
						fprintf ( stderr, "error: %d: instruction operand unreachable, not on zero-page: %s\n", yylineno, ftab[i].symbol );
					}
				break;

				case fixup_relative:
					if ( ptr->ref == NULL )
					{
						ptr->ref = ir_tail;
					}
					di = -(ptr->next->addr - ptr->ref->addr);
					printf ( "fix rel distance: %d\n", di );
					if ( (di >= -128) && (di <= 127) ) 
					{
						ptr->data[1] = di&0xFF;
						ftab[i].fixed = 1;
					}
					else
					{
						sprintf ( e, "branch to label out of range: %s", symtab->symbol );
						yyerror ( e );
						fprintf ( stderr, "error: %d: branch to label out of range: %s\n", yylineno, symtab->symbol );
					}
				break;

				case fixup_immediate_big:
					if ( ptr->ref == NULL )
					{
						ptr->ref = ir_tail;
					}
					ptr->data[1] = (symtab->addr>>8)&0xFF;
					ftab[i].fixed = 1;
				break;

				case fixup_immediate:
					if ( ptr->ref == NULL )
					{
						ptr->ref = ir_tail;
					}
					if ( symtab->addr < 0x100 )
					{
						ptr->data[1] = symtab->addr&0xFF;
					}
					else
					{
						sprintf ( e, "immediate value too large: %s", symtab->symbol );
						yyerror ( e );
						fprintf ( stderr, "error: %d: immediate value too large: %s\n", yylineno, symtab->symbol );
					}
					ftab[i].fixed = 1;
				break;

				case fixup_immediate_little:
					if ( ptr->ref == NULL )
					{
						ptr->ref = ir_tail;
					}
					ptr->data[1] = symtab->addr&0xFF;
					ftab[i].fixed = 1;
				break;
			}
		}
	}

   //dump_fixup_table ();
}

unsigned int distance ( ir_table* from, ir_table* to )
{
	ir_table* ptr;

	unsigned int d = 0;
	for ( ptr = from; ptr != to->next; ptr = ptr->next )
	{
		d += ptr->len;
	}
	return d;
}

void check_fixup ( void )
{
	int i;

	// do fixups
	for ( i = 0; i < stab_ent; i++ )
	{
		fixup ( &(stab[i]) );
	}

	// check fixups
	for ( i = 0; i < ftab_ent; i++ )
	{
		if ( ftab[i].fixed == 0 )
		{
			sprintf ( e, "reference to undefined or unreachable symbol: %s", ftab[i].symbol );
			yyerror ( e );
			fprintf ( stderr, "error: %d: reference to undefined or unreachable symbol: %s\n", ftab[i].lineno, ftab[i].symbol );
		}
	}
}

unsigned char add_fixup ( char* symbol, fixup_type type, int modifier )
{
	unsigned char a = 1;
	unsigned int i;

	if ( ftab == NULL )
	{
		ftab = (fixup_table*)calloc ( FTAB_ENT_INC, sizeof(fixup_table) );
		if ( ftab != NULL )
		{
			ftab_max += FTAB_ENT_INC;
			ftab[ftab_ent].symbol = (char*)malloc ( strlen(symbol)+1 );
			if ( ftab[ftab_ent].symbol != NULL )
			{
				memset ( ftab[ftab_ent].symbol, 0, strlen(symbol)+1 );
				strncpy ( ftab[ftab_ent].symbol, symbol, strlen(symbol) );
				ftab[ftab_ent].type = type;
				ftab[ftab_ent].ir = ir_tail; // assumes fixing up last emited instruction
				ftab[ftab_ent].lineno = yylineno;
				ftab[ftab_ent].fixed = 0;
				ftab[ftab_ent].modifier = modifier;
			}
		}
		else
		{
			sprintf ( e, "unable to allocate memory for fixup" );
			yyerror ( e );
			fprintf ( stderr, "error: unable to allocate memory for fixup!\n" );
		}
	}
	else
	{
		if ( ftab_ent < ftab_max )
		{
			ftab[ftab_ent].symbol = (char*)malloc ( strlen(symbol)+1 );
			if ( ftab[ftab_ent].symbol != NULL )
			{
				memset ( ftab[ftab_ent].symbol, 0, strlen(symbol)+1 );
				strncpy ( ftab[ftab_ent].symbol, symbol, strlen(symbol) );
				ftab[ftab_ent].type = type;
				ftab[ftab_ent].ir = ir_tail; // assumes fixing up last emited instruction
				ftab[ftab_ent].lineno = yylineno;
				ftab[ftab_ent].fixed = 0;
				ftab[ftab_ent].modifier = modifier;
			}
		}
		else
		{
			ftab_max += FTAB_ENT_INC;
			ftab = (fixup_table*)realloc ( ftab, ftab_max*sizeof(fixup_table) );
			if ( ftab != NULL )
			{
				ftab[ftab_ent].symbol = (char*)malloc ( strlen(symbol)+1 );
				if ( ftab[ftab_ent].symbol != NULL )
				{
					memset ( ftab[ftab_ent].symbol, 0, strlen(symbol)+1 );
					strncpy ( ftab[ftab_ent].symbol, symbol, strlen(symbol) );
					ftab[ftab_ent].type = type;
					ftab[ftab_ent].ir = ir_tail; // assumes fixing up last emited instruction
					ftab[ftab_ent].lineno = yylineno;
					ftab[ftab_ent].fixed = 0;
					ftab[ftab_ent].modifier = modifier;
				}
			}
			else
			{
				sprintf ( e, "unable to allocate memory for fixup" );
				yyerror ( e );
				fprintf ( stderr, "error: unable to allocate memory for fixup!\n" );
			}
		}
	}

	ftab_ent++;

	//dump_fixup_table ();

	return a;
}

unsigned char add_binary_bank ( segment_type type, char* symbol )
{
	unsigned char a = 1;
	unsigned int i;

	if ( btab == NULL )
	{
		btab = (binary_table*)calloc ( BTAB_ENT_INC, sizeof(binary_table) );
		if ( btab != NULL )
		{
			btab_max += BTAB_ENT_INC;
			btab[btab_ent].symbol = (char*)malloc ( strlen(symbol)+1 );
			if ( btab[btab_ent].symbol != NULL )
			{
				memset ( btab[btab_ent].symbol, 0, strlen(symbol)+1 );
				strncpy ( btab[btab_ent].symbol, symbol, strlen(symbol) );
				btab[btab_ent].idx = btab_ent;
				btab[btab_ent].type = type;
				btab[btab_ent].addr = 0;
			}
		}
		else
		{
			sprintf ( e, "unable to allocate memory for tables" );
			yyerror ( e );
			fprintf ( stderr, "error: unable to allocate memory for tables!\n" );
		}
	}
	else
	{
		if ( btab_ent < btab_max )
		{
			btab[btab_ent].symbol = (char*)malloc ( strlen(symbol)+1 );
			if ( btab[btab_ent].symbol != NULL )
			{
				memset ( btab[btab_ent].symbol, 0, strlen(symbol)+1 );
				strncpy ( btab[btab_ent].symbol, symbol, strlen(symbol) );
				btab[btab_ent].idx = btab_ent;
				btab[btab_ent].type = type;
				btab[btab_ent].addr = 0;
			}
		}
		else
		{
			btab_max += BTAB_ENT_INC;
			btab = (binary_table*)realloc ( btab, btab_max*sizeof(binary_table) );
			if ( btab != NULL )
			{
				btab[btab_ent].symbol = (char*)malloc ( strlen(symbol)+1 );
				if ( btab[btab_ent].symbol != NULL )
				{
					memset ( btab[btab_ent].symbol, 0, strlen(symbol)+1 );
					strncpy ( btab[btab_ent].symbol, symbol, strlen(symbol) );
					btab[btab_ent].idx = btab_ent;
					btab[btab_ent].type = type;
					btab[btab_ent].addr = 0;
				}
			}
			else
			{
				sprintf ( e, "unable to allocate memory for tables" );
				yyerror ( e );
				fprintf ( stderr, "error: unable to allocate memory for tables!\n" );
			}
		}
	}

	cur = &(btab[btab_ent]);
	btab_ent++;

	//dump_binary_table ();

	return a;
}

unsigned char set_binary_bank ( segment_type type, char* bankname )
{
	unsigned char a = 0;
	unsigned int i;
	for ( i = 0; i < btab_ent; i++ )
	{
		if ( (type == btab[i].type) &&
		     (strcmp(bankname,btab[i].symbol) == 0) )
		{
			a = 1;
			cur = &(btab[i]);
			return a;
		}
	}
	return a;
}

void output_binary_direct ( char** buffer, int* size )
{
	int pos = 0;
	ir_table* ptr1;
	ir_table* ptr2;
	ir_table* ptr3;
	ir_table* ptr4 = NULL;
	ir_table* ptrl;
	unsigned int addr;
	int       i;
	int       bank;
	int       lowest_bank_addr = -1;
	int       lowest_bank;

	(*buffer) = (char*) malloc ( DEFAULT_DIRECT );
	(*size) = DEFAULT_DIRECT;
	if ( (*buffer) != NULL )
	{
		for ( bank = 0; bank < btab_ent; bank++ )
		{
			lowest_bank_addr = 0xFFFFFFFF;
			for ( ptrl = ir_head; ptrl != NULL; ptrl = ptrl->next )
			{
				if ( btab[ptrl->btab_ent].type == text_segment )
				{
					if ( (ptrl->addr < lowest_bank_addr) && (ptrl->emitted == 0) )
					{
						lowest_bank_addr = ptrl->addr;
						lowest_bank = ptrl->btab_ent;
					}
				}
			}

			ptr4 = NULL;
			for ( ptr1 = ir_head; ptr1 != NULL; ptr1 = ptr1->next )
			{
				if ( (ptr1->btab_ent == lowest_bank) && (btab[ptr1->btab_ent].type == text_segment) )
				{
					addr = 0xFFFFFFFF;
				
					// find lowest addr
					ptr3 = NULL;
					for ( ptr2 = ir_head; ptr2 != NULL; ptr2 = ptr2->next )
					{
						if ( (ptr2->btab_ent == lowest_bank) && (btab[ptr2->btab_ent].type == text_segment) )
						{
							if ( (ptr2->addr < addr) && (ptr2->emitted == 0) )
							{
								ptr3 = ptr2;
								addr = ptr3->addr;
							}
						}
						else if ( (ptr2->btab_ent == lowest_bank) && (btab[ptr2->btab_ent].type == data_segment) )
						{
							ptr2->emitted = 1;
						}
					}
					if ( ptr3 != NULL )
					{
						ptr3->emitted = 1;
						ptr4 = ptr3;
						if ( ptr3->multi == 0 )
						{
							for ( i = 0; i < ptr3->len; i++ )
							{
								(*buffer)[pos++] = (ptr3->data[i])&0xFF;
								if ( pos == (*size) )
								{
									(*size) += DEFAULT_DIRECT;
									(*buffer) = (char*) realloc ( (*buffer), (*size) );
								}
							}
						}
						else
						{
							for ( i = 0; i < ptr3->len; i++ )
							{
								(*buffer)[pos++] = (ptr3->data[0])&0xFF;
								if ( pos == (*size) )
								{
									(*size) += DEFAULT_DIRECT;
									(*buffer) = (char*) realloc ( (*buffer), (*size) );
								}
							}
						}
					}
				}
			}
		}
	}
	(*size) = pos;
}

void output_binary ( void )
{
#if 0
	ir_table* ptr1;
	ir_table* ptr2;
	ir_table* ptr3 = NULL;
	ir_table* ptr4 = NULL;
	unsigned int addr;
	int       i;
	int ctr = 0;

	for ( ptr1 = ir_head; ptr1 != NULL; ptr1 = ptr1->next )
	{
		if ( btab[ptr1->btab_ent].type == text_segment )
		{
			addr = 0xFFFFFFFF;
		
			// find lowest addr
			for ( ptr2 = ir_head; ptr2 != NULL; ptr2 = ptr2->next )
			{
				if ( btab[ptr2->btab_ent].type == text_segment )
				{
					if ( (ptr2->addr < addr) && (ptr2->emitted == 0) )
					{
						ptr3 = ptr2;
						addr = ptr3->addr;
					}
				}
				else
				{
					ptr2->emitted = 1;
				}
			}

			if ( (ptr4 != NULL) &&
				  (ptr4->addr+ptr4->len < ptr3->addr) )
			{
				printf ( "\n\nneed to output %d zeros\n\n", ptr3->addr-(ptr4->addr+ptr4->len) );
				for ( i = 0; i < ptr3->addr-(ptr4->addr+ptr4->len); i++ )
				{
	//				fprintf ( stdout, "%02X", 0 );
					ctr++;
				}
			}

			ptr3->emitted = 1;
			ptr4 = ptr3;
			fprintf ( stdout, "%04X %04X: ", ctr, ptr3->addr );
			for ( i = 0; i < ptr3->len; i++ )
			{
				fprintf ( stdout, "%02X ", ptr3->data[i] );
				ctr++;
			}
			fprintf ( stdout, "\n" );
		}
	}
	printf ( "\n%Xh bytes output\n", ctr );
#endif
   //dump_ir_table ();
}

unsigned char valid_instr_amode ( int instr, int amode )
{
	unsigned char a = INVALID_INSTR;
	int i;

	for ( i = 0; i < 256; i++ )
	{
		if ( (strlen(m_6502opcode[i].name) > 0) && 
			  (strncmp(m_6502opcode[i].name,m_6502opcode[instr].name,3) == 0) &&
			  (m_6502opcode[i].amode == amode) )
		{
			a = i;
			return a;
		}
	}
	return a;
}

unsigned char update_symbol_addr ( symbol_table* stab, unsigned int addr )
{
	unsigned char a = 0;

	stab->addr = addr;

	return a;
}

unsigned char update_symbol_ir ( symbol_table* stab )
{
	unsigned char a = 0;

	stab->ir = ir_tail; // assumes updating latest emited instruction...

	return a;
}

unsigned int find_symbol_addr ( char* sym, int bank )
{
	unsigned int addr = SYMBOL_NOT_FOUND;
	symbol_table* ptr = find_symbol ( sym, bank );

	if ( ptr != NULL )
	{
		addr = ptr->addr;
	}

	return addr;
}

void dump_ir_table ( void )
{
	ir_table* ptr = ir_head;
	int       i;

	for ( ; ptr != NULL; ptr = ptr->next )
	{
		if ( ptr->multi == 0 )
		{
			printf ( "%08x %d %04X: ", ptr, ptr->btab_ent, ptr->addr );
			for ( i = 0; i < ptr->len; i++ )
			{
				printf ( "%02X ", ptr->data[i] );
			}
			if ( ptr->fixed == 1 )
			{
				printf ( "(fixed)" );
			}
			printf ( "\n" );
		}
		else
		{
			printf ( "%08x %d %04X: %05X (%d) %02X\n", ptr, ptr->btab_ent, ptr->addr, ptr->len, ptr->len, ptr->data[0] );
		}
	}
}

void dump_symbol_table ( void )
{
	int i;
	for ( i = 0; i < stab_ent; i++ )
	{
		printf ( "%d: %s %s value %04X (%08x) %d\n", i, stab[i].symbol, stab[i].global?"global":"", stab[i].addr, stab[i].ir );
	}
}

void dump_fixup_table ( void )
{
	int i;
	for ( i = 0; i < ftab_ent; i++ )
	{
		printf ( "%d: %s fixed-%d type-%d ir-%08x\n", i, ftab[i].symbol, ftab[i].fixed, ftab[i].type, ftab[i].ir );
	}
}

void dump_binary_table ( void )
{
	int i;
	for ( i = 0; i < btab_ent; i++ )
	{
		printf ( "%c%d: %s %s addr-%04X\n", cur==&(btab[i])?'*':' ', i, btab[i].type==text_segment?"TEXT":"DATA", btab[i].symbol, btab[i].addr );
	}
}

void set_binary_addr ( unsigned int addr )
{
	cur->addr = addr;
	//dump_binary_table ();
}

ir_table* emit_ir ( void )
{
	ir_table* ptr;
	if ( ir_tail == NULL )
	{
		ir_head = (ir_table*) malloc ( sizeof(ir_table) );
		if ( ir_head != NULL )
		{
			ir_tail = ir_head;
			ir_tail->btab_ent = cur->idx;
			ir_tail->addr = 0;
			ir_tail->emitted = 0;
			ir_tail->multi = 0;
			ir_tail->label = 0;
			ir_tail->fixed = 0;
			ir_tail->ref = NULL;
			ir_tail->next = NULL;
			ir_tail->prev = NULL;
		}
		else
		{
			yyerror ( "cannot allocate memory" );
			fprintf ( stderr, "error: cannot allocate memory!\n" );
		}
	}
	else
	{
		ptr = (ir_table*) malloc ( sizeof(ir_table) );
		if ( ptr != NULL )
		{
			ir_tail->next = ptr;
			ptr->prev = ir_tail;
			ptr->next = NULL;
			ir_tail = ptr;
			ir_tail->btab_ent = cur->idx;
			ir_tail->addr = 0;
			ir_tail->emitted = 0;
			ir_tail->multi = 0;
			ir_tail->label = 0;
			ir_tail->fixed = 0;
			ir_tail->ref = NULL;
		}
		else
		{
			yyerror ( "cannot allocate memory" );
			fprintf ( stderr, "error: cannot allocate memory!\n" );
		}
	}
   //dump_ir_table ();
	return ir_tail;
}

void emit_fix ( void )
{
	ir_table* ptr = emit_ir ();
	ptr->len = 0;
	ptr->fixed = 1;
	ptr->addr = cur->addr;
	cur->addr += ptr->len;
}

void emit_label ( void )
{
	ir_table* ptr = emit_ir ();
	ptr->len = 0;
	ptr->label = 1;
	ptr->addr = cur->addr;
	cur->addr += ptr->len;
}

void emit_bin ( unsigned char data )
{
	ir_table* ptr = emit_ir ();
	ptr->data[0] = data;
	ptr->len = 1;
	ptr->addr = cur->addr;
	cur->addr += ptr->len;
}

void emit_bin_ref ( ir_table* ref )
{
	ir_table* ptr = emit_ir ();
	ptr->data[0] = 0;
	ptr->len = 1;
	ptr->addr = cur->addr;
	ptr->ref = ref;
	cur->addr += ptr->len;
}

void emit_bin2 ( unsigned short data )
{
	ir_table* ptr = emit_ir ();
	ptr->data[0] = data&0xFF;
	ptr->data[1] = (data>>8)&0xFF;
	ptr->len = 2;
	ptr->addr = cur->addr;
	cur->addr += ptr->len;
}

void emit_bin2_ref ( ir_table* ref )
{
	ir_table* ptr = emit_ir ();
	ptr->data[0] = 0;
	ptr->data[1] = 0;
	ptr->len = 2;
	ptr->addr = cur->addr;
	ptr->ref = ref;
	cur->addr += ptr->len;
}

void emit_binm ( unsigned char data, int m )
{
	ir_table* ptr = emit_ir ();
	ptr->data[0] = data;
	ptr->len = m;
	ptr->multi = 1;
	ptr->addr = cur->addr;
	cur->addr += ptr->len;
}

void emit_bin_implied ( C6502_opcode* opcode )
{
	ir_table* ptr = emit_ir ();
	ptr->data[0] = opcode->op;
	ptr->len = 1;
	ptr->addr = cur->addr;
	cur->addr += ptr->len;
}

void emit_bin_accumulator ( C6502_opcode* opcode )
{
	ir_table* ptr = emit_ir ();
	ptr->data[0] = opcode->op;
	ptr->len = 1;
	ptr->addr = cur->addr;
	cur->addr += ptr->len;
}

void emit_bin_immediate ( C6502_opcode* opcode, unsigned char data )
{
	ir_table* ptr = emit_ir ();
	ptr->data[0] = opcode->op;
	ptr->data[1] = data;
	ptr->len = 2;
	ptr->addr = cur->addr;
	cur->addr += ptr->len;
}

void emit_bin_immediate_ref ( C6502_opcode* opcode, ir_table* ref )
{
	ir_table* ptr = emit_ir ();
	ptr->data[0] = opcode->op;
	ptr->data[1] = 0;
	ptr->len = 2;
	ptr->addr = cur->addr;
	ptr->ref = ref;
	cur->addr += ptr->len;
}

void emit_bin_absolute ( C6502_opcode* opcode, unsigned short data )
{
	ir_table* ptr = emit_ir ();
	ptr->data[0] = opcode->op;
	ptr->data[1] = data&0xFF;
	ptr->data[2] = (data>>8)&0xFF;
	ptr->len = 3;
	ptr->addr = cur->addr;
	cur->addr += ptr->len;
}

void emit_bin_absolute_ref ( C6502_opcode* opcode, ir_table* ref )
{
	ir_table* ptr = emit_ir ();
	ptr->data[0] = opcode->op;
	ptr->data[1] = 0;
	ptr->data[2] = 0;
	ptr->len = 3;
	ptr->addr = cur->addr;
	ptr->ref = ref;
	cur->addr += ptr->len;
}

void emit_bin_zeropage ( C6502_opcode* opcode, unsigned char data )
{
	ir_table* ptr = emit_ir ();
	ptr->data[0] = opcode->op;
	ptr->data[1] = data;
	ptr->len = 2;
	ptr->addr = cur->addr;
	cur->addr += ptr->len;
}

void emit_bin_zeropage_ref ( C6502_opcode* opcode, ir_table* ref )
{
	ir_table* ptr = emit_ir ();
	ptr->data[0] = opcode->op;
	ptr->data[1] = 0;
	ptr->len = 2;
	ptr->addr = cur->addr;
	ptr->ref = ref;
	cur->addr += ptr->len;
}

void emit_bin_relative ( C6502_opcode* opcode, char data )
{
	ir_table* ptr = emit_ir ();
	ptr->data[0] = opcode->op;
	ptr->data[1] = data&0xFF;
	ptr->len = 2;
	ptr->addr = cur->addr;
	cur->addr += ptr->len;
}

void emit_bin_relative_ref ( C6502_opcode* opcode, ir_table* ref )
{
	ir_table* ptr = emit_ir ();
	ptr->data[0] = opcode->op;
	ptr->data[1] = 0;
	ptr->len = 2;
	ptr->addr = cur->addr;
	ptr->ref = ref;
	cur->addr += ptr->len;
}

void emit_bin_indirect ( C6502_opcode* opcode, unsigned short data )
{
	ir_table* ptr = emit_ir ();
	ptr->data[0] = opcode->op;
	ptr->data[1] = data&0xFF;
	ptr->data[2] = (data>>8)&0xFF;
	ptr->len = 3;
	ptr->addr = cur->addr;
	cur->addr += ptr->len;
}

void emit_bin_indirect_ref ( C6502_opcode* opcode, ir_table* ref )
{
	ir_table* ptr = emit_ir ();
	ptr->data[0] = opcode->op;
	ptr->data[1] = 0;
	ptr->data[2] = 0;
	ptr->len = 3;
	ptr->addr = cur->addr;
	ptr->ref = ref;
	cur->addr += ptr->len;
}

void emit_bin_abs_idx_x ( C6502_opcode* opcode, unsigned short data )
{
	ir_table* ptr = emit_ir ();
	ptr->data[0] = opcode->op;
	ptr->data[1] = data&0xFF;
	ptr->data[2] = (data>>8)&0xFF;
	ptr->len = 3;
	ptr->addr = cur->addr;
	cur->addr += ptr->len;
}

void emit_bin_abs_idx_x_ref ( C6502_opcode* opcode, ir_table* ref )
{
	ir_table* ptr = emit_ir ();
	ptr->data[0] = opcode->op;
	ptr->data[1] = 0;
	ptr->data[2] = 0;
	ptr->len = 3;
	ptr->addr = cur->addr;
	ptr->ref = ref;
	cur->addr += ptr->len;
}

void emit_bin_abs_idx_y ( C6502_opcode* opcode, unsigned short data )
{
	ir_table* ptr = emit_ir ();
	ptr->data[0] = opcode->op;
	ptr->data[1] = data&0xFF;
	ptr->data[2] = (data>>8)&0xFF;
	ptr->len = 3;
	ptr->addr = cur->addr;
	cur->addr += ptr->len;
}

void emit_bin_abs_idx_y_ref ( C6502_opcode* opcode, ir_table* ref )
{
	ir_table* ptr = emit_ir ();
	ptr->data[0] = opcode->op;
	ptr->data[1] = 0;
	ptr->data[2] = 0;
	ptr->len = 3;
	ptr->addr = cur->addr;
	ptr->ref = ref;
	cur->addr += ptr->len;
}

void emit_bin_zp_idx_x ( C6502_opcode* opcode, unsigned char data )
{
	ir_table* ptr = emit_ir ();
	ptr->data[0] = opcode->op;
	ptr->data[1] = data;
	ptr->len = 2;
	ptr->addr = cur->addr;
	cur->addr += ptr->len;
}

void emit_bin_zp_idx_x_ref ( C6502_opcode* opcode, ir_table* ref )
{
	ir_table* ptr = emit_ir ();
	ptr->data[0] = opcode->op;
	ptr->data[1] = 0;
	ptr->len = 2;
	ptr->addr = cur->addr;
	ptr->ref = ref;
	cur->addr += ptr->len;
}

void emit_bin_zp_idx_y ( C6502_opcode* opcode, unsigned char data )
{
	ir_table* ptr = emit_ir ();
	ptr->data[0] = opcode->op;
	ptr->data[1] = data;
	ptr->len = 2;
	ptr->addr = cur->addr;
	cur->addr += ptr->len;
}

void emit_bin_zp_idx_y_ref ( C6502_opcode* opcode, ir_table* ref )
{
	ir_table* ptr = emit_ir ();
	ptr->data[0] = opcode->op;
	ptr->data[1] = 0;
	ptr->len = 2;
	ptr->addr = cur->addr;
	ptr->ref = ref;
	cur->addr += ptr->len;
}

void emit_bin_pre_idx_ind ( C6502_opcode* opcode, unsigned char data )
{
	ir_table* ptr = emit_ir ();
	ptr->data[0] = opcode->op;
	ptr->data[1] = data;
	ptr->len = 2;
	ptr->addr = cur->addr;
	cur->addr += ptr->len;
}

void emit_bin_pre_idx_ind_ref ( C6502_opcode* opcode, ir_table* ref )
{
	ir_table* ptr = emit_ir ();
	ptr->data[0] = opcode->op;
	ptr->data[1] = 0;
	ptr->len = 2;
	ptr->addr = cur->addr;
	ptr->ref = ref;
	cur->addr += ptr->len;
}

void emit_bin_post_idx_ind ( C6502_opcode* opcode, unsigned char data )
{
	ir_table* ptr = emit_ir ();
	ptr->data[0] = opcode->op;
	ptr->data[1] = data;
	ptr->len = 2;
	ptr->addr = cur->addr;
	cur->addr += ptr->len;
}

void emit_bin_post_idx_ind_ref ( C6502_opcode* opcode, ir_table* ref )
{
	ir_table* ptr = emit_ir ();
	ptr->data[0] = opcode->op;
	ptr->data[1] = 0;
	ptr->len = 2;
	ptr->addr = cur->addr;
	ptr->ref = ref;
	cur->addr += ptr->len;
}
