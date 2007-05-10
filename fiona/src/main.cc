/*****************************************************************************
 * Copyright 2005, 2006 Peter Massuthe, Daniela Weinberg,                    *
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
 * \file    main.cc
 *
 * \brief   main
 *
 * \author  responsible: Daniela Weinberg <weinberg@informatik.hu-berlin.de>
 *
 * \note    This file is part of the tool Fiona and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 */

#include "owfn.h"
#include "main.h"
#include "state.h"
#include "IG.h"
#include "OG.h"
#include "Exchangeability.h"
#include "options.h" 
#include "debug.h"
#include "OGFromFile.h"
#include <list>

// #defines YY_FLEX_HAS_YYLEX_DESTROY if we can call yylex_destroy()
#include "lexer_owfn_wrap.h"

#ifdef LOG_NEW
#include "newlogger.h"
#endif

#include <libgen.h>

using namespace std;

extern int owfn_yydebug;
extern int owfn_yy_flex_debug;

extern FILE *owfn_yyin;
extern int owfn_yyerror();
extern int owfn_yyparse();

#ifdef YY_FLEX_HAS_YYLEX_DESTROY
extern int owfn_yylex_destroy();
#endif

extern int og_yydebug;
extern int og_yy_flex_debug;

extern FILE *og_yyin;
extern int og_yyerror();
extern int og_yyparse();

#ifdef YY_FLEX_HAS_YYLEX_DESTROY
extern int og_yylex_destroy();
#endif

extern unsigned int State::card;
// extern char * netfile;
extern list<char*> netfiles;
extern OGFromFile* OGToParse;

int garbagefound;
unsigned int NonEmptyHash;
unsigned int numberOfDecodes;
unsigned int numberDeletedVertices;

unsigned int numberOfEvents;

void myown_newhandler()
{
    cerr << "new failed, memory exhausted" << endl;
    exit(2);
}

void readnet() {
    owfn_yydebug = 0;
    owfn_yy_flex_debug = 0;
	// diagnosefilename = (char *) 0;
    if(netfile) {
        owfn_yyin = fopen(netfile,"r");
        if(!owfn_yyin) {
            cerr << "cannot open netfile: " << netfile << "\n\n";
            exit(4);
        }
		trace(TRACE_1, "--------------------------------------------------------------\n");
		trace(TRACE_1, "reading from file " + string(netfile) + "\n");
		//  diagnosefilename = netfile;
    }

    PN = new oWFN();

    owfn_yyparse();
    fclose(owfn_yyin);

    unsigned int ii;
    for(ii = 0; ii < PN->getPlaceCount();ii++) {
        PN->CurrentMarking[ii] = PN->getPlace(ii)->initial_marking;
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


OGFromFile* readog(const std::string& ogfile) {
    og_yydebug = 0;
    og_yy_flex_debug = 0;
    assert(ogfile != "");

    og_yyin = fopen(ogfile.c_str(), "r");
    if (!og_yyin) {
        cerr << "cannot open OG file '" << ogfile << "' for reading'" << endl;
        exit(4);
    }

    trace(TRACE_1, "--------------------------------------------------------------\n");
    trace(TRACE_1, "reading from file " + ogfile + "\n");

    OGToParse = new OGFromFile();
    ogfileToParse = ogfile;

    og_yyparse();
    fclose(og_yyin);
    return OGToParse;
}


//! \fn void adjustOptionValues()
//! \brief adjusts values for -e and -c options
void adjustOptionValues() {
	// report ...

	if (options[O_MESSAGES_MAX] == true) {
		trace(TRACE_0, "interface message bound set to: " + intToString(messages_manual) +"\n");
	}
	trace(TRACE_0, "considering the following events:\n");
//	trace(TRACE_0, "considering max. " + intToString(numberOfEvents) + " events at all:\n");
	trace(TRACE_0, "    sending events:\n" );
	for (unsigned int e=0; e < PN->getInputPlaceCount(); e++) {
		trace(TRACE_0, "\t !" + string(PN->getInputPlace(e)->name));
		if (options[O_EVENT_USE_MAX]) {
			trace(TRACE_0, "\t(max. " + intToString(PN->getInputPlace(e)->max_occurence) + "x)\n");
		} else {
			trace(TRACE_0, "\t(no limit)\n");
		}
	}
	trace(TRACE_0, "    receiving events:\n" );
	for (unsigned int e=0; e < PN->getOutputPlaceCount(); e++) {
		trace(TRACE_0, "\t ?" + string(PN->getOutputPlace(e)->name));
		if (options[O_EVENT_USE_MAX]) {
			trace(TRACE_0, "\t(max. " + intToString(PN->getOutputPlace(e)->max_occurence) + "x)\n");
		} else {
			trace(TRACE_0, "\t(no limit)\n");
		}
	}
	
	trace(TRACE_0, "\n");
}


// **********************************************************************************
// ********                   MAIN                                           ********
// **********************************************************************************

int main(int argc, char ** argv) {
	
//	bool readExpliciteOG = false;
//	parse_command_line(argc, argv);
//	if (readExpliciteOG){
//		readog(ogfileToMatch);
//		trace(TRACE_0, "HIT A KEY TO CONTINUE"); getchar();
//		NewLogger::printall();
//	}
//	else{
//		list<char*>::iterator netiter = netfiles.begin();
//		Exchangeability* bdd = new Exchangeability(*netiter);
//		bdd->reorder();
//		trace(TRACE_0, "HIT A KEY TO CONTINUE"); getchar();
//		bdd->printMemoryInUse();
//	}
//	return 0;
	

	set_new_handler(&myown_newhandler);

	list<oWFN*> petrinets;

	// evaluate command line options
	parse_command_line(argc, argv);

// ---------------- exchangeability ---------------------

    if (options[O_EX] == true) {
        //check equality of two operating guidelines
        if (netfiles.size() == 2) {
            list<char*>::iterator netiter = netfiles.begin();
            Exchangeability* og1 = new Exchangeability(*netiter);
            Exchangeability* og2 = new Exchangeability(*(++netiter));
            trace(TRACE_0, "The two operating guidelines are equal: ");
            if (og1->check(og2) == true) {
                trace(TRACE_0, "YES\n");
            } else {
                trace(TRACE_0, "NO\n");
            }
        } else {
            cerr << "Error: \t If option -x is used, exactly two oWFN must be entered\n" << endl;
        }
        
        return 0;
    }
    
// ------------------- reading OG ------------------------

    OGFromFile* OGToMatch = NULL;
    if (options[O_MATCH]) {
        OGToMatch = readog(ogfileToMatch);
    }

// ----------- enforcing a constraint automaton ----------

	if (options[O_CONSTRAINT]) {

		netfile = *netfiles.begin();
	
		// open the OG to be constrained
		string ogfile = string(netfile) + ".a.og";
		trace(TRACE_0, "You want to constrain the OG:  " + ogfile + "\n");

		OGFromFile* og = readog(ogfile);

		// open the constraint automaton (same syntax as OG)
		trace(TRACE_0, "   to respect the constraint:  " + constraintfile +
		    "\n\n"); 

		OGFromFile* constraint = readog(constraintfile);

		OGFromFile* constrainedOG = og->enforce(constraint);

		trace(TRACE_0, " computed the constrained OG:  " +
		    string(netfile) + ".a.og.under." + platform_basename(constraintfile) +
		    ".out\n\n");


		constrainedOG->removeFalseNodes();
		constrainedOG->printDotFile();

#ifdef YY_FLEX_HAS_YYLEX_DESTROY
		// Destroy buffer of OG parser.
		//  Must NOT be called before fclose(og_yyin);
		og_yylex_destroy();
#endif

		delete constrainedOG;
		delete constraint;
		delete og;
		return 0;
	}

// ---------------- reading all nets ---------------------
	if (options[O_OWFN_NAME] ) {
		list<char*>::iterator netiter = netfiles.begin();
		
		do {
			numberOfDecodes = 0;
			
			garbagefound = 0;
			State::card = 0;          // number of states
			
			numberDeletedVertices = 0;
			
			numberOfEvents = 0;
			
			// prepare getting the net
			try {
				PlaceTable = new SymbolTab<PlSymbol>;
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
			
			netiter++;
		} while (netfiles.begin() != netfiles.end() && netiter != netfiles.end());
	}
    
#ifdef YY_FLEX_HAS_YYLEX_DESTROY
    // Delete lexer buffer for parsing oWFNs.
    // Must NOT be called before fclose(owfn_yyin);
    owfn_yylex_destroy();
#endif
	
// ------------- reading all OG-files -----------------------
    OGFromFile::ogs_t OGsFromFiles;
	for (OGFromFile::ogfiles_t::const_iterator iOgFile = ogfiles.begin();
         iOgFile != ogfiles.end(); ++iOgFile)
    {
        OGsFromFiles.push_back(readog(*iOgFile));
    }
	
#ifdef YY_FLEX_HAS_YYLEX_DESTROY
		// Destroy buffer of OG parser.
		//  Must NOT be called before fclose(og_yyin);
		og_yylex_destroy();
#endif

// -------------- calculating the product og -------------
    if (options[O_PRODUCTOG])
    {
        if (OGsFromFiles.size() < 2)
        {
			cerr << "Error: \t Give at least two OGs to build their product!\n" << endl;
            exit(1);
        }
        OGFromFile* productOG = OGFromFile::product(OGsFromFiles);

        if (!options[O_OUTFILEPREFIX])
        {
            outfilePrefix = OGFromFile::getProductOGFilePrefix(ogfiles);
        }

        productOG->printDotFile(outfilePrefix);
        delete productOG;

        for (OGFromFile::ogs_t::const_iterator iOg = OGsFromFiles.begin();
             iOg != OGsFromFiles.end(); ++iOg)
        {
            delete *iOg;
        }

        return 0;
    }

// ------------- simulation on OGFromFile --------------------
	
	if ( options[O_SIMULATES] ) {
		if ( OGsFromFiles.size() == 2 ) {
			list<OGFromFile*>::iterator OGFFIter = OGsFromFiles.begin();
			OGFromFile *simulator = *OGFFIter;
			OGFromFile *simulant = *(++OGFFIter);
			if ( simulator->simulates(simulant) )
				trace(TRACE_0, "\nThe first OG simulates the second one.\n" );
			else
				trace(TRACE_0, "\nThe first OG doesn't simulate the second one.\n");
		}
		else
			cerr << "Error: \t If option -S is used, exactly two OG files must be entered\n" << endl;
	}

// **************** start doing things :) *********************

    for (list<oWFN*>::iterator net = petrinets.begin();
         net != petrinets.end(); net++) {
        PN = *net;
        netfile = PN->filename; 

        numberOfDecodes = 0;
        garbagefound = 0;
        State::card = 0;          // number of states
        numberDeletedVertices = 0;

        trace(TRACE_0, "\n--------------------------------------------------------------\n");
        if (netfile) {
            trace(TRACE_0, "processing net " + string(netfile) + " ...\n");
        }	
        // report the net
        trace(TRACE_0, "    places: " + intToString(PN->getPlaceCount()));
        trace(TRACE_0, " (including " + intToString(PN->getInputPlaceCount()) + " input places, " + intToString(PN->getOutputPlaceCount()) + " output places)\n");
        trace(TRACE_0, "    transitions: " + intToString(PN->getTransitionCount()) + "\n\n");
    
        if (PN->FinalCondition) {
            trace(TRACE_0, "finalcondition used\n\n");
		} else {
			if (PN->FinalMarking) {
				trace(TRACE_0, "finalmarking used\n\n");
			} else {
				trace(TRACE_0, "neither finalcondition nor finalmarking given\n");
			}
		}

		// adjust events_manual and print limit of considering events
		adjustOptionValues();
        
        // start computation
        time_t seconds, seconds2;
        
		if (options[O_MATCH]) {
			// ------------------------- matching --------------------------------

			string reasonForFailedMatch;
			if (PN->matchesWithOG(OGToMatch, reasonForFailedMatch)) {
				trace(TRACE_0, "oWFN matches with OG: YES\n");
			} else {
				trace(TRACE_0, "oWFN matches with OG: NO\n");
				trace(TRACE_0, "Match failed, because: " +
				reasonForFailedMatch + "\n");
			}
		} else if (parameters[P_OG]) {
			// ---------------- operating guideline is built ---------------------

            operatingGuidelines * graph = new operatingGuidelines(PN);
            trace(TRACE_0, "building the operating guideline...\n");
            seconds = time (NULL);
    
            graph->printProgressFirst();
            graph->calculateRootNode();	// creates the root node and calculates its reachability graph (set of states)
            
            if (options[O_OTF]){
                graph->bdd->convertRootNode(graph->getRoot());
            }
            
            graph->buildGraph(graph->getRoot(), 1); // build operating guideline
            
            seconds2 = time (NULL);
            trace(TRACE_0, "\nbuilding the operating guideline finished.\n\n");
            cout << difftime(seconds2,seconds) << " s consumed for building graph" << endl;
    
            trace(TRACE_0, "\nnet is controllable: ");
            if (graph->getRoot()->getColor() == BLUE) {
                trace(TRACE_0, "YES\n\n");
            } else {
                trace(TRACE_0, "NO\n\n");
            }
            
            trace(TRACE_0, "number of states calculated: " + intToString(State::card) + "\n");
            trace(TRACE_0, "OG: number of nodes: " + intToString(graph->getNumberOfNodes()) + "\n");
            trace(TRACE_0, "    number of edges: " + intToString(graph->getNumberOfEdges()) + "\n");
            trace(TRACE_0, "    (numberDeletedVertices: " + intToString(numberDeletedVertices) + ")\n");
                
            trace(TRACE_5, "printNodeStatistics...\n");
            graph->printNodeStatistics();
            trace(TRACE_5, "printDotFile...\n");
            graph->printDotFile();
            trace(TRACE_5, "printOGFile...\n");
            graph->printOGFile();
            
            if (options[O_OTF]) {
                //graph->bdd->printDotFile();
                graph->bdd->save("OTF");
            }
            
            if (options[O_BDD]) {
                trace(TRACE_0, "\nbuilding the BDDs...\n");
                seconds = time (NULL);
                graph->convertToBdd();      
                seconds2 = time (NULL);
                cout << difftime(seconds2,seconds) << " s consumed for building and reordering the BDDs" << endl;
                 
                //graph->bdd->printDotFile();
                graph->bdd->save();
            }
    
            trace(TRACE_5, "computation finished -- trying to delete graph\n");
            // trace(TRACE_0, "HIT A KEY TO CONTINUE"); getchar();
            delete graph;
            trace(TRACE_5, "graph deleted\n");

        } else {
			// ---------------- interaction graph is built ---------------------
            
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
                trace(TRACE_0, "\nbuilding the interaction graph finished.\n");
            }

            cout << difftime(seconds2,seconds) << " s consumed for building graph" << endl;
    
            trace(TRACE_0, "\nnet is controllable: ");
            if (graph->getRoot()->getColor() == BLUE) {
                trace(TRACE_0, "YES\n\n");
            } else {
                trace(TRACE_0, "NO\n\n");
            }
            trace(TRACE_0, "number of states calculated: " + intToString(State::card) + "\n");
            trace(TRACE_0, "IG: number of nodes: " + intToString(graph->getNumberOfNodes()) + "\n");
            trace(TRACE_0, "    number of edges: " + intToString(graph->getNumberOfEdges()) + "\n");
            trace(TRACE_0, "    (numberDeletedVertices: " + intToString(numberDeletedVertices) + ")\n");

            graph->printNodeStatistics();
            graph->printDotFile();				// for IG

            trace(TRACE_5, "computation finished -- trying to delete graph\n");
//				trace(TRACE_0, "HIT A KEY TO CONTINUE"); getchar();
            delete graph;
            trace(TRACE_5, "graph deleted\n");
        }

        delete PN;
    
//		cout << "numberOfDecodes: " << numberOfDecodes << endl;
    
    } // end of "for all nets ..."

    if (options[O_MATCH]) {
        delete OGToMatch;
    }

#ifdef LOG_NEW
    NewLogger::printall();
#endif
    
	return 0;
}


std::string platform_basename(const std::string& path)
{
#ifdef WIN32
    string::size_type posOfLastBackslash = path.find_last_of('\\');
    return path.substr(posOfLastBackslash + 1);
#else
    char* ppath = (char*)malloc(path.size() + sizeof(char));
    strcpy(ppath, path.c_str());
    std::string result = string(::basename(ppath));
    free(ppath);
    return result;
#endif
}
