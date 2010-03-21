word    [_'@a-zA-Z][_'@a-zA-Z0-9]*
datab1  ".byte"
datab2  ".db"
datab3  ".dcb"
datab4  ".dc.b"
ascii   ".ascii"
dataw1  ".word"
dataw2  ".dw"
dataw3  ".dcw"
dataw4  ".dc.w"
dataw5  ".address"
dsb     ".dsb"
dsw     ".dsw"
origin  ".org"
space   ".space"
advance ".advance"
pad     ".pad"
align   ".align"
text    ".text"
segment ".segment"
data    ".data"
incbin  ".incbin"
incobj  ".incobj"
include ".include"
repeat  ".rept"
endrepeat ".endr"
enum    ".enum"
endenum ".ende"
hexdigits "$"([0-9a-fA-F]{1,5})"h"?
decdigits ([0-9]{0,5})
octdigits "0"([0-9]{0,6})
bindigits "%"([01]{1,16})
label    {word}?":"([+]*|[-]*)
quotedstring [\"].*[\"]
comment  ";".*$
%s ENUMSTATE
%{
#include "pasm_grm.tab.h"
#include "pasm_types.h"

extern void add_error ( char* s );
extern char e [ 256 ];

#define STAB_ENT_INC 32

unsigned int stab_ent = 0;
unsigned int stab_max = 0;
#ifndef __APPLE__ //strange mac os x related bug? :S
int          yylineno = 0;
#endif
int parenlevel = 0;

symbol_table* stab = NULL;

symbol_table* cltab [ 4096 ];
int cltab_ent = 0;

symbol_table* current_label = NULL;

extern binary_table* btab;
extern binary_table* cur;

int autolabel = 0x1000;

char* text [ 4096 ];
int textidx = 0;

unsigned char add_symbol ( char* symbol, symbol_table** ptr );
symbol_table* find_symbol ( char* symbol, int btab_ent );
void add_cheap_label ( symbol_table* ptr );
void clear_cheap_labels ( void );

number_type* get_next_numtype ( void )
{
   number_type* p = (number_type*) malloc ( sizeof(number_type) );
   p->zp_ok = 0;
   return p;
}

ref_type* get_next_reftype ( void )
{
   ref_type* p = (ref_type*) malloc ( sizeof(ref_type) );
   return p;
}

expr_type* get_next_exprtype ( void )
{
   expr_type* p = (expr_type*) malloc ( sizeof(expr_type) );
   p->parent = NULL;
   p->left = NULL;
   p->right = NULL;
   return p;
}

unsigned char valid_instr ( char* instr );
unsigned char addressing_mode ( unsigned char instr );
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
   ref->ref.stab_ent = p->idx;
	yylval.ref = ref;
	return LABEL;
}
{word} { 
	unsigned char f;
	char* start = yytext;
	ref_type* ref;

   if ( (yytext[1] == 0) &&
        ((yytext[0] == 'x') ||
        (yytext[0] == 'X')) )
   {
      return tolower(yytext[0]);
   }
   else if ( (yytext[1] == 0) &&
             ((yytext[0] == 'y') ||
             (yytext[0] == 'Y')) )
   {
	   return tolower(yytext[0]);
   }

	symbol_table* p = NULL;
	if ( (p=find_symbol(start,cur->idx)) != NULL )
	{
		ref = get_next_reftype ();

      ref->type = reference_symtab;
      ref->ref.stab_ent = p->idx;
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
{datab1} |
{datab2} |
{datab3} |
{datab4} |
{ascii} {
   return DATAB; 
}
{dataw1} |
{dataw2} |
{dataw3} |
{dataw4} |
{dataw5} {
   return DATAW; 
}
{origin} { 
   return ORIGIN;
}
{dsb}  {
   return FILLSPACEB;
}
{dsw}  {
   return FILLSPACEW;
}
{space} { 
   return VARSPACE;
}
{advance} |
{pad} {
   return ADVANCE;
}
{align} {
   return ALIGN;
}
{repeat} {
   return REPEAT;
}
{endrepeat} {
   return ENDREPEAT;
}
{enum} {
//   BEGIN(ENUMSTATE);
   return ENUMERATE;
}
{endenum} {
//   BEGIN(INITIAL);
   return ENDENUMERATE;
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
[ \t]+ {
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

[@#=+\-\*\/\|&\~\<\>\[\]] { return yytext[0]; }

[\(] { parenlevel++; return yytext[0]; }
[\)] { parenlevel--; return yytext[0]; }
[,] { return yytext[0]; }

. { 
	sprintf ( e, "parse error in input at: %s", yytext );
	add_error ( e );
	fprintf ( stderr, "error: %d: parse error in input\n", yylineno );
}
%%
int get_current_buffer ( void )
{
	return (int)YY_CURRENT_BUFFER;
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
              ((cltab[i]->btab_ent == bank) || (cltab[i]->expr) ||
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
         if ( (strncmp(symbol,stab[i].symbol,strlen(symbol)) == 0) &&
              ((stab[i].btab_ent == bank) || (stab[i].expr) ||
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
}

void clear_cheap_labels ( void )
{
	cltab_ent = 0;
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
            stab[stab_ent].idx = stab_ent;
				memset ( stab[stab_ent].symbol, 0, strlen(symbol)+1 );
				strncpy ( stab[stab_ent].symbol, symbol, strlen(symbol) );
				stab[stab_ent].ir = NULL;
				stab[stab_ent].btab_ent = cur->idx;
            stab[stab_ent].expr = NULL;
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
            stab[stab_ent].idx = stab_ent;
            memset ( stab[stab_ent].symbol, 0, strlen(symbol)+1 );
				strncpy ( stab[stab_ent].symbol, symbol, strlen(symbol) );
				stab[stab_ent].ir = NULL;
				stab[stab_ent].btab_ent = cur->idx;
            stab[stab_ent].expr = NULL;
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
               stab[stab_ent].idx = stab_ent;
               memset ( stab[stab_ent].symbol, 0, strlen(symbol)+1 );
					strncpy ( stab[stab_ent].symbol, symbol, strlen(symbol) );
					stab[stab_ent].ir = NULL;
					stab[stab_ent].btab_ent = cur->idx;
               stab[stab_ent].expr = NULL;
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
