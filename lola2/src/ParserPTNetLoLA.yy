/*!

\author Karsten
\file ParserPTNetLoLA.yy
\status new

Parses a place transition net in LoLA syntax.
*/

%{
#include "Symbol.h"
#include "SymbolTable.h"
#include <string>

using std::string;

void yyerror(char const*);
void yyerrors(char* token, char const* format, ...);
%}

%union {
}


///// 4 LINES ADDED BY NIELS
%error-verbose
%token_table
%defines
%yacc

%token _FINAL_ _AUTOMATON_ _SAFE_ _NEXT_ _ANALYSE_ _PLACE_ _MARKING_
_TRANSITION_ _CONSUME_ _PRODUCE_ _comma_ _colon_ _semicolon_ IDENTIFIER NUMBER
_equals_ _AND_ _OR_ _EXPATH_ _ALLPATH_ _ALWAYS_ _EVENTUALLY_ _UNTIL_ _NOT_
_greaterorequal_ _greaterthan_ _lessorequal_ _lessthan_ _notequal_ _FORMULA_
_leftparenthesis_ _rightparenthesis_ _STATE_ _PATH_ _GENERATOR_ _RECORD_ _END_
_SORT_ _FUNCTION_ _DO_ _ARRAY_ _ENUMERATE_ _CONSTANT_ _BOOLEAN_ _OF_ _BEGIN_
_WHILE_ _IF_ _THEN_ _ELSE_ _SWITCH_ _CASE_ _REPEAT_ _FOR_ _TO_ _ALL_ _EXIT_
_RETURN_ _TRUE_ _FALSE_ _MOD_ _VAR_ _GUARD_ _iff_ _implies_ _leftbracket_
_rightbracket_ _dot_ _plus_ _minus_ _times_ _divide_ _slash_ _EXISTS_ _STRONG_
_WEAK_ _FAIR_

%{
extern YYSTYPE yylval;
extern int yylex();
extern FILE *yyin;
extern int yylineno;
extern int yycolno;
%}

%%

net:
  _PLACE_ placelists _semicolon_  /* declare places */
  _MARKING_ markinglist _semicolon_ /* specify initial marking */
  transitionlist                    /* define transitions & arcs */
;


placelists:
  capacity placelist    /* several places may share unqiue capacity */ 
| placelists _semicolon_ capacity placelist
;


capacity:
  /* empty */          /* empty capacity = unlimited capacity */ 
| _SAFE_ _colon_       /* SAFE without number = 1-SAFE */
| _SAFE_ NUMBER _colon_ /* at most k tokens expected on these places */
;


placelist:
  placelist _comma_ place 
| place 
;


place:   /* name of a place, expected not to be present yet */
  nodeident 
;


nodeident:  /* for places and transitions, names may be idents or numbers */
  IDENTIFIER  
| NUMBER 
;


markinglist:
  /* empty */
| marking
| markinglist _comma_ marking
;


marking:
  nodeident _colon_ NUMBER 
;


transitionlist:
  transition
| transitionlist transition
;


transition:
  _TRANSITION_ tname fairness
  _CONSUME_ arclist _semicolon_ 
  _PRODUCE_ arclist _semicolon_ 
;

fairness:
	/* empty */    /* empty = may be treated unfair */
| _WEAK_ _FAIR_ 
| _STRONG_ _FAIR_ 
;


tname:    /* name of transition, expected not tp be present yet */
	nodeident
;

arclist:
  /* empty */       
| arc              
| arc _comma_ arclist 
;


arc:
  placereference _colon_ NUMBER 
;

placereference:   /* name of a place, expected to be present */
	nodeident
;
%%

/// display a parser error and exit
void yyerrors(char* token, char const* format, ...) {
    fprintf(stderr, "%s: %s:%d:%d - ", _ctool_(PACKAGE),
    _cfilename_(basename(diagnosefilename)), yylineno, yycolno);

    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);

    fprintf(stderr, "\n");
    message("error near '%s'", token);
    displayFileError(diagnosefilename, yylineno, token);
    abort(3, "syntax error");
}


/// display a parser error and exit
void yyerror(char const* mess) {
    yyerrors(yytext, mess);
}
