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

extern Reporter* rep;

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

%token IDENTIFIER NUMBER
%token _RELEASE_ _NEXTSTATE_ _INITIAL_ _FORMULA_ _AND_ _NOT_ _OR_ _XOR_ _iff_ _ALLPATH_ _ALWAYS_ _EVENTUALLY_ _EXPATH_ _UNTIL_ _REACHABLE_ _INVARIANT_ _IMPOSSIBLE_ _notequal_ _implies_ _equals_ _plus_ _minus_ _times_ _leftparenthesis_ _rightparenthesis_ _greaterthan_ _lessthan_ _greaterorequal_ _lessorequal_ _semicolon_ _TRUE_ _FALSE_ _FIREABLE_ _DEADLOCK_

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
__attribute__((noreturn)) void ptformula_yyerrors(char* token, const char* format, ...) {
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
__attribute__((noreturn)) void ptformula_error(char const* mess) {
    ptformula_yyerrors(ptformula_text, mess);
}
