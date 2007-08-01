/*****************************************************************************
 * Copyright 2005, 2006 Peter Massuthe, Daniela Weinberg,                    *
 *                      Jan Bretschneider, Christian Gierds, Leonard Kern    *
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

extern unsigned int State::state_count;
// extern char * netfile;
extern list<char*> netfiles;
extern OGFromFile* OGToParse;

int garbagefound;
unsigned int NonEmptyHash;
unsigned int numberOfDecodes;
unsigned int numberDeletedVertices;

unsigned int numberOfEvents;

void myown_newhandler() {
    cerr << "new failed, memory exhausted" << endl;
    exit(2);
}


//! \brief reads an oWFN from netfile
void readnet() {
    owfn_yydebug = 0;
    owfn_yy_flex_debug = 0;
	// diagnosefilename = (char *) 0;
    if (netfile) {
        owfn_yyin = fopen(netfile,"r");
        if(!owfn_yyin) {
            cerr << "cannot open netfile: " << netfile << "\n\n";
            exit(4);
        }
		trace(TRACE_5, "reading from file " + string(netfile) + "\n");
		//  diagnosefilename = netfile;
    }

    PN = new oWFN();

    owfn_yyparse();
    fclose(owfn_yyin);

    for(unsigned int i = 0; i < PN->getPlaceCount(); i++) {
        PN->CurrentMarking[i] = PN->getPlace(i)->initial_marking;
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

    // PN->removeisolated();
    // TODO: better removal of places 
    // doesn't work with, since array for input and output places
    // depend on the order of the Places array, reordering results in
    // a heavy crash
}


//! \brief reads an OG from ogfile
OGFromFile* readog(const std::string& ogfile) {
    og_yydebug = 0;
    og_yy_flex_debug = 0;
    assert(ogfile != "");

    og_yyin = fopen(ogfile.c_str(), "r");
    if (!og_yyin) {
        cerr << "cannot open OG file '" << ogfile << "' for reading'" << endl;
        exit(4);
    }

    trace(TRACE_1, "=================================================================\n");
    trace(TRACE_1, "reading from file " + ogfile + "\n");

    OGToParse = new OGFromFile();
    ogfileToParse = ogfile;

    og_yyparse();
    fclose(og_yyin);
    return OGToParse;
}


//! \brief reads all OGs from a list
void readAllOGs(OGFromFile::ogs_t& theOGs) {
    for (OGFromFile::ogfiles_t::const_iterator iOgFile = ogfiles.begin();
         iOgFile != ogfiles.end(); ++iOgFile) {

        theOGs.push_back(readog(*iOgFile));
    }
}


//! \brief reports values for -e and -m option
void reportOptionValues() {

    trace(TRACE_5, "-e option found: ");
    if (options[O_EVENT_USE_MAX]) {
        trace(TRACE_5, "yes\n");
    } else {
        trace(TRACE_5, "no\n");
    }

    // if options[O_EVENT_USE_MAX] (with non-negative value) is set, then
    // values for max_occurence are totally ignored (overwritten)
    // otherwise, take values of max_occurence
    if (options[O_EVENT_USE_MAX]) {
        assert(events_manual >= 0);
        for (unsigned int e = 0; e < PN->getInputPlaceCount(); e++) {
            PN->getInputPlace(e)->max_occurence = events_manual;
        }
        for (unsigned int e = 0; e < PN->getOutputPlaceCount(); e++) {
            PN->getOutputPlace(e)->max_occurence = events_manual;
        }
    }

	// report events
    if (debug_level == TRACE_0) {
        if (options[O_EVENT_USE_MAX]) {
            trace(TRACE_0, "each event considered max: " + intToString(events_manual) +"\n");
        }
    } else {
        trace(TRACE_0, "considering the following events:\n");
        trace(TRACE_0, "    sending events:\n" );
        for (unsigned int e = 0; e < PN->getInputPlaceCount(); e++) {
            trace(TRACE_0, "\t!" + string(PN->getInputPlace(e)->name));
            if (PN->getInputPlace(e)->max_occurence >= 0) {
                trace(TRACE_0, "\t(max. " + intToString(PN->getInputPlace(e)->max_occurence) + "x)\n");
            } else {
                trace(TRACE_0, "\t(no limit)\n");
            }
        }
        trace(TRACE_0, "    receiving events:\n" );
        for (unsigned int e = 0; e < PN->getOutputPlaceCount(); e++) {
            trace(TRACE_0, "\t?" + string(PN->getOutputPlace(e)->name));
            if (PN->getOutputPlace(e)->max_occurence >= 0) {
                trace(TRACE_0, "\t(max. " + intToString(PN->getOutputPlace(e)->max_occurence) + "x)\n");
            } else {
                trace(TRACE_0, "\t(no limit)\n");
            }
        }
    }

    // report message bound
    if (options[O_MESSAGES_MAX]) {
        trace(TRACE_0, "interface message bound set to: " + intToString(messages_manual) +"\n");
    }
    trace(TRACE_0, "\n");
}


// **********************************************************************************
// *******                    mode dependent functions                       ********
// **********************************************************************************


// check the exchangeability of two nets (using BDDs)
void checkExchangeability() {
    list<char*>::iterator netiter = netfiles.begin();
    Exchangeability* og1 = new Exchangeability(*netiter);
    Exchangeability* og2 = new Exchangeability(*(++netiter));
    trace(TRACE_0, "The two operating guidelines are equal: ");
    if (og1->check(og2) == true) {
        trace(TRACE_0, "YES\n");
    } else {
        trace(TRACE_0, "NO\n");
    }
}


// match a net against an og
void matchNet(OGFromFile* OGToMatch, oWFN* PN) {
    string reasonForFailedMatch;
    if (PN->matchesWithOG(OGToMatch, reasonForFailedMatch)) {
    	trace(TRACE_0, "oWFN matches with OG: YES\n");
    } else {
    	trace(TRACE_0, "oWFN matches with OG: NO\n");
    	trace(TRACE_0, "Match failed, because: " +
    	reasonForFailedMatch + "\n");
    }
    delete OGToMatch;
}


// create an OG of an oWFN
void computeOG(oWFN* PN) {
    
    time_t seconds, seconds2;

    OG * graph = new OG(PN);

    trace(TRACE_0, "building the operating guideline...\n");
    seconds = time (NULL);

    graph->printProgressFirst();
    graph->calculateRootNode();	// creates the root node and calculates its reachability graph (set of states)

    if (options[O_OTF]){
        graph->bdd->convertRootNode(graph->getRoot());
    }

    graph->buildGraph(); // build operating guideline

    seconds2 = time (NULL);

    trace(TRACE_0, "\nbuilding the operating guideline finished.\n");
    cout << "    " << difftime(seconds2,seconds) << " s consumed for building graph" << endl;

    trace(TRACE_0, "\nnet is controllable: ");
    if (graph->getRoot()->getColor() == BLUE) {
        trace(TRACE_0, "YES\n\n");
    } else {
        trace(TRACE_0, "NO\n\n");
    }

    // print statistics
    trace(TRACE_0, "OG statistics:\n");
    graph->printGraphStatistics();
    trace(TRACE_0, "\n");

    // generate output files
    if (!options[O_NOOUTPUTFILES]) {
        graph->printGraphToDot();      // .out
        graph->printOGtoFile();        // .og

        if (options[O_SYNTHESIZE_PARTNER_OWFN]) {
            graph->printGraphToSTG();	    
        }

        if (options[O_OTF]) {
            //graph->bdd->printGraphToDot();
            graph->bdd->save("OTF");
        }
    }

    if (options[O_BDD]) {
        trace(TRACE_0, "\nbuilding the BDDs...\n");
        seconds = time (NULL);
        graph->convertToBdd();      
        seconds2 = time (NULL);
        cout << difftime(seconds2,seconds) << " s consumed for building and reordering the BDDs" << endl;

        //graph->bdd->printGraphToDot();
        if (!options[O_NOOUTPUTFILES]) {
            graph->bdd->save();
        }
    }

    trace(TRACE_5, "computation finished -- trying to delete graph\n");
    delete graph;
    trace(TRACE_5, "graph deleted\n");

    // trace(TRACE_0, "HIT A KEY TO CONTINUE"); getchar();

    trace(TRACE_0, "=================================================================\n");
    trace(TRACE_0, "\n");
}


// create an IG of an oWFN
void computeIG(oWFN* PN) {

    time_t seconds, seconds2;

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

    // print statistics
    trace(TRACE_0, "IG statistics:\n");
    graph->printGraphStatistics();
    trace(TRACE_0, "\n");

    if (!options[O_NOOUTPUTFILES]) {
        // generate output files
        graph->printGraphToDot();      // .out

        if (options[O_SYNTHESIZE_PARTNER_OWFN]) {
            graph->printGraphToSTG();	    
        }
    }

    trace(TRACE_5, "computation finished -- trying to delete graph\n");
//			trace(TRACE_0, "HIT A KEY TO CONTINUE"); getchar();
    delete graph;
    trace(TRACE_5, "graph deleted\n");
    trace(TRACE_0, "=================================================================\n");
    trace(TRACE_0, "\n");
}


// create the productOG of all given OGs
void computeProductOG(OGFromFile::ogs_t OGsFromFiles) {
    trace("Building product of the following OGs:\n");
    for (OGFromFile::ogfiles_t::const_iterator iOgFile = ogfiles.begin();
         iOgFile != ogfiles.end(); ++iOgFile) {
        trace(*iOgFile); trace("\n");
    }
    trace("\n");

    OGFromFile* productOG = OGFromFile::product(OGsFromFiles);

    if (productOG->hasNoRoot()) {
        trace("The product OG is empty.\n\n");
    }

    if (!options[O_OUTFILEPREFIX]) {
        outfilePrefix = OGFromFile::getProductOGFilePrefix(ogfiles);
    }

    if (!options[O_NOOUTPUTFILES]) {
        trace("Saving product OG to:\n");
        trace(OGFromFile::addOGFileSuffix(outfilePrefix)); trace("\n");
        productOG->printOGFile(outfilePrefix);
        trace("\n");

        productOG->printOGFile(outfilePrefix);
        productOG->printDotFile(outfilePrefix);
    }

    delete productOG;

    for (OGFromFile::ogs_t::const_iterator iOg = OGsFromFiles.begin();
         iOg != OGsFromFiles.end(); ++iOg) {
        delete *iOg;
    }
}


// check for simulation relation of two given OGs
void checkSimulation(OGFromFile::ogs_t OGsFromFiles) {

    list<OGFromFile*>::iterator OGFromFileIter = OGsFromFiles.begin();
    OGFromFile *simulator = *OGFromFileIter;
    OGFromFile *simulant = *(++OGFromFileIter);

    if (simulator->simulates(simulant)) {
        trace(TRACE_0, "\nThe first OG has all the strategies of the second one, possibly more.\n" );
    } else {
        trace(TRACE_0, "\nThe second OG has a strategy which the first one hasn't.\n");
    }
}


// check if two given ogs are equal
void checkEquality(OGFromFile::ogs_t OGsFromFiles) {

    list<OGFromFile*>::iterator OGFromFileIter = OGsFromFiles.begin();
    OGFromFile *simulator = *OGFromFileIter;
    OGFromFile *simulant = *(++OGFromFileIter);

    if (simulator->simulates(simulant)) {
        if (simulant->simulates(simulator)) {
            trace(TRACE_0, "\nThe two OGs are equivalent, that is, they have the same strategies.\n");
        } else {
            trace(TRACE_0, "\nThe first OG has a strategy which the second one hasn't.\n");
        }
    } else {
        trace(TRACE_0, "\nThe second OG has a strategy which the first one hasn't.\n");
    }
}


// computes the number of Services that are determined by every single OG
void countServices(OGFromFile::ogs_t OGsFromFiles) {

    for (list<OGFromFile*>::iterator OGFromFileIter = OGsFromFiles.begin();
         OGFromFileIter != OGsFromFiles.end(); OGFromFileIter++) {

        if ((*OGFromFileIter)->acyclic()) {
            trace("The given OG is acyclic\n\n");
        } else {
            trace("The given OG is is NOT ayclic\n\n");
        }
    }
}


// checks whether an OG is acyclic
void checkAcyclicity(OGFromFile::ogs_t OGsFromFiles) {

    for (list<OGFromFile*>::iterator OGFromFileIter = OGsFromFiles.begin();
         OGFromFileIter != OGsFromFiles.end(); OGFromFileIter++) {

        if ((*OGFromFileIter)->acyclic()) {
            trace("The given OG is acyclic\n\n");
        } else {
            trace("The given OG is is NOT ayclic\n\n");
        }
    }
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

    OGFromFile* OGToMatch = NULL;
    set_new_handler(&myown_newhandler);

    // evaluate command line options
    parse_command_line(argc, argv);


// **********************************************************************************
// check if the numbers of inputfiles corresponds to the chosen mode
    if (options[O_EX] == true && netfiles.size() != 2) {
        cerr << "Error: \t If option -x is used, exactly two oWFNs must be entered\n" << endl;
        exit(1);
    }

    if (options[O_MATCH] && ogfiles.size() != 1) {
        cerr << "Exactly one operating guideline must be given for matching!\n" << endl;
        exit(1);
    }

    if (options[O_PRODUCTOG] && ogfiles.size() < 2) {
        cerr << "Error: \t Give at least two OGs to build their product!\n" << endl;
        exit(1);
    }

    if (options[O_SIMULATES] && ogfiles.size() != 2) {
        cerr << "Error: \t If option -t simulation is used, exactly two OG files must be entered\n" << endl;
        exit(1);
    }

    if (options[O_EQUALS] && ogfiles.size() != 2) {
        cerr << "Error: \t If option -t equals is used, exactly two OG files must be entered\n" << endl;
        exit(1);
    }


// **********************************************************************************
// start OG file dependant operations

    if (options[O_MATCH] || options[O_PRODUCTOG] || options[O_SIMULATES] ||
        options[O_EQUALS] || options[O_COUNT_SERVICES] || options[O_CHECK_ACYCLIC]) {

        // reading all OG-files
        OGFromFile::ogs_t OGsFromFiles;
        readAllOGs(OGsFromFiles);
    
#ifdef YY_FLEX_HAS_YYLEX_DESTROY
    		// Destroy buffer of OG parser.
    		// Must NOT be called before fclose(og_yyin);
    		og_yylex_destroy();
#endif

        if (options[O_MATCH]) {
            // matching against an OG
            ogfileToMatch = *(ogfiles.begin());
            OGToMatch = readog(ogfileToMatch);
        }

        if (options[O_PRODUCTOG]) {
            // calculating the product OG
            computeProductOG(OGsFromFiles);    
            return 0;
        }

        if (options[O_SIMULATES]) {
            // simulation on OGFromFile
            checkSimulation(OGsFromFiles);
            return 0;
        }

        if (options[O_EQUALS]) {
            // equivalence on OGFromFile
            checkEquality(OGsFromFiles);
            return 0;
        }

        if (options[O_COUNT_SERVICES]) {
            // counts the number of deterministic strategies
            // that are characterized by a given OG
            countServices(OGsFromFiles);
            return 0;
        }

        if (options[O_CHECK_ACYCLIC]) {
            // counts the number of deterministic strategies
            // that are characterized by a given OG
            checkAcyclicity(OGsFromFiles);
            return 0;
        }
    }


// **********************************************************************************
// start petrinet-file dependant operations

    if (options[O_EX] || options[O_MATCH] || parameters[P_OG] || parameters[P_IG]) {

        if (options[O_EX] == true) {
            // checking exchangeability using BDDs
            checkExchangeability();
            return 0;
        }

        // ---------------- processing every single net -------------------
        for (list<char*>::iterator netiter = netfiles.begin();
            netiter != netfiles.end(); ++netiter) {
       
            numberOfEvents = 0;
            numberOfDecodes = 0;
            garbagefound = 0;
            global_progress = 0;
            show_progress = 0;
            State::state_count = 0;          // number of states
            numberDeletedVertices = 0;

            // prepare getting the net
            PlaceTable = new SymbolTab<PlSymbol>;

            // get the net
            netfile = *netiter;
            readnet();
            PN->filename = netfile;
            delete PlaceTable;   

            trace(TRACE_0, "=================================================================\n");
            if (netfile) {
                trace(TRACE_0, "processing net " + string(netfile) + "\n");
            }
            // report the net
            trace(TRACE_0, "    places: " + intToString(PN->getPlaceCount()));
            trace(TRACE_0, " (including " + intToString(PN->getInputPlaceCount()) + " input places, " + intToString(PN->getOutputPlaceCount()) + " output places)\n");
            trace(TRACE_0, "    transitions: " + intToString(PN->getTransitionCount()) + "\n");
            trace(TRACE_0, "    ports: " + intToString(PN->getPortCount()) + "\n\n");

            if (PN->FinalCondition) {
                trace(TRACE_0, "finalcondition used\n\n");
            } else if (PN->FinalMarking) {
                trace(TRACE_0, "finalmarking used\n\n");
            } else {
                trace(TRACE_0, "neither finalcondition nor finalmarking given\n");
            }

            // adjust events_manual and print limit of considering events
            reportOptionValues();

            // start computation
                
            if (options[O_MATCH]) {
                // matching current oWFN against the OG 
                matchNet(OGToMatch, PN);    
            }

            if (parameters[P_OG]) {
                // computing OG of the current oWFN
                computeOG(PN);
            }

            if (parameters[P_IG]) {
                // computing IG of the current oWFN
                computeIG(PN);
            }

            delete PN;
            trace(TRACE_5, "net deleted\n");

            // cout << "numberOfDecodes: " << numberOfDecodes << endl;   
        }

#ifdef YY_FLEX_HAS_YYLEX_DESTROY
        // Delete lexer buffer for parsing oWFNs.
        // Must NOT be called before fclose(owfn_yyin);
        owfn_yylex_destroy();
#endif
    } // end of petrinet dependant operations

#ifdef LOG_NEW
    NewLogger::printall();
#endif

	return 0;
}


std::string platform_basename(const std::string& path) {
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

