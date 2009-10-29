
%token-table

%defines

%{

#include "option.h"
#include "tool.h"
#include "tool_syntax.h"
#include <cstdlib>
#include <iostream>
#include <QtCore/QString>

using std::cout;
using std::endl;

/* program variables */
extern Tool * t;

/* parser variables */
char * section;
Option * o;

extern int yylex();
extern int yyerror(const char *);

%}

%union
{
  char * long_;
  char short_;
}

%token SSHORT
%token SSTRING

%type<short_> SSHORT
%type<long_> SSTRING

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
%token CONTROL_IN
%token CONTROL_OUT

%start input

%%

input:
  /* empty */
| input statement
;

statement:
  KEY_PACKAGE SSTRING
  {
    t->setPackage(QString($2));
  }
| KEY_VERSION SSTRING
  {
    t->setVersion(QString($2));
  }
| KEY_PURPOSE SSTRING
  {
    t->setPurpose(QString($2));
  }
| KEY_USAGE SSTRING
  {
    t->setUsage(QString($2));
  }
| KEY_DESCRIPTION SSTRING
  {
    t->setDescription(QString($2));
  }
| KEY_ARGS SSTRING
| KEY_OPTION { o = new Option(); } SSTRING SSHORT SSTRING opts
  {
    o->long_ = QString($3);
    o->short_ = QString($4);
    o->description_ = QString($5);
  	if (section == "")
  	  t->addOption(o);
  	else
  	  t->addOption(QString(section), o);
  }
| KEY_SECTION SSTRING
  {
    section = $2;
  }
| KEY_SECTION SSTRING KEY_SECTIONDESC ASSIGN SSTRING 
  { 
    section = $2;
    t->mapSection(QString(section), QString($5));
  }
| KEY_TEXT SSTRING
  {
    t->setText(QString($2));
  }
;

opts:
  /* empty */
| opts KEY_FLAG onoff
  { 
    o->flagGiven_ = true; 
  }
| opts KEY_DETAILS ASSIGN SSTRING
  {
    o->details_ = QString($4);
  }
| opts argtype
| opts KEY_TYPESTR ASSIGN SSTRING
  {
    o->typeString_ = QString($4);
  }
| opts KEY_VALUES ASSIGN values
| opts KEY_DEFAULT ASSIGN SSTRING
  {
    o->default_ = QString($4);
  }
| opts KEY_DEPENDON ASSIGN SSTRING
  {
    o->dependon_ = QString($4);
  }
| opts required
| opts KEY_ARGOPTIONAL
  {
    o->argoptional_ = true;
  }
| opts KEY_MULTIPLE
  {
    o->multiple_ = true;
  }
| opts KEY_HIDDEN
  {
    o->hidden_ = true; 
  }
| opts CONTROL_IN
  {
    o->type_ = INPUT;
  }
| opts CONTROL_OUT
  {
    o->type_ = OUTPUT;
  }
;

argtype:
  KEY_STRING
| KEY_INT
| KEY_SHORT
| KEY_LONG
| KEY_FLOAT
| KEY_DOUBLE
| KEY_LONGDOUBLE
| KEY_LONGLONG
;

values:
  SSTRING
  {
    o->values_.push_back(QString($1));
  }
| values COMMA SSTRING
  {
    o->values_.push_back(QString($3));
  }
;

onoff:
  KEY_ON
  {
    o->flag_ = true; 
  }
| KEY_OFF
  {
    o->flag_ = false; 
  }
;

required:
  KEY_REQUIRED
  {
    o->required_ = true; 
  }
| KEY_OPTIONAL
  {
    o->required_ = false; 
  }
;
