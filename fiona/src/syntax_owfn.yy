/* *****************************************************************************\
   * Copyright 2005, 2006 Peter Massuthe, Daniela Weinberg, Karsten Wolf,      *
   *                      Jan Bretschneider, Christian Gierds                  *
   *                                                                           *
   * This file is part of Fiona.                                               *
   *                                                                           *
   * Fiona is free software; you can redistribute it and/or modify it          *
   * under the terms of the GNU General Public License as published by the     *
   * Free Software Foundation; either version 2 of the License, or (at your    *
   * option) any later version.                                                *
   *                                                                           *
   * Fiona is distributed in the hope that it will be useful, but WITHOUT      *
   * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or     *
   * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for  *
   * more details.                                                             *
   *                                                                           *
   * You should have received a copy of the GNU General Public License along   *
   * with Fiona; if not, write to the Free Software Foundation, Inc., 51       *
   * Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.                      *
   *****************************************************************************/

/*!
 * \file    syntax_owfn.yy
 *
 * \brief   the parser
 *
 * \author  responsible: Daniela Weinberg <weinberg@informatik.hu-berlin.de>
 *
 * \note    This file is part of the tool Fiona and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 */

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
extern char* owfn_yytext;
extern int owfn_yylex();


// defined in "debug.h"
extern int owfn_yyerror(const char *);

extern unsigned int numberOfEvents;

//using namespace std;

#include "mynew.h"
#include "dimensions.h"
#include "binDecision.h"
#include "symboltab.h"
#include "owfnTransition.h"
#include "owfnPlace.h"
#include "owfn.h"
#include "formula.h"
#include "options.h"

#include "main.h"

#include<stdio.h>
#include<limits.h>
#include<set>



/* list of places and multiplicities to become arcs */
class arc_list {
 public:
	PlSymbol * place;
	unsigned int nu;
    arc_list * next;

    ~arc_list()
    {
        // It's our responsibility to delete 'next' so that eventually the
        // whole linked list is deleted. We shall not delete 'place' because
        // other classes still want to use the PlSymbol pointed to by 'place'.
        delete next;
    }

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
oWFN * PN;					// the petri net

placeType type = INTERNAL;		/* type of place */

%}

// Bison options
%name-prefix="owfn_yy"

// the terminal symbols (tokens)

%token key_safe key_place key_internal key_input key_output
%token key_marking key_finalmarking key_finalcondition
%token key_transition key_consume key_produce
%token key_all_other_places_empty
%token key_all_other_internal_places_empty
%token key_all_other_external_places_empty
%token key_max_unique_events key_on_loop key_max_occurrences
%token key_true key_false lcontrol rcontrol
%token comma colon semicolon ident number negative_number
%token lpar rpar

%nonassoc op_eq op_ne op_gt op_lt op_ge op_le
%left op_not
%left op_and
%left op_or


// the start symbol of the grammar
// %start tProcess

// Bison generates a list of all used tokens in file "syntax.h" (for flex)
%token_table


%union {
	char * str;
	int value;
	int * exl;
	arc_list * al;
	owfnPlace * pl;
	owfnTransition * tr;
	TrSymbol * ts;
	formula * form;
}


/* the types of the non-terminal symbols */
%type <str> ident
%type <str> number
%type <str> negative_number
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

net: key_place place_area key_marking
		{
			unsigned int i = 0;

			// Create array of places
			PN->Places = new owfnPlace * [PlaceTable->getSize()];

			PlSymbol* plSymbol = NULL;
			PlaceTable->initGetNextSymbol();
			while ((plSymbol = PlaceTable->getNextSymbol()) != NULL) {
				// Ignore (do not add to PN) external places if oWFN should
				// be matched with an OG. That is because we match the
				// reachability graph of the inner of the oWFN with the OG.
				if (options[O_MATCH] &&
					plSymbol->getPlace()->type != INTERNAL)
				{
					continue;
				}

				PN->addPlace(i++, plSymbol->getPlace());
			}
			
			PN->CurrentMarking = new unsigned int [PlaceTable->getSize()];
			PN->FinalMarking = new unsigned int [PlaceTable->getSize()];
	
		  	for (unsigned int i = 0; i < PlaceTable->getSize(); i++) {
		  		PN->FinalMarking[i] = 0;
		  	}
		  
			
		}
		
		markinglist semicolon final transitionlist 
		{
			// Create array of transitions 
			PN->Transitions = new owfnTransition * [TransitionTable->getSize()];
			unsigned int i = 0;
			TrSymbol* trSymbol = NULL;
			TransitionTable->initGetNextSymbol();
			while ((trSymbol = TransitionTable->getNextSymbol()) != NULL) {
				PN->addTransition(i++, trSymbol->getTransition());
			}

			// Create arc list of places pass 1 (count nr of arcs)
			for(unsigned int i = 0; i < PN->getTransitionCnt(); i++) {
				for(unsigned int j = 0; j < PN->Transitions[i]->NrOfArriving; j++) {
					owfnPlace* pl = PN->Transitions[i]->ArrivingArcs[j]->pl;

					// Ignore external places if oWFN should be matched with an
					// OG. That is because we match the reachability graph of
					// the inner of the oWFN with the OG.
					if (options[O_MATCH] && pl->type != INTERNAL) {
						continue;
					}

					pl->NrOfLeaving++;
				}
				for(unsigned int j=0;j < PN->Transitions[i]->NrOfLeaving;j++) {
					owfnPlace* pl = PN->Transitions[i]->LeavingArcs[j]->pl;

					// Ignore external places if oWFN should be matched with an
					// OG. That is because we match the reachability graph of
					// the inner of the oWFN with the OG.
					if (options[O_MATCH] && pl->type != INTERNAL) {
						continue;
					}

					pl->NrOfArriving++;
				}
			}
		
			// pass 2 (allocate arc arrays)
			for (unsigned int i = 0; i < PN->getPlaceCnt(); i++) {
				PN->Places[i]->ArrivingArcs = new Arc * [PN->Places[i]->NrOfArriving+10];
				PN->Places[i]->NrOfArriving = 0;
				PN->Places[i]->LeavingArcs = new Arc * [PN->Places[i]->NrOfLeaving+10];
				PN->Places[i]->NrOfLeaving = 0;
			}
		
			// pass 3 (fill in arcs)
			for (unsigned int i = 0; i < TransitionTable->getSize(); i++) {
				for(unsigned int j=0; j < PN->Transitions[i]->NrOfLeaving;j++) {
					owfnPlace * pl = PN->Transitions[i]->LeavingArcs[j]->pl;
                    if (options[O_MATCH] && pl->type != INTERNAL) {
                        continue;
                    }
					pl->ArrivingArcs[pl->NrOfArriving] = PN->Transitions[i]->LeavingArcs[j];
					pl->NrOfArriving++;
					
//					if (pl->type == OUTPUT) {
//						PN->commDepth += PN->Transitions[i]->LeavingArcs[j]->Multiplicity;
//					}
				}
				for (unsigned int j = 0;j < PN->Transitions[i]->NrOfArriving; j++) {
					owfnPlace * pl = PN->Transitions[i]->ArrivingArcs[j]->pl;
                    if (options[O_MATCH] && pl->type != INTERNAL) {
                        continue;
                    }
					pl->LeavingArcs[pl->NrOfLeaving] = PN->Transitions[i]->ArrivingArcs[j];
					pl->NrOfLeaving ++;
					
//					if (pl->type == INPUT) {
//						PN->commDepth += PN->Transitions[i]->ArrivingArcs[j]->Multiplicity;
//					}
				}
			}
		}
;

final: key_finalmarking finalmarkinglist semicolon
| key_finalcondition statepredicate semicolon {
	PN->FinalCondition = $2;

	// merge() and posate() can only be called on FinalCondition after the PN
	// and the FinalCondition (in this order) have been initialized with the
	// initial marking. This is done after parsing is complete.
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
				free($2);
			}
;

placelist:  placelist comma place 
|  place
|  /* empty */ 
;

place: nodeident {
	if(PlaceTable->lookup($1)) {
		string error = "Place name " + string($1) + " was used twice!";
		owfn_yyerror(error.c_str());
	}
	P = new owfnPlace($1, type, PN);
	// Set PS because nonterminal 'controlcommands' depends on it being set.
	PS = new PlSymbol(P);
	PlaceTable->add(PS);
	P->capacity = CurrentCapacity;
	P->nrbits = CurrentCapacity > 0 ? logzwo(CurrentCapacity) : 32;
	P->max_occurence = events_manual;
	free($1);
	if (type == INPUT || type == OUTPUT) {
	    numberOfEvents += events_manual;
	}
    }
    controlcommands
;

nodeident: ident { $$ = $1;}
| number  {$$ = $1; }
;

controlcommands:
  /* emtpy */
| lcontrol commands rcontrol
;

commands:
  /* empty */
| key_max_unique_events op_eq number commands
    {
    }
| key_on_loop op_eq key_true commands
    {
    }
| key_on_loop op_eq key_false commands
    {
    }
| key_max_occurrences op_eq number commands
    {
        sscanf($3, "%u", &(PS->getPlace()->max_occurence));
        free($3);
        if (options[O_EVENT_USE_MAX] &&
            PS->getPlace()->max_occurence > events_manual)
        {
            PS->getPlace()->max_occurence = events_manual;
        }
        numberOfEvents += PS->getPlace()->max_occurence - events_manual;
    }
| key_max_occurrences op_eq negative_number commands
    {
        sscanf($3, "%d", &(PS->getPlace()->max_occurence));
        free($3);
        if (options[O_EVENT_USE_MAX] &&
            PS->getPlace()->max_occurence > events_manual)
        {
            PS->getPlace()->max_occurence = events_manual;
        }
        numberOfEvents += PS->getPlace()->max_occurence - events_manual;
    }
;

markinglist:
  /* empty */ 
| marking
| markinglist comma marking
;

marking: 
  nodeident colon number 
      {
	unsigned int i;
	PS = (PlSymbol *) PlaceTable->lookup($1);
	if(!PS) {
		string error = "Place " + string($1) + " does not exist!";
		owfn_yyerror(error.c_str());
	}
	sscanf($3,"%u",&i);
	*(PS->getPlace()) += i;
	free($1);
	free($3);
      } 
| nodeident
      {
	unsigned int i;
	PS = (PlSymbol *) PlaceTable->lookup($1);
	if(!PS) {
		string error = "Place " + string($1) + " does not exist!";
		owfn_yyerror(error.c_str());
	}
	sscanf("1","%u",&i);
	*(PS->getPlace()) += i;
	free($1);
      }
;

finalmarkinglist: 
| finalmarking
| finalmarkinglist comma finalmarking
;

finalmarking: 
  nodeident colon number 
      {
	unsigned int i;
	PS = (PlSymbol *) PlaceTable->lookup($1);
	if(!PS) {
		string error = "Place " + string($1) + " does not exist!";
		owfn_yyerror(error.c_str());
	}
	sscanf($3,"%u",&i);
	PN->FinalMarking[PS->getPlace()->index] = i;
	free($1);
	free($3);
      }
| nodeident
      {
	unsigned int i;
	PS = (PlSymbol *) PlaceTable->lookup($1);
	if(!PS) {
		string error = "Place " + string($1) + " does not exist!";
		owfn_yyerror(error.c_str());
	}
	sscanf("1","%u",&i);
	PN->FinalMarking[PS->getPlace()->index] = i;
	free($1);
      }
;


transitionlist: transitionlist transition
| /* empty */
;

transition: key_transition tname key_consume arclist semicolon key_produce arclist semicolon  {
	unsigned int card;
	unsigned int i;
	arc_list * current;
	/* 1. Transition anlegen */
	if(TransitionTable -> lookup($2)) {
		string error = "Transition name " + string($2) + " was used twice!";
		owfn_yyerror(error.c_str());
	}
	T = new owfnTransition($2);	// counter++ in PN
	TransitionTable->add(new TrSymbol(T));

	/* Anzahl der Boegen */
	card = 0;
	for(current = $4; current; current = current->next) {
		// Ignore external places if oWFN should be matched with an
		// OG. That is because we match the reachability graph of
		// the inner of the oWFN with the OG.
		if (options[O_MATCH] && current->place->getPlace()->type != INTERNAL) {
			continue;
		}

		++card;
	}
		
	T->ArrivingArcs = new  Arc * [card+10];
	/* Schleife ueber alle Boegen */
	for(current = $4; current; current = current->next) {
		if (current->place->getPlace()->type == OUTPUT) {
			string msg = string("Transition '") + T->name + "' reads from "
				"output place '" + current->place->getPlace()->name +
				"' which is not allowed.";
			owfn_yyerror(msg.c_str());
		}

		if (options[O_MATCH] && current->place->getPlace()->type != INTERNAL) {
			if (T->hasNonTauLabelForMatching()) {
				string msg = string("Transition '") + T->name + "' sends or "
					"receives more than one message which is not allowed.";
				owfn_yyerror(msg.c_str());
			}
			string labelForMatching = string("?") +
				current->place->getPlace()->name;
			T->setLabelForMatching(labelForMatching);
			continue;
		}
	/* gibt es Bogen schon? */

		for(i = 0; i < T->NrOfArriving;i++) {
			if(current->place->getPlace() == T->ArrivingArcs[i]->pl) {
				/* Bogen existiert, nur Vielfachheit addieren */
				*(T->ArrivingArcs[i]) += current->nu;
				break;
			}
		}

		if(i >= T->NrOfArriving) {
			T->ArrivingArcs[T->NrOfArriving] =
				new Arc(T, current->place->getPlace(), true, current->nu);
			T->NrOfArriving++;
			current->place->getPlace()->references ++;
		}
	}

	/* 2. Outliste eintragen */

	/* Anzahl der Boegen */
	card = 0;
	for(current = $7; current; current = current->next) {
		++card;
	}

	T->LeavingArcs = new  Arc * [card+10];
	/* Schleife ueber alle Boegen */
	for(current = $7; current; current = current->next) {
		if (current->place->getPlace()->type == INPUT) {
			string msg = string("Transition '") + T->name + "' writes to an "
				"input place '" + current->place->getPlace()->name +
				"' which is not allowed.";
			owfn_yyerror(msg.c_str());
		}

		if (options[O_MATCH] && current->place->getPlace()->type != INTERNAL) {
			if (T->hasNonTauLabelForMatching()) {
				string msg = string("Transition '") + T->name + "' sends or "
					"receives more than one message which is not allowed.";
				owfn_yyerror(msg.c_str());
			}
			string labelForMatching = string("!") +
				current->place->getPlace()->name;
			T->setLabelForMatching(labelForMatching);
			continue;
		}
		/* gibt es Bogen schon? */

		for(i = 0; i < T->NrOfLeaving; i++) {
			if(current->place->getPlace() == T->LeavingArcs[i]->pl) {
				/* Bogen existiert, nur Vielfachheit addieren */
				*(T->LeavingArcs[i]) += current->nu;
				break;
			}
		}

		if(i >= T->NrOfLeaving) {
			T->LeavingArcs[T->NrOfLeaving] =
				new Arc(T,current->place->getPlace(), false, current->nu);
			T->NrOfLeaving++;
			current->place->getPlace()->references++;
		}
	}

	free($2);

	// delete arc_list because they are no longer used.
	delete $4;
	delete $7;
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

arc: 
  nodeident colon number 
      {
	unsigned int i;
	PS = (PlSymbol *) PlaceTable -> lookup($1);
	if(!PS) {
		string error = "Place " + string($1) + " does not exist!";
		owfn_yyerror(error.c_str());
	}
	$$ = new arc_list;
	$$->place = PS;
	$$->next = (arc_list *)  0;
	sscanf($3,"%u",&i);
	$$->nu = i;
	free($1);
	free($3);
      }
| nodeident
      {
	unsigned int i;
	PS = (PlSymbol *) PlaceTable -> lookup($1);
	if(!PS) {
		string error = "Place " + string($1) + " does not exist!";
		owfn_yyerror(error.c_str());
	}
	$$ = new arc_list;
	$$->place = PS;
	$$->next = (arc_list *)  0;
	sscanf("1","%u",&i);
	$$->nu = i;
	free($1);
      }
;

statepredicate: lpar statepredicate rpar {
	$$ = $2;
}
| statepredicate op_and statepredicate {
	$$ = new binarybooleanformula(conj,$1,$3);
}
| statepredicate op_and key_all_other_places_empty {
	//
	// Warning: code duplication! Keep the rules for
	// key_all_other_places_empty, key_all_other_internal_places_empty and
	// key_all_other_external_places_empty in sync!
	//
	formula* lhs = $1;
	set<owfnPlace*> places_in_lhs;
	lhs->collectplaces(places_in_lhs);
	set<owfnPlace*> all_other_places;
	for (size_t iplace = 0; iplace != PN->getPlaceCnt(); ++iplace)
	{
		owfnPlace* current_place = PN->Places[iplace];
		if (places_in_lhs.find(current_place) == places_in_lhs.end())
		{
			all_other_places.insert(current_place);
		}
	}

	if (all_other_places.size() == 0)
	{
		$$ = $1;
	}
	else
	{
		booleanformula* rhs = new booleanformula();
		rhs->type = conj;
		rhs->cardsub = all_other_places.size();
		rhs->sub = new formula*[rhs->cardsub];
		size_t iplace = 0;
		for (set<owfnPlace*>::const_iterator itplace = all_other_places.begin();
			 itplace != all_other_places.end(); ++itplace)
		{
			rhs->sub[iplace] = new atomicformula(eq, *itplace, 0);
            rhs->sub[iplace]->parent = rhs;
            rhs->sub[iplace]->parentindex = iplace;
			++iplace;
		}
		
		$$ = new binarybooleanformula(conj, lhs, rhs);
	}
}
| statepredicate op_and key_all_other_internal_places_empty {
	//
	// Warning: code duplication! Keep the rules for
	// key_all_other_places_empty, key_all_other_internal_places_empty and
	// key_all_other_external_places_empty in sync!
	//
	formula* lhs = $1;
	set<owfnPlace*> places_in_lhs;
	lhs->collectplaces(places_in_lhs);
	set<owfnPlace*> all_other_internal_places;
	for (size_t iplace = 0; iplace != PN->getPlaceCnt(); ++iplace)
	{
		owfnPlace* current_place = PN->Places[iplace];
		if ((current_place->type == INTERNAL) &&
		    (places_in_lhs.find(current_place) == places_in_lhs.end()))
		{
			all_other_internal_places.insert(current_place);
		}
	}

	if (all_other_internal_places.size() == 0)
	{
		$$ = $1;
	}
	else
	{
		booleanformula* rhs = new booleanformula();
		rhs->type = conj;
		rhs->cardsub = all_other_internal_places.size();
		rhs->sub = new formula*[rhs->cardsub];
		size_t iplace = 0;
		for (set<owfnPlace*>::const_iterator itplace =
		     all_other_internal_places.begin();
		     itplace != all_other_internal_places.end(); ++itplace)
		{
			rhs->sub[iplace] = new atomicformula(eq, *itplace, 0);
            rhs->sub[iplace]->parent = rhs;
            rhs->sub[iplace]->parentindex = iplace;
			++iplace;
		}
		
		$$ = new binarybooleanformula(conj, lhs, rhs);
	}
}
| statepredicate op_and key_all_other_external_places_empty {
	//
	// Warning: code duplication! Keep the rules for
	// key_all_other_places_empty, key_all_other_internal_places_empty and
	// key_all_other_external_places_empty in sync!
	//
	formula* lhs = $1;
	set<owfnPlace*> places_in_lhs;
	lhs->collectplaces(places_in_lhs);
	set<owfnPlace*> all_other_external_places;

    // We cannot use PN->inputPlacesArray and PN->outputPlacesArray here
    // because they are not initialized yet. They would be initialized only
    // after PN->initialize() were called, but we cannot wait until then.
	for (size_t iplace = 0; iplace != PN->getPlaceCnt(); ++iplace)
	{
		owfnPlace* current_place = PN->Places[iplace];
		if ((current_place->type != INTERNAL) &&
		    (places_in_lhs.find(current_place) == places_in_lhs.end()))
		{
			all_other_external_places.insert(current_place);
		}
	}

	if (all_other_external_places.size() == 0)
	{
		$$ = $1;
	}
	else
	{
		booleanformula* rhs = new booleanformula();
		rhs->type = conj;
		rhs->cardsub = all_other_external_places.size();
		rhs->sub = new formula*[rhs->cardsub];
		size_t iplace = 0;
		for (set<owfnPlace*>::const_iterator itplace =
		     all_other_external_places.begin();
		     itplace != all_other_external_places.end(); ++itplace)
		{
			rhs->sub[iplace] = new atomicformula(eq, *itplace, 0);
            rhs->sub[iplace]->parent = rhs;
            rhs->sub[iplace]->parentindex = iplace;
			++iplace;
		}
		
		$$ = new binarybooleanformula(conj, lhs, rhs);
	}
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
		string error = "Place " + string($1) + " does not exist!";
		owfn_yyerror(error.c_str());
	}
	sscanf($3,"%u",&i);
	$$ = new atomicformula(eq, PS->getPlace(), i);
	free($1);
	free($3);
}
| nodeident op_ne number {
	unsigned int i;
	PS = (PlSymbol *) PlaceTable->lookup($1);
	if(!PS) {
		string error = "Place " + string($1) + " does not exist!";
		owfn_yyerror(error.c_str());
	}
	sscanf($3,"%u",&i);
	$$ = new atomicformula(neq, PS->getPlace(), i);
	free($1);
	free($3);
}
| nodeident op_lt number {
	unsigned int i;
	PS = (PlSymbol *) PlaceTable->lookup($1);
	if(!PS) {
		string error = "Place " + string($1) + " does not exist!";
		owfn_yyerror(error.c_str());
	}
	sscanf($3,"%u",&i);
	$$ = new atomicformula(lt, PS->getPlace(), i);
	free($1);
	free($3);
}
| nodeident op_gt number {
	unsigned int i;
	PS = (PlSymbol *) PlaceTable->lookup($1);
	if(!PS) {
		string error = "Place " + string($1) + " does not exist!";
		owfn_yyerror(error.c_str());
	}
	sscanf($3,"%u",&i);
	$$ = new atomicformula(gt, PS->getPlace(), i);
	free($1);
	free($3);
}
| nodeident op_ge number {
	unsigned int i;
	PS = (PlSymbol *) PlaceTable->lookup($1);
	if(!PS) {
		string error = "Place " + string($1) + " does not exist!";
		owfn_yyerror(error.c_str());
	}
	sscanf($3,"%u",&i);
	$$ = new atomicformula(geq, PS->getPlace(), i);
	free($1);
	free($3);
}
| nodeident op_le number {
	unsigned int i;
	PS = (PlSymbol *) PlaceTable->lookup($1);
	if(!PS) {
		string error = "Place " + string($1) + " does not exist!";
		owfn_yyerror(error.c_str());
	}
	sscanf($3,"%u",&i);
	$$ = new atomicformula(leq, PS->getPlace(), i);
	free($1);
	free($3);
}

