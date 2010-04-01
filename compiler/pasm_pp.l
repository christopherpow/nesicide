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

%x MACRO_NAME MACRO_ARGS MACRO_BODY INVOKE_ARGVALS
%x REPEAT_COUNT

%{

// Information encased in %{ ... }% is copied directly into the
// lexical output C source file, so anything inside here is
// valid C syntax.  Outside of the C fence, only lex syntax is
// expected.

#include "string.h"
#include "pasm_types.h"

typedef struct _ppkeyword
{
   const char* directive;
   const char* dotdirective;
   void (*handler)();
} ppkeyword;

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

// For line counting...
#ifndef __APPLE__ //strange mac os x related bug? :S
int          pplineno = 0;
#endif

// Previous lexer state for returning
int ppprevious_state = INITIAL;

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
   int   text_size;
   struct _ppsymbol_list* stab; // macro symbols
   struct _ppsymbol_list* vtab; // macro symbol values
} ppmacro_table;

int nestlevel = 0;

ppmacro_table* ppmtab;
int ppmtab_ent = 0;
int ppmtab_max = 0;
int current_macro [ MAX_NESTS ] = { -1, };
int create_macro ( char* symbol );
void copy_to_macro ( int macro, char* s );
unsigned char add_macro_symbol ( ppsymbol_list* list, char* symbol );
void check_macro_invocation ( int macro );
void dump_macro ( int macro );
char* create_macro_invocation ( int macro );
void destroy_macro_values ( int macro );

// Repeat stuff.
int repeat_start [ MAX_NESTS ];
int repeat_count [ MAX_NESTS ];

// Interface to add assembler errors to output
extern void add_error ( char* s );
extern char e [ 256 ];

// Preprocessor output.
void output_text ( char* text, int length );
char* output_buffer;
int   output_length;
int   output_blocks;

int pperror(char* s)
{
   add_error ( s );
}

void k_macro ( void )
{
   if ( nestlevel < (MAX_NESTS-1) )
   {
      nestlevel++;
      BEGIN MACRO_NAME;
   }
   else
   {
      pperror ( "illegal nested macro" );
   }
}

void k_endm ( void )
{
   if ( nestlevel )
   {
//      dump_macro ( current_macro[nestlevel] );
      current_macro [ nestlevel ] = -1;
      nestlevel--;
   }
   else
   {
      pperror ( "endm without corresponding macro" );
   }
   BEGIN INITIAL;
}

void k_rept ( void )
{
   if ( nestlevel < (MAX_NESTS-1) )
   {
      nestlevel++;
      repeat_start [ nestlevel ] = output_length;
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

   if ( nestlevel )
   {
      // store current length because it's going to grow...
      length = strlen ( output_buffer+repeat_start[nestlevel] );

      // repeat stuff...but take off one for the original that
      // defined the stuff to repeat first.
      repeat_count [ nestlevel ]--;
      while ( repeat_count[nestlevel] > 0)
      {
         output_text ( output_buffer+repeat_start[nestlevel], length );
         repeat_count [ nestlevel ]--;
      }

      nestlevel--;
   }
   else
   {
      pperror ( "endr without corresponding rept" );
   }
   BEGIN INITIAL;
}

// Keyword information.  Since keywords are parsed by the
// 'identifier' rule along with labels, forward-use variables,
// and even assembly mnemonics, we need a mechanism to search
// for keywords and handle them.  This is similar to the mechanism
// used to determine an assembler mnemonic in the 'identifier'
// regular expression.
ppkeyword ppkeywords [] =
{
   { "include", ".include", NULL },
   { "incsrc", ".incsrc", NULL },
   { "rept", ".rept", k_rept },
   { "endr", ".endr", k_endr },
   { "macro", ".macro", k_macro },
   { "endm", ".endm", k_endm},
   { "ifdef", ".ifdef", NULL },
   { "ifndef", ".ifndef", NULL },
   { "if", ".if", NULL },
   { "elseif", ".elseif", NULL },
   { "else", ".else", NULL },
   { "endif", ".endif", NULL },
   { "equ", ".equ", NULL },
   { NULL, NULL, NULL }
};

%}

%%

<INITIAL>{identifier} {
   ppkeyword* k = ppkeywords;
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
         if ( nestlevel < (MAX_NESTS-1) )
         {
            nestlevel++;
            current_macro [ nestlevel ] = m;
            ppprevious_state = INITIAL;
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

<INITIAL>[\r\n] {
   output_text ( pptext, ppleng );
}

<INITIAL>[ \t] {
   output_text ( pptext, ppleng );
}

<MACRO_NAME>{identifier} {
   current_macro [ nestlevel ] = create_macro ( yytext );
   BEGIN MACRO_ARGS;
}

<MACRO_ARGS>[^(, \t\r\n)] {
   add_macro_symbol ( ppmtab[current_macro[nestlevel]].stab, yytext );
}

<MACRO_NAME,MACRO_ARGS>[\r\n] {
   output_text ( pptext, ppleng );
   BEGIN MACRO_BODY;
}

<MACRO_ARGS,INVOKE_ARGVALS>[, \t] {
   // throw away!
}

<MACRO_NAME,REPEAT_COUNT>[ \t] {
   // throw away!
}

<MACRO_BODY>{identifier} {
   ppkeyword* k = ppkeywords;
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
         if ( nestlevel < (MAX_NESTS-1) )
         {
            nestlevel++;
            current_macro [ nestlevel ] = m;
            ppprevious_state = MACRO_BODY;
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
      copy_to_macro ( current_macro[nestlevel], yytext );
   }
}

<MACRO_BODY>. {
   copy_to_macro ( current_macro[nestlevel], yytext );
}

<MACRO_BODY>[\r\n] {
   output_text ( pptext, ppleng );
   copy_to_macro ( current_macro[nestlevel], yytext );
}

<INVOKE_ARGVALS>[^(, \t\r\n)]+ {
   add_macro_symbol ( ppmtab[current_macro[nestlevel]].vtab, yytext );
}

<INVOKE_ARGVALS>[\r\n] {
   char* invoketext;

   output_text ( pptext, ppleng );

   BEGIN ppprevious_state;

   check_macro_invocation ( current_macro[nestlevel] );
   invoketext = create_macro_invocation ( current_macro[nestlevel] );

   switch ( ppprevious_state )
   {
      case INITIAL:
         output_text ( invoketext, strlen(invoketext) );
      break;

      case MACRO_BODY:
         // copy this macro's invocation text to the macro we came from...
         copy_to_macro ( current_macro[nestlevel-1], invoketext );
      break;
   }

//         dump_macro ( current_macro[nestlevel] );
   free ( invoketext );
   destroy_macro_values ( current_macro[nestlevel] );
   current_macro [ nestlevel ] = -1;
   nestlevel--;
}

<REPEAT_COUNT>{hexdigits} {
   char* ptr = pptext;
   if ( ptr[0] == '$' ) ptr++;
   repeat_count [ nestlevel ] = strtoul(ptr,NULL,16);
}

<REPEAT_COUNT>{decdigits} {
   repeat_count [ nestlevel ] = strtoul(pptext,NULL,10);
}

<REPEAT_COUNT>{octdigits} {
   repeat_count [ nestlevel ] = strtoul(pptext,NULL,8);
}

<REPEAT_COUNT>{bindigits} {
   char* ptr = pptext;
   if ( ptr[0] == '%' ) ptr++;
   repeat_count [ nestlevel ] = strtoul(ptr,NULL,2);
}

<REPEAT_COUNT>[\r\n] {
   output_text ( pptext, ppleng );
   BEGIN INITIAL;
}

<INITIAL,MACRO_NAME,MACRO_ARGS,INVOKE_ARGVALS,REPEAT_COUNT>. {
   output_text ( pptext, ppleng );
}

%%

#undef ppwrap
int ppwrap(void)
{
   return 1;
}

int preprocess ( char* buffer_in, char** buffer_out, int* length )
{
   output_buffer = NULL;
   output_length = 0;
   output_blocks = 0;

   if ( buffer_in )
   {
      pp_scan_string ( buffer_in );
      ppin = NULL;
   }

   // Preprocessor lexer only returns when done...
   pplex ();

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
            ppmtab[ppmtab_ent].text_size = MACRO_DEFAULT_ALLOC;
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
            ppmtab[ppmtab_ent].text_size = MACRO_DEFAULT_ALLOC;
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
               ppmtab[ppmtab_ent].text_size = MACRO_DEFAULT_ALLOC;
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

void copy_to_macro ( int macro, char* s )
{

   ppmtab[macro].text_size += strlen(s);
// CPTODO: handle reallocing...
   strcat ( ppmtab[macro].text, s );
}

unsigned char add_macro_symbol ( ppsymbol_list* list, char* symbol )
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
         add_macro_symbol ( ppmtab[macro].vtab, " " );

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
   static char scratch [ MACRO_DEFAULT_ALLOC ];
   char* invokedtext = malloc ( MACRO_DEFAULT_ALLOC );
   ppsymbol_list* slist = ppmtab[macro].stab;
   ppsymbol_list* vlist = ppmtab[macro].vtab;
   ppsymbol_table* sptr = slist->head;
   ppsymbol_table* vptr = vlist->head;
   char* ptr;
   int i;
   int valid;

   // copy the macro text to the new invokedtext to start...
   strcpy ( invokedtext, ppmtab[macro].text );

   // go through each macro symbol and replace the text with
   // the symbol value wherever the symbol value is found...
   while ( sptr != NULL )
   {
      ptr = invokedtext;
      while ( (ptr=strstr(ptr,sptr->symbol)) != NULL )
      {
         // check to make sure this is actually a valid replacement
         // and not somewhere in the middle of another word...
         valid = 1;
         if ( ptr > invokedtext )
         {
            if ( isalnum(*(ptr-1)) )
            {
               // an alphanumeric character exists before me, probably
               // not a valid replacement here...
               valid = 0;
               ptr++;
            }
         }
         if ( ptr < (invokedtext+strlen(invokedtext)-1) )
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
               }
            }
            else
            {
               // move the remainder of the text ahead and fill
               // in the symbol's invoke value...
               strcpy ( scratch, ptr+strlen(sptr->symbol) );
               memcpy ( ptr, vptr->symbol, strlen(vptr->symbol) );
               strcpy ( ptr+strlen(vptr->symbol), scratch );
            }
         }
      }

      sptr = sptr->next;
      vptr = vptr->next;
   }
   return invokedtext;
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
