/*****************************************************************************\
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
owfnTransition *T; /** The transition that is currently parsed. */
Symbol * S;
PlSymbol * PS;
oWFN * PN;					// the petri net
string current_port; ///< the currently parsed port

placeType type = INTERNAL;		/* type of place */

/**
 * See ignoredPlacesDueToMatching.
 */
typedef std::set<owfnPlace*> ignoredPlacesDueToMatching_t;

/**
 * Set of all places that were not added to the parsed oWFN, because matching
 * should be performed on the oWFN. Those places must be deleted when parsing
 * is done, because they won't be delete by the oWFN.
 */
ignoredPlacesDueToMatching_t ignoredPlacesDueToMatching;

enum InTransitionParsePosition { IN_CONSUME, IN_PRODUCE };
InTransitionParsePosition inTransitionParsePosition; 

%}

// Bison options
%name-prefix="owfn_yy"

// the terminal symbols (tokens)

%token KEY_SAFE KEY_PLACE KEY_INTERNAL KEY_INPUT KEY_OUTPUT
%token KEY_MARKING KEY_FINALMARKING KEY_FINALCONDITION
%token KEY_TRANSITION KEY_CONSUME KEY_PRODUCE KEY_PORT KEY_PORTS
%token KEY_ALL_OTHER_PLACES_EMPTY
%token KEY_ALL_OTHER_INTERNAL_PLACES_EMPTY
%token KEY_ALL_OTHER_EXTERNAL_PLACES_EMPTY
%token KEY_MAX_UNIQUE_EVENTS KEY_ON_LOOP KEY_MAX_OCCURRENCES
%token KEY_TRUE KEY_FALSE LCONTROL RCONTROL
%token COMMA COLON SEMICOLON IDENT NUMBER NEGATIVE_NUMBER
%token LPAR RPAR

%left OP_OR
%left OP_AND
%left OP_NOT
%nonassoc OP_EQ OP_NE OP_GT OP_LT OP_GE OP_LE


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
%type <str> IDENT
%type <str> NUMBER
%type <str> NEGATIVE_NUMBER
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

net:
		{
			ignoredPlacesDueToMatching.clear();
		}
	KEY_PLACE place_area port_area KEY_MARKING
		{
			PlSymbol* plSymbol = NULL;
			PlaceTable->initGetNextSymbol();
			while ((plSymbol = PlaceTable->getNextSymbol()) != NULL) {
				// Ignore (do not add to PN) external places if oWFN should
				// be matched with an OG. That is because we match the
				// reachability graph of the inner of the oWFN with the OG.
				if (options[O_MATCH] &&
					plSymbol->getPlace()->type != INTERNAL)
				{
					ignoredPlacesDueToMatching.insert(plSymbol->getPlace());
					continue;
				}

				PN->addPlace(plSymbol->getPlace());
			}
			
			PN->CurrentMarking = new unsigned int [PlaceTable->getSize()];
			PN->FinalMarking = new unsigned int [PlaceTable->getSize()];
	
		  	for (unsigned int i = 0; i < PlaceTable->getSize(); i++) {
		  		PN->FinalMarking[i] = 0;
		  	}
		  
			
		}
		
		markinglist SEMICOLON final transitionlist
		{
			// fill in arcs
			for (unsigned int i = 0; i < PN->getTransitionCount(); i++) {
				for (unsigned int j=0;
				    j < PN->getTransition(i)->getLeavingArcsCount(); j++)
				{
					owfnPlace * pl = PN->getTransition(i)->getLeavingArc(j)->pl;
					if (options[O_MATCH] && pl->type != INTERNAL)
					{
						continue;
					}
					pl->addArrivingArc(PN->getTransition(i)->getLeavingArc(j));
				}
				for (unsigned int j = 0;
				     j < PN->getTransition(i)->getArrivingArcsCount(); j++)
				{
					owfnPlace * pl = PN->getTransition(i)->getArrivingArc(j)->pl;
					if (options[O_MATCH] && pl->type != INTERNAL)
					{
						continue;
					}
					pl->addLeavingArc(PN->getTransition(i)->getArrivingArc(j));
				}
			}

			// Delete all places that were not added while parsing this oWFN
			// for matching.
			for (ignoredPlacesDueToMatching_t::const_iterator iplace =
			     ignoredPlacesDueToMatching.begin();
			     iplace != ignoredPlacesDueToMatching.end(); ++iplace)
			{
				delete *iplace;
			}
		}
;

final: KEY_FINALMARKING finalmarkinglist SEMICOLON
| KEY_FINALCONDITION statepredicate SEMICOLON {
	PN->FinalCondition = $2;

	// merge() and posate() can only be called on FinalCondition after the PN
	// and the FinalCondition (in this order) have been initialized with the
	// initial marking. This is done after parsing is complete.
}
;

place_area: place_area_internal place_area_input  place_area_output
	  | place_area_lola
;

place_area_input: KEY_INPUT {type = INPUT; } placelists SEMICOLON
		| /* empty */
;

place_area_output: KEY_OUTPUT {type = OUTPUT; } placelists SEMICOLON
		 | /* empty */
;

place_area_internal: KEY_INTERNAL {type = INTERNAL; } placelists SEMICOLON
		   | /* empty */
;

place_area_lola: {type = INTERNAL; } placelists SEMICOLON;

placelists: capacity placelist 
| placelists SEMICOLON capacity placelist
;

capacity: { CurrentCapacity = CAPACITY;}
| KEY_SAFE    COLON  {CurrentCapacity = 1;}
| KEY_SAFE NUMBER  COLON { 
				sscanf($2, "%u", &CurrentCapacity);
				free($2);
			}
;

placelist:  placelist COMMA place 
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
	// set max_occurence to default value
	P->max_occurence = events_manual;
	free($1);
	if (type == INPUT || type == OUTPUT) {
	    numberOfEvents += events_manual;
	}
    }
    controlcommands
;

nodeident: IDENT { $$ = $1;}
| NUMBER  {$$ = $1; }
;

controlcommands:
  /* emtpy */
| LCONTROL commands RCONTROL
;

commands:
  /* empty */
| KEY_MAX_UNIQUE_EVENTS OP_EQ NUMBER commands
    {
    }
| KEY_ON_LOOP OP_EQ KEY_TRUE commands
    {
    }
| KEY_ON_LOOP OP_EQ KEY_FALSE commands
    {
    }
| KEY_MAX_OCCURRENCES OP_EQ NUMBER commands
    {
        // set max_occurence to value that was given in oWFN file
        sscanf($3, "%u", &(PS->getPlace()->max_occurence));
        free($3);
//    	options[O_EVENT_USE_MAX] = true;
//        if (options[O_EVENT_USE_MAX] &&
//            PS->getPlace()->max_occurence > events_manual)
//        {
//            PS->getPlace()->max_occurence = events_manual;
//        }
//        numberOfEvents += PS->getPlace()->max_occurence - events_manual;
    }
| KEY_MAX_OCCURRENCES OP_EQ NEGATIVE_NUMBER commands
    {
        sscanf($3, "%d", &(PS->getPlace()->max_occurence));
        free($3);
//        if (options[O_EVENT_USE_MAX] &&
//            PS->getPlace()->max_occurence > events_manual)
//        {
//            PS->getPlace()->max_occurence = events_manual;
//        }
//        numberOfEvents += PS->getPlace()->max_occurence - events_manual;
    }
;



/*************
 * the ports *
 *************/

port_area:
  /* empty */
| KEY_PORTS port_list
;

port_list:
  port_definition
| port_list port_definition
;

port_definition:
  KEY_PORT nodeident
    { current_port = std::string($2); }
  COLON port_participant_list SEMICOLON
;

port_participant_list:
  port_participant
| port_participant_list COMMA port_participant
;

port_participant:
  nodeident
    {
        PS = static_cast<PlSymbol *>( PlaceTable->lookup($1) );
	
        if (PS == NULL) {
            string error = "Place " + string($1) + " does not exist!";
            owfn_yyerror(error.c_str());
        } else {
            PN->add_place_to_port(PS->getPlace(), current_port);
        }
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
| finalmarkinglist COMMA finalmarking
;

finalmarking: 
  nodeident COLON NUMBER 
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

transition: KEY_TRANSITION tname
	{
		T = new owfnTransition($2);
		if (!PN->addTransition(T))
		{
			string error = "Transition name " + string($2) + " was used twice!";
			owfn_yyerror(error.c_str());
		}
	}
	KEY_CONSUME
	{
		inTransitionParsePosition = IN_CONSUME;
	}
	arclist SEMICOLON KEY_PRODUCE
	{
		inTransitionParsePosition = IN_PRODUCE;
	}
	arclist SEMICOLON
	{
	unsigned int card;
	unsigned int i;
	arc_list * current;

	/* Anzahl der Boegen */
	card = 0;
	for(current = $6; current; current = current->next) {
		// Ignore external places if oWFN should be matched with an
		// OG. That is because we match the reachability graph of
		// the inner of the oWFN with the OG.
		if (options[O_MATCH] && current->place->getPlace()->type != INTERNAL) {
			continue;
		}

		++card;
	}
		
	/* Schleife ueber alle Boegen */
	for(current = $6; current; current = current->next) {
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
			T->setLabelForMatching(
			    current->place->getPlace()->getLabelForMatching());

			continue;
		}
	/* gibt es Bogen schon? */

		for(i = 0; i < T->getArrivingArcsCount();i++) {
			if(current->place->getPlace() == T->getArrivingArc(i)->pl) {
				/* Bogen existiert, nur Vielfachheit addieren */
				*(T->getArrivingArc(i)) += current->nu;
				break;
			}
		}

		if(i >= T->getArrivingArcsCount()) {
			T->addArrivingArc(
				new Arc(T, current->place->getPlace(), true, current->nu)
			);
			current->place->getPlace()->references ++;
		}
	}

	/* 2. Outliste eintragen */

	/* Anzahl der Boegen */
	card = 0;
	for(current = $10; current; current = current->next) {
		++card;
	}

	/* Schleife ueber alle Boegen */
	for(current = $10; current; current = current->next) {
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
			T->setLabelForMatching(
			    current->place->getPlace()->getLabelForMatching());

			continue;
		}
		/* gibt es Bogen schon? */

		for(i = 0; i < T->getLeavingArcsCount(); i++) {
			if(current->place->getPlace() == T->getLeavingArc(i)->pl) {
				/* Bogen existiert, nur Vielfachheit addieren */
				*(T->getLeavingArc(i)) += current->nu;
				break;
			}
		}

		if(i >= T->getLeavingArcsCount()) {
			T->addLeavingArc(
				new Arc(T,current->place->getPlace(), false, current->nu)
			);
			current->place->getPlace()->references++;
		}
	}

	free($2);

	// delete arc_list because they are no longer used.
	delete $6;
	delete $10;
}
;


tname:   IDENT 
| NUMBER 
;


arclist: { $$ = (arc_list *) 0;}
| arc {$$ = $1;}
| arc COMMA arclist {
			$1-> next = $3;
			$$ = $1;
		}
;

arc: 
  nodeident COLON NUMBER 
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

statepredicate: LPAR statepredicate RPAR {
	$$ = $2;
}
| statepredicate OP_AND statepredicate {
	$$ = new binarybooleanformula(conj,$1,$3);
}
| statepredicate OP_AND KEY_ALL_OTHER_PLACES_EMPTY {
	//
	// Warning: code duplication! Keep the rules for
	// KEY_ALL_OTHER_PLACES_EMPTY, KEY_ALL_OTHER_INTERNAL_PLACES_EMPTY and
	// KEY_ALL_OTHER_EXTERNAL_PLACES_EMPTY in sync!
	//
	formula* lhs = $1;
	set<owfnPlace*> places_in_lhs;
	lhs->collectplaces(places_in_lhs);
	set<owfnPlace*> all_other_places;
	for (size_t iplace = 0; iplace != PN->getPlaceCount(); ++iplace)
	{
		owfnPlace* current_place = PN->getPlace(iplace);
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
| statepredicate OP_AND KEY_ALL_OTHER_INTERNAL_PLACES_EMPTY {
	//
	// Warning: code duplication! Keep the rules for
	// KEY_ALL_OTHER_PLACES_EMPTY, KEY_ALL_OTHER_INTERNAL_PLACES_EMPTY and
	// KEY_ALL_OTHER_EXTERNAL_PLACES_EMPTY in sync!
	//
	formula* lhs = $1;
	set<owfnPlace*> places_in_lhs;
	lhs->collectplaces(places_in_lhs);
	set<owfnPlace*> all_other_internal_places;
	for (size_t iplace = 0; iplace != PN->getPlaceCount(); ++iplace)
	{
		owfnPlace* current_place = PN->getPlace(iplace);
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
| statepredicate OP_AND KEY_ALL_OTHER_EXTERNAL_PLACES_EMPTY {
	//
	// Warning: code duplication! Keep the rules for
	// KEY_ALL_OTHER_PLACES_EMPTY, KEY_ALL_OTHER_INTERNAL_PLACES_EMPTY and
	// KEY_ALL_OTHER_EXTERNAL_PLACES_EMPTY in sync!
	//
	formula* lhs = $1;
	set<owfnPlace*> places_in_lhs;
	lhs->collectplaces(places_in_lhs);
	set<owfnPlace*> all_other_external_places;

    // We cannot use PN->inputPlacesArray and PN->outputPlacesArray here
    // because they are not initialized yet. They would be initialized only
    // after PN->initialize() were called, but we cannot wait until then.
	for (size_t iplace = 0; iplace != PN->getPlaceCount(); ++iplace)
	{
		owfnPlace* current_place = PN->getPlace(iplace);
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
| statepredicate OP_OR statepredicate {
	$$ = new binarybooleanformula(disj,$1,$3);
}
| OP_NOT statepredicate {
	$$ = new unarybooleanformula(neg,$2);
}
| nodeident OP_EQ NUMBER {
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
| nodeident OP_NE NUMBER {
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
| nodeident OP_LT NUMBER {
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
| nodeident OP_GT NUMBER {
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
| nodeident OP_GE NUMBER {
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
| nodeident OP_LE NUMBER {
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

