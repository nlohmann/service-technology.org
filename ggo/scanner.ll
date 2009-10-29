
%option yylineno
%option outfile="scanner.c"
%option noyywrap

%{

#include "parser.h"
#include <iostream>
#include <string>

using std::cerr;
using std::endl;
using std::string;

extern string STRING_token;
extern string LONG_token;
extern char SHORT_token;

extern bool isString;

int yyerror(const string &);
int yyerror(const char *);

%}

%s STRING_STATE

string        [^#\"]*
short         [a-zA-Z0-9\-^\"#]

%%

"package"               { return KEY_PACKAGE; }
"version"               { return KEY_VERSION; }
"purpose"               { return KEY_PURPOSE; }
"usage"                 { return KEY_USAGE; }
"description"           { return KEY_DESCRIPTION; }
"args"                  { return KEY_ARGS; }
"option"                { return KEY_OPTION; }
"flag"                  { return KEY_FLAG; }
"section"               { return KEY_SECTION; }
"sectiondesc"           { return KEY_SECTIONDESC; }
"text"                  { return KEY_TEXT; }

"typestr"               { return KEY_TYPESTR; }
"required"              { return KEY_REQUIRED; }
"optional"              { return KEY_OPTIONAL; }
"dependon"              { return KEY_DEPENDON; }
"default"               { return KEY_DEFAULT; }
"hidden"                { return KEY_HIDDEN; }
"argoptional"           { return KEY_ARGOPTIONAL; }
"details"               { return KEY_DETAILS; }
"values"                { return KEY_VALUES; }
"multiple"              { return KEY_MULTIPLE; }

"string"                { return KEY_STRING; }
"int"                   { return KEY_INT; }
"short"                 { return KEY_SHORT; }
"long"                  { return KEY_LONG; }
"float"                 { return KEY_FLOAT; }
"double"                { return KEY_DOUBLE; }
"longdouble"            { return KEY_LONGDOUBLE; }
"longlong"              { return KEY_LONGLONG; }

"on"                    { return KEY_ON; }
"off"                   { return KEY_OFF; }

"="                     { return ASSIGN; }
","                     { return COMMA; }
"#!"					{ return CONTROL; }

<INITIAL>"\""           { BEGIN(STRING_STATE); }
<STRING_STATE>"\""      { BEGIN(INITIAL); }

<STRING_STATE>{string}      {
                              STRING_token = yytext;
                              return STRING;
                            }
                            
{short}                     { 
                              SHORT_token = yytext[0]; 
                              return SHORT;
                            }
                            
[\n \t\r]+                  { /* skip */ }
                            
.                           { yyerror("lexical error"); }



%%

int yyerror(const string & msg)
{
  cerr << msg << " near '" << yytext << "' on line " << yylineno << endl;
}

int yyerror(const char * msg)
{
  cerr << msg << " near '" << yytext << "' on line " << yylineno << endl;
}

