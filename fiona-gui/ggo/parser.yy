
%output="parser.c"
%token-table

%defines

%{

#include <iostream>
#include <cstdlib>
#include <string>

using std::cout;
using std::endl;
using std::string;

string STRING_token;
char SHORT_token;

bool isString = true;

extern int yylex();

extern int yyerror(const string &);
extern int yyerror(const char *);

%}

%token KEY_PACKAGE
%token KEY_VERSION
%token KEY_PURPOSE
%token KEY_USAGE
%token KEY_DESCRIPTION
%token KEY_ARGS
%token KEY_OPTION
%token KEY_FLAG
%token KEY_SECTION
%token KEY_SECTIONDESC
%token KEY_TEXT

%token KEY_TYPESTR
%token KEY_REQUIRED
%token KEY_OPTIONAL
%token KEY_DEPENDON
%token KEY_DEFAULT
%token KEY_HIDDEN
%token KEY_ARGOPTIONAL
%token KEY_DETAILS
%token KEY_VALUES
%token KEY_MULTIPLE

%token KEY_STRING
%token KEY_INT
%token KEY_SHORT
%token KEY_LONG
%token KEY_FLOAT
%token KEY_DOUBLE
%token KEY_LONGDOUBLE
%token KEY_LONGLONG

%token KEY_ON
%token KEY_OFF

%token ASSIGN
%token COMMA
%token CONTROL

%token STRING
%token SHORT

%start input

%%

input:
  /* empty */
| input statement
;

statement:
  KEY_PACKAGE STRING
  {
    cout << "package \"" << STRING_token << "\"" << endl;
  }
| KEY_VERSION STRING
  {
    cout << "version \"" << STRING_token << "\"" << endl;
  }
| KEY_PURPOSE STRING
  {
    cout << "purpose \"" << STRING_token << "\"" << endl;
  }
| KEY_USAGE STRING
  {
    cout << "usage \"" << STRING_token << "\"" << endl;
  }
| KEY_DESCRIPTION STRING
  {
    cout << "description \"" << STRING_token << "\"" << endl;
  }
| KEY_ARGS STRING
  {
    cout << "args \"" << STRING_token << "\"" << endl;
  }
| KEY_OPTION STRING { cout << "option \"" << STRING_token; } SHORT STRING 
  { cout << "\" " << SHORT_token << endl << "\"" << STRING_token 
         << "\"" << endl; } opts
| KEY_SECTION STRING
  {
    cout << "section \"" << STRING_token << "\"" << endl;
  }
| KEY_SECTION STRING { cout << "section \"" << STRING_token << "\" "; } KEY_SECTIONDESC ASSIGN STRING { cout << "sectiondesc=\"" << STRING_token << "\"" << endl; }
| KEY_TEXT STRING
  {
    cout << "text \"" << STRING_token << "\"" << endl;
  }
| CONTROL KEY_SECTION STRING
  {
    cout << "#! section \"" << STRING_token << "\"" << endl;
  }
;

opts:
  /* empty */
| opts KEY_FLAG onoff
| opts KEY_DETAILS ASSIGN STRING
| opts argtype
| opts KEY_TYPESTR ASSIGN STRING
| opts KEY_VALUES ASSIGN values
| opts KEY_DEFAULT ASSIGN STRING
| opts KEY_DEPENDON ASSIGN STRING
| opts required
| opts KEY_ARGOPTIONAL
| opts KEY_MULTIPLE
| opts KEY_HIDDEN
;

argtype:
  KEY_STRING
  { cout << "string" << endl; }
| KEY_INT
  { cout << "int" << endl; }
| KEY_SHORT
  { cout << "short" << endl; }
| KEY_LONG
  { cout << "long" << endl; }
| KEY_FLOAT
  { cout << "float" << endl; }
| KEY_DOUBLE
  { cout << "double" << endl; }
| KEY_LONGDOUBLE
  { cout << "longdouble" << endl; }
| KEY_LONGLONG
  { cout << "longlong" << endl; }
;

values:
  STRING
| values COMMA STRING
;

onoff:
  KEY_ON
| KEY_OFF
;

required:
  KEY_REQUIRED
| KEY_OPTIONAL
;

%%

int main()
{
  yyparse();
  
  return EXIT_SUCCESS;
}

