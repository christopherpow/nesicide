word    [_'@a-zA-Z][_\-'@a-zA-Z0-9]+
datab   ".db"
datab2  ".byte"
ascii   ".ascii"
dataw   ".dw"
dataw2  ".word"
dataw3  ".address"
origin  ".org"
space   ".space"
advance ".advance"
text    ".text"
segment ".segment"
data    ".data"
incbin  ".incbin"
incobj  ".incobj"
include ".include"
hexdigits "$"([0-9a-fA-F]{1,5})"h"?
decdigits ([0-9]{0,5})
octdigits "0"([0-9]{0,6})
bindigits "%"([01]{1,16})
label		"@"?{word}?":"([+]*|[-]*)
quotedstring [\"].*[\"]
comment  ";".*$
%{
#include "pasm_grm.tab.h"
#include "pasm_types.h"
#include "6502.h"

extern void add_error ( char* s );
extern char e [ 256 ];

#define STAB_ENT_INC 32

unsigned int stab_ent = 0;
unsigned int stab_max = 0;
int          yylineno = 1;

symbol_table* stab = NULL;

symbol_table* cltab [ 4096 ];
int cltab_ent = 0;

symbol_table* current_label = NULL;

extern binary_table* btab;
extern binary_table* cur;

int autolabel = 0x1000;

number_ref_compound_type com [ 4096 ];
ref_type ref [ 4096 ];
number_type num [ 4096 ];
char* text [ 4096 ];
int comidx = 0;
int numidx = 0;
int refidx = 0;
int textidx = 0;

unsigned char add_symbol ( char* symbol, symbol_table** ptr );
symbol_table* find_symbol ( char* symbol, int btab_ent );
void add_cheap_label ( symbol_table* ptr );
void clear_cheap_labels ( void );

number_type* get_next_numtype ( void )
{
	number_type* p = &(num[numidx]);
	numidx++;
	if ( numidx == (sizeof(num)/sizeof(num[0])) )
	{
		numidx = 0;
	}
	return p;
}

ref_type* get_next_reftype ( void )
{
	ref_type* p = &(ref[refidx]);
	refidx++;
	if ( refidx == (sizeof(ref)/sizeof(ref[0])) )
	{
		refidx = 0;
	}
	return p;
}

number_ref_compound_type* get_next_compoundtype ( void )
{
	number_ref_compound_type* p = &(com[comidx]);
	comidx++;
	if ( comidx == (sizeof(com)/sizeof(com[0])) )
	{
		comidx = 0;
	}
	return p;
}

void copy_compound_type ( number_ref_compound_type* d, number_ref_compound_type* s )
{
	memcpy ( d, s, sizeof(number_ref_compound_type) );
}

void combine_compound_types ( number_ref_compound_type* d, 
                              number_ref_compound_type* s1,
										number_ref_compound_type* s2  )
{
	memcpy ( d, s1, sizeof(number_ref_compound_type) );
}

unsigned int get_compound_value ( number_ref_compound_type* s )
{
	if ( s->type == number_only )
	{
	printf ( "num\n" );
		return s->num.number;
	}
	else if ( s->type == reference_only )
	{
	printf ( "ref\n" );
		return s->ref.ref.symtab->addr;
	}
	else
	{
	printf ( "com\n" );
		return s->ref.ref.symtab->addr + s->num.number;
	}
}

unsigned char valid_instr ( char* instr );
%}
%%
{label}	{ 
	char* label = strdup(yytext);
	unsigned char f;
	symbol_table* p = NULL;
	ref_type* ref;
	label[strlen(label)-1] = 0;
	if ( strlen(label) == 0 )
	{
		sprintf ( label, "L%05X", autolabel++ );
	}
	f=add_symbol(label,&p);
	if ( !f )
	{
		sprintf ( e, "multiple declarations of symbol: %s", label ); 
		add_error ( e );
		fprintf ( stderr, "error: %d: multiple declarations of symbol: %s\n", yylineno, label );
	}
	if ( label[0] == '@' )
	{
		add_cheap_label ( p ); 
	}
	else
	{
		clear_cheap_labels ();
	}
	current_label = p;
	ref = get_next_reftype ();
	ref->type = reference_symtab;
	ref->ref.symtab = p;
	yylval.ref = ref;
	return LABEL;
}
{word} { 
	unsigned char f;
	char* start = yytext;
	ref_type* ref;

	symbol_table* p = NULL;
	if ( (p=find_symbol(start,cur->idx)) != NULL )
	{
		ref = get_next_reftype ();

		if ( p->global )
		{
			ref->type = reference_global;
		}
		else
		{
			ref->type = reference_symtab;
		}

		ref->modifier = no_modifier;

		ref->ref.symtab = p;
		yylval.ref = ref;
		return LABELREF;
	}
	else if ( (f=valid_instr(yytext)) != INVALID_INSTR )
	{
		yylval.instr = f;
		return INSTR;
	}
	else if ( yytext[0] == '\'' &&
	          yytext[strlen(yytext)-1] == '\'' )
	{
		yylval.text = strdup ( yytext );
		return QUOTEDSTRING;
	}

	/* pass through as a possible forward-reference label */
	ref = get_next_reftype ();
	ref->type = reference_symbol;
	ref->ref.symbol = strdup(start);
	yylval.ref = ref;
	return LABELREF;
}
{datab} |
{datab2} |
{ascii} { 
   return DATAB; 
}
{dataw} |
{dataw2} |
{dataw3} { 
   return DATAW; 
}
{origin} { 
   return ORIGN;
}
{space} { 
   return SPACE;
}
{advance} { 
   return ADVANCE;
}
{text} |
{segment} {
	yylval.seg = text_segment;
	return SEGMENT;
}
{data} {
	yylval.seg = data_segment;
	return SEGMENT;
}
{incbin} {
	return INCBIN;
}
{incobj} {
	return INCOBJ;
}
{include} {
	return INCLUDE;
}
{quotedstring} {
	yylval.text = strdup ( yytext );
	return QUOTEDSTRING;
}
{hexdigits} { 
	number_type* num = get_next_numtype ();
	num->number = strtoul(yytext+1,NULL,16);
	num->zp_ok = 0;
	if ( (num->number >= -128) &&
	     (num->number < 256) )
	{
		num->zp_ok = 1;
	}
	yylval.num = num;
   return DIGITS; 
}
{decdigits} { 
	number_type* num = get_next_numtype ();
	num->number = strtoul(yytext,NULL,10);
	num->zp_ok = 0;
	if ( (num->number >= -128) &&
		  (num->number < 256) )
	{
		num->zp_ok = 1;
	}
	yylval.num = num;
	return DIGITS; 
}
{octdigits} { 
	number_type* num = get_next_numtype ();
	num->number = strtoul(yytext+1,NULL,8);
	num->zp_ok = 0;
	if ( (num->number >= -128) &&
	     (num->number < 256) )
	{
		num->zp_ok = 1;
	}
	yylval.num = num;
   return DIGITS; 
}
{bindigits} { 
	number_type* num = get_next_numtype ();
	num->number = strtoul(yytext+1,NULL,2);
	num->zp_ok = 0;
	if ( (num->number >= -128) &&
	     (num->number < 256) )
	{
		num->zp_ok = 1;
	}
	yylval.num = num;
   return DIGITS; 
}
{comment} { 
   /* throw away */
}
[ \t]* { 
   /* throw away */ 
}
\r\n {
   yylineno++;
	return TERM;
}
\n\r {
   yylineno++;
	return TERM;
}
\n {
   yylineno++;
	return TERM;
}

[@#=,+\-\*\/\(\)\|&\~\<\>\[\]] { return yytext[0]; }

[xyXY] { return tolower(yytext[0]); }

. { 
	sprintf ( e, "parse error in input at: %s", yytext );
	add_error ( e );
	fprintf ( stderr, "error: %d: parse error in input\n", yylineno );
}
%%
int get_current_buffer ( void )
{
	return (int)yy_current_buffer;
}

symbol_table* find_symbol ( char* symbol, int bank )
{
	unsigned int i;
	symbol_table* ptr = NULL;

	// check cheap labels if it is a cheap label!
	if ( symbol[0] == '@' )
	{
		for ( i = 0; i < cltab_ent; i++ )
		{
			if ( (strcmp(cltab[i]->symbol,symbol) == 0) &&
				  ((cltab[i]->btab_ent == bank) || (cltab[i]->global) ||
				  (((btab[bank].type == text_segment) && (btab[cltab[i]->btab_ent].type == data_segment)) || 
				  ((btab[bank].type == data_segment) && (btab[cltab[i]->btab_ent].type == text_segment)))) )
			{
				ptr = cltab[i];
			}
		}
	}
	else
	{
		// otherwise, search symbol table entirely...
		for ( i = 0; i < stab_ent; i++ )
		{
			if ( (strcmp(stab[i].symbol,symbol) == 0) &&
				  ((stab[i].btab_ent == bank) || (stab[i].global) ||
				  (((btab[bank].type == text_segment) && (btab[stab[i].btab_ent].type == data_segment)) || 
				  ((btab[bank].type == data_segment) && (btab[stab[i].btab_ent].type == text_segment)))) )
			{
				ptr = &(stab[i]);
			}
		}
	}
			
	return ptr;
}

void add_cheap_label ( symbol_table* ptr )
{
	int idx;

	cltab [ cltab_ent ] = ptr;
	cltab_ent++;
	if ( cltab_ent == 4096 )
	{
		sprintf ( e, "too many cheap labels (@label:)" );
		add_error ( e );
		fprintf ( stderr, "error: too many cheap labels (@label:)\n" );
	}

	for ( idx = 0; idx < cltab_ent; idx++ )
	{
		printf ( "cheap: %s\n", cltab[idx]->symbol );
	}
}

void clear_cheap_labels ( void )
{
	cltab_ent = 0;
//	printf ( "cheap cleared!\n" );
}

unsigned char add_symbol ( char* symbol, symbol_table** ptr )
{
	unsigned char a = 1;
	unsigned int i;

	*ptr = NULL;

	if ( symbol[0] != '@' )
	{
		for ( i = 0; i < stab_ent; i++ )
		{
			if ( (strcmp(stab[i].symbol,symbol) == 0) && 
				  ((stab[i].btab_ent == cur->idx) ||
				  (btab[stab[i].btab_ent].type == data_segment)) )
			{
				a = 0;
				*ptr = &(stab[i]);
				return a;
			}
		}
	}
		
	if ( stab == NULL )
	{
		stab = (symbol_table*)calloc ( STAB_ENT_INC, sizeof(symbol_table) );
		if ( stab != NULL )
		{
			stab_max += STAB_ENT_INC;
			stab[stab_ent].symbol = (char*)malloc ( strlen(symbol)+1 );
			if ( stab[stab_ent].symbol != NULL )
			{
				memset ( stab[stab_ent].symbol, 0, strlen(symbol)+1 );
				strncpy ( stab[stab_ent].symbol, symbol, strlen(symbol) );
				stab[stab_ent].addr = 0x0000;
				stab[stab_ent].ir = NULL;
				stab[stab_ent].btab_ent = cur->idx;
				stab[stab_ent].global = 0;
			}
		}
		else
		{
			sprintf ( e, "unable to allocate memory for symbols" );
			add_error ( e );
			fprintf ( stderr, "error: unable to allocate memory for symbols!\n" );
		}
	}
	else
	{
		if ( stab_ent < stab_max )
		{
			stab[stab_ent].symbol = (char*)malloc ( strlen(symbol)+1 );
			if ( stab[stab_ent].symbol != NULL )
			{
				memset ( stab[stab_ent].symbol, 0, strlen(symbol)+1 );
				strncpy ( stab[stab_ent].symbol, symbol, strlen(symbol) );
				stab[stab_ent].addr = 0x0000;
				stab[stab_ent].ir = NULL;
				stab[stab_ent].btab_ent = cur->idx;
				stab[stab_ent].global = 0;
			}
		}
		else
		{
			stab_max += STAB_ENT_INC;
			stab = (symbol_table*) realloc ( stab, stab_max*sizeof(symbol_table) );
			if ( stab != NULL )
			{
				stab[stab_ent].symbol = (char*)malloc ( strlen(symbol)+1 );
				if ( stab[stab_ent].symbol != NULL )
				{
					memset ( stab[stab_ent].symbol, 0, strlen(symbol)+1 );
					strncpy ( stab[stab_ent].symbol, symbol, strlen(symbol) );
					stab[stab_ent].addr = 0x0000;
					stab[stab_ent].ir = NULL;
					stab[stab_ent].btab_ent = cur->idx;
					stab[stab_ent].global = 0;
				}
			}
			else
			{
				sprintf ( e, "unable to allocate memory for symbols" );
				add_error ( e );
				fprintf ( stderr, "error: unable to allocate memory for symbols!\n" );
			}
		}
	}

	*ptr = &(stab[stab_ent]);
	stab_ent++;

	return a;
}

unsigned char valid_instr ( char* instr )
{
	unsigned char a = INVALID_INSTR;
	char     upinstr [ 4 ];
	int i;

	for ( i = 0; i < 3; i++ )
	{
		upinstr[i] = toupper(instr[i]);
	}
	upinstr[3] = 0;

	if ( strlen(instr) == 3 )
	{
		for ( i = 0; i < 256; i++ )
		{
			if ( (strlen(m_6502opcode[i].name) > 0) && (strcmp(m_6502opcode[i].name,upinstr) == 0) )
			{
				a = i;
				return a;
			}
		}
	}

	return a;
}
