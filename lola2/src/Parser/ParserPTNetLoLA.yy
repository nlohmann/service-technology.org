/*!
\author Karsten
\file ParserPTNetLoLA.yy
\status approved 25.01.2012

\todo Detaillierte Dateifehlermeldungen abschaltbar.
\todo TheResult und TheCapacity in einen Namespace "parser::" packen

Parses a place transition net in LoLA syntax.
*/

%{
#include <config.h>
#include <limits.h>
#include <libgen.h>
#include <cstdarg>
#include <cstdio>
#include <Core/Dimensions.h>
#include <Parser/PlaceSymbol.h>
#include <Parser/TransitionSymbol.h>
#include <Parser/SymbolTable.h>
#include <Parser/ParserPTNet.h>
#include <Parser/FairnessAssumptions.h>
#include <Parser/ArcList.h>
#include <InputOutput/Reporter.h>
#include <InputOutput/InputOutput.h>

extern int currentFile;
extern Reporter* rep;
extern Input* netFile;

/// the current token text from Flex
extern char* ptnetlola_text;

void ptnetlola_error(char const*);
void yyerrors(char* token, const char* format, ...);
%}

%union {
    char* attributeString;
    fairnessAssumption_t attributeFairness;
    ArcList* attributeArcList;
}

%error-verbose /* more verbose and specific error message string */
%defines       /* write an output file containing macro definitions for the token types */
%name-prefix="ptnetlola_"

%type <attributeString> nodeident
%type <attributeFairness> fairness
%type <attributeArcList> arclist
%type <attributeArcList> arc
%type <attributeString> NUMBER
%type <attributeString> IDENTIFIER

%token IDENTIFIER NUMBER _CONSUME_ _FAIR_ _PLACE_ _MARKING_ _PRODUCE_ _SAFE_ _STRONG_ _TRANSITION_ _WEAK_ _colon_ _comma_ _semicolon_


%{
extern YYSTYPE ptnetlola_lval;
extern int ptnetlola_lex();
extern FILE* ptnetlola_in;
extern int ptnetlola_lineno;
extern int ptnetlola_colno;
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
        PlaceSymbol *p = new PlaceSymbol($3, TheCapacity);
        if (UNLIKELY (! TheResult->PlaceTable->insert(p)))
        {
            yyerrors($3, "place '%s' name used twice", $3);
        }
    }
| nodeident
    {
        // this action is only triggered for the first place - there can be no duplicates here!
        TheResult->PlaceTable->insert(new PlaceSymbol($1, TheCapacity));
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
        p->addInitialMarking((capacity_t)atoi($3));
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
        p->addInitialMarking(1);
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
        for (al = $3; al; al = al->getNext())
        {
            if (al->getPlace() == $1->getPlace())
            {
                break;
            }
        }
        if (al)
        {   
            //duplicate detected
            al->addMultiplicity($1->getMultiplicity());
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
    ptnetlola_parse();
    return(TheResult);
}

/// display a parser error and exit
__attribute__((noreturn)) void yyerrors(char* token, const char* format, ...) {
    va_list args;
    va_start(args, format);
    char* errormessage = NULL;
    const int res = vasprintf(&errormessage, format, args);
    assert(res != -1);
    rep->status(errormessage);
    free(errormessage);
    va_end(args);

    rep->status("%s:%d:%d - error near '%s'", rep->markup(MARKUP_FILE, basename((char*)netFile->getFilename())).str(), ptnetlola_lineno, ptnetlola_colno, token);

    rep->abort(ERROR_SYNTAX);
}

/// display a parser error and exit
__attribute__((noreturn)) void ptnetlola_error(char const* mess) {
    yyerrors(ptnetlola_text, mess);
}
