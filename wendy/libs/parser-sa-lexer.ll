 /*****************************************************************************
  * flex options
  ****************************************************************************/

/* created lexer should be called "lex.yy.c" to make the ylwrap script work */
%option outfile="lex.yy.c"

/* plain c scanner: the prefix is our "namespace" */
%option prefix="pnapi_sa_"

/* we read only one file */
%option noyywrap

/* yyunput not needed (fix compiler warning) */
%option nounput

/* maintain line number for error reporting */
%option yylineno


 /*****************************************************************************
  * C declarations
  ****************************************************************************/
%{

#include <iostream>
using std::cerr;
using std::endl;

#include "parser.h"
#include "parser-sa.h"

#define yystream pnapi::parser::stream
#define yylineno pnapi::parser::line
#define yytext   pnapi::parser::token
#define yyerror  pnapi::parser::error

#define yylex    pnapi::parser::sa::lex

/* hack to read input from a C++ stream */
#define YY_INPUT(buf,result,max_size)   \
   yystream->read(buf, max_size); \
   if (yystream->bad()) \
     YY_FATAL_ERROR("input in flex scanner failed"); \
   result = yystream->gcount();

/* hack to overwrite YY_FATAL_ERROR behavior */
#define fprintf(file,fmt,msg) \
   yyerror(msg);

%}


 /*****************************************************************************
  * regular expressions
  ****************************************************************************/


 /* a start condition to skip comments */
%s COMMENT

%%

 /* comments */
"{"                         { BEGIN(COMMENT); }
<COMMENT>"}"                { BEGIN(INITIAL); }
<COMMENT>[^}]*              { /* skip */      }

 /* interface part of SA format */
"INTERFACE"                 { return KEY_INTERFACE; }
"INPUT"                     { return KEY_INPUT;     }
"OUTPUT"                    { return KEY_OUTPUT;    }

 /* nodes part of SA format */
"NODES"                     { return KEY_NODES;     }
"TAU"                       { return KEY_TAU;       }
"FINAL"                     { return KEY_FINAL;     }
"INITIAL"                   { return KEY_INITIAL;  }

 /* signs */
","                         { return COMMA;     }
":"                         { return COLON;     }
";"                         { return SEMICOLON; }
"->"                        { return ARROW;     }

 /* identifiers */
[0-9][0-9]*                 {
                pnapi_sa_lval.yt_int = atoi(yytext); return NUMBER;
                            }
[^,;:()\t \n\r\{\}][^,;:()\t \n\r\{\}]*  {
                pnapi_sa_lval.yt_string = (std::string *)(new std::string(yytext)); return IDENT;
                            }
                            
[\t \n\r]*                  { /* remove whitespaces */ }

 /* anything else */
.                           { yyerror("unexpected lexical token"); }

