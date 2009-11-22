/*****************************************************************************\
 Fiona2wendy -- compiling Fiona OGs to Wendy OGs

 Copyright (C) 2009  Christian Sura <christian.sura@uni-rostock.de>

 Fiona2wendy is free software: you can redistribute it and/or modify it under the
 terms of the GNU Affero General Public License as published by the Free
 Software Foundation, either version 3 of the License, or (at your option)
 any later version.

 Fiona2wendy is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for
 more details.

 You should have received a copy of the GNU Affero General Public License
 along with Fiona2wendy.  If not, see <http://www.gnu.org/licenses/>. 
\*****************************************************************************/

%option outfile="lex.yy.c"
%option noyywrap
%option yylineno
%option nodefault
%option nounput

%{
#include <cstring>
#include <iostream>
#include "syntax-f2w.h"
#include "config.h"

int yyerror(char const*);

// needed for red/blue nodes
int colonCount;

extern std::ostream * myOut;

%}

whitespace     [\n\r\t ]
identifier     [^,;:!?#()\t \n\r\{\}=]+
number         [0-9]+

%s COMMENT


%%


"{"                                     { (*myOut) << "{"; BEGIN(COMMENT);      }
<COMMENT>"}"                            { (*myOut) << "}\n\n"; BEGIN(INITIAL);      }
<COMMENT>[^}]*                          { (*myOut) << yytext; /* copy comment */  }

"INTERFACE"    { return K_INTERFACE; }
"INPUT"        { return K_INPUT; }
"OUTPUT"       { return K_OUTPUT; }
"NODES"        { return K_NODES; }
"INITIALNODE"  { return K_INITIALNODE; }
"TRANSITIONS"  { return K_TRANSITIONS; }
"final"        { return K_FINAL; }
"true"         { return K_TRUE; }
"false"        { return K_FALSE; }

","            { colonCount = 0; return COMMA; }
";"            { colonCount = 0; return SEMICOLON; }
":"            { ++colonCount; return COLON; }
"!"            { return SEND; }
"?"            { return RECEIVE; }
"#"            { return SYNCHRONOUS; }
"("            { return LPAR; }
")"            { return RPAR; }
"+"            { return OP_OR; }
"*"            { return OP_AND; }
"->"           { return ARROW; }

{number}       { yylval.value = atoi(yytext); return NUMBER;  }
{identifier}   { if(colonCount < 2){yylval.str = strdup(yytext); return IDENT;} }

{whitespace}                            { /* do nothing */             }

.                                       { yyerror("lexical error"); }


%%


int yyerror(char const *msg) {
    std::cerr << PACKAGE << ": " << yylineno
              << ": ERROR near '" << yytext 
              << "': " << msg << std::endl;
    exit(EXIT_FAILURE);
}

