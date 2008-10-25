/*****************************************************************************\
 Copyright (c) 2008  Robert Danitz, Christian Gierds, Niels Lohmann,
                     Peter Massuthe, Richard Mu"ller, Janine Ott,
                     Jan Su"rmeli, Daniela Weinberg, Karsten Wolf,
                     Martin Znamirowski

 Copyright (c) 2007  Jan Bretschneider, Robert Danitz, Christian Gierds,
                     Kathrin Kaschner, Leonard Kern, Niels Lohmann,
                     Peter Massuthe, Daniela Weinberg, Karsten Wolf

 Copyright (c) 2006  Kathrin Kaschner, Peter Massuthe, Daniela Weinberg,
                     Karsten Wolf

 This file is part of Fiona, see <http://service-technology.org/fiona>.

 Fiona is free software; you can redistribute it and/or modify it under the
 terms of the GNU General Public License as published by the Free Software
 Foundation; either version 3 of the License, or (at your option) any later
 version.
 
 Fiona is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License along with
 Fiona (see file COPYING). If not, see <http://www.gnu.org/licenses/>.
\*****************************************************************************/

/* dot.lex */
%option outfile="lex.yy.c"
%option prefix="dot_yy"
%option noyywrap
%option yylineno
%option nodefault


%{
#include "dot2tex.h"
#include "syntax_dot.h"
int dot_yyerror(char const *s);
%}

alpha       [a-zA-Z_]
digit       [0-9]
str         {alpha}({alpha}|{digit})*

number      -?("."{digit}+)|({digit}+("."{digit}*)?)

dq_str      (\"[^\"]*\")

html_str    "<"{str}*">"

id          {str}|{number}|{dq_str}|{html_str} 

%%

"digraph"   { dot_yylval.str_val = new std::string(dot_yytext); return KW_DIGRAPH; }
"graph"     { dot_yylval.str_val = new std::string(dot_yytext); return KW_GRAPH; }
"subgraph"  { dot_yylval.str_val = new std::string(dot_yytext); return KW_SUBGRAPH; }
"node "      { dot_yylval.str_val = new std::string(dot_yytext); return KW_NODE; }
"edge"      { dot_yylval.str_val = new std::string(dot_yytext); return KW_EDGE; }
"strict"    { dot_yylval.str_val = new std::string(dot_yytext); return KW_STRICT; }
{id}        { dot_yylval.str_val = new std::string(dot_yytext); return KW_ID; }
"{"         { dot_yylval.str_val = new std::string(dot_yytext); return KW_LBRACE; }
"}"         { dot_yylval.str_val = new std::string(dot_yytext); return KW_RBRACE; }
"["         { dot_yylval.str_val = new std::string(dot_yytext); return KW_LBRACKET; }
"]"         { dot_yylval.str_val = new std::string(dot_yytext); return KW_RBRACKET; }
"("         { dot_yylval.str_val = new std::string(dot_yytext); return KW_LPARAN; }
")"         { dot_yylval.str_val = new std::string(dot_yytext); return KW_RPARAN; }
","         { dot_yylval.str_val = new std::string(dot_yytext); return KW_COMMA; }
";"         { dot_yylval.str_val = new std::string(dot_yytext); return KW_SEMICOLON; }
":"         { dot_yylval.str_val = new std::string(dot_yytext); return KW_COLON; }
"="         { dot_yylval.str_val = new std::string(dot_yytext); return KW_EQUAL; }
"@"         { dot_yylval.str_val = new std::string(dot_yytext); return KW_AT; }
"->"        { dot_yylval.str_val = new std::string(dot_yytext); return KW_EDGEOP; }


[ \t]*      {}
[\n]        { /* skip */}

.           { std::cerr << "SCANNER "; dot_yyerror(""); exit(1);	}

