/*****************************************************************************
 * Copyright 2005, 2006, 2007 Peter Massuthe, Daniela Weinberg,              *
 *           Jan Bretschneider, Christian Gierds, Leonard Kern               *
 *                                                                           *
 * Copyright 2008                                                            *
 *   Peter Massuthe, Daniela Weinberg                                        *
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
#include "GastexGraph.h"
#include "owfn.h"
#include "state.h"
#include "IG.h"
#include "OG.h"
#include "Exchangeability.h"
#include "options.h"
#include "debug.h"
#include "main.h"
#include "adapter.h"
#include "dot2tex.h"

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

extern int dot_yylineno;
extern int dot_yydebug;
extern int dot_yy_flex_debug;
extern FILE* dot_yyin;
extern int dot_yyerror();
extern int dot_yyparse();

#ifdef YY_FLEX_HAS_YYLEX_DESTROY
extern int dot_yylex_destroy();
#endif

/// Deletes all OGs in 'OGsFromFiles' and clears list.
void deleteOGs(AnnotatedGraph::ogs_t& OGsFromFiles);

extern unsigned int State::state_count;
extern unsigned int State::state_count_stored_in_binDec;
extern std::list<std::string> netfiles;
extern std::list<std::string> ogfiles;
extern AnnotatedGraph* OGToParse;

extern string STG2oWFN_main(vector<string> &, string, set<string>&, set<string>&);

// will be filled during parsing
extern GasTexGraph* gastexGraph;

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

    cerr << "Reading PN file:" << owfnfile << endl;
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

    cerr << "Allocating memory..." << endl;

    PN = new oWFN();

    cerr << "Assigning filename." << endl;
    if (owfnfile == "<stdin>") {
        PN->filename = "stdin";
    }
    else PN->filename = owfnfile;

    cerr << "Preparing to parse" << endl;

    owfnfileToParse = owfnfile;

    cerr << "Parse now." << endl;
    owfn_yyparse();

    cerr << "Parsing complete." << endl;
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
    } else if (PN->FinalMarkingList.size() > 0) {
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
    trace(TRACE_1, "reading from file " + ogfile + "\n");
    og_yyin = fopen(ogfile.c_str(), "r");
    if (!og_yyin) {
        cerr << "cannot open OG file '" << ogfile << "' for reading'\n" << endl;
        exit(4);
    }
    OGToParse = new AnnotatedGraph();
    ogfileToParse = ogfile; // for debug - declaration in debug.cc
    og_yyparse();
    fclose(og_yyin);

    OGToParse->setFilename(ogfile);
    trace(TRACE_1, "file successfully read\n");

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

    trace(TRACE_5, "-E option found: ");
    if (options[O_READ_EVENTS]) {
        trace(TRACE_5, "yes\n");
    } else {
        trace(TRACE_5, "no\n");
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
            trace(TRACE_0, "    !" + string(PN->getInputPlace(e)->name));
            if (PN->getInputPlace(e)->max_occurrence >= 0) {
                trace(TRACE_0, "    (max. " + intToString(PN->getInputPlace(e)->max_occurrence) + "x)\n");
            } else {
                trace(TRACE_0, "    (no limit)\n");
            }
        }
        trace(TRACE_0, "    receiving events:\n");
        for (unsigned int e = 0; e < PN->getOutputPlaceCount(); e++) {
            trace(TRACE_0, "    ?" + string(PN->getOutputPlace(e)->name));
            if (PN->getOutputPlace(e)->max_occurrence >= 0) {
                trace(TRACE_0, "    (max. " + intToString(PN->getOutputPlace(e)->max_occurrence) + "x)\n");
            } else {
                trace(TRACE_0, "    (no limit)\n");
            }
        }
    }

    // report message bound
    if (options[O_MESSAGES_MAX]) {
        trace(TRACE_0, "interface message bound set to: "
                        + intToString(messages_manual) + "\n");
    }
    
    if (parameters[P_RESPONSIVE]) {
    	trace(TRACE_0, "\ncalculation of responsive partner(s)\n");
    }
    
    trace(TRACE_0, "\n");
}


 // **********************************************************************************
// *******                    mode dependent functions                       ********
// **********************************************************************************


//! \brief create a GasTex file of annotated dot file 'myDotFile'
//! \param myDotFile the dot file with layout annotations
//! \param myFilePrefix the file prefix to add '.tex' to
void makeGasTex(std::string myDotFile, std::string myFilePrefix) {
    trace(TRACE_1, "makeGasTex called for file: ");
    trace(TRACE_1, myDotFile + "\n");
    trace(TRACE_1, "prefix of the file is: ");
    trace(TRACE_1, myFilePrefix + "\n");

    string dotFileName = myDotFile;

    dot_yylineno = 1;
    dot_yydebug = 0;

    // try to open the dot file for reading
    dot_yyin = fopen((dotFileName).c_str(), "r");
    if (!dot_yyin) {
        cerr << "cannot open dot file '" << dotFileName << "' for reading'\n" << endl;
        exit(4);
    }

    // clear the graph holding the parsed informations
    if (gastexGraph) {
        delete gastexGraph;
    }

    // preparing the global variable gastexGraph for parsing
    gastexGraph = new GasTexGraph();

    // parsing the dot file into variable gastexGraph
    dot_yyparse();
    fclose(dot_yyin);

    string texFileName = myFilePrefix + ".tex";

    // writing gastexGraph to new tex file
    gastexGraph->makeGasTex(texFileName);

    trace(TRACE_0, texFileName + " generated\n");
}


//! \brief create a GasTex file of the given IG/OG
//! \param graph the IG/OG to generate a GasTex file from
void makeGasTex(CommunicationGraph* graph) {

    trace(TRACE_1, "Internal translation of the graph into GasTex format...\n");

    string outfilePrefixWithOptions = options[O_OUTFILEPREFIX] ? outfilePrefix : graph->getFilename();

    if (!options[O_CALC_ALL_STATES]) {
        outfilePrefixWithOptions += ".R";
    }

    if (parameters[P_DIAGNOSIS]) {
        outfilePrefixWithOptions += ".diag";
    } else {
        if (parameters[P_OG]) {
            outfilePrefixWithOptions += ".og";
        } else {
            outfilePrefixWithOptions += ".ig";
        }
    }

    string outFileName = outfilePrefixWithOptions + ".dot";

    dot_yylineno = 1;
    dot_yydebug = 0;

    // try to open the dot file for reading
    dot_yyin = fopen((outFileName).c_str(), "r");
    if (!dot_yyin) {
        cerr << "cannot open graph file '" << outFileName << "' for reading'\n" << endl;
        exit(4);
    }

    // clear the graph holding the parsed informations
    if (gastexGraph) {
        delete gastexGraph;
    }

    // preparing the global variable gastexGraph for parsing
    gastexGraph = new GasTexGraph();

    // parsing the dot file into variable gastexGraph
    dot_yyparse();

    fclose(dot_yyin);

    string texFileName = outfilePrefixWithOptions + ".tex";

    // writing gastexGraph to new tex file
    gastexGraph->makeGasTex(texFileName);

    trace(TRACE_0, texFileName + " generated\n");
}


//! \brief create a GasTex file of the given IG/OG
//! \param graph the IG/OG to generate a GasTex file from
//! \param fileNamePrefix the filenameprefix for adding .dot and .tex, resp.
//!        fileNamePrefix.dot is assumed to exist (as a layout annotated dot file)
void makeGasTex(AnnotatedGraph* graph, string fileNamePrefix) {

    trace(TRACE_1, "Internal translation of the graph into GasTex format...\n");

    string dotFileName = fileNamePrefix + ".dot";
    string texFileName = fileNamePrefix + ".tex";

    dot_yylineno = 1;
    dot_yydebug = 0;

    // try to open the dot file for reading
    dot_yyin = fopen((dotFileName).c_str(), "r");
    if (!dot_yyin) {
        cerr << "cannot open annotated dot file '" << dotFileName << "' for reading'\n" << endl;
        exit(4);
    }

    // clear the graph holding the parsed informations
    if (gastexGraph) {
        delete gastexGraph;
    }

    // preparing the global variable gastexGraph for parsing
    gastexGraph = new GasTexGraph();

    // parsing the dot file into variable gastexGraph
    dot_yyparse();

    fclose(dot_yyin);

    // writing gastexGraph to new tex file
    gastexGraph->makeGasTex(texFileName);

    trace(TRACE_0, texFileName + " generated\n");
}


//! \brief generate a public view for a given og
//! \param OG an og to generate the public view of
//! \param graphName a name for the graph in the output
void computePublicView(AnnotatedGraph* OG, string graphName, bool fromOWFN) {

    // if the OG is empty, there is no public view and the computation
    // will be aborted
    if(OG->hasNoRoot() || OG->getRoot()->getColor() == RED) {
        if (fromOWFN) {
            trace(TRACE_0,"\nThe given oWFN is not controllable. No PublicView will be generated.\n\n");
        } else {
            trace(TRACE_0,"\nThe given OG describes no partners. No PublicView will be generated.\n\n");
        }
        return;
    }

    trace(TRACE_0, "generating the public view for "); 
    trace(graphName);
    
    trace("\n");

    // dont create the public view automaton's png if it becomes to big
    unsigned int maxSizeForDot = 150;

    outfilePrefix = AnnotatedGraph::stripOGFileSuffix(graphName);
    outfilePrefix += ".pv.sa";

    // create an empty graph to be made the public view
    Graph* cleanPV = new Graph();

    // give the graph to the OG to be generated from
    OG->transformToPublicView(cleanPV, fromOWFN);

    // generate output files
    if (!options[O_NOOUTPUTFILES]) {

        // test whether this graph is too big
        if(cleanPV->numberOfNodes() > maxSizeForDot && !parameters[P_NOPNG]) {
            trace(TRACE_0, "the public view service automaton is to big to generate a dot file\n\n");
        } else {
            trace(TRACE_0, "generating dot output...\n");

            // .out
            cleanPV->printDotFile(outfilePrefix, "public view of " + graphName);
        }

        // create sets for transferring the interface from the original owfn
        // to the newly created one. If the original owfn is not known, these
        // sets will stay empty
        set<string> inputs;
        set<string> outputs;

        // Only fill those sets if the original interface is known
        if (fromOWFN) {
            CommunicationGraph* casted = dynamic_cast<CommunicationGraph*>(OG);
            casted->returnInterface(inputs,outputs);
        }

        //transform to owfn
        PNapi::PetriNet* PVoWFN = new PNapi::PetriNet();
        PVoWFN->set_format(PNapi::FORMAT_OWFN, true);
        cleanPV->transformToOWFN(PVoWFN, inputs, outputs);

        // print the information of the public view's owfn
        trace(TRACE_1, "Public View oWFN statistics:\n");
        trace(PVoWFN->information());
        trace(TRACE_1, "\n");

        // create the stream for the owfn output
        ofstream output;
        const string owfnOutput = AnnotatedGraph::stripOGFileSuffix(graphName) + ".pv.owfn";
        output.open (owfnOutput.c_str(),ios::out);

        (output) << (*PVoWFN);
        output.close();

        trace(TRACE_0, "=================================================================\n");
        trace(TRACE_0, "\n");
    }
}


//! \brief create an IG of an oWFN
//! \param PN pointer to the given oWFN whose IG will be computed
//! \param igFilename reference to a string containing the filename of the created IG
interactionGraph* computeIG(oWFN* PN, string& igFilename) {

    time_t seconds, seconds2;
    interactionGraph * graph = new interactionGraph(PN);
    igFilename = "";


    // print information about reduction rules (if desired)
    if (options[O_CALC_REDUCED_IG] || parameters[P_USE_EAD]) {
        trace(TRACE_0, "building the reduced interaction graph by using reduction rule(s)\n");
        if (parameters[P_USE_CRE]) {
            trace(TRACE_0, "     \"combine receiving events\"\n");
        }
        if (parameters[P_USE_RBS]) {
            trace(TRACE_0, "     \"receiving before sending\"\n");
        }
        if (parameters[P_USE_EAD]) {
            trace(TRACE_0, "     \"early detection\"\n");
        }
    } else {
        trace(TRACE_0, "building the interaction graph...\n");
    }
    trace(TRACE_0, "\n");

    
    // build interaction graph
    seconds = time (NULL);
    graph->buildGraph();
    seconds2 = time (NULL);

    if (options[O_CALC_REDUCED_IG]) {
        trace(TRACE_0, "building the reduced interaction graph finished.\n");
    } else {
        trace(TRACE_0, "\nbuilding the interaction graph finished.\n");
    }
    cout << difftime(seconds2, seconds) << " s consumed for building graph" << endl;


    // add interface information to graph: Input -> Output and vice versa
    // input/outputPlacenames contain all possible labels for the IG
    for (unsigned int i = 0; i < PN->getInputPlaceCount(); i++) {
        graph->outputPlacenames.insert( (PN->getInputPlace(i))->name );
    }
    for (unsigned int i = 0; i < PN->getOutputPlaceCount(); i++) {
        graph->inputPlacenames.insert( PN->getOutputPlace(i)->name );
    }


    // print statistics
    trace(TRACE_0, "\nnet is controllable: ");
    if (graph->getRoot()->getColor() == BLUE) {
        trace(TRACE_0, "YES\n\n");
    } else {
        trace(TRACE_0, "NO\n\n");
    }

    trace(TRACE_0, "IG statistics:\n");
    graph->printGraphStatistics();
    trace(TRACE_0, "\n");


    // create output files if needed
    if (!options[O_NOOUTPUTFILES]) {
        if (parameters[P_DIAGNOSIS]) {
            graph->diagnose();
        }

        if (!parameters[P_EQ_R]) {    // don't create png if we are in eqr mode
            // generate output files
            graph->printGraphToDot(); // .out, .png

            if (parameters[P_TEX]) {
                makeGasTex(graph);
            }
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

        // the second parameter is true, since the oWFN this IG was generated
        // from still exists and additional information are available
        graph->printOGFile(igFilename, true);
    }

    return graph;
}


string computeIG(oWFN* PN) {

    string igFilename = "";
    interactionGraph* graph = computeIG(PN, igFilename);

    trace(TRACE_5, "computation finished -- trying to delete graph\n");

    graph->clearNodeSet();
    delete graph;

    trace(TRACE_5, "graph deleted\n");
    trace(TRACE_0, "=================================================================\n");
    trace(TRACE_0, "\n");

    return igFilename;
}


//! \brief creates a small partner out of a given IG
//! \param IG a pointer to an IG of an oWFN whose small partner will be computed
//! \return a string containing the filename of the partner owfn
string computeSmallPartner(AnnotatedGraph* IG) {

    trace(TRACE_5, "string computeSmallPartner(AnnotatedGraph*) : start\n");
    trace(TRACE_0, "\n\nComputing partner oWFN \n");

    // 1. step: was done before (computing the IG)
    // 2. step: if the net is controllable create STG file for petrify out of computed IG
    string stgFilename = "";
    vector<string> edgeLabels; // renamend transitions 

    if (!IG->hasNoRoot() && IG->getRoot()->getColor() == BLUE) {
        trace(TRACE_1, "    Creating STG File\n");
        stgFilename = IG->printGraphToSTG(edgeLabels);
    } else {
        trace(TRACE_0, "\n    Cannot synthesize a partner for a net, that is not controllable\n\n");
        return "";
    }


    // 3. step: invoke petrify on created STG file
    // prepare petrify command line and execute system command if possible
    string pnFilename = stgFilename.substr(0, stgFilename.size() - 4) + ".pn"; // change .stg to .pn

    string systemcall = string(HAVE_PETRIFY) + " " + stgFilename + " -dead -ip -nolog -o " + pnFilename;
    if (HAVE_PETRIFY != "not found") {
        trace(TRACE_1, "    Calling Petrify.\n");
        trace(TRACE_2, "        " + systemcall + "\n");
        system(systemcall.c_str());
    } else {
        trace(TRACE_0, "cannot execute command as Petrify was not found in path\n");
        return "";
    }


    // 4. step: create oWFN out of the petrinet computed by petrify
    // 5. step: complete interface with the interface information stored in the
    // IG (done in STG2oWFN_main
    trace(TRACE_1, "    Converting petrify result to petri net.\n");
    string owfnFilename = STG2oWFN_main( edgeLabels, pnFilename, IG->inputPlacenames, IG->outputPlacenames );


    trace(TRACE_0, "Partner synthesis completed. Created file: " + owfnFilename + "\n");
    trace(TRACE_5, "string computeSmallPartner(AnnotatedGraph*) : end\n");

    return owfnFilename; // return partner filename
}


//! \brief creates a small partner out of a given oWFN
//! \param PN a pointer to an oWFN whose small partner will be computed
//! \return a string containing the filename of the computed IG from given oWFN
string computeSmallPartner(oWFN* givenPN) {

    // 1. step: compute the IG of given net
    string igFilename = "";
    interactionGraph* graph = computeIG(givenPN, igFilename);


    // 2.-4. step wil be done with former computed IG (ignoring partner filename)
    computeSmallPartner(graph);


    // garbage collection
    delete graph;

    return igFilename;
}


//! \brief creates a small partner out of a given IG
//! \param IG a pointer to an IG of an oWFN whose small partner will be computed
//! \return a string containing the filename of the partner owfn.
string computeMostPermissivePartner(AnnotatedGraph* OG) {

    trace(TRACE_5, "string computeMostPermissivePartner(AnnotatedGraph*) : start\n");
    trace(TRACE_0, "\n\nComputing partner oWFN \n");

    // 1. step: was done before (computing the OG)
    // 2. step: if the net is controllable create STG file for petrify out of computed IG
    string stgFilename = "";
    vector<string> edgeLabels; // renamend transitions 

    if (!OG->hasNoRoot() && OG->getRoot()->getColor() == BLUE) {
        trace(TRACE_1, "    Creating STG File\n");
        stgFilename = OG->printGraphToSTG(edgeLabels);
    } else {
        trace(TRACE_0, "\nCannot synthesize a partner for a net, that is not controllable\n\n");
        return "";
    }


    // 3. step: invoke petrify on created STG file
    // prepare petrify command line and execute system command if possible
    string pnFilename = stgFilename.substr(0, stgFilename.size() - 4) + ".pn"; // change .stg to .pn

    string systemcall = string(HAVE_PETRIFY) + " " + stgFilename + " -dead -ip -nolog -o " + pnFilename;
    if (HAVE_PETRIFY != "not found") {
        trace(TRACE_1, "    Calling Petrify.\n");
        trace(TRACE_2, "        " + systemcall + "\n");
        system(systemcall.c_str());
    } else {
        trace(TRACE_0, "cannot execute command as Petrify was not found in path\n");
        return "";
    }


    // 4. step: create oWFN out of the petrinet computed by petrify
    trace(TRACE_1, "    Converting petrify result to petri net.\n");
    string owfnFilename = STG2oWFN_main( edgeLabels, pnFilename, OG->inputPlacenames, OG->outputPlacenames );

    trace(TRACE_0, "Partner synthesis completed. Created file: " + owfnFilename + "\n");
    trace(TRACE_5, "string computeMostPermissivePartner(AnnotatedGraph*) : end\n");

    return owfnFilename; // return partner filename
}


//! \brief create an OG of an oWFN
//! \param PN the given oWFN
string computeOG(oWFN* PN) {

    time_t  buildGraphTime1, buildGraphTime2,
            seconds, seconds2,
            graphStatsTime1, graphStatsTime2,
            removeFalseNodesTime1, removeFalseNodesTime2,
            removeUnreachableNodesTime1, removeUnreachableNodesTime2;

    OG* graph = new OG(PN);
    bool controllable = false;
    string ogFilename = "";

    trace(TRACE_0, "building the operating guideline...\n");
    graph->printProgressFirst();

    seconds = time(NULL);

    // build operating guideline
    trace(TRACE_1, "Building the graph...\n");
    buildGraphTime1 = time(NULL);
    graph->buildGraph();
    buildGraphTime2 = time(NULL);
    trace(TRACE_1, "finished building the graph\n");
    trace(TRACE_2, "    " + intToString((int) difftime(buildGraphTime2, buildGraphTime1)) + " s consumed.\n\n");

    // add interface information to graph: Input -> Output and vice versa
    // input/outputPlacenames contain all possible labels for the IG
    for (unsigned int i = 0; i < PN->getInputPlaceCount(); i++) {
        graph->outputPlacenames.insert( (PN->getInputPlace(i))->name );
    }
    for (unsigned int i = 0; i < PN->getOutputPlaceCount(); i++) {
        graph->inputPlacenames.insert( PN->getOutputPlace(i)->name );
    }

    // print statistics
    trace(TRACE_1, "computing graph statistics...\n");
    graphStatsTime1 = time(NULL);
    graph->computeGraphStatistics();
    graphStatsTime2 = time(NULL);
    trace(TRACE_1, "finished computing graph statistics\n");
    trace(TRACE_2, "    " + intToString((int) difftime(graphStatsTime2, graphStatsTime2)) + " s consumed.\n\n");


    if (!parameters[P_SHOW_RED_NODES] && !parameters[P_SHOW_ALL_NODES]) {
        trace(TRACE_1, "removing false nodes...\n");
        removeFalseNodesTime1 = time(NULL);
        graph->removeReachableFalseNodes();
        removeFalseNodesTime2 = time(NULL);
        trace(TRACE_1, "finished removing false nodes\n");
        trace(TRACE_2, "    " + intToString((int) difftime(removeFalseNodesTime2, removeFalseNodesTime1)) + " s consumed.\n\n");

        trace(TRACE_1, "removing unreachable nodes...\n");
        removeUnreachableNodesTime1 = time(NULL);
        graph->removeUnreachableNodes();
        removeUnreachableNodesTime2 = time(NULL);
        trace(TRACE_1, "finished removing unreachable nodes\n");
        trace(TRACE_2, "    " + intToString((int) difftime(removeUnreachableNodesTime2, removeUnreachableNodesTime2)) + " s consumed.\n\n");
    }


    seconds2 = time(NULL);

    trace(TRACE_0, "\nbuilding the operating guideline finished.\n");
    trace(TRACE_0, "    " + intToString((int) difftime(seconds2, seconds)) + " s overall consumed for OG computation.\n\n");

    trace(TRACE_0, "\nnet is controllable: ");
    if (graph->hasNoRoot() || graph->getRoot()->getColor()==RED) {
        trace(TRACE_0, "NO\n\n");
    } else {
        trace(TRACE_0, "YES\n\n");
        controllable = true;
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

        computePublicView(graph, publicViewName, true);

        delete graph;
        return "";
    }

    // generate output files
    if (!options[O_NOOUTPUTFILES]) {
        // distributed controllability?
        if (parameters[P_DISTRIBUTED]) {
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

        if (!parameters[P_EQ_R]) {    // don't create png if we are in eqr mode
             graph->printGraphToDot(); // .out, .png

            if (parameters[P_TEX]) {
                makeGasTex(graph);
            }
        }

        if (options[O_OUTFILEPREFIX]) {
            ogFilename = outfilePrefix;
        } else {
            ogFilename = graph->getFilename();
        }

        if (!options[O_CALC_ALL_STATES]) {
            ogFilename += ".R";
        }

        // the second parameter is true, since the oWFN this OG was generated
        // from still exists and additional information are available
        graph->printOGFile(ogFilename, true);
        //vector<string> edgeLabels;                        // renamend transitions
        if (parameters[P_SYNTHESIZE_PARTNER_OWFN]) {
            computeMostPermissivePartner(graph);
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

        // the second parameter is false, since this OG has no underlying oWFN
        productOG->printOGFile(outfilePrefix, false);
        productOG->printDotFile(outfilePrefix);
        trace("\n");
    }

    delete productOG;
}


//! \brief match a net against an og
//! \param OGToMatch an og to be matched against
//! \param PN an oWFN to matched against an og
void checkMatching(AnnotatedGraph* OGToMatch, oWFN* PN) {
    string reasonForFailedMatch;
    trace(TRACE_0, ("matching " + PN->filename + " with " + (OGToMatch->getFilename())+ "...\n\n"));
    if (PN->matchesWithOG(OGToMatch, reasonForFailedMatch)) {
        trace(TRACE_1, "\n");
        trace(TRACE_0, "oWFN matches with OG: YES\n\n");
    } else {
        trace(TRACE_1, "\n");
        trace(TRACE_0, "Match failed: " +reasonForFailedMatch + "\n\n");
        trace(TRACE_0, "oWFN matches with OG: NO\n\n");
    }
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

    time_t  buildGraphTime1, buildGraphTime2,
            seconds, seconds2,
            graphStatsTime1, graphStatsTime2,
            removeFalseNodesTime1, removeFalseNodesTime2,
            removeUnreachableNodesTime1, removeUnreachableNodesTime2;

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

        seconds = time(NULL);

        trace(TRACE_1, "Building the graph...\n");
        buildGraphTime1 = time(NULL);
        graph->buildGraph(); // build operating guideline
        buildGraphTime2 = time(NULL);
        trace(TRACE_1, "finished building the graph\n");
        trace(TRACE_2, "    " + intToString((int) difftime(buildGraphTime2, buildGraphTime1)) + " s consumed.\n");

        trace(TRACE_1, "computing graph statistics...\n");
        graphStatsTime1 = time(NULL);
        graph->computeGraphStatistics();
        graphStatsTime2 = time(NULL);
        trace(TRACE_1, "finished computing graph statistics\n");
        trace(TRACE_2, "    " + intToString((int) difftime(graphStatsTime2, graphStatsTime2)) + " s consumed.\n");


        if (!parameters[P_SHOW_RED_NODES] && !parameters[P_SHOW_ALL_NODES]) {
            trace(TRACE_1, "removing false nodes...\n");
            removeFalseNodesTime1 = time(NULL);
            graph->removeReachableFalseNodes();
            removeFalseNodesTime2 = time(NULL);
            trace(TRACE_1, "finished removing false nodes\n");
            trace(TRACE_2, "    " + intToString((int) difftime(removeFalseNodesTime2, removeFalseNodesTime1)) + " s consumed.\n");

            trace(TRACE_1, "removing unreachable nodes...\n");
            removeUnreachableNodesTime1 = time(NULL);
            graph->removeUnreachableNodes();
            removeUnreachableNodesTime2 = time(NULL);
            trace(TRACE_1, "finished removing unreachable nodes\n");
            trace(TRACE_2, "    " + intToString((int) difftime(removeUnreachableNodesTime2, removeUnreachableNodesTime2)) + " s consumed.\n");
        }


        seconds2 = time(NULL);

        trace(TRACE_0, "\nbuilding the operating guideline finished.\n");
        trace(TRACE_0, "    " + intToString((int) difftime(seconds2, seconds)) + " s overall consumed for OG computation.\n");

        // print statistics
        trace(TRACE_0, "OG statistics:\n");
        graph->printGraphStatistics();
        trace(TRACE_0, "\n");

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

    firstOG->removeReachableFalseNodes();
    secondOG->removeReachableFalseNodes();

    seconds = time (NULL);

    trace(TRACE_0, "\n=================================================================\n");
    trace(TRACE_0, "checking whether " + firstOG->getFilename() + " simulates " + secondOG->getFilename() + "...\n\n");
    if (firstOG->simulates(secondOG)) {
        trace(TRACE_1, "result: simulation holds: YES\n\n");
        trace(TRACE_0, "The first OG characterizes all strategies of the second one.\n\n");
    } else {
        trace(TRACE_1, "result: simulation holds: NO\n\n");
        trace(TRACE_0, "The second OG characterizes at least one strategy that is\n");
        trace(TRACE_0, "not characterized by the first one.\n\n");
    }

    seconds2 = time (NULL);
    cout << "    " << difftime(seconds2, seconds) << " s consumed for checking equivalence" << endl << endl;

    if (!calledWithNet) {
        trace(TRACE_0, "Attention: This result is only valid if the given OGs are complete\n");
        trace(TRACE_0, "           (i.e., \"-s empty\" option was set and \"-m\" option high enough)\n\n");
    } else if (netfiles.size() == 1) {
        trace(TRACE_0, "Attention: This result is only valid if the given OG is complete\n");
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

    if (!parameters[P_EQ_R]) {
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

    time_t  buildGraphTime1, buildGraphTime2,
            seconds, seconds2,
            graphStatsTime1, graphStatsTime2,
            removeFalseNodesTime1, removeFalseNodesTime2,
            removeUnreachableNodesTime1, removeUnreachableNodesTime2;

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

        seconds = time (NULL);

        // compute OG
        OG* graph = new OG(PN);

        trace(TRACE_0, "building the operating guideline...\n");
        graph->printProgressFirst();

        seconds = time(NULL);

        trace(TRACE_1, "Building the graph...\n");
        buildGraphTime1 = time(NULL);
        graph->buildGraph(); // build operating guideline
        buildGraphTime2 = time(NULL);
        trace(TRACE_1, "finished building the graph\n");
        trace(TRACE_2, "    " + intToString((int) difftime(buildGraphTime2, buildGraphTime1)) + " s consumed.\n\n");

        trace(TRACE_1, "computing graph statistics...\n");
        graphStatsTime1 = time(NULL);
        graph->computeGraphStatistics();
        graphStatsTime2 = time(NULL);
        trace(TRACE_1, "finished computing graph statistics\n");
        trace(TRACE_2, "    " + intToString((int) difftime(graphStatsTime2, graphStatsTime2)) + " s consumed.\n\n");


        if (!parameters[P_SHOW_RED_NODES] && !parameters[P_SHOW_ALL_NODES]) {
            trace(TRACE_1, "removing false nodes...\n");
            removeFalseNodesTime1 = time(NULL);
            graph->removeReachableFalseNodes();
            removeFalseNodesTime2 = time(NULL);
            trace(TRACE_1, "finished removing false nodes\n");
            trace(TRACE_2, "    " + intToString((int) difftime(removeFalseNodesTime2, removeFalseNodesTime1)) + " s consumed.\n\n");

            trace(TRACE_1, "removing unreachable nodes...\n");
            removeUnreachableNodesTime1 = time(NULL);
            graph->removeUnreachableNodes();
            removeUnreachableNodesTime2 = time(NULL);
            trace(TRACE_1, "finished removing unreachable nodes\n");
            trace(TRACE_2, "    " + intToString((int) difftime(removeUnreachableNodesTime2, removeUnreachableNodesTime2)) + " s consumed.\n\n");
        }


        seconds2 = time(NULL);

        trace(TRACE_0, "\nbuilding the operating guideline finished.\n");
        trace(TRACE_0, "    " + intToString((int) difftime(seconds2, seconds)) + " s overall consumed for OG computation.\n\n");

        seconds2 = time (NULL);
        cout << "    " << difftime(seconds2, seconds) << " s consumed for building graph" << endl << endl;

        // print statistics
        trace(TRACE_0, "OG statistics:\n");
        graph->printGraphStatistics();
        trace(TRACE_0, "\n");

        // add new OG to the list
        if (!OGfirst && netfiles.size() == 1) {
            OGsFromFiles.push_front(graph);
        } else {
            OGsFromFiles.push_back(graph);
        }
        graph->deleteOWFN();

        netiter++;
    }

    trace(TRACE_0, "=================================================================\n");
    trace(TRACE_0, "\n\nchecking equivalence of the OGs...\n");

    AnnotatedGraph::ogs_t::const_iterator currentOGfile = OGsFromFiles.begin();
    AnnotatedGraph *firstOG = *currentOGfile;
    AnnotatedGraph *secondOG = *(++currentOGfile);

    firstOG->removeReachableFalseNodes();
    secondOG->removeReachableFalseNodes();

    seconds = time (NULL);

    trace(TRACE_1, "checking whether " + firstOG->getFilename() + " is equivalent to " + secondOG->getFilename() + "\n");

    // Check the equivalence
    bool areEquivalent = firstOG->isEquivalent(secondOG);

    trace(TRACE_0, "\nresult: " + firstOG->getFilename() + " and " + secondOG->getFilename() + " are equivalent:");

    if (areEquivalent) {
        trace(TRACE_0, " YES\n\n");
    } else {
        trace(TRACE_0, " NO\n\n");
    }

    seconds2 = time (NULL);
    cout << "    " << difftime(seconds2, seconds) << " s consumed for checking equivalence" << endl << endl;

    if (!calledWithNet && !parameters[P_EQ_R]) {
        trace(TRACE_0, "Attention: This result is only valid if the given OGs are complete\n");
        trace(TRACE_0, "           (i.e., \"-s empty\" option was set and \"-m\" option high enough)\n\n");
    } else if (netfiles.size() == 1 && !parameters[P_EQ_R]) {
        trace(TRACE_0, "Attention: This result is only valid if the given OG is complete\n");
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
        globals::filename = PN->filename + " \\n Final Condition: "
                + PN->finalConditionString;
    } else {
        globals::filename = PN->filename + " \\n Final Marking: "
                + PN->finalMarkingString;
    }

    // set the output format to dot
    PNapiNet->set_format(PNapi::FORMAT_DOT, true);

    // determine the output file name
    string outFileName;
    if (!options[O_OUTFILEPREFIX]) {
        outFileName = globals::output_filename;
    } else {
        outFileName = outfilePrefix;
    }

    string dotFileName = outFileName + ".out";

    // create the output
    if (!options[O_NOOUTPUTFILES]) {

        // create the outputfilestream
        ofstream *out = new ofstream(dotFileName.c_str(), ofstream::out | ofstream::trunc | ofstream::binary);

        // check whether the stream was succesfully created
        if (!out->is_open()) {
            trace(TRACE_0, "File \"" + dotFileName + "\" could not be opened for writing access!\n");
            exit(-1);
        }

        // create the dot and write it to the file
        (*out) << (*PNapiNet);

        // close the outputfilestream
        (*out) << flush;
        (out)->close();
        delete(out);
        out = NULL;

        if (parameters[P_TEX]) {
            // annotate .dot file
            int exitvalue = system(("dot -Tdot " + dotFileName + " -o " + outFileName + ".dot").c_str());
            if (exitvalue == 0) {
                trace(TRACE_0, (outFileName + ".dot generated\n\n"));
            } else {
                trace(TRACE_0, "error: Dot exited with non zero value! here\n\n");
                exit(-1);
            }

            // transforming .dot file into gastex format
            makeGasTex((outFileName + ".dot"), (outFileName));
        }

        // call dot to create the png file
        int exitvalue = system(("dot -q -Tpng -o \"" + outFileName + ".png\" " + dotFileName).c_str());

        if (exitvalue == 0) {
            trace(TRACE_0, (outFileName + ".png generated\n\n"));
        } else {
            trace(TRACE_0, "error: Dot exited with non zero value! dort\n\n" + intToString(exitvalue));
        }
    }
}


//! \brief reduce the given oWFN
//! \param PN an oWFN to reduce
void reduceOWFN(oWFN* PN) {

    trace(TRACE_1, "Internal translation of the net into PNapi format...\n");

    // translate the net into PNapi format
    PNapi::PetriNet* PNapiNet = PN->returnPNapiNet();

    // set strings needed in PNapi output
    globals::output_filename = PN->filename;
    globals::filename = PN->filename;

    // set the output format to dot
    PNapiNet->set_format(PNapi::FORMAT_OWFN, true);

    trace(TRACE_1, "Performing structural reduction ...\n\n");

    // calling the reduce function of the pnapi with reduction level 5
    PNapiNet->reduce(globals::reduction_level);

    // Statistics of the reduced oWFN
    trace(TRACE_0, "Reduced oWFN statistics:\n");
    trace(PNapiNet->information());
    trace(TRACE_0, "\n\n");

    string outFileName;

    if (!options[O_OUTFILEPREFIX]) {
        outFileName = globals::output_filename;
    } else {
        outFileName = outfilePrefix;
    }

    // create the output
    if (!options[O_NOOUTPUTFILES]) {
        PNapiNet->set_format(PNapi::FORMAT_OWFN, true);

        ofstream output;
        const string owfnOutput = outFileName + ".reduced.owfn";
        output.open (owfnOutput.c_str(),ios::out);

        (output) << (*PNapiNet);
        output.close();
    }

    delete PNapiNet;
}


//! \brief normalize the given oWFN
//! \param PN an oWFN to normalize
void normalizeOWFN(oWFN* PN) {

    trace(TRACE_5, "void normalizeOWFN(oWFN*) : start\n");

    // normalize
    trace(TRACE_0, "Performing normalization ...\n\n");
    oWFN* normalOWFN = PN->returnNormalOWFN();


    // translate the net into PNapi format
    PNapi::PetriNet* PNapiNet = normalOWFN->returnPNapiNet();
    globals::output_filename = normalOWFN->filename; // set strings needed in PNapi output
    globals::filename = normalOWFN->filename;
    PNapiNet->set_format(PNapi::FORMAT_OWFN, true); // set the output format to dot


    // Statistics of the reduced oWFN
    trace(TRACE_0, "Normalized oWFN statistics:\n");
    trace(PNapiNet->information());
    trace(TRACE_0, "\n\n");


    // create the output
    if (!options[O_NOOUTPUTFILES]) {

        string outFileName;
        if (!options[O_OUTFILEPREFIX]) {
            outFileName = globals::output_filename;
        } else {
            outFileName = outfilePrefix;
        }

        ofstream output;
        const string owfnOutput = outFileName.erase((outFileName.size()-5), outFileName.size()) + ".normalized.owfn";
        output.open (owfnOutput.c_str(), ios::out);

        (output) << (*PNapiNet);
        output.close();
    }

    delete PNapiNet;
    delete normalOWFN;

    trace(TRACE_5, "void normalizeOWFN(oWFN*) : end\n");
}


//! \brief compute the number of tree service automata that match with the given OG
//! \param OG an og to compute the number of matching tree service automata for
//! \param graphName a string with the name of the file, the og was taken from
void countStrategies(AnnotatedGraph* OG, string graphName) {

    trace("Processing: ");
    trace(graphName);
    trace("\n");

    trace(TRACE_1, "Checking if the OG is acyclic\n");

    if (OG->isAcyclic()) {

        time_t seconds, seconds2;

        seconds = time (NULL);
        trace("Started computing the number of matching tree service automata\n");
        // Compute and show the number of Services
        long double computedNumber = OG->numberOfStrategies();

        stringstream longDoubleZahl;
        longDoubleZahl << computedNumber;
        if (computedNumber == 0) {
            trace("The computed number resulted 0, which implies that the service is not controllable.");
        } else if (longDoubleZahl.str() != "inf"){
            trace("Computed number of matching tree service automata: " + longDoubleZahl.str() + "\n");
        } else {
            trace("The computed number exceeds long double (approximately 10^4900)!\n");
        }
        seconds2 = time (NULL);

        cout << difftime(seconds2, seconds) << " s consumed for computation" << endl << endl;

    } else {
        trace("Cannot compute number of matching tree service automata, since the given OG is not acyclic\n\n");
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

    AnnotatedGraph* OGToMatch = NULL;
    set_new_handler(&myown_newhandler);

    // evaluate command line options
    parse_command_line(argc, argv);


    // **********************************************************************************
    // ********                start dot file dependant operations               ********
    // **********************************************************************************

    if (parameters[P_GASTEX]) {
        // if -t tex was called, then outfileprefix still contains file suffix

        // remember suffix in dotFileName
        string dotFileName = outfilePrefix;
        // try to remove .out suffix
        static const string dotFileSuffix = ".out";
        if (outfilePrefix.substr(outfilePrefix.size() - dotFileSuffix.size()) == dotFileSuffix) {
            outfilePrefix=outfilePrefix.substr(0, outfilePrefix.size() - dotFileSuffix.size());
        }
        makeGasTex(dotFileName, outfilePrefix);
    }

    // **********************************************************************************
    // ********                start OG file dependant operations                ********
    // ********                       (all OGs read first)                       ********
    // **********************************************************************************

    if (parameters[P_PRODUCTOG] ||
        parameters[P_SIMULATES] || (parameters[P_EX] && !options[O_BDD])) {// || parameters[P_READ_OG]) {

        // reading all OG-files
        AnnotatedGraph::ogs_t OGsFromFiles;
        readAllOGs(OGsFromFiles);

#ifdef YY_FLEX_HAS_YYLEX_DESTROY
        // Destroy buffer of OG parser.
        // Must NOT be called before fclose(og_yyin);
        og_yylex_destroy();
#endif

/*        if (parameters[P_READ_OG]) {
            trace(TRACE_0, "OG was read from file\n");
            trace(TRACE_0, "HIT A KEY TO CONTINUE"); getchar();

            // only print OG size information
#ifdef LOG_NEW
            NewLogger::printall();
#endif
            return 0;
        }
*/
        if (parameters[P_PRODUCTOG]) {
            // calculating the product OG
            computeProductOG(OGsFromFiles);
            deleteOGs(OGsFromFiles);
            return 0;
        }

        if (parameters[P_SIMULATES]) {
            // simulation on AnnotatedGraph
            checkSimulation(OGsFromFiles);
            // deleteOGs(OGsFromFiles);
            // return 0;
        }

        if (parameters[P_EX] && !options[O_BDD]) {
            // equivalence on (explicit representation of) operating guidelines
            checkEquivalence(OGsFromFiles);
            return 0;
        }
    }

    // **********************************************************************************
    // ********                start OG file dependant operations                ********
    // ********                  (OGs read one after the other)                  ********
    // **********************************************************************************

    else if (parameters[P_COUNT_SERVICES] || parameters[P_CHECK_ACYCLIC] ||
             parameters[P_CHECK_FALSE_NODES] || parameters[P_REMOVE_FALSE_NODES] ||
             parameters[P_PV] || parameters[P_MINIMIZE_OG] || parameters[P_READ_OG] ||
             (parameters[P_PNG] && ogfiles.size() != 0) ||
             (parameters[P_SYNTHESIZE_PARTNER_OWFN] && !parameters[P_ADAPTER] &&
              !parameters[P_SMALLADAPTER] && ogfiles.size() > 0) ) {

        // Abort if there are no OGs at all
        if (ogfiles.begin() == ogfiles.end() && !(parameters[P_PV])) {
            trace("Error:  No OGs have been given for computation\n\n");
            trace("        Enter \"fiona --help\" for more information\n\n");
        }

        // iterate all input files
        for (AnnotatedGraph::ogfiles_t::const_iterator iOgFile = ogfiles.begin(); iOgFile != ogfiles.end(); ++iOgFile) {

            AnnotatedGraph* readOG = readog(*iOgFile);

            if (parameters[P_SYNTHESIZE_PARTNER_OWFN]) {
                // computes partner out of IG or a most permissive partner from an og 

                // it is possible that we read an old OG file where no interface information
                // were stored, so we have to compute them from the graph
                if (readOG->inputPlacenames.empty() && readOG->outputPlacenames.empty()) {
                    readOG->computeInterfaceFromGraph();
                }

                if (parameters[P_IG]) {
                    computeSmallPartner(readOG);
                } else if (parameters[P_OG]) {
                    computeMostPermissivePartner(readOG);
                }
                delete readOG;
            }
            
            else if (parameters[P_PV]) {
                // computes a service automaton "public view" which has the same
                // OG as given in readOG
                readOG->removeReachableFalseNodes();
                computePublicView(readOG, (*iOgFile), false);
                delete readOG;
            }

            else if (parameters[P_MINIMIZE_OG]) {
                // minimizes a given OG

                string ogFilename = "";

                //readOG->removeReachableFalseNodes();
                readOG->minimizeGraph();

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                // generate output files
                if (!options[O_NOOUTPUTFILES]) {

                    if (options[O_OUTFILEPREFIX]) {
                        ogFilename = outfilePrefix;
                    } else {
                        ogFilename = readOG->getFilename() + ".minimal";
                    }
                    trace("Saving minimized annotated graph to:\n");
                    trace(readOG->addOGFileSuffix(ogFilename));
                    trace("\n\n");

                    // the second parameter is false, since no oWFN is given
                    readOG->printOGFile(ogFilename, false);

                    readOG->printDotFile(ogFilename); // .out, .png

                    if (parameters[P_TEX]) {
                        makeGasTex(readOG, ogFilename);
                    }
                }
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                delete readOG;
            }

            else if (parameters[P_COUNT_SERVICES]) {
                // counts the number of deterministic strategies
                // that are characterized by a given OG
                readOG->removeReachableFalseNodes();
                countStrategies(readOG, (*iOgFile));
                delete readOG;
            }

            else if (parameters[P_CHECK_ACYCLIC]) {
                // counts the number of deterministic strategies
                // that are characterized by a given OG
                readOG->removeReachableFalseNodes();
                checkAcyclicity(readOG, (*iOgFile));
                delete readOG;
            }

            else if (parameters[P_CHECK_FALSE_NODES]) {
                // checks if there are nodes in the og which violate the annotation
                trace(TRACE_0, "Checking '" + readOG->getFilename() + "' ");
                trace(TRACE_0, "for nodes with false annotation...");

                std::vector<AnnotatedGraphNode*> falseNodes;
                readOG->findFalseNodes(&falseNodes);

                if (falseNodes.size() == 0) {
                    trace(TRACE_0, "\n     No nodes with false annotation found!");
                } else {
                    trace(TRACE_0, "\n     Node(s) with false annotation found!");
                    std::vector<AnnotatedGraphNode*>::iterator it = falseNodes.begin();

                    while (it != falseNodes.end()) {
                        AnnotatedGraphNode* currentNode = *it;
                        trace(TRACE_1, "\n         Node '" + currentNode->getName() + "' violates its annotation. ");
                        trace(TRACE_2, "\n             Annotation is: " + currentNode->getAnnotationAsString());
                        ++it;
                    }
                }

                trace(TRACE_0, "\n\n");
                delete readOG;
            }

            else if (parameters[P_REMOVE_FALSE_NODES]) {
                // checks if there are nodes in the og which violate the annotation
                string newFilename;
                if (options[O_OUTFILEPREFIX]) {
                    newFilename = outfilePrefix;
                } else {
                    newFilename = AnnotatedGraph::stripOGFileSuffix(readOG->getFilename()) + ".blue";
                }
                time_t seconds, seconds2;

                if(TRACE_1 <= debug_level) {
                    readOG->computeAndPrintGraphStatistics();
                }

                trace(TRACE_0, "Removing nodes from '" + readOG->getFilename() + "' ");
                trace(TRACE_0, "that violate their own annotation...\n");
                seconds = time(NULL);
                readOG->removeReachableFalseNodes();
                seconds2 = time(NULL);
                trace(TRACE_0, "Removed all nodes with false annotation.\n");
                cout << "    " << difftime(seconds2, seconds) << " s consumed for removing reachable false nodes" << endl << endl;

                trace(TRACE_0, "Removing unreachable nodes from '" + readOG->getFilename() + "' \n");
                seconds = time(NULL);
                readOG->removeUnreachableNodes();
                seconds2 = time(NULL);
                trace(TRACE_0, "Removed all unreachable nodes.\n");
                cout << "    " << difftime(seconds2, seconds) << " s consumed removing unreachable nodes" << endl << endl;

                if(TRACE_1 <= debug_level) {
                    readOG->computeAndPrintGraphStatistics();
                }

                    if (!options[O_NOOUTPUTFILES]) {
                    trace(TRACE_0, "\nCreating new .og-file without false nodes... \n");
                    // the second parameter is false, since the read OG has no underlying oWFN
                    readOG->printOGFile(newFilename, false);
                    trace(TRACE_0, "New .og-file '" + newFilename + ".og' succesfully created.\n\n");
                }

                delete readOG;

            }

            else if (parameters[P_READ_OG]) {
                trace(TRACE_0, "OG was read from file '" + readOG->getFilename() + "'\n");
                readOG->computeAndPrintGraphStatistics();
                trace(TRACE_0, "HIT A KEY TO CONTINUE"); getchar();
                trace(TRACE_0, "\n");
                delete readOG;
            }

            else if (parameters[P_PNG]) {
                readOG->computeAndPrintGraphStatistics();
                string newFilename;
                if (options[O_OUTFILEPREFIX]) {
                    newFilename = outfilePrefix;
                } else {
                    newFilename = readOG->getFilename();
                }
                readOG->printDotFile(newFilename);
                readOG->computeAndPrintGraphStatistics();
                delete readOG;
            }
        }

#ifdef YY_FLEX_HAS_YYLEX_DESTROY
        // Destroy buffer of OG parser.
        // Must NOT be called before fclose(og_yyin);
        og_yylex_destroy();
#endif

#ifdef LOG_NEW
        NewLogger::printall();
#endif

        if (!parameters[P_PV] && !parameters[P_PNG]) {
            return 0;
        }
    }

    // **********************************************************************************
    // ********                start PN file dependant operations                ********
    // **********************************************************************************

    // netfiles only used for getting the correct name strings
    if (parameters[P_EX] && options[O_BDD]) {
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
        // return 0;
    }
    else if (parameters[P_ADAPTER] || parameters[P_SMALLADAPTER]) {

        Adapter adapter;
        adapter.generateAdapter();
    } 
    else if (parameters[P_IG] || parameters[P_OG] || parameters[P_MATCH] ||
             parameters[P_PNG] || parameters[P_REDUCE] || parameters[P_NORMALIZE] ||
             parameters[P_PV] || parameters[P_MATCH_PARTNER]) {

        if (parameters[P_MATCH]) {
            assert(ogfiles.size() == 1);
            // we match multiple oWFNs with one OG,
            // so read the og first, then iterate over the nets
            OGToMatch = readog(*(ogfiles.begin()));
        }

        string fileName;    // name of og-file
        ogfiles.clear();

        // ---------------- processing every single net -------------------
        for (list<std::string>::iterator netiter = netfiles.begin(); netiter != netfiles.end(); ++netiter) {

            // calculate the graph of the same net twice --> once with -R and once with no -R
            // in case the option -t eqR is set, otherwise we go through this loop only once
            for (int i = (parameters[P_EQ_R]) ? 0 : 1; i <= 1; i++) {
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

                if (currentowfnfile == "<stdin>") {
                    currentowfnfile = "stdin";
                }
                
                // start computation
                fileName = "";

                if (parameters[P_IG]) {
                    reportOptionValues(); // adjust events_manual and print limit of considering events
                    if (parameters[P_SYNTHESIZE_PARTNER_OWFN]) {
                        fileName = computeSmallPartner(PN); // compute a small partner
                    } else {
                        fileName = computeIG(PN);           // computing IG of the current oWFN 
                    }
                }

                if (parameters[P_OG] || parameters[P_PV]) {
                    reportOptionValues();        // adjust events_manual and print limit of considering events
                    //if (parameters[P_SYNTHESIZE_PARTNER_OWFN]) {
                    //    // TODO compute the most permissive partner
                    //} else {
                        fileName = computeOG(PN);    // computing OG of the current oWFN
                    //}
                }

                if (parameters[P_MATCH]) {
                    checkMatching(OGToMatch, PN); // matching the current oWFN against the single OG
                }

                if (parameters[P_PNG]) {
                    makePNG(PN); // create a png file of the given net
                }

                if (parameters[P_REDUCE]) {
                    reduceOWFN(PN);    // reduce given net
                }

                if (parameters[P_NORMALIZE]) {
                    normalizeOWFN(PN);    // normalize given net
                }

                if (parameters[P_EQ_R]) {
                    // reverse reduction mode for the next loop
                    options[O_CALC_ALL_STATES] = !options[O_CALC_ALL_STATES];
                    // remember file name of og-file to check equivalence later on
                    ogfiles.push_back(AnnotatedGraph::addOGFileSuffix(fileName));
                }
                
                if (parameters[P_MATCH_PARTNER]) {
                    
                    parameters[P_SYNTHESIZE_PARTNER_OWFN] = true;
                    parameters[P_OG] = true;
                    
                    string ogfilename = computeOG(PN);
                    
                    parameters[P_SYNTHESIZE_PARTNER_OWFN] = false;
                    parameters[P_OG] = false;
                    string PNFileName = currentowfnfile;    
                    string netfile = PNFileName.substr(0, PNFileName.find(".owfn") );
                    string partnerfilename = netfile + "-partner.owfn";

                    system(("src/fiona -t match " + ogfilename + ".og " + partnerfilename).c_str());
                    
                }            
                
                //delete PN;
                //trace(TRACE_5, "net deleted\n");
            }

            // in case the option -t eqR is set, check equivalence for both computed graphs
            if (parameters[P_EQ_R]) {
                // reading all og-files
                AnnotatedGraph::ogs_t OGsFromFiles;
                readAllOGs(OGsFromFiles);

                // check equivalence of both graphs
                checkEquivalence(OGsFromFiles);

                OGsFromFiles.clear();
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
