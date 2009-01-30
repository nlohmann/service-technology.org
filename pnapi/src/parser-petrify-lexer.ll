 // -*- C++ -*-

%option outfile="lex.yy.c"
%option prefix="pnapi_petrify_"
%option noyywrap
%option nounput
%option yylineno


%{
#include "parser.h"
#include "parser-petrify.h"

#define yylex pnapi::parser::petrify::lex
#define yyerror pnapi::parser::petrify::error
#define yystream pnapi::parser::petrify::stream

/* hack to read input from a C++ stream */
#define YY_INPUT(buf,result,max_size) \
   yystream->read(buf, max_size); \
   if (yystream->bad()) \
     YY_FATAL_ERROR("input in flex scanner failed"); \
   result = yystream->gcount();

/* hack to overwrite YY_FATAL_ERROR behavior */
#define fprintf(file,fmt,msg) \
   yyerror(msg);
%}

%s COMMENT

namestart		[A-Za-z\200-\377_]
namechar		[A-Za-z\200-\377_0-9/.:]
number			[0-9]+
placename		"p"{number}
name			{namestart}{namechar}*
transitionname1		"t"{number}("/"{number})?
transitionname2		"out."{name}
transitionname3		"in."{name}


%%
 /* RULES */

"#"             { BEGIN(COMMENT); }
<COMMENT>[\n\r] { BEGIN(INITIAL); }
<COMMENT>[^\n]* {                 }


".model"	{ return K_MODEL; }
".dummy"	{ return K_DUMMY; }
".graph"	{ return K_GRAPH; }
".marking"	{ return K_MARKING; }
".end"		{ return K_END; }

"{"		{ return OPENBRACE; }
"}"		{ return CLOSEBRACE; }

{placename}		{ pnapi_petrify_lval.str = strdup(yytext);
                          return PLACENAME; }

{transitionname1}	{ pnapi_petrify_lval.str = strdup(yytext);
                          return TRANSITIONNAME; }

{transitionname2}	{ pnapi_petrify_lval.str = strdup(yytext);
                          return TRANSITIONNAME; }

{transitionname3}	{ pnapi_petrify_lval.str = strdup(yytext);
                          return TRANSITIONNAME; }

"finalize"		{ pnapi_petrify_lval.str = strdup(yytext);
                          return TRANSITIONNAME; }

{name}			{ return IDENTIFIER; }

[\n\r]        { return NEWLINE; }

[ \t]         { break; }

.             { yyerror("lexical error"); }
