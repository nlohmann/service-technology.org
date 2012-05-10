%{
#include <config.h>
#include <limits.h>
#include <libgen.h>
#include <cstdarg>
#include <cstdio>
#include "cmdline.h"
#include "Core/Dimensions.h"
#include "Parser/PlaceSymbol.h"
#include "Parser/TransitionSymbol.h"
#include "Parser/SymbolTable.h"
#include "Parser/ParserPTNet.h"
#include "Parser/FairnessAssumptions.h"
#include "Parser/ArcList.h"
#include "InputOutput/Reporter.h"
#include "InputOutput/InputOutput.h"

#include "Parser/ast-system-k.h"
#include "Parser/ast-system-yystype.h"

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
%type <yt_tStatePredicate> negation
%type <yt_tStatePredicate> conjunction
%type <yt_tStatePredicate> disjunction
%type <yt_tStatePredicate> implication
%type <yt_tStatePredicate> equivalence
%type <yt_tTerm> term

%token IDENTIFIER NUMBER
%token _FORMULA_ _AND_ _NOT_ _OR_ _iff_ _notequal_ _implies_ _equals_ _plus_ _minus_ _times_ _leftparenthesis_ _rightparenthesis_ _greaterthan_ _lessthan_ _greaterorequal_ _lessorequal_ _semicolon_


%{
extern YYSTYPE ptformula_lval;
extern int ptformula_lex();
extern FILE* ptformula_in;
extern int ptformula_lineno;
extern int ptformula_colno;
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
  atomic_proposition
    { $$ = AtomicProposition($1); }
| negation
    { $$ = $1; }
| conjunction
    { $$ = $1; }
| disjunction
    { $$ = $1; }
| implication
    { $$ = $1; }
| equivalence
    { $$ = $1; }
;

negation:
  _NOT_ _leftparenthesis_ statepredicate _rightparenthesis_
    { $$ = Negation($3); }
;

conjunction:
  _leftparenthesis_ statepredicate _AND_ statepredicate _rightparenthesis_
    { $$ = Conjunction($2, $4); }
;

disjunction:
  _leftparenthesis_ statepredicate _OR_ statepredicate _rightparenthesis_
    { $$ = Disjunction($2, $4); }
;

implication:
  _leftparenthesis_ statepredicate _implies_ statepredicate _rightparenthesis_
    { $$ = Implication($2, $4); }
;

equivalence:
  _leftparenthesis_ statepredicate _iff_ statepredicate _rightparenthesis_
    { $$ = Equivalence($2, $4); }
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
;

term:
  IDENTIFIER
    {
      Symbol *p = symbolTables->PlaceTable->lookup($1->name);
      if (p == NULL)
      {
          ptformula_yyerrors(ptformula_text, "place unknown");
      }
      $$ = Node(mkinteger(p->getIndex()));
    }
| NUMBER
    { $$ = Number($1); }
| _leftparenthesis_ term _plus_ term _rightparenthesis_
    { $$ = Sum($2, $4); }
| _leftparenthesis_ term _minus_ term _rightparenthesis_
    { $$ = Difference($2, $4); }
| _leftparenthesis_ NUMBER _times_ term _rightparenthesis_
    { $$ = Product($2, $4); }
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
}

/// display a parser error and exit
__attribute__((noreturn)) void ptformula_error(char const* mess) {
    ptformula_yyerrors(ptformula_text, mess);
}
