/*****************************************************************************\
 IG2SA -- compiling Fiona IGs to Wendy SAs

 Copyright (C) 2009  Christian Sura <christian.sura@uni-rostock.de>

 IG2SA is free software: you can redistribute it and/or modify it under the
 terms of the GNU Affero General Public License as published by the Free
 Software Foundation, either version 3 of the License, or (at your option)
 any later version.

 IG2SA is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for
 more details.

 You should have received a copy of the GNU Affero General Public License
 along with IG2SA.  If not, see <http://www.gnu.org/licenses/>. 
\*****************************************************************************/

%option outfile="lex.yy.c"
%option noyywrap
%option yylineno
%option nodefault
%option nounput

%{
#include <cstring>
#include <iostream>
#include <set>
#include <string>
#include "syntax-ig2sa.h"
#include "config.h"

int yyerror(char const*);

extern std::ostream * myOut;

extern std::set<std::string> synchronous;

%}

whitespace     [\n\r\t ]
identifier     [^,;:!?#()\t \n\r\{\}=]+
number         [0-9]+

%s COMMENT
%s NODES
%s ANNOTATION
%s SYNCH


%%


"{"                                     { (*myOut) << "{"; BEGIN(COMMENT);      }
<COMMENT>"}"                            { (*myOut) << "}\n\n"; BEGIN(INITIAL);      }
<COMMENT>[^}]*                          { (*myOut) << yytext; /* copy comment */  }

"INTERFACE"    { return K_INTERFACE; }
"INPUT"        { return K_INPUT; }
"OUTPUT"       { return K_OUTPUT; }
"NODES"        { BEGIN(NODES); return K_NODES; }

<NODES>{number}          { yylval.value = atoi(yytext); BEGIN(ANNOTATION); return NUMBER; }
<ANNOTATION>"finalnode"  { return K_FINAL; }
<ANNOTATION>","          { BEGIN(NODES); return COMMA; }
<ANNOTATION>"#"          { BEGIN(SYNCH); }
<SYNCH>{identifier}      { synchronous.insert(yytext); BEGIN(ANNOTATION); }
<ANNOTATION>[^#,; \t]*   { /* ignore */ }
<ANNOTATION>[ \t]*       { /* hack to identify "finalnode" */ } 
<ANNOTATION>";"               { BEGIN(INITIAL); return SEMICOLON; }

"INITIALNODE"  { return K_INITIALNODE; }
"TRANSITIONS"  { return K_TRANSITIONS; }

","            { return COMMA; }
";"            { return SEMICOLON; }
":"            { return COLON; }
"!"            { return SEND; }
"?"            { return RECEIVE; }
"#"            { return SYNCHRONOUS; }
"+"            { return OP_OR; }
"*"            { return OP_AND; }
"->"           { return ARROW; }

{number}       { yylval.value = atoi(yytext); return NUMBER; }
<INITIAL>{identifier}   { yylval.str = strdup(yytext); return IDENT; }

{whitespace}                            { /* do nothing */             }

.                                       { yyerror("lexical error"); }


%%


int yyerror(char const *msg) {
    std::cerr << PACKAGE << ": " << yylineno
              << ": ERROR near '" << yytext 
              << "': " << msg << std::endl;
    exit(EXIT_FAILURE);
}

