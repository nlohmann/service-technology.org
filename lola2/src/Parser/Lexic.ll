/*!
\file
\brief lexic for LoLA low-level Petri nets
\author Karsten
\status approved 25.01.2012
\ingroup g_frontend

Mainly copied from LoLA1

\todo Herausfinden, ob es Probleme bei zu langen Kommentaren/Bezeichnern gibt. Idee: Maximallänge angeben.
\todo Präfix hinzufügen?
*/

/* yylineno: we want line numbering
   nounput: we don't need yyunput() */
%option yylineno
%option nounput
%option outfile="lex.yy.c"
%option prefix="ptnetlola_"

%{
#include <cmdline.h>
#include <Core/Dimensions.h>            // for yylval union
#include <Parser/ArcList.h>             // for yylval union
#include <Parser/ParserPTNetLoLA.hh>
#include <InputOutput/InputOutput.h>

extern void ptnetlola_error(char const* mess);
unsigned int ptnetlola_colno = 1;

/*!
\brief This macro is executed prior to the matched rule's action.

We use this macro to set set #ptnetlola_lloc to the positions of #ptnetlola_text. It further
manages the current column number #ptnetlola_colno. See Flex's manual
http://flex.sourceforge.net/manual/Misc-Macros.html for more information on
the macro.
*/
#define YY_USER_ACTION \
  ptnetlola_lloc.first_line = ptnetlola_lloc.last_line = ptnetlola_lineno; \
  ptnetlola_lloc.first_column = ptnetlola_colno; \
  ptnetlola_lloc.last_column = ptnetlola_colno+ptnetlola_leng-1; \
  ptnetlola_colno += ptnetlola_leng;

%}

%s IN_COMMENT

%%

 /* from http://flex.sourceforge.net/manual/How-can-I-match-C_002dstyle-comments_003f.html */
"/*"                   { BEGIN(IN_COMMENT); }
<IN_COMMENT>"*/"       { BEGIN(INITIAL); }
<IN_COMMENT>[^*\n\r]+  { /* comments */ }
<IN_COMMENT>"*"        { /* comments */ }
<IN_COMMENT>[\n\r]     { /* comments */ }

"{"[^\n\r]*"}"         { /* comments */ }

CONSUME                { return KEY_CONSUME; }
FAIR                   { return KEY_FAIR; }
MARKING                { return KEY_MARKING; }
PLACE                  { return KEY_PLACE; }
PRODUCE                { return KEY_PRODUCE; }
SAFE                   { return KEY_SAFE; }
STRONG                 { return KEY_STRONG; }
TRANSITION             { return KEY_TRANSITION; }
WEAK                   { return KEY_WEAK; }

\:                     { return COLON; }
,                      { return COMMA; }
\;                     { return SEMICOLON; }

[\n\r]                 { ptnetlola_colno = 1; /* whitespace */ }
[\t ]                  { /* whitespace */ }

[0-9]+                 { ptnetlola_lval.attributeString = strdup(ptnetlola_text); return NUMBER; }
[^,;:()\t \n\r\{\}]+   { ptnetlola_lval.attributeString = strdup(ptnetlola_text); return IDENTIFIER; }

.                      { ptnetlola_error("lexical error"); }

%%

/*!
\brief handler for EOF

When the scanner receives an end-of-file indication from YY_INPUT, it then
checks the `yywrap()' function. If `yywrap()' returns false (zero), then it
is assumed that the function has gone ahead and set up yyin to point to another
input file, and scanning continues. If it returns true (non-zero), then the
scanner terminates, returning 0 to its caller. Note that in either case, the
start condition remains unchanged; it does not revert to INITIAL.

\return 0 (a new file was opened and parsing continues) or 1 (parsing complete)
*/
int ptnetlola_wrap()
{
    extern Input *currentInputFile;
    extern gengetopt_args_info args_info;
    extern int currentFile;

    if (currentFile == (int)args_info.inputs_num-1 or currentFile == -1)
    {
        // last or only file parsed - done parsing
        return 1;
    }
    else
    {
        // open next file
        delete currentInputFile;
        currentInputFile = new Input("net", args_info.inputs[++currentFile]);

        ptnetlola_colno = 1;
        ptnetlola_lineno = 1;
        return 0;
    }
}
