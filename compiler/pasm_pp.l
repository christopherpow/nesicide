%{

// This is the lexical analyzer for the 6502 assembler preprocessor
// built into NESICIDE, an Integrated Development Environment for
// the 8-bit Nintendo Entertainment System.

// This section declares the complex regular expressions used
// in token matching rules below.

%}

identifier ([_'.@a-zA-Z][_'.@a-zA-Z0-9]*)

hexdigits      (("$"([0-9a-fA-F]{1,8}))|(([0-9a-fA-F]{1,8})"h"))
decdigits      ([1-9]([0-9]{0,8}))
octdigits      "0"([0-9]{0,8})
bindigits      (("%"([01]{1,16}))|(([01]{1,16})"b"))

comment        ";".*$

%x PARSE_EQUATES CAPTURE_EQUATE
%x PARSE_MACROS MACRO_NAME MACRO_ARGS MACRO_BODY INVOKE_ARGVALS
%x PARSE_VARIABLES CAPTURE_VARIABLE
%x PARSE_REPEATS REPEAT_COUNT

%{

// Information encased in %{ ... }% is copied directly into the
// lexical output C source file, so anything inside here is
// valid C syntax.  Outside of the C fence, only lex syntax is
// expected.

#include "string.h"
#include "pasm_types.h"

// Track how many directives were processed in each go-around
// and when the number processed hits 0, we're done.  If we
// go around several times, quit.
#define MAX_PASSES 5
int directives_processed = 0;

typedef struct _ppkeyword
{
   const char* directive;
   const char* dotdirective;
   void (*handler)();
} ppkeyword;

// Symbol table stuff.
typedef struct _ppsymbol_table
{
   char*                 symbol;
   struct _ppsymbol_table* next;
   struct _ppsymbol_table* prev;
} ppsymbol_table;

typedef struct _ppsymbol_list
{
   struct _ppsymbol_table* head;
   struct _ppsymbol_table* tail;
} ppsymbol_list;

unsigned char add_ppsymbol ( ppsymbol_list* list, char* symbol );
ppsymbol_table* find_ppsymbol ( ppsymbol_list* list, char* symbol );

// For line counting...
#ifndef __APPLE__ //strange mac os x related bug? :S
int          pplineno = 0;
#endif

// Previous lexer state for returning.
// State movement within lexer is:
// Start -> PARSE_EQUATES: parse any equ directives and perform
//          equivalency replacements in input text.
// ... -> PARSE_MACROS: parse any macro/endm directives, form
//        macro structures including macro text, perform macro
//        invocation expansions.
// ... -> PARSE_VARIABLES: parse any variable = expression statements
//        and form values for each variable to be used in a similar fashion
//        to equ directive, but variables are reduced to numeric value first.
//        (variable values are needed for repeat loop counting)
// ... -> PARSE_REPEATS: parse any rept/endr directives and
//        perform repeated replacements in input text.
// ... -> Finish: Pass preprocessed input text as output text to
//        the assembler stage.
int ppprevious_state = PARSE_EQUATES;

#define OUTPUT_DEFAULT_ALLOC 1024
#define MAX_NESTS            10

// Macro stuff.
#define MACRO_DEFAULT_ALLOC 1024
#define PPMTAB_ENT_INC 1

typedef struct _ppmacro_table
{
   int idx; // used for self-reference and also for scoping variables
   char* symbol;
   char* text;
   int   text_blocks;
   int   text_length;
   struct _ppsymbol_list* stab; // macro symbols
   struct _ppsymbol_list* vtab; // macro symbol values
} ppmacro_table;

ppmacro_table* ppmtab;
int ppmtab_ent = 0;
int ppmtab_max = 0;
int macronestlevel = 0;
int current_macro [ MAX_NESTS ] = { -1, };
int create_macro ( char* symbol );
void copy_to_macro ( int macro, char* string, int length );
void check_macro_invocation ( int macro );
void dump_macro ( int macro );
char* create_macro_invocation ( int macro );
void destroy_macro_values ( int macro );

// Repeat stuff.
int repeat_start [ MAX_NESTS ];
int repeat_count [ MAX_NESTS ];
int reptnestlevel = 0;

// Equate stuff.
typedef struct _ppequate_table
{
   struct _ppsymbol_list stab; // symbols
   struct _ppsymbol_list vtab; // symbol values
} ppvariable_table;

ppvariable_table ppequtab;
char* equsym = NULL;
void dump_equates ( void );

// Variable stuff.
ppvariable_table ppvartab;
extern symbol_list* global_stab;
void dump_variables ( void );

int replace_symbols_with_values ( char* text, ppsymbol_list* slist, ppsymbol_list* vlist );
void assign_variable_values ( void );

// Temporary storage for line reconstructions to pass to parser...
char line [ 2048 ];

// Temporary repeat label to pass to parser to invoke variable = expression rule...
const char* reptsym = "----@@@@";

// Interface to add assembler errors to output
extern void add_error ( char* s );
extern char e [ 256 ];

// Preprocessor redo pass input buffer.
char* input_buffer;

// Preprocessor output.
void output_text ( char* text, int length );
void backup_output_text ( void );
char* output_buffer;
int   output_length;
int   output_blocks;

// Marker for where to back-up to in output stream when
// erasing things that shouldn't go past the preprocessor.
int   output_marker = -1;

// Assembler pass counter.
int pass;

int pperror(char* s)
{
   add_error ( s );
}

void k_macro ( void )
{
   if ( macronestlevel < (MAX_NESTS-1) )
   {
      macronestlevel++;
      BEGIN MACRO_NAME;
   }
   else
   {
      pperror ( "illegal nested macro" );
   }
}

void k_endm ( void )
{
   if ( macronestlevel )
   {
//      dump_macro ( current_macro[macronestlevel] );
      current_macro [ macronestlevel ] = -1;
      macronestlevel--;
   }
   else
   {
      pperror ( "endm without corresponding macro" );
   }
   BEGIN PARSE_MACROS;
}

void k_rept ( void )
{
   if ( reptnestlevel < (MAX_NESTS-1) )
   {
      reptnestlevel++;

      repeat_start [ reptnestlevel ] = output_length;
      ppprevious_state = PARSE_REPEATS;
      BEGIN REPEAT_COUNT;
   }
   else
   {
      sprintf ( e, "too many nested repeats, max is %d", MAX_NESTS );
      pperror ( e );
   }
}

void k_endr ( void )
{
   int length;

   if ( reptnestlevel )
   {
      // store current length because it's going to grow...
      length = strlen ( output_buffer+repeat_start[reptnestlevel] );

      // repeat stuff...but take off one for the original that
      // defined the stuff to repeat first.
      repeat_count [ reptnestlevel ]--;
      while ( repeat_count[reptnestlevel] > 0)
      {
         output_text ( output_buffer+repeat_start[reptnestlevel], length );
         repeat_count [ reptnestlevel ]--;
      }

      reptnestlevel--;
   }
   else
   {
      pperror ( "endr without corresponding rept" );
   }
   BEGIN PARSE_REPEATS;
}

void k_equ ( void )
{
   BEGIN CAPTURE_EQUATE;
}

// Keyword information.  Since keywords are parsed by the
// 'identifier' rule along with labels, forward-use variables,
// and even assembly mnemonics, we need a mechanism to search
// for keywords and handle them.  This is similar to the mechanism
// used to determine an assembler mnemonic in the 'identifier'
// regular expression.
ppkeyword ppkeywords_equ [] =
{
   { "equ", ".equ", k_equ },
   { NULL, NULL, NULL }
};

ppkeyword ppkeywords_macro [] =
{
   { "macro", ".macro", k_macro },
   { "endm", ".endm", k_endm},
   { NULL, NULL, NULL }
};

ppkeyword ppkeywords_repeat [] =
{
   { "rept", ".rept", k_rept },
   { "endr", ".endr", k_endr },
   { NULL, NULL, NULL }
};

ppkeyword ppkeywords [] =
{
   { "include", ".include", NULL },
   { "incsrc", ".incsrc", NULL },
   { "ifdef", ".ifdef", NULL },
   { "ifndef", ".ifndef", NULL },
   { "if", ".if", NULL },
   { "elseif", ".elseif", NULL },
   { "else", ".else", NULL },
   { "endif", ".endif", NULL },
   { NULL, NULL, NULL }
};

%}

%%

<*>{comment} {
   // throw away!
}

<PARSE_EQUATES>{identifier} {
   ppkeyword* k = ppkeywords_equ;
   int m;
   int echo = 1;

   // Check for directives...
   while ( k->directive != NULL )
   {
      if ( ((strlen(k->directive) == ppleng) &&
           (strnicmp(yytext,k->directive,strlen(k->directive)) == 0)) ||
           ((strlen(k->dotdirective) == ppleng) &&
           (strnicmp(yytext,k->dotdirective,strlen(k->dotdirective)) == 0)) )
      {
         if ( k->handler )
         {
            directives_processed++;
            k->handler ();
         }

         echo = 0;
      }
      k++;
   }

   if ( echo )
   {
      // Copy this identifier just incase it is an equate symbol
      free ( equsym );
      equsym = strdup ( pptext );

      output_marker = output_length;
      output_text ( pptext, ppleng );
   }
}

<PARSE_EQUATES>[ \t\r\n] {
   output_text ( pptext, ppleng );
}

<CAPTURE_EQUATE>[^; \t\r\n]+ {
   ppsymbol_table* ptr = find_ppsymbol ( &(ppequtab.stab), equsym );
   if ( !ptr )
   {
      add_ppsymbol ( &(ppequtab.stab), equsym );
      add_ppsymbol ( &(ppequtab.vtab), pptext );
//      dump_equates ();
   }
   else
   {
      sprintf ( e, "%s: equate symbol redefined", equsym );
      pperror ( e );
   }
   free ( equsym );
   equsym = NULL;
}

<CAPTURE_EQUATE>[ \t] {
   // throw away!
}

<CAPTURE_EQUATE>[\r\n] {
   // remove the line we just parsed...
   backup_output_text ();

   output_text ( pptext, ppleng );

   BEGIN PARSE_EQUATES;
}

<PARSE_VARIABLES>{identifier} {
   // Copy this identifier just incase it is an equate symbol
   free ( equsym );
   equsym = strdup ( pptext );

   output_marker = output_length;
   output_text ( pptext, ppleng );
}

<PARSE_VARIABLES>[ \t\r\n] {
   output_text ( pptext, ppleng );
}

<PARSE_VARIABLES>[=] {
   output_text ( pptext, ppleng );
   BEGIN CAPTURE_VARIABLE;
}

<CAPTURE_VARIABLE>[^;\r\n]+ {
   int buf;

   // Kill any previous declarations, use this one...
   ppsymbol_table* ptr = find_ppsymbol ( &(ppvartab.stab), equsym );
   if ( ptr )
   {
      ptr->symbol [ 0 ] = SYMBOL_KILLER;
   }
   add_ppsymbol ( &(ppvartab.stab), equsym );
   add_ppsymbol ( &(ppvartab.vtab), pptext );
   dump_variables ();

   sprintf ( line, "%s = %s\r\n", equsym, pptext );

   // Use parser to parse expression...
   buf = asm_scan_string ( line );
   asmparse();
   asm_flush_buffer ( buf );
   asm_delete_buffer ( buf );

   free ( equsym );
   equsym = NULL;
}

<CAPTURE_VARIABLE>[\r\n] {
   // remove the line we just parsed...
   backup_output_text ();

   output_text ( pptext, ppleng );

   directives_processed++;

   BEGIN PARSE_VARIABLES;
}

<PARSE_MACROS>{identifier} {
   ppkeyword* k = ppkeywords_macro;
   int m;
   int echo = 1;

   // Check for directives...
   while ( k->directive != NULL )
   {
      if ( ((strlen(k->directive) == ppleng) &&
           (strnicmp(yytext,k->directive,strlen(k->directive)) == 0)) ||
           ((strlen(k->dotdirective) == ppleng) &&
           (strnicmp(yytext,k->dotdirective,strlen(k->dotdirective)) == 0)) )
      {
         if ( k->handler )
         {
            directives_processed++;
            k->handler ();
         }

         echo = 0;
      }
      k++;
   }

   // Check for macro invocations...
   for ( m = 0; m < ppmtab_ent; m++ )
   {
      if ( (strlen(ppmtab[m].symbol) == ppleng) &&
           (strnicmp(yytext,ppmtab[m].symbol,strlen(ppmtab[m].symbol)) == 0) )
      {
         if ( macronestlevel < (MAX_NESTS-1) )
         {
            directives_processed++;
            macronestlevel++;
            current_macro [ macronestlevel ] = m;
            ppprevious_state = PARSE_MACROS;
            BEGIN INVOKE_ARGVALS;
         }
         else
         {
            sprintf ( e, "too many nested repeats, max is %d", MAX_NESTS );
            pperror ( e );
         }

         echo = 0;
      }
   }

   if ( echo )
   {
      output_text ( pptext, ppleng );
   }
}

<PARSE_MACROS>[ \t\r\n] {
   output_text ( pptext, ppleng );
}

<MACRO_NAME>{identifier} {
   current_macro [ macronestlevel ] = create_macro ( yytext );
   BEGIN MACRO_ARGS;
}

<MACRO_NAME>[ \t] {
   // throw away!
}

<MACRO_ARGS>[^;, \t\r\n]+ {
   add_ppsymbol ( ppmtab[current_macro[macronestlevel]].stab, yytext );
}

<MACRO_NAME,MACRO_ARGS>[\r\n] {
   output_text ( pptext, ppleng );
   BEGIN MACRO_BODY;
}

<MACRO_ARGS,INVOKE_ARGVALS>[, \t] {
   // throw away!
}

<MACRO_BODY>{identifier} {
   ppkeyword* k = ppkeywords_macro;
   int m;
   int echo = 1;

   // Check for directives...
   while ( k->directive != NULL )
   {
      // Only look for endm directives in a macro...
      // This could be easier done...
      if ( k->handler == k_endm )
      {
         if ( ((strlen(k->directive) == ppleng) &&
              (strnicmp(yytext,k->directive,strlen(k->directive)) == 0)) ||
              ((strlen(k->dotdirective) == ppleng) &&
              (strnicmp(yytext,k->dotdirective,strlen(k->dotdirective)) == 0)) )
         {
            if ( k->handler )
            {
               directives_processed++;
               k->handler ();
            }

            echo = 0;
         }
      }
      k++;
   }

   if ( echo )
   {
      // Copy this identifier just incase it is an equate symbol
      free ( equsym );
      equsym = strdup ( pptext );

      // Output text anyway...
      copy_to_macro ( current_macro[macronestlevel], pptext, ppleng );
   }
}

<MACRO_BODY>[^;\r\n] {
   copy_to_macro ( current_macro[macronestlevel], pptext, ppleng );
}

<MACRO_BODY>[\r\n] {
   output_text ( pptext, ppleng );
   copy_to_macro ( current_macro[macronestlevel], pptext, ppleng );
}

<INVOKE_ARGVALS>[^;, \t\r\n]+ {
   add_ppsymbol ( ppmtab[current_macro[macronestlevel]].vtab, yytext );
}

<INVOKE_ARGVALS>[\r\n] {
   char* invoketext;

   BEGIN ppprevious_state;

   check_macro_invocation ( current_macro[macronestlevel] );
   invoketext = create_macro_invocation ( current_macro[macronestlevel] );

   switch ( ppprevious_state )
   {
      case PARSE_MACROS:
         output_text ( pptext, ppleng );
         output_text ( invoketext, strlen(invoketext) );
      break;

      case MACRO_BODY:
         // copy this macro's invocation text to the macro we came from...
         copy_to_macro ( current_macro[macronestlevel-1], pptext, ppleng );
         copy_to_macro ( current_macro[macronestlevel-1], invoketext, strlen(invoketext) );
      break;
   }

//         dump_macro ( current_macro[macronestlevel] );
   free ( invoketext );
   destroy_macro_values ( current_macro[macronestlevel] );
   current_macro [ macronestlevel ] = -1;
   macronestlevel--;
}

<PARSE_REPEATS>{identifier} {
   ppkeyword* k = ppkeywords_repeat;
   int m;
   int echo = 1;

   // Check for directives...
   while ( k->directive != NULL )
   {
      if ( ((strlen(k->directive) == ppleng) &&
           (strnicmp(yytext,k->directive,strlen(k->directive)) == 0)) ||
           ((strlen(k->dotdirective) == ppleng) &&
           (strnicmp(yytext,k->dotdirective,strlen(k->dotdirective)) == 0)) )
      {
         if ( k->handler )
         {
            directives_processed++;
            k->handler ();
         }

         echo = 0;
      }
      k++;
   }

   if ( echo )
   {
      output_text ( pptext, ppleng );
   }
}

<PARSE_REPEATS>[\r\n] {
   output_text ( pptext, ppleng );
}

<REPEAT_COUNT>[^;\r\n]+ {
   symbol_table* ptr;
   int buf;

   // Kill any previous declarations, use this one...
   ppsymbol_table* ppptr = find_ppsymbol ( &(ppvartab.stab), (char*)reptsym );
   if ( ppptr )
   {
      ppptr->symbol [ 0 ] = SYMBOL_KILLER;
   }
   add_ppsymbol ( &(ppvartab.stab), (char*)reptsym );
   add_ppsymbol ( &(ppvartab.vtab), pptext );
   dump_variables ();

   sprintf ( line, "%s = %s\r\n", reptsym, pptext );

   // Use parser to parse expression...
   buf = asm_scan_string ( line );
   asmparse();
   asm_flush_buffer ( buf );
   asm_delete_buffer ( buf );

   // Find symbol we should have just parsed...
   ptr = find_symbol ( (char*)reptsym );
   if ( ptr )
   {
      // Figure out if expression exists and is fully parsed...
      if ( (ptr->expr) &&
           (ptr->expr->type == expression_number) )
      {
         repeat_count [ reptnestlevel ] = ptr->expr->node.num->number;
      }
      else
      {
      }
   }
   else
   {
   }
}

<REPEAT_COUNT>[\r\n] {
   output_text ( pptext, ppleng );
   BEGIN PARSE_REPEATS;
}

<INITIAL>[\r\n] {
   output_text ( pptext, ppleng );
}

<INITIAL,PARSE_MACROS,MACRO_NAME,MACRO_ARGS,MACRO_BODY,INVOKE_ARGVALS,PARSE_EQUATES,CAPTURE_EQUATE,PARSE_VARIABLES,CAPTURE_VARIABLE,PARSE_REPEATS,REPEAT_COUNT>. {
   output_text ( pptext, ppleng );
}

%%

#undef ppwrap
int ppwrap(void)
{
   int go_around = 0;

   switch ( pass )
   {
      case PARSE_EQUATES:
         // do any equate replacements necessary...
         directives_processed = replace_symbols_with_values ( output_buffer, &(ppequtab.stab), &(ppequtab.vtab) );

         if ( directives_processed == 0 )
         {
            pass = PARSE_MACROS;
         }
         go_around = 1;
      break;

      case PARSE_MACROS:
         if ( directives_processed == 0 )
         {
            pass = PARSE_VARIABLES;
         }
         go_around = 1;
      break;

      case PARSE_VARIABLES:
         // reduce any expressions found...
         reduce_expressions ();

         // assign variable values for repeats stage...
         assign_variable_values ();

         // do any equate replacements necessary...
         replace_symbols_with_values ( output_buffer, &(ppvartab.stab), &(ppvartab.vtab) );

         pass = PARSE_REPEATS;
         go_around = 1;
      break;

      case PARSE_REPEATS:
         // we're done...INITIAL is a bit of a misnomer...
         pass = INITIAL;
         go_around = 0;
      break;
   }

//      printf ( "\n%d %d INPUT: \n%s\n\n", pass, directives_processed, input_buffer );
//      printf ( "\nOUTPUT: \n%s\n\n", output_buffer );

   if ( go_around )
   {
      // go around again...
      free ( input_buffer );
      input_buffer = strdup ( output_buffer );
      free ( output_buffer );
      output_buffer = NULL;
      output_length = 0;
      output_blocks = 0;
      directives_processed = 0;
      pp_scan_string ( input_buffer );
      BEGIN pass;
      ppin = NULL;
      return 0;
   }

   return 1;
}

int preprocess ( char* buffer_in, char** buffer_out, int* length )
{
   // Initialize the preprocessor buffers...
   input_buffer = NULL;
   output_buffer = NULL;
   output_length = 0;
   output_blocks = 0;
   output_marker = -1;
   directives_processed = 0;
   pass = PARSE_EQUATES;
   ppprevious_state = PARSE_EQUATES;

   if ( ppmtab )
   {

   }
   ppmtab_ent = 0;
   ppmtab_max = 0;
   macronestlevel = 0;

   reptnestlevel = 0;

   equsym = NULL;

   // If buffer_in isn't null it means we're being called from
   // the library code.  In that case the string to parse is
   // passed to us.  In the executable case the parse material
   // comes from a file (that could be stdin).
   if ( buffer_in )
   {
      // Redirect lex to the string to parse...
      input_buffer = strdup ( buffer_in );
      pp_scan_string ( input_buffer );
      ppin = NULL;
   }

   // First things first, parse for equates...
   BEGIN pass;

   // Preprocessor lexer returns when done...
   pplex ();

   // Clean up...
   free ( input_buffer );

   // Return preprocessed output...
   (*buffer_out) = output_buffer;
   (*length) = output_length;

   return output_length;
}

void output_text ( char* text, int length )
{
   int blocks = (output_length+length)/OUTPUT_DEFAULT_ALLOC;

   if ( output_buffer )
   {
      if ( blocks > output_blocks )
      {
         output_buffer = realloc ( output_buffer, (blocks+1)*OUTPUT_DEFAULT_ALLOC );
         output_buffer [ blocks*OUTPUT_DEFAULT_ALLOC ] = 0;
         output_blocks = blocks;
      }
   }
   else
   {
      output_buffer = malloc ( OUTPUT_DEFAULT_ALLOC );
      output_buffer [ 0 ] = 0;
   }
   strncat ( output_buffer, text, length );
   output_length += length;
}

void backup_output_text ( void )
{
   while ( (output_length >= output_marker) &&
           (output_buffer[output_length] != '\r') &&
           (output_buffer[output_length] != '\n') )
   {
      output_buffer[output_length] = 0;
      output_length--;
   }
}

int create_macro ( char* symbol )
{
   int idx = -1;

   if ( ppmtab == NULL )
   {
      ppmtab = (ppmacro_table*)calloc ( PPMTAB_ENT_INC, sizeof(ppmacro_table) );
      if ( ppmtab != NULL )
      {
         ppmtab_max += PPMTAB_ENT_INC;
         ppmtab[ppmtab_ent].symbol = (char*)malloc ( strlen(symbol)+1 );
         if ( ppmtab[ppmtab_ent].symbol != NULL )
         {
            memset ( ppmtab[ppmtab_ent].symbol, 0, strlen(symbol)+1 );
            strncpy ( ppmtab[ppmtab_ent].symbol, symbol, strlen(symbol) );
            ppmtab[ppmtab_ent].idx = ppmtab_ent;
            ppmtab[ppmtab_ent].text = malloc ( MACRO_DEFAULT_ALLOC );
            ppmtab[ppmtab_ent].text[0] = 0;
            ppmtab[ppmtab_ent].text_blocks = 0;
            ppmtab[ppmtab_ent].text_length = 0;
            ppmtab[ppmtab_ent].stab = (ppsymbol_list*) malloc ( sizeof(ppsymbol_list) );
            ppmtab[ppmtab_ent].stab->head = NULL;
            ppmtab[ppmtab_ent].stab->tail = NULL;
            ppmtab[ppmtab_ent].vtab = (ppsymbol_list*) malloc ( sizeof(ppsymbol_list) );
            ppmtab[ppmtab_ent].vtab->head = NULL;
            ppmtab[ppmtab_ent].vtab->tail = NULL;
         }
      }
      else
      {
         sprintf ( e, "unable to allocate memory for tables" );
         pperror ( e );
      }
   }
   else
   {
      if ( ppmtab_ent < ppmtab_max )
      {
         ppmtab[ppmtab_ent].symbol = (char*)malloc ( strlen(symbol)+1 );
         if ( ppmtab[ppmtab_ent].symbol != NULL )
         {
            memset ( ppmtab[ppmtab_ent].symbol, 0, strlen(symbol)+1 );
            strncpy ( ppmtab[ppmtab_ent].symbol, symbol, strlen(symbol) );
            ppmtab[ppmtab_ent].idx = ppmtab_ent;
            ppmtab[ppmtab_ent].text = malloc ( MACRO_DEFAULT_ALLOC );
            ppmtab[ppmtab_ent].text[0] = 0;
            ppmtab[ppmtab_ent].text_blocks = 0;
            ppmtab[ppmtab_ent].text_length = 0;
            ppmtab[ppmtab_ent].stab = (ppsymbol_list*) malloc ( sizeof(ppsymbol_list) );
            ppmtab[ppmtab_ent].stab->head = NULL;
            ppmtab[ppmtab_ent].stab->tail = NULL;
            ppmtab[ppmtab_ent].vtab = (ppsymbol_list*) malloc ( sizeof(ppsymbol_list) );
            ppmtab[ppmtab_ent].vtab->head = NULL;
            ppmtab[ppmtab_ent].vtab->tail = NULL;
         }
      }
      else
      {
         ppmtab_max += PPMTAB_ENT_INC;
         ppmtab = (ppmacro_table*)realloc ( ppmtab, ppmtab_max*sizeof(ppmacro_table) );
         if ( ppmtab != NULL )
         {
            ppmtab[ppmtab_ent].symbol = (char*)malloc ( strlen(symbol)+1 );
            if ( ppmtab[ppmtab_ent].symbol != NULL )
            {
               memset ( ppmtab[ppmtab_ent].symbol, 0, strlen(symbol)+1 );
               strncpy ( ppmtab[ppmtab_ent].symbol, symbol, strlen(symbol) );
               ppmtab[ppmtab_ent].idx = ppmtab_ent;
               ppmtab[ppmtab_ent].text = malloc ( MACRO_DEFAULT_ALLOC );
               ppmtab[ppmtab_ent].text[0] = 0;
               ppmtab[ppmtab_ent].text_blocks = 0;
               ppmtab[ppmtab_ent].text_length = 0;
               ppmtab[ppmtab_ent].stab = (ppsymbol_list*) malloc ( sizeof(ppsymbol_list) );
               ppmtab[ppmtab_ent].stab->head = NULL;
               ppmtab[ppmtab_ent].stab->tail = NULL;
               ppmtab[ppmtab_ent].vtab = (ppsymbol_list*) malloc ( sizeof(ppsymbol_list) );
               ppmtab[ppmtab_ent].vtab->head = NULL;
               ppmtab[ppmtab_ent].vtab->tail = NULL;
            }
         }
         else
         {
            sprintf ( e, "unable to allocate memory for tables" );
            pperror ( e );
         }
      }
   }

   idx = ppmtab_ent;

   ppmtab_ent++;

   return idx;
}

void copy_to_macro ( int macro, char* string, int length )
{
   int blocks = (ppmtab[macro].text_length+length)/MACRO_DEFAULT_ALLOC;

   // macro text block is already allocated on macro creation...
   if ( blocks > ppmtab[macro].text_blocks )
   {
      ppmtab [ macro ].text = realloc ( ppmtab[macro].text, (blocks+1)*MACRO_DEFAULT_ALLOC );
      ppmtab [ macro ].text [ blocks*MACRO_DEFAULT_ALLOC ] = 0;
      ppmtab [ macro ].text_blocks = blocks;
   }
   strncat ( ppmtab[macro].text, string, length );
   ppmtab [ macro ].text_length += length;
}

ppsymbol_table* find_ppsymbol ( ppsymbol_list* list, char* symbol )
{
   unsigned int i;
   ppsymbol_table* ptr = NULL;
   unsigned char found  = 0;

   for ( ptr = list->tail; ptr != NULL; ptr = ptr->prev )
   {
      if ( (strlen(symbol) == strlen(ptr->symbol)) &&
           (strcmp(symbol,ptr->symbol) == 0) )
      {
         found = 1;
         break;
      }
   }

   // ptr gets set to NULL if not found in loops above...
   return ptr;
}

unsigned char add_ppsymbol ( ppsymbol_list* list, char* symbol )
{
   ppsymbol_table* ptr;
   unsigned char a = 1;

   if ( list->tail == NULL )
   {
      list->head = (ppsymbol_table*) malloc ( sizeof(ppsymbol_table) );
      if ( list->head != NULL )
      {
         list->tail = list->head;
         list->tail->symbol = (char*)malloc ( strlen(symbol)+1 );
         if ( list->tail->symbol != NULL )
         {
            memset ( list->tail->symbol, 0, strlen(symbol)+1 );
            strncpy ( list->tail->symbol, symbol, strlen(symbol) );
         }
         list->tail->next = NULL;
         list->tail->prev = NULL;
      }
      else
      {
         pperror ( "cannot allocate memory" );
      }
   }
   else
   {
      ptr = (ppsymbol_table*) malloc ( sizeof(ppsymbol_table) );
      if ( ptr != NULL )
      {
         list->tail->next = ptr;
         ptr->prev = list->tail;
         ptr->next = NULL;
         list->tail = ptr;
         list->tail->symbol = (char*)malloc ( strlen(symbol)+1 );
         if ( list->tail->symbol != NULL )
         {
            memset ( list->tail->symbol, 0, strlen(symbol)+1 );
            strncpy ( list->tail->symbol, symbol, strlen(symbol) );
         }
      }
      else
      {
         asmerror ( "cannot allocate memory" );
      }
   }

   return a;
}

void check_macro_invocation ( int macro )
{
   ppsymbol_list* slist = ppmtab[macro].stab;
   ppsymbol_list* vlist = ppmtab[macro].vtab;
   ppsymbol_table* sptr = slist->head;
   ppsymbol_table* vptr = vlist->head;
   int i =0;

   // Check that all macro symbols have values declared on
   // the current invocation...errors if not.
   while ( sptr != NULL )
   {
      // Is there a value?
      if ( !vptr )
      {
         // add empty value to prevent seg-fault...
         add_ppsymbol ( ppmtab[macro].vtab, " " );

         // but this macro invocation is not valid...
         sprintf ( e, "missing value for argument '%s' in macro invocation", sptr->symbol );
         pperror ( e );
      }
      sptr = sptr->next;
      if ( vptr )
      {
         vptr = vptr->next;
      }
   }
}

char* create_macro_invocation ( int macro )
{
   // assume the macro won't double in size during invocation...
   char* invokedtext = malloc ( ppmtab[macro].text_length*2 );

   // copy the macro text to the new invokedtext to start...
   strcpy ( invokedtext, ppmtab[macro].text );

   // replace symbols with values...
   replace_symbols_with_values ( invokedtext,
                                 ppmtab[macro].stab,
                                 ppmtab[macro].vtab );

   return invokedtext;
}

void assign_variable_values ( void )
{
   ppsymbol_list* slist = &(ppvartab.stab);
   ppsymbol_list* vlist = &(ppvartab.vtab);
   ppsymbol_table* sptr = slist->head;
   ppsymbol_table* vptr = vlist->head;
   symbol_table* ptr;

   while ( sptr != NULL )
   {
      // find symbol to grab it's expression...
      ptr = find_symbol ( sptr->symbol );
      if ( ptr && vptr )
      {
         free ( vptr->symbol );
         vptr->symbol = NULL;
         convert_expression_to_string ( ptr->expr, &(vptr->symbol) );
      }
      sptr = sptr->next;
      if ( vptr )
      {
         vptr = vptr->next;
      }
   }
}

int replace_symbols_with_values ( char* text, ppsymbol_list* slist, ppsymbol_list* vlist )
{
   ppsymbol_table* sptr = slist->head;
   ppsymbol_table* vptr = vlist->head;
   char* ptr;
   int i;
   int valid;
   int replacements = 0;

   // go through each symbol and replace the text with
   // the symbol value wherever the symbol value is found...
   while ( sptr != NULL )
   {
      ptr = text;
      while ( (ptr=strstr(ptr,sptr->symbol)) != NULL )
      {
         // check to make sure this is actually a valid replacement
         // and not somewhere in the middle of another word...
         valid = 1;
         if ( ptr > text )
         {
            if ( isalnum(*(ptr-1)) )
            {
               // an alphanumeric character exists before me, probably
               // not a valid replacement here...
               valid = 0;
               ptr++;
            }
         }
         if ( ptr < (text+strlen(text)-1) )
         {
            if ( isalnum(*(ptr+strlen(sptr->symbol))) )
            {
               // an alphanumeric character exists after me, probably
               // not a valid replacement here...
               valid = 0;
               ptr++;
            }
         }

         if ( valid )
         {
            // symbol found...figure out if we can replace directly
            // or if we need to masticate things a bit.  we can replace
            // directly if the symbol's invoke value is equal or less
            // characters than the symbol name.
            if ( strlen(vptr->symbol) <= strlen(sptr->symbol) )
            {
               // replace and pad with spaces
               // use memcpy so no NULL is added...
               memcpy ( ptr, vptr->symbol, strlen(vptr->symbol) );
               ptr += strlen ( vptr->symbol );
               for ( i = 0; i < (strlen(sptr->symbol)-strlen(vptr->symbol)); i++ )
               {
                  (*ptr) = ' ';
                  ptr++;
               }
            }
            else
            {
               // move the remainder of the text ahead and fill
               // in the symbol's invoke value...
               // use memcpy so no NULL is added...
               for ( i = 0; i < strlen(ptr); i++ )
               {
                  *(ptr+strlen(ptr)-i+(strlen(vptr->symbol)-strlen(sptr->symbol))) = *(ptr+strlen(ptr)-i);
               }
               *(ptr+strlen(ptr)+1) = 0;
               memcpy ( ptr, vptr->symbol, strlen(vptr->symbol) );
            }

            // we did a replacement, keep track...
            replacements++;
         }
      }

      sptr = sptr->next;
      vptr = vptr->next;
   }

   return replacements;
}

void destroy_macro_values ( int macro )
{
   ppsymbol_list* vlist = ppmtab[macro].vtab;
   ppsymbol_table* vptr = vlist->head;
   ppsymbol_table* dptr = NULL;

   while ( vptr != NULL )
   {
      dptr = vptr;
      vptr = vptr->next;
      free ( dptr->symbol );
      free ( dptr );
   }
   vlist->head = NULL;
   vlist->tail = NULL;
}

void dump_macro ( int macro )
{
   ppsymbol_list* slist = ppmtab[macro].stab;
   ppsymbol_list* vlist = ppmtab[macro].vtab;
   ppsymbol_table* sptr = slist->head;
   ppsymbol_table* vptr = vlist->head;
   int i =0;

   printf ( "%d: macro %s\n", macro, ppmtab[macro].symbol );
   printf ( "symbols [values]:\n" );
   while ( sptr != NULL )
   {
      if ( vptr )
      {
         printf ( "%d: %s [%s]\n", i, sptr->symbol, vptr->symbol );
      }
      else
      {
         printf ( "%d: %s []\n", i, sptr->symbol );
      }
      i++;
      sptr = sptr->next;
      if ( vptr )
      {
         vptr = vptr->next;
      }
   }
   printf ( "text:\n" );
   printf ( "%s", ppmtab[macro].text );
   printf ( ":text\n" );
}

void dump_equates ( void )
{
   ppsymbol_list* slist = &(ppequtab.stab);
   ppsymbol_list* vlist = &(ppequtab.vtab);
   ppsymbol_table* sptr = slist->head;
   ppsymbol_table* vptr = vlist->head;
   int i =0;

   printf ( "equates\n" );
   printf ( "symbols [values]:\n" );
   while ( sptr != NULL )
   {
      if ( vptr )
      {
         printf ( "%d: %s [%s]\n", i, sptr->symbol, vptr->symbol );
      }
      else
      {
         printf ( "%d: %s []\n", i, sptr->symbol );
      }
      i++;
      sptr = sptr->next;
      if ( vptr )
      {
         vptr = vptr->next;
      }
   }
}

void dump_variables ( void )
{
   ppsymbol_list* slist = &(ppvartab.stab);
   ppsymbol_list* vlist = &(ppvartab.vtab);
   ppsymbol_table* sptr = slist->head;
   ppsymbol_table* vptr = vlist->head;
   int i =0;

   printf ( "variables\n" );
   printf ( "symbols [values]:\n" );
   while ( sptr != NULL )
   {
      if ( vptr )
      {
         printf ( "%d: %s [%s]\n", i, sptr->symbol, vptr->symbol );
      }
      else
      {
         printf ( "%d: %s []\n", i, sptr->symbol );
      }
      i++;
      sptr = sptr->next;
      if ( vptr )
      {
         vptr = vptr->next;
      }
   }
}
