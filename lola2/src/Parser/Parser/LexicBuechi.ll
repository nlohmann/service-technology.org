/*!
\file
\brief lexic for Büchi automata
\author Erik
\status not approved
\ingroup g_frontend

\todo Herausfinden, ob es Probleme bei zu langen Kommentaren/Bezeichnern gibt. Idee: Maximallänge angeben.
\todo Präfix hinzufügen?
*/

/* yylineno: we want line numbering
   nounput: we don't need yyunput()
   noyywrap: we don't support multiple formula files */
%option yylineno
%option nounput
%option noyywrap
%option outfile="lex.yy.c"
%option prefix="ptbuechi_"

%{
#include <Parser/Parser/ast-system-k.h>
#include <Parser/Parser/ast-system-yystype.h>
#include <Parser/Parser/ParserPTBuechi.hh>

extern void ptbuechi_error(char const* mess);
unsigned int ptbuechi_colno = 1;

/*!
\brief This macro is executed prior to the matched rule's action.

We use this macro to set set #ptbuechi_lloc to the positions of #ptbuechi_text. It further
manages the current column number #ptbuechi_colno. See Flex's manual
http://flex.sourceforge.net/manual/Misc-Macros.html for more information on
the macro.
*/
#define YY_USER_ACTION \
  ptbuechi_lloc.first_line = ptbuechi_lloc.last_line = ptbuechi_lineno; \
  ptbuechi_lloc.first_column = ptbuechi_colno; \
  ptbuechi_lloc.last_column = ptbuechi_colno+ptbuechi_leng-1; \
  ptbuechi_colno += ptbuechi_leng;
%}

%s IN_COMMENT

%%

 /* from http://flex.sourceforge.net/manual/How-can-I-match-C_002dstyle-comments_003f.html */
"/*"                   { BEGIN(IN_COMMENT); }
<IN_COMMENT>"*/"       { BEGIN(INITIAL); }
<IN_COMMENT>[^*\n\r]+  { /* comments */ }
<IN_COMMENT>"*"        { /* comments */ }
<IN_COMMENT>[\n\r]     { /* comments */ }

FIREABLE               { return _FIREABLE_; }
DEADLOCK               { return _DEADLOCK_; }
INITIAL                { return _INITIAL_; }

buechi                 { return _buechi_; }
accept                 { return _accept_; }
AND                    { return _AND_; }
NOT                    { return _NOT_; }
OR                     { return _OR_; }
XOR                    { return _XOR_; }
TRUE                   { return _TRUE_; }
FALSE                  { return _FALSE_; }

\:                     { return _colon_; }
\,                     { return _comma_; }
=\>                    { return _then_; }
\;                     { return _semicolon_; }
\<\-\>                 { return _iff_; }
!=                     { return _notequal_; }
\<\>                   { return _notequal_; }
\-\>                   { return _implies_; }
=                      { return _equals_; }
\+                     { return _plus_; }
\-                     { return _minus_; }
\*                     { return _times_; }
\(                     { return _leftparenthesis_; }
\)                     { return _rightparenthesis_; }
\{                     { return _braceleft_; }
\}                     { return _braceright_; }
[>]                    { return _greaterthan_; }
[<]                    { return _lessthan_; }
[#]                    { return _notequal_; }
[>]=                   { return _greaterorequal_; }
[<]=                   { return _lessorequal_; }

[\n\r]                 { ptbuechi_colno = 1; /* whitespace */ }
[\t ]                  {  /* whitespace */ }

[0-9]+                 { ptbuechi_lval.yt_integer = kc::mkinteger(atoi(ptbuechi_text)); return NUMBER; }
[^,;:()\t \n\r\{\}]+   { ptbuechi_lval.yt_casestring = kc::mkcasestring(ptbuechi_text); return IDENTIFIER; }
