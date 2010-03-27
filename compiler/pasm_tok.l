%{

// This is the lexical analyzer for the 6502 assembler built
// into NESICIDE, an Integrated Development Environment for
// the 8-bit Nintendo Entertainment System.

// This section declares the complex regular expressions used
// in token matching rules below.

// Lexer states (%x below) are exclusive, meaning when the lexer is in
// a state it will not attempt to match a token against
// a rule not explicitly identified as being a member of that state's
// token matching ruleset.
// For example:
// The HEX state is used to indicate to the lexer that it should
// switch its interpretation of a hexadecimal number between the
// normal $xxxx (where x is 0-9a-f) to the more compact xxxxxxxx
// where the hex string can be any number of hex digits long.
// Lexer states are very useful in delineating how the lexer
// represents a token stream to the parser.  They, in effect, impose
// some of the grammar rules that the parser would otherwise have
// a hard time disambiguating on the lexer.

%}

autolabel1     (([\-]+)|([\+]+))
autolabel2_    (([\-]([\-]+))|([\+]([\+]+)))
identifier1    ("$"|{autolabel1}|([_'.@a-zA-Z][_'.@a-zA-Z0-9]*)|{autolabel1}?([_'.@a-zA-Z][_'.@a-zA-Z0-9]*))":"?
identifier2    ("$"|{autolabel2_}|([_'.@a-zA-Z][_'.@a-zA-Z0-9]*)|{autolabel1}?([_'.@a-zA-Z][_'.@a-zA-Z0-9]*))":"?
hexdigits      (("$"([0-9a-fA-F]{1,8}))|(([0-9a-fA-F]{1,8})"h"))
decdigits      ([1-9]([0-9]{0,8}))
octdigits      "0"([0-9]{0,8})
bindigits      (("%"([01]{1,16}))|(([01]{1,16})"b"))
quotedstring   ([\"][^\"]*[\"])|(['][^']*['])
comment        ";".*$
rawhex         ([0-9a-fA-F]+)
whitespace     ([ \t]+)

%x HEX SOURCE NOLABELREF

%{

// Information encased in %{ ... }% is copied directly into the
// lexical output C source file, so anything inside here is
// valid C syntax.  Outside of the C fence, only lex syntax is
// expected.

#include "pasm_grm.tab.h"
#include "pasm_types.h"
#include "string.h"

// For line counting...
#ifndef __APPLE__ //strange mac os x related bug? :S
int          yylineno = 0;
#endif

// Previous lexer state for returning
int previous_state = INITIAL;

// Interface to add assembler errors to output
extern void add_error ( char* s );
extern char e [ 256 ];

// Keyword information.  Since keywords are parsed by the
// 'identifier' rule along with labels, forward-use variables,
// and even assembly mnemonics, we need a mechanism to search
// for keywords and handle them.  This is similar to the mechanism
// used to determine an assembler mnemonic in the 'identifier'
// regular expression.
typedef struct _keyword
{
   const char* directive;
   const char* dotdirective;
   int         token;
   void (*handler)();
} keyword;

// The .hex directive has a special lexer sub-state associated
// with it that changes how the lexer parses hex strings.  This
// function is executed when the .hex directive is encountered
// in the token stream.  It switches the lexer into the HEX
// sub-state.
void d_hex ( void )
{
   BEGIN HEX;
}

// The .segment, .text, and .data directives send the same
// SEGMENT token to the parser so they need to indicate which
// one they are by sending data along with the token.  This
// function is executed when the .segment or .text directive
// is encountered in the token stream.
void d_textsegment ( void )
{
   yylval.seg = text_segment;
}

// The .segment, .text, and .data directives send the same
// SEGMENT token to the parser so they need to indicate which
// one they are by sending data along with the token.  This
// function is executed when the .data directive
// is encountered in the token stream.
void d_datasegment ( void )
{
   yylval.seg = data_segment;
}

// Table of assembler keywords.  These should look a bit like ASM6.
keyword keywords [] =
{
   { "byte", ".byte", DATAB, NULL },
   { "db", ".db", DATAB, NULL },
   { "dcb", ".dcb", DATAB, NULL },
   { "dc.b", ".dc.b", DATAB, NULL },
   { "ascii", ".ascii", DATAB, NULL },
   { "hex", ".hex", DATABHEX, d_hex },
   { "word", ".word", DATAW, NULL },
   { "dw", ".dw", DATAW, NULL },
   { "dcw", ".dcw", DATAW, NULL },
   { "dc.w", ".dc.w", DATAW, NULL },
   { "address", ".address", DATAW, NULL },
   { "dsb", ".dsb", FILLSPACEB, NULL },
   { "ds.b", ".ds.b", FILLSPACEB, NULL },
   { "dsw", ".dsw", FILLSPACEW, NULL },
   { "ds.w", ".ds.w", FILLSPACEW, NULL },
   { "org", ".org", ORIGIN, NULL },
   { "base", ".base", BASE, NULL },
   { "space", ".space", VARSPACE, NULL },
   { "advance", ".advance", ADVANCE, NULL },
   { "pad", ".pad", ADVANCE, NULL },
   { "align", ".align", ALIGN, NULL },
   { "text", ".text", SEGMENT, d_textsegment },
   { "segment", ".segment", SEGMENT, d_textsegment },
   { "data", ".data", SEGMENT, d_datasegment },
   { "incbin", ".incbin", INCBIN, NULL },
   { "bin", ".bin", INCBIN, NULL },
   { "incobj", ".incobj", INCOBJ, NULL },
   { "include", ".include", INCLUDE, NULL },
   { "incsrc", ".incsrc", INCLUDE, NULL },
   { "rept", ".rept", REPEAT, NULL },
   { "endr", ".endr", ENDREPEAT, NULL },
   { "enum", ".enum", ENUMERATE, NULL },
   { "ende", ".ende", ENDENUMERATE, NULL },
   { "macro", ".macro", MACRO, NULL },
   { "endm", ".endm", ENDMACRO, NULL },
   { "fillvalue", ".fillvalue", FILLVALUE, NULL },
   { "ifdef", ".ifdef", IFDEF, NULL },
   { "ifndef", ".ifndef", IFNDEF, NULL },
   { "if", ".if", IF, NULL },
   { "elseif", ".elseif", ELSEIF, NULL },
   { "else", ".else", ELSE, NULL },
   { "endif", ".endif", ENDIF, NULL },
   { NULL, NULL, 0, NULL }
};

// To reduce memory thrashing, don't allocate a new
// page of symbols until 32 symbols have been declared.
#define STAB_ENT_INC 32

// Symbol table management.  The symbol table is available
// to both the lexer and parser.
unsigned int stab_ent = 0;
unsigned int stab_max = 0;
symbol_table* stab = NULL;
unsigned char add_symbol ( symbol_table* stab, char* symbol, symbol_table** ptr );
symbol_table* find_symbol ( char* symbol, int btab_ent );

// Cheap-label symbol table. Cheap labels exist only until
// the next normal label is declared.  For example:
// label1: sei
//         cld
// @1:     lda $2002
//         bpl @1
// @2:     lda $2002
//         bpl @2
// label2: <-- @1 and @2 are destroyed here.
symbol_table* cltab [ 4096 ];
int cltab_ent = 0;
void add_cheap_label ( symbol_table* ptr );
void clear_cheap_labels ( void );

// Index of last declared label, for useful error messages like:
// error: <line>: after <symbol>: blah...
int current_label = -1;

// Binary bank management.  Each time a .segment, or .text or
// .data directive declares a uniquely named bank, a bank table
// entry is created.  While in a particular bank the current
// PC within that bank is tracked.  Switching between banks by
// issuing repeat .text, .segment, or .data directives will pick
// up in the assembly stream at the PC where it was last left.
extern binary_table* btab;
extern binary_table* cur;

// Routines to allocate new structures to pass to the parser.
// The parser makes use of these also as it is reducing and
// shifting things around.
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
text_type* get_next_texttype ( void )
{
   text_type* p = (text_type*) malloc ( sizeof(text_type) );
   p->string = NULL;
   p->length = 0;
   return p;
}

// This function checks for valid assembler mnemonics in
// tokens.
unsigned char valid_instr ( char* instr );
%}
%%
<INITIAL>{identifier1} {
	unsigned char f;
	ref_type* ref;
   text_type* text;
   keyword* k = keywords;

   // Assume that when we're done with this token we'll be processing a line of
   // assembly source code...
   BEGIN SOURCE;

   while ( k->directive != NULL )
   {
      if ( ((strlen(k->directive) == strlen(yytext)) &&
           (strnicmp(yytext,k->directive,strlen(k->directive)) == 0)) ||
           ((strlen(k->dotdirective) == strlen(yytext)) &&
           (strnicmp(yytext,k->dotdirective,strlen(k->dotdirective)) == 0)) )
      {
         if ( k->handler )
         {
            k->handler ();
         }
         yylval.directive = strdup ( k->directive );
         return k->token;
      }
      k++;
   }

   // If it's a valid instruction it's not a label...
   if ( (f=valid_instr(yytext)) != INVALID_INSTR )
   {
      yylval.instr = f;
      return INSTR;
   }
   else
   {
      char* label = strdup(yytext);
      unsigned char f;
      symbol_table* p = NULL;
      ref_type* ref;
      if ( label[strlen(label)-1] == ':' )
      {
         label[strlen(label)-1] = 0;
      }
      f=add_symbol(stab,label,&p);
      if ( !f )
      {
         if ( (label[0] == '-') ||
              (label[0] == '+') )
         {
            // re-declaration of autolabels is allowed...
            // set this symbol as invalid...
            p->symbol[0] = SYMBOL_KILLER;

            // this time it should add...
            f=add_symbol(stab,label,&p);
         }
         else
         {
            sprintf ( e, "multiple declarations of symbol: %s", label );
            add_error ( e );
         }
      }
      if ( label[0] == '@' )
      {
         add_cheap_label ( p );
      }
      else
      {
         clear_cheap_labels ();
      }
      current_label = p->idx;
      ref = get_next_reftype ();
      ref->type = reference_symtab;
      ref->ref.stab_ent = p->idx;
      yylval.ref = ref;
      return LABEL;
   }
}

<SOURCE>{identifier2} {
   unsigned char f;
   ref_type* ref;
   text_type* text;
   keyword* k = keywords;

   while ( k->directive != NULL )
   {
      if ( ((strlen(k->directive) == strlen(yytext)) &&
           (strnicmp(yytext,k->directive,strlen(k->directive)) == 0)) ||
           ((strlen(k->dotdirective) == strlen(yytext)) &&
           (strnicmp(yytext,k->dotdirective,strlen(k->dotdirective)) == 0)) )
      {
         if ( k->handler )
         {
            k->handler ();
         }
         yylval.directive = strdup ( k->directive );
         return k->token;
      }
      k++;
   }

   if ( yytext[1] == 0 )
   {
      if ( (yytext[0] == 'x') ||
           (yytext[0] == 'X') )
      {
         return tolower(yytext[0]);
      }
      else if ( (yytext[0] == 'y') ||
                (yytext[0] == 'Y') )
      {
         return tolower(yytext[0]);
      }
   }

   symbol_table* p = NULL;
   if ( (p=find_symbol(yytext,cur->idx)) != NULL )
   {
      ref = get_next_reftype ();

      ref->type = reference_symtab;
      ref->ref.stab_ent = p->idx;
      yylval.ref = ref;
      previous_state = SOURCE;
      BEGIN NOLABELREF;
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
      text = get_next_texttype ();
      text->string = strdup ( yytext+1 );
      text->string[strlen(text->string)-1] = 0;
      // need to pass length so string mods made by expressions don't lose track of it if a
      // byte before the end of the string becomes 0.  for example, "AB"-"A" makes the
      // first byte zero and strlen() then returns 0 which is invalid.
      text->length = strlen(text->string);
      yylval.text = text;
      return QUOTEDSTRING;
   }

   /* pass through as a possible forward-reference label */
   ref = get_next_reftype ();
   ref->type = reference_symbol;
   ref->ref.symbol = strdup(yytext);
   yylval.ref = ref;
   previous_state = SOURCE;
   BEGIN NOLABELREF;
   return LABELREF;
}

<SOURCE,NOLABELREF>{quotedstring} {
   text_type* text = get_next_texttype ();
   text->string = strdup ( yytext+1 );
   text->string[strlen(text->string)-1] = 0;
   // need to pass length so string mods made by expressions don't lose track of it if a
   // byte before the end of the string becomes 0.  for example, "AB"-"A" makes the
   // first byte zero and strlen() then returns 0 which is invalid.
   text->length = strlen(text->string);
   yylval.text = text;
   previous_state = yy_current_state;
   return QUOTEDSTRING;
}

<HEX>{rawhex} {
   text_type* text = get_next_texttype ();
   text->string = strdup ( yytext );
   text->length = strlen(text->string);
   yylval.text = text;
   return QUOTEDSTRING;
}

<SOURCE>{hexdigits} {
   char* ptr = yytext;
	number_type* num = get_next_numtype ();
   if ( ptr[0] == '$' ) ptr++;
   num->number = strtoul(ptr,NULL,16);
	if ( (num->number >= -128) &&
	     (num->number < 256) )
	{
		num->zp_ok = 1;
	}
	yylval.num = num;
   return DIGITS;
}

<SOURCE>{decdigits} {
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

<SOURCE>{octdigits} {
	number_type* num = get_next_numtype ();
   num->number = strtoul(yytext,NULL,8);
	if ( (num->number >= -128) &&
	     (num->number < 256) )
	{
		num->zp_ok = 1;
	}
	yylval.num = num;
   return DIGITS;
}

<SOURCE>{bindigits} {
   char* ptr = yytext;
   number_type* num = get_next_numtype ();
   if ( ptr[0] == '%' ) ptr++;

   num->number = strtoul(ptr,NULL,2);
   if ( (num->number >= -128) &&
        (num->number < 256) )
   {
      num->zp_ok = 1;
   }
   yylval.num = num;
   return DIGITS;
}

<INITIAL,SOURCE,HEX,NOLABELREF>{comment} {
   BEGIN INITIAL;
   /* throw away */
}

<INITIAL,SOURCE,HEX>{whitespace} {
   /* throw away */
}

<NOLABELREF>{whitespace} {
   BEGIN previous_state;
   /* throw away */
}

<INITIAL,SOURCE,HEX,NOLABELREF>\r\n {
   yylineno++;
   BEGIN INITIAL;
   return TERM;
}

<INITIAL,SOURCE,HEX,NOLABELREF>\n\r {
   yylineno++;
   BEGIN INITIAL;
   return TERM;
}

<INITIAL,SOURCE,HEX,NOLABELREF>\n {
   yylineno++;
   BEGIN INITIAL;
   return TERM;
}

<SOURCE>"<<" {
   return LSHIFT;
}

<NOLABELREF>"<<" {
   BEGIN previous_state;
   return LSHIFT;
}

<SOURCE>">>" {
   return RSHIFT;
}

<NOLABELREF>">>" {
   BEGIN previous_state;
   return RSHIFT;
}

<SOURCE>"&&" {
   return LOGAND;
}

<NOLABELREF>"&&" {
   BEGIN previous_state;
   return LOGAND;
}

<SOURCE>"||" {
   return LOGOR;
}

<NOLABELREF>"||" {
   BEGIN previous_state;
   return LOGOR;
}

<SOURCE>"<=" {
   return LTEQ;
}

<NOLABELREF>"<=" {
   BEGIN previous_state;
   return LTEQ;
}

<SOURCE>">=" {
   return GTEQ;
}

<NOLABELREF>">=" {
   BEGIN previous_state;
   return GTEQ;
}

<SOURCE>"==" {
   return EQEQ;
}

<NOLABELREF>"==" {
   BEGIN previous_state;
   return EQEQ;
}

<SOURCE>"!=" |
<SOURCE>"<>" {
   return NOTEQ;
}

<NOLABELREF>"!=" |
<NOLABELREF>"<>" {
   BEGIN previous_state;
   return NOTEQ;
}

<SOURCE>[&|!@#=%\+\-\*\/\|&\~\<\>\[\],\(\)\^] {
   return yytext[0];
}

<NOLABELREF>[&|!@#=%\+\-\*\/\|&\~\<\>\[\],\(\)\^] {
   BEGIN previous_state;
   return yytext[0];
}

<INITIAL,SOURCE,HEX,NOLABELREF>. {
   // ignore, error should already be given
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
            break;
			}
		}
	}
	else
	{
		// otherwise, search symbol table entirely...
		for ( i = 0; i < stab_ent; i++ )
		{
         if ( (strlen(symbol) == strlen(stab[i].symbol)) &&
              (strcmp(symbol,stab[i].symbol) == 0) &&
              ((stab[i].btab_ent == bank) || (stab[i].expr) ||
				  (((btab[bank].type == text_segment) && (btab[stab[i].btab_ent].type == data_segment)) || 
				  ((btab[bank].type == data_segment) && (btab[stab[i].btab_ent].type == text_segment)))) )
			{
				ptr = &(stab[i]);
            break;
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
	}
}

void clear_cheap_labels ( void )
{
   int label;
   for ( label = 0; label < cltab_ent; label++ )
   {
      // cheap hack to wipe away a temporary symbol...
      cltab[label]->symbol[0] = SYMBOL_KILLER;
   }
	cltab_ent = 0;
}

unsigned char add_symbol ( symbol_table* stab, char* symbol, symbol_table** ptr )
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
			}
		}
	}

	*ptr = &(stab[stab_ent]);
	stab_ent++;

	return a;
}
