%{
#include <config.h>
#include <limits.h>
#include <libgen.h>
#include <cstdarg>
#include <cstdio>
#include <set>
#include <cmdline.h>
#include <Core/Dimensions.h>
#include <Parser/PlaceSymbol.h>
#include <Parser/TransitionSymbol.h>
#include <Parser/SymbolTable.h>
#include <Parser/ParserPTNet.h>
#include <Parser/FairnessAssumptions.h>
#include <Parser/ArcList.h>
#include <InputOutput/Reporter.h>
#include <InputOutput/InputOutput.h>
#include <Net/Net.h>

#include <Parser/ast-system-k.h>
#include <Parser/ast-system-yystype.h>

extern ParserPTNet* symbolTables;

extern gengetopt_args_info args_info;

using namespace kc;

/// the current token text from Flex
extern char* ptformula_text;

void ptformula_error(char const*);
void ptformula_yyerrors(char* token, const char* format, ...);
%}

%error-verbose /* more verbose and specific error message string */
%defines       /* write an output file containing macro definitions for the token types */
%name-prefix="ptformula_"

%type <yt_integer> NUMBER
%type <yt_casestring> IDENTIFIER

%type <yt_tFormula> formula
%type <yt_tStatePredicate> statepredicate
%type <yt_tAtomicProposition> atomic_proposition
%type <yt_tTerm> term

%token IDENTIFIER          "identifier"
%token NUMBER              "number"
%token _RELEASE_           "temporal operator RELEASE"
%token _NEXTSTATE_         "temporal operator NEXTSTATE"
%token _INITIAL_           "keyword INITIAL"
%token _FORMULA_           "keyword FORMULA"
%token _AND_               "Boolean conjuction"
%token _NOT_               "Boolean negation"
%token _OR_                "Boolean disjunction"
%token _XOR_               "Boolean exclusive disjunction"
%token _iff_               "Boolean iff"
%token _ALLPATH_           "path quantifier ALLPATH"
%token _ALWAYS_            "temporal operator ALWAYS"
%token _EVENTUALLY_        "temporal operator EVENTUALLY"
%token _EXPATH_            "path quantifier EXPATH"
%token _UNTIL_             "temporal operator UNTIL"
%token _REACHABLE_         "keyword REACHABLE"
%token _INVARIANT_         "keyword INVARIANT"
%token _IMPOSSIBLE_        "keyword IMPOSSIBLE"
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
extern YYSTYPE ptformula_lval;
extern int ptformula_lex();
extern FILE* ptformula_in;
extern int ptformula_lineno;
extern int ptformula_colno;

std::set<index_t> target_place;
std::set<index_t> target_transition;
%}

%{
/* globals */
tFormula TheFormula;
%}

%%

formula:
  _FORMULA_ statepredicate _semicolon_
    { TheFormula = $$ = StatePredicateFormula($2); }
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
| _ALLPATH_ statepredicate
    { $$ = AllPath($2); }
| _EXPATH_ statepredicate
    { $$ = ExPath($2); }
| _ALWAYS_ statepredicate
    { $$ = Always($2); }
| _EVENTUALLY_ statepredicate
    { $$ = Eventually($2); }
| _leftparenthesis_ statepredicate _UNTIL_ statepredicate _rightparenthesis_
    { $$ = Until($2, $4); }
| _leftparenthesis_ statepredicate _RELEASE_ statepredicate _rightparenthesis_
    { $$ = Release($2, $4); }
| _NEXTSTATE_ statepredicate
    { $$ = NextState($2); }
| _REACHABLE_ statepredicate
    { $$ = ExPath(Eventually($2)); }
| _INVARIANT_ statepredicate
    { $$ = AllPath(Always($2)); }
| _IMPOSSIBLE_ statepredicate
    { $$ = AllPath(Always(Negation($2))); }
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
        if (t == NULL)
        {
            ptformula_yyerrors(ptformula_text, "transition %s unknown", $3->name);
        }
        $$ = Fireable(mkinteger(t->getIndex()));
        target_transition.insert(t->getIndex());
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
        if (p == NULL)
        {
            ptformula_yyerrors(ptformula_text, "place %s unknown", $1->name);
        }
        $$ = Node(mkinteger(p->getIndex()));
        target_place.insert(p->getIndex());
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
void ptformula_yyerrors(char* token, const char* format, ...) __attribute__((noreturn));
void ptformula_yyerrors(char* token, const char* format, ...) {
    va_list args;
    va_start(args, format);
    char* errormessage = NULL;
    const int res = vasprintf(&errormessage, format, args);
    assert(res != -1);
    rep->status(errormessage);
    free(errormessage);
    va_end(args);

    rep->status("%s:%d:%d - error near '%s'", rep->markup(MARKUP_FILE, basename((char*)args_info.formula_arg)).str(), ptformula_lineno, ptformula_colno, token);
//    rep->status("%d:%d - error near '%s'", ptformula_lineno, ptformula_colno, token);

    rep->abort(ERROR_SYNTAX);
    exit(EXIT_ERROR); // needed to corrently recognize noreturn since rep->abort is virtual
}

/// display a parser error and exit
void ptformula_error(char const* mess) __attribute__((noreturn));
void ptformula_error(char const* mess) {
    ptformula_yyerrors(ptformula_text, mess);
}
