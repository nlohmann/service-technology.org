%{
/* Prologue: Syntax and usage of the prologue.
 * Bison Declarations: Syntax and usage of the Bison declarations section.
 * Grammar Rules: Syntax and usage of the grammar rules section.
 * Epilogue: Syntax and usage of the epilogue.  */

// options for Bison
#define YYDEBUG 1
#define YYERROR_VERBOSE 0  // for verbose error messages


// to avoid the message "parser stack overflow"
#define YYMAXDEPTH 1000000
#define YYINITDEPTH 10000


// from flex
extern char* yytext;
extern int yylex();


// defined in "debug.h"
extern int yyerror(const char *);

//using namespace std;

#include "mynew.h"
#include "dimensions.h"
#include "graph.h"
#include "symboltab.h"
#include "owfnTransition.h"
#include "owfnPlace.h"
#include "owfn.h"
#include "formula.h"

#include "main.h"

#include<stdio.h>
#include<limits.h>


/* list of places and multiplicities to become arcs */
class arc_list {
 public:
	PlSymbol * place;
	unsigned int nu;
    arc_list * next;

    // Provides user defined operator new. Needed to trace all new operations
    // on this class.
#undef new
    NEW_OPERATOR(arc_list)
#define new NEW_NEW
};   


int CurrentCapacity;
owfnPlace *P;
owfnTransition *T;
Symbol * S;
PlSymbol * PS;
TrSymbol * TS;
SymbolTab * GlobalTable;
SymbolTab * LocalTable;
oWFN * PN;					// the petri net

placeType type = INTERNAL;		/* type of place */

%}

// the terminal symbols (tokens)

%token key_safe key_place key_internal key_input key_output
%token key_marking key_finalmarking key_finalcondition
%token key_transition key_consume key_produce
%token comma colon semicolon ident number
%token op_and op_or op_not op_gt op_lt op_ge op_le op_eq op_ne lpar rpar

%nonassoc op_eq op_ne op_gt op_lt op_ge op_le
%left op_not
%left op_and op_or


// the start symbol of the grammar
// %start tProcess


// Bison generates a list of all used tokens in file "syntax.h" (for flex)
%token_table


%union {
	char * str;
	int value;
	int * exl;
	arc_list * al;
	IdList * idl;
	owfnPlace * pl;
	owfnTransition * tr;
	TrSymbol * ts;
	formula * form;
}


/* the types of the non-terminal symbols */
%type <str> ident
%type <str> number
%type <str> tname
%type <str> nodeident
%type <al> arclist
%type <al> arc
%type <form> statepredicate


%%

/* Grammar rules */

input:  net { 
}
;

net: key_place place_area key_marking {LocalTable = (SymbolTab *) 0;} 
		{
			unsigned int i,h,j;
			Symbol * ss;
			
			// Create array of places
			PN->Places = new owfnPlace * [PlaceTable->card + 10];
			
			i = 0;
		  
			for(h = 0; h < PlaceTable->size; h++) {
				for(ss = PlaceTable->table[h]; ss; ss = ss->next) {
					PN->addPlace(i++, ((PlSymbol *) ss)->place);
				}
			}
			
			PlaceTable->card = i;
			
			PN->CurrentMarking = new unsigned int [PlaceTable->card + 10];
			PN->FinalMarking = new unsigned int [PlaceTable->card + 10];
	
		  	for (i = 0; i < PlaceTable->card; i++) {
		  		PN->FinalMarking[i] = 0;
		  	}
		  
			
		}
		
		markinglist semicolon final transitionlist 
		{
			unsigned int i,h,j;
			Symbol * ss;
			
			// Create array of transitions 
			PN->Transitions = new owfnTransition * [TransitionTable -> card+10];
			i = 0;
			for(h = 0; h < TransitionTable->size; h++) {
				for(ss = TransitionTable -> table[h];ss;ss = ss -> next) {
					PN->addTransition(i++, ((TrSymbol *) ss)->transition);
				}
			}
			TransitionTable->card = i;
			
			// Create arc list of places pass 1 (count nr of arcs)
			for(i = 0; i < TransitionTable->card; i++) {
				for(j = 0; j < PN->Transitions[i]->NrOfArriving; j++) {
					PN->Transitions[i]->ArrivingArcs[j]->pl->NrOfLeaving++;
				}
				for(j=0;j < PN->Transitions[i]->NrOfLeaving;j++) {
					PN->Transitions[i]->LeavingArcs[j]->pl->NrOfArriving++;
				}
			}
		
			// pass 2 (allocate arc arrays)
			for(i = 0; i < PlaceTable->card; i++) {
				PN->Places[i]->ArrivingArcs = new Arc * [PN->Places[i]->NrOfArriving+10];
				PN->Places[i]->NrOfArriving = 0;
				PN->Places[i]->LeavingArcs = new Arc * [PN->Places[i]->NrOfLeaving+10];
				PN->Places[i]->NrOfLeaving = 0;
			}
		
			// pass 3 (fill in arcs)
			for(i = 0; i < TransitionTable->card; i++) {
				for(j=0; j < PN->Transitions[i]->NrOfLeaving;j++) {
					owfnPlace * pl;
					pl = PN->Transitions[i]->LeavingArcs[j]->pl;
					pl->ArrivingArcs[pl->NrOfArriving] = PN->Transitions[i]->LeavingArcs[j];
					pl->NrOfArriving++;
					
					if (pl->type == OUTPUT) {
						PN->commDepth += PN->Transitions[i]->LeavingArcs[j]->Multiplicity;
					}
				}
				for(j = 0;j < PN->Transitions[i]->NrOfArriving; j++) {
					owfnPlace * pl;
					pl = PN->Transitions[i]->ArrivingArcs[j]->pl;
					pl->LeavingArcs[pl->NrOfLeaving] = PN->Transitions[i]->ArrivingArcs[j];
					pl->NrOfLeaving ++;
					
					if (pl->type == INPUT) {
						PN->commDepth += PN->Transitions[i]->ArrivingArcs[j]->Multiplicity;
					}
				}
			}
			for(i = 0; i < TransitionTable->card; i++)
			{
			}
			PN->startOfEnabledList = PN->Transitions[0];
		}
;

final: key_finalmarking finalmarkinglist semicolon
| key_finalcondition statepredicate semicolon {
	formula * F;
	F = $2;
	F = F -> posate();
	F = F -> merge();
	F -> setstatic();
	PN -> FinalCondition = F;
}
;

place_area: place_area_internal place_area_input  place_area_output
	  | place_area_lola
;

place_area_input: key_input {type = INPUT; } placelists semicolon
		| /* empty */
;

place_area_output: key_output {type = OUTPUT; } placelists semicolon
		 | /* empty */
;

place_area_internal: key_internal {type = INTERNAL; } placelists semicolon
		   | /* empty */
;

place_area_lola: {type = INTERNAL; } placelists semicolon;

placelists: capacity placelist 
| placelists semicolon capacity placelist
;

capacity: { CurrentCapacity = CAPACITY;}
| key_safe    colon  {CurrentCapacity = 1;}
| key_safe number  colon { 
				sscanf($2, "%u", &CurrentCapacity);
			}
;

placelist:  placelist comma place 
|  place 
;

place: nodeident {
	if(PlaceTable->lookup($1)) {
		yyerror("Place name used twice");
	}
	P = new owfnPlace($1, type, PN);
	if (type == INPUT) {
		PN->placeInputCnt++;
	} else if (type == OUTPUT) {
		PN->placeOutputCnt++;
	} 
	PS = new PlSymbol(P);
	P->capacity = CurrentCapacity;
	P->nrbits = CurrentCapacity > 0 ? logzwo(CurrentCapacity) : 32;
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
	PS = (PlSymbol *) PlaceTable->lookup($1);
	if(!PS) {
		yyerror("place does not exist");
	}
	sscanf($3,"%u",&i);
	*(PS->place) += i;
}
;

finalmarkinglist: 
| finalmarking
| finalmarkinglist comma finalmarking
;

finalmarking: nodeident colon number {
	unsigned int i;
	PS = (PlSymbol *) PlaceTable->lookup($1);
	if(!PS) {
		yyerror("place does not exist");
	}
	sscanf($3,"%u",&i);
	PN->FinalMarking[PS->place->index] = i;
}
;


transitionlist: transition
| transitionlist transition
;

transition: key_transition tname key_consume arclist semicolon key_produce arclist semicolon  {
	unsigned int card;
	unsigned int i;
	arc_list * current;
	/* 1. Transition anlegen */
	if(TransitionTable -> lookup($2)) {
		yyerror("transition name used twice");
	}
	TS = new TrSymbol($2);
	T = TS->transition = new owfnTransition(TS->name);	// counter++ in PN
								// KS: macht Konstruktor von owfnTransition
	/* 2. Inliste eintragen */

	/* Anzahl der Boegen */
	for(card = 0, current = $4; current; card++, current = current->next);
		
	T->ArrivingArcs = new  Arc * [card+10];
	/* Schleife ueber alle Boegen */
	for(current = $4;current;current = current -> next) {
	/* gibt es Bogen schon? */

		for(i = 0; i < T->NrOfArriving;i++) {
			if(current->place -> place == T->ArrivingArcs[i]->pl) {
				/* Bogen existiert, nur Vielfachheit addieren */
				*(T->ArrivingArcs[i]) += current->nu;
				break;
			}
		}

		if(i >= T->NrOfArriving) {
			T->ArrivingArcs[T->NrOfArriving] = new Arc(T,current->place->place,true,current->nu);
			T->NrOfArriving++;
			current -> place -> place -> references ++;
		}
	}

	/* 2. Outliste eintragen */

	/* Anzahl der Boegen */
	for(card = 0, current = $7; current; card++, current = current->next);

	T->LeavingArcs = new  Arc * [card+10];
	/* Schleife ueber alle Boegen */
	for(current = $7; current; current = current->next) {
		/* gibt es Bogen schon? */

		for(i = 0; i < T->NrOfLeaving; i++) {
			if(current->place->place == T->LeavingArcs[i]->pl) {
				/* Bogen existiert, nur Vielfachheit addieren */
				*(T->LeavingArcs[i]) += current->nu;
				break;
			}
		}

		if(i >= T->NrOfLeaving) {
			T->LeavingArcs[T->NrOfLeaving] = new Arc(T,current->place->place, false, current->nu);
			T->NrOfLeaving++;
			current->place->place->references++;
		}
	}

}
;


tname:   ident 
| number 
;


arclist: { $$ = (arc_list *) 0;}
| arc {$$ = $1;}
| arc comma arclist {
			$1-> next = $3;
			$$ = $1;
		}
;

arc: nodeident colon number {
	unsigned int i;
	PS = (PlSymbol *) PlaceTable -> lookup($1);
	if(!PS) {
		yyerror("place does not exist");
	}
	$$ = new arc_list;
	$$->place = PS;
	$$->next = (arc_list *)  0;
	sscanf($3,"%u",&i);
	$$->nu = i;
    }
;

statepredicate: lpar statepredicate rpar {
	$$ = $2;
}
| statepredicate op_and statepredicate {
	$$ = new binarybooleanformula(conj,$1,$3);
}
| statepredicate op_or statepredicate {
	$$ = new binarybooleanformula(disj,$1,$3);
}
| op_not statepredicate {
	$$ = new unarybooleanformula(neg,$2);
}
| nodeident op_eq number {
	unsigned int i;
	PS = (PlSymbol *) PlaceTable->lookup($1);
	if(!PS) {
		yyerror("place does not exist");
	}
	sscanf($3,"%u",&i);
	$$ = new atomicformula(eq,PS->place,i);
}
| nodeident op_ne number {
	unsigned int i;
	PS = (PlSymbol *) PlaceTable->lookup($1);
	if(!PS) {
		yyerror("place does not exist");
	}
	sscanf($3,"%u",&i);
	$$ = new atomicformula(neq,PS->place,i);
}
| nodeident op_lt number {
	unsigned int i;
	PS = (PlSymbol *) PlaceTable->lookup($1);
	if(!PS) {
		yyerror("place does not exist");
	}
	sscanf($3,"%u",&i);
	$$ = new atomicformula(lt,PS->place,i);
}
| nodeident op_gt number {
	unsigned int i;
	PS = (PlSymbol *) PlaceTable->lookup($1);
	if(!PS) {
		yyerror("place does not exist");
	}
	sscanf($3,"%u",&i);
	$$ = new atomicformula(gt,PS->place,i);
}
| nodeident op_ge number {
	unsigned int i;
	PS = (PlSymbol *) PlaceTable->lookup($1);
	if(!PS) {
		yyerror("place does not exist");
	}
	sscanf($3,"%u",&i);
	$$ = new atomicformula(geq,PS->place,i);
}
| nodeident op_le number {
	unsigned int i;
	PS = (PlSymbol *) PlaceTable->lookup($1);
	if(!PS) {
		yyerror("place does not exist");
	}
	sscanf($3,"%u",&i);
	$$ = new atomicformula(leq,PS->place,i);
}
