%{
#include "dimensions.H"
#include "net.H"
#include "graph.H"
#include "symboltab.H"
#include "formula.H"
#include "buchi.H"
#include "unfold.H"
#include <cstdio>
#include <limits.h>

extern UBooType * TheBooType;
extern UNumType * TheNumType;
extern char *yytext;

#define YYDEBUG 1
void yyerror(char const *);

class arc_list
{
 public:
	PlSymbol * place;
	UTermList * mt;
	unsigned int nu;
    arc_list    * next;
};   

class case_list
{
	public:
	UStatement * stm;
	UExpression * exp;
	case_list * next;
};

/* list of places and multiplicities to become arcs */

int CurrentCapacity;
UFunction * CurrentFunction;
Place *P;
Transition *T;
Symbol * S;
PlSymbol * PS;
TrSymbol * TS;
SymbolTab * GlobalTable;
SymbolTab * LocalTable;
UVar * V;
VaSymbol * VS;
%}
%union
{
	char * str;
	int value;
	UType * t;
	URcList * rcl;
	UEnList * el;
	ULVal * lval;
	int * exl;
	UStatement * stm;
	case_list * cl;
	UFunction * fu;
	UExpression * ex;
	arc_list * al;
	formula * form;
	IdList * idl;
	UTermList * tlist;
	Place * pl;
	Transition * tr;
	fmode * fm;
	TrSymbol * ts;
	VaSymbol * varsy;
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
%type <str> ident
%type <str> eqqual
%type <str> comma
%type <str> number
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

	
%token key_final key_automaton key_safe key_next key_analyse key_place key_marking key_transition key_consume key_produce comma colon semicolon ident number eqqual tand tor exists forall globally future until tnot tgeq tgt tleq tlt tneq key_formula lpar rpar key_state key_path key_generator key_record key_end key_sort key_function key_do key_array key_enumerate key_constant key_boolean key_of key_begin key_while key_if key_then key_else key_switch key_case key_repeat key_for key_to key_all key_exit key_return key_true key_false key_mod key_var key_guard tiff timplies lbrack rbrack dot pplus mminus times divide slash key_exists key_strong key_weak key_fair
%nonassoc tgeq tgt tleq tlt tneq eqqual
%left tnot exists forall globally future key_next
%left tand tor pplus mminus times divide key_mod
%left until
%nonassoc colon
%{
extern YYSTYPE yylval;

//// 1 LINE REMOVED BY NIELS //// #include"lex.yy.c"
//// 3 LINES ADDED BY NIELS
extern int yylex();
extern FILE *yyin;
extern int yylineno;
 %}

%%
input: declarations net            { 
F = (formula *) 0 ; 
}
     | declarations net formulaheader ctlformula { 
F = $4;
}
	| declarations net key_analyse key_place aplace {
	F = (formula *) 0;
	CheckPlace = $5;
#ifdef STUBBORN
	Transitions[0]->StartOfStubbornList = (Transition *) 0;
int i;
	for(i=0;CheckPlace -> PreTransitions[i];i++)
	{
		CheckPlace->PreTransitions[i]->instubborn = true;
		CheckPlace->PreTransitions[i]->NextStubborn = Transitions[0]->StartOfStubbornList;
		Transitions[0]->StartOfStubbornList = CheckPlace->PreTransitions[i];
	}
	Transitions[0]->EndOfStubbornList = LastAttractor = CheckPlace -> PreTransitions[0];
#endif
                }
	| declarations net key_analyse key_transition atransition {
	F = (formula *) 0;
	CheckTransition = $5;
#ifdef STUBBORN
	Transitions[0]->EndOfStubbornList = LastAttractor = Transitions[0]->StartOfStubbornList = CheckTransition;
	CheckTransition -> NextStubborn = (Transition *) 0;
	CheckTransition -> instubborn = true;
#endif
	}
	| declarations net key_analyse key_marking amarkinglist  { F = (formula *) 0;}
	| declarations net key_automaton automaton  {F = (formula *) 0;}
     ; 
formulaheader: key_formula {LocalTable = new SymbolTab(2);}
			;
atransition: nodeident {
				TS = (TrSymbol *) TransitionTable -> lookup($1);
				if(!TS) yyerror("transition does not exist");
				if(TS -> vars && TS -> vars -> card)
				{
					yyerror("HL transition requires firing mode");
				}
				$$ = TS -> transition;
			}

		   | hlprefix lbrack firingmode rbrack {
		   unsigned int i, j,card;
		   fmode * fm;
		   // unused: VaSymbol * v;
		   UValue * vl;
		   char ** cc;
		   char ** inst;
		   unsigned int len;
		   for(card=0,fm=$3;fm;fm = fm -> next,card++);
		   if(card != $1 -> vars -> card)
		   {
			yyerror("firing mode incomplete");
		   }
		   cc = new char * [ card + 10];
		   inst= new char * [ card + 10];
		   len = strlen($1->name) + 4;
		   j=0;
		   for(i=0;i<LocalTable->size;i++)
		   {
			for(VS = (VaSymbol *) (LocalTable -> table[i]);VS; VS = (VaSymbol *) (VS -> next))
			{
				UValue * pl;
				for(fm=$3;fm;fm=fm ->next)
				{	
					if(fm -> v == VS) break;
				}
				if(!fm) yyerror("firing mode incomplete");
				vl = fm -> t -> evaluate();
				pl = VS -> var -> type -> make();
				pl -> assign(vl);
				cc[j] = pl -> text();
				inst[j] = new char [strlen(VS -> name)+strlen(cc[j]) + 20];
				strcpy(inst[j],VS -> name);
				strcpy(inst[j]+strlen(inst[j]),"=");
				strcpy(inst[j]+strlen(inst[j]),cc[j]);
				len += strlen(inst[j]) + 1;
				j++;
			}
		   } 
		   char * llt;
		   llt = new char[len+20];
		   strcpy(llt,$1 -> name);
		   strcpy(llt+strlen(llt),".[");
		   for(j=0;j<card;j++)
		   {
			strcpy(llt + strlen(llt),inst[j]);
			strcpy(llt + strlen(llt),"|");
		   }
		   strcpy(llt + (strlen(llt) - 1),"]");
		   TS = (TrSymbol *) TransitionTable -> lookup(llt);
		   if(!TS) yyerror("transition instance does not exist");
		   if(TS -> vars && TS -> vars -> card)
		   {
			yyerror("HL and LL transition names mixed up");
		   }
		   $$ = TS -> transition;
		   }
		   ;
hlprefix: nodeident dot {
			TS = (TrSymbol *) TransitionTable -> lookup($1);
			if(!TS) yyerror("transition does not exist");
			if( ( !(TS -> vars) || TS -> vars -> card == 0))
			{
				yyerror("only HL transitions require firing mode");
			}
			$$ = TS;
			LocalTable = TS -> vars;
			}
			;
firingmode: nodeident eqqual expression  {
			VS = (VaSymbol *) LocalTable -> lookup($1);
			if(!VS) yyerror("transition does not have this variable");
			if(! (VS -> var -> type -> iscompatible($3 -> type)))
			{
				yyerror("expression not compatible with transition variable");
			}
			$$ = new fmode;
			$$ -> next = (fmode *) 0;
			$$ -> v = VS;
			$$ -> t = $3;
			}
            | firingmode slash nodeident eqqual expression {
			fmode * fm;
			VS = (VaSymbol *) LocalTable -> lookup($3);
			if(!VS) yyerror("transition does not have this variable");
			if(! (VS -> var -> type -> iscompatible($5 -> type)))
			{
				yyerror("expression not compatible with transition variable");
			}
			$$ = new fmode;
			$$ -> next = $1;
			$$ -> v = VS;
			$$ -> t = $5 ;
			for(fm = $1; fm ; fm = fm -> next)
			{
				if(fm -> v == $$ -> v)
				{
					yyerror("variable appears twice in firing mode");
				}
			}
			}
			;

aplace: nodeident {
	PS = (PlSymbol *) PlaceTable ->lookup($1); 
	if(!PS) yyerror("Place does not exist");
	if(PS -> sort) yyerror("HL places require instance");
	$$ = PS -> place;
	}
	  | nodeident dot expression {
	PS = (PlSymbol *) PlaceTable ->lookup($1); 
	if(!PS) yyerror("Place does not exist");
	if(!(PS -> sort)) yyerror("LL places do not require instance");
	if(!(PS -> sort -> iscompatible($3 -> type)))
	{	
		yyerror("place color not compatible to place sort");
	}
	UValue * vl , * pl;
	vl = $3 -> evaluate();
	pl = PS -> sort -> make();
	pl -> assign(vl);
	char * inst;
	inst = pl -> text();
	char * ll;
	ll = new char [strlen(PS -> name) + strlen(inst) + 20];
	strcpy(ll,PS -> name);
	strcpy(ll + strlen(ll),".");
	strcpy(ll + strlen(ll),inst);
	PS = (PlSymbol *) PlaceTable -> lookup(ll);
	if(!PS) yyerror("place instance does not exist");
	if(PS->sort) yyerror("mixed up HL and LL place names");
	$$ = PS -> place;
	}
	  ;
declarations: 
			| declarations declaration
			;
declaration: key_sort sortdeclarations
		   | key_function {LocalTable = new SymbolTab(1);} functiondeclaration
		   ;
sortdeclarations: sortdeclaration |
				   sortdeclarations sortdeclaration
				   ;
sortdeclaration:  ident eqqual sortdescription semicolon {
				// sort symbols are globally visible. A sort entry in the
				// symbol table relates a name to a sort description (UType)

				SoSymbol * s;
				if( (s = (SoSymbol *) (GlobalTable -> lookup($1))) )
				{
					yyerror("sort symbol name already used");
				}
				s = new SoSymbol($1,$3);
			}
				;
sortdescription : key_boolean { $$ = TheBooType; }
				| ident {
							// assign an additional name to an existing sort
							SoSymbol * s;
							s = (SoSymbol *) (GlobalTable -> lookup($1));
							if(!s) yyerror("undefined sort name");
							if(s -> kind != so) yyerror("sort name expected");
							$$ = s -> type;
						}
				| key_record recordcomponents key_end {
						// unused: URcList * rl;
						$$ = new URecType($2);
						}
				| key_array sortdescription key_of sortdescription {
						// index type must be scalar
						if($2 -> tag != boo && $2 ->tag != num && $2 -> tag != enu)
						{
							yyerror("non-scalar type as index of array");
						}
						$$ = new UArrType($2,$4);
						}
				| lbrack number comma number rbrack {
				// integer interval
				unsigned int l,r;
				sscanf($2,"%u",&l);
				sscanf($4,"%u",&r);
				{
					if(l > r) yyerror("negative range in integer type");
				}
				$$ = new UNumType(l,r);
			}
				| key_enumerate enums key_end {
					$$ = new UEnuType($2);
					}
				;
enums:   enu
	 | enu enums { 
					$1 -> next = $2;
					$$ = $1;
				 }
	;
enu: ident {
			EnSymbol * e;
			UEnList * eel;
			e = (EnSymbol *) GlobalTable -> lookup($1);
			if(e) yyerror("element name of enumeration already used");
			e = new EnSymbol($1);
			eel = new UEnList;
			eel -> sy = e;
			eel -> next = (UEnList *) 0;
			$$ = eel;
			}
	;
recordcomponents: recordcomponent
				  | recordcomponents recordcomponent 
					{
						$2 -> next = $1;
						$$ = $2;
					}
						;
recordcomponent: ident colon sortdescription semicolon {
						RcSymbol * r;
						URcList * rl;
						r = (RcSymbol *) GlobalTable -> lookup($1);
						if(r) yyerror("record component name already used");
						r = new RcSymbol($1,$3);
						rl = new URcList;
						rl -> next = (URcList *) 0;
						rl -> sy = r;
						rl -> ty = $3;
						$$ = rl;
					}
					;
	
functiondeclaration: head vardeclarations body
						{
							$1 -> body = $3;
							$1 -> localsymb = LocalTable;
						}
					;
vardeclarations:  | key_var vdeclarations ;
vdeclarations : | vdeclarations vdeclaration ;
vdeclaration: identlist colon sortdescription semicolon {
							IdList * il;
							for(il = $1;il;il=il->next)
							{	
								VaSymbol * v;
								UVar * vvv;
								if( (v = (VaSymbol *) (LocalTable -> lookup(il -> name))) )
								{
									yyerror("variable name already used");
								}
								vvv = new UVar($3);
								v = new VaSymbol(il -> name,vvv);
							}
							}
							;
identlist : ident {
				$$ = new IdList;
				$$ -> name = $1;
				$$ -> next = (IdList *) 0;
				}
		| ident comma identlist {
				$$ = new IdList;
				$$ -> name = $1;
				$$ -> next = $3;
			}
		;
		
head: ident lpar fparlists rpar colon sortdescription {
			FcSymbol * fs;
			UFunction * f;
			fs = (FcSymbol *) GlobalTable -> lookup($1);
			if(fs)
			{
				yyerror("function name already used");
			}
			CurrentFunction = f = new UFunction();
			fs = new FcSymbol($1,f);
			f -> type = $6;
			f -> localsymb = LocalTable;
			f -> result = (UValueList *) 0;
			f -> resultstack = (UResultList *) 0;
			f -> arity = LocalTable -> card;
			f -> formalpar = new UVar * [f -> arity +5];
			int i;
			i = 0;
			for(unsigned int j = 0; j < LocalTable -> size; j++)
			{
				Symbol * s;
				for(s = LocalTable -> table[j]; s; s = s -> next)
				{	
					f -> formalpar[(f->arity -1) -i++] = ((VaSymbol *) s) -> var;
				}
			}
				
				$$ = f;
		}
;
fparlists : | fparlist | fparlists semicolon fparlist ;
fparlist: identlist colon sortdescription {
		IdList * il;
		for(il = $1; il; il = il -> next)
		{
								VaSymbol * v;
								UVar * vvv;
								if( (v = (VaSymbol *) (LocalTable -> lookup(il -> name))) )
								{
									yyerror("variable name already used");
								}
								vvv = new UVar($3);
								v = new VaSymbol(il -> name,vvv);
			
		}
		}
		;
body: key_begin statement_seq key_end { $$ = $2; } 
	;
statement_seq : statement
	      | statement_seq semicolon statement { 
										   UStatement * s;
										   s = new USequenceStatement;
                                          ((USequenceStatement * ) s) -> first = $1;
                                          ((USequenceStatement *) s) -> second = $3;
											$$ = s;
					}
	      ;
statement:    while_statement
         |    repeat_statement
         |    for_statement
         |    forall_statement
         |    if_statement
         |    case_statement
         |    assignment
         |    return_statement
         |    exit_statement 
         ;
while_statement: key_while expression key_do statement_seq key_end {
		if($2 -> type -> tag != boo)
		{
			yyerror("while condition must be boolean");
		}
		$$ = new UWhileStatement;
		((UWhileStatement *) $$) -> cond = $2;
		((UWhileStatement *) $$) -> body = $4;
		}
	;
repeat_statement: key_repeat statement_seq until expression key_end {
		if($4 -> type -> tag != boo)
		{
			yyerror("while condition must be boolean");
		}
		$$ = new URepeatStatement;
		((URepeatStatement *) $$) -> cond = $4;
		((URepeatStatement *) $$) -> body = $2;
		}
	;
for_statement: key_for ident eqqual expression key_to expression key_do statement_seq key_end {
		VaSymbol * v;
		v = (VaSymbol *) LocalTable -> lookup($2);
		if(!v) yyerror("loop variable not declared");
		$$ = new UForStatement;
		((UForStatement *) $$) -> var = v -> var;
		if(! ( v->var->type -> iscompatible($4 -> type)))
		{
			yyerror("initial expression of for statement not compatible to counter variable");
		}
		if(! ( v->var->type -> iscompatible($6 -> type)))
		{
			yyerror("exit expression of for statement not compatible to counter variable");
		}
		((UForStatement *) $$) -> init = $4;
		((UForStatement *) $$) -> finit = $6;
		((UForStatement *) $$) -> body = $8;
		}
		;
forall_statement: key_for key_all ident key_do statement_seq key_end {
			VaSymbol * v;
			v = (VaSymbol *) LocalTable -> lookup($3);
			if(!v) yyerror("loop variable not declared");
			$$ = new UForallStatement;
			((UForallStatement *) $$) -> var = v -> var;
			((UForallStatement *) $$) -> body = $5;
			}
		;
if_statement:   key_if expression key_then statement_seq key_end {
			if($2 -> type -> tag != boo)
			{
				yyerror("condition in if statement must be boolean");
			}
			$$ = new UConditionalStatement;
			((UConditionalStatement *) $$) -> cond = $2;
			((UConditionalStatement *) $$) -> yes = $4;
			((UConditionalStatement *) $$) -> no = new UNopStatement;
			}
	     |  key_if expression key_then statement_seq key_else statement_seq key_end {
			if($2 -> type -> tag != boo)
			{
				yyerror("condition in if statement must be boolean");
			}
			$$ = new UConditionalStatement;
			((UConditionalStatement *) $$) -> cond = $2;
			((UConditionalStatement *) $$) -> yes = $4;
			((UConditionalStatement *) $$) -> no = $6;
			}
              ;
return_statement : key_return expression  {
		   if(!($2 -> type -> iscompatible(CurrentFunction -> type)))
		   {
			yyerror("returned value incompatible to function type");
		   }
		   $$ = new UReturnStatement;
		   ((UReturnStatement *) $$) -> fct = CurrentFunction;
		   ((UReturnStatement *) $$) -> exp = $2;
		   }
             ;
exit_statement: key_exit {
			$$ = new UExitStatement;
			((UExitStatement *) $$) -> fct = CurrentFunction;
			 }
              ;
case_statement : key_switch expression caselist key_end {
		unsigned int crd;
		case_list * l;
		for(l = $3,crd = 0; l; l = l -> next,crd++)
		{
			if(!($2 -> type -> iscompatible(l -> exp -> type)))
			{
				yyerror("case item incompatible to case expression");
			}
	    	}
		$$ = new UCaseStatement;
		((UCaseStatement *) $$) -> exp = $2;
		((UCaseStatement *) $$) -> cond = new UExpression * [crd + 10];
		((UCaseStatement *) $$) -> yes = new UStatement * [crd + 10];
		((UCaseStatement *) $$) -> def = new UNopStatement;
		for(l=$3,crd=0; l; l = l -> next,crd++)
		{	
			((UCaseStatement *) $$) -> cond[crd] = l -> exp;
			((UCaseStatement *) $$) -> yes[crd] = l -> stm;
		}
		((UCaseStatement *) $$) -> card = crd;
		}
               | key_switch expression caselist key_else statement_seq key_end {
		unsigned int crd;
		case_list * l;
		for(l = $3,crd = 0; l; l = l -> next,crd++)
		{
			if(!($2 -> type -> iscompatible(l -> exp -> type)))
			{
				yyerror("case item incompatible to case expression");
			}
	    	}
		$$ = new UCaseStatement;
		((UCaseStatement *) $$) -> exp = $2;
		((UCaseStatement *) $$) -> cond = new UExpression * [crd + 10];
		((UCaseStatement *) $$) -> yes = new UStatement * [crd + 10];
		((UCaseStatement *) $$) -> def = $5;
		for(l=$3,crd=0; l; l = l -> next,crd++)
		{	
			((UCaseStatement *) $$) -> cond[crd] = l -> exp;
			((UCaseStatement *) $$) -> yes[crd] = l -> stm;
		}
		((UCaseStatement *) $$) -> card = crd;
		}
		;
caselist :         { $$ = (case_list *) 0;}
		| case caselist {$1 -> next = $2; $$ = $1;}
		;
case : key_case expression colon statement_seq { $$ = new case_list;
					$$ -> exp = $2;
					$$ -> stm = $4;
					$$ -> next = (case_list *) 0;
					}
	;
assignment: lvalue eqqual expression {
		if(!($1 -> type -> iscompatible($3->type)))
		{	
			yyerror("incompatible types in assignment");
		}	
		$$ = new UAssignStatement;
		((UAssignStatement *) $$) -> left = $1;
		((UAssignStatement *) $$) -> right = $3;
		}
	;
lvalue: ident {
		VaSymbol * v;
		v = (VaSymbol *) (LocalTable -> lookup($1));
		if(!v) yyerror("variable not defined");
		$$ = new UVarLVal;
		((UVarLVal*) $$) -> var = v -> var;
		$$ -> type = ((UVarLVal *) $$) -> var -> type;
		}
	  | lvalue lbrack expression rbrack {
		if($1 -> type -> tag != arr)
		{
			yyerror("component of something not an array referenced");
		}
		if(($3 -> type -> tag != boo) && ($3 -> type -> tag != num) && ($3 -> type -> tag != enu))
		{
			yyerror("non-scalar expression for array index");
		}
		$$ = new UArrayLVal;
		$$ -> type = ((UArrType *) ($1 -> type)) -> component;
		((UArrayLVal *) $$) -> indextype = ((UArrType *) ($1 -> type)) -> index;
		((UArrayLVal *) $$) -> parent = $1;
		((UArrayLVal *) $$) -> idx = $3;
		}
	  | lvalue dot ident {
			RcSymbol *r;
			r = (RcSymbol *) (GlobalTable -> lookup($3));
			if((!r) || (r -> kind != rc))
			{
				yyerror("record component unknown");
			}
			if($1 -> type -> tag != rec)
			{
				yyerror("component of something not a record referenced");
			}
			if(r -> index >= ((URecType *) ($1 -> type)) -> card ||(r -> type != ((URecType *) ($1 -> type)) -> component[r->index]))
			{
				yyerror("record type does not have such component");
			}
			$$ = new URecordLVal;
			$$ -> type = ((URecType *) ($1 -> type)) -> component[r -> index];
			((URecordLVal *) $$) -> parent = $1;
			((URecordLVal *) $$) -> offset = r -> index;
		}
	  ;
expression : expression tiff express {
				UDivExpression * e;
				if(!deep_compatible($1 -> type,$3 -> type, boo))
				{
					yyerror("boolean operator applied to non-boolean operands");
				}
				e = new UDivExpression;
				e -> left = $1;
				e -> right = $3;
				e -> type = $1 -> type;
				$$ = e;
				}
		   | expression timplies express {
				USubExpression * e;
				if(!deep_compatible($1 -> type,$3 -> type, boo))
				{
					yyerror("boolean operator applied to non-boolean operands");
				}
				e = new USubExpression;
				e -> left = $1;
				e -> right = $3;
				e -> type = $1 -> type;
				$$ = e;
				}
		   | express
		   ;
express: express tand expre {
				UMulExpression * e;
				if(!deep_compatible($1 -> type,$3 -> type, boo))
				{
					yyerror("boolean operator applied to non-boolean operands");
				}
				e = new UMulExpression;
				e -> left = $1;
				e -> right = $3;
				e -> type = $1 -> type;
				$$ = e;
				}
	   | express tor expre {
				UAddExpression * e;
				if(!deep_compatible($1 -> type,$3 -> type, boo))
				{
					yyerror("boolean operator applied to non-boolean operands");
				}
				e = new UAddExpression;
				e -> left = $1;
				e -> right = $3;
				e -> type = $1 -> type;
				$$ = e;
				}
	   | expre
	   ;
expre: tnot expre {
				UNegExpression * e;
				if(!deep_compatible($2 -> type,boo))
				{
					yyerror("boolean operator applied to non-boolean operand");
				}
				e = new UNegExpression;
				e -> left = $2;
				e -> type = $2 -> type;
				$$ = e;
				}
	 | expr
	 ;
expr:     expr eqqual exp {
				UEqualExpression * e;
				e = new UEqualExpression;
				e -> left = $1;
				e -> right = $3;
				e -> type = TheBooType;
				$$ = e;
				}
		  | expr tgt exp {
				UGreaterthanExpression * e;
				e = new UGreaterthanExpression;
				e -> left = $1;
				e -> right = $3;
				e -> type = TheBooType;
				$$ = e;
				}
		  | expr tlt exp {
				ULessthanExpression * e;
				e = new ULessthanExpression;
				e -> left = $1;
				e -> right = $3;
				e -> type = TheBooType;
				$$ = e;
				}
		  | expr tgeq exp {
				UGreatereqqualExpression * e;
				e = new UGreatereqqualExpression;
				e -> left = $1;
				e -> right = $3;
				e -> type = TheBooType;
				$$ = e;
				}
		  | expr tleq exp {
				ULesseqqualExpression * e;
				e = new ULesseqqualExpression;
				e -> left = $1;
				e -> right = $3;
				e -> type = TheBooType;
				$$ = e;
				}
		  | expr tneq exp {
				UUneqqualExpression * e;
				e = new UUneqqualExpression;
				e -> left = $1;
				e -> right = $3;
				e -> type = TheBooType;
				$$ = e;
				}
		  | exp
		  ;
exp : exp pplus term {
				UAddExpression * e;
				if(!deep_compatible($1 -> type,$3 -> type, num))
				{
					yyerror("integer operator applied to non-integer operands");
				}
				e = new UAddExpression;
				e -> left = $1;
				e -> right = $3;
				e -> type = $1 -> type;
				$$ = e;
				}
	| exp mminus term {
				USubExpression * e;
				if(!deep_compatible($1 -> type,$3 -> type, num))
				{
					yyerror("integer operator applied to non-integer operands");
				}
				e = new USubExpression;
				e -> left = $1;
				e -> right = $3;
				e -> type = $1 -> type;
				$$ = e;
				}
	| term
	;
term : term times factor {
				UMulExpression * e;
				if(!deep_compatible($1 -> type,$3 -> type, num))
				{
					yyerror("integer operator applied to non-integer operands");
				}
				e = new UMulExpression;
				e -> left = $1;
				e -> right = $3;
				e -> type = $1 -> type;
				$$ = e;
				}
	 | term divide factor {
				UDivExpression * e;
				if(!deep_compatible($1 -> type,$3 -> type, num))
				{
					yyerror("integer operator applied to non-integer operands");
				}
				e = new UDivExpression;
				e -> left = $1;
				e -> right = $3;
				e -> type = $1 -> type;
				$$ = e;
				}
	 | term key_mod factor {
				UModExpression * e;
				if(!deep_compatible($1 -> type,$3 -> type, num))
				{
					yyerror("integer operator applied to non-integer operands");
				}
				e = new UModExpression;
				e -> left = $1;
				e -> right = $3;
				e -> type = $1 -> type;
				$$ = e;
				}
	 | factor
	 ;
factor : mminus fac {
				UNegExpression * e;
				if(!deep_compatible($2 -> type,num))
				{
					yyerror("integer operator applied to non-integer operands");
				}
				e = new UNegExpression;
				e -> left = $2;
				e -> type = $2 -> type;
				$$ = e;
				}
	   | fac
	   ;
fac: ident {
		Symbol * s;
		s = LocalTable -> lookup($1);
		if(s)
		{
			// s is local variable

			VaSymbol * v;
			ULvalExpression * e;
			ULVal * l;
			v = (VaSymbol *) s;
			l = new UVarLVal;
			((UVarLVal *) l) -> var = v -> var;
			((UVarLVal *) l) -> type = ((UVarLVal *) l) -> var -> type;
			e = new ULvalExpression;
			e -> type = l -> type;
			e -> lval = l;
			$$ = e;
		}
		else
		{
			// try global symbol
			EnSymbol *n;
			UEnuconstantExpression * e;
			s = GlobalTable -> lookup($1);
			if(!s) 
			{
				printf($1);
				yyerror("identifier not defined");
			}
			if(s->kind != en) yyerror("identifier of wrong kind");
			n = (EnSymbol *) s;
			e = new UEnuconstantExpression;
			e -> type = n -> type;
			e -> nu = n -> ord;
			$$ = e;
		}
		}
   | lvalue dot ident {
			RcSymbol *r;
			URecordLVal * l;
			ULvalExpression * e;
			r = (RcSymbol *) (GlobalTable -> lookup($3));
			if((!r) || (r -> kind != rc))
			{
				yyerror("record component unknown");
			}
			if($1 -> type -> tag != rec)
			{
				yyerror("component of something not a record referenced");
			}
			if(r -> index >= ((URecType *) ($1 -> type)) -> card ||(r -> type != ((URecType *) ($1 -> type)) -> component[r->index]))
			{
				yyerror("record type does not have this component");
			}
			l = new URecordLVal;
			l -> type = ((URecType *) ($1 -> type)) -> component[r -> index];
			l -> parent = $1;
			l -> offset = r -> index;
			e = new ULvalExpression;
			e -> type = l -> type;
			e -> lval = l;
			$$ = e;
		}
   | lvalue lbrack expression rbrack {
		UArrayLVal * a;
		ULvalExpression * e;
		if($1 -> type -> tag != arr)
		{
			yyerror("component of something not an array referenced");
		}
		if(($3 -> type -> tag != boo) && ($3 -> type -> tag != num) && ($3 -> type -> tag != enu))
		{
			yyerror("non-scalar expression for array index");
		}
		a = new UArrayLVal;
		a -> type = ((UArrType *) ($1 -> type)) -> component;
		a -> parent = $1;
		a -> indextype = ((UArrType *) ($1 -> type)) -> index;
		a -> idx = $3;
		e = new ULvalExpression;
		e -> type = a -> type;
		e -> lval = a;
		$$ =e;
		}
   | lpar expression rpar {$$ = $2;}
   | key_true {
		$$ = new UTrueExpression;
		$$ -> type = TheBooType;
		}
   | key_false {
		$$ = new UFalseExpression;
		$$ -> type = TheBooType;
		}
   | functioncall
   | arrayvalue
   | number {$$ = new UIntconstantExpression;
			 sscanf($1,"%u",&(((UIntconstantExpression *) $$) -> nu)); 
			 $$ -> type = TheNumType;}
   ;
functioncall : ident lpar expressionlist rpar {
					UCallExpression * e;
					case_list * c;
					FcSymbol * f;
					int i;
					f = (FcSymbol *) GlobalTable -> lookup($1);
					if(!f) yyerror("undefined function called");
					e = new UCallExpression;
					e -> fct = f -> function;
					e -> type = f -> function -> type;
					e -> currentpar = new UExpression * [f -> function -> arity + 10];
					for(i = 0,c=$3; i < f->  function -> arity; i++)
					{
						if(!c) yyerror("too few arguments to function");
						e -> currentpar[i] = c -> exp;
						if(!(c -> exp -> type -> iscompatible(f -> function -> formalpar[i]->type)))
						{
							yyerror("type mismatch in call parameter");
						}
						c = c -> next;
					}
					if(c) yyerror("to many arguments to function");
					$$ = e;
				}
			 ;
expressionlist: {$$ = (case_list *) 0;}
			  | expression {
					$$ = new case_list;
					$$ -> exp = $1;
					$$ -> next = (case_list *) 0;
				}
			  | expression comma expressionlist {
					$$ = new case_list;
					$$ -> exp = $1;
					$$ -> next = $3;
				}
			  ;
arrayvalue: lbrack valuelist rbrack {
					UNumType * it;
					UArrType * at;
					UType * ct;
					unsigned int h;
					int i;
					case_list * c;
					UArrayExpression * e;

					for(c=$2,h=0;c;c = c -> next,h++);
					it = new UNumType(1,h);
					ct = $2 -> exp -> type;
					at = new UArrType(it,ct);
					e = new UArrayExpression;
					e -> type = at;
					e -> card = h;
					e -> cont = new UExpression * [h+10];
					for(i = 0,c = $2; i < h; i++,c = c -> next)
					{
						e -> cont[i] = c -> exp;
						if(!(ct -> iscompatible(c -> exp -> type)))
						{
							yyerror("incompatible types in array value");
						}

					}
					$$ = e;
				}
		  ;
valuelist: expression {
				$$ = new case_list;
				$$ ->exp = $1;
				$$ -> next = (case_list *) 0;
			}
		 | expression slash valuelist {
				$$ = new case_list;
				$$ -> next = $3;
				$$ -> exp = $1;
			}
		 ;
amarkinglist: amarking
           | amarkinglist comma amarking
           ;
amarking: nodeident colon number {
 unsigned int i;
  PS = (PlSymbol *) PlaceTable -> lookup($1);
  if(!PS)
    {
      yyerror("place does not exist");
    }
  if(PS -> sort)
  {
	// HL place, number nicht erlaubt
	yyerror("markings of high level places must be term expressions");
  }
  else
  {
  // LL place, number ist als Anzahl zu interpretieren
  sscanf($3,"%u",&i);
  PS->place->target_marking += i;
  }
 }
	| nodeident colon multiterm {
				char * inst, * ll;
				UTermList * tl;
				UValueList * vl, * currentvl;
				UValue * pv;
				PlSymbol * PSI;
				PS = (PlSymbol *) PlaceTable -> lookup($1);
				if(!PS)
				{
					yyerror("place does not exist");
				}
				if(!PS -> sort)
				{	
					yyerror("multiterm expression not allowed for low level places");
				}
				pv = PS -> sort -> make();
				for(tl = $3; tl; tl = tl -> next) // do for all mt components
				{
					// check type compatibility
					if(!(PS -> sort -> iscompatible(tl -> t -> type)))
					{
						yyerror("marking expression not compatible to sort of place");
					}
					vl = tl -> t -> evaluate();
					for(currentvl = vl; currentvl;currentvl = currentvl -> next)
					{
						pv -> assign(currentvl -> val); // type adjustment
						inst = pv -> text();
						ll = new char [strlen($1) + strlen(inst) + 20];
						strcpy(ll,$1);
						strcpy(ll + strlen($1),".");
						strcpy(ll + (strlen($1) + 1), inst);
						PSI = (PlSymbol *) PlaceTable -> lookup(ll); 
						if(!PSI)
						{
							yyerror("place instance does not exist");
						}
						PSI -> place -> target_marking += tl -> mult;
					}
				}
			}
	;
net: key_place placelists semicolon key_marking {LocalTable = (SymbolTab *) 0;} markinglist semicolon transitionlist {
  unsigned int i,h,j;
  Symbol * ss;
  // Create array of places
  Places = new Place * [PlaceTable -> card+10];
  CurrentMarking = new unsigned int [PlaceTable -> card+10];
  i = 0;
  for(h=0;h<PlaceTable -> size;h++)
    {
      for(ss= PlaceTable -> table[h];ss;ss = ss -> next)
	{
	  if(!(((PlSymbol *) ss) -> sort))
	  {
		  Places[i++] = ((PlSymbol *) ss) -> place;
	  }
	}
    }
	PlaceTable->card = i;
#ifdef WITHFORMULA
	for(i=0;i<PlaceTable->card;i++)
	{
		Places[i]->propositions = (formula **) 0;
	}
#endif
  // Create array of transitions 
  Transitions = new Transition * [TransitionTable -> card+10];
  i = 0;
  for(h=0;h<TransitionTable -> size;h++)
    {
      for(ss = TransitionTable -> table[h];ss;ss = ss -> next)
	{
	  if(!(((TrSymbol *) ss) -> vars))
	  {
		  Transitions[i++] = ((TrSymbol *) ss) -> transition;
	  }
	}
    }
	TransitionTable->card = i;
  // Create arc list of places pass 1 (count nr of arcs)
  for(i = 0; i < TransitionTable -> card;i++)
    {
      for(j=0;j < Transitions[i]->NrOfArriving;j++)
	{
	  Transitions[i]->ArrivingArcs[j]->pl->NrOfLeaving++;
	}
      for(j=0;j < Transitions[i]->NrOfLeaving;j++)
	{
	  Transitions[i]->LeavingArcs[j]->pl->NrOfArriving++;
	}
    }
  // pass 2 (allocate arc arrays)
  for(i=0;i<PlaceTable -> card;i++)
    {
      Places[i]->ArrivingArcs = new Arc * [Places[i]->NrOfArriving+10];
      Places[i]->NrOfArriving = 0;
      Places[i]->LeavingArcs = new Arc * [Places[i]->NrOfLeaving+10];
      Places[i]->NrOfLeaving = 0;
    }
  // pass 3 (fill in arcs)
  for(i=0;i<TransitionTable -> card;i++)
    {
      for(j=0;j < Transitions[i]->NrOfLeaving;j++)
	{
	  Place * pl;
	  pl = Transitions[i]->LeavingArcs[j]->pl;
	  pl->ArrivingArcs[pl->NrOfArriving] = Transitions[i]->LeavingArcs[j];
	  pl->NrOfArriving ++;
	}
      for(j=0;j < Transitions[i]->NrOfArriving;j++)
	{
	  Place * pl;
	  pl = Transitions[i]->ArrivingArcs[j]->pl;
	  pl->LeavingArcs[pl->NrOfLeaving] = Transitions[i]->ArrivingArcs[j];
	  pl->NrOfLeaving ++;
	}
    }
    for(i=0;i<TransitionTable->card;i++)
    {
#ifdef STUBBORN
      Transitions[i] -> mustbeincluded = Transitions[i]->conflicting;
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
  for(i= 0;i < PlaceTable -> card;i++) Places[i]->initialize();
#endif
  Transitions[0]-> StartOfEnabledList = Transitions[0];
// The following pieces of code initialize static attractor sets for
// various problems.
#ifdef BOUNDEDNET
#ifdef STUBBORN
	// initialize list of pumping transitions
	LastAttractor = (Transition*)0;
	int p,c,a; // produced, consumed tokens, current arc
	for(i=0;i< TransitionTable -> card;i++)
	{
		// count produced tokens
		for(a=0,p=0;a<Transitions[i]->NrOfLeaving;a++)
		{
			p += Transitions[i]->LeavingArcs[a] -> Multiplicity;
		}
		// count consumed tokens
		for(a=0,c=0;a<Transitions[i]->NrOfArriving;a++)
		{
			c += Transitions[i]->ArrivingArcs[a]->Multiplicity;
		}
		if(p > c)
		{
			Transitions[i]->instubborn = true;
			if(LastAttractor)
			{
				Transitions[i]->NextStubborn = 
					Transitions[i]->StartOfStubbornList;
				Transitions[i]->StartOfStubbornList = Transitions[i];
			}
			else
			{
				Transitions[i]->StartOfStubbornList = LastAttractor = Transitions[i];
				Transitions[i]-> NextStubborn = (Transition *) 0;
			}
		}
	}
		
#endif
#endif
}
        ;
placelists: capacity placelist 
	  | placelists semicolon capacity placelist
	  ;
capacity:            { CurrentCapacity = CAPACITY;}
	  | key_safe    colon  {CurrentCapacity = 1;}
	  | key_safe number  colon { sscanf($2,"%d",&CurrentCapacity);}
          ;
placelist:  placelist comma place 
           |  place 
           ;
place:   nodeident {
			 if(PlaceTable -> lookup($1))
			 {
			   yyerror("Place name used twice");
		     }
			 P = new Place($1);
		     PS = new PlSymbol(P);
			 PS -> sort = (UType *) 0;
			 P -> capacity = CurrentCapacity;
			 P -> nrbits = CurrentCapacity > 0 ? logzwo(CurrentCapacity) : 32;
		}
		 | nodeident colon sortdescription {
			// high level place: unfold to all instances
			char * c;
			if(PlaceTable -> lookup($1))
			{
				yyerror("Place name used twice");
			}
			c = new char [strlen($1)+10];
			strcpy(c,$1);
			PS =  new PlSymbol(c);
			PS -> sort = $3;
			UValue * v;
			v = $3 -> make();
			do
			{
				char * lowlevelplace;
				char * lowtag;
				lowtag = v -> text();
				lowlevelplace = new char [ strlen(c) + strlen(lowtag) + 20];
				strcpy(lowlevelplace,c);
				strcpy(lowlevelplace + strlen(c), ".");
				strcpy(lowlevelplace + strlen(c) + 1, lowtag);
				if(PlaceTable -> lookup(lowlevelplace))
				{
					yyerror("Place instance name already used");
				}
				P = new Place(lowlevelplace);
				P -> capacity = CurrentCapacity;
				P -> nrbits = CurrentCapacity > 0 ? logzwo(CurrentCapacity) : 32;
				PS = new PlSymbol(P);
				PS -> sort = (UType *) 0;
				(*v)++;
			} while(!(v -> isfirst()));
			}
     ;
nodeident: ident { $$ = $1;}
		 | number  {$$ = $1; }
		 ;
markinglist: 
		   | marking
           | markinglist comma marking
           ;
marking: nodeident colon number {
  unsigned int i;
  PS = (PlSymbol *) PlaceTable -> lookup($1);
  if(!PS)
    {
      yyerror("place does not exist");
    }
  if(PS -> sort)
  {
	// HL place, number nicht erlaubt
	yyerror("markings of high level places must be term expressions");
  }
  else
  {
  // LL place, number ist als Anzahl zu interpretieren
  sscanf($3,"%u",&i);
  *(PS->place) += i;
  }
 }
			| nodeident colon multiterm {
				char * inst, * ll;
				UTermList * tl;
				UValueList * vl, * currentvl;
				UValue * pv;
				PlSymbol * PSI;
				PS = (PlSymbol *) PlaceTable -> lookup($1);
				if(!PS)
				{
					yyerror("place does not exist");
				}
				if(!PS -> sort)
				{	
					yyerror("multiterm expression not allowed for low level places");
				}
				pv = PS -> sort -> make();
				for(tl = $3; tl; tl = tl -> next) // do for all mt components
				{
					// check type compatibility
					if(!(PS -> sort -> iscompatible(tl -> t -> type)))
					{
						yyerror("marking expression not compatible to sort of place");
					}
					vl = tl -> t -> evaluate();
					for(currentvl = vl; currentvl;currentvl = currentvl -> next)
					{
						pv -> assign(currentvl -> val); // type adjustment
						inst = pv -> text();
						ll = new char [strlen($1) + strlen(inst) + 20];
						strcpy(ll,$1);
						strcpy(ll + strlen($1),".");
						strcpy(ll + (strlen($1) + 1), inst);
						PSI = (PlSymbol *) PlaceTable -> lookup(ll); 
						if(!PSI)
						{
							yyerror("place instance does not exist");
						}
						(* PSI -> place) += tl -> mult;
					}
				}
			}
 ;
multiterm:  mtcomponent { $$ = $1;}
		 | multiterm pplus mtcomponent {$3 -> next = $1; $$ = $3;}
		 ;
mtcomponent: hlterm { $$ = $1;} 
		   | hlterm colon number { unsigned int i; sscanf($3,"%u",&i);$1 -> mult = i;
								 $$ = $1;}
		   ;
hlterm : ident {
		UTermList * tl;
		UVarTerm * vt;
		if(!LocalTable)
		{
			yyerror("only constant terms are allowed in this context");
		}
		VS =(VaSymbol *)  (LocalTable -> lookup($1));
		if(!VS)
		{
			yyerror("undeclared variable in term");
		}
		tl = new UTermList;
		tl -> next = (UTermList *) 0;
		tl -> mult = 1;
		vt = new UVarTerm;
		tl -> t = vt;
		vt -> v = VS -> var;
		vt -> type = VS -> var -> type;
		$$ =tl;
		}
	 | ident lpar termlist rpar {
		FcSymbol * FS;
		UTermList * tl;
		UOpTerm * ot;
		FS = (FcSymbol *) GlobalTable -> lookup($1);
		if(!FS) yyerror("operation symbol not declared");
		if(FS ->kind != fc) yyerror("wrong symbol used as operation symbol");
		unsigned int i;
		UTermList * l;
		for(i=0,l=$3;l;i++,l=l->next);
		if(i != FS -> function -> arity) yyerror("wrong number of arguments");
		tl = new UTermList;
		tl -> next = (UTermList *) 0;
		tl -> mult = 1;
		ot = new UOpTerm;
		tl -> t = ot;
		ot -> arity = i;
		ot -> f = FS -> function;
		ot -> sub = new UTerm * [i+5];
		for(i=0,l=$3;i<ot -> arity;i++,l= l -> next)
		{
			if(!(ot -> f -> formalpar[i] -> type -> iscompatible(l -> t -> type)))
			{
				yyerror("type mismatch in subterm(s)");
			}
			ot -> sub[i] = l -> t;
		}
		ot -> type = ot -> f -> type;
		$$ = tl;
	   }
	 ;
termlist : {$$ = (UTermList *) 0;}
		 | hlterm { $$ = $1;}
		 | hlterm comma termlist {
			$1 -> next = $3;
			$$ = $1;
			}
		 ;

transitionlist: transition
              | transitionlist transition
              ;
transition: key_transition tname transitionvariables guard key_consume arclist semicolon key_produce arclist semicolon  {
  unsigned int card;
  unsigned int i;
  arc_list * current;
  /* 1. Transition anlegen */
  if(TransitionTable -> lookup($2))
    {
      yyerror("transition name used twice");
    }
  TS = new TrSymbol($2);
  TS -> vars = LocalTable;
  TS -> guard = $4;

  // unfold HL transitions
  // -> create transition instance for every assignment to the variables that
  //    matches the guard


  // init: initially, all variables have initial value

  while(1)
  {
	if(TS -> guard)
	{
		UValue * v;
		v = TS -> guard -> evaluate();
		if(((UBooValue * )v) -> v == false) goto nextass;
	}
	// A) create LL transition with current variable assignment

  /* generate name */
  char * llt;
  if((!LocalTable) || LocalTable -> card == 0)
  {
	llt = TS -> name;
	TS -> vars = (SymbolTab *) 0;
  }
  else
  {
  char ** assignment;
  unsigned int len;
  len = 0;
  assignment = new char * [LocalTable -> card + 1000];
  VaSymbol * vs;
  unsigned int i,j;
  j=0;
  for(i=0;i < LocalTable -> size; i++)
  {
	for(vs = (VaSymbol *) (LocalTable-> table[i]); vs ; vs = (VaSymbol *) vs -> next)
	{
		char * inst;
		inst = vs -> var -> value -> text();
		assignment[j] = new char[strlen(vs -> name) + 10 + strlen(inst)];
		strcpy(assignment[j],vs -> name);
		strcpy(assignment[j]+strlen(vs -> name),"=");
		strcpy(assignment[j]+strlen(vs -> name)+1,inst);
		len += strlen(assignment[j++]);
	}
  }
  llt = new char [ strlen(TS -> name)  + len + LocalTable -> card + 1000];
  strcpy(llt,TS->name);
  strcpy(llt + strlen(llt),".[");
  for(i=0;i<LocalTable -> card; i++)
  {
	strcpy(llt + strlen(llt),assignment[i]);
	strcpy(llt + strlen(llt),"|");
  }
  strcpy(llt + (strlen(llt) - 1), "]");
  }
  TrSymbol * TSI;
  if((!LocalTable) || LocalTable -> card == 0)
  {
	TSI = TS;
  }
  else
  {
  TSI = (TrSymbol *) TransitionTable -> lookup(llt);
  if(TSI) yyerror("transition instance already exists");
  TSI = new TrSymbol(llt);
  TSI -> vars = (SymbolTab *) 0;
  TSI -> guard = (UExpression *) 0;
  }
  T = TSI -> transition = new Transition(TSI -> name);
  T -> fairness = $3;
  /* 2. Inliste eintragen */
  /* HL-Boegen in LL-Boegen uebersetzen und zur Liste hinzufuegen */
  arc_list * root;
  root = $6;
  for(current = root;current; current = current -> next)
  {
	if(current -> mt)
	{
		// traverse multiterm
		arc_list * a;
		UTermList * mc;
		UValueList * vl;
		UValueList * vc;
		UValue * pv;
		pv = current -> place -> sort -> make();
		for(mc = current -> mt; mc ; mc = mc -> next)
		{	
			vl = mc -> t -> evaluate();

			for(vc = vl; vc; vc  = vc -> next)
			{
				char * inst;
				char * ll;
				pv -> assign(vc -> val);
				inst = pv -> text();
				ll = new char [strlen(current -> place -> name) + strlen(inst) + 20];
				strcpy(ll,current->place->name);
				strcpy(ll+strlen(current->place->name),".");
				strcpy(ll+strlen(current->place->name)+1,inst);
				PS = (PlSymbol *) PlaceTable -> lookup(ll);
				if(!ll) yyerror("place instance does not exist");
				if(PS -> sort) yyerror("arcs to HL places are not allowed");
				a = new arc_list;
				a -> place = PS;
				a -> mt = (UTermList *) 0;
				a -> nu =mc -> mult;
				a -> next = root;
				root = a;
			}
		}
	}
  }
  /* Anzahl der Boegen */
  for(card = 0, current = root;current;card++,current = current -> next);
  T->ArrivingArcs = new  Arc * [card+10];
  /* Schleife ueber alle Boegen */
  for(current = root;current;current = current -> next)
    {
	  /* Bogen ist nur HL-Bogen */
	  if(current -> place -> sort) continue;
      /* gibt es Bogen schon? */

      for(i = 0; i < T->NrOfArriving;i++)
	{
	  if(current->place -> place == T->ArrivingArcs[i]->pl)
	    {
	      /* Bogen existiert, nur Vielfachheit addieren */
	      *(T->ArrivingArcs[i]) += current->nu;
	      break;
	    }
	}
      if(i>=T->NrOfArriving)
	{
	  T->ArrivingArcs[T->NrOfArriving] = new Arc(T,current->place->place,true,current->nu);
	  T->NrOfArriving++;
	  current -> place -> place -> references ++;
	}
    }
  /* 2. Outliste eintragen */
  root = $9;
  for(current = root;current; current = current -> next)
  {
	if(current -> mt)
	{
		// traverse multiterm
		arc_list * a;
		UTermList * mc;
		UValueList * vl;
		UValueList * vc;
		UValue * pv;
		pv = current -> place -> sort -> make();
		for(mc = current -> mt; mc ; mc = mc -> next)
		{	
			vl = mc -> t -> evaluate();
			for(vc = vl; vc; vc  = vc -> next)
			{
				char * inst;
				char * ll;
				pv -> assign(vc -> val);
				inst = pv -> text();
				ll = new char [strlen(current -> place -> name) + strlen(inst) + 20];
				strcpy(ll,current->place->name);
				strcpy(ll+strlen(current->place->name),".");
				strcpy(ll+strlen(current->place->name)+1,inst);
				PS = (PlSymbol *) PlaceTable -> lookup(ll);
				if(!ll) yyerror("place instance does not exist");
				a = new arc_list;
				a -> place = PS;
				a -> mt = (UTermList *) 0;
				a -> nu =mc -> mult;
				a -> next = root;
				root = a;
			}
		}
	}
  }
  /* Anzahl der Boegen */
  for(card = 0, current = root;current;card++,current = current -> next);
  T->LeavingArcs = new  Arc * [card+10];
  /* Schleife ueber alle Boegen */
  for(current = root;current;current = current -> next)
    {
	  /* Bogen ist nur HL-Bogen */
	  if(current -> place -> sort) continue;
      /* gibt es Bogen schon? */

      for(i = 0; i < T->NrOfLeaving;i++)
	{
	  if(current->place -> place == T->LeavingArcs[i]->pl)
	    {
	      /* Bogen existiert, nur Vielfachheit addieren */
	      *(T->LeavingArcs[i]) += current->nu;
	      break;
	    }
	}
      if(i>=T->NrOfLeaving)
	{
	  T->LeavingArcs[T->NrOfLeaving] = new Arc(T,current->place -> place,false,current->nu);
	  T -> NrOfLeaving++;
	  current -> place -> place -> references ++;
	}
    }
	// B) switch to next assignment
nextass:
	if((!LocalTable) || LocalTable -> card == 0) break;
	unsigned int k;
	VaSymbol * vv;
	for(k=0;k < LocalTable -> size;k++)
	{
		for(vv = (VaSymbol *) (LocalTable-> table[k]); vv;vv = (VaSymbol *) (vv -> next))
		{
			(*(vv -> var -> value)) ++;
			if(!(vv -> var -> value -> isfirst()) ) break;
		}
		if(vv) break;
	}
	if(!vv) break;
	}
}
;
transitionvariables: vardeclarations {$$ = 0;}
		   | key_weak key_fair vardeclarations {$$ = 1;}
		   | key_strong key_fair vardeclarations {$$ = 2;}
                   ;
tname:   ident {LocalTable = new SymbolTab(256);}
		 | number {LocalTable = new SymbolTab(256);}
    ;
guard: { $$ = (UExpression *) 0;}
	 | key_guard expression {if($2 -> type -> tag != boo) yyerror("guard expression must be boolean"); $$ = $2;}
     ;
arclist: { $$ = (arc_list *) 0;}
	| arc {$$ = $1;}
      | arc comma arclist {
      $1-> next = $3;
	  $$ = $1;
    }
      ;
arc:  nodeident colon number {
      unsigned int i;
      PS = (PlSymbol *) PlaceTable -> lookup($1);
      if(!PS)
	{
	  yyerror("place does not exist");
	}
	if(PS -> sort)
	{
		yyerror("arc expression of high level places must be term expressions");
	}
      $$ = new arc_list;
      $$ -> place = PS;
      $$-> next = (arc_list *)  0;
      sscanf($3,"%u",&i);
      $$ -> nu = i;
	  $$ -> mt = (UTermList *) 0;
    }
	| nodeident colon multiterm {
		UTermList * tl;
		PS = (PlSymbol *) PlaceTable -> lookup($1);
		if(!PS)
		{
			yyerror("place does not exist");
		}
		if(!(PS -> sort))
		{
			yyerror("low level places require numerical multiplicity");
		}
		$$ = new arc_list;
		$$ -> place = PS; 
		$$ -> nu = 0;
		$$ -> mt = $3;
		$$ -> next = (arc_list *) 0;
		for(tl = $3; tl; tl = tl -> next)
		{
			if(!(PS -> sort -> iscompatible(tl -> t -> type)))
			{
				yyerror("type mismatch between place and arc expression");
			}
		}
	}
	;
numex:     number { 
			$$ = new UIntconstantExpression();
			sscanf($1,"%u",&(((UIntconstantExpression *) $$) -> nu));
			}

     |     lpar expression rpar {
			if($2 -> type -> tag != num)
			{
				yyerror("integer expression expected");
			}
			$$ = $2;
			}
	 ;
ctlformula:   cplace eqqual numex   {
					((hlatomicformula *) $1) -> k = $3;
					$1 -> type = eq;
					$$ = $1;
				    }
          |   cplace tneq numex  {
					((hlatomicformula *) $1) -> k = $3;
					$1 -> type = neq;
					$$ = $1;
				  }
          |   cplace tleq numex  {
					((hlatomicformula *) $1) -> k = $3;
					$1 -> type = leq;
					$$ = $1;
				  }
          |   cplace tgeq numex  {
					((hlatomicformula *) $1) -> k = $3;
					$1 -> type= geq;
					$$ = $1;
				    }
          |   cplace tlt numex  {
					((hlatomicformula *) $1) -> k = $3;
					$1 -> type = lt;
					$$ = $1;
				    }
          |   cplace tgt numex {
					((hlatomicformula *) $1) -> k = $3;
					$1 -> type = gt;
					$$ = $1;
	                            }
          |   ctlformula tand ctlformula  {
					   $$ = new binarybooleanformula(conj,$1,$3);
                                           }
          |   ctlformula tor ctlformula  {
			                   $$ = new binarybooleanformula(disj,$1,$3);
                                          }
          |   tnot ctlformula  {
                                        $$ = new unarybooleanformula(neg,$2);
                                 }
		  | lbrack expression rbrack {
				if($2 -> type -> tag != boo)
				{	
					yyerror("formula requires boolean expression");
				}
				$$ = new staticformula($2);
					   }
	  | key_exists  quantification colon ctlformula {
		$2 -> name[0] = '\0';
		$$ = new quantifiedformula(qe,$2->var,$4);
		}
	  | key_all quantification colon ctlformula {
		$2 -> name[0] = '\0';
		$$ = new quantifiedformula(qa,$2->var,$4);
		}
          |   lpar ctlformula rpar  {
                                     $$ = $2;
                                    }
          |   exists transformula lbrack ctlformula until ctlformula rbrack  {
                                  $$ = new untilformula(eu,$4,$6,(transitionformula *) $2);
                                                              }
          |   forall transformula lbrack ctlformula until ctlformula rbrack    {
                             $$ = new untilformula(au,$4,$6,(transitionformula *) $2);
                                                              }
          |   exists transformula globally ctlformula  {
                           $$ = new unarytemporalformula(eg,$4,(transitionformula *) $2);
                                           }
          |   forall transformula globally ctlformula  {
                                $$ = new unarytemporalformula(ag,$4,(transitionformula *) $2);
                                           }
          |   exists transformula key_next ctlformula  {
                                      $$ = new unarytemporalformula(ex,$4,(transitionformula *) $2);
                                         }
          |   forall transformula key_next ctlformula  {
				$$ = new unarytemporalformula(ax,$4,(transitionformula *) $2);
                                        }
          |   exists transformula future ctlformula  {
                                      $$ = new unarytemporalformula(ef,$4,(transitionformula *) $2);
                                         }
          |   forall transformula future ctlformula  {
				$$ = new unarytemporalformula(af,$4,(transitionformula *) $2);
                                        }
;
cplace: nodeident {
				    PS = (PlSymbol *) PlaceTable -> lookup($1);
                    if(!PS) yyerror("Place does not exist");
					if(PS -> sort) yyerror("HL places require instance");
					$$ = new hlatomicformula(neq,PS,(UExpression *) 0);
				  }
	  | nodeident dot lpar expression rpar {
				    PS = (PlSymbol *) PlaceTable -> lookup($1);
                    if(!PS) yyerror("Place does not exist");
                    if(!(PS-> sort)) yyerror("LL places do not require instance");
					if(!(PS -> sort ->iscompatible($4 -> type)))
					{
						yyerror("place color incompatible to place sort");
					}
					$$ = new hlatomicformula(neq,PS,$4);
				}
	  ;
quantification: nodeident colon sortdescription {
		UVar * vv;
		VS = (VaSymbol *) LocalTable -> lookup($1);
		if(VS) yyerror("variable used twice in formula");
		vv = new UVar($3);
		$$ = new VaSymbol($1,vv);
	}
	;
transformula:  { $$ = (transitionformula *) 0;}
			| transitionformula { $$ = $1;}
			;
transitionformula:  key_exists quantification colon transitionformula {
					$2 -> name[0] = '\0';
					$$ = new quantifiedformula(qe,$2 -> var,$4);
					}
				| key_all quantification colon transitionformula {
					$2 -> name[0] = '\0';
					$$ = new quantifiedformula(qa,$2 -> var,$4);
					}
 				| transitionformula tand transitionformula {
						$$ = new binarybooleanformula(conj, $1, $3);
					}
				| transitionformula tor transitionformula {
						$$ = new binarybooleanformula(disj, $1, $3);
					}
				| tnot transitionformula {
						$$ = new unarybooleanformula(neg,$2);
					}
				| lpar transitionformula rpar {$$ = $2;}
				| formulatransition {
					if($1->vars && $1 -> vars -> card)
					{
						yyerror("HL transition requires firing mode");
					}
					$$ = new transitionformula($1 -> transition);
				}
				| formulatransition dot lbrack parfiringmode rbrack {
					if((! $1 -> vars) || ($1 -> vars -> card == 0))
					{
						yyerror("LL transition does not require firing mode");
					}
					$$ = new transitionformula($1,$4);
				}
				;
formulatransition: nodeident {
					TS = (TrSymbol *) TransitionTable -> lookup($1);
					if(!TS) yyerror("transition does not exist");
					$$ = TS;
				}
				;
parfiringmode: fmodeblock  { $$ = $1;}
			 | parfiringmode slash fmodeblock {
					$1 -> next = $3;
					$$ = $1;
				}
			;
fmodeblock: nodeident eqqual expression {
			VS = (VaSymbol *) (TS -> vars -> lookup($1));
			if(!VS) yyerror("no such transition variable");
			if(! (VS -> var -> type -> iscompatible($3 -> type)))
			{
				yyerror("variable binding incompatible with variable type");
			}
			$$ = new fmode;
			$$ -> v = VS;
			$$ -> t = $3 ;
			$$ -> next = (fmode *) 0;
		}
		  ;
automaton:	{
#ifdef WITHFORMULA
	BuchiTable = new SymbolTab(10);} statepart finalpart transitionpart {
				{
						int i,j;
						buchitransition * bt;
						for(i=0;i<buchistate::nr;i++)
						{
							buchistate * b = buchiautomaton[i];
							b -> delta = new buchitransition * [b -> nrdelta+1];
							for(j=0,bt = b -> transitionlist;bt;bt = bt -> next,j++)
							{
								b -> delta[j] = bt;
							}
						}
					    }
						// process all guard formulas
						int i, j, res;
						for(i=0;i<Places[0]->NrSignificant;i++)
						{
							Places[i]-> cardprop = 0;
							Places[i]-> propositions = (formula **) 0;
						}
						for(i=0;i<buchistate::nr;i++)
						for(j=0;j<buchiautomaton[i]->nrdelta;j++)
						{
							buchiautomaton[i]->delta[j]->guard =
							buchiautomaton[i]->delta[j]->guard -> replacequantifiers();
							buchiautomaton[i]->delta[j]->guard -> tempcard = 0;
							buchiautomaton[i]->delta[j]->guard =
							buchiautomaton[i]->delta[j]->guard -> merge();
							buchiautomaton[i]->delta[j]->guard =
							buchiautomaton[i]->delta[j]->guard -> reduce(&res);
							if(res == 0) buchiautomaton[i]->delta[j] = (buchitransition *) 0;
							buchiautomaton[i]->delta[j]->guard =
							buchiautomaton[i]->delta[j]->guard -> posate();
							buchiautomaton[i]->delta[j]->guard -> tempcard = 0;
						}
						for(i=0;i<buchistate::nr;i++)
						for(j=0;j<buchiautomaton[i]->nrdelta;j++)
						{
							buchiautomaton[i]->delta[j]->guard -> setstatic();
							if(buchiautomaton[i]->delta[j]->guard -> tempcard)
							{
								yyerror("temporal operators not allowed in buchi automaton");
							}
						}
						
#endif						
		}
	;
statepart: key_state statelist semicolon {
			// unused: int i;
			buchistate * b;
			buchiautomaton = new buchistate * [buchistate::nr];
			for(b=initialbuchistate;b;b=b->next)
			{
				buchiautomaton[b->code] = b;
			}
				 }
	;
statelist: statelist comma ident  { StSymbol * s ; 
					if( (s = (StSymbol *) (BuchiTable -> lookup($3))) )
					{
						yyerror("State name in Buchi automaton used twice");
					}
					s = new StSymbol($3);
					if(!initialbuchistate)
					{
						initialbuchistate = s -> state;
					}
					else
					{
						s -> state -> next = initialbuchistate -> next;
						initialbuchistate -> next = s -> state;
					}  
				  }
	 | ident                  { StSymbol * s;
					if( (s = (StSymbol *) (BuchiTable -> lookup($1))) )
					{
						yyerror("State name in Buchi automaton used twice");
					}
					s = new StSymbol($1);
					if(!initialbuchistate)
					{
						initialbuchistate = s -> state;
					}
					else
					{
						s -> state -> next = initialbuchistate -> next;
						initialbuchistate -> next = s -> state;
					}  
				  }
	 ;
finalpart: key_final finallist semicolon 
	 ;
finallist: finallist comma ident { StSymbol * s;
					s = (StSymbol *) BuchiTable -> lookup($3);
					if(!s) yyerror("state does not exist");
					s -> state -> final = 1;
				 }
	 | ident 		 { StSymbol * s;
					s = (StSymbol *) BuchiTable -> lookup($1);
					if(!s) yyerror("state does not exist");
					s -> state -> final = 1;
	 }						  
	 ;
transitionpart:  
              |  transitionpart btransition 
	      ;
btransition : key_transition ident key_to ident key_guard {LocalTable = new SymbolTab(2); } ctlformula {
			StSymbol * from, * to;
			buchitransition * bt;
			from = (StSymbol *) BuchiTable -> lookup($2);
			if(!from) yyerror("state does not exist");
			to = (StSymbol *) BuchiTable -> lookup($4);
			if(!to) yyerror("state does not exist");
			bt = new buchitransition;
			bt -> next = from -> state -> transitionlist;
			from -> state -> transitionlist = bt;
			(from -> state -> nrdelta )++;
			bt -> delta = to -> state;
			bt -> guard = $7;
		}
	    ;
%%


char * diagnosefilename;


void readnet()
{
	yydebug = 0;
	diagnosefilename = (char *) 0;
	if(netfile)
	{
		yyin = fopen(netfile,"r");
		if(!yyin)
		{
      fprintf(stderr, "lola: cannot open netfile '%s'\n", netfile);
			exit(4);
		}
		diagnosefilename = netfile;
	}
	GlobalTable = new SymbolTab(1024);
	TheBooType = new UBooType();
	TheNumType = new UNumType(0,INT_MAX);
	yyparse();
	unsigned int ii;
	for(ii=0;ii < Places[0]->cnt;ii++)
	{
	 	CurrentMarking[ii] = Places[ii]->initial_marking;
		Places[ii]->index = ii;
	}
if(F) 
{
	  F = F -> replacequantifiers();
	  F -> tempcard = 0;
	  F = F -> merge();
#if defined(MODELCHECKING) 
	unsigned int i;
	for(i=0;i< Transitions[0]->cnt;i++)
	{
		Transitions[i] -> lstfired = new unsigned int [F -> tempcard];
		Transitions[i] -> lstdisabled = new unsigned int [F -> tempcard];
	}
#endif
}
}

/*
bool taskfile = false;

int yywrap()
{
	yylineno = 1;
	if(taskfile) return 1;
	if(!analysefile) return 1;
	taskfile = true;
	yyin = fopen(analysefile,"r");
	if(!yyin)
	{
		cerr << "cannot open analysis task file: " << analysefile << "\n";
		exit(4);
	}
	diagnosefilename = analysefile;
	return(0);
}
*/


/// display a parser error and exit
void yyerror(char const * mess) {
    fprintf(stderr, "lola: %s:%d: error near '%s': %s\n", diagnosefilename, yylineno, yytext, mess);
	exit(3);
}

