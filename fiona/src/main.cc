#include "state.h"
#include "IG.h"
#include "OG.h"
#include "owfn.h"
#include "options.h"
#include "main.h"
#include <list>

// #defines YY_FLEX_HAS_YYLEX_DESTROY if we can call yylex_destroy()
#include "lexer_wrap.h"

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
extern unsigned int State::card;
// extern char * netfile;
extern list<char*> netfiles;

char * reserve;
//char * diagnosefilename;

int garbagefound;
unsigned int NonEmptyHash;
unsigned int numberOfDecodes;
unsigned int numberDeletedVertices;

unsigned int numberOfEvents;

inline void garbagecollection() {
}

void myown_newhandler() {
    delete reserve;
        cerr << "new failed\n";
    throw bad_alloc();
}

void readnet() {
    yydebug = 0;
    yy_flex_debug = 0;
	// diagnosefilename = (char *) 0;
    if(netfile) {
        yyin = fopen(netfile,"r");
        if(!yyin) {
            cerr << "cannot open netfile: " << netfile << "\n\n";
            exit(4);
        }
		trace(TRACE_1, "--------------------------------------------------------------\n");
		trace(TRACE_1, "reading from file " + string(netfile) + "\n");
		//  diagnosefilename = netfile;
    }

    PN = new oWFN();

    yyparse();
    fclose(yyin);
    
#ifdef YY_FLEX_HAS_YYLEX_DESTROY     
    yylex_destroy(); // must NOT be called before fclose(yyin);
#endif

    unsigned int ii;
    for(ii = 0; ii < PN->getPlaceCnt();ii++) {
        PN->CurrentMarking[ii] = PN->Places[ii]->initial_marking;
        PN->Places[ii]->index = ii;
	}

    PN->initialize();

    // Initialize final condition (if present) with initial marking.
    // Only afterwards is merging allowed, because merge requires subformulas
    // to have sensible values.
    if (PN->FinalCondition) {
        PN->FinalCondition->init(PN->CurrentMarking);
        
        formula* oldFinalCondition = PN->FinalCondition;
        PN->FinalCondition = PN->FinalCondition->merge();
        delete oldFinalCondition;

        PN->FinalCondition = PN->FinalCondition->posate();
        PN->FinalCondition->setstatic();
    }
}


int yywrap() {
    return 1;
}

//! \fn void adjustOptionValues()
//! \brief adjusts values for -e and -c options
void adjustOptionValues() {
	if (options[O_COMM_DEPTH] == true) {
		// adjusting commDepth if dominated by events
		if (commDepth_manual > numberOfEvents) {
			trace(TRACE_1, "manual commDepth is set too high ... adjusting it\n");
			PN->commDepth = numberOfEvents;
		} else {
			PN->commDepth = commDepth_manual;
		}

		// adjusting events if dominated by commDepth
		if (options[O_EVENT_USE_MAX] == true) {
			if (PN->getCommDepth() < events_manual) {
			    trace(TRACE_1, "number of events to be used is set too high\n");
			    events_manual = PN->commDepth;
			}
		}
	} else {
		// compute commDepth if not specified by -c option
		trace(TRACE_1, "standard commDepth too high ... adjusting it\n");
		if (PN->commDepth > numberOfEvents) {
			PN->commDepth = numberOfEvents;
		}
	}
	// report ...
	trace(TRACE_0, "communication depth: " + intToString(PN->getCommDepth()) + "\n");
	if (options[O_MESSAGES_MAX] == true) {
		trace(TRACE_0, "interface message bound set to: " + intToString(messages_manual) +"\n");
	}
	trace(TRACE_0, "considering max. " + intToString(numberOfEvents) + " events at all\n\n");
	
	options[O_EVENT_USE_MAX] = true;	
}



// **********************************************************************************
// ********                   MAIN                                           ********
// **********************************************************************************

int main(int argc, char ** argv) {

	unsigned int i, h;

	list<oWFN*> petrinets;

	// 0. eigenen New-Handler installieren
	try {
	    set_new_handler(&myown_newhandler);
	    // evaluate command line options
	    parse_command_line(argc, argv);
	
	    list<char*>::iterator netiter = netfiles.begin();

		do {
			numberOfDecodes = 0;
	
			garbagefound = 0;
			State::card = 0;          // number of states
	
			numberDeletedVertices = 0;

			numberOfEvents = 0;
	
			// prepare getting the net
			try {
				PlaceTable = new SymbolTab(65536);
				TransitionTable = new SymbolTab(65536);
			}
			catch(bad_alloc) {
		        char mess[] = "\nnot enough space to read net\n";
		        //write(2,mess,sizeof(mess));
		        cerr << mess;
		        _exit(2);
			}
	
			// get the net
			try {
				if (netiter != netfiles.end()) {
		  			netfile = *netiter;
				}
				readnet();  // Parser;
	
		        // PN->removeisolated();
				// TODO: better removal of places 
				// doesn't work with, since array for input and output places
				// depend on the order of the Places array, reordering results in
				// a heavy crash
	
			} 
			catch(bad_alloc) {
				char mess [] = "\nnot enough space to store net\n";
				//write(2,mess,sizeof(mess));
				cerr << mess;
				_exit(2);
			}
	        
			PN->filename = netfile;
			petrinets.push_back(PN);
			
			delete PlaceTable;
			delete TransitionTable;
			
			netiter++;
		} while (netfiles.begin() != netfiles.end() && netiter != netfiles.end());

		for (list<oWFN*>::iterator net = petrinets.begin();
			 net != petrinets.end(); net++) {
			PN = *net;
			netfile = PN->filename; 
	
			GlobalTable = new SymbolTab(1024);
	
			numberOfDecodes = 0;
			garbagefound = 0;
			State::card = 0;          // number of states
			numberDeletedVertices = 0;
	
			trace(TRACE_0, "\n--------------------------------------------------------------\n");
			if (netfile) {
				trace(TRACE_0, "processing net " + string(netfile) + " ...\n");
			}	
			// report the net
			trace(TRACE_0, "    places: " + intToString(PN->getPlaceCnt()));
			trace(TRACE_0, " (including " + intToString(PN->getInputPlaceCnt()) + " input places, " + intToString(PN->getOutputPlaceCnt()) + " output places)\n");
			trace(TRACE_0, "    transitions: " + intToString(PN->getTransitionCnt()) + "\n\n");
		
			if (PN->FinalCondition) {
				trace(TRACE_0, "finalcondition used\n\n");
			} else {
				if (PN->FinalMarking) {
					trace(TRACE_0, "finalmarking used\n\n");
				} else {
					trace(TRACE_0, "neither finalcondition nor finalmarking given\n");
				}
			}

			// adjust commDepth and events_manual
			adjustOptionValues();
	
			// ------------------- start computation -------------------------
			time_t seconds, seconds2;
	
			if (parameters[P_OG]) {
	
		        // operating guideline is built
		        operatingGuidelines * graph = new operatingGuidelines(PN);
		        trace(TRACE_0, "building the operating guideline...\n");
		        seconds = time (NULL);
		
		        graph->calculateRootNode();	// creates the root node and calculates its reachability graph (set of states)
				graph->buildGraph(graph->getRoot()); // build operating guideline
				
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
		        
		        if (options[O_BDD] == true) {
					trace(TRACE_0, "\nbuilding the BDDs...\n");
					seconds = time (NULL);
					graph->convertToBdd();      
					seconds2 = time (NULL);
					cout << difftime(seconds2,seconds) << " s consumed for building and reordering the BDDs" << endl;
		        	 
					//graph->bdd->printDotFile();
					//graph->bdd->print();
					graph->bdd->save();
		        }
		
				trace(TRACE_5, "computation finished -- trying to delete graph\n");
//				getchar();
				delete graph;
				trace(TRACE_5, "graph deleted\n");
	        
			} else {
		        
		        // interaction graph is built
		        interactionGraph * graph = new interactionGraph(PN);
		
		        if (options[O_CALC_REDUCED_IG]) {
					trace(TRACE_0, "building the reduced interaction graph...\n");
				} else {
					trace(TRACE_0, "building the interaction graph...\n");
				}
		        seconds = time (NULL);
		        graph->buildGraph();                    // build interaction graph
		        seconds2 = time (NULL);
		        if (options[O_CALC_REDUCED_IG]) {
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
	
				trace(TRACE_5, "computation finished -- trying to delete graph\n");
//				getchar();
				delete graph;
				trace(TRACE_5, "graph deleted\n");
			}
	
			delete PN;
			delete GlobalTable;	
		
//			cout << "numberOfDecodes: " << numberOfDecodes << endl;
		
			trace(TRACE_0, "--------------------------------------------------------------\n\n");
		} // end of "for all nets ..."
	} // end try
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
