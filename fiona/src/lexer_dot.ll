/* dot.lex */
%option outfile="lex.yy.c"
%option prefix="dot_yy"
%option noyywrap
%option yylineno
%option nodefault


%{
#include "dot2tex.h"
#include "syntax_dot.h"
int dot_yyerror(char *s);

string buffer[max_lines];

int minx;
int maxx;
int miny;
int maxy;

%}

literal     [a-zA-Z]
digit       [0-9]
myint       ({digit})+
id          ({digit}|{literal})+
mystr       (\"[^\"]*\")

%%

"digraph"   { dot_yylval.str_val = new std::string(dot_yytext); return KW_DIGRAPH; }
"graph"     { dot_yylval.str_val = new std::string(dot_yytext); return KW_GRAPH; }
"node"      { dot_yylval.str_val = new std::string(dot_yytext); return KW_NODE; }
"edge"      { dot_yylval.str_val = new std::string(dot_yytext); return KW_EDGE; }
"strict"    { dot_yylval.str_val = new std::string(dot_yytext); return KW_STRICT; }
{myint}     { dot_yylval.str_val = new std::string(dot_yytext); return KW_INT; }
{id}        { dot_yylval.str_val = new std::string(dot_yytext); return KW_ID; }
{mystr}     { dot_yylval.str_val = new std::string(dot_yytext); return KW_STRING; }
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

[ \t]*		{}
[\n]		{ /* skip */}

.		    { std::cerr << "SCANNER "; dot_yyerror(""); exit(1);	}

