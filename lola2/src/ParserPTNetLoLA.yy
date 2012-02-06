/*!
\author Karsten
\file ParserPTNetLoLA.yy
\status approved 25.01.2012

\todo Fehlermeldungen auch über den Reporter abhandeln.
\todo Detaillierte Dateifehlermeldungen abschaltbar.
\todo TheResult und TheCapacity in einen Namespace "parser::" packen

Parses a place transition net in LoLA syntax.
*/

%{
#include <config.h>
#include <limits.h>
#include <cstdarg>
#include <cstdio>
#include "PlaceSymbol.h"
#include "TransitionSymbol.h"
#include "SymbolTable.h"
#include "ParserPTNet.h"
#include "FairnessAssumptions.h"
#include "ArcList.h"
#include "Dimensions.h"
#include "Reporter.h"

extern Reporter *rep;

/// the current token text from Flex
extern char* yytext;

void yyerror(char const*);
void yyerrors(char* token, char const* format, ...);
%}

%union {
    char* attributeString;
    fairnessAssumption_t attributeFairness;
    ArcList* attributeArcList;
}

%type <attributeString> nodeident
%type <attributeFairness> fairness
%type <attributeArcList> arclist
%type <attributeArcList> arc
%type <attributeString> NUMBER
%type <attributeString> IDENTIFIER

%error-verbose /* more verbose and specific error message string */
%defines       /* write an output file containing macro definitions for the token types */

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
extern FILE* yyin;
extern int yylineno;
extern int yycolno;
%}

%{
// This list contains a few global variables. Their purpose is to
// propagate semantic values top down or left to right which is
// impossible with the attributes of bison. Thee variables must be initialized
// whenever this parser in invoked. After termination of the parser,
// their values become meaningless.

/// The object containing the final outcome of the parsing process
ParserPTNet*  TheResult;
/// The value of the currently active capacity statement
capacity_t TheCapacity;
%}

%%

net:
  _PLACE_ placelists _semicolon_    /* declare places */
  _MARKING_ markinglist _semicolon_ /* specify initial marking */
  transitionlist                    /* define transitions & arcs */
;


placelists:
  capacity placelist    /* several places may share unqiue capacity */
| placelists _semicolon_ capacity placelist
;


capacity:
  /* empty */            /* empty capacity = unlimited capacity */
    {
        TheCapacity = (capacity_t)MAX_CAPACITY;
    }
| _SAFE_ _colon_       /* SAFE without number = 1-SAFE */
    {
        TheCapacity = 1;
    }
| _SAFE_ NUMBER _colon_ /* at most k tokens expected on these places */
    {
        TheCapacity = (capacity_t)atoi($2);
        free($2);
    }
;


placelist:
  placelist _comma_ nodeident
    {
        PlaceSymbol* p = new PlaceSymbol($3, TheCapacity);
        if (UNLIKELY (! TheResult->PlaceTable->insert(p)))
        {
            yyerrors($3, "place '%s' name used twice", $3);
        }
    }
| nodeident
    {
        PlaceSymbol* p = new PlaceSymbol($1,TheCapacity);
        if (UNLIKELY (! TheResult->PlaceTable->insert(p)))
        {
            yyerrors($1, "place '%s' name used twice", $1);
        }
    }
;

nodeident:  /* for places and transitions, names may be idents or numbers */
  IDENTIFIER
    {
        $$ = $1;
    }
| NUMBER
    {
        $$ = $1; /* result is string version of number */
    }
;


markinglist:
  /* empty */
| marking
| markinglist _comma_ marking
;


marking:
  nodeident _colon_ NUMBER
    {
        PlaceSymbol* p = reinterpret_cast<PlaceSymbol*>(TheResult->PlaceTable->lookup($1));
        if (UNLIKELY (!p))
        {
            yyerrors($1, "place '%s' does not exist", $1);
        }
        p -> addInitialMarking((capacity_t)atoi($3));
        free($3);
        free($1);
    }
| nodeident  /* default: 1 token */
    {
        PlaceSymbol* p = reinterpret_cast<PlaceSymbol*>(TheResult->PlaceTable->lookup($1));
        if (UNLIKELY (!p))
        {
            yyerrors($1, "place '%s' does not exist", $1);
        }
        p -> addInitialMarking(1);
        free($1);
    }
;


transitionlist:
  transition
| transitionlist transition
;


transition:
  _TRANSITION_ nodeident fairness
  _CONSUME_ arclist _semicolon_
  _PRODUCE_ arclist _semicolon_
    {
        TransitionSymbol* t = new TransitionSymbol($2, $3, $5, $8);
        if (UNLIKELY (! TheResult->TransitionTable->insert(t)))
        {
            yyerrors($2, "transition name '%s' used twice", $2);
        }
    }
;

fairness:
    /* empty */    /* empty = may be treated unfair */
    {
        $$ = NO_FAIRNESS;
    }
| _WEAK_ _FAIR_
    {
        $$ = WEAK_FAIRNESS;
    }
| _STRONG_ _FAIR_
    {
        $$ = STRONG_FAIRNESS;
    }
;


arclist:
  /* empty */
    {
        $$ = NULL;
    }
| arc
    {
        $$ = $1;
    }
| arc _comma_ arclist
    {
    // check for duplicate
    ArcList * al;
    for(al = $3; al; al = al -> getNext())
    {
        if(al -> getPlace() == $1 -> getPlace())
        {
            break;
        }
    }
    if(al)
    {   
        //duplicate detected
        al -> addMultiplicity($1 -> getMultiplicity());
        delete $1;
        $$ = $3;
    }
    else
    {
        // no duplicate detected
        $1->setNext((Symbol*)$3);
        $$ = $1;
    }
    
    }
;


arc:
  nodeident _colon_ NUMBER
    {
        PlaceSymbol* p = reinterpret_cast<PlaceSymbol*>(TheResult->PlaceTable->lookup($1));
        if (UNLIKELY (!p))
        {
            yyerrors($1, "place '%s' does not exist", $1);
        }
        $$ = new ArcList(p, (mult_t)atoi($3));
        free($3);
        free($1);
    }
| nodeident   /* default: multiplicity 1 */
    {
        PlaceSymbol* p = reinterpret_cast<PlaceSymbol*>(TheResult->PlaceTable->lookup($1));
        if (UNLIKELY (!p))
        {
            yyerrors($1, "place '%s' does not exist", $1);
        }
        $$ = new ArcList(p, 1);
        free($1);
    }
;

%%

/// Wrapping the Parser
ParserPTNet* ParserPTNetLoLA()
{
    TheResult = new ParserPTNet();
    yyparse();
    return(TheResult);
}

/// display a parser error and exit
void yyerrors(char* token, char const* format, ...) {
    va_list args;
    va_start(args, format);
    rep->status(format, args);
    va_end(args);

    rep->status("%d:%d - error near '%s'", yylineno, yycolno, token);

    rep->abort(ERROR_SYNTAX);
}


/// display a parser error and exit
void yyerror(char const* mess) {
    yyerrors(yytext, mess);
}
