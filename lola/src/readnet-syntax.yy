/*****************************************************************************\
 LoLA -- a Low Level Petri Net Analyzer

 Copyright (C)  1998, 1999, 2000, 2001, 2002, 2003, 2004, 2005, 2006, 2007,
                2008, 2009  Karsten Wolf <lola@service-technology.org>

 LoLA is free software: you can redistribute it and/or modify it under the
 terms of the GNU Affero General Public License as published by the Free
 Software Foundation, either version 3 of the License, or (at your option)
 any later version.

 LoLA is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for
 more details.

 You should have received a copy of the GNU Affero General Public License
 along with LoLA.  If not, see <http://www.gnu.org/licenses/>. 
\*****************************************************************************/


%{
#include "dimensions.H"
#include "net.H"
#include "graph.H"
#include "symboltab.H"
#include "formula.H"
#include "buchi.H"
#include "unfold.H"
#include "verbose.h"
#include <cstdio>
#include <climits>
#include <unistd.h>
#include <cstdarg>
#include <libgen.h>


extern UBooType* TheBooType;
extern UNumType* TheNumType;
extern char* yytext;

#define YYDEBUG 1
void yyerror(char const*);
void yyerrors(char* token, char const* format, ...);

class arc_list {
    public:
        PlSymbol* place;
        UTermList* mt;
        unsigned int nu;
        arc_list*     next;
};

class case_list {
    public:
        UStatement* stm;
        UExpression* exp;
        case_list* next;
};

/* list of places and multiplicities to become arcs */

int CurrentCapacity;
UFunction* CurrentFunction;
Place* P;
Transition* T;
Symbol* S;
PlSymbol* PS;
TrSymbol* TS;
SymbolTab* GlobalTable;
SymbolTab* LocalTable;
UVar* V;
VaSymbol* VS;

%}

%union {
    char* str;
    int value;
    UType* t;
    URcList* rcl;
    UEnList* el;
    ULVal* lval;
    int* exl;
    UStatement* stm;
    case_list* cl;
    UFunction* fu;
    UExpression* ex;
    arc_list* al;
    formula* form;
    IdList* idl;
    UTermList* tlist;
    Place* pl;
    Transition* tr;
    fmode* fm;
    TrSymbol* ts;
    VaSymbol* varsy;
}


///// 4 LINES ADDED BY NIELS
%error-verbose
%token_table
%defines
%yacc


%type <varsy> quantification
%type <value> transitionvariables
%type <fm> firingmode
%type <fm> parfiringmode
%type <fm> fmodeblock
%type <pl> aplace
%type <tr> atransition
%type <ts> hlprefix
%type <ts> formulatransition
%type <cl> valuelist
%type <cl> expressionlist
%type <ex> arrayvalue
%type <ex> functioncall
%type <ex> expression
%type <ex> numex
%type <ex> express
%type <ex> expre
%type <ex> expr
%type <ex> exp
%type <ex> term
%type <ex> factor
%type <ex> fac
%type <ex> guard
%type <lval> lvalue
%type <cl> caselist
%type <cl> case
%type <stm> while_statement
%type <stm> repeat_statement
%type <stm> for_statement
%type <stm> forall_statement
%type <stm> if_statement
%type <stm> case_statement
%type <stm> assignment
%type <stm> return_statement
%type <stm> exit_statement 
%type <idl> identlist
%type <stm> statement_seq
%type <stm> statement
%type <stm> body
%type <fu> head
%type <el> enu
%type <el> enums
%type <rcl> recordcomponents
%type <rcl> recordcomponent
%type <t> sortdescription
%type <str> IDENTIFIER
%type <str> _equals_
%type <str> _comma_
%type <str> NUMBER
%type <str> tname
%type <str> nodeident
%type <al> arclist
%type <al> arc
%type <form> ctlformula
%type <form> transformula
%type <form> transitionformula
%type <form> cplace
%type <tlist> multiterm
%type <tlist> mtcomponent
%type <tlist> termlist
%type <tlist> hlterm

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

%nonassoc _greaterorequal_ _greaterthan_ _lessorequal_ _lessthan_ _notequal_ _equals_
%left _NOT_ _EXPATH_ _ALLPATH_ _ALWAYS_ _EVENTUALLY_ _NEXT_
%left _AND_ _OR_ _plus_ _minus_ _times_ _divide_ _MOD_
%left _UNTIL_
%nonassoc _colon_

%{
extern YYSTYPE yylval;
extern int yylex();
extern FILE *yyin;
extern int yylineno;
%}

%%

input:
  declarations net { F = NULL; }
| declarations net formulaheader ctlformula { F = $4; }
| declarations net _ANALYSE_ _PLACE_ aplace {
    F = NULL;
    CheckPlace = $5;
#ifdef STUBBORN
    Transitions[0]->StartOfStubbornList = NULL;
    int i;
    for (i = 0; CheckPlace->PreTransitions[i]; i++) {
        CheckPlace->PreTransitions[i]->instubborn = true;
        CheckPlace->PreTransitions[i]->NextStubborn = Transitions[0]->StartOfStubbornList;
        Transitions[0]->StartOfStubbornList = CheckPlace->PreTransitions[i];
    }
    Transitions[0]->EndOfStubbornList = LastAttractor = CheckPlace->PreTransitions[0];
#endif
    }
| declarations net _ANALYSE_ _TRANSITION_ atransition {
        F = NULL;
        CheckTransition = $5;
#ifdef STUBBORN
        Transitions[0]->EndOfStubbornList = LastAttractor = Transitions[0]->StartOfStubbornList = CheckTransition;
        CheckTransition->NextStubborn = NULL;
        CheckTransition->instubborn = true;
#endif
    }
| declarations net _ANALYSE_ _MARKING_ amarkinglist  { F = NULL; }
| declarations net _AUTOMATON_ automaton  {F = NULL; }
;


formulaheader:
  _FORMULA_ { LocalTable = new SymbolTab(2); }
;


atransition:
  nodeident {
        TS = (TrSymbol*) TransitionTable->lookup($1);
        if (!TS) {
            yyerrors($1, "transition '%s' does not exist", _cimportant_($1));
        }
        if (TS->vars && TS->vars->card) {
            yyerrors($1, "high-level transition '%s' requires firing mode", _cimportant_($1));
        }
        $$ = TS->transition;
    }
| hlprefix _leftbracket_ firingmode _rightbracket_ {
        unsigned int i, j, card;
        fmode* fm;
        UValue* vl;
        unsigned int len;
        for (card = 0, fm = $3; fm; fm = fm->next, card++);
        if (card != $1->vars->card) {
            yyerror("firing mode incomplete");
        }
        char** cc = new char* [ card + 10];
        char** inst = new char* [ card + 10];
        len = strlen($1->name) + 4;
        j = 0;
        for (i = 0; i < LocalTable->size; i++) {
            for (VS = (VaSymbol*)(LocalTable->table[i]); VS; VS = (VaSymbol*)(VS->next)) {
                for (fm = $3; fm; fm = fm->next) {
                    if (fm->v == VS) {
                        break;
                    }
                }
                if (!fm) {
                    yyerror("firing mode incomplete");
                }
                vl = fm->t->evaluate();
                UValue* pl = VS->var->type->make();
                pl->assign(vl);
                cc[j] = pl->text();
                inst[j] = new char [strlen(VS->name) + strlen(cc[j]) + 20];
                strcpy(inst[j], VS->name);
                strcpy(inst[j] + strlen(inst[j]), "=");
                strcpy(inst[j] + strlen(inst[j]), cc[j]);
                len += strlen(inst[j]) + 1;
                j++;
            }
        }
        char* llt = new char[len + 20];
        strcpy(llt, $1->name);
        strcpy(llt + strlen(llt), ".[");
        for (j = 0; j < card; j++) {
            strcpy(llt + strlen(llt), inst[j]);
            strcpy(llt + strlen(llt), "|");
        }
        strcpy(llt + (strlen(llt) - 1), "]");
        TS = (TrSymbol*) TransitionTable->lookup(llt);
        if (!TS) {
            yyerror("transition instance does not exist");
        }
        if (TS->vars && TS->vars->card) {
            yyerror("high-level and low-level transition names mixed up");
        }
        $$ = TS->transition;
    }
;


hlprefix:
  nodeident _dot_ {
        TS = (TrSymbol*) TransitionTable->lookup($1);
        if (!TS) {
            yyerrors($1, "high-level transition '%s' does not exist", _cimportant_($1));
        }
        if ((!(TS->vars) || TS->vars->card == 0)) {
            yyerrors($1, "high-level transition '%s' requires firing mode", _cimportant_($1));
        }
        $$ = TS;
        LocalTable = TS->vars;
    }
;


firingmode:
  nodeident _equals_ expression  {
        VS = (VaSymbol*) LocalTable->lookup($1);
        if (!VS) {
            yyerrors(yytext, "transition '%s' does not have this variable", _cimportant_($1));
        }
        if (!(VS->var->type->iscompatible($3->type))) {
            yyerrors(yytext, "expression not compatible with variable of transition '%s'", _cimportant_($1));
        }
        $$ = new fmode;
        $$->next = NULL;
        $$->v = VS;
        $$->t = $3;
    }
| firingmode _slash_ nodeident _equals_ expression {
        VS = (VaSymbol*) LocalTable->lookup($3);
        if (!VS) {
            yyerrors(yytext, "transition '%s' does not have this variable", _cimportant_($3));
        }
        if (!(VS->var->type->iscompatible($5->type))) {
            yyerrors(yytext, "expression not compatible with variable of transition '%s'", _cimportant_($3));
        }
        $$ = new fmode;
        $$->next = $1;
        $$->v = VS;
        $$->t = $5 ;
        for (fmode* fm = $1; fm ; fm = fm->next) {
            if (fm->v == $$->v) {
                yyerror("variable appears twice in firing mode");
            }
        }
    }
;


aplace:
  nodeident {
        PS = (PlSymbol*) PlaceTable->lookup($1);
        if (!PS) {
            yyerrors($1, "place '%s' does not exist", _cimportant_($1));
        }
        if (PS->sort) {
            yyerrors($1, "high-level '%s' place requires instance", _cimportant_($1));
        }
        $$ = PS->place;
    }
| nodeident _dot_ expression {
        PS = (PlSymbol*) PlaceTable->lookup($1);
        if (!PS) {
            yyerrors($1, "place '%s' does not exist", _cimportant_($1));
        }
        if (!(PS->sort)) {
            yyerrors($1, "low-level place '%s' does not require an instance", _cimportant_($1));
        }
        if (!(PS->sort->iscompatible($3->type))) {
            yyerrors($1, "place color not compatible to sort of place '%s'", _cimportant_($1));
        }
        UValue* vl = $3->evaluate();
        UValue* pl = PS->sort->make();
        pl->assign(vl);
        char* inst = pl->text();
        char* ll = new char [strlen(PS->name) + strlen(inst) + 20];
        strcpy(ll, PS->name);
        strcpy(ll + strlen(ll), ".");
        strcpy(ll + strlen(ll), inst);
        PS = (PlSymbol*) PlaceTable->lookup(ll);
        if (!PS) {
            yyerror("place instance does not exist");
        }
        if (PS->sort) {
            yyerror("mixed up high-level and low-level place names");
        }
        $$ = PS->place;
    }
;


declarations:
  /* empty */
| declarations declaration
;


declaration:
  _SORT_ sortdeclarations
| _FUNCTION_ {LocalTable = new SymbolTab(1); } functiondeclaration
;


sortdeclarations:
  sortdeclaration
| sortdeclarations sortdeclaration
;


sortdeclaration:
  IDENTIFIER _equals_ sortdescription _semicolon_ {
        // sort symbols are globally visible. A sort entry in the
        // symbol table relates a name to a sort description (UType)

        SoSymbol* s;
        if ((s = (SoSymbol*)(GlobalTable->lookup($1)))) {
            yyerrors($1, "sort symbol name '%s' already used", _cimportant_($1));
        }
        s = new SoSymbol($1, $3);
    }
;


sortdescription:
  _BOOLEAN_ { $$ = TheBooType; }
| IDENTIFIER {
        // assign an additional name to an existing sort
        SoSymbol* s = (SoSymbol*)(GlobalTable->lookup($1));
        if (!s) {
            yyerrors($1, "undefined sort name '%s'", _cimportant_($1));
        }
        if (s->kind != so) {
            yyerrors($1, "sort name expected - '%s' is not a sort", _cimportant_($1));
        }
        $$ = s->type;
    }
| _RECORD_ recordcomponents _END_ {
        $$ = new URecType($2);
    }
| _ARRAY_ sortdescription _OF_ sortdescription {
        // index type must be scalar
        if ($2->tag != boo && $2->tag != num && $2->tag != enu) {
            yyerror("non-scalar type as index of array");
        }
        $$ = new UArrType($2, $4);
    }
| _leftbracket_ NUMBER _comma_ NUMBER _rightbracket_ {
        // integer interval
        const unsigned int l = atoi($2);
        const unsigned int r = atoi($4);
        if (l > r) {
            yyerrors(yytext, "negative range in integer type: lower bound (%d) greater than upper bound (%d)", l, r);
        }
        $$ = new UNumType(l, r);
    }
| _ENUMERATE_ enums _END_ {
        $$ = new UEnuType($2);
    }
;


enums:
  enu
| enu enums { 
        $1->next = $2;
        $$ = $1;
    }
;


enu:
  IDENTIFIER {
        EnSymbol* e = (EnSymbol*) GlobalTable->lookup($1);
        if (e) {
            yyerrors($1, "element name '%s' of enumeration already used", _cimportant_($1));
        }
        e = new EnSymbol($1);
        UEnList* eel = new UEnList;
        eel->sy = e;
        eel->next = NULL;
        $$ = eel;
    }
;


recordcomponents:
  recordcomponent
| recordcomponents recordcomponent {
        $2->next = $1;
        $$ = $2;
    }
;


recordcomponent:
  IDENTIFIER _colon_ sortdescription _semicolon_ {
        RcSymbol* r = (RcSymbol*) GlobalTable->lookup($1);
        if (r) {
            yyerrors($1, "record component name '%s' already used", _cimportant_($1));
        }
        r = new RcSymbol($1, $3);
        URcList* rl = new URcList;
        rl->next = NULL;
        rl->sy = r;
        rl->ty = $3;
        $$ = rl;
    }
;


functiondeclaration:
  head vardeclarations body {
        $1->body = $3;
        $1->localsymb = LocalTable;
    }
;


vardeclarations:
  /* empty */
| _VAR_ vdeclarations
;


vdeclarations:
  /* empty */
| vdeclarations vdeclaration
;


vdeclaration:
  identlist _colon_ sortdescription _semicolon_ {
        for (IdList* il = $1; il; il = il->next) {
            VaSymbol* v;
            UVar* vvv;
            if ((v = (VaSymbol*)(LocalTable->lookup(il->name)))) {
                yyerrors(il->name, "variable name '%s' already used", _cimportant_(il->name));
            }
            vvv = new UVar($3);
            v = new VaSymbol(il->name, vvv);
        }
    }
;


identlist:
  IDENTIFIER {
        $$ = new IdList;
        $$->name = $1;
        $$->next = NULL;
    }
| IDENTIFIER _comma_ identlist {
        $$ = new IdList;
        $$->name = $1;
        $$->next = $3;
    }
;


head:
  IDENTIFIER _leftparenthesis_ fparlists _rightparenthesis_ _colon_ sortdescription {
        UFunction* f;
        FcSymbol* fs = (FcSymbol*) GlobalTable->lookup($1);
        if (fs) {
            yyerrors($1, "function name '%s' already used", _cimportant_($1));
        }
        CurrentFunction = f = new UFunction();
        fs = new FcSymbol($1, f);
        f->type = $6;
        f->localsymb = LocalTable;
        f->result = NULL;
        f->resultstack = NULL;
        f->arity = LocalTable->card;
        f->formalpar = new UVar* [f->arity + 5];
        int i;
        i = 0;
        for (unsigned int j = 0; j < LocalTable->size; j++) {
            Symbol* s;
            for (s = LocalTable->table[j]; s; s = s->next) {
                f->formalpar[(f->arity - 1) - i++] = ((VaSymbol*) s)->var;
            }
        }

        $$ = f;
    }
;

fparlists:
  /* empty */
| fparlist
| fparlists _semicolon_ fparlist ;


fparlist:
  identlist _colon_ sortdescription {
        for (IdList* il = $1; il; il = il->next) {
            VaSymbol* v;
            UVar* vvv;
            if ((v = (VaSymbol*)(LocalTable->lookup(il->name)))) {
                yyerrors(il->name, "variable name '%s' already used", _cimportant_(il->name));
            }
            vvv = new UVar($3);
            v = new VaSymbol(il->name, vvv);

        }
    }
;


body:
  _BEGIN_ statement_seq _END_ { $$ = $2; } 
;


statement_seq:
  statement
| statement_seq _semicolon_ statement { 
        UStatement* s = new USequenceStatement;
        ((USequenceStatement*) s)->first = $1;
        ((USequenceStatement*) s)->second = $3;
        $$ = s;
    }
;


statement:
  while_statement
| repeat_statement
| for_statement
| forall_statement
| if_statement
| case_statement
| assignment
| return_statement
| exit_statement 
;


while_statement:
  _WHILE_ expression _DO_ statement_seq _END_ {
        if ($2->type->tag != boo) {
            yyerror("while condition must be Boolean");
        }
        $$ = new UWhileStatement;
        ((UWhileStatement*) $$)->cond = $2;
        ((UWhileStatement*) $$)->body = $4;
    }
;


repeat_statement:
  _REPEAT_ statement_seq _UNTIL_ expression _END_ {
        if ($4->type->tag != boo) {
            yyerror("repeat condition must be Boolean");
        }
        $$ = new URepeatStatement;
        ((URepeatStatement*) $$)->cond = $4;
        ((URepeatStatement*) $$)->body = $2;
    }
;


for_statement:
  _FOR_ IDENTIFIER _equals_ expression _TO_ expression _DO_ statement_seq _END_ {
        VaSymbol* v = (VaSymbol*) LocalTable->lookup($2);
        if (!v) {
            yyerrors($2, "loop variable '%s' not declared", _cimportant_($2));
        }
        $$ = new UForStatement;
        ((UForStatement*) $$)->var = v->var;
        if (!(v->var->type->iscompatible($4->type))) {
            yyerror("initial expression of for statement not compatible to counter variable");
        }
        if (!(v->var->type->iscompatible($6->type))) {
            yyerror("exit expression of for statement not compatible to counter variable");
        }
        ((UForStatement*) $$)->init = $4;
        ((UForStatement*) $$)->finit = $6;
        ((UForStatement*) $$)->body = $8;
    }
;


forall_statement:
  _FOR_ _ALL_ IDENTIFIER _DO_ statement_seq _END_ {
        VaSymbol* v;
        v = (VaSymbol*) LocalTable->lookup($3);
        if (!v) {
            yyerrors($3, "loop variable '%s' not declared", _cimportant_($3));
        }
        $$ = new UForallStatement;
        ((UForallStatement*) $$)->var = v->var;
        ((UForallStatement*) $$)->body = $5;
    }
;


if_statement:
  _IF_ expression _THEN_ statement_seq _END_ {
        if ($2->type->tag != boo) {
            yyerror("condition in if statement must be Boolean");
        }
        $$ = new UConditionalStatement;
        ((UConditionalStatement*) $$)->cond = $2;
        ((UConditionalStatement*) $$)->yes = $4;
        ((UConditionalStatement*) $$)->no = new UNopStatement;
    }
| _IF_ expression _THEN_ statement_seq _ELSE_ statement_seq _END_ {
        if ($2->type->tag != boo) {
            yyerror("condition in if statement must be Boolean");
        }
        $$ = new UConditionalStatement;
        ((UConditionalStatement*) $$)->cond = $2;
        ((UConditionalStatement*) $$)->yes = $4;
        ((UConditionalStatement*) $$)->no = $6;
    }
;


return_statement:
  _RETURN_ expression {
        if (!($2->type->iscompatible(CurrentFunction->type))) {
            yyerror("returned value incompatible to function type");
        }
        $$ = new UReturnStatement;
        ((UReturnStatement*) $$)->fct = CurrentFunction;
        ((UReturnStatement*) $$)->exp = $2;
    }
;


exit_statement:
  _EXIT_ {
        $$ = new UExitStatement;
        ((UExitStatement*) $$)->fct = CurrentFunction;
    }
;


case_statement:
  _SWITCH_ expression caselist _END_ {
        unsigned int crd;
        case_list* l;
        for (l = $3, crd = 0; l; l = l->next, crd++) {
            if (!($2->type->iscompatible(l->exp->type))) {
                yyerror("case item incompatible to case expression");
            }
        }
        $$ = new UCaseStatement;
        ((UCaseStatement*) $$)->exp = $2;
        ((UCaseStatement*) $$)->cond = new UExpression* [crd + 10];
        ((UCaseStatement*) $$)->yes = new UStatement* [crd + 10];
        ((UCaseStatement*) $$)->def = new UNopStatement;
        for (l = $3, crd = 0; l; l = l->next, crd++) {
            ((UCaseStatement*) $$)->cond[crd] = l->exp;
            ((UCaseStatement*) $$)->yes[crd] = l->stm;
        }
        ((UCaseStatement*) $$)->card = crd;
    }
| _SWITCH_ expression caselist _ELSE_ statement_seq _END_ {
        unsigned int crd;
        case_list* l;
        for (l = $3, crd = 0; l; l = l->next, crd++) {
            if (!($2->type->iscompatible(l->exp->type))) {
                yyerror("case item incompatible to case expression");
            }
        }
        $$ = new UCaseStatement;
        ((UCaseStatement*) $$)->exp = $2;
        ((UCaseStatement*) $$)->cond = new UExpression* [crd + 10];
        ((UCaseStatement*) $$)->yes = new UStatement* [crd + 10];
        ((UCaseStatement*) $$)->def = $5;
        for (l = $3, crd = 0; l; l = l->next, crd++) {
            ((UCaseStatement*) $$)->cond[crd] = l->exp;
            ((UCaseStatement*) $$)->yes[crd] = l->stm;
        }
        ((UCaseStatement*) $$)->card = crd;
    }
;


caselist:
  /* empty */   { $$ = NULL; }
| case caselist { $1->next = $2; $$ = $1; }
;


case:
  _CASE_ expression _colon_ statement_seq {
        $$ = new case_list;
        $$->exp = $2;
        $$->stm = $4;
        $$->next = NULL;
    }
;


assignment:
  lvalue _equals_ expression {
        if (!($1->type->iscompatible($3->type))) {
            yyerror("incompatible types in assignment");
        }
        $$ = new UAssignStatement;
        ((UAssignStatement*) $$)->left = $1;
        ((UAssignStatement*) $$)->right = $3;
    }
;


lvalue:
  IDENTIFIER {
        VaSymbol* v = (VaSymbol*)(LocalTable->lookup($1));
        if (!v) {
            yyerrors($1, "variable '%s' not defined", _cimportant_($1));
        }
        $$ = new UVarLVal;
        ((UVarLVal*) $$)->var = v->var;
        $$->type = ((UVarLVal*) $$)->var->type;
    }
| lvalue _leftbracket_ expression _rightbracket_ {
        if ($1->type->tag != arr) {
            yyerrors(yytext, "component of '%s' referenced which is not an array", $1);
        }
        if (($3->type->tag != boo) && ($3->type->tag != num) && ($3->type->tag != enu)) {
            yyerror("non-scalar expression for array index");
        }
        $$ = new UArrayLVal;
        $$->type = ((UArrType*)($1->type))->component;
        ((UArrayLVal*) $$)->indextype = ((UArrType*)($1->type))->index;
        ((UArrayLVal*) $$)->parent = $1;
        ((UArrayLVal*) $$)->idx = $3;
    }
| lvalue _dot_ IDENTIFIER {
        RcSymbol* r = (RcSymbol*)(GlobalTable->lookup($3));
        if ((!r) || (r->kind != rc)) {
            yyerrors($3, "record component '%s' unknown", _cimportant_($3));
        }
        if ($1->type->tag != rec) {
            yyerrors($3, "component '%s' of something not a record referenced", _cimportant_($3));
        }
        if (r->index >= ((URecType*)($1->type))->card || (r->type != ((URecType*)($1->type))->component[r->index])) {
            yyerrors($3, "record type does not have a component '%s'", _cimportant_($3));
        }
        $$ = new URecordLVal;
        $$->type = ((URecType*)($1->type))->component[r->index];
        ((URecordLVal*) $$)->parent = $1;
        ((URecordLVal*) $$)->offset = r->index;
    }
;


expression:
  expression _iff_ express {
        if (!deep_compatible($1->type, $3->type, boo)) {
            yyerror("Boolean implication operator applied to non-Boolean operands");
        }
        UDivExpression* e = new UDivExpression;
        e->left = $1;
        e->right = $3;
        e->type = $1->type;
        $$ = e;
    }
| expression _implies_ express {
        if (!deep_compatible($1->type, $3->type, boo)) {
            yyerror("Boolean operator applied to non-Boolean operands");
        }
        USubExpression* e = new USubExpression;
        e->left = $1;
        e->right = $3;
        e->type = $1->type;
        $$ = e;
    }
| express
;


express:
  express _AND_ expre {
        if (!deep_compatible($1->type, $3->type, boo)) {
            yyerror("Boolean conjunction operator applied to non-Boolean operands");
        }
        UMulExpression* e = new UMulExpression;
        e->left = $1;
        e->right = $3;
        e->type = $1->type;
        $$ = e;
    }
| express _OR_ expre {
        if (!deep_compatible($1->type, $3->type, boo)) {
            yyerror("Boolean disjunction operator applied to non-Boolean operands");
        }
        UAddExpression* e = new UAddExpression;
        e->left = $1;
        e->right = $3;
        e->type = $1->type;
        $$ = e;
    }
| expre
;


expre:
  _NOT_ expre {
        if (!deep_compatible($2->type, boo)) {
            yyerror("Boolean negation operator applied to non-Boolean operand");
        }
        UNegExpression* e = new UNegExpression;
        e->left = $2;
        e->type = $2->type;
        $$ = e;
    }
| expr
;


expr:
  expr _equals_ exp {
        UEqualExpression* e = new UEqualExpression;
        e->left = $1;
        e->right = $3;
        e->type = TheBooType;
        $$ = e;
    }
| expr _greaterthan_ exp {
        UGreaterthanExpression* e = new UGreaterthanExpression;
        e->left = $1;
        e->right = $3;
        e->type = TheBooType;
        $$ = e;
    }
| expr _lessthan_ exp {
        ULessthanExpression* e = new ULessthanExpression;
        e->left = $1;
        e->right = $3;
        e->type = TheBooType;
        $$ = e;
    }
| expr _greaterorequal_ exp {
        UGreatereqqualExpression* e = new UGreatereqqualExpression;
        e->left = $1;
        e->right = $3;
        e->type = TheBooType;
        $$ = e;
    }
| expr _lessorequal_ exp {
        ULesseqqualExpression* e = new ULesseqqualExpression;
        e->left = $1;
        e->right = $3;
        e->type = TheBooType;
        $$ = e;
    }
| expr _notequal_ exp {
        UUneqqualExpression* e = new UUneqqualExpression;
        e->left = $1;
        e->right = $3;
        e->type = TheBooType;
        $$ = e;
    }
| exp
;


exp:
 exp _plus_ term {
        if (!deep_compatible($1->type, $3->type, num)) {
            yyerror("integer operator applied to non-integer operands");
        }
        UAddExpression* e = new UAddExpression;
        e->left = $1;
        e->right = $3;
        e->type = $1->type;
        $$ = e;
    }
| exp _minus_ term {
        if (!deep_compatible($1->type, $3->type, num)) {
            yyerror("integer operator applied to non-integer operands");
        }
        USubExpression* e = new USubExpression;
        e->left = $1;
        e->right = $3;
        e->type = $1->type;
        $$ = e;
    }
| term
;


term:
  term _times_ factor {
        if (!deep_compatible($1->type, $3->type, num)) {
            yyerror("integer multiplication operator applied to non-integer operands");
        }
        UMulExpression* e = new UMulExpression;
        e->left = $1;
        e->right = $3;
        e->type = $1->type;
        $$ = e;
    }
| term _divide_ factor {
        if (!deep_compatible($1->type, $3->type, num)) {
            yyerror("integer division operator applied to non-integer operands");
        }
        UDivExpression* e = new UDivExpression;
        e->left = $1;
        e->right = $3;
        e->type = $1->type;
        $$ = e;
    }
| term _MOD_ factor {
        if (!deep_compatible($1->type, $3->type, num)) {
            yyerror("integer modulo operator applied to non-integer operands");
        }
        UModExpression* e = new UModExpression;
        e->left = $1;
        e->right = $3;
        e->type = $1->type;
        $$ = e;
    }
| factor
;


factor:
  _minus_ fac {
        if (!deep_compatible($2->type, num)) {
            yyerror("unary minus operator applied to non-integer operands");
        }
        UNegExpression* e = new UNegExpression;
        e->left = $2;
        e->type = $2->type;
        $$ = e;
    }
| fac
;


fac:
  IDENTIFIER {
        Symbol* s = LocalTable->lookup($1);
        if (s) {
            // s is local variable
            VaSymbol* v = (VaSymbol*)s;
            ULVal* l = new UVarLVal;
            ((UVarLVal*) l)->var = v->var;
            ((UVarLVal*) l)->type = ((UVarLVal*) l)->var->type;
            ULvalExpression* e = new ULvalExpression;
            e->type = l->type;
            e->lval = l;
            $$ = e;
        } else {
            // try global symbol
            s = GlobalTable->lookup($1);
            if (!s) {
                yyerrors($1, "identifier '%s' not defined", _cimportant_($1));
            }
            if (s->kind != en) {
                yyerrors($1, "identifier '%s' of wrong kind", _cimportant_($1));
            }
            EnSymbol* n = (EnSymbol*) s;
            UEnuconstantExpression* e = new UEnuconstantExpression;
            e->type = n->type;
            e->nu = n->ord;
            $$ = e;
        }
    }
| lvalue _dot_ IDENTIFIER {
        RcSymbol* r = (RcSymbol*)(GlobalTable->lookup($3));
        if ((!r) || (r->kind != rc)) {
            yyerror("record component unknown");
        }
        if ($1->type->tag != rec) {
            yyerror("component of something not a record referenced");
        }
        if (r->index >= ((URecType*)($1->type))->card || (r->type != ((URecType*)($1->type))->component[r->index])) {
            yyerror("record type does not have this component");
        }
        URecordLVal* l = new URecordLVal;
        l->type = ((URecType*)($1->type))->component[r->index];
        l->parent = $1;
        l->offset = r->index;
        ULvalExpression* e = new ULvalExpression;
        e->type = l->type;
        e->lval = l;
        $$ = e;
    }
| lvalue _leftbracket_ expression _rightbracket_ {
        if ($1->type->tag != arr) {
            yyerror("component of something not an array referenced");
        }
        if (($3->type->tag != boo) && ($3->type->tag != num) && ($3->type->tag != enu)) {
            yyerror("non-scalar expression for array index");
        }
        UArrayLVal* a = new UArrayLVal;
        a->type = ((UArrType*)($1->type))->component;
        a->parent = $1;
        a->indextype = ((UArrType*)($1->type))->index;
        a->idx = $3;
        ULvalExpression* e = new ULvalExpression;
        e->type = a->type;
        e->lval = a;
        $$ = e;
    }
| _leftparenthesis_ expression _rightparenthesis_ { $$ = $2; }
| _TRUE_ {
        $$ = new UTrueExpression;
        $$->type = TheBooType;
    }
| _FALSE_ {
        $$ = new UFalseExpression;
        $$->type = TheBooType;
    }
| functioncall
| arrayvalue
| NUMBER {
        $$ = new UIntconstantExpression;
        ((UIntconstantExpression*)$$)->nu = atoi($1);
        $$->type = TheNumType;
    }
;


functioncall:
  IDENTIFIER _leftparenthesis_ expressionlist _rightparenthesis_ {
        case_list* c;
        int i;
        FcSymbol* f = (FcSymbol*) GlobalTable->lookup($1);
        if (!f) {
            yyerrors($1, "undefined function '%s' called", _cimportant_($1));
        }
        UCallExpression* e = new UCallExpression;
        e->fct = f->function;
        e->type = f->function->type;
        e->currentpar = new UExpression* [f->function->arity + 10];
        for (i = 0, c = $3; i < f-> function->arity; i++) {
            if (!c) {
                yyerrors(yytext, "too few arguments to function '%s'", _cimportant_($1));
            }
            e->currentpar[i] = c->exp;
            if (!(c->exp->type->iscompatible(f->function->formalpar[i]->type))) {
                yyerrors(yytext, "type mismatch in call parameter of function '%s'", _cimportant_($1));
            }
            c = c->next;
        }
        if (c) {
            yyerrors(yytext, "too many arguments to function '%s'", _cimportant_($1));
        }
        $$ = e;
    }
;


expressionlist:
  /* empty */ { $$ = NULL; }
| expression {
        $$ = new case_list;
        $$->exp = $1;
        $$->next = NULL;
    }
| expression _comma_ expressionlist {
        $$ = new case_list;
        $$->exp = $1;
        $$->next = $3;
    }
;


arrayvalue:
  _leftbracket_ valuelist _rightbracket_ {
        unsigned int h;
        int i;
        case_list* c;

        for (c = $2, h = 0; c; c = c->next, h++) {
            ;
        }
        UNumType* it = new UNumType(1, h);
        UType* ct = $2->exp->type;
        UArrType* at = new UArrType(it, ct);
        UArrayExpression* e = new UArrayExpression;
        e->type = at;
        e->card = h;
        e->cont = new UExpression* [h + 10];
        for (i = 0, c = $2; i < (int)h; i++, c = c->next) {
            e->cont[i] = c->exp;
            if (!(ct->iscompatible(c->exp->type))) {
                yyerror("incompatible types in array value");
            }

        }
        $$ = e;
    }
;


valuelist:
  expression {
        $$ = new case_list;
        $$->exp = $1;
        $$->next = NULL;
    }
| expression _slash_ valuelist {
        $$ = new case_list;
        $$->next = $3;
        $$->exp = $1;
    }
;


amarkinglist:
  amarking
| amarkinglist _comma_ amarking
;


amarking:
  nodeident _colon_ NUMBER {
        PS = (PlSymbol*) PlaceTable->lookup($1);
        if (!PS) {
            yyerrors($1, "place '%s' does not exist", _cimportant_($1));
        }
        if (PS->sort) {
            // HL place, number nicht erlaubt
            yyerrors(yytext, "marking of high-level place '%s' must be a term expression", _cimportant_($1));
        } else {
            // LL place, number ist als Anzahl zu interpretieren
            PS->place->target_marking += atoi($3);
        }
    }
| nodeident _colon_ multiterm {
        char* inst, * ll;
        UTermList* tl;
        UValueList* vl, * currentvl;
        UValue* pv;
        PlSymbol* PSI;
        PS = (PlSymbol*) PlaceTable->lookup($1);
        if (!PS) {
            yyerror("place does not exist");
        }
        if (!PS->sort) {
            yyerrors($1, "multiterm expression not allowed for low-level place '%s'", _cimportant_($1));
        }
        pv = PS->sort->make();
        for (tl = $3; tl; tl = tl->next) { // do for all mt components
            // check type compatibility
            if (!(PS->sort->iscompatible(tl->t->type))) {
                yyerror("marking expression not compatible to sort of place");
            }
            vl = tl->t->evaluate();
            for (currentvl = vl; currentvl; currentvl = currentvl->next) {
                pv->assign(currentvl->val); // type adjustment
                inst = pv->text();
                ll = new char [strlen($1) + strlen(inst) + 20];
                strcpy(ll, $1);
                strcpy(ll + strlen($1), ".");
                strcpy(ll + (strlen($1) + 1), inst);
                PSI = (PlSymbol*) PlaceTable->lookup(ll);
                if (!PSI) {
                    yyerror("place instance does not exist");
                }
                PSI->place->target_marking += tl->mult;
            }
        }
    }
;


net:
  _PLACE_ placelists _semicolon_ _MARKING_ {LocalTable = NULL; }
  markinglist _semicolon_ transitionlist {
        unsigned int i, h, j;
        Symbol* ss;
        // Create array of places
        Places = new Place* [PlaceTable->card + 10];
        CurrentMarking = new unsigned int [PlaceTable->card + 10];
        i = 0;
        for (h = 0; h < PlaceTable->size; h++) {
            for (ss = PlaceTable->table[h]; ss; ss = ss->next) {
                if (!(((PlSymbol*) ss)->sort)) {
                    Places[i++] = ((PlSymbol*) ss)->place;
                }
            }
        }
        PlaceTable->card = i;
#ifdef WITHFORMULA
        for (i = 0; i < PlaceTable->card; i++) {
            Places[i]->propositions = NULL;
        }
#endif
        // Create array of transitions
        Transitions = new Transition* [TransitionTable->card + 10];
        i = 0;
        for (h = 0; h < TransitionTable->size; h++) {
            for (ss = TransitionTable->table[h]; ss; ss = ss->next) {
                if (!(((TrSymbol*) ss)->vars)) {
                    Transitions[i++] = ((TrSymbol*) ss)->transition;
                }
            }
        }
        TransitionTable->card = i;
        // Create arc list of places pass 1 (count nr of arcs)
        for (i = 0; i < TransitionTable->card; i++) {
            for (j = 0; j < Transitions[i]->NrOfArriving; j++) {
                Transitions[i]->ArrivingArcs[j]->pl->NrOfLeaving++;
            }
            for (j = 0; j < Transitions[i]->NrOfLeaving; j++) {
                Transitions[i]->LeavingArcs[j]->pl->NrOfArriving++;
            }
        }
        // pass 2 (allocate arc arrays)
        for (i = 0; i < PlaceTable->card; i++) {
            Places[i]->ArrivingArcs = new Arc * [Places[i]->NrOfArriving + 10];
            Places[i]->NrOfArriving = 0;
            Places[i]->LeavingArcs = new Arc * [Places[i]->NrOfLeaving + 10];
            Places[i]->NrOfLeaving = 0;
        }
        // pass 3 (fill in arcs)
        for (i = 0; i < TransitionTable->card; i++) {
            for (j = 0; j < Transitions[i]->NrOfLeaving; j++) {
                Place* pl;
                pl = Transitions[i]->LeavingArcs[j]->pl;
                pl->ArrivingArcs[pl->NrOfArriving] = Transitions[i]->LeavingArcs[j];
                pl->NrOfArriving ++;
            }
            for (j = 0; j < Transitions[i]->NrOfArriving; j++) {
                Place* pl;
                pl = Transitions[i]->ArrivingArcs[j]->pl;
                pl->LeavingArcs[pl->NrOfLeaving] = Transitions[i]->ArrivingArcs[j];
                pl->NrOfLeaving ++;
            }
        }
        for (i = 0; i < TransitionTable->card; i++) {
#ifdef STUBBORN
            Transitions[i]->mustbeincluded = Transitions[i]->conflicting;
#if defined(EXTENDED) && defined(MODELCHECKING)
            Transitions[i]->lstfired = new unsigned int [10];
            Transitions[i]->lstdisabled = new unsigned int [10];
#endif
#endif
        }
#if defined(EXTENDED) && defined(MODELCHECKING)
        formulaindex = 0;
        currentdfsnum = 0;
#endif
        // initialize places
#ifdef STUBBORN
        for (i = 0; i < PlaceTable->card; i++) {
            Places[i]->initialize();
        }
#endif
        Transitions[0]->StartOfEnabledList = Transitions[0];
        // The following pieces of code initialize static attractor sets for
        // various problems.
#ifdef BOUNDEDNET
#ifdef STUBBORN
        // initialize list of pumping transitions
        LastAttractor = NULL;
        int p, c, a; // produced, consumed tokens, current arc
        for (i = 0; i < TransitionTable->card; i++) {
            // count produced tokens
            for (a = 0, p = 0; a < Transitions[i]->NrOfLeaving; a++) {
                p += Transitions[i]->LeavingArcs[a]->Multiplicity;
            }
            // count consumed tokens
            for (a = 0, c = 0; a < Transitions[i]->NrOfArriving; a++) {
                c += Transitions[i]->ArrivingArcs[a]->Multiplicity;
            }
            if (p > c) {
                Transitions[i]->instubborn = true;
                if (LastAttractor) {
                    Transitions[i]->NextStubborn =
                        Transitions[i]->StartOfStubbornList;
                    Transitions[i]->StartOfStubbornList = Transitions[i];
                } else {
                    Transitions[i]->StartOfStubbornList = LastAttractor = Transitions[i];
                    Transitions[i]->NextStubborn = NULL;
                }
            }
        }

#endif
#endif
    }
;


placelists:
  capacity placelist 
| placelists _semicolon_ capacity placelist
;


capacity:
  /* empty */           { CurrentCapacity = CAPACITY; }
| _SAFE_ _colon_        { CurrentCapacity = 1; }
| _SAFE_ NUMBER _colon_ { CurrentCapacity = atoi($2); }
;


placelist:
  placelist _comma_ place 
| place 
;


place:
  nodeident {
        if (PlaceTable->lookup($1)) {
            yyerrors($1, "place '%s' name used twice", _cimportant_($1));
        }
        P = new Place($1);
        PS = new PlSymbol(P);
        PS->sort = NULL;
        P->capacity = CurrentCapacity;
        P->nrbits = CurrentCapacity > 0 ? logzwo(CurrentCapacity) : 32;
    }
| nodeident _colon_ sortdescription {
        // high level place: unfold to all instances
        char* c;
        if (PlaceTable->lookup($1)) {
            yyerrors($1, "place '%s' name used twice", _cimportant_($1));
        }
        c = new char [strlen($1) + 10];
        strcpy(c, $1);
        PS =  new PlSymbol(c);
        PS->sort = $3;
        UValue* v;
        v = $3->make();
        do {
            char* lowlevelplace;
            char* lowtag;
            lowtag = v->text();
            lowlevelplace = new char [ strlen(c) + strlen(lowtag) + 20];
            strcpy(lowlevelplace, c);
            strcpy(lowlevelplace + strlen(c), ".");
            strcpy(lowlevelplace + strlen(c) + 1, lowtag);
            if (PlaceTable->lookup(lowlevelplace)) {
                yyerror("place instance name already used");
            }
            P = new Place(lowlevelplace);
            P->capacity = CurrentCapacity;
            P->nrbits = CurrentCapacity > 0 ? logzwo(CurrentCapacity) : 32;
            PS = new PlSymbol(P);
            PS->sort = NULL;
            (*v)++;
        } while (!(v->isfirst()));
    }
;


nodeident:
  IDENTIFIER  { $$ = $1; }
| NUMBER { $$ = $1; }
;


markinglist:
  /* empty */
| marking
| markinglist _comma_ marking
;


marking:
  nodeident _colon_ NUMBER {
        PS = (PlSymbol*) PlaceTable->lookup($1);
        if (!PS) {
            yyerrors($1, "place '%s' does not exist", _cimportant_($1));
        }
        if (PS->sort) {
            // HL place, number nicht erlaubt
            yyerrors(yytext, "marking of high-level place '%s' must be a term expression", _cimportant_($1));
        } else {
            // LL place, number ist als Anzahl zu interpretieren
            *(PS->place) += atoi($3);
        }
    }
| nodeident _colon_ multiterm {
        UValueList* vl, * currentvl;
        PlSymbol* PSI;
        PS = (PlSymbol*) PlaceTable->lookup($1);
        if (!PS) {
            yyerror("place does not exist");
        }
        if (!PS->sort) {
            yyerrors($1, "multiterm expression not allowed for low-level place '%s'", _cimportant_($1));
        }
        UValue* pv = PS->sort->make();
        for (UTermList* tl = $3; tl; tl = tl->next) { // do for all mt components
            // check type compatibility
            if (!(PS->sort->iscompatible(tl->t->type))) {
                yyerrors(yytext, "marking expression not compatible to sort of place '%s'", _cimportant_($1));
            }
            vl = tl->t->evaluate();
            for (currentvl = vl; currentvl; currentvl = currentvl->next) {
                pv->assign(currentvl->val); // type adjustment
                char* inst = pv->text();
                char* ll = new char [strlen($1) + strlen(inst) + 20];
                strcpy(ll, $1);
                strcpy(ll + strlen($1), ".");
                strcpy(ll + (strlen($1) + 1), inst);
                PSI = (PlSymbol*) PlaceTable->lookup(ll);
                if (!PSI) {
                    yyerror("place instance does not exist");
                }
                (* PSI->place) += tl->mult;
            }
        }
    }
;


multiterm:
  mtcomponent { $$ = $1; }
| multiterm _plus_ mtcomponent { $3->next = $1; $$ = $3; }
;


mtcomponent:
  hlterm { $$ = $1; } 
| hlterm _colon_ NUMBER {
        $1->mult = atoi($3);
        $$ = $1;
    }
;


hlterm:
  IDENTIFIER {
        if (!LocalTable) {
            yyerror("only constant terms are allowed in this context");
        }
        VS = (VaSymbol*)(LocalTable->lookup($1));
        if (!VS) {
            yyerrors($1, "undeclared variable '%s' in term", _cimportant_($1));
        }
        UTermList* tl = new UTermList;
        tl->next = NULL;
        tl->mult = 1;
        UVarTerm* vt = new UVarTerm;
        tl->t = vt;
        vt->v = VS->var;
        vt->type = VS->var->type;
        $$ = tl;
    }
| IDENTIFIER _leftparenthesis_ termlist _rightparenthesis_ {
        FcSymbol* FS = (FcSymbol*) GlobalTable->lookup($1);
        if (!FS) {
            yyerrors($1, "operation symbol '%s' not declared", _cimportant_($1));
        }
        if (FS->kind != fc) {
            yyerror("wrong symbol used as operation symbol");
        }
        unsigned int i;
        UTermList* l;
        for (i = 0, l = $3; l; i++, l = l->next) {
            ;
        }
        if ((int)i != FS->function->arity) {
            yyerrors(yytext, "function '%s' needs exactly %d arguments", _cimportant_($1), FS->function->arity);
        }
        UTermList* tl = new UTermList;
        tl->next = NULL;
        tl->mult = 1;
        UOpTerm* ot = new UOpTerm;
        tl->t = ot;
        ot->arity = i;
        ot->f = FS->function;
        ot->sub = new UTerm* [i + 5];
        for (i = 0, l = $3; i < ot->arity; i++, l = l->next) {
            if (!(ot->f->formalpar[i]->type->iscompatible(l->t->type))) {
                yyerror("type mismatch in subterm(s)");
            }
            ot->sub[i] = l->t;
        }
        ot->type = ot->f->type;
        $$ = tl;
    }
;


termlist:
  /* empty */             { $$ = NULL; }
| hlterm                  { $$ = $1; }
| hlterm _comma_ termlist { $1->next = $3; $$ = $1; }
;


transitionlist:
  transition
| transitionlist transition
;


transition:
  _TRANSITION_ tname {
        if (TransitionTable->lookup($2)) {
            yyerrors($2, "transition name '%s' used twice", _cimportant_($2));
        }
    }
  transitionvariables guard _CONSUME_ arclist _semicolon_ _PRODUCE_ arclist _semicolon_ {
        unsigned int card;
        unsigned int i;
        arc_list* current;
        /* 1. Transition anlegen */
        TS = new TrSymbol($2);
        TS->vars = LocalTable;
        TS->guard = $5;

        // unfold HL transitions
        //->create transition instance for every assignment to the variables that
        //    matches the guard


        // init: initially, all variables have initial value

        while (1) {
            if (TS->guard) {
                UValue* v;
                v = TS->guard->evaluate();
                if (((UBooValue*)v)->v == false) {
                    goto nextass;
                }
            }
            // A) create LL transition with current variable assignment

            /* generate name */
            char* llt;
            if ((!LocalTable) || LocalTable->card == 0) {
                llt = TS->name;
                TS->vars = NULL;
            } else {
                char** assignment;
                unsigned int len;
                len = 0;
                assignment = new char * [LocalTable->card + 1000];
                VaSymbol* vs;
                unsigned int i, j;
                j = 0;
                for (i = 0; i < LocalTable->size; i++) {
                    for (vs = (VaSymbol*)(LocalTable->table[i]); vs ; vs = (VaSymbol*) vs->next) {
                        char* inst;
                        inst = vs->var->value->text();
                        assignment[j] = new char[strlen(vs->name) + 10 + strlen(inst)];
                        strcpy(assignment[j], vs->name);
                        strcpy(assignment[j] + strlen(vs->name), "=");
                        strcpy(assignment[j] + strlen(vs->name) + 1, inst);
                        len += strlen(assignment[j++]);
                    }
                }
                llt = new char [ strlen(TS->name)  + len + LocalTable->card + 1000];
                strcpy(llt, TS->name);
                strcpy(llt + strlen(llt), ".[");
                for (i = 0; i < LocalTable->card; i++) {
                    strcpy(llt + strlen(llt), assignment[i]);
                    strcpy(llt + strlen(llt), "|");
                }
                strcpy(llt + (strlen(llt) - 1), "]");
            }
            TrSymbol* TSI;
            if ((!LocalTable) || LocalTable->card == 0) {
                TSI = TS;
            } else {
                TSI = (TrSymbol*) TransitionTable->lookup(llt);
                if (TSI) {
                    yyerror("transition instance already exists");
                }
                TSI = new TrSymbol(llt);
                TSI->vars = NULL;
                TSI->guard = NULL;
            }
            T = TSI->transition = new Transition(TSI->name);
            T->fairness = $4;
            /* 2. Inliste eintragen */
            /* HL-Boegen in LL-Boegen uebersetzen und zur Liste hinzufuegen */
            arc_list* root;
            root = $7;
            for (current = root; current; current = current->next) {
                if (current->mt) {
                    // traverse multiterm
                    arc_list* a;
                    UTermList* mc;
                    UValueList* vl;
                    UValueList* vc;
                    UValue* pv;
                    pv = current->place->sort->make();
                    for (mc = current->mt; mc ; mc = mc->next) {
                        vl = mc->t->evaluate();

                        for (vc = vl; vc; vc  = vc->next) {
                            char* inst;
                            char* ll;
                            pv->assign(vc->val);
                            inst = pv->text();
                            ll = new char [strlen(current->place->name) + strlen(inst) + 20];
                            strcpy(ll, current->place->name);
                            strcpy(ll + strlen(current->place->name), ".");
                            strcpy(ll + strlen(current->place->name) + 1, inst);
                            PS = (PlSymbol*) PlaceTable->lookup(ll);
                            if (!ll) {
                                yyerror("place instance does not exist");
                            }
                            if (PS->sort) {
                                yyerror("arcs to high-level places are not allowed");
                            }
                            a = new arc_list;
                            a->place = PS;
                            a->mt = NULL;
                            a->nu = mc->mult;
                            a->next = root;
                            root = a;
                        }
                    }
                }
            }
            /* Anzahl der Boegen */
            for (card = 0, current = root; current; card++, current = current->next) {
                ;
            }
            T->ArrivingArcs = new Arc * [card + 10];
            /* Schleife ueber alle Boegen */
            for (current = root; current; current = current->next) {
                /* Bogen ist nur HL-Bogen */
                if (current->place->sort) {
                    continue;
                }
                /* Vielfachheit 0 */
                if (current->nu == 0) {
                    continue;
                }
                /* gibt es Bogen schon? */

                for (i = 0; i < T->NrOfArriving; i++) {
                    if (current->place->place == T->ArrivingArcs[i]->pl) {
                        /* Bogen existiert, nur Vielfachheit addieren */
                        *(T->ArrivingArcs[i]) += current->nu;
                        break;
                    }
                }
                if (i >= T->NrOfArriving) {
                    T->ArrivingArcs[T->NrOfArriving] = new Arc(T, current->place->place, true, current->nu);
                    T->NrOfArriving++;
                    current->place->place->references ++;
                }
            }
            /* 2. Outliste eintragen */
            root = $10;
            for (current = root; current; current = current->next) {
                if (current->mt) {
                    // traverse multiterm
                    arc_list* a;
                    UTermList* mc;
                    UValueList* vl;
                    UValueList* vc;
                    UValue* pv;
                    pv = current->place->sort->make();
                    for (mc = current->mt; mc ; mc = mc->next) {
                        vl = mc->t->evaluate();
                        for (vc = vl; vc; vc  = vc->next) {
                            char* inst;
                            char* ll;
                            pv->assign(vc->val);
                            inst = pv->text();
                            ll = new char [strlen(current->place->name) + strlen(inst) + 20];
                            strcpy(ll, current->place->name);
                            strcpy(ll + strlen(current->place->name), ".");
                            strcpy(ll + strlen(current->place->name) + 1, inst);
                            PS = (PlSymbol*) PlaceTable->lookup(ll);
                            if (!ll) {
                                yyerror("place instance does not exist");
                            }
                            a = new arc_list;
                            a->place = PS;
                            a->mt = NULL;
                            a->nu = mc->mult;
                            a->next = root;
                            root = a;
                        }
                    }
                }
            }
            /* Anzahl der Boegen */
            for (card = 0, current = root; current; card++, current = current->next) {
                ;
            }
            T->LeavingArcs = new Arc* [card + 10];
            /* Schleife ueber alle Boegen */
            for (current = root; current; current = current->next) {
                /* Bogen ist nur HL-Bogen */
                if (current->place->sort) {
                    continue;
                }
                /* Vielfachheit 0 */
                if (current->nu == 0) {
                    continue;
                }
                /* gibt es Bogen schon? */

                for (i = 0; i < T->NrOfLeaving; i++) {
                    if (current->place->place == T->LeavingArcs[i]->pl) {
                        /* Bogen existiert, nur Vielfachheit addieren */
                        *(T->LeavingArcs[i]) += current->nu;
                        break;
                    }
                }
                if (i >= T->NrOfLeaving) {
                    T->LeavingArcs[T->NrOfLeaving] = new Arc(T, current->place->place, false, current->nu);
                    T->NrOfLeaving++;
                    current->place->place->references ++;
                }
            }
// B) switch to next assignment
nextass:
            if ((!LocalTable) || LocalTable->card == 0) {
                break;
            }
            unsigned int k;
            VaSymbol* vv;
            for (k = 0; k < LocalTable->size; k++) {
                for (vv = (VaSymbol*)(LocalTable->table[k]); vv; vv = (VaSymbol*)(vv->next)) {
                    (*(vv->var->value)) ++;
                    if (!(vv->var->value->isfirst())) {
                        break;
                    }
                }
                if (vv) {
                    break;
                }
            }
            if (!vv) {
                break;
            }
        }
    }
;


transitionvariables:
  vardeclarations                 { $$ = 0; }
| _WEAK_ _FAIR_ vardeclarations   { $$ = 1; }
| _STRONG_ _FAIR_ vardeclarations { $$ = 2; }
;


tname:
  IDENTIFIER { LocalTable = new SymbolTab(256); }
| NUMBER     { LocalTable = new SymbolTab(256); }
;


guard:
  /* empty */ { $$ = NULL; }
| _GUARD_ expression {
        if ($2->type->tag != boo) {
            yyerror("guard expression must be Boolean");
        }
        $$ = $2;
    }
;


arclist:
  /* empty */       { $$ = NULL; }
| arc               { $$ = $1; }
| arc _comma_ arclist { $1->next = $3; $$ = $1; }
;


arc:
  nodeident _colon_ NUMBER {
        PS = (PlSymbol*) PlaceTable->lookup($1);
        if (!PS) {
            yyerrors($1, "place '%s' does not exist", _cimportant_($1));
        }
        if (PS->sort) {
            yyerrors(yytext, "arc expression of high level place '%s' must be term expressions", _cimportant_($1));
        }
        $$ = new arc_list;
        $$->place = PS;
        $$->next = (arc_list*)  0;
        $$->nu = atoi($3);
        $$->mt = NULL;
    }
| nodeident _colon_ multiterm {
        PS = (PlSymbol*) PlaceTable->lookup($1);
        if (!PS) {
            yyerrors($1, "place '%s' does not exist", _cimportant_($1));
        }
        if (!(PS->sort)) {
            yyerrors($1, "low-level place '%s' requires numerical multiplicity", _cimportant_($1));
        }
        $$ = new arc_list;
        $$->place = PS;
        $$->nu = 0;
        $$->mt = $3;
        $$->next = NULL;
        for (UTermList* tl = $3; tl; tl = tl->next) {
            if (!(PS->sort->iscompatible(tl->t->type))) {
                yyerrors($1, "type mismatch between place '%s' and arc expression", _cimportant_($1));
            }
        }
    }
;


numex:
  NUMBER { 
        $$ = new UIntconstantExpression();
        ((UIntconstantExpression*)$$)->nu = atoi($1);
    }
| _leftparenthesis_ expression _rightparenthesis_ {
        if ($2->type->tag != num) {
            yyerror("integer expression expected");
        }
        $$ = $2;
    }
;


ctlformula:
  cplace _equals_ numex {
        ((hlatomicformula*) $1)->k = $3;
        $1->type = eq;
        $$ = $1;
    }
| cplace _notequal_ numex {
        ((hlatomicformula*) $1)->k = $3;
        $1->type = neq;
        $$ = $1;
    }
| cplace _lessorequal_ numex {
        ((hlatomicformula*) $1)->k = $3;
        $1->type = leq;
        $$ = $1;
    }
| cplace _greaterorequal_ numex {
        ((hlatomicformula*) $1)->k = $3;
        $1->type = geq;
        $$ = $1;
    }
| cplace _lessthan_ numex {
        ((hlatomicformula*) $1)->k = $3;
        $1->type = lt;
        $$ = $1;
    }
| cplace _greaterthan_ numex {
        ((hlatomicformula*) $1)->k = $3;
        $1->type = gt;
        $$ = $1;
    }
| ctlformula _AND_ ctlformula {
        $$ = new binarybooleanformula(conj, $1, $3);
    }
| ctlformula _OR_ ctlformula {
        $$ = new binarybooleanformula(disj, $1, $3);
    }
| _NOT_ ctlformula {
        $$ = new unarybooleanformula(neg, $2);
     }
| _leftbracket_ expression _rightbracket_ {
        if ($2->type->tag != boo) {
            yyerror("formula requires Boolean expression");
        }
        $$ = new staticformula($2);
    }
| _EXISTS_ quantification _colon_ ctlformula {
        $2->name[0] = '0';
        $$ = new quantifiedformula(qe, $2->var, $4);
    }
| _ALL_ quantification _colon_ ctlformula {
        $2->name[0] = '0';
        $$ = new quantifiedformula(qa, $2->var, $4);
    }
| _leftparenthesis_ ctlformula _rightparenthesis_ {
        $$ = $2;
    }
| _EXPATH_ transformula _leftbracket_ ctlformula _UNTIL_ ctlformula _rightbracket_ {
        $$ = new untilformula(eu, $4, $6, (transitionformula*) $2);
    }
| _ALLPATH_ transformula _leftbracket_ ctlformula _UNTIL_ ctlformula _rightbracket_ {
        $$ = new untilformula(au, $4, $6, (transitionformula*) $2);
    }
| _EXPATH_ transformula _ALWAYS_ ctlformula {
        $$ = new unarytemporalformula(eg, $4, (transitionformula*) $2);
    }
| _ALLPATH_ transformula _ALWAYS_ ctlformula  {
        $$ = new unarytemporalformula(ag, $4, (transitionformula*) $2);
    }
| _EXPATH_ transformula _NEXT_ ctlformula {
        $$ = new unarytemporalformula(ex, $4, (transitionformula*) $2);
    }
| _ALLPATH_ transformula _NEXT_ ctlformula {
        $$ = new unarytemporalformula(ax, $4, (transitionformula*) $2);
    }
| _EXPATH_ transformula _EVENTUALLY_ ctlformula  {
        $$ = new unarytemporalformula(ef, $4, (transitionformula*) $2);
    }
| _ALLPATH_ transformula _EVENTUALLY_ ctlformula  {
        $$ = new unarytemporalformula(af, $4, (transitionformula*) $2);
    }
;


cplace:
  nodeident {
        PS = (PlSymbol*) PlaceTable->lookup($1);
        if (!PS) {
            yyerrors($1, "place '%s' does not exist", _cimportant_($1));
        }
        if (PS->sort) {
            yyerrors($1, "high-level place '%s' requires an instance", _cimportant_($1));
        }
        $$ = new hlatomicformula(neq, PS, NULL);
    }
| nodeident _dot_ _leftparenthesis_ expression _rightparenthesis_ {
        PS = (PlSymbol*) PlaceTable->lookup($1);
        if (!PS) {
            yyerrors($1, "place '%s' does not exist", _cimportant_($1));
        }
        if (!(PS->sort)) {
            yyerrors($1, "low-level place '%s' does not require an instance", _cimportant_($1));
        }
        if (!(PS->sort->iscompatible($4->type))) {
            yyerrors($1, "place color incompatible to sort of place '%s'", _cimportant_($1));
        }
        $$ = new hlatomicformula(neq, PS, $4);
    }
;


quantification:
  nodeident _colon_ sortdescription {
        VS = (VaSymbol*) LocalTable->lookup($1);
        if (VS) {
            yyerrors($1, "variable used '%s' twice in formula", _cimportant_($1));
        }
        UVar* vv = new UVar($3);
        $$ = new VaSymbol($1, vv);
    }
;


transformula:
  /* empty */       { $$ = NULL; }
| transitionformula { $$ = $1; }
;


transitionformula:
  _EXISTS_ quantification _colon_ transitionformula {
        $2->name[0] = '\0';
        $$ = new quantifiedformula(qe,$2->var,$4);
    }
| _ALL_ quantification _colon_ transitionformula {
        $2->name[0] = '\0';
        $$ = new quantifiedformula(qa,$2->var,$4);
    }
| transitionformula _AND_ transitionformula {
        $$ = new binarybooleanformula(conj, $1, $3);
    }
| transitionformula _OR_ transitionformula {
        $$ = new binarybooleanformula(disj, $1, $3);
    }
| _NOT_ transitionformula {
        $$ = new unarybooleanformula(neg,$2);
    }
| _leftparenthesis_ transitionformula _rightparenthesis_ { $$ = $2; }
| formulatransition {
#if defined(STATESPACE) && defined(STUBBORN)
        $1->transition->visible = true;
        //                    std::cerr << "visible transition: " << $1->name << "n";
#endif
        if ($1->vars && $1->vars->card) {
            yyerrors($1->name, "high-level transition '%s' requires firing mode", _cimportant_($1->name));
        }
        $$ = new transitionformula($1->transition);
    }
| formulatransition _dot_ _leftbracket_ parfiringmode _rightbracket_ {
#if defined(STATESPACE) && defined(STUBBORN)
        $1->transition->visible = true;
        //                    std::cerr << "visible transition: " << $1->name << "n";
#endif
        if ((! $1->vars) || ($1->vars->card == 0)) {
            yyerrors($1->name, "low-level transition '%s' does not require firing mode", _cimportant_($1->name));
        }
        $$ = new transitionformula($1, $4);
    }
;


formulatransition:
  nodeident {
        TS = (TrSymbol*) TransitionTable->lookup($1);
        if (!TS) {
            yyerrors($1, "transition '%s' does not exist", _cimportant_($1));
        }
        $$ = TS;
    }
;


parfiringmode:
  fmodeblock                       { $$ = $1; }
| parfiringmode _slash_ fmodeblock { $1->next = $3; $$ = $1; }
;


fmodeblock:
  nodeident _equals_ expression {
        VS = (VaSymbol*)(TS->vars->lookup($1));
        if (!VS) {
            yyerrors($1, "no such transition variable '%s'", _cimportant_($1));
        }
        if (!(VS->var->type->iscompatible($3->type))) {
            yyerror("variable binding incompatible with variable type");
        }
        $$ = new fmode;
        $$->v = VS;
        $$->t = $3 ;
        $$->next = NULL;
    }
;


automaton:
    {
#ifdef WITHFORMULA
        BuchiTable = new SymbolTab(10);
    } statepart finalpart transitionpart {
        {
            buchitransition* bt;
            for (int i = 0; i < buchistate::nr; i++) {
                buchistate* b = buchiautomaton[i];
                b->delta = new buchitransition * [b->nrdelta + 1];
                for (int j = 0, bt = b->transitionlist; bt; bt = bt->next, j++) {
                    b->delta[j] = bt;
                }
            }
        }
        // process all guard formulas
        int i, j, res;
        for (i = 0; i < Places[0]->NrSignificant; i++) {
            Places[i]->cardprop = 0;
            Places[i]->propositions = NULL;
        }
        for (i = 0; i < buchistate::nr; i++)
            for (j = 0; j < buchiautomaton[i]->nrdelta; j++) {
                buchiautomaton[i]->delta[j]->guard = buchiautomaton[i]->delta[j]->guard->replacequantifiers();
                buchiautomaton[i]->delta[j]->guard->tempcard = 0;
                buchiautomaton[i]->delta[j]->guard = buchiautomaton[i]->delta[j]->guard->merge();
                buchiautomaton[i]->delta[j]->guard = buchiautomaton[i]->delta[j]->guard->reduce(&res);
                if (res == 0) {
                    buchiautomaton[i]->delta[j] = NULL;
                }
                buchiautomaton[i]->delta[j]->guard = buchiautomaton[i]->delta[j]->guard->posate();
                buchiautomaton[i]->delta[j]->guard->tempcard = 0;
            }
        for (i = 0; i < buchistate::nr; i++)
            for (j = 0; j < buchiautomaton[i]->nrdelta; j++) {
                buchiautomaton[i]->delta[j]->guard->setstatic();
                if (buchiautomaton[i]->delta[j]->guard->tempcard) {
                    yyerror("temporal operators not allowed in Buchi automaton");
                }
            }

#endif
    }
;


statepart:
  _STATE_ statelist _semicolon_ {
        buchiautomaton = new buchistate* [buchistate::nr];
        for (buchistate* b = initialbuchistate; b; b = b->next) {
            buchiautomaton[b->code] = b;
        }
    }
;


statelist:
  statelist _comma_ IDENTIFIER {
        StSymbol* s ;
        if ((s = (StSymbol*)(BuchiTable->lookup($3)))) {
            yyerrors($3, "state name '%s' in Buchi automaton used twice", _cimportant_($3));
        }
        s = new StSymbol($3);
        if (!initialbuchistate) {
            initialbuchistate = s->state;
        } else {
            s->state->next = initialbuchistate->next;
            initialbuchistate->next = s->state;
        }
    }
| IDENTIFIER {
        StSymbol* s;
        if ((s = (StSymbol*)(BuchiTable->lookup($1)))) {
            yyerrors($1, "state name '%s' in Buchi automaton used twice", _cimportant_($1));
        }
        s = new StSymbol($1);
        if (!initialbuchistate) {
            initialbuchistate = s->state;
        } else {
            s->state->next = initialbuchistate->next;
            initialbuchistate->next = s->state;
        }
    }
;


finalpart:
  _FINAL_ finallist _semicolon_ 
;


finallist:
  finallist _comma_ IDENTIFIER {
        StSymbol* s = (StSymbol*) BuchiTable->lookup($3);
        if (!s) {
            yyerrors($3, "state '%s' does not exist", _cimportant_($3));
        }
        s->state->final = 1;
    }
| IDENTIFIER {
        StSymbol* s = (StSymbol*) BuchiTable->lookup($1);
        if (!s) {
            yyerrors($1, "state '%s' does not exist", _cimportant_($1));
        }
        s->state->final = 1;
    }
;


transitionpart:
  /* empty */
| transitionpart btransition 
;


btransition:
  _TRANSITION_ IDENTIFIER _TO_ IDENTIFIER _GUARD_ {LocalTable = new SymbolTab(2); } ctlformula {
        StSymbol* from = (StSymbol*) BuchiTable->lookup($2);
        if (!from) {
            yyerrors($2, "state '%s' does not exist", _cimportant_($2));
        }
        StSymbol* to = (StSymbol*) BuchiTable->lookup($4);
        if (!to) {
            yyerrors($4, "state '%s' does not exist", _cimportant_($4));
        }
        buchitransition* bt = new buchitransition;
        bt->next = from->state->transitionlist;
        from->state->transitionlist = bt;
        (from->state->nrdelta)++;
        bt->delta = to->state;
        bt->guard = $7;
    }
;


%%

char * diagnosefilename;


void readnet() {
    yydebug = 0;
    diagnosefilename = NULL;
    if (netfile) {
        yyin = fopen(netfile, "r");
        if (!yyin) {
            abort(4, "cannot open net file '%s'", _cfilename_(netfile));
        }
        diagnosefilename = netfile;
    }
    GlobalTable = new SymbolTab(1024);
    TheBooType = new UBooType();
    TheNumType = new UNumType(0, INT_MAX);
    yyparse();
    for (unsigned int ii = 0; ii < Places[0]->cnt; ii++) {
        CurrentMarking[ii] = Places[ii]->initial_marking;
        Places[ii]->index = ii;
    }
    if (F) {
        F = F->replacequantifiers();
        F->tempcard = 0;
        F = F->merge();
#if defined(MODELCHECKING)
        unsigned int i;
        for (i = 0; i < Transitions[0]->cnt; i++) {
            Transitions[i]->lstfired = new unsigned int [F->tempcard];
            Transitions[i]->lstdisabled = new unsigned int [F->tempcard];
        }
#endif
    }
}


/// display a parser error and exit
void yyerrors(char* token, char const* format, ...) {
    fprintf(stderr, "%s: %s:%d - ", _ctool_(PACKAGE), _cfilename_(basename(diagnosefilename)), yylineno);

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
