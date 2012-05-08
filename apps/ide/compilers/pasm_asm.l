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
hexdigits      ((0[xX]([0-9a-fA-F]{1,8}))|("$"([0-9a-fA-F]{1,8}))|(([0-9a-fA-F]{1,8})"h"))
decdigits      ([1-9]([0-9]{0,8}))
octdigits      "0"([0-9]{0,8})
bindigits      (("%"([01]{1,16}))|(([01]{1,16})"b"))
quotedstring   ([\"][^\"]*[\"])|(['][^']*['])
rawhex         ([0-9a-fA-F]+)
whitespace     ([ \t]+)
linenum        ^"!line"
filename       ^"!file"
breadcrumb     ^"!".+"!"[1-9]([0-9]*)"!"

%x HEX SOURCE NOLABELREF

%{

// Information encased in %{ ... }% is copied directly into the
// lexical output C source file, so anything inside here is
// valid C syntax.  Outside of the C fence, only lex syntax is
// expected.

#include "pasm_asm.tab.h"
#include "pasm_types.h"
#include "string.h"
#if !defined ( WINDOWS )
#define strnicmp strncasecmp
#endif

// For line counting...
int recovered_linenum;

// Previous lexer state for returning
int previous_state;

// Interface to add assembler errors to output
extern void add_error ( char* s );
extern char e [ 256 ];

// Keyword processing.
void d_hex ( void );
void d_textsegment ( void );
void d_datasegment ( void );
keyword asmkeywords [];

// Symbol table management.  The symbol table is available
// to both the lexer and parser.
symbol_list* current_stab = NULL;
symbol_list global_stab;
unsigned char add_symbol ( symbol_list* list, char* symbol, symbol_table** ptr );
void delete_symbol ( symbol_list* list, char* symbol );
symbol_table* find_symbol ( const char* symbol );

// Index of last declared label, for useful error messages like:
// error: <line>: after <symbol>: blah...
symbol_table* current_label = NULL;

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

// This function checks for valid assembler mnemonics in tokens.
unsigned char valid_instr ( char* instr );

extern char currentFile [];

%}

%%

<*>{breadcrumb} {
   char* ptr;

   // Recover the file name from the breadcrumb...
   strcpy ( currentFile, asmtext+1 ); // Skip the first !
   ptr = strchr(currentFile,'!');
   (*ptr) = '\0';                     // Remove the trailing !
   
   // Recover the line number from the breadcrumb...
   ptr = strchr(asmtext+1,'!')+1;     // Get to the second !
   recovered_linenum = strtoul ( ptr, NULL, 10 );
}

<INITIAL>{identifier1} {
   unsigned char f;
   ref_type* ref;
   text_type* text;
   keyword* k = asmkeywords;
   int m;

   // Assume that when we're done with this token we'll be processing a line of
   // assembly source code...
   BEGIN SOURCE;

   // Check for directives...
   while ( k->directive != NULL )
   {
      if ( ((strlen(k->directive) == asmleng) &&
           (strnicmp(asmtext,k->directive,strlen(k->directive)) == 0)) ||
           ((strlen(k->dotdirective) == asmleng) &&
           (strnicmp(asmtext,k->dotdirective,strlen(k->dotdirective)) == 0)) )
      {
         if ( k->handler )
         {
            k->handler ();
         }
         asmlval.directive = strdup ( k->directive );
         return k->token;
      }
      k++;
   }

   // If it's a valid instruction it's not a label...
   if ( (f=valid_instr(asmtext)) != INVALID_INSTR )
   {
      asmlval.instr = f;
      return INSTR;
   }
   else
   {
      char* label = strdup(asmtext);
      unsigned char f;
      symbol_table* p = NULL;
      ref_type* ref;
      if ( label[strlen(label)-1] == ':' )
      {
         label[strlen(label)-1] = 0;
      }
      f=add_symbol(current_stab,label,&p);
      if ( !f )
      {
         sprintf ( e, "multiple declarations of symbol: %s", label );
         add_error ( e );
      }
      current_label = p;
      ref = get_next_reftype ();
      ref->type = reference_symtab;
      ref->ref.symtab = p;
      asmlval.ref = ref;
      return LABEL;
   }
}

<SOURCE>{identifier2} {
   unsigned char f;
   ref_type* ref;
   text_type* text;
   keyword* k = asmkeywords;

   // Check for directives...
   while ( k->directive != NULL )
   {
      if ( ((strlen(k->directive) == asmleng) &&
           (strnicmp(asmtext,k->directive,strlen(k->directive)) == 0)) ||
           ((strlen(k->dotdirective) == asmleng) &&
           (strnicmp(asmtext,k->dotdirective,strlen(k->dotdirective)) == 0)) )
      {
         if ( k->handler )
         {
            k->handler ();
         }
         asmlval.directive = strdup ( k->directive );
         return k->token;
      }
      k++;
   }

   if ( asmtext[1] == 0 )
   {
      // Return individual X or Y to parser...
      if ( (asmtext[0] == 'x') ||
           (asmtext[0] == 'X') )
      {
         return tolower(asmtext[0]);
      }
      else if ( (asmtext[0] == 'y') ||
                (asmtext[0] == 'Y') )
      {
         return tolower(asmtext[0]);
      }
   }

   symbol_table* p = NULL;
   if ( (p=find_symbol(asmtext)) != NULL )
   {
      ref = get_next_reftype ();

      ref->type = reference_symtab;
      ref->ref.symtab = p;
      asmlval.ref = ref;
      previous_state = SOURCE;
      BEGIN NOLABELREF;
      return LABELREF;
   }
   else if ( (f=valid_instr(asmtext)) != INVALID_INSTR )
   {
      asmlval.instr = f;
      return INSTR;
   }
   else if ( asmtext[0] == '\'' &&
             asmtext[asmleng-1] == '\'' )
   {
      text = get_next_texttype ();
      text->string = strdup ( asmtext+1 );
      text->string[strlen(text->string)-1] = 0;
      // need to pass length so string mods made by expressions don't lose track of it if a
      // byte before the end of the string becomes 0.  for example, "AB"-"A" makes the
      // first byte zero and strlen() then returns 0 which is invalid.
      text->length = strlen(text->string);
      asmlval.text = text;
      return QUOTEDSTRING;
   }

   /* pass through as a possible forward-reference label */
   ref = get_next_reftype ();
   ref->type = reference_symbol;
   ref->ref.symbol = strdup(asmtext);
   asmlval.ref = ref;
   previous_state = SOURCE;
   BEGIN NOLABELREF;
   return LABELREF;
}

<SOURCE,NOLABELREF>{quotedstring} {
   text_type* text = get_next_texttype ();
   text->string = strdup ( asmtext+1 );
   text->string[strlen(text->string)-1] = 0;
   // need to pass length so string mods made by expressions don't lose track of it if a
   // byte before the end of the string becomes 0.  for example, "AB"-"A" makes the
   // first byte zero and strlen() then returns 0 which is invalid.
   text->length = strlen(text->string);
   asmlval.text = text;
   previous_state = yy_current_state;
   return QUOTEDSTRING;
}

<HEX>{rawhex} {
   text_type* text = get_next_texttype ();
   text->string = strdup ( asmtext );
   text->length = strlen(text->string);
   asmlval.text = text;
   return QUOTEDSTRING;
}

<SOURCE>{hexdigits} {
   char* ptr = asmtext;
   number_type* num = get_next_numtype ();
   if ( ptr[0] == '$' ) ptr++;
   num->number = strtoul(ptr,NULL,16);
   if ( (num->number >= -128) &&
        (num->number < 256) )
   {
      num->zp_ok = 1;
   }
   asmlval.num = num;
   previous_state = SOURCE;
   BEGIN NOLABELREF;
   return DIGITS;
}

<SOURCE>{decdigits} {
   number_type* num = get_next_numtype ();
   num->number = strtoul(asmtext,NULL,10);
   if ( (num->number >= -128) &&
        (num->number < 256) )
   {
      num->zp_ok = 1;
   }
   asmlval.num = num;
   previous_state = SOURCE;
   BEGIN NOLABELREF;
   return DIGITS;
}

<SOURCE>{octdigits} {
   number_type* num = get_next_numtype ();
   num->number = strtoul(asmtext,NULL,8);
   if ( (num->number >= -128) &&
        (num->number < 256) )
   {
      num->zp_ok = 1;
   }
   asmlval.num = num;
   previous_state = SOURCE;
   BEGIN NOLABELREF;
   return DIGITS;
}

<SOURCE>{bindigits} {
   char* ptr = asmtext;
   number_type* num = get_next_numtype ();
   if ( ptr[0] == '%' ) ptr++;

   num->number = strtoul(ptr,NULL,2);
   if ( (num->number >= -128) &&
        (num->number < 256) )
   {
      num->zp_ok = 1;
   }
   asmlval.num = num;
   previous_state = SOURCE;
   BEGIN NOLABELREF;
   return DIGITS;
}

<INITIAL,SOURCE,HEX>{whitespace} {
   /* throw away */
}

<NOLABELREF>{whitespace} {
   BEGIN previous_state;
   /* throw away */
}

<INITIAL,SOURCE,HEX,NOLABELREF>\r\n |
<INITIAL,SOURCE,HEX,NOLABELREF>\n\r |
<INITIAL,SOURCE,HEX,NOLABELREF>\n {
   BEGIN INITIAL;
   return TERM;
}
<SOURCE>"<<" {
   previous_state = SOURCE;
   return LSHIFT;
}

<NOLABELREF>"<<" {
   BEGIN previous_state;
   return LSHIFT;
}

<SOURCE>">>" {
   previous_state = SOURCE;
   return RSHIFT;
}

<NOLABELREF>">>" {
   BEGIN previous_state;
   return RSHIFT;
}

<SOURCE>"&&" {
   previous_state = SOURCE;
   return LOGAND;
}

<NOLABELREF>"&&" {
   BEGIN previous_state;
   return LOGAND;
}

<SOURCE>"||" {
   previous_state = SOURCE;
   return LOGOR;
}

<NOLABELREF>"||" {
   BEGIN previous_state;
   return LOGOR;
}

<SOURCE>"<=" {
   previous_state = SOURCE;
   return LTEQ;
}

<NOLABELREF>"<=" {
   BEGIN previous_state;
   return LTEQ;
}

<SOURCE>">=" {
   previous_state = SOURCE;
   return GTEQ;
}

<NOLABELREF>">=" {
   BEGIN previous_state;
   return GTEQ;
}

<SOURCE>"==" {
   previous_state = SOURCE;
   return EQEQ;
}

<NOLABELREF>"==" {
   BEGIN previous_state;
   return EQEQ;
}

<SOURCE>"!=" |
<SOURCE>"<>" {
   previous_state = SOURCE;
   return NOTEQ;
}

<NOLABELREF>"!=" |
<NOLABELREF>"<>" {
   BEGIN previous_state;
   return NOTEQ;
}

<SOURCE>[&|!@#=%\+\-\*\/\|&\~\<\>\[\],\(\)\^] {
   previous_state = SOURCE;
   return asmtext[0];
}

<NOLABELREF>[&|!@#=%\+\-\*\/\|&\~\<\>\[\],\(\)\^] {
   BEGIN previous_state;
   return asmtext[0];
}

<INITIAL,SOURCE,HEX,NOLABELREF>. {
   // ignore, error should already be given
}

%%

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
   asmlval.seg = text_segment;
}

// The .segment, .text, and .data directives send the same
// SEGMENT token to the parser so they need to indicate which
// one they are by sending data along with the token.  This
// function is executed when the .data directive
// is encountered in the token stream.
void d_datasegment ( void )
{
   asmlval.seg = data_segment;
}

// Keyword information.  Since keywords are parsed by the
// 'identifier' rule along with labels, forward-use variables,
// and even assembly mnemonics, we need a mechanism to search
// for keywords and handle them.  This is similar to the mechanism
// used to determine an assembler mnemonic in the 'identifier'
// regular expression.
keyword asmkeywords [] =
{
   { "byte", ".byte", DATAB, NULL },
   { "db", ".db", DATAB, NULL },
   { "dl", ".dl", DATAL, NULL },
   { "dh", ".dh", DATAH, NULL },
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
   { "enum", ".enum", ENUMERATE, NULL },
   { "ende", ".ende", ENDENUMERATE, NULL },
   { "advance", ".advance", ADVANCE, NULL },
   { "pad", ".pad", ADVANCE, NULL },
   { "align", ".align", ALIGN, NULL },
   { "text", ".text", SEGMENT, d_textsegment },
   { "segment", ".segment", SEGMENT, d_textsegment },
   { "data", ".data", SEGMENT, d_datasegment },
   { "incbin", ".incbin", INCBIN, NULL },
   { "bin", ".bin", INCBIN, NULL },
   { "fillvalue", ".fillvalue", FILLVALUE, NULL },
   { "if", ".if", IF, NULL },
   { "elseif", ".elseif", ELSEIF, NULL },
   { "else", ".else", ELSE, NULL },
   { "ifdef", ".ifdef", IF, NULL },
   { "ifndef", ".ifndef", IF, NULL },
   { "endif", ".endif", ENDIF, NULL },
   { "marker", ".marker", MARKER, NULL },
   { NULL, NULL, 0, NULL }
};

long asm_get_current_buffer ( void )
{
   return (long)YY_CURRENT_BUFFER;
}

symbol_table* find_symbol ( const char* symbol )
{
   unsigned int i;
   symbol_table* ptr = NULL;
   symbol_list* stab = current_stab;
   unsigned char found  = 0;

   do
   {
      // If not found in a previous search, continue...
      if ( !found )
      {
         // otherwise, search symbol table entirely...
         // for now just search global symbol table but eventually
         // we'll make this routine search scoped tables...
         for ( ptr = stab->tail; ptr != NULL; ptr = ptr->prev )
         {
            if ( (strlen(symbol) == strlen(ptr->symbol)) &&
                 (strcmp(symbol,ptr->symbol) == 0) &&
                 (ptr->alive) )
            {
               found = 1;
               break;
            }
         }
      }
      stab = stab->up;
   } while ( stab != NULL );

   // ptr gets set to NULL if not found in loops above...
   return ptr;
}

unsigned char add_symbol ( symbol_list* list, char* symbol, symbol_table** ptr )
{
   unsigned char a = 1;
   unsigned int i;

   (*ptr) = NULL;

   for ( (*ptr) = list->head; (*ptr) != NULL; (*ptr) = (*ptr)->next )
   {
      if ( (strlen(symbol) == strlen((*ptr)->symbol)) &&
           (strcmp((*ptr)->symbol,symbol) == 0) )
      {
         if ( ((*ptr)->symbol[0] == '+') ||
              ((*ptr)->symbol[0] == '-') )
         {
            // Allow re-declaration of temporary symbols...
            (*ptr)->alive = 0;
         }
         else
         {
            a = 0;
            return a;
         }
      }
   }

   if ( list->tail == NULL )
   {
      list->head = (symbol_table*) malloc ( sizeof(symbol_table) );
      if ( list->head != NULL )
      {
         list->tail = list->head;
         list->tail->symbol = (char*)malloc ( strlen(symbol)+1 );
         if ( list->tail->symbol != NULL )
         {
            memset ( list->tail->symbol, 0, strlen(symbol)+1 );
            strncpy ( list->tail->symbol, symbol, strlen(symbol) );
            list->tail->alive = 1;
            list->tail->ir = NULL;
            if ( cur )
            {
               list->tail->btab_ent = cur->idx;
            }
            else
            {
               list->tail->btab_ent = 0;
            }
            list->tail->expr = NULL;
         }
         list->tail->next = NULL;
         list->tail->prev = NULL;
      }
      else
      {
         asmerror ( "cannot allocate memory" );
      }
   }
   else
   {
      (*ptr) = (symbol_table*) malloc ( sizeof(symbol_table) );
      if ( (*ptr) != NULL )
      {
         list->tail->next = (*ptr);
         (*ptr)->prev = list->tail;
         (*ptr)->next = NULL;
         list->tail = (*ptr);
         list->tail->symbol = (char*)malloc ( strlen(symbol)+1 );
         if ( list->tail->symbol != NULL )
         {
            memset ( list->tail->symbol, 0, strlen(symbol)+1 );
            strncpy ( list->tail->symbol, symbol, strlen(symbol) );
            list->tail->alive = 1;
            list->tail->ir = NULL;
            if ( cur )
            {
               list->tail->btab_ent = cur->idx;
            }
            else
            {
               list->tail->btab_ent = 0;
            }
            list->tail->expr = NULL;
         }
      }
      else
      {
         asmerror ( "cannot allocate memory" );
      }
   }

   (*ptr) = list->tail;

   return a;
}

void delete_symbol ( symbol_list* list, char* symbol )
{
   symbol_table* ptr;
   symbol_table* ptd = NULL;

   for ( ptr = list->head; ptr != NULL; ptr = ptr->next )
   {
      if ( (strlen(symbol) == strlen(ptr->symbol)) &&
           (strcmp(ptr->symbol,symbol) == 0) )
      {
         if ( ptr->prev )
         {
            ptr->prev->next = ptr->next;
         }
         else
         {
            list->head = ptr->next;
         }
         if ( ptr->next )
         {
            ptr->next->prev = ptr->prev;
         }
         else
         {
            list->tail = ptr->next;
         }
         ptd = ptr;
      }
   }
   if ( ptd )
   {
      free ( ptd->symbol );
      if ( ptd->expr )
      {
         destroy_expression ( ptd->expr );
      }
      free ( ptd );
   }
}
