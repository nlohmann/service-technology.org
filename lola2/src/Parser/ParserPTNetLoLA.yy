/*!
\file
\brief net syntax
\author Karsten
\status approved 25.01.2012
\ingroup g_frontend

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
#include <Parser/ArcList.h>
#include <Parser/error.h>

%}

%union {
    char* attributeString;
    fairnessAssumption_t attributeFairness;
    ArcList* attributeArcList;
}

%error-verbose /* more verbose and specific error message string */
%defines       /* write an output file containing macro definitions for the token types */
%name-prefix="ptnetlola_"
%locations     /* we want to use token locations for better error messages */

%type <attributeString> nodeident
%type <attributeFairness> fairness
%type <attributeArcList> arclist
%type <attributeArcList> arc
%type <attributeString> NUMBER
%type <attributeString> IDENTIFIER

%token IDENTIFIER     "identifier"
%token NUMBER         "number"
%token KEY_CONSUME    "keyword CONSUME"
%token KEY_FAIR       "keyword FAIR"
%token KEY_PLACE      "keyword PLACE"
%token KEY_MARKING    "keyword MARKING"
%token KEY_PRODUCE    "keyword PRODUCE"
%token KEY_SAFE       "keyword SAFE"
%token KEY_STRONG     "keyword STRONG"
%token KEY_TRANSITION "keyword TRANSITION"
%token KEY_WEAK       "keyword WEAK"
%token COLON          "colon"
%token COMMA          "comma"
%token SEMICOLON      "semicolon"
%token END 0          "end of file"

%{
// parser essentials
extern int ptnetlola_lex();
void ptnetlola_error(char const*);

extern YYSTYPE ptnetlola_lval;
%}

%{
// This list contains a few global variables. Their purpose is to
// propagate semantic values top down or left to right which is
// impossible with the attributes of bison. Thee variables must be initialized
// whenever this parser in invoked. After termination of the parser,
// their values become meaningless.

/// The object containing the final outcome of the parsing process
ParserPTNet* TheResult;
/// The value of the currently active capacity statement
capacity_t TheCapacity;
%}

%%

net:
  KEY_PLACE placelists SEMICOLON    /* declare places */
  KEY_MARKING markinglist SEMICOLON /* specify initial marking */
  transitionlist                    /* define transitions & arcs */
;

placelists:
  capacity placelist    /* several places may share unqiue capacity */
| placelists SEMICOLON capacity placelist
;

capacity:
  /* empty */
    {
        /* empty capacity = unlimited capacity */
        TheCapacity = (capacity_t)MAX_CAPACITY;
    }
| KEY_SAFE COLON
    {
        /* SAFE without number = 1-SAFE */
        TheCapacity = 1;
    }
| KEY_SAFE NUMBER COLON
    {
        /* at most k tokens expected on these places */
        TheCapacity = (capacity_t)atoi($2);
        free($2);
    }
;

placelist:
  placelist COMMA nodeident
    {
        PlaceSymbol *p = new PlaceSymbol($3, TheCapacity);
        if (UNLIKELY (! TheResult->PlaceTable->insert(p)))
        {
            yyerrors($3, @3, "place '%s' name used twice", $3);
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
| markinglist COMMA marking
;

marking:
  nodeident COLON NUMBER
    {
        PlaceSymbol* p = reinterpret_cast<PlaceSymbol*>(TheResult->PlaceTable->lookup($1));
        if (UNLIKELY (!p))
        {
            yyerrors($1, @1, "place '%s' does not exist", $1);
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
            yyerrors($1, @1, "place '%s' does not exist", $1);
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
  KEY_TRANSITION nodeident fairness
  KEY_CONSUME arclist SEMICOLON
  KEY_PRODUCE arclist SEMICOLON
    {
        TransitionSymbol* t = new TransitionSymbol($2, $3, $5, $8);
        if (UNLIKELY (! TheResult->TransitionTable->insert(t)))
        {
            yyerrors($2, @2, "transition name '%s' used twice", $2);
        }
    }
;

fairness:
    /* empty */
    {
        /* no fairness given */
        $$ = NO_FAIRNESS;
    }
| KEY_WEAK KEY_FAIR
    {
        $$ = WEAK_FAIRNESS;
    }
| KEY_STRONG KEY_FAIR
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
| arc COMMA arclist
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
  nodeident COLON NUMBER
    {
        PlaceSymbol* p = reinterpret_cast<PlaceSymbol*>(TheResult->PlaceTable->lookup($1));
        if (UNLIKELY (!p))
        {
            yyerrors($1, @1, "place '%s' does not exist", $1);
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
            yyerrors($1, @1, "place '%s' does not exist", $1);
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
    return TheResult;
}

/// display a parser error and exit
void ptnetlola_error(char const* mess) __attribute__((noreturn));
void ptnetlola_error(char const* mess)
{
    extern char* ptnetlola_text;  ///< the current token text from Flex
    yyerrors(ptnetlola_text, ptnetlola_lloc, mess);
}
