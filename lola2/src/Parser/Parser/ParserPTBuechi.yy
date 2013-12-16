/*!
\file
\brief Büchi automaton syntax
\author <unknown>
\status new
\ingroup g_frontend

Parses a Büchi automaton in LoLA syntax.
*/

%{
#include <config.h>
#include <limits.h>
#include <libgen.h>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <set>
#include <Parser/SymbolTable/PlaceSymbol.h>
#include <Parser/SymbolTable/TransitionSymbol.h>
#include <Parser/SymbolTable/SymbolTable.h>
#include <Parser/Parser/ParserPTNet.h>
#include <Parser/Parser/error.h>
#include <Parser/Parser/ast-system-k.h>       // for kc namespace
#include <Parser/Parser/ast-system-yystype.h> // for YYSTYPE

extern ParserPTNet* symbolTables;
extern SymbolTable* buechiStateTable;
%}

%error-verbose /* more verbose and specific error message string */
%defines       /* write an output file containing macro definitions for the token types */
%name-prefix="ptbuechi_"
%locations     /* we want to use token locations for better error messages */

%type <yt_tBuechiAutomata> buechiAutomata
%type <yt_tBuechiRules> buechiRule
%type <yt_tBuechiRules> buechiRules
%type <yt_tTransitionRules> transitionRules
%type <yt_tAcceptingSet> acceptingsets
%type <yt_tStatePredicate> statepredicate
%type <yt_tAtomicProposition> atomic_proposition
%type <yt_tTerm> term
%type <yt_integer> NUMBER
%type <yt_casestring> IDENTIFIER

%token IDENTIFIER          "identifier"
%token NUMBER              "number"
%token _accept_            "keyword accept"
%token _buechi_            "keyword buechi"
%token _braceleft_         "opening brace"
%token _braceright_        "closing brace"        
%token _comma_             "comma"
%token _then_              "transition =>"
%token _colon_             "colon"
%token _INITIAL_           "keyword INITIAL"
%token _AND_               "Boolean conjuction"
%token _NOT_               "Boolean negation"
%token _OR_                "Boolean disjunction"
%token _XOR_               "Boolean exclusive disjunction"
%token _iff_               "Boolean iff"
%token _notequal_          "not-equals sign"
%token _implies_           "Boolean implication"
%token _equals_            "equals sign"
%token _plus_              "plus sign"
%token _minus_             "minus sign"
%token _times_             "multiplication sign"
%token _leftparenthesis_   "opening parenthesis"
%token _rightparenthesis_  "closing parenthesis"
%token _greaterthan_       "greater-than sign"
%token _lessthan_          "less-than sign"
%token _greaterorequal_    "greater-than-or-equal sign"
%token _lessorequal_       "less-than-or-equal sign"
%token _semicolon_         "semicolon"
%token _TRUE_              "Boolean TRUE"
%token _FALSE_             "Boolean FALSE"
%token _FIREABLE_          "keyword FIREABLE"
%token _DEADLOCK_          "keyword DEADLOCK"
%token END 0               "end of file"

// precedences (lowest written first, e.g. PLUS/MINUS) and precedences
%left _OR_ _XOR_
%left _AND_
%left _implies_
%left _iff_
%left _equals_ _notequal_
%left _lessthan_ _lessorequal_ _greaterthan_ _greaterorequal_
%left _plus_ _minus_
%left _times_
%right _NOT_
%right _ALWAYS_ _EVENTUALLY_ _NEXTSTATE_
%right _UNTIL_
%right _ALLPATH_ _EXPATH_
%right _REACHABLE_ _INVARIANT_ _IMPOSSIBLE_

%{
// parser essentials
extern int ptbuechi_lex();
void ptbuechi_error(char const*);

int currentNextIndex = 0;
%}

%{
/* globals */
tBuechiAutomata TheBuechi;
uint32_t currentState;
%}

%%

buechiAutomata:
  _buechi_ _braceleft_ buechiRules _braceright_
  _accept_ _braceleft_ acceptingsets _braceright_ _semicolon_
    { TheBuechi = BuechiAutomaton($3,$7); }
;

buechiRules:
  /* empty */
    { $$ = EmptyBuechiRules(); }
| buechiRule
    { $$ = $1; }
| buechiRule _comma_ buechiRules
    { $$ = BuechiRules($1,$3); }
;

buechiRule:
  IDENTIFIER
      {
  	      Symbol *t = buechiStateTable->lookup($1->name);
  	      if (t == NULL)
          {
  		      t = new Symbol($1->name);
  		      buechiStateTable->insert(t);
  		      t->setIndex(currentNextIndex++);
  	      }
      }
  _colon_ transitionRules
      {
          Symbol *t = buechiStateTable->lookup($1->name);
          $$ = BuechiRule((mkinteger(t->getIndex())),$4); $1->free(true);
      }
;

transitionRules:
  /* empty */
    { $$ = EmptyTransitionRules(); }
| statepredicate _then_ IDENTIFIER transitionRules
    {
        Symbol *t = buechiStateTable->lookup($3->name);
	  	if (UNLIKELY(t == NULL)){
	  		buechiStateTable->insert(new Symbol($3->name));
	  		t = buechiStateTable->lookup($3->name);
	  		t->setIndex(currentNextIndex++);
	  	}
        $$ = TransitionRules(TransitionRule(StatePredicateFormula($1),mkinteger(t->getIndex())),$4);
    }
;

acceptingsets:
  /* empty */
    { $$ = EmptyAcceptingSet(); }
| IDENTIFIER
    {
        Symbol *t = buechiStateTable->lookup($1->name);
        if (UNLIKELY(t == NULL))
        {
            yyerrors($1->name, @1, "state '%s' unknown", $1->name);
        }
        $$ = AcceptingState(mkinteger(t->getIndex()));
    }
| IDENTIFIER _comma_ acceptingsets
    {
        Symbol *t = buechiStateTable->lookup($1->name);
        if (UNLIKELY(t == NULL))
        {
            yyerrors($1->name, @1, "state '%s' unknown", $1->name);
        }
        $$ = AcceptingSet(AcceptingState(mkinteger(t->getIndex())),$3);
    }
;

statepredicate:
  _leftparenthesis_ statepredicate _rightparenthesis_
    { $$ = $2; }
| atomic_proposition
    { $$ = AtomicProposition($1); }
| _NOT_ statepredicate
    { $$ = Negation($2); }
| statepredicate _AND_ statepredicate
    { $$ = Conjunction($1, $3); }
| statepredicate _OR_ statepredicate
    { $$ = Disjunction($1, $3); }
| statepredicate _XOR_ statepredicate
    { $$ = ExclusiveDisjunction($1, $3); }
| statepredicate _implies_ statepredicate
    { $$ = Implication($1, $3); }
| statepredicate _iff_ statepredicate
    { $$ = Equivalence($1, $3); }
;

atomic_proposition:
  term _equals_ term
    { $$ = EqualsAtomicProposition($1, $3); }
| term _notequal_ term
    { $$ = NotEqualsAtomicProposition($1, $3); }
| term _greaterthan_ term
    { $$ = GreaterAtomicProposition($1, $3); }
| term _greaterorequal_ term
    { $$ = GreaterEqualAtomicProposition($1, $3); }
| term _lessthan_ term
    { $$ = LessAtomicProposition($1, $3); }
| term _lessorequal_ term
    { $$ = LessEqualAtomicProposition($1, $3); }
| _TRUE_
    { $$ = True(); }
| _FALSE_
    { $$ = False(); }
| _FIREABLE_ _leftparenthesis_ IDENTIFIER _rightparenthesis_
    {
        Symbol *t = symbolTables->TransitionTable->lookup($3->name);
        if (UNLIKELY(t == NULL))
        {
            yyerrors($3->name, @3, "transition '%s' unknown", $3->name);
        }
        $$ = Fireable(mkinteger(t->getIndex()));
    }
| _DEADLOCK_
    { $$ = aDeadlock(); }
| _INITIAL_
    { $$ = Initial(); }
;

term:
  _leftparenthesis_ term _rightparenthesis_
    { $$ = $2; }
| IDENTIFIER
    {
        Symbol *p = symbolTables->PlaceTable->lookup($1->name);
        if (UNLIKELY(p == NULL))
        {
            yyerrors($1->name, @1, "place '%s' unknown", $1->name);
        }
        $$ = Node(mkinteger(p->getIndex()));
    }
| NUMBER
    { $$ = Number($1); }
| term _plus_ term
    { $$ = Sum($1, $3); }
| term _minus_ term
    { $$ = Difference($1, $3); }
| NUMBER _times_ term
    { $$ = Product($1, $3); }
;

%%

/// display a parser error and exit
void ptbuechi_error(char const* mess) __attribute__((noreturn));
void ptbuechi_error(char const* mess) {
    extern char* ptbuechi_text; ///< the current token text from Flex
    yyerrors(ptbuechi_text, ptbuechi_lloc, mess);
}
