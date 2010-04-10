 /*****************************************************************************
  * flex options
  ****************************************************************************/

/* create a c++ lexer */
%option c++

/* we provide out own class */
%option yyclass="pnapi::parser::sa::yy::Lexer"

/* we need to prefix its base class */
%option prefix="Sa"

/* created lexer should be called "lex.yy.c" to make the ylwrap script work */
%option outfile="lex.yy.c"

/* we read only one file */
%option noyywrap

/* yyunput not needed (fix compiler warning) */
%option nounput

/* maintain line number for error reporting */
%option yylineno

/* get rid of isatty */
%option never-interactive


 /*****************************************************************************
  * C declarations
  ****************************************************************************/
%{

#include "config.h"

#include "parser-sa-wrapper.h"

#include <iostream>

/* tokens are defined in a struct in a class */
typedef pnapi::parser::sa::yy::BisonParser::token tt;

%}


 /*****************************************************************************
  * regular expressions
  ****************************************************************************/

%s COMMENT

whitespace     [\n\r\t ]
identifier     [^,;:()\t \n\r\{\}=]+
number         [0-9]+


%%


"{"                                     { BEGIN(COMMENT);              }
<COMMENT>"}"                            { BEGIN(INITIAL);              }
<COMMENT>[^}]*                          { /* do nothing */             }

"NODES"                                 { return tt::KEY_NODES;            }
"INITIAL"                               { return tt::KEY_INITIAL;          }
"FINAL"                                 { return tt::KEY_FINAL;            }

"INTERFACE"                             { return tt::KEY_INTERFACE;        }
"INPUT"                                 { return tt::KEY_INPUT;            }
"OUTPUT"                                { return tt::KEY_OUTPUT;           }
"SYNCHRONOUS"                           { return tt::KEY_SYNCHRONOUS;      }

":"                                     { return tt::COLON;                }
";"                                     { return tt::SEMICOLON;            }
","                                     { return tt::COMMA;                }
"->"                                    { return tt::ARROW;                }

{number}     { yylval->yt_int = atoi(yytext); return tt::NUMBER;             }
{identifier} { yylval->yt_str = strdup(yytext); return tt::IDENT;}

{whitespace}                            { /* do nothing */             }

.                                       { LexerError("lexical error"); }
