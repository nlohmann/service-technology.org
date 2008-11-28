/*****************************************************************************\
 Copyright (c) 2008  Robert Danitz, Christian Gierds, Niels Lohmann,
                     Peter Massuthe, Richard Mu"ller, Janine Ott,
                     Jan Su"rmeli, Daniela Weinberg, Karsten Wolf,
                     Martin Znamirowski

 Copyright (c) 2007  Jan Bretschneider, Robert Danitz, Christian Gierds,
                     Kathrin Kaschner, Leonard Kern, Niels Lohmann,
                     Peter Massuthe, Daniela Weinberg, Karsten Wolf

 Copyright (c) 2006  Kathrin Kaschner, Peter Massuthe, Daniela Weinberg,
                     Karsten Wolf

 This file is part of Fiona, see <http://service-technology.org/fiona>.

 Fiona is free software; you can redistribute it and/or modify it under the
 terms of the GNU General Public License as published by the Free Software
 Foundation; either version 3 of the License, or (at your option) any later
 version.
 
 Fiona is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License along with
 Fiona (see file COPYING). If not, see <http://www.gnu.org/licenses/>.
\*****************************************************************************/

/*!
 * \file    syntax_owfn.yy
 *
 * \brief   the parser
 *
 * \author  responsible: Daniela Weinberg <weinberg@informatik.hu-berlin.de>
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
#include <string>


// [LUHME XV] gehören Deklarationen/C-Code in die .yy-Datei oder in eine eigene Datei?
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

// [LUHME XV] Kommentare ergänzen
int CurrentCapacity;
owfnPlace *P;
owfnTransition *T; /** The transition that is currently parsed. */
Symbol * S;
PlSymbol * PS;
oWFN * PN;                  // the petri net
unsigned int * finalMarking;
string current_port = "";   // the currently parsed port

placeType type = INTERNAL;      /* type of place */

enum InTransitionParsePosition { IN_CONSUME, IN_PRODUCE };
InTransitionParsePosition inTransitionParsePosition; 

%}

// Bison options
%name-prefix="owfn_yy"

// the terminal symbols (tokens)

%token KEY_SAFE KEY_INTERFACE KEY_PLACE KEY_INTERNAL KEY_INPUT KEY_OUTPUT
%token KEY_SYNCHRONIZE KEY_SYNCHRONOUS
%token KEY_MARKING KEY_FINALMARKING KEY_NOFINALMARKING KEY_FINALCONDITION
%token KEY_TRANSITION KEY_CONSUME KEY_PRODUCE KEY_PORT KEY_PORTS
%token KEY_ALL_OTHER_PLACES_EMPTY
%token KEY_ALL_OTHER_INTERNAL_PLACES_EMPTY
%token KEY_ALL_OTHER_EXTERNAL_PLACES_EMPTY
%token KEY_MAX_UNIQUE_EVENTS KEY_ON_LOOP KEY_MAX_OCCURRENCES
%token KEY_TRUE KEY_FALSE LCONTROL RCONTROL
%token COMMA COLON SEMICOLON IDENT NUMBER NEGATIVE_NUMBER
%token LPAR RPAR LBRA RBRA

%left OP_OR
%left OP_AND
%left OP_NOT
%nonassoc OP_EQ OP_NE OP_GT OP_LT OP_GE OP_LE


// the start symbol of the grammar
// %start tProcess

// Bison generates a list of all used tokens in file "syntax.h" (for flex)
%token_table

// [LUHME XV] Kommentare ergänzen, welche Felder werden überhaupt verwendet?
%union {
	char * str;
	int value;
	int * exl;          ///< expression list
	arc_list * al;
	owfnPlace * pl;
	owfnTransition * tr;
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

input:  net
;

/* [LUHME XV] Semantik aus dem Regel-Inneren ans Ende der Regel verlagern */
/* [LUHME XV] evtl. durch Auftrennen der Regel */
net:
		/* [LUHME XV] C-Code zwischen "interface" "KEY_MARKING" */
		interface KEY_MARKING
		{
			// copy place symbol table to PN
			PlSymbol* plSymbol = NULL;
			PlaceTable->initGetNextSymbol();
			while ((plSymbol = PlaceTable->getNextSymbol()) != NULL) {
				PN->addPlace(plSymbol->getPlace());
			}
			// [LUHME XV] in die "oWFN::initialize()"-Methode verschieben
			PN->CurrentMarking = new unsigned int [PlaceTable->getSize()];
			
		}
		
		markinglist SEMICOLON final transitionlist
		{
			// fill in arcs
			for (unsigned int i = 0; i < PN->getTransitionCount(); i++) {
				for (unsigned int j=0;
					j < PN->getTransition(i)->getLeavingArcsCount(); j++)
				{
					owfnPlace * pl = PN->getTransition(i)->getLeavingArc(j)->pl;
					pl->addArrivingArc(PN->getTransition(i)->getLeavingArc(j));
				}
				for (unsigned int j = 0;
					 j < PN->getTransition(i)->getArrivingArcsCount(); j++)
				{
					owfnPlace * pl = PN->getTransition(i)->getArrivingArc(j)->pl;
					pl->addLeavingArc(PN->getTransition(i)->getArrivingArc(j));
				}
			}
		}
;


/********************************
 * the new interface definition *
 ********************************/
/* [LUHME XV] "interface" umbenennen in "placeDeclaration" */
interface:
  KEY_PLACE place_area port_area /* deprecated syntax */
| KEY_INTERFACE interface_area KEY_PLACE {type = INTERNAL; } placelists SEMICOLON /* new Luhme XIV syntax */
;

interface_area:
  place_area_input place_area_output synchronous_area /* only one port without name */
| port_list_new /* list of named ports */
;

port_list_new:
  port_definition_new
| port_list_new port_definition_new
;

port_definition_new:
  KEY_PORT nodeident
	{ current_port = std::string($2); }
  place_area_input place_area_output synchronous_area
;

synchronous_area:
  KEY_SYNCHRONOUS labellist SEMICOLON
| /* empty */
;

labellist:
  nodeident
| labellist COMMA nodeident
;



final:
	/* [LUHME XV] wird das wirklich gebraucht? alternativ: das Schlüsselwort: FINALMARKING weglassen*/
	KEY_NOFINALMARKING /* the net has NO final marking (opposed to the empty final marking) */
|
	KEY_FINALMARKING 
	multiplefinalmarkinglists 
	SEMICOLON
| 
	KEY_FINALCONDITION statepredicate SEMICOLON 
	{
	PN->FinalCondition = $2;

	// merge() and posate() can only be called on FinalCondition after the PN
	// and the FinalCondition (in this order) have been initialized with the
	// initial marking. This is done after parsing is complete.
	}
| 
	KEY_FINALCONDITION SEMICOLON 
	{
	  PN->FinalCondition = NULL;
	}
;

multiplefinalmarkinglists:
	{
		/* [LUHME XV] WTF? member "finalMarkingString" durch Methode zum Erzeugen ersetzen*/
		PN->finalMarkingString += "[";

		finalMarking = new unsigned int [PlaceTable->getSize()];

		// initialize finalMarking, places not occurring in the finalMarking are set to zero    
		for (unsigned int i = 0; i < PlaceTable->getSize(); i++) {
			finalMarking[i] = 0;
		}
	}
	finalmarkinglist 
	{
		PN->finalMarkingString += "]";

		PN->FinalMarkingList.push_back(finalMarking);
	}
|
	multiplefinalmarkinglists SEMICOLON     
	{
		PN->finalMarkingString += ", [";

		finalMarking = new unsigned int [PlaceTable->getSize()];

		// initialize finalMarking, places not occurring in the finalMarking are set to zero    
		for (unsigned int i = 0; i < PlaceTable->getSize(); i++) {
			finalMarking[i] = 0;
		}
	}
	finalmarkinglist
	{
		PN->finalMarkingString += "]";

		PN->FinalMarkingList.push_back(finalMarking);
	}
;

place_area: place_area_internal place_area_input  place_area_output
	  | place_area_lola
;

place_area_input:
  KEY_INPUT {type = INPUT; } placelists SEMICOLON
| /* empty */
;

place_area_output:
  KEY_OUTPUT {type = OUTPUT; } placelists SEMICOLON
| /* empty */
;

place_area_internal:
  KEY_INTERNAL {type = INTERNAL; } placelists SEMICOLON
| /* empty */
;

place_area_lola: {type = INTERNAL; } placelists SEMICOLON;

placelists: capacity placelist 
| placelists SEMICOLON capacity placelist
;

capacity: { CurrentCapacity = CAPACITY; }
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
	P->nrbits = (CurrentCapacity > 0 && logzwo(CurrentCapacity) < 32) ? logzwo(CurrentCapacity) + 1 : 32;
	// set max_occurrence to default value
	P->max_occurrence = events_manual;

        // For receiving events increase the max occurence by one in order to reach the empty node
        if(type == OUTPUT && events_manual != -1) {
            P->max_occurrence = P->max_occurrence + 1;
        }

	free($1);
	if (type == INPUT || type == OUTPUT) {
		numberOfEvents += events_manual;
	}
	
	/* annotate the place with the current port name if specified */
	if (current_port != "" && type != INTERNAL)
	  PN->add_place_to_port(PS->getPlace(), current_port);
	
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
		// set max_occurrence to value that was given in oWFN file
		if (options[O_READ_EVENTS]) {
			sscanf($3, "%u", &(PS->getPlace()->max_occurrence));
                        
                        // For receiving events increase the max occurence by one in order to reach the empty node
                        if(type == OUTPUT) {
                            PS->getPlace()->max_occurrence = PS->getPlace()->max_occurrence + 1;
                        }
			free($3);
		}
//      options[O_EVENT_USE_MAX] = true;
//        if (options[O_EVENT_USE_MAX] &&
//            PS->getPlace()->max_occurrence > events_manual)
//        {
//            PS->getPlace()->max_occurrence = events_manual;
//        }
//        numberOfEvents += PS->getPlace()->max_occurrence - events_manual;
	}
| KEY_MAX_OCCURRENCES OP_EQ NEGATIVE_NUMBER commands
	{
		//set max_occurrence to value that was given in oWFN file
		if (options[O_READ_EVENTS]) {
			sscanf($3, "%d", &(PS->getPlace()->max_occurrence));
			free($3);
		}
//        if (options[O_EVENT_USE_MAX] &&
//            PS->getPlace()->max_occurrence > events_manual)
//        {
//            PS->getPlace()->max_occurrence = events_manual;
//        }
//        numberOfEvents += PS->getPlace()->max_occurrence - events_manual;
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
  nodeident
	{ current_port = std::string($1); }
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


/* [LUHME XV] optional "markinglist", "markinglist" nicht leer */
/* [LUHME XV] "markinglist" in "marked_places_list" umbenennen */
/* [LUHME XV] Groß-Klein-"_"-Schreibung konsolidieren */
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
	*(PS->getPlace()) += i; // add i tokens to the place
	free($1);
	free($3);
	  } 
| nodeident /* we assume 1 token, if no NUMBER is given*/
	  {
	PS = (PlSymbol *) PlaceTable->lookup($1);
	if(!PS) {
		string error = "Place " + string($1) + " does not exist!";
		owfn_yyerror(error.c_str());
	}
	*(PS->getPlace()) += 1; // add 1 token to the place
	free($1);
	  }
;

finalmarkinglist: 
| finalmarking
| finalmarkinglist COMMA 
	{
	PN->finalMarkingString = PN->finalMarkingString + ", ";
	}
	finalmarking
;

// [LUHME XV] syntax von initial marking und final marking kann gleichartig geparst werden
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
	// [LUHME XV] warum nicht auch hier += wie bei initial marking?
	finalMarking[PN->getPlaceIndex(PS->getPlace())] = i;
		if (i < 5) {
			
			for (unsigned int n = 0; n < i; n++) {
				
				if (n != 0) {
					
					PN->finalMarkingString = PN->finalMarkingString + ", ";
				}
				PN->finalMarkingString = PN->finalMarkingString + string($1);
			}
		} else {
			
			PN->finalMarkingString = PN->finalMarkingString + string($1) + ":" + string($3);
		}
	
	free($1);
	free($3);
	  }
| nodeident /* we assume 1 token, if no NUMBER is given*/
	  {
	PS = (PlSymbol *) PlaceTable->lookup($1);
	if(!PS) {
		string error = "Place " + string($1) + " does not exist!";
		owfn_yyerror(error.c_str());
	}
	finalMarking[PN->getPlaceIndex(PS->getPlace())] = 1;
		PN->finalMarkingString = PN->finalMarkingString + string($1);
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
		// [LUHME XV] kann wahrscheinlich gelöscht werden, wird hier nicht benutzt
		inTransitionParsePosition = IN_CONSUME;
	}
	arclist SEMICOLON KEY_PRODUCE
	{
		// [LUHME XV] kann wahrscheinlich gelöscht werden, wird hier nicht benutzt
		inTransitionParsePosition = IN_PRODUCE;
	}
	arclist SEMICOLON
	{
	unsigned int i;
	arc_list * current;

	/* Schleife ueber alle eingehenden Boegen */
	for(current = $6; current; current = current->next) {
		if (current->place->getPlace()->type == OUTPUT) {
			string msg = string("Transition '") + T->name + "' consumes from "
				"output place '" + current->place->getPlace()->name +
				"' which is not allowed.";
			owfn_yyerror(msg.c_str());
		}
		
        /* gibt es Bogen schon? */
		for(i = 0; i < T->getArrivingArcsCount();i++) {
			if(current->place->getPlace() == T->getArrivingArc(i)->pl) {
				/* Bogen existiert, nur Vielfachheit addieren */
				*(T->getArrivingArc(i)) += current->nu;
				break;
			}
		}

		// variable i is still needed, don't even think about touching it
		if(i >= T->getArrivingArcsCount()) {
			T->addArrivingArc( new Arc(T, current->place->getPlace(), true, current->nu) );
			current->place->getPlace()->references ++;
		}
	}


	/* Schleife ueber alle ausgehenden Boegen */
	for(current = $10; current; current = current->next) {
		if (current->place->getPlace()->type == INPUT) {
			string msg = string("Transition '") + T->name + "' produces to an "
				"input place '" + current->place->getPlace()->name +
				"' which is not allowed.";
			owfn_yyerror(msg.c_str());
		}

		/* gibt es Bogen schon? */
		for(i = 0; i < T->getLeavingArcsCount(); i++) {
			if(current->place->getPlace() == T->getLeavingArc(i)->pl) {
				/* Bogen existiert, nur Vielfachheit addieren */
				*(T->getLeavingArc(i)) += current->nu;
				break;
			}
		}

		// variable i is still needed, don't even think about touching it
		if(i >= T->getLeavingArcsCount()) {
			T->addLeavingArc( new Arc(T,current->place->getPlace(), false, current->nu) );
			current->place->getPlace()->references++;
		}
	}
}
opt_synchronize_label
{

	free($2);

	// delete arc_list because they are no longer used
	delete $6;
	delete $10;
}
;

opt_synchronize_label:
  /* empty */
| KEY_SYNCHRONIZE
;


tname:   IDENT 
| NUMBER 
;


arclist: /* empty */ { $$ = (arc_list *) 0;}
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
	$$->nu = i;     // set arc-weight
	free($1);
	free($3);
	  }
| nodeident /* if no arc weight is given, we assume "1" */
	  {
	PS = (PlSymbol *) PlaceTable -> lookup($1);
	if(!PS) {
		string error = "Place " + string($1) + " does not exist!";
		owfn_yyerror(error.c_str());
	}
	$$ = new arc_list;
	$$->place = PS;
	$$->next = (arc_list *)  0;
	$$->nu = 1;     // set arc-weight to 1
	free($1);
	  }
;

statepredicate: LPAR
    {
        PN->finalConditionString += "(";
    }
    statepredicate RPAR 
	{
	$$ = $3;
	PN->finalConditionString += ")";
}
| statepredicate OP_AND
    {
        PN->finalConditionString = PN->finalConditionString + " AND ";    
    }
    statepredicate 
	{
	// [LUHME XV] "conj" groß oder zwei Klassen "...formulaConj" und "...formulaDisj"
	$$ = new binarybooleanformula(conj,$1,$4);
}
| statepredicate OP_OR 
    {
		PN->finalConditionString = PN->finalConditionString + " OR ";  
    }
    statepredicate 
	{
		$$ = new binarybooleanformula(disj,$1,$4);
}
| OP_NOT 
	{
		PN->finalConditionString = PN->finalConditionString + "NOT ";      
	}
    statepredicate 
	{
	   $$ = new unarybooleanformula(neg,$3);
}
| statepredicate OP_AND KEY_ALL_OTHER_PLACES_EMPTY {
	// [LUHME XV] Entfalten der Formel verschieben,
	// [LUHME XV] KEY_ALL_OTHER_PLACES_EMPTY, etc. als atomare Formel repräsentieren
	//
	// Warning: code duplication! Keep the rules for
	// KEY_ALL_OTHER_PLACES_EMPTY, KEY_ALL_OTHER_INTERNAL_PLACES_EMPTY and
	// KEY_ALL_OTHER_EXTERNAL_PLACES_EMPTY in sync!
	//
	formula* lhs = $1;
	set<owfnPlace*> places_in_lhs;
	lhs->collectplaces(places_in_lhs);
	set<owfnPlace*> all_other_places;
	// [LUHME XV] std::set_difference() in "<algorithm>" verwenden?
	for (size_t iplace = 0; iplace != PN->getPlaceCount(); ++iplace)
	{
		owfnPlace* current_place = PN->getPlace(iplace);
		if (places_in_lhs.find(current_place) == places_in_lhs.end())
		{
			all_other_places.insert(current_place);
		}
	}

	if (all_other_places.empty())
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
			// [LUHME XV] Symbol "eq" in Großbuchstaben oder eigene Methode/Klasse
			rhs->sub[iplace] = new atomicformula(eq, *itplace, 0);
			rhs->sub[iplace]->parent = rhs;
			rhs->sub[iplace]->parentindex = iplace;
			++iplace;
		}
		
		$$ = new binarybooleanformula(conj, lhs, rhs);
	}
	PN->finalConditionString = PN->finalConditionString + " AND ALL_OTHER_PLACES_EMPTY";  
}
| statepredicate OP_AND KEY_ALL_OTHER_INTERNAL_PLACES_EMPTY {
	// [LUHME XV] Entfalten der Formel verschieben, s.o.
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
	PN->finalConditionString = PN->finalConditionString + " AND ALL_OTHER_INTERNAL_PLACES_EMPTY";  
}
| statepredicate OP_AND KEY_ALL_OTHER_EXTERNAL_PLACES_EMPTY {
	// [LUHME XV] Entfalten der Formel verschieben, s.o.
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
	PN->finalConditionString = PN->finalConditionString + " AND ALL_OTHER_EXTERNAL_PLACES_EMPTY";  
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
	PN->finalConditionString = PN->finalConditionString + string($1) + " = " + string($3);  
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
	PN->finalConditionString = PN->finalConditionString + string($1) + " != " + string($3);  
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
	PN->finalConditionString = PN->finalConditionString + string($1) + " < " + string($3);  
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
	PN->finalConditionString = PN->finalConditionString + string($1) + " > " + string($3);  
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
	PN->finalConditionString = PN->finalConditionString + string($1) + " >= " + string($3);  
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
	PN->finalConditionString = PN->finalConditionString + string($1) + " <= " + string($3);  
	free($1);
	free($3);
}
