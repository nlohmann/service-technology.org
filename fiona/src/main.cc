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
 * \file    main.cc
 *
 * \brief   main
 *
 * \author  responsible: Daniela Weinberg <weinberg@informatik.hu-berlin.de>
 */

#include "AnnotatedGraph.h"
#include "ConstraintOG.h"
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
#include <ctime>
#include "pnapi.h"

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

extern int covog_yylineno;
extern int covog_yydebug;
extern int covog_yy_flex_debug;
extern FILE* covog_yyin;
extern int covog_yyerror();
extern int covog_yyparse();

#ifdef YY_FLEX_HAS_YYLEX_DESTROY
extern int covog_yylex_destroy();
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
extern std::list<std::string> covfiles;
extern AnnotatedGraph* OGToParse;
extern GraphFormulaCNF* covConstraint;

extern std::string givenPortName;

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

// The global exit code. The main method will return this value.
unsigned short int globalExitCode = 0;

//! \brief sets the global exit code. Calls exit if exit code is greater equal
//exitAtThisCode which is defined in options.h as a constant.
//\param newExitCode an exit code as defined in options.h.
void setExitCode(unsigned short int newExitCode) {

	globalExitCode = newExitCode;

    if (newExitCode >= exitAtThisCode) {
        switch ( globalExitCode ) {
        case EC_FILE_ERROR:
            trace("\nError: A file error occurred. Exit.\n\n");
            break;
        case EC_MEMORY_EXHAUSTED:
            cerr << "new failed, memory exhausted"<< endl;
            break;
        case EC_DOT_ERROR:
            trace( "\nerror: Dot exited with non zero value! here\n\n");
            break;
        case EC_NO_CUDD_FILE:
            cerr << "\nTo check equivalence of two nets, the BDD representations of"
                 << "\nthe corresponding OGs must have been computed before.\n"
                 << "\nEnter \"fiona --help\" for more information" << endl;
            break;
        case EC_NO_RULES_FILE:
            cerr << "cannot open rules file '" << adapterRulesFile << "' for reading'\n" << endl;
            break;
        }

        exit(globalExitCode);
    }

}

//! \brief an exit function in case the memory is exhausted
void myown_newhandler() {
    setExitCode(EC_MEMORY_EXHAUSTED);
}



//! \brief reads an oWFN from owfnfile
//! \param owfnfile the owfnfile to be read from
void readnet(const std::string& owfnfile) {

    owfn_yylineno = 1;
    owfn_yydebug = 0;
    owfn_yy_flex_debug = 0;
    assert(owfnfile != "");
    // diagnosefilename = (char *) 0;

    TRACE(TRACE_1, "reading from file " + owfnfile + "\n");

    if (owfnfile == "<stdin>") {
        owfn_yyin = stdin;
    }
    else owfn_yyin = fopen(owfnfile.c_str(), "r");
    if (!owfn_yyin) {
        setExitCode(EC_FILE_ERROR);
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


    // for autonomous controllability, the net has to be changed
    // now is the time to do so

    if (parameters[P_AUTONOMOUS]) {
        if (givenPortName == "") {
            trace( "error: no port given\n\n");
            setExitCode(EC_NO_PORT_GIVEN);
        }
        if (PN->ports[givenPortName].empty()) {
            trace( "error: port not found\n\n");
            setExitCode(EC_PORT_NOT_FOUND);
        }
        PN->restrictToPort(givenPortName);
    }

    for (unsigned int i = 0; i < PN->getPlaceCount(); i++) {
        PN->CurrentMarking[i] = PN->getPlace(i)->initial_marking;
    }

    PN->initialize();
    // Initialize final condition (if present) with initial marking.
    // Only afterwards merging is allowed, because merge requires subformulas
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

    TRACE(TRACE_1, "reading from file " + owfnfile + " finished.\n");
}


//! \brief reports the net statistics
void reportNet() {
    trace( "    places: " + intToString(PN->getPlaceCount()));
    trace( " (including " + intToString(PN->getInputPlaceCount()));
    trace( " input places, " + intToString(PN->getOutputPlaceCount()));
    trace( " output places)\n");
    trace( "    transitions: " + intToString(PN->getTransitionCount()) + "\n");
    trace( "    ports: " + intToString(PN->getPortCount()) + "\n\n");
    if (PN->FinalCondition) {
        trace( "finalcondition used\n\n");
    } else if (PN->FinalMarkingList.size() > 0) {
        trace( "finalmarking used\n\n");
    } else {
        trace( "neither finalcondition nor finalmarking given\n");
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
    TRACE(TRACE_1, "reading from file " + ogfile + "\n");
    og_yyin = fopen(ogfile.c_str(), "r");
    if (!og_yyin) {
        cerr << "cannot open OG file '" << ogfile << "' for reading'\n" << endl;
        setExitCode(EC_FILE_ERROR);
    }
    OGToParse = new AnnotatedGraph();
    ogfileToParse = ogfile; // for debug - declaration in debug.cc
    og_yyparse();
    fclose(og_yyin);

    OGToParse->setFilename(ogfile);
    TRACE(TRACE_1, "file successfully read\n");

    return OGToParse;
}


//! \brief reads an og with a (global) constraint (constraint og for short)
//! from ogfile
//! \param ogfile constraint og
//! \return returns the OG object
AnnotatedGraph* readconstraintog(const std::string& ogfile, GraphFormulaCNF*& covconstraint) {
    covog_yylineno = 1;
    covog_yydebug = 0;
    covog_yy_flex_debug = 0;
    assert(ogfile != "");
    TRACE(TRACE_1, "reading from file " + ogfile + "\n");
    covog_yyin = fopen(ogfile.c_str(), "r");
    if (!covog_yyin) {
        cerr << "cannot open constraint OG file '" << ogfile << "' for reading'\n" << endl;
        setExitCode(EC_FILE_ERROR);
    }
    OGToParse = new AnnotatedGraph;
    ogfileToParse = ogfile;
    covog_yyparse();
    fclose(covog_yyin);

    OGToParse->setFilename(ogfile);

    covconstraint = covConstraint;
    TRACE(TRACE_1, "file successfully read\n");

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
    TRACE(TRACE_5, "main: deleteOGs(const AnnotatedGraph::ogs_t& OGsFromFiles) : start\n");

    for (AnnotatedGraph::ogs_t::const_iterator iOg = OGsFromFiles.begin();
         iOg != OGsFromFiles.end(); ++iOg) {
        delete *iOg;
    }

    OGsFromFiles.clear();

    TRACE(TRACE_5, "main: deleteOGs(const AnnotatedGraph::ogs_t& OGsFromFiles) : end\n");
}


//! \brief reports values for -e and -m option
void reportOptionValues() {
    TRACE(TRACE_5, "-e option found: ");
    if (options[O_EVENT_USE_MAX]) {
        TRACE(TRACE_5, "yes\n");
    } else {
        TRACE(TRACE_5, "no\n");
    }

    // max_occurrence is set for each input/output place of the net in the owfn file
    // syntax in the owfn file:
    //      abort {$ MAX_OCCURRENCES = 2 $}
    // (use -1 for unbounded occurrence)
    TRACE(TRACE_5, "-E option found: ");
    if (options[O_READ_EVENTS]) {
        TRACE(TRACE_5, "yes\n");

        // store the max occurrence information
        string maxOccurrenceString = "";

        // if the max occurrence property of at least one input/output place is set (>0)
        // then the maxOccurenceString will be shown
        bool showMaxOccurrences = false;

        maxOccurrenceString += "considering the following events:\n";
        maxOccurrenceString += "    sending events:\n";

        for (unsigned int e = 0; e < PN->getInputPlaceCount(); e++) {
            maxOccurrenceString += "        !" + string(PN->getInputPlace(e)->name);
            if (PN->getInputPlace(e)->max_occurrence >= 0) {
                maxOccurrenceString += " (max. " + intToString(PN->getInputPlace(e)->max_occurrence) + "x)\n";
                showMaxOccurrences = true;
            } else {
                maxOccurrenceString += " (unbounded)\n";
            }
        }

        maxOccurrenceString += "    receiving events:\n";
        for (unsigned int e = 0; e < PN->getOutputPlaceCount(); e++) {
            maxOccurrenceString += "        ?" + string(PN->getOutputPlace(e)->name);
            if (PN->getOutputPlace(e)->max_occurrence >= 0) {
                maxOccurrenceString += " (max. " + intToString(PN->getOutputPlace(e)->max_occurrence) + "x)\n";
                showMaxOccurrences = true;
            } else {
                maxOccurrenceString += " (unbounded)\n";
            }
        }

        maxOccurrenceString += "\n";

        // is there at least one input/output place for which the max_occurrence property is set,
        // then show the whole maxOccurrenceString
        // otherwise discard the maxOccurrenceString
        if (showMaxOccurrences) {
            trace( maxOccurrenceString);
        }
    } else {
        TRACE(TRACE_5, "no\n");
    }

    if (options[O_EVENT_USE_MAX]) {
        if (events_manual == -1) {
            trace( "each event considered max: unbounded\n");
        } else {
            trace( "each event considered max: " + intToString(events_manual) + "\n");
        }
    }

    // report message bound
    if (options[O_MESSAGES_MAX]) {
        trace( "interface message bound set to: "
                        + intToString(messages_manual) + "\n");
    }

    if (parameters[P_RESPONSIVE]) {
        trace( "\ncalculation of responsive partner(s)\n");
    }

    trace( "\n");
}


 // **********************************************************************************
// *******                    mode dependent functions                       ********
// **********************************************************************************


//! \brief create a GasTex file of annotated dot file 'myDotFile'
//! \param myDotFile the dot file with layout annotations
//! \param myFilePrefix the file prefix to add '.tex' to
void makeGasTex(std::string myDotFile, std::string myFilePrefix,
                GasTexGraph::STYLE style) {
    TRACE(TRACE_1, "makeGasTex called for file: ");
    TRACE(TRACE_1, myDotFile + "\n");
    TRACE(TRACE_1, "prefix of the file is: ");
    TRACE(TRACE_1, myFilePrefix + "\n");

    string dotFileName = myDotFile;

    dot_yylineno = 1;
    dot_yydebug = 0;

    // try to open the dot file for reading
    dot_yyin = fopen((dotFileName).c_str(), "r");
    if (!dot_yyin) {
        cerr << "cannot open dot file '" << dotFileName << "' for reading'\n" << endl;
        setExitCode(EC_FILE_ERROR);
    }

    // clear the graph holding the parsed informations
    if (gastexGraph) {
        delete gastexGraph;
    }

    // preparing the global variable gastexGraph for parsing
    gastexGraph = new GasTexGraph();

    // the style has to be set before parsing
    gastexGraph->setStyle(style);
    // parsing the dot file into variable gastexGraph
    dot_yyparse();
    fclose(dot_yyin);

    string texFileName = myFilePrefix + ".tex";

    // writing gastexGraph to new tex file
    gastexGraph->makeGasTex(texFileName);

    trace( texFileName + " generated\n");
}


//! \brief creates all output files with respect to the parameters
//! \param graph An object of AnnotatedGraph, CommunicationGraph, IG or OG
//! \param a title for the dot file
void createOutputFiles(AnnotatedGraph* graph, string prefix, string dotFileTitle) {
    if (!parameters[P_NODOT]) {

        string dotFileName = graph->createDotFile(prefix, dotFileTitle); // .out

        if (!parameters[P_NOPNG] && dotFileName != "") {
            string pngres = graph->createPNGFile(prefix, dotFileName);
            if (pngres != "") trace(pngres + " generated\n");

        }

        if (parameters[P_TEX] && dotFileName != "") {
            string annotatedDotFileName = graph->createAnnotatedDotFile(prefix, dotFileName); // .dot
            makeGasTex(annotatedDotFileName, prefix, GasTexGraph::STYLE_OG);
        }
    }
}


//! \brief output a given public view into dot (.out), png and owfn.
//! \param graphName a name for the graph in the output
//! \param pv The public view
//! \param fromOWFN If set to true, the method uses information of the PN
//! \param inputs A set of input places that will be put into the resulting owfn - can be omitted.
//! \param outputs A set of output places that will be put into the resulting owfn - can be omitted.
//! \todo check: parameter fromOWFN is not used
void outputPublicView(string graphName, Graph* pv, bool fromOWFN, set<string> inputs = set<string>(), set<string> outputs = set<string>()) {

    // dont create the public view automaton's png if it becomes to big
    // not used: unsigned int maxSizeForDot = 150;

    // Determine filename
    outfilePrefix = AnnotatedGraph::stripOGFileSuffix(graphName);
    outfilePrefix += ".pv.sa";

    // generate output files
    if (!options[O_NOOUTPUTFILES]) {

        if (!parameters[P_NODOT]) {

            string title = "Public view of ";
            title += graphName;
            string dotfile = pv->createDotFile(outfilePrefix, title);
            if (!parameters[P_NOPNG] && dotfile != "") {
                pv->createPNGFile(outfilePrefix, dotfile);
            }

        }

        //transform to owfn
        PNapi::PetriNet* PVoWFN = new PNapi::PetriNet();
        PVoWFN->set_format(PNapi::FORMAT_OWFN, true);

        pv->transformToOWFN(PVoWFN, inputs, outputs);

        // print the information of the public view's owfn
        TRACE(TRACE_1, "Public View oWFN statistics:\n");
        trace(PVoWFN->information());
        TRACE(TRACE_1, "\n");

        // create the stream for the owfn output
        ofstream output;
        const string owfnOutput = AnnotatedGraph::stripOGFileSuffix(graphName) + ".pv.owfn";
        output.open (owfnOutput.c_str(),ios::out);
        if (!output.good()) {
            output.close();
            setExitCode(EC_FILE_ERROR);
        }
        (output) << (*PVoWFN);
        output.close();

        trace("\n=================================================================\n");
        trace("\n");

    }

}



//! \brief generate a public view for a given og
//! \param OG an og to generate the public view of
//! \param graphName a name for the graph in the output
//! \param fromOWFN If set to true, the method uses information of the PN
//! \param keepOG if set to true, the method first creates a copy of the OG - otherwise it will be changed.
//! \return the public view as a pointer to a Graph object on the heap.
Graph* computePublicView(AnnotatedGraph* OG, string graphName, bool fromOWFN, bool keepOG=false) {

    // if the OG is empty, there is no public view and the computation
    // will be aborted
    if (OG->hasNoRoot() || OG->getRoot()->getColor() == RED) {
        if (fromOWFN) {
            trace("\nThe given oWFN is not controllable. No PublicView will be generated.\n\n");
        } else {
            trace("\nThe given OG describes no partners. No PublicView will be generated.\n\n");
        }
        setExitCode(1);
        return NULL;
    }

    trace("generating the public view for ");
    trace(graphName);

    trace("\n");


    AnnotatedGraph* annotatedPV; // This public view will be copied into a graph object later.

    // If the keepOG parameter is true, create a copy of the OG.
    // If not, simply work on the given OG.
    if (keepOG) {
        annotatedPV = new AnnotatedGraph(); // Create a new object as destination for the deep copy.
        OG->toAnnotatedGraph(annotatedPV);  // Perform deep copy.
    } else {
        annotatedPV = OG;                   // Work on the given OG.
    }

    // Transformation into a Public View.
    // The pv object will be transformed - i.e. either the given OG or
    // a deep copy.
    annotatedPV->transformToPV(fromOWFN);

    // Transform the AnnotatedGraph object annotatedPV into a Graph.
    Graph* pv = new Graph();       // Create a new object as destination for the copy.
    annotatedPV->toGraph(pv);               // Perform copy to Graph - getting rid of annotations.

    // If the given OG was duplicated, the duplicate has to be deleted.
    // Otherwise nothing is to be done in this step.
    if (keepOG) {
        delete annotatedPV;
    }

    // Return the Graph pointer.
    return pv;

}


//! \brief create an IG of an oWFN
//! \param PN pointer to the given oWFN whose IG will be computed
//! \param igFilename reference to a string containing the filename of the created IG
InteractionGraph* computeIG(oWFN* PN, string& igFilename) {

    // print information about reduction rules (if desired)
    if (options[O_CALC_REDUCED_IG] || parameters[P_USE_EAD]) {
        trace( "building the reduced interaction graph by using reduction rule(s)\n");
        if (parameters[P_USE_CRE]) {
            trace( "     \"combine receiving events\"\n");
        }
        if (parameters[P_USE_RBS]) {
            trace( "     \"receiving before sending\"\n");
        }
        if (parameters[P_USE_EAD]) {
            trace( "     \"early detection\"\n");
        }
    } else {
        trace( "building the interaction graph...\n");
    }
    trace( "\n");

    // [LUHME XV] Konstruktor und "buildGraph()" verheiraten?
    InteractionGraph * IG = new InteractionGraph(PN);
    igFilename = "";

    // build interaction graph
    time_t buildGraph_start, buildGraph_end;
    buildGraph_start = time (NULL);

    // [LUHME XV] "buildGraph" in "build" umbenennen
    IG->buildGraph();
    buildGraph_end = time (NULL);

    if (options[O_CALC_REDUCED_IG]) {
        trace( "building the reduced interaction graph finished.\n");
    } else {
        trace( "\nbuilding the interaction graph finished.\n");
    }
    // [LUHME XV] Tracen, Zeit in Millisekunden nehmen.
    cout << difftime(buildGraph_end, buildGraph_start) << " s consumed for building graph" << endl;

    // [LUHME XV] gehört in die Klasse "InteractionGraph" in die Dateiausgabe
    // add interface information to graph: Input -> Output and vice versa
    // input/outputPlacenames contain all possible labels for the IG
    for (unsigned int i = 0; i < PN->getInputPlaceCount(); i++) {
        IG->outputPlacenames.insert( (PN->getInputPlace(i))->name );
    }
    for (unsigned int i = 0; i < PN->getOutputPlaceCount(); i++) {
        IG->inputPlacenames.insert( PN->getOutputPlace(i)->name );
    }

    // print statistics
    trace( "\nnet is controllable: ");
    if (IG->hasBlueRoot()) {
        trace( "YES\n\n");
    } else {
        trace( "NO\n\n");
        setExitCode(1);
    }

    trace( "IG statistics:\n");
    IG->printGraphStatistics();
    trace( "\n");

//    graph->computeGraphStatistics();
//    trace( "IG statistics:\n");
//    graph->printGraphStatistics();
//    trace( "\n");


    // create output files if needed
    if (!options[O_NOOUTPUTFILES]) {
        if (parameters[P_DIAGNOSIS]) {
            IG->diagnose();
        }

        if (!parameters[P_EQ_R]) {    // don't create png if we are in eqr mode
            // generate output files
            string basefilename = options[O_OUTFILEPREFIX] ? outfilePrefix : PN->filename;
            basefilename += IG->getSuffix();
            createOutputFiles(IG, basefilename, "IG of " + PN->filename);
        }

        // create also an .og file to enable comparison of different IGs

        // [LUHME XV] alles von hier an in eine "createFilename(Type)"-Funktion
        if (options[O_OUTFILEPREFIX]) {
            igFilename = outfilePrefix;
        } else {
            igFilename = IG->getFilename();
        }

        igFilename += ".ig";

        // the second parameter is true, since the oWFN this IG was generated
        // from still exists and additional information are available
        IG->createOGFile(igFilename, true);
    }

    return IG;
}


// [LUHME XV] Brauchen wir diesen Wrapper?
string computeIG(oWFN* PN) {

    // [LUHME XV] "igFilename" als Member von InteractionGraph
    string igFilename = "";
    // [LUHME XV] lieber als Konstruktoraufruf hier?
    InteractionGraph* graph = computeIG(PN, igFilename);

    TRACE(TRACE_5, "computation finished -- trying to delete graph\n");

    // [LUHME XV] Soll/muss das hierher?
    graph->clearNodeSet();
    delete graph;

    TRACE(TRACE_5, "graph deleted\n");
    trace( "=================================================================\n");
    trace( "\n");

    return igFilename;
}


//! \brief creates a small partner out of a given IG
//! \param IG a pointer to an IG of an oWFN whose small partner will be computed
//! \return a string containing the filename of the partner owfn
string computeSmallPartner(AnnotatedGraph* IG) {

    TRACE(TRACE_5, "string computeSmallPartner(AnnotatedGraph*) : start\n");
    trace( "\n\nComputing partner oWFN \n");

    // 1. step: was done before (computing the IG)
    // 2. step: if the net is controllable create STG file for petrify out of computed IG
    string stgFilename = "";
    vector<string> edgeLabels; // renamend transitions

    if (!IG->hasNoRoot() && IG->getRoot()->getColor() == BLUE) {
        TRACE(TRACE_1, "    Creating STG File\n");
        stgFilename = IG->printGraphToSTG(edgeLabels);
    } else {
        trace("\n    Cannot synthesize a partner for a net, that is not controllable\n\n");
        return "";
    }

    // 3. step: invoke petrify on created STG file
    // prepare petrify command line and execute system command if possible
    string pnFilename = stgFilename.substr(0, stgFilename.size() - 4) + ".pn"; // change .stg to .pn

    string systemcall = string(CONFIG_PETRIFY) + " " + stgFilename + " -dead -ip -nolog -o " + pnFilename;
    if (CONFIG_PETRIFY != "not found") {
        TRACE(TRACE_1, "    Calling Petrify.\n");
        TRACE(TRACE_2, "        " + systemcall + "\n");
        system(systemcall.c_str());
    } else {
        trace("cannot execute command as Petrify was not found in path\n");
        return "";
    }


    // 4. step: create oWFN out of the petrinet computed by petrify
    // 5. step: complete interface with the interface information stored in the
    // IG (done in STG2oWFN_main
    TRACE(TRACE_1, "    Converting petrify result to petri net.\n");
    string owfnFilename = STG2oWFN_main(edgeLabels, pnFilename, IG->inputPlacenames, IG->outputPlacenames);


    trace("Partner synthesis completed. Created file: " + owfnFilename + "\n");
    TRACE(TRACE_5, "string computeSmallPartner(AnnotatedGraph*) : end\n");

    return owfnFilename; // return partner filename
}


//! \brief creates a small partner out of a given oWFN
//! \param PN a pointer to an oWFN whose small partner will be computed
//! \return a string containing the filename of the computed IG from given oWFN
string computeSmallPartner(oWFN* givenPN) {

    // 1. step: compute the IG of given net
    string igFilename = "";
    InteractionGraph* graph = computeIG(givenPN, igFilename);

    // 2.-4. step wil be done with former computed IG (ignoring partner filename)
    computeSmallPartner(graph);

    // garbage collection
    delete graph;

    return igFilename;
}


//! \brief creates the most permissive partner out of a given OG
//! \param OG a pointer to an OG of an oWFN whose most permissive partner will be computed
//! \return a string containing the filename of the partner owfn.
string computeMostPermissivePartner(AnnotatedGraph* OG) {

    TRACE(TRACE_5, "string computeMostPermissivePartner(AnnotatedGraph*) : start\n");
    trace( "\n\nComputing partner oWFN \n");

    // 1. step: was done before (computing the OG)
    // 2. step: if the net is controllable create STG file for petrify out of computed OG
    string stgFilename = "";
    vector<string> edgeLabels; // renamend transitions

    if (!OG->hasNoRoot() && OG->getRoot()->getColor() == BLUE) {
        TRACE(TRACE_1, "    Creating STG File\n");
        stgFilename = OG->printGraphToSTG(edgeLabels);
    } else {
        trace( "\nCannot synthesize a partner for a net, that is not controllable\n\n");
        return "";
    }


    // 3. step: invoke petrify on created STG file
    // prepare petrify command line and execute system command if possible
    string pnFilename = stgFilename.substr(0, stgFilename.size() - 4) + ".pn"; // change .stg to .pn

    string systemcall = string(CONFIG_PETRIFY) + " " + stgFilename + " -dead -ip -nolog -o " + pnFilename;
    if (CONFIG_PETRIFY != "not found") {
        TRACE(TRACE_1, "    Calling Petrify.\n");
        TRACE(TRACE_2, "        " + systemcall + "\n");
        system(systemcall.c_str());
    } else {
        trace( "cannot execute command as Petrify was not found in path\n");
        return "";
    }


    // 4. step: create oWFN out of the petrinet computed by petrify
    TRACE(TRACE_1, "    Converting petrify result to petri net.\n");
    string owfnFilename = STG2oWFN_main( edgeLabels, pnFilename, OG->inputPlacenames, OG->outputPlacenames );

    trace( "Partner synthesis completed. Created file: " + owfnFilename + "\n");
    TRACE(TRACE_5, "string computeMostPermissivePartner(AnnotatedGraph*) : end\n");

    return owfnFilename; // return partner filename
}


//! \brief create an OG of an oWFN
//! \param PN the given oWFN
string computeOG(oWFN* PN) {

    time_t  buildGraphTime_start, buildGraphTime_end,
            overAllTime_start, overAllTime_end,
            graphStatsTime_start, graphStatsTime_end,
            removeFalseNodesTime_start, removeFalseNodesTime_end,
            removeUnreachableNodesTime_start, removeUnreachableNodesTime_end;

    OG* graph;

    if (parameters[P_COVER]) {
        // operating guideline with global constraint

        graph = new ConstraintOG(PN);
        trace("reading the coverability constraint from file ");
        trace(PN->filename + ".cov" + "\n\n");
        static_cast<ConstraintOG*>(graph)->readCovConstraint(PN->filename + ".cov");
    } else {
        // basic operating guideline
        graph = new OG(PN);
    }

    bool controllable = false;
    string ogFilename = "";

    if (parameters[P_COVER]) {
        trace( "building the operating guideline with global constraint...\n");
    } else {
        trace( "building the operating guideline...\n");
    }

    graph->printProgressFirst();

    overAllTime_start = time(NULL);

    // build operating guideline
    TRACE(TRACE_1, "Building the graph...\n");
    buildGraphTime_start = time(NULL);

    // [LUHME XV] in "build()" umbenennen
    graph->buildGraph();

    buildGraphTime_end = time(NULL);

    TRACE(TRACE_1, "finished building the graph\n");
    // [LUHME XV] Zeit in Millisekunden nehmen (�BERALL!)
    TRACE(TRACE_2, "    " + intToString((int) difftime(buildGraphTime_end, buildGraphTime_start)) + " s consumed.\n\n");

    // add interface information to graph: Input -> Output and vice versa
    // input/outputPlacenames contain all possible labels for the IG
    for (unsigned int i = 0; i < PN->getInputPlaceCount(); i++) {
        graph->outputPlacenames.insert( (PN->getInputPlace(i))->name );
    }
    for (unsigned int i = 0; i < PN->getOutputPlaceCount(); i++) {
        graph->inputPlacenames.insert( PN->getOutputPlace(i)->name );
    }

//    if (!parameters[P_SHOW_RED_NODES] && !parameters[P_SHOW_ALL_NODES]) {
////        TRACE(TRACE_1, "removing false nodes...\n");
////        removeFalseNodesTime1 = time(NULL);
////        graph->removeReachableFalseNodes();
////        removeFalseNodesTime2 = time(NULL);
////        TRACE(TRACE_1, "finished removing false nodes\n");
////        TRACE(TRACE_2, "    " + intToString((int) difftime(removeFalseNodesTime2, removeFalseNodesTime1)) + " s consumed.\n\n");
////
//        TRACE(TRACE_1, "removing unreachable nodes...\n");
//        removeUnreachableNodesTime_start = time(NULL);
//        graph->removeUnreachableNodes();
//        removeUnreachableNodesTime_end = time(NULL);
//        TRACE(TRACE_1, "finished removing unreachable nodes\n");
//        TRACE(TRACE_2, "    " + intToString((int) difftime(removeUnreachableNodesTime_end, removeUnreachableNodesTime_end)) + " s consumed.\n\n");
//    }

    overAllTime_end = time(NULL);

    if (parameters[P_COVER]) {
        trace( "\nbuilding the operating guideline with global constraint finished.\n");
    } else {
        trace( "\nbuilding the operating guideline finished.\n");
    }
    trace( "    " + intToString((int) difftime(overAllTime_end, overAllTime_start)) + " s overall consumed for OG computation.\n\n");

    trace( "\nnet is controllable: ");
    if (!graph->hasBlueRoot() ||            (parameters[P_COVER] && static_cast<ConstraintOG*>(graph)->getCovConstraint()->equals() == FALSE)) {
        trace( "NO\n\n");
        setExitCode(1);
    } else {
        trace( "YES\n\n");
        controllable = true;
    }

    if (parameters[P_COVER]) {
        set<string> uncovered =
                static_cast<ConstraintOG*>(graph)->computeUncoverables();
        if (!uncovered.empty()) {
            trace("The following oWFN places or transitions can not be covered:\n");
            for (set<string>::iterator i = uncovered.begin();
                 i != uncovered.end(); ++i) {
                trace((*i));
                trace(", ");
            }
            trace("\b\b \n\n");
        }
    }

//    sleep(100);

    // print statistics
    trace( "OG statistics:\n");
    graph->printGraphStatistics();
    trace( "\n");

    // switch to publicview generation if the mode is PV
    if (parameters[P_PV]) {

        string publicViewName;

        // [LUHME XV] Dateinamen-Manipulation in FileNameHandler auslagern
        if (options[O_OUTFILEPREFIX]) {
            publicViewName = outfilePrefix;
        } else {
            publicViewName = graph->getFilename();
            publicViewName = publicViewName.substr(0, publicViewName.size()-5);
        }

        Graph* PV = computePublicView(graph, publicViewName, true);
        if (PV != NULL) {
            CommunicationGraph* casted = dynamic_cast<CommunicationGraph*>(graph);
            set<string> inputs, outputs;
            casted->returnInterface(inputs,outputs);
            outputPublicView(publicViewName, PV, true, inputs, outputs);
            delete PV;
        }
        delete graph;
        return "";
    }

    // generate output files
    if (!options[O_NOOUTPUTFILES]) {
        // distributed controllability?
        if (parameters[P_DISTRIBUTED]) {
            trace( "\nannotating OG for distributed controllability\n");

            bool graphChanged = true;
            while (graphChanged) {
                graphChanged = graph->annotateGraphDistributedly();
                graph->correctNodeColorsAndShortenAnnotations();
                // cerr << endl;
            }

            parameters[P_SHOW_EMPTY_NODE] = false;

            trace( "\nnet is distributedly controllable: ");
            if (graph->hasBlueRoot()) {
                trace( "MAYBE\n\n");
            } else {
                trace( "NO\n\n");
                setExitCode(1);
                parameters[P_SHOW_ALL_NODES] = true;
            }
        }

        if (!parameters[P_EQ_R]) {    // don't create png if we are in eqr mode
            string basefilename = options[O_OUTFILEPREFIX] ? outfilePrefix : PN->filename;
            basefilename += graph->getSuffix();
            if (parameters[P_COVER]) {
                createOutputFiles(graph, basefilename, "global constraint: " +
                                  static_cast<ConstraintOG*>(graph)->getCovConstraint()->asString() +
                                  "\\n\\nOG with global constraint of " + PN->filename);
            } else {
                createOutputFiles(graph, basefilename, "OG of " + PN->filename);
            }
        }

        if (options[O_OUTFILEPREFIX]) {
            ogFilename = outfilePrefix;
        } else {
            ogFilename = graph->getFilename();
        }

        // the second parameter is true, since the oWFN this OG was generated
        // from still exists and additional information are available
        graph->createOGFile(ogFilename, true);
        //vector<string> edgeLabels;                        // renamend transitions
        if (parameters[P_SYNTHESIZE_PARTNER_OWFN]) {
            if (parameters[P_EFFICIENT_PARTNER_OWFN]) {

                // recolor inefficient nodes for finding cost efficient partner
                graph->recolorInefficientNodes();

                // DEBUG
                //string basefilename = options[O_OUTFILEPREFIX] ? outfilePrefix : PN->filename;
                //basefilename += graph->getSuffix();
                //createOutputFiles(graph, basefilename + "_recolored", "OG of " + PN->filename);
                //graph->createOGFile(ogFilename + "_recolored", true);
            }
            computeMostPermissivePartner(graph);
        }

        if (options[O_OTF]) {
            //graph->bdd->createDotFile();
            graph->bdd->save("OTF");
        }
    }

    if (options[O_BDD]) {
        trace( "\nbuilding the BDDs...\n");
        overAllTime_start = time (NULL);
        graph->convertToBdd();
        overAllTime_end = time (NULL);
        cout << difftime(overAllTime_end, overAllTime_start) << " s consumed for building and reordering the BDDs" << endl;

        //graph->bdd->createDotFile();
        if (!options[O_NOOUTPUTFILES]) {
            graph->bdd->save();
        }
    }

    TRACE(TRACE_5, "computation finished -- trying to delete graph\n");
    graph->clearNodeSet();
    delete graph;
    graph = NULL;
    TRACE(TRACE_5, "graph deleted\n");

    // trace( "HIT A KEY TO CONTINUE"); getchar();
    trace( "=================================================================\n");
    trace( "\n");

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
    trace( "Attention: This result is only valid if the given OGs are complete\n");
    trace( "           (i.e., \"-s empty\" option was set and \"-m\" option high enough)\n\n");

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
        productOG->createOGFile(outfilePrefix, false);

        createOutputFiles(productOG, outfilePrefix, AnnotatedGraph::addOGFileSuffix(outfilePrefix));

        trace("\n");
    }

    delete productOG;
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

        // simulation on OGs depends heavily on empty node,
        // so set the correct options to compute OG with empty node
        options[O_SHOW_NODES] = true;
        parameters[P_SHOW_EMPTY_NODE] = true;
    }

    time_t  buildGraphTime_start, buildGraphTime_end,
            overAllTime_start, overAllTime_end,
            graphStatsTime_start, graphStatsTime_end,
            removeFalseNodesTime_start, removeFalseNodesTime_end,
            removeUnreachableNodesTime_start, removeUnreachableNodesTime_end;

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
        trace( "=================================================================\n");
        trace( "processing net " + currentowfnfile + "\n");
        reportNet();
        delete PlaceTable;

        // compute OG
        OG* graph = new OG(PN);

        trace( "building the operating guideline...\n");
        graph->printProgressFirst();

        overAllTime_start = time(NULL);

        TRACE(TRACE_1, "Building the graph...\n");
        buildGraphTime_start = time(NULL);
        graph->buildGraph(); // build operating guideline
        buildGraphTime_end = time(NULL);
        TRACE(TRACE_1, "finished building the graph\n");
        TRACE(TRACE_2, "    " + intToString((int) difftime(buildGraphTime_end, buildGraphTime_start)) + " s consumed.\n");

//        TRACE(TRACE_1, "computing graph statistics...\n");
//        graphStatsTime_start = time(NULL);
//        graph->computeGraphStatistics();
//        graphStatsTime_end = time(NULL);
//        TRACE(TRACE_1, "finished computing graph statistics\n");
//        TRACE(TRACE_2, "    " + intToString((int) difftime(graphStatsTime_end, graphStatsTime_end)) + " s consumed.\n");
//
//
//        if (!parameters[P_SHOW_RED_NODES] && !parameters[P_SHOW_ALL_NODES]) {
//            TRACE(TRACE_1, "removing false nodes...\n");
//            removeFalseNodesTime_start = time(NULL);
//            graph->removeReachableFalseNodes();
//            removeFalseNodesTime_end = time(NULL);
//            TRACE(TRACE_1, "finished removing false nodes\n");
//            TRACE(TRACE_2, "    " + intToString((int) difftime(removeFalseNodesTime_end, removeFalseNodesTime_start)) + " s consumed.\n");
//
//            TRACE(TRACE_1, "removing unreachable nodes...\n");
//            removeUnreachableNodesTime_start = time(NULL);
//            graph->removeUnreachableNodes();
//            removeUnreachableNodesTime_end = time(NULL);
//            TRACE(TRACE_1, "finished removing unreachable nodes\n");
//            TRACE(TRACE_2, "    " + intToString((int) difftime(removeUnreachableNodesTime_end, removeUnreachableNodesTime_end)) + " s consumed.\n");
//        }


        overAllTime_end = time(NULL);

        trace( "\nbuilding the operating guideline finished.\n");
        trace( "    " + intToString((int) difftime(overAllTime_end, overAllTime_start)) + " s overall consumed for OG computation.\n");

        // print statistics
        trace( "OG statistics:\n");
        graph->printGraphStatistics();
        trace( "\n");

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

    overAllTime_start = time (NULL);

    trace( "\n=================================================================\n");
    trace( "checking whether " + firstOG->getFilename() + " simulates " + secondOG->getFilename() + "...\n\n");
    if (firstOG->simulates(secondOG)) {
        TRACE(TRACE_1, "result: simulation holds: YES\n\n");
        trace( "The first OG characterizes all strategies of the second one.\n\n");
    } else {
        TRACE(TRACE_1, "result: simulation holds: NO\n\n");
        setExitCode(1);
        trace( "The second OG characterizes at least one strategy that is\n");
        trace( "not characterized by the first one.\n\n");
    }

    overAllTime_end = time (NULL);
    cout << "    " << difftime(overAllTime_end, overAllTime_start) << " s consumed for checking equivalence" << endl << endl;

    if (!calledWithNet) {
        trace( "Attention: This result is only valid if the given OGs are complete\n");
        trace( "           (i.e., \"-s empty\" option was set and \"-m\" option high enough)\n\n");
    } else if (netfiles.size() == 1) {
        trace( "Attention: This result is only valid if the given OG is complete\n");
        trace( "           (i.e., \"-s empty\" option was set and \"-m\" option high enough)\n\n");
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

    time_t  buildGraphTime_start, buildGraphTime_end,
            overAllTime_start, overAllTime_end,
            graphStatsTime_start, graphStatsTime_end,
            removeFalseNodesTime_start, removeFalseNodesTime_end,
            removeUnreachableNodesTime_start, removeUnreachableNodesTime_end;

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
        trace( "=================================================================\n");
        trace( "processing net " + currentowfnfile + "\n");
        reportNet();
        delete PlaceTable;

        overAllTime_start = time (NULL);

        // compute OG
        OG* graph = new OG(PN);

        trace( "building the operating guideline...\n");
        graph->printProgressFirst();

        overAllTime_start = time(NULL);

        TRACE(TRACE_1, "Building the graph...\n");
        buildGraphTime_start = time(NULL);
        graph->buildGraph(); // build operating guideline
        buildGraphTime_end = time(NULL);
        TRACE(TRACE_1, "finished building the graph\n");
        TRACE(TRACE_2, "    " + intToString((int) difftime(buildGraphTime_end, buildGraphTime_start)) + " s consumed.\n\n");

//        TRACE(TRACE_1, "computing graph statistics...\n");
//        graphStatsTime_start = time(NULL);
//        graph->computeGraphStatistics();
//        graphStatsTime_end = time(NULL);
//        TRACE(TRACE_1, "finished computing graph statistics\n");
//        TRACE(TRACE_2, "    " + intToString((int) difftime(graphStatsTime_end, graphStatsTime_end)) + " s consumed.\n\n");
//
//
//        if (!parameters[P_SHOW_RED_NODES] && !parameters[P_SHOW_ALL_NODES]) {
//            TRACE(TRACE_1, "removing false nodes...\n");
//            removeFalseNodesTime_start = time(NULL);
//            graph->removeReachableFalseNodes();
//            removeFalseNodesTime_end = time(NULL);
//            TRACE(TRACE_1, "finished removing false nodes\n");
//            TRACE(TRACE_2, "    " + intToString((int) difftime(removeFalseNodesTime_end, removeFalseNodesTime_start)) + " s consumed.\n\n");
//
//            TRACE(TRACE_1, "removing unreachable nodes...\n");
//            removeUnreachableNodesTime_start = time(NULL);
//            graph->removeUnreachableNodes();
//            removeUnreachableNodesTime_end = time(NULL);
//            TRACE(TRACE_1, "finished removing unreachable nodes\n");
//            TRACE(TRACE_2, "    " + intToString((int) difftime(removeUnreachableNodesTime_end, removeUnreachableNodesTime_end)) + " s consumed.\n\n");
//        }


        overAllTime_end = time(NULL);

        trace( "\nbuilding the operating guideline finished.\n");
        trace( "    " + intToString((int) difftime(overAllTime_end, overAllTime_start)) + " s overall consumed for OG computation.\n\n");

        overAllTime_end = time (NULL);
        cout << "    " << difftime(overAllTime_end, overAllTime_start) << " s consumed for building graph" << endl << endl;

        // print statistics
        trace( "OG statistics:\n");
        graph->printGraphStatistics();
        trace( "\n");

        // add new OG to the list
        if (!OGfirst && netfiles.size() == 1) {
            OGsFromFiles.push_front(graph);
        } else {
            OGsFromFiles.push_back(graph);
        }
        graph->deleteOWFN();

        netiter++;
    }

    trace( "=================================================================\n");
    trace( "\n\nchecking equivalence of the OGs...\n");

    AnnotatedGraph::ogs_t::const_iterator currentOGfile = OGsFromFiles.begin();
    AnnotatedGraph *firstOG = *currentOGfile;
    AnnotatedGraph *secondOG = *(++currentOGfile);

    firstOG->removeReachableFalseNodes();
    secondOG->removeReachableFalseNodes();

    overAllTime_start = time (NULL);

    TRACE(TRACE_1, "checking whether " + firstOG->getFilename() + " is equivalent to " + secondOG->getFilename() + "\n");

    // Check the equivalence
    bool areEquivalent = firstOG->isEquivalent(secondOG);

    trace( "\nresult: " + firstOG->getFilename() + " and " + secondOG->getFilename() + " are equivalent:");

    if (areEquivalent) {
        trace( " YES\n\n");
    } else {
        trace( " NO\n\n");
        setExitCode(1);
    }

    overAllTime_end = time (NULL);
    cout << "    " << difftime(overAllTime_end, overAllTime_start) << " s consumed for checking equivalence" << endl << endl;

    if (!calledWithNet && !parameters[P_EQ_R]) {
        trace( "Attention: This result is only valid if the given OGs are complete\n");
        trace( "           (i.e., \"-s empty\" option was set and \"-m\" option high enough)\n\n");
    } else if (netfiles.size() == 1 && !parameters[P_EQ_R]) {
        trace( "Attention: This result is only valid if the given OG is complete\n");
        trace( "           (i.e., \"-s empty\" option was set and \"-m\" option high enough)\n\n");
    }

    deleteOGs(OGsFromFiles);
}


//! \brief create a PNG of the given oWFN
//! \param PN an oWFN to generate a PNG from
void makePNG(oWFN* PN) {

    TRACE(TRACE_1, "Internal translation of the net into PNapi format...\n");

    // translate the net into PNapi format
    PNapi::PetriNet* PNapiNet = PN->returnPNapiNet();

    // set strings needed in PNapi output
    globals::output_filename = PN->filename;
    if ( PN->FinalCondition != NULL  ) {
        if ( PN->finalConditionString != "" ) {
            globals::filename = PN->filename + " \\n Final Condition: "
                    + PN->finalConditionString;
        } else {
            globals::filename = PN->filename + " \\n Final Condition: "
                    + PN->FinalCondition->getString();
        }
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
            trace( "File \"" + dotFileName + "\" could not be opened for writing access!\n");
           setExitCode(EC_FILE_ERROR);
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
            int exitvalue = system((string(CONFIG_DOT) + " -Tdot " + dotFileName + " -o " + outFileName + ".dot").c_str());
            if (exitvalue == 0) {
                trace( (outFileName + ".dot generated\n\n"));
            } else {
                setExitCode(EC_DOT_ERROR);
            }

            // transforming .dot file into gastex format
            makeGasTex((outFileName + ".dot"), (outFileName), GasTexGraph::STYLE_OWFN);
        }

        if (!parameters[P_NOPNG]) {
            // call dot to create the png file
            int exitvalue = system((string(CONFIG_DOT) + " -q -Tpng -o \"" + outFileName + ".png\" " + dotFileName).c_str());

            if (exitvalue == 0) {
                trace( (outFileName + ".png generated\n\n"));
            } else {
                trace( "error: Dot exited with non zero value! dort\n\n" + intToString(exitvalue));
            }
        }
    }
}


//! \brief reduce the given oWFN
//! \param PN an oWFN to reduce
void reduceOWFN(oWFN* PN) {

    TRACE(TRACE_1, "Internal translation of the net into PNapi format...\n");

    // translate the net into PNapi format
    PNapi::PetriNet* PNapiNet = PN->returnPNapiNet();

    // set strings needed in PNapi output
    globals::output_filename = PN->filename;
    globals::filename = PN->filename;

    // set the output format to dot
    PNapiNet->set_format(PNapi::FORMAT_OWFN, true);

    TRACE(TRACE_1, "Performing structural reduction ...\n\n");

    // calling the reduce function of the pnapi with reduction level 5
    PNapiNet->reduce(globals::reduction_level);

    // Statistics of the reduced oWFN
    trace( "Reduced oWFN statistics:\n");
    trace(PNapiNet->information());
    trace( "\n\n");

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
        if (!output.good()) {
            output.close();
            setExitCode(EC_FILE_ERROR);
        }
        (output) << (*PNapiNet);
        output.close();
    }

    delete PNapiNet;
}


//! \brief normalize the given oWFN
//! \param PN an oWFN to normalize
oWFN* normalizeOWFN(oWFN* PN) {

    TRACE(TRACE_5, "void normalizeOWFN(oWFN*) : start\n");

    // normalize
    trace( "Performing normalization ...\n\n");
    oWFN* normalOWFN = PN->returnNormalOWFN();


    // translate the net into PNapi format
    PNapi::PetriNet* PNapiNet = normalOWFN->returnPNapiNet();
    globals::output_filename = normalOWFN->filename; // set strings needed in PNapi output
    globals::filename = normalOWFN->filename;
    PNapiNet->set_format(PNapi::FORMAT_OWFN, true); // set the output format to dot


    // Statistics of the reduced oWFN
    trace( "Normalized oWFN statistics:\n");
    trace(PNapiNet->information());
    trace( "\n\n");


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
        if (!output.good()) {
            output.close();
            setExitCode(EC_FILE_ERROR);
        }

        (output) << (*PNapiNet);
        output.close();
    }

    delete PNapiNet;

    TRACE(TRACE_5, "void normalizeOWFN(oWFN*) : end\n");
    return normalOWFN;
}


//! \brief match a net against an (Constraint) OG
//! \param OGToMatch an OG to be matched against an oWFN
//! \param PN an oWFN to be matched against an OG
//! \param covConstraint constraint to match against when checking coverability
void checkMatching(list<AnnotatedGraph*>& OGsToMatch, oWFN* PN, GraphFormulaCNF* covConstraint = 0) {

    TRACE(TRACE_5, "void checkMatching(AnnotatedGraph*, oWFN*) : start\n");

    // normalize given PN if necessary
    oWFN* normalOWFN = PN;
    if ( !PN->isNormal() ) {
        trace("normalizing current petrinet " + PN->filename + "\n");
        // TODO: this is a dirty hack to prevent normalization from outputting
        // some files, but it is ok because there will be a general
        // reorganisation of output files soon
        bool temp = options[O_NOOUTPUTFILES];
        options[O_NOOUTPUTFILES] = true;
        normalOWFN = normalizeOWFN(PN);
        options[O_NOOUTPUTFILES] = temp;
    }

    // match the oWFN with given OG
    string reasonForFailedMatch;
    trace(("matching " + PN->filename + " and ...\n\n"));
    for (list<AnnotatedGraph*>::iterator OGToMatch = OGsToMatch.begin();
         OGToMatch != OGsToMatch.end(); ++OGToMatch) {
        // use only the labeled core

        oWFN* coreOWFN = normalOWFN->returnMatchingOWFN();

        if (parameters[P_COVER]) {
            if ( coreOWFN->matchesWithConstraintOG((*OGToMatch), covConstraint, reasonForFailedMatch) ) {
                TRACE(TRACE_1, "\n");
                trace(((*OGToMatch)->getFilename()) + ": YES\n");
            } else {
                TRACE(TRACE_1, "\n");
                TRACE(TRACE_1, "Match failed: " + reasonForFailedMatch + "\n\n");
                trace(((*OGToMatch)->getFilename()) + ": NO\n");
                setExitCode(1);
            }
        } else {
            if (coreOWFN->matchesWithOG((*OGToMatch), reasonForFailedMatch) ) {
                TRACE(TRACE_1, "\n");
                trace(((*OGToMatch)->getFilename()) + ": YES\n");
            } else {
                TRACE(TRACE_1, "\n");
                trace("Match failed: " + reasonForFailedMatch + "\n\n");
                trace(((*OGToMatch)->getFilename()) + ": NO\n");
                setExitCode(1);
            }
        }
        delete coreOWFN;
    }
    trace("\n");


    // garbage collection
    if ( normalOWFN != PN ) delete normalOWFN;

    TRACE(TRACE_5, "void checkMatching(AnnotatedGraph*, oWFN*) : start\n");
}


//! \brief compute the number of tree service automata that match with the given OG
//! \param OG an og to compute the number of matching tree service automata for
//! \param graphName a string with the name of the file, the og was taken from
void countStrategies(AnnotatedGraph* OG, string graphName) {

    trace("Processing: ");
    trace(graphName);
    trace("\n");

    TRACE(TRACE_1, "Checking if the OG is acyclic\n");

    if (OG->isAcyclic()) {

        time_t countingTime_start, countingTime_end;

        countingTime_start = time (NULL);
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
        countingTime_end = time (NULL);

        cout << difftime(countingTime_end, countingTime_start) << " s consumed for computation" << endl << endl;

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

    TRACE(TRACE_1, "Checking if the OG is acyclic\n");

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

    // print debug information about system variables, the current compilation
    // in case of unusal behavior of Fiona, with this information it might be easier
    // to track down the bug(s)
    if (argc == 2 && std::string(argv[1]) == "--bug") {
        printf("\n\n");
        printf("Please email the following information to %s:\n", PACKAGE_BUGREPORT);
        printf("- tool:               %s\n", PACKAGE_NAME);
        printf("- version:            %s\n", PACKAGE_VERSION);
        printf("- compilation date:   %s\n", __DATE__);
        printf("- compiler version:   %s\n", __VERSION__);
        printf("- platform:           %s\n", CONFIG_BUILDSYSTEM);
        printf("- config ASSERT:      %s\n", CONFIG_ENABLEASSERT);
        printf("- config UNIVERSAL:   %s\n", CONFIG_ENABLEUNIVERSAL);
        printf("- config ENABLE64BIT: %s\n", CONFIG_ENABLE64BIT);
        printf("- config WIN32:       %s\n", CONFIG_ENABLEWIN32);
        printf("- external dot:       %s\n", CONFIG_DOT);
        printf("- external Petrify:   %s\n", CONFIG_PETRIFY);
#ifdef LOG_NEW
        printf("- use the newlogger:  yes\n");
#else
        printf("- use the newlogger:  no\n");
#endif
        printf("\n\n");

        exit(0);
    }


    // [LUHME XV] IFDEF ueberpruefen
    // [LUHME XV] Jans New-Handler
    set_new_handler(&myown_newhandler);

    // evaluate command line options
    parse_command_line(argc, argv);
#ifdef LOG_NEW
    // Add the at exit function for the newlogger
    atexit(newloggerAtExit);
#endif



    // **********************************************************************************
    // ********                start dot file dependant operations               ********
    // **********************************************************************************

    // [LUHME XV] Parameter "-t tex" ist evtl. überflüssig
    // [LUHME XV] gehört hier nicht hin
    if (parameters[P_GASTEX]) {
        // if -t tex was called, then outfileprefix still contains file suffix

        // [LUHME XV] es gibt eine Funktion "basename()" um Dateinamen/Erweiterung zu managen
        // remember suffix in dotFileName
        string dotFileName = outfilePrefix;
        // try to remove .out suffix
        static const string dotFileSuffix = ".out";
        if (outfilePrefix.substr(outfilePrefix.size() - dotFileSuffix.size()) == dotFileSuffix) {
            outfilePrefix=outfilePrefix.substr(0, outfilePrefix.size() - dotFileSuffix.size());
        }

        //! \TODO: find out of which type the graph in the dot file is (og? owfn?)
        makeGasTex(dotFileName, outfilePrefix, GasTexGraph::STYLE_OG);
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
            trace( "OG was read from file\n");
            trace( "HIT A KEY TO CONTINUE"); getchar();

            // only print OG size information
            return 0;
        }
*/

    // [LUHME XV] Soll hier tatsächlich schon aus der main() gesprungen werden?
    // [LUHME XV] Wegen Speicher-Aufräumen??? Dateien schließen? Newlogger-Ausgaben?
    // [LUHME XV] gleiches für P_SIMULATES und P_EX Rufe
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

            return(globalExitCode);
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

        // [LUHME XV] Fehlermeldung auslagern?
        // [LUHME XV] Generell Fehlermeldungen in eine Datei auslagern und mit Fehlercode rufen (siehe BPEL2oWFN)
        // Abort if there are no OGs at all
        if (ogfiles.begin() == ogfiles.end() && !(parameters[P_PV])) {
            trace("Error:  No OGs have been given for computation\n\n");
            trace("        Enter \"fiona --help\" for more information\n\n");
            setExitCode(EC_BAD_CALL);
        }

        // iterate all input files
        for (AnnotatedGraph::ogfiles_t::const_iterator iOgFile = ogfiles.begin(); iOgFile != ogfiles.end(); ++iOgFile) {

            // [LUHME XV] Was wird hier wirklich gelesen? IGs? OGs? Annotated graphs? ???
            // [LUHME XV] Warum ist readOG ein Pointer? Objekt reicht? Konstruktoraufruf stattdessen?
            AnnotatedGraph* readOG = readog(*iOgFile);

            if (parameters[P_SYNTHESIZE_PARTNER_OWFN]) {
                // computes partner out of IG or a most permissive partner from an og

                // it is possible that we read an old OG file where no interface information
                // were stored, so we have to compute them from the graph
                if (readOG->inputPlacenames.empty() && readOG->outputPlacenames.empty()) {
                    readOG->computeInterfaceFromGraph();
                }

                // [LUHME XV] "smallpartner" und "mostpermissivepartner"-Optionen umbennennen
                // [LUHME XV] "-t smallpartner" -> "-t partner"
                // [LUHME XV] "-t mostpermissivepartner" -> "-t partner --mostpermissive"
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
                Graph* PV = computePublicView(readOG, (*iOgFile), false);
                if (PV != NULL) {
                    outputPublicView((*iOgFile), PV, false);
                    delete PV;
                }
                delete readOG;
            }

            else if (parameters[P_MINIMIZE_OG]) {
                // minimizes a given OG

                trace( "=================================================================\n");
                trace( "processing OG " + readOG->getFilename() + "\n");
                readOG->computeAndPrintGraphStatistics();
                trace("\n");

                //readOG->removeReachableFalseNodes();
                readOG->minimizeGraph();

                trace( "size after minimization:\n");
                readOG->computeAndPrintGraphStatistics();
                trace("\n");

                string ogFilename("");

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
                    readOG->createOGFile(ogFilename, false);
                    createOutputFiles(readOG, ogFilename, ogFilename);
                }
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                trace( "=================================================================\n");
                trace("\n");
                delete readOG;
            }

            else if (parameters[P_COUNT_SERVICES]) {
                // counts the number of deterministic strategies
                // that are characterized by a given OG
                readOG->removeReachableFalseNodes();
                countStrategies(readOG, (*iOgFile));
                delete readOG;
            }

            // [LUHME XV] als Paramter raus??
            else if (parameters[P_CHECK_ACYCLIC]) {
                // counts the number of deterministic strategies
                // that are characterized by a given OG
                readOG->removeReachableFalseNodes();
                checkAcyclicity(readOG, (*iOgFile));
                delete readOG;
            }

            // [LUHME XV] Raus?
            else if (parameters[P_CHECK_FALSE_NODES]) {
                // checks if there are nodes in the og which violate the annotation
                trace( "Checking '" + readOG->getFilename() + "' ");
                trace( "for nodes with false annotation...");

                std::vector<AnnotatedGraphNode*> falseNodes;
                readOG->findFalseNodes(&falseNodes);

                if (falseNodes.size() == 0) {
                    trace( "\n     No nodes with false annotation found!");
                } else {
                    trace( "\n     Node(s) with false annotation found!");
                    std::vector<AnnotatedGraphNode*>::iterator it = falseNodes.begin();

                    while (it != falseNodes.end()) {
                        AnnotatedGraphNode* currentNode = *it;
                        TRACE(TRACE_1, "\n         Node '" + currentNode->getName() + "' violates its annotation. ");
                        TRACE(TRACE_2, "\n             Annotation is: " + currentNode->getAnnotationAsString());
                        ++it;
                    }
                }

                trace( "\n\n");
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

                if(TRACE_1 <= debug_level) {
                    readOG->computeAndPrintGraphStatistics();
                }

                time_t removingNodesTime_start, removingNodesTime_end;

                trace( "Removing nodes from '" + readOG->getFilename() + "' ");
                trace( "that violate their own annotation...\n");
                removingNodesTime_start = time(NULL);
                readOG->removeReachableFalseNodes();
                removingNodesTime_end = time(NULL);
                trace( "Removed all nodes with false annotation.\n");
                cout << "    " << difftime(removingNodesTime_end, removingNodesTime_start) << " s consumed for removing reachable false nodes" << endl << endl;

                trace( "Removing unreachable nodes from '" + readOG->getFilename() + "' \n");
                removingNodesTime_start = time(NULL);
                readOG->removeUnreachableNodes();
                removingNodesTime_end = time(NULL);
                trace( "Removed all unreachable nodes.\n");
                cout << "    " << difftime(removingNodesTime_end, removingNodesTime_start) << " s consumed removing unreachable nodes" << endl << endl;

                if (TRACE_1 <= debug_level) {
                    readOG->computeAndPrintGraphStatistics();
                }

                if (!options[O_NOOUTPUTFILES]) {
                    trace( "\nCreating new .og-file without false nodes... \n");
                    // the second parameter is false, since the read OG has no underlying oWFN
                    readOG->createOGFile(newFilename, false);
                    trace( "New .og-file '" + newFilename + ".og' successfully created.\n\n");
                }

                delete readOG;

            }

            // [LUHME XV] Parameter in "P_READ_OG_ONLY" umbenennen
            else if (parameters[P_READ_OG]) {
                trace( "OG was read from file '" + readOG->getFilename() + "'\n");
                readOG->computeAndPrintGraphStatistics();
                trace( "HIT A KEY TO CONTINUE"); getchar();
                trace( "\n");
                delete readOG;
            }

            else if (parameters[P_PNG]) {
                trace( "=================================================================\n");
                trace( "Processing OG " + readOG->getFilename() + "\n");
                readOG->computeAndPrintGraphStatistics();
                string newFilename;
                if (options[O_OUTFILEPREFIX]) {
                    newFilename = outfilePrefix;
                } else {
                    newFilename = readOG->getFilename();
                }
                createOutputFiles(readOG, newFilename, newFilename);
                //trace( (newFilename + ".png generated\n\n"));
                delete readOG;
            }
        }

// [LUHME XV] Wurde das oben nicht schon alles gemacht? Wurde zwischenzeitlich noch was eingelesen?
#ifdef YY_FLEX_HAS_YYLEX_DESTROY
        // Destroy buffer of OG parser.
        // Must NOT be called before fclose(og_yyin);
        og_yylex_destroy();
#endif


        // [LUHME XV] WTF?
        if (!parameters[P_PV] && !parameters[P_PNG]) {
        	// [LUHME XV] Warum an dieser Stelle?


        	exit(globalExitCode);
        }
    } //// END OF OG INPUT

    // **********************************************************************************
    // ********                start PN file dependent operations                ********
    // **********************************************************************************

    // netfiles only used for getting the correct name strings
    if (parameters[P_EX] && options[O_BDD]) {
        // checking exchangeability using BDDs

        // [LUHME XV] Hier soll eine vernünftige Fehlermeldung hin
        assert(netfiles.size() == 2);
        // the BDD representations of the given nets are assumed to exist already

        // [LUHME XV] Klasse "Exchangeability" umbennen?
        // [LUHME XV] Code mit Christian S. Austauschbarkeit zusammenlegen
        // [LUHME XV] hier nicht "netfiles" benutzen
        list<std::string>::iterator netiter = netfiles.begin();
        Exchangeability* og1 = new Exchangeability(*netiter);
        Exchangeability* og2 = new Exchangeability(*(++netiter));

        // [LUHME XV] Ausgabe "YES"/"NO" in og->check() auslagern?
        // [LUHME XV] "check" in "checkEquivalence" umbenennen?
        // [LUHME XV] Variablennamen ändern; irgendwas mit "BDD-OG"
        trace( "The two operating guidelines are equal: ");
        if (og1->check(og2) == true) {
            trace( "YES\n\n");
        } else {
            trace( "NO\n\n");
            setExitCode(1);
        }
        // return 0;
    }
    else if (parameters[P_ADAPTER] || parameters[P_SMALLADAPTER]) {

        Adapter adapter;
        // [LUHME XV] Netze explizit übergeben
        adapter.generate();
    }
    else if (parameters[P_IG] || parameters[P_OG] || parameters[P_MATCH] ||
             parameters[P_PNG] || parameters[P_REDUCE] || parameters[P_NORMALIZE] ||
             parameters[P_PV]) {

        list<AnnotatedGraph*> OGsToMatch;
        GraphFormulaCNF* covConstraintToMatch = NULL;
        if (parameters[P_MATCH]) {
            // iterate all og files
            for (AnnotatedGraph::ogfiles_t::const_iterator iOgFile = ogfiles.begin();
                 iOgFile != ogfiles.end(); ++iOgFile) {
                if (parameters[P_COVER]) {
                    OGsToMatch.push_back(readconstraintog(*(iOgFile), covConstraintToMatch));
                } else {
                    OGsToMatch.push_back(readog(*(iOgFile)));
                }
            }
        }

        string fileName;    // name of og-file
        ogfiles.clear();

        // ---------------- processing every single net -------------------
        for (list<std::string>::iterator netiter = netfiles.begin(); netiter != netfiles.end(); ++netiter) {

            // [LUHME XV] Fliegt irgendwann raus.
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

                // [LUHME XV] gehört in den Parser
                // prepare getting the net
                PlaceTable = new SymbolTab<PlSymbol>;

                // [LUHME XV] readnet() und reportNet() als Members der Klasse "oWFN"
                trace( "=================================================================\n");
                trace( "processing net " + currentowfnfile + "\n");
                // get the net
                // [LUHME XV] Wo kommt "PN" her? Wird unten gebraucht aber vom Parser initialisiert... WTF
                readnet(currentowfnfile);
                reportNet();
                delete PlaceTable;

                // [LUHME XV] Mal in LoLA oder BPEL2oWFN gucken, wie man dort peibt
                if (currentowfnfile == "<stdin>") {
                    currentowfnfile = "stdin";
                }

                // [LUHME XV] verschieben
                fileName = "";

                // start computation
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
                    //    fileName = computeOG(PN);    // computing OG of the current oWFN
                    //}

                    // computing OG of the current oWFN
                    fileName = computeOG(PN);
                }

                if (parameters[P_MATCH]) {
                    // matching the current oWFN against the current OG
                    if(parameters[P_COVER]) {
                        checkMatching(OGsToMatch, PN, covConstraint);
                    } else {
                        checkMatching(OGsToMatch, PN);
                    }
                }

                if (parameters[P_PNG]) {
                    makePNG(PN); // create a png file of the given net
                }

                if (parameters[P_REDUCE]) {
                    reduceOWFN(PN);    // reduce given net
                }

                if (parameters[P_NORMALIZE]) {
                    oWFN* normalOWFN = normalizeOWFN(PN); // normalize given net
                    delete normalOWFN;
                }

                if (parameters[P_EQ_R]) {
                    // reverse reduction mode for the next loop
                    options[O_CALC_ALL_STATES] = !options[O_CALC_ALL_STATES];
                    // remember file name of og-file to check equivalence later on
                    ogfiles.push_back(AnnotatedGraph::addOGFileSuffix(fileName));
                }

                //delete PN;
                //TRACE(TRACE_5, "net deleted\n");
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

    return(globalExitCode);

}


// [LUHME XV] Sowas gibt es als Standardmethode "basename()"
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


