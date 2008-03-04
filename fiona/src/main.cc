/*****************************************************************************
 * Copyright 2005, 2006, 2007 Peter Massuthe, Daniela Weinberg,              *
 *           Jan Bretschneider, Christian Gierds, Leonard Kern               *
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
 *          project "Tools4BPEL" at the Humboldt-Universitt zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 */

#include "AnnotatedGraph.h"
#include "owfn.h"
#include "state.h"
#include "IG.h"
#include "OG.h"
#include "Exchangeability.h"
#include "options.h"
#include "debug.h"
#include "main.h"

#include <list>
#include <iostream>
#include <sstream>
#include <fstream>
#include "pnapi/pnapi.h"

#include "GraphFormula.h"

// #defines YY_FLEX_HAS_YYLEX_DESTROY if we can call yylex_destroy()
#include "lexer_owfn_wrap.h"
#ifdef LOG_NEW
#include "newlogger.h"
#endif
#include <libgen.h>

using namespace std;

extern int owfn_yylineno;
extern int owfn_yydebug;
extern int owfn_yy_flex_debug;
extern FILE* owfn_yyin;
extern int owfn_yyerror();
extern int owfn_yyparse();

#ifdef YY_FLEX_HAS_YYLEX_DESTROY
extern int owfn_yylex_destroy();
#endif

extern int og_yylineno;
extern int og_yydebug;
extern int og_yy_flex_debug;
extern FILE* og_yyin;
extern int og_yyerror();
extern int og_yyparse();

#ifdef YY_FLEX_HAS_YYLEX_DESTROY
extern int og_yylex_destroy();
#endif

/// Deletes all OGs in 'OGsFromFiles' and clears list.
void deleteOGs(AnnotatedGraph::ogs_t& OGsFromFiles);

extern unsigned int State::state_count;
extern unsigned int State::state_count_stored_in_binDec;
extern std::list<std::string> netfiles;
extern std::list<std::string> ogfiles;
extern AnnotatedGraph* OGToParse;

// the currently considered owfn from the owfn list given by the command line;
// used only in main.cc
std::string currentowfnfile;


int garbagefound;
unsigned int NonEmptyHash;
unsigned int numberOfDecodes;
unsigned int numberDeletedVertices;
unsigned int numberOfEvents;


//! \brief an exit function in case the memory is exhausted
void myown_newhandler() {
    cerr << "new failed, memory exhausted"<< endl;
    exit(2);
}


//! \brief reads an oWFN from owfnfile
//! \param owfnfile the owfnfile to be read from
void readnet(const std::string& owfnfile) {
    owfn_yylineno = 1;
    owfn_yydebug = 0;
    owfn_yy_flex_debug = 0;
    assert(owfnfile != "");
    // diagnosefilename = (char *) 0;

    trace(TRACE_5, "reading from file " + owfnfile + "\n");
    if (owfnfile == "<stdin>") {
        owfn_yyin = stdin;
    }
    else owfn_yyin = fopen(owfnfile.c_str(), "r");
    if (!owfn_yyin) {
        cerr << "cannot open owfn file " << owfnfile << "' for reading'\n" << endl;
        exit(4);
    }
    // diagnosefilename = owfnfile;

    PN = new oWFN();
    if (owfnfile == "<stdin>") {
        PN->filename = "stdin";
    }
    else PN->filename = owfnfile;

    owfnfileToParse = owfnfile;
    owfn_yyparse();
    fclose(owfn_yyin);
    for (unsigned int i = 0; i < PN->getPlaceCount(); i++) {
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


//! \brief reports the net statistics
void reportNet() {
    trace(TRACE_0, "    places: " + intToString(PN->getPlaceCount()));
    trace(TRACE_0, " (including " + intToString(PN->getInputPlaceCount()));
    trace(TRACE_0, " input places, " + intToString(PN->getOutputPlaceCount()));
    trace(TRACE_0, " output places)\n");
    trace(TRACE_0, "    transitions: " + intToString(PN->getTransitionCount()) + "\n");
    trace(TRACE_0, "    ports: " + intToString(PN->getPortCount()) + "\n\n");
    if (PN->FinalCondition) {
        trace(TRACE_0, "finalcondition used\n\n");
    } else if (PN->FinalMarking) {
        trace(TRACE_0, "finalmarking used\n\n");
    } else {
        trace(TRACE_0, "neither finalcondition nor finalmarking given\n");
    }
}


//! \brief reads an OG from ogfile
//! \param ogfile to be read from
//! \return returns the OG object
AnnotatedGraph* readog(const std::string& ogfile) {
    og_yylineno = 1;
    og_yydebug = 0;
    og_yy_flex_debug = 0;
    assert(ogfile != "");
    trace(TRACE_5, "reading from file " + ogfile + "\n");
    og_yyin = fopen(ogfile.c_str(), "r");
    if (!og_yyin) {
        cerr << "cannot open OG file '" << ogfile << "' for reading'\n" << endl;
        exit(4);
    }
    OGToParse = new AnnotatedGraph();
    ogfileToParse = ogfile;
    og_yyparse();
    fclose(og_yyin);

    return OGToParse;
}


//! \brief reads all OGs from files and adds them to the given list
//! \param theOGs a list of OGs
void readAllOGs(AnnotatedGraph::ogs_t& theOGs) {

    for (AnnotatedGraph::ogfiles_t::const_iterator iOgFile = ogfiles.begin();
         iOgFile != ogfiles.end(); ++iOgFile) {

        theOGs.push_back(readog(*iOgFile));
    }
}


//! \brief deletes all OGs from a list
//! \param OGsFromFiles a list of OGs
void deleteOGs(AnnotatedGraph::ogs_t& OGsFromFiles) {
    trace(TRACE_5, "main: deleteOGs(const AnnotatedGraph::ogs_t& OGsFromFiles) : start\n");

    for (AnnotatedGraph::ogs_t::const_iterator iOg = OGsFromFiles.begin();
         iOg != OGsFromFiles.end(); ++iOg) {
        delete *iOg;
    }

    OGsFromFiles.clear();

    trace(TRACE_5, "main: deleteOGs(const AnnotatedGraph::ogs_t& OGsFromFiles) : end\n");
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
            trace(TRACE_0, "each event considered max: "
                    + intToString(events_manual) +"\n");
        }
    } else {
        trace(TRACE_0, "considering the following events:\n");
        trace(TRACE_0, "    sending events:\n");
        for (unsigned int e = 0; e < PN->getInputPlaceCount(); e++) {
            trace(TRACE_0, "\t!" + string(PN->getInputPlace(e)->name));
            if (PN->getInputPlace(e)->max_occurence >= 0) {
                trace(TRACE_0, "\t(max. " + intToString(PN->getInputPlace(e)->max_occurence) + "x)\n");
            } else {
                trace(TRACE_0, "\t(no limit)\n");
            }
        }
        trace(TRACE_0, "    receiving events:\n");
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
        trace(TRACE_0, "interface message bound set to: "
                + intToString(messages_manual) +"\n");
    }
    trace(TRACE_0, "\n");
}


 // **********************************************************************************
// *******                    mode dependent functions                       ********
// **********************************************************************************

//! \brief create an IG of an oWFN
//! \param PN the given oWFN
string computeIG(oWFN* PN) {

    time_t seconds, seconds2;
    interactionGraph * graph = new interactionGraph(PN);
    bool controllable = false;

    string igFilename = "";
    
    if (options[O_CALC_REDUCED_IG]) {
        trace(TRACE_0, "building the reduced interaction graph...\n");
    } else {
        trace(TRACE_0, "building the interaction graph...\n");
    }

    seconds = time (NULL);

    graph->buildGraph(); // build interaction graph

    seconds2 = time (NULL);

    if (options[O_CALC_REDUCED_IG]) {
        trace(TRACE_0, "building the reduced interaction graph finished.\n");
    } else {
        trace(TRACE_0, "\nbuilding the interaction graph finished.\n");
    }

    cout << difftime(seconds2, seconds) << " s consumed for building graph" << endl;

    trace(TRACE_0, "\nnet is controllable: ");
    if (graph->getRoot()->getColor() == BLUE) {
        trace(TRACE_0, "YES\n\n");
        controllable = true;
    } else {
        trace(TRACE_0, "NO\n\n");
    }

    // print statistics
    trace(TRACE_0, "IG statistics:\n");
    graph->printGraphStatistics();
    trace(TRACE_0, "\n");

    if (!options[O_NOOUTPUTFILES]) {
        if (options[O_DIAGNOSIS]) {
            graph->diagnose();
        }

        if (!options[O_EQ_R]) {	// don't create png if we are in eqr mode
        	// generate output files
        	graph->printGraphToDot(); // .out, .png
        }

/* create also an .og file to enable comparison of different IGs */
        
        if (options[O_OUTFILEPREFIX]) {
            igFilename = outfilePrefix;
        } else {
            igFilename = graph->getFilename();
        }

        if (!options[O_CALC_ALL_STATES]) {
            igFilename += ".R";
        }

        igFilename += ".ig";
        
        graph->printOGFile(igFilename);

/* */
        if (options[O_SYNTHESIZE_PARTNER_OWFN]) {
            if (controllable) {
                graph->printGraphToSTG();
            } else {
                trace(TRACE_0, "\nCannot synthesize a partner for a net, that is not controllable\n\n");
            }
        }
    }

    trace(TRACE_5, "computation finished -- trying to delete graph\n");
    // trace(TRACE_0, "HIT A KEY TO CONTINUE"); getchar();

    graph->clearNodeSet();
    delete graph;
    trace(TRACE_5, "graph deleted\n");
    trace(TRACE_0, "=================================================================\n");
    trace(TRACE_0, "\n");
    
    return igFilename;
}


//! \brief generate a public view for a given og
//! \param OG an og to generate the public view of
//! \param graphName a name for the graph in the output
void computePublicView(AnnotatedGraph* OG, string graphName) {

    trace(TRACE_0, "generating the public view for ");
    trace(graphName);
    trace("\n\n");

    unsigned int maxSizeForDot = 120;
    
    outfilePrefix = AnnotatedGraph::stripOGFileSuffix(graphName);
    outfilePrefix += ".sa";

    Graph* cleanPV = new Graph();
    
    OG->transformToPublicView(cleanPV);

    // generate output files
    if (!options[O_NOOUTPUTFILES]) {
    	
    	if(cleanPV->numberOfNodes() > maxSizeForDot)
    	{
    		trace(TRACE_0, "the public view service automaton is to big to generate a dot file\n\n");
    	} else {
    		trace(TRACE_0, "generating dot output...\n");

    		// .out
    		cleanPV->printDotFile(outfilePrefix, "public view of " + graphName);
    	}
    	
        //transform to owfn
        PNapi::PetriNet* PVoWFN = new PNapi::PetriNet(); 
        PVoWFN->set_format(PNapi::FORMAT_OWFN, true);
        cleanPV->transformToOWFN(PVoWFN);

		trace(TRACE_0, "Public View oWFN statistics:\n");
        trace(PVoWFN->information());
		trace(TRACE_0, "\n");
        
        ofstream output;
        const string owfnOutput = AnnotatedGraph::stripOGFileSuffix(graphName) + ".sa.owfn";
        output.open (owfnOutput.c_str(),ios::out);

        (output) << (*PVoWFN);
        output.close();

        trace(TRACE_0, "=================================================================\n");
        trace(TRACE_0, "\n");
    }
}


//! \brief create an OG of an oWFN
//! \param PN the given oWFN
string computeOG(oWFN* PN) {

    time_t seconds, seconds2;
    OG* graph = new OG(PN);
    bool controllable = false;
    string ogFilename = "";
    
    trace(TRACE_0, "building the operating guideline...\n");
    seconds = time (NULL);

    graph->printProgressFirst();

    graph->buildGraph(); // build operating guideline

    seconds2 = time (NULL);

    trace(TRACE_0, "\nbuilding the operating guideline finished.\n");

    cout << "    " << difftime(seconds2, seconds) << " s consumed for building graph" << endl;

    trace(TRACE_0, "\nnet is controllable: ");
    if (graph->getRoot()->getColor() == BLUE) {
        trace(TRACE_0, "YES\n\n");
        controllable = true;
    } else {
        trace(TRACE_0, "NO\n\n");
    }

    // print statistics
    trace(TRACE_0, "OG statistics:\n");
    graph->printGraphStatistics();
    trace(TRACE_0, "\n");

    // switch to publicview generation if the mode is PV
    if (parameters[P_PV]) {
        
        string publicViewName;
        
        if (options[O_OUTFILEPREFIX]) {
            publicViewName = outfilePrefix;
        } else {
            publicViewName = graph->getFilename();
            publicViewName = publicViewName.substr(0, publicViewName.size()-5);
        }
        
        computePublicView(graph, publicViewName);
        
        delete graph;
        return "";
    }
    
    // generate output files
    if (!options[O_NOOUTPUTFILES]) {
        // distributed controllability?
        if (options[O_DISTRIBUTED]) {
            trace(TRACE_0, "\nannotating OG for distributed controllability\n");

            bool graphChanged = true;
            while (graphChanged) {
                graphChanged = graph->annotateGraphDistributedly();
                graph->correctNodeColorsAndShortenAnnotations();
                // cerr << endl;
            }

            parameters[P_SHOW_EMPTY_NODE] = false;

            trace(TRACE_0, "\nnet is distributedly controllable: ");
            if (graph->getRoot()->getColor() == BLUE) {
                trace(TRACE_0, "MAYBE\n\n");
            } else {
                trace(TRACE_0, "NO\n\n");
                parameters[P_SHOW_ALL_NODES] = true;
            }
        }

        if (!options[O_EQ_R]) {	// don't create png if we are in eqr mode
        	graph->printGraphToDot(); // .out, .png
        }

        if (options[O_OUTFILEPREFIX]) {
            ogFilename = outfilePrefix;
        } else {
            ogFilename = graph->getFilename();
        }
        
        if (!options[O_CALC_ALL_STATES]) {
            ogFilename += ".R";
        }

        graph->printOGFile(ogFilename);

        if (options[O_SYNTHESIZE_PARTNER_OWFN]) {
            if (controllable) {
                graph->printGraphToSTG();
            } else {
                trace(TRACE_0, "\nCannot synthesize a partner for a net, that is not controllable\n\n");
            }
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
        cout << difftime(seconds2, seconds) << " s consumed for building and reordering the BDDs" << endl;

        //graph->bdd->printGraphToDot();
        if (!options[O_NOOUTPUTFILES]) {
            graph->bdd->save();
        }
    }

    trace(TRACE_5, "computation finished -- trying to delete graph\n");
    graph->clearNodeSet();
    delete graph;
    trace(TRACE_5, "graph deleted\n");

    // trace(TRACE_0, "HIT A KEY TO CONTINUE"); getchar();
    trace(TRACE_0, "=================================================================\n");
    trace(TRACE_0, "\n");

    return ogFilename;
}


//! \brief create the productOG of all given OGs
//! \param OGsFromFiles a list of all OGs for the product
void computeProductOG(const AnnotatedGraph::ogs_t& OGsFromFiles) {

    trace("Building product of the following OGs:\n");

    // A node annotated with true can only appear in product OG
    // if all argument OGs contain a true node.
    // Such a node should not be rejected, so we adjust parameters as if
    // -s empty were given.
    if (!options[O_SHOW_NODES]) {
        // but only if not explicitly requested to hide empty node on command line
        // by -s blue, for example
        parameters[P_SHOW_BLUE_NODES] = true;
        parameters[P_SHOW_EMPTY_NODE] = true;
        parameters[P_SHOW_RED_NODES] = false;
        parameters[P_SHOW_ALL_NODES] = false;
    }

    for (AnnotatedGraph::ogfiles_t::const_iterator iOgFile = ogfiles.begin();
         iOgFile != ogfiles.end(); ++iOgFile) {

        trace(*iOgFile + "\n");
    }
    trace("\n");

    AnnotatedGraph* productOG = AnnotatedGraph::product(OGsFromFiles);

    if (productOG->hasNoRoot()) {
        trace("The product OG is empty.\n\n");
    }

    if (!options[O_OUTFILEPREFIX]) {
        outfilePrefix = AnnotatedGraph::getProductOGFilePrefix(ogfiles);
    }

    if (!options[O_NOOUTPUTFILES]) {
        trace("Saving product OG to:\n");
        trace(AnnotatedGraph::addOGFileSuffix(outfilePrefix));
        trace("\n\n");
//        productOG->printOGFile(outfilePrefix);
//        trace("\n");

        productOG->printOGFile(outfilePrefix);
        productOG->printDotFile(outfilePrefix);
    }

    delete productOG;
}


//! \brief match a net against an og
//! \param OGToMatch an og to be matched against
//! \param PN an oWFN to matched against an og
void checkMatching(AnnotatedGraph* OGToMatch, oWFN* PN) {
    string reasonForFailedMatch;
    if (PN->matchesWithOG(OGToMatch, reasonForFailedMatch)) {
        trace(TRACE_0, "oWFN matches with OG: YES\n\n");
    } else {
        trace(TRACE_0, "oWFN matches with OG: NO\n");
        trace(TRACE_0, "Match failed: " +reasonForFailedMatch + "\n\n");
    }
    delete OGToMatch;
}


//! \brief check for simulation relation of two given OGs
//! \param OGsFromFiles a list containing exactly two OGs
void checkSimulation(AnnotatedGraph::ogs_t& OGsFromFiles) {
    
    // the OGs given by command line are already stored in OGsFromFiles
    
    bool calledWithNet = false;
    
    // we can only check exactly 2 OGs for simulation
    // a possible violation should have been rejected by options.cc
    assert(netfiles.size() + OGsFromFiles.size() == 2);
    
    if (OGsFromFiles.size() < 2) {
        // oWFN(s) was given on command line, so compute the corresponding OGs
        calledWithNet = true;
        
        // simulation  on OGs depends heavily on empty node,
        // so set the correct options to compute OG with empty node
        options[O_SHOW_NODES] = true;
        parameters[P_SHOW_EMPTY_NODE] = true;
    }
    
    // generate the OGs
    list<std::string>::iterator netiter = netfiles.begin();
    while (OGsFromFiles.size() < 2) {
        
        numberOfEvents = 0;
        numberOfDecodes = 0;
        garbagefound = 0;
        global_progress = 0;
        show_progress = 0;
        State::state_count = 0; // number of states
        State::state_count_stored_in_binDec = 0; // number of states
        numberDeletedVertices = 0;
        
        currentowfnfile = *netiter;
        assert(currentowfnfile != "");
        
        // prepare getting the net
        PlaceTable = new SymbolTab<PlSymbol>;
        
        // get the net into variable PN
        readnet(currentowfnfile);
        trace(TRACE_0, "=================================================================\n");
        trace(TRACE_0, "processing net " + currentowfnfile + "\n");
        reportNet();
        delete PlaceTable;
       
        // compute OG
        OG* graph = new OG(PN);
        trace(TRACE_0, "building the operating guideline...\n");
        graph->printProgressFirst();
        graph->buildGraph(); // build operating guideline
        trace(TRACE_0, "\nbuilding the operating guideline finished.\n\n");
        
        // add new OG to the list
        if (!OGfirst && netfiles.size() == 1) {
            OGsFromFiles.push_front(graph);
        } else {
            OGsFromFiles.push_back(graph);
        }
        // we just need the graph and no longer the oWFN
        graph->deleteOWFN();
        PN = NULL;

        netiter++;
    }
    
    //    // restore state of parameters
    //    options[O_SHOW_NODES] = tempO_SHOW_NODES;
    //    parameters[P_SHOW_EMPTY_NODE] = tempP_SHOW_EMPTY_NODE;
    
    AnnotatedGraph::ogs_t::const_iterator currentOGfile = OGsFromFiles.begin();
    AnnotatedGraph* firstOG = *(currentOGfile++);
    AnnotatedGraph* secondOG = *(currentOGfile);

    firstOG->removeFalseNodes();
    secondOG->removeFalseNodes();
    
    trace(TRACE_1, "checking simulation\n");
    if (firstOG->simulates(secondOG)) {
        trace(TRACE_1, "simulation holds\n\n");
        trace(TRACE_0, "The first OG characterizes all strategies of the second one.\n\n");
    } else {
        trace(TRACE_1, "simulation does not hold\n\n");
        trace(TRACE_0, "The second OG characterizes at least one strategy that is\n");
        trace(TRACE_0, "not characterized by the first one.\n\n");
    }
    
    if (!calledWithNet) {
        trace(TRACE_0, "Attention: This result is only valid if the given OGs are complete\n");
        trace(TRACE_0, "           (i.e., \"-s empty\" option was set and \"-m\" option high enough)\n\n");
    }

    deleteOGs(OGsFromFiles);
}


//! \brief check if two given OGs characterize the same set of strategies;
//!        if called with an oWFN, then the corresponding OG is computed first
//! \param OGsFromFiles a list containing all OGs that are not provided as oWFNs
void checkEquivalence(AnnotatedGraph::ogs_t& OGsFromFiles) {

    // the OGs given by command line are already stored in OGsFromFiles

    bool calledWithNet = false;

    // we can only check exactly 2 OGs for equivalence
    // a possible violation should have been rejected by options.cc
    
    if (!options[O_EQ_R]) {
    	assert(netfiles.size() + OGsFromFiles.size() == 2);
    } else {
    	assert(OGsFromFiles.size() == 2);
    }

    if (OGsFromFiles.size() < 2) {
        // oWFN(s) was given on command line, so compute the corresponding OGs
        calledWithNet = true;

        // equivalence on OGs depends heavily on empty node,
        // so set the correct options to compute OG with empty node
        options[O_SHOW_NODES] = true;
        parameters[P_SHOW_EMPTY_NODE] = true;
    }

    // generate the OGs
    list<std::string>::iterator netiter = netfiles.begin();
    while (OGsFromFiles.size() < 2) {

        numberOfEvents = 0;
        numberOfDecodes = 0;
        garbagefound = 0;
        global_progress = 0;
        show_progress = 0;
        State::state_count = 0; // number of states
        State::state_count_stored_in_binDec = 0; // number of states
        numberDeletedVertices = 0;

        currentowfnfile = *netiter;
        assert(currentowfnfile != "");

        // prepare getting the net
        PlaceTable = new SymbolTab<PlSymbol>;

        // get the net into variable PN
        readnet(currentowfnfile);
        trace(TRACE_0, "=================================================================\n");
        trace(TRACE_0, "processing net " + currentowfnfile + "\n");
        reportNet();
        delete PlaceTable;

        // compute OG
        OG* graph = new OG(PN);
        trace(TRACE_0, "building the operating guideline...\n");
        graph->printProgressFirst();
        graph->buildGraph(); // build operating guideline
        trace(TRACE_0, "\nbuilding the operating guideline finished.\n\n");

        // add new OG to the list
        if (!OGfirst && netfiles.size() == 1) {
            OGsFromFiles.push_front(graph);
        } else {
            OGsFromFiles.push_back(graph);
        }
        graph->deleteOWFN();

        netiter++;
    }

    trace(TRACE_0, "\n=================================================================\n");
    trace(TRACE_0, "Checking equivalence of generated OGs...\n\n");            

    AnnotatedGraph::ogs_t::const_iterator currentOGfile = OGsFromFiles.begin();
    AnnotatedGraph *firstOG = *currentOGfile;
    AnnotatedGraph *secondOG = *(++currentOGfile);

    firstOG->removeFalseNodes();
    secondOG->removeFalseNodes();

    trace(TRACE_1, "checking first simulation: (\"" + firstOG->getFilename() + "\" simulates \"" + secondOG->getFilename() + "\")\n");
    if (firstOG->simulates(secondOG)) {
        trace(TRACE_1, "first simulation (\"" + firstOG->getFilename() + "\" simulates \"" + secondOG->getFilename() + "\") holds\n\n");
        trace(TRACE_1, "checking second simulation (\"" + firstOG->getFilename() + "\" simulates \"" + secondOG->getFilename() + "\")\n");
        if (secondOG->simulates(firstOG)) {
            trace(TRACE_1, "second simulation holds (\"" + secondOG->getFilename() + "\" simulates \"" + firstOG->getFilename() + "\")\n\n");
            trace(TRACE_0, "The two OGs characterize the same strategies.\n\n");
        } else {
            trace(TRACE_1, "second simulation (\"" + secondOG->getFilename() + "\" simulates \"" + firstOG->getFilename() + "\") does not hold\n\n");
            trace(TRACE_0, "The first OG (\"" + firstOG->getFilename() + "\") characterizes all strategies of the second one (\"" + secondOG->getFilename() + "\")\n");
            trace(TRACE_0, "(and at least one more).\n\n");
        }
    } else {
        trace(TRACE_1, "first simulation (\"" + firstOG->getFilename() + "\" simulates \"" + secondOG->getFilename() + "\") does not hold\n\n");
        trace(TRACE_1, "checking second simulation (\"" + secondOG->getFilename() + "\" simulates \"" + firstOG->getFilename() + "\")\n");
        if (secondOG->simulates(firstOG)) {
            trace(TRACE_1, "second simulation holds (\"" + secondOG->getFilename() + "\" simulates \"" + firstOG->getFilename() + "\")\n\n");
            trace(TRACE_0, "The second OG (\"" + secondOG->getFilename() + "\") characterizes all strategies of the first one (\"" + firstOG->getFilename() + "\")\n");
            trace(TRACE_0, "(and at least one more).\n\n");
        } else {
            trace(TRACE_1, "second simulation (\"" + secondOG->getFilename() + "\" simulates \"" + firstOG->getFilename() + "\") does not hold\n\n");
            trace(TRACE_0, "Both OGs characterize at least one strategy that is\n");
            trace(TRACE_0, "not characterized by the other one.\n\n");
        }
    }

    if (!calledWithNet && !options[O_EQ_R]) {
        trace(TRACE_0, "Attention: This result is only valid if the given OGs are complete\n");
        trace(TRACE_0, "           (i.e., \"-s empty\" option was set and \"-m\" option high enough)\n\n");
    }

    deleteOGs(OGsFromFiles);
}


//! \brief modifies the first OG such that it simulates the second OG
//!        by filtering non-simulating branches
//! \param OGsFromFiles a list containing exactly two OGs
void filterOG(const AnnotatedGraph::ogs_t& OGsFromFiles) {

    // exactly 2 OGs must be given
    assert(OGsFromFiles.size() == 2);

    AnnotatedGraph::ogs_t::const_iterator GraphIter = OGsFromFiles.begin();
    AnnotatedGraph *lhs = *GraphIter;
    AnnotatedGraph *rhs = *(++GraphIter);

    trace("Filter OG from file ");
    trace(*ogfiles.begin() + " through OG from file ");
    trace(*(++ogfiles.begin()) + ".\n\n");

    lhs->filter(rhs);

    if (lhs->hasNoRoot()) {
        trace("The filtered OG is empty.\n\n");
    }

    if (!options[O_OUTFILEPREFIX]) {
        outfilePrefix = AnnotatedGraph::stripOGFileSuffix(*ogfiles.begin()) + ".filtered";
    }

    if (!options[O_NOOUTPUTFILES]) {
        trace("Saving filtered OG to:\n");
        trace(AnnotatedGraph::addOGFileSuffix(outfilePrefix));
        trace("\n\n");

        lhs->printOGFile(outfilePrefix);
        lhs->printDotFile(outfilePrefix);
    }

    delete rhs;
    delete lhs;
}


//! \brief check for simulation relation of two given OGs while covering all external transitions
//! \param OGsFromFiles a list containing exactly two OGs
void checkCovSimulation(AnnotatedGraph::ogs_t& OGsFromFiles) {
    
    // the OGs given by command line are already stored in OGsFromFiles
    
    bool calledWithNet = false;
    
    // we can only check exactly 2 OGs for simulation
    // a possible violation should have been rejected by options.cc
    assert(netfiles.size() + OGsFromFiles.size() == 2);
    
    if (OGsFromFiles.size() < 2) {
        // oWFN(s) was given on command line, so compute the corresponding OGs
        calledWithNet = true;
        
        // simulation  on OGs depends heavily on empty node,
        // so set the correct options to compute OG with empty node
        options[O_SHOW_NODES] = true;
        parameters[P_SHOW_EMPTY_NODE] = true;
    }
    
    // generate the OGs
    list<std::string>::iterator netiter = netfiles.begin();
    while (OGsFromFiles.size() < 2) {
        
        numberOfEvents = 0;
        numberOfDecodes = 0;
        garbagefound = 0;
        global_progress = 0;
        show_progress = 0;
        State::state_count = 0; // number of states
        State::state_count_stored_in_binDec = 0; // number of states
        numberDeletedVertices = 0;
        
        currentowfnfile = *netiter;
        assert(currentowfnfile != "");
        
        // prepare getting the net
        PlaceTable = new SymbolTab<PlSymbol>;
        
        // get the net into variable PN
        readnet(currentowfnfile);
        trace(TRACE_0, "=================================================================\n");
        trace(TRACE_0, "processing net " + currentowfnfile + "\n");
        reportNet();
        delete PlaceTable;
        
        // compute OG
        OG* graph = new OG(PN);
        trace(TRACE_0, "building the operating guideline...\n");
        graph->printProgressFirst();
        graph->buildGraph(); // build operating guideline
        trace(TRACE_0, "\nbuilding the operating guideline finished.\n\n");
        
        // add new OG to the list
        if (!OGfirst && netfiles.size() == 1) {
            OGsFromFiles.push_front(graph);
        } else {
            OGsFromFiles.push_back(graph);
        }
        graph->deleteOWFN();
        
        netiter++;
    }
    
    //    // restore state of parameters
    //    options[O_SHOW_NODES] = tempO_SHOW_NODES;
    //    parameters[P_SHOW_EMPTY_NODE] = tempP_SHOW_EMPTY_NODE;
    
    AnnotatedGraph::ogs_t::const_iterator currentOGfile = OGsFromFiles.begin();
    AnnotatedGraph *firstOG = *currentOGfile;
    AnnotatedGraph *secondOG = *(++currentOGfile);
    
    firstOG->removeFalseNodes();
    secondOG->removeFalseNodes();
    
    trace(TRACE_1, "checking simulation\n");
    if (firstOG->covSimulates(secondOG)) {
        trace(TRACE_1, "simulation holds\n\n");
        trace(TRACE_0, "The first OG characterizes all strategies of the second one while covering all external transitions.\n\n");
    } else {
        trace(TRACE_1, "simulation does not hold\n\n");
        trace(TRACE_0, "The second OG characterizes at least one strategy that is\n");
        trace(TRACE_0, "not characterized by the first one or does not cover all external transitions.\n\n");
    }
    
    if (!calledWithNet) {
        trace(TRACE_0, "Attention: This result is only valid if the given OGs are complete\n");
        trace(TRACE_0, "           (i.e., \"-s empty\" option was set and \"-m\" option high enough)\n\n");
    }
    deleteOGs(OGsFromFiles);
}


//! \brief create a PNG of the given oWFN
//! \param PN an oWFN to generate a PNG from
void makePNG(oWFN* PN) {

    trace(TRACE_1, "Internal translation of the net into PNapi format...\n");

    // translate the net into PNapi format
    PNapi::PetriNet* PNapiNet = PN->returnPNapiNet();

    // set strings needed in PNapi output
    globals::output_filename = PN->filename;
    if (PN->finalConditionString != "") {
        globals::filename = PN->filename + " | Final Condition: "
                + PN->finalConditionString;
    } else {
        globals::filename = PN->filename + " | Final Marking: "
                + PN->finalMarkingString;
    }

    // create temporary stream as target for the dot output of the PNapiNet
    ostringstream* dot = new ostringstream(ostringstream::out);

    // set the output format to dot
    PNapiNet->set_format(PNapi::FORMAT_DOT, true);

    trace(TRACE_1, "Creating dot stream\n");

    // create the dot
    (*dot) << (*PNapiNet);

    // generate a string from the stream to be modified for piping
    string dotString = dot->str();

    delete(dot);

    unsigned int position;
    unsigned int deletePosition;

    trace(TRACE_3, "Modifiyng dot stream\n");

    // delete all comments in the dot output of the PNapiNet, since the endlines will 
    // be deleted for echo piping and "//" comments won't work anymore
    int counter = 0;
    while ((position = dotString.find_first_of("/", counter)) != string::npos) {
        if (dotString.at(position+1)=='/') {
            deletePosition = dotString.find_first_of("\n", (position + 2));
            dotString.erase(position, (deletePosition - position));
        } else {
          counter = position+1;
        }
    }

    // delete all endlines and escape all quotes for the echo string
    for (unsigned int i = 0; i != dotString.size(); i++) {
        char testchar;
        testchar = dotString[i];

        if (testchar == '\n') {
            dotString.erase(i, 1);
            dotString.insert(i, " ");
        }

        if (testchar == '"') {
            dotString.insert(i, "\\");
            i++;
        }
    }

    string outFileName;    
    
    if (!options[O_OUTFILEPREFIX]) {
        outFileName = globals::output_filename;
    } else {
        outFileName = outfilePrefix;
    }

    // finish the string for the system call
    dotString = "echo \"" + dotString + "\" | dot -q -Tpng -o \""
            + outFileName + ".png\"";

    // create the output
    if (!options[O_NOOUTPUTFILES]) {
        trace(TRACE_1, "Piping the stream to dot\n");
        system(dotString.c_str());
        trace(TRACE_0, (outFileName + ".png generated\n\n"));
    }
}


//! \brief compute the number of strategies that are characterized by a given OG
//! \param OG an og to compute the number of strategies for
//! \param graphName a string with the name of the file, the og was taken from
void countStrategies(AnnotatedGraph* OG, string graphName) {

    trace("Processing: ");
    trace(graphName);
    trace("\n");

    trace(TRACE_1, "Checking if the OG is acyclic\n");

    if (OG->isAcyclic()) {

        time_t seconds, seconds2;

        seconds = time (NULL);
        trace("Started computing the number of strategies\n");
        // Compute and show the number of Services
        trace("Computed number of strategies: " + intToString(OG->numberOfServices()) + "\n");
        seconds2 = time (NULL);

        cout << difftime(seconds2, seconds) << " s consumed for computation" << endl << endl;

    } else {
        trace("Cannot compute number of strategies, since the given OG is not acyclic\n\n");
    }
}


//! \brief check whether an OG is acyclic
//! \param OG an og to compute the be checked for acyclicity
//! \param graphName a string with the name of the file, the og was taken from
void checkAcyclicity(AnnotatedGraph* OG, string graphName) {

    trace("Processing: ");
    trace(graphName);
    trace("\n");

    trace(TRACE_1, "Checking if the OG is acyclic\n");

    if (OG->isAcyclic()) {
        trace("The given OG is acyclic\n\n");
    } else {
        trace("The given OG is NOT ayclic\n\n");
    }
}


// **********************************************************************************
// **********************************************************************************
// ********                               MAIN                               ********
// **********************************************************************************
// **********************************************************************************


int main(int argc, char** argv) {

//	bool readExpliciteOG = false;
//	parse_command_line(argc, argv);
//	if (readExpliciteOG){
//		readog(ogfileToMatch);
//		trace(TRACE_0, "HIT A KEY TO CONTINUE"); getchar();
//		NewLogger::printall();
//	}
//	else{
//		list<std::string>::iterator netiter = netfiles.begin();
//		Exchangeability* bdd = new Exchangeability(*netiter);
//		bdd->reorder();
//		trace(TRACE_0, "HIT A KEY TO CONTINUE"); getchar();
//		bdd->printMemoryInUse();
//	}
//	return 0;


/*    GraphFormulaCNF* annotation = new GraphFormulaCNF();
	GraphFormulaMultiaryOr* myclause = new GraphFormulaMultiaryOr();
	
    GraphFormulaLiteral* myliteral = new GraphFormulaLiteral("1-1stLiteral");
	myclause->addSubFormula(myliteral);
	
//	myliteral = new GraphFormulaLiteral("1-2ndLiteral");
//	myclause->addSubFormula(myliteral);
//	
//	myliteral = new GraphFormulaLiteral("1-3rdLiteral");
//	myclause->addSubFormula(myliteral);
//	
//	myliteral = new GraphFormulaLiteral("1-4thLiteral");
//	myclause->addSubFormula(myliteral);

	annotation->addClause(myclause);

	cout << "anno size: " << annotation->size() << endl;
	cout << "annotation: " << annotation->asString() << endl;
	
	myclause = new GraphFormulaMultiaryOr();
	
	myliteral = new GraphFormulaLiteral("2-1stLiteral");
	myclause->addSubFormula(myliteral);
	
	myliteral = new GraphFormulaLiteral("2-2ndLiteral");
	myclause->addSubFormula(myliteral);
	
	myliteral = new GraphFormulaLiteral("2-3rdLiteral");
	myclause->addSubFormula(myliteral);
	
	myliteral = new GraphFormulaLiteral("2-4thLiteral");
	myclause->addSubFormula(myliteral);

	annotation->addClause(myclause);
	
	cout << "annotation: " << annotation->asString() << endl;

	cout << "anno size: " << annotation->size() << endl;
	annotation->removeLiteralForReal("2-1stLiteral");
	
	cout << "annotation: " << annotation->asString() << endl;
	cout << "anno size: " << annotation->size() << endl;
	
	return 0; */

    AnnotatedGraph* OGToMatch = NULL;
    set_new_handler(&myown_newhandler);

    // evaluate command line options
    parse_command_line(argc, argv);


    // **********************************************************************************
    // ********                start OG file dependant operations                ********
    // ********                       (all OGs read first)                       ********
    // **********************************************************************************

    if (options[O_PRODUCTOG] || options[O_SIMULATES_WITH_COV] || parameters[P_READ_OG] ||
        options[O_SIMULATES] || (options[O_EX] && !options[O_BDD]) || options[O_FILTER]) {

        // reading all OG-files
        AnnotatedGraph::ogs_t OGsFromFiles;
        readAllOGs(OGsFromFiles);

#ifdef YY_FLEX_HAS_YYLEX_DESTROY
        // Destroy buffer of OG parser.
        // Must NOT be called before fclose(og_yyin);
        og_yylex_destroy();
#endif

        if (parameters[P_READ_OG]) {
            trace(TRACE_0, "OG was read from file\n");
            trace(TRACE_0, "HIT A KEY TO CONTINUE"); getchar();

            // only print OG size information
#ifdef LOG_NEW
            NewLogger::printall();
#endif
            return 0;
        }

        if (options[O_PRODUCTOG]) {
            // calculating the product OG
            computeProductOG(OGsFromFiles);
            deleteOGs(OGsFromFiles);
            return 0;
        }

        if (options[O_SIMULATES]) {
            // simulation on AnnotatedGraph
            checkSimulation(OGsFromFiles);
            // deleteOGs(OGsFromFiles);
            // return 0;
        }

        if (options[O_SIMULATES_WITH_COV]) {
            // simulation on AnnotatedGraph
            checkCovSimulation(OGsFromFiles);
            //deleteOGs(OGsFromFiles);
            return 0;
        }

        if (options[O_EX] && !options[O_BDD]) {
            // equivalence on (explicit representation of) operating guidelines
            checkEquivalence(OGsFromFiles);
            return 0;
        }

        if (options[O_FILTER]) {
            // filtration on OG
            filterOG(OGsFromFiles);
            return 0;
        }
    }

    // **********************************************************************************
    // ********                start OG file dependant operations                ********
    // ********                  (OGs read one after the other)                  ********
    // **********************************************************************************

    else if (parameters[P_PV] || options[O_COUNT_SERVICES] || options[O_CHECK_ACYCLIC]) {

        // Abort if there are no OGs at all
        if (ogfiles.begin() == ogfiles.end() && !(parameters[P_PV])) {
            trace("Error:  No OGs have been given for computation\n\n");
            trace("        Enter \"fiona --help\" for more information\n\n");        
        }

        // iterate all input files
        for (AnnotatedGraph::ogfiles_t::const_iterator iOgFile = ogfiles.begin();
             iOgFile != ogfiles.end(); ++iOgFile) {

            AnnotatedGraph* readOG = readog(*iOgFile);

            if (parameters[P_PV]) {
                // computes a service automaton "public view" which has the same
                // OG as given in readOG
                computePublicView(readOG, (*iOgFile));
                delete readOG;
            }

            if (options[O_COUNT_SERVICES]) {
                // counts the number of deterministic strategies
                // that are characterized by a given OG
                countStrategies(readOG, (*iOgFile));
                delete readOG;
            }

            if (options[O_CHECK_ACYCLIC]) {
                // counts the number of deterministic strategies
                // that are characterized by a given OG
                checkAcyclicity(readOG, (*iOgFile));
                delete readOG;
            }
        }
#ifdef YY_FLEX_HAS_YYLEX_DESTROY
        // Destroy buffer of OG parser.
        // Must NOT be called before fclose(og_yyin);
        og_yylex_destroy();
#endif
        if (!parameters[P_PV]) {
            return 0;
        }
    }

    // **********************************************************************************
    // ********                start PN file dependant operations                ********
    // **********************************************************************************

    // netfiles only used for getting the correct name strings
    if (options[O_EX] && options[O_BDD]) {
        // checking exchangeability using BDDs

        assert(netfiles.size() == 2);
        // the BDD representations of the given nets are assumed to exist already

        list<std::string>::iterator netiter = netfiles.begin();
        Exchangeability* og1 = new Exchangeability(*netiter);
        Exchangeability* og2 = new Exchangeability(*(++netiter));

        trace(TRACE_0, "The two operating guidelines are equal: ");
        if (og1->check(og2) == true) {
            trace(TRACE_0, "YES\n\n");
        } else {
            trace(TRACE_0, "NO\n\n");
        }
        return 0;
    }

    if (parameters[P_IG] || parameters[P_OG] || options[O_MATCH] || options[O_PNG] || parameters[P_PV]) {

        if (options[O_MATCH]) {
            assert(ogfiles.size() == 1);
            // we match multiple oWFNs with one OG,
            // so read the og first, then iterate over the nets
            OGToMatch = readog(*(ogfiles.begin()));
        }

        string fileName;	// name of og-file
        int loop = 0;		// in case the option -t eqR is set, we need each netfile to be processed twice
        ogfiles.clear();
        
        if (!options[O_EQ_R]) {		// option is not set, so we don't do the loop
        	loop = 27;				// loop needs to be higher than 1 ;-)
        }
        
        // ---------------- processing every single net -------------------
        for (list<std::string>::iterator netiter = netfiles.begin();
             netiter != netfiles.end(); ++netiter) {

        	// calculate the graph of the same net twice --> once with -R and once with no -R
        	// in case the option -t eqR is set, otherwise we go through this loop only once
	        do {
	            numberOfEvents = 0;
	            numberOfDecodes = 0;
	            garbagefound = 0;
	            global_progress = 0;
	            show_progress = 0;
	            State::state_count = 0; // number of states
	            State::state_count_stored_in_binDec = 0; // number of states
	            numberDeletedVertices = 0;
	
	            currentowfnfile = *netiter;
	            assert(currentowfnfile != "");
	
	            // prepare getting the net
	            PlaceTable = new SymbolTab<PlSymbol>;
	
	            // get the net
	            readnet(currentowfnfile);
	            trace(TRACE_0, "=================================================================\n");
	            trace(TRACE_0, "processing net " + currentowfnfile + "\n");
	            reportNet();
	            delete PlaceTable;

                    if (currentowfnfile == "<stdin>") currentowfnfile = "stdin";
	
	            // start computation
	            fileName = "";		// name of computed og-file
	            
	            if (parameters[P_IG]) {
	                // computing IG of the current oWFN
	                reportOptionValues(); // adjust events_manual and print limit of considering events
	                fileName = computeIG(PN);
	            }
	
	            if (parameters[P_OG] || parameters[P_PV]) {
	                // computing OG of the current oWFN
	                reportOptionValues(); // adjust events_manual and print limit of considering events
	                fileName = computeOG(PN);
	            }
	
	            if (options[O_MATCH]) {
	                // matching the current oWFN against the single OG 
	                checkMatching(OGToMatch, PN);
	            }
	
	            if (options[O_PNG]) {
	                // create a png file of the given net
	                makePNG(PN);
	            }
	
	            if (options[O_EQ_R]) {
	            	// reverse reduction mode for the next loop
		        options[O_CALC_ALL_STATES] = !options[O_CALC_ALL_STATES];
		        // remember file name of og-file to check equivalence later on
	            	ogfiles.push_back(AnnotatedGraph::addOGFileSuffix(fileName));
	            }

	            //delete PN;
       	            //trace(TRACE_5, "net deleted\n");

       	            loop++;
	        } while (loop <= 1);	// calculate the graph of the same net twice --> once with -R and once with no -R
	        
	        if (options[O_EQ_R]) {
	            // reading all og-files
	            AnnotatedGraph::ogs_t OGsFromFiles;
	            readAllOGs(OGsFromFiles);
	            
	            // check equivalence of both graphs
	            checkEquivalence(OGsFromFiles);
	            
	            OGsFromFiles.clear();
	            loop = 0;
	            ogfiles.clear();
	        }
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
