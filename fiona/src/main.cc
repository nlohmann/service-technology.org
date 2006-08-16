#include "mynew.h"
#include "dimensions.h"
#include "symboltab.h"
#include "state.h"
#include "IG.h"
#include "OG.h"
#include "owfn.h"
#include "owfnPlace.h"

#include "options.h"
#include "debug.h"
#include "lexer_wrap.h"


#include <cstdlib> 
#include <time.h>

#include "main.h"

#include<fstream>
#include<iostream>
#include<ctype.h>
#include<stdlib.h>
#include<stdio.h>
#include<unistd.h>
#include<new>

#ifdef LOG_NEW
#include "newlogger.h"
#endif

using namespace std;


extern int yydebug;
extern int yy_flex_debug;

extern FILE *yyin;
extern int yyerror();
extern int yyparse();

#ifdef YY_FLEX_HAS_YYLEX_DESTROY     
extern int yylex_destroy();
#endif

extern SymbolTab* GlobalTable;

//extern char * lownetfile;

char * lownetfile;
char * pnmlfile;

char * analysefile;
char * graphfile;
char * pathfile;
char * statefile;
char * symmfile;
char * netbasename;

bool hflg, Nflg, nflg, Aflg, Sflg, Yflg, Pflg,GMflg, aflg, sflg,
        yflg,pflg,gmflg, cflg;

char graphformat;

int garbagefound;
char * reserve;

unsigned int NonEmptyHash;


unsigned int numberDeletedVertices;


extern unsigned int State::card;

unsigned int numberOfDecodes;

inline void garbagecollection() {
}

void myown_newhandler() {
    delete reserve;
        cerr << "new failed\n";
    throw bad_alloc();
}

char * diagnosefilename;

extern char * netfile;

void readnet() {
    yydebug = 0;
    yy_flex_debug = 0;
    diagnosefilename = (char *) 0;
    if(netfile)
    {
        yyin = fopen(netfile,"r");
        if(!yyin)
        {
            cerr << "cannot open netfile: " << netfile << "\n";
            exit(4);
        }
        diagnosefilename = netfile;
    }
    GlobalTable = new SymbolTab(1024);

    PN = new oWFN();

    yyparse();
    fclose(yyin);
    
#ifdef YY_FLEX_HAS_YYLEX_DESTROY     
    yylex_destroy(); // must NOT be called before fclose(yyin);
#endif

    unsigned int ii;
    for(ii = 0; ii < PN->getPlaceCnt();ii++)
    {
        PN->CurrentMarking[ii] = PN->Places[ii]->initial_marking;
        PN->Places[ii]->index = ii;
   }

    PN->initialize();
}


int yywrap() {
    return 1;
}


// ----------------------------------------------------------------------------------
// MAIN
// ----------------------------------------------------------------------------------

int main(int argc, char ** argv) {
  unsigned int i, h;

	numberOfDecodes = 0;

  garbagefound = 0;
  State::card = 0;          // number of states

	numberDeletedVertices = 0;

  // 0. eigenen New-Handler installieren
  try {
    set_new_handler(&myown_newhandler);
    garbagefound = 0;

	// evaluate command line options
	parse_command_line(argc, argv);

    trace(TRACE_0, "\n--------------------------------------------------------------\n");

    // prepare getting the net
    try {
        PlaceTable = new SymbolTab(65536);
        TransitionTable = new SymbolTab(65536);
    } catch(bad_alloc) {
        char mess[] = "\nnot enough space to read net\n";
        //write(2,mess,sizeof(mess));
        cerr << mess;
        _exit(2);
    }

    // get the net
    try {
        readnet();							// Parser

        // PN->removeisolated();
		// TODO: better removal of places 
		// doesn't work with, since array for input and output places
		// depend on the order of the Places array, reordering results in
		// a heavy crash

    } catch(bad_alloc) {
        char mess [] = "\nnot enough space to store net\n";
        //write(2,mess,sizeof(mess));
        cerr << mess;
        _exit(2);
    }
        
    delete PlaceTable;
    delete TransitionTable;

    // report the net
    trace(TRACE_0, "places: " + intToString(PN->getPlaceCnt()));
    trace(TRACE_0, " (including " + intToString(PN->getInputPlaceCnt()) + " input places, " + intToString(PN->getOutputPlaceCnt()) + " output places)\n");
    trace(TRACE_0, "transitions: " + intToString(PN->getTransitionCnt()) + "\n\n");
	
	if (parameters[P_OG]) {
	    // adjust commDepth and events_manual
	    if (options[O_COMM_DEPTH] == true) {
	        PN->commDepth = commDepth_manual;
	    }
	
	    if (options[O_EVENT_USE_MAX] = true) {
			if (PN->getCommDepth() > events_manual * (PN->placeInputCnt + PN->placeOutputCnt)) {
				trace(TRACE_0, "commDepth is set too high\n");
				PN->commDepth = events_manual * (PN->placeInputCnt + PN->placeOutputCnt);
			}
	    }
	    
	    if (options[O_EVENT_USE_MAX] = true) {
			if (PN->getCommDepth() < events_manual) {
				trace(TRACE_0, "number of events to be used is set too high\n");
				events_manual = PN->commDepth;
			}
	    }
	    
		// report communication depth and events use    
	    trace(TRACE_0, "communication depth: " + intToString(PN->getCommDepth()) + "\n");
	    if (options[O_EVENT_USE_MAX] == true) {
	        trace(TRACE_0, "considering each event max. " + intToString(events_manual) + " times\n\n");
	    }
	}

	// ------------------- start computation -------------------------
    time_t seconds, seconds2;

    if (parameters[P_OG]) {
        // operating guideline is built
        operatingGuidelines * graph = new operatingGuidelines(PN);
        trace(TRACE_0, "building the operating guideline...\n");
        seconds = time (NULL);
        graph->buildGraph();                    // build operating guideline
        seconds2 = time (NULL);
        trace(TRACE_0, "building the operating guideline finished.\n");

        cout << difftime(seconds2,seconds) << " s consumed for building graph" << endl;

        trace(TRACE_0, "\nnet is controllable: ");
        if (graph->getRoot()->getColor() == BLUE) {
            trace(TRACE_0, "YES\n\n");
        } else {
            trace(TRACE_0, "NO\n\n");
        }
        trace(TRACE_0, "number of states calculated: " + intToString(State::card) + "\n");
        trace(TRACE_0, "OG: number of nodes: " + intToString(graph->getNumberOfVertices()) + "\n");
        trace(TRACE_0, "    number of edges: " + intToString(graph->getNumberOfEdges()) + "\n");
		trace(TRACE_0, "    (numberDeletedVertices: " + intToString(numberDeletedVertices) + ")\n");
                
        graph->printDotFile();
        
        if (options[O_BDD] == true){
        	trace(TRACE_0, "\nbuilding the BDDs...\n");
        	seconds = time (NULL);
        	graph->convertToBdd();      
        	seconds2 = time (NULL);
        	cout << difftime(seconds2,seconds) << " s consumed for building and reordering the BDDs" << endl;
        	 
        	//graph->bdd->printDotFile();
        	//graph->bdd->print();
        }
        
//		cout << "\nOG computation finished\n\t\t\t...please hit any key" << endl;
//		getchar();
	
	    delete graph;
        
    } else {
        // interaction graph is built
        interactionGraph * graph = new interactionGraph(PN);

        if (parameters[P_CALC_REDUCED_IG]) {
            trace(TRACE_0, "building the reduced interaction graph...\n");
        } else {
            trace(TRACE_0, "building the interaction graph...\n");
        }
        seconds = time (NULL);
        graph->buildGraph();                    // build interaction graph
        seconds2 = time (NULL);
        if (parameters[P_CALC_REDUCED_IG]) {
            trace(TRACE_0, "building the reduced interaction graph finished.\n");
        } else {
            trace(TRACE_0, "building the interaction graph finished.\n");
        }

        cout << difftime(seconds2,seconds) << " s consumed for building graph" << endl;

        trace(TRACE_0, "\nnet is controllable: ");
        if (graph->getRoot()->getColor() == BLUE) {
            trace(TRACE_0, "YES\n\n");
        } else {
            trace(TRACE_0, "NO\n\n");
        }
        trace(TRACE_0, "number of states calculated: " + intToString(State::card) + "\n");
        trace(TRACE_0, "IG: number of nodes: " + intToString(graph->getNumberOfVertices()) + "\n");
        trace(TRACE_0, "    number of edges: " + intToString(graph->getNumberOfEdges()) + "\n");
		trace(TRACE_0, "    (numberDeletedVertices: " + intToString(numberDeletedVertices) + ")\n");

        graph->printDotFile();				// for IG

//		cout << "\nIG computation finished\n\t\t\t...please hit any key" << endl;
//		getchar();
	
	    delete graph;
    }

	delete PN;
    delete GlobalTable;	
	
//	cout << "numberOfDecodes: " << numberOfDecodes << endl;
	
    trace(TRACE_0, "--------------------------------------------------------------\n\n");

}
    catch(bad_alloc) {
        char mess [] = "memory exhausted\n";
        cerr << mess;
        _exit(2);
    }

#ifdef LOG_NEW
    NewLogger::printall();
#endif
    
    return 0;
}
