identifier   ([_'.@a-zA-Z][_'.@a-zA-Z0-9]*)":"?
hexdigits    (("$"([0-9a-fA-F]{1,8}))|(([0-9a-fA-F]{1,8})"h"))
decdigits    ([1-9]([0-9]{0,8}))
octdigits    "0"([0-9]{0,8})
bindigits    (("%"([01]{1,16}))|(([01]{1,16})"b"))
quotedstring ([\"][^\"]*[\"])|(['][^']*['])
comment      ";".*$
%x HEX
%{
#include "pasm_grm.tab.h"
#include "pasm_types.h"
#include "string.h"

// For line counting...
#ifndef __APPLE__ //strange mac os x related bug? :S
int          yylineno = 0;
#endif

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
unsigned char add_symbol ( char* symbol, symbol_table** ptr );
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

// For automatic labels like "-", "++", etc.  This isn't done
// yet but I',m thinking it'd be easier to give them a name and
// use that name.  If the name is numeric, then, hopefully,
// the count of the number of +'s or -'s will be able to be
// subtracted from the last-emitted autolabel number and thus
// the address of the appropriate autolabel can be found by number.
int autolabel = 0x1000;

// This is a flag used to indicate whether or not we are looking
// at the first token on a new line in the lexer.  If that is
// the case, we use that knowledge to reduce the amount of
// checking done on the input token.  If, as is most likely the case,
// the input token matches the 'identifier' regular expression,
// we ONLY want to consider whether this first token is a directive,
// an assembler mnemonic, or a new label declaration.  This helps
// the parser, too.
int symbol_on_line = 0;

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
{identifier} {
	unsigned char f;
	char* start = yytext;
	ref_type* ref;
   text_type* text;
   keyword* k = keywords;

   // Don't need to check for labels if this isn't the first
   // symbol found on a new line, because labels must be first.
   symbol_on_line++;

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
         return k->token;
      }
      k++;
   }

   // Check for label if this is the first symbol on a new line...
   if ( symbol_on_line == 1 )
   {
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
         current_label = p->idx;
         ref = get_next_reftype ();
         ref->type = reference_symtab;
         ref->ref.stab_ent = p->idx;
         yylval.ref = ref;
         return LABEL;
      }
   }
   else
   {
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
         else if ( yytext[0] == '$' )
         {
            return yytext[0];
         }
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
      ref->ref.symbol = strdup(start);
      yylval.ref = ref;
      return LABELREF;
   }
}

{quotedstring} {
   text_type* text = get_next_texttype ();
   text->string = strdup ( yytext+1 );
   text->string[strlen(text->string)-1] = 0;
   // need to pass length so string mods made by expressions don't lose track of it if a
   // byte before the end of the string becomes 0.  for example, "AB"-"A" makes the
   // first byte zero and strlen() then returns 0 which is invalid.
   text->length = strlen(text->string);
   yylval.text = text;
   return QUOTEDSTRING;
}

<HEX>[0-9a-fA-F]+ {
   text_type* text = get_next_texttype ();
   text->string = strdup ( yytext );
   text->length = strlen(text->string);
   yylval.text = text;
   return QUOTEDSTRING;
}

{hexdigits} {
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
   num->number = strtoul(yytext,NULL,8);
	if ( (num->number >= -128) &&
	     (num->number < 256) )
	{
		num->zp_ok = 1;
	}
	yylval.num = num;
   return DIGITS;
}

{bindigits} { 
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

<INITIAL,HEX>{comment} {
   /* throw away */
}

<INITIAL,HEX>[ \t]+ {
   /* throw away */
}

<INITIAL,HEX>\r\n {
   yylineno++;
   symbol_on_line = 0;
   BEGIN INITIAL;
   return TERM;
}

<INITIAL,HEX>\n\r {
   yylineno++;
   symbol_on_line = 0;
   BEGIN INITIAL;
   return TERM;
}

<INITIAL,HEX>\n {
   yylineno++;
   symbol_on_line = 0;
   BEGIN INITIAL;
   return TERM;
}

"<<" { return LSHIFT; }

">>" { return RSHIFT; }

"&&" { return LOGAND; }

"||" { return LOGOR; }

"<=" { return LTEQ; }

">=" { return GTEQ; }

"==" { return EQEQ; }

"!=" |
"<>" { return NOTEQ; }

[&|!@#=%\+\-\*\/\|&\~\<\>\[\],\(\)\$\^] { return yytext[0]; }

<INITIAL,HEX>. {
   sprintf ( e, "parse error at: %s", yytext );
	add_error ( e );
   fprintf ( stderr, "error: %d: parse error at: %s\n", yylineno, yytext );
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
		fprintf ( stderr, "error: too many cheap labels (@label:)\n" );
	}
}

void clear_cheap_labels ( void )
{
   int label;
   for ( label = 0; label < cltab_ent; label++ )
   {
      // cheap hack to wipe away a temporary symbol...
      strcpy ( cltab[label]->symbol, "{;UNUSED;}" );
   }
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
