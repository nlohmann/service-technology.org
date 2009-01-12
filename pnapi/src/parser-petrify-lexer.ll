 // -*- C++ -*-

%option prefix="pnapi_petrify_"
%option noyywrap
%option nounput
%option yylineno


%{
#include <istream>
#include "parser.h"
#include "parser-petrify.h"

#define pnapi_yyistream pnapi_petrify_istream

#define YY_FATAL_ERROR(msg) \
   throw msg;

#define YY_INPUT(buf,result,max_size) \
   pnapi_yyistream->read(buf, max_size); \
   if (pnapi_yyistream->bad()) \
     YY_FATAL_ERROR("input in flex scanner failed"); \
   result = pnapi_yyistream->gcount();

extern int pnapi_petrify_error(const char *);

// global variable for stream pointer
std::istream * pnapi_yyistream = NULL;
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

.             { pnapi_petrify_error("lexical error"); }
