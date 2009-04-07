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
 * \file    adapter.cc
 *
 * \brief   all functionality for adapter generation
 *
 * \author  responsible: Christian Gierds <gierds@informatik.hu-berlin.de>
 */

#include "adapter.h"
#include "debug.h"
#include "helpers.h"
#include "pnapi.h"
#include "options.h"
#include "owfn.h"
#include "state.h"

#include <cassert>
#include <fstream>
#include <iostream>
#include <sstream>
#include <list>
#include <string>
#include <ctime>

using namespace std;
using namespace PNapi;


// global variables - used for parsing rules file
// remember: typedef pair< list< string >, list< string > > adapterRule;
list< adapterRule > hiddenRules;       // from parser: read rules for hidden communication
list< adapterRule > observableRules;   // from parser: read rules for observable communication
list< adapterRule > controllableRules; // from parser: read rules for controllable communication
list< adapterRule > totalRules;        // from parser: read rules for total (observable and controllable) communication
map< string, ruleType > ruleTypePerChannel;         // from parser: ruleType per channel
map< string, unsigned int > consumeRulesPerChannel; // from parser: number of rules which consume from channel
//map< string, unsigned int > produceRulesPerChannel; // from parser: number of rules which produce for channel



//! \brief a basic constructor of Adapter
Adapter::Adapter()
{
    TRACE(TRACE_5, "Adapter::Adapter() : start\n");

    // initialize member variables

    TRACE(TRACE_5, "Adapter::Adapter() : end\n");
}


//! \brief a basic destructor of Adapter
Adapter::~Adapter()
{
    TRACE(TRACE_5, "Adapter::~Adapter() : start\n");

    // delete member variables

    TRACE(TRACE_5, "Adapter::~Adapter() : end\n");
}


//! \brief open output file
//! \param name filename of the output file
//! \return output file stream to the output file
//! \post output file with filename name opened and bound to stream file
ostream * Adapter::openOutput(string name)
{
  ofstream * file = new ofstream(name.c_str(), ofstream::out | ofstream::trunc | ofstream::binary);

  if (!file->is_open())
  trace( "File \"" + name + "\" could not be opened for writing access!\n");

  return file;
}


//! \brief close output file
//! \param file file stream of the output file
//! \post out file addressed by output file stream #file closed
void Adapter::closeOutput(ostream * file)
{
  if ( file != NULL )
  {
    (*file) << flush;
    (static_cast<ofstream*>(file))->close();
    delete(file);
    file = NULL;
  }
}


//! \brief output given petrinet as oWFN withg given prefix
//! \param petrinet a pointer to a petrinet
//! \param prefix a string which contains the prefix of the created oWFN file
void Adapter::outputPNasOWFN(PNapi::PetriNet * petrinet, string prefix)
{
    ostream * output = openOutput(prefix + ".owfn");

    TRACE(TRACE_1, "-> Printing Petri net " + prefix + " as oWFN ...\n");
    petrinet->set_format(PNapi::FORMAT_OWFN);
    (*output) << *petrinet;

    closeOutput(output);
    output = NULL;
}


//! \brief output given petrinet as PNG with given prefix
//! \param petrinet a pointer to a petrinet
//! \param prefix a string which contains the prefix of the created oWFN file
void Adapter::outputPNasPNG(PNapi::PetriNet * petrinet, string prefix)
{
	if ( !parameters[P_NODOT] && !parameters[P_NOPNG] )
	{
	    ostream * output = openOutput(prefix + ".dot");

	    TRACE(TRACE_1, "-> Printing Petri net " + prefix + " as PNG ...\n");
	    petrinet->set_format(PNapi::FORMAT_DOT);
	    (*output) << *petrinet;

	    closeOutput(output);

		string systemcall = string(CONFIG_DOT) + " -q -Tpng -o\"" + prefix + ".png\" \"" + prefix + ".dot\"";
		TRACE(TRACE_1, "Invoking dot with the following options:\n");
		TRACE(TRACE_1, systemcall + "\n\n");
		system(systemcall.c_str());
	}
}


//! \brief read service from file and combine current prefix with given prefixes
//! \param filename a string containing the filename of the service
//! \param netname a reference to current netname
//! \return a pointer to the read petrinet
PNapi::PetriNet * Adapter::readService(string filename, string & netname)
{
    extern void readnet(const string & owfnfile); // extern function to read oWFN from file
    //extern void reportNet();					  // function to print report about a net
    extern oWFN * PN;							  // pointer to global oWFN PN


    // read current netname
    assert(filename != "");
    netname = filename.substr(0, filename.find_last_of("."));
    if (netname.find_last_of("/") != string::npos)
    {
    	netname = netname.substr(netname.find_last_of("/") + 1);
    }
    else if (netname.find_last_of("\\") != string::npos)
    {
    	netname = netname.substr(netname.find_last_of("\\") + 1);
    }
    TRACE(TRACE_1, "-> reading net:\t\t" + filename + "\n");
    TRACE(TRACE_1, "netname:\t\t" + netname + "\n");
    assert(netname != "");
    
    
    // get oWFN into variable PN and convert it to petrinet
    PlaceTable = new SymbolTab<PlSymbol>;
    readnet(filename);
    delete PlaceTable;
    assert(PN != NULL);

    PNapi::PetriNet * currentPN = PN->returnPNapiNet();
    assert(currentPN != NULL);
    PN = NULL;

    return currentPN;
}


//! \brief expand rewriter with regard to the interface of given service
//! \param pattern a pointer to the current rewriter
//! \param service a pointer to the service whose interface is used to expand the rewriter
void Adapter::expandRewriterWithInterface(PetriNet * rewriter, PetriNet * service) {

    assert(rewriter != NULL);
    assert(service != NULL);

    // We iterate over all interface places from given service and expand the
    // rewriter by creating input/output pattern for each interface place.
    set<Place*> interface = service->getInterfacePlaces();
    for (set<Place*>::iterator place = interface.begin(); place != interface.end(); place++) {

        // WARNING - Handle internal name creation with care:
        // - all service places are disjoint and we added a prefix
        //   consisting of a non-negative number and '_' (eg. "0_")
        // - all internal rewriter places get a prefix "E_" by definition
        //
        // - all interface rewriter places which are used to communicate with
        //   the adapter controller about sending to/receiving from the given
        //   services get a prefix "S_" or "R_" by definition
        // - all transitions from/to interface rewriter places mentioned above
        //   get a prefix "ST_" or "RT_" by definition
        //
        // - all interface rewriter places which are used to communicate with
        //   the adapter controller about the execution/finished execution of
        //   given rules get a name "X_RuleN" or "F_RuleN" by definition (N >= 0)
        // - all transition from/to interface rewriter places mentioned above
        //   get a name "XT_RuleN" by definition (N >= 0)
        //
        // - all interface rewriter places which are used to communicate with
        //   the given services keep their name (because they will be composed)
        assert(*place != NULL);
        string placeName = (*place)->nodeFullName();
        string oldName = placeName.substr( placeName.find_first_of("_") + 1 );

        // Handle input place by creating a pattern which allows the adapter to
        // send a message from an internal place to the input place. This
        // sending can be triggerd from the adapter controller.
        if ((*place)->type == PNapi::IN) {

            Place * p_out      = rewriter->newPlace(placeName, PNapi::OUT);
            Place * p_internal = rewriter->findPlace("E_" + oldName);
            if (p_internal == NULL) {
                p_internal = rewriter->newPlace("E_" + oldName, PNapi::INTERNAL, messages_manual);
            }

            // Create a transition for sending and 2 corresponding arcs.
            Transition * t = rewriter->newTransition("ST_" + oldName);
            rewriter->newArc(t, p_out);
            rewriter->newArc(p_internal, t);

            // Create an interface place for communication with the adapter
            // controller if and only if the handled input place is used in
            // standard adapter rules.
            if ( ruleTypePerChannel[oldName] == ADAPT_TOTAL ) {
                Place * p_trigger  = rewriter->newPlace("S_" + oldName, PNapi::IN);
                rewriter->newArc(p_trigger, t);

                // There may be a conflict between the sending transition and
                // one or more internal rules. This information is needed because
                // of optimization 1 (look at method expandRewriterWithRules).
                consumeRulesPerChannel[oldName]++;

                //cerr << "DEBUG handled input for standard rule" << endl;
            } else {
                //cerr << "DEBUG handled input for nonstandard rule" << endl;
            }
        }
        // Handle output place by creating a pattern which allows the adapter to
        // transfer a message from the output place to an internal place. This
        // receiving can trigger the adapter controller.
        else if ((*place)->type == OUT) {

            Place * p_in       = rewriter->newPlace(placeName, PNapi::IN);
            Place * p_internal = rewriter->findPlace("E_" + oldName);
            if (p_internal == NULL) {
                p_internal = rewriter->newPlace("E_" + oldName, PNapi::INTERNAL, messages_manual);
            }

            // Create a transition for sending and 2 corresponding arcs.
            Transition * t = rewriter->newTransition("RT_" + oldName);
            rewriter->newArc(p_in, t);
            rewriter->newArc(t, p_internal);

            // Create an interface place for communication with the adapter
            // controller if and only if the handled output place is used in
            // standard adapter rules.
            if ( ruleTypePerChannel[oldName] == ADAPT_TOTAL ) {
                Place * p_trigger  = rewriter->newPlace("R_" + oldName, PNapi::OUT);
                rewriter->newArc(t, p_trigger);

                //cerr << "DEBUG handled input for standard rule" << endl;
            } else {
                //cerr << "DEBUG handled output for nonstandard rule" << endl;
            }
        }
    }
}


void Adapter::expandRewriterWithHiddenRules(PNapi::PetriNet* rewriter) {

    assert(rewriter != NULL);

    // This implements the idea of a partial communication flow:
    // Invisible communication is invisible to the controller.
    unsigned int ruleNr = 0;
    for(list<adapterRule>::iterator rule = hiddenRules.begin(); rule != hiddenRules.end(); rule++) {

        Transition * t = rewriter->newTransition("XT_invisibleRule" + intToString(ruleNr));

        // handle first channel list of current adapter rule
        TRACE(TRACE_1, "handling invisible rule:\t");
        for(list<string>::iterator channel = rule->first.begin(); channel != rule->first.end(); channel++) {

            // handle a channel as internal place
            TRACE(TRACE_1, *channel + " ");
            Place * p = rewriter->findPlace("E_" + *channel);
            assert(p != NULL);
            rewriter->newArc(p, t);
        }

        // handle second channel list of current adapter rule
        TRACE(TRACE_1, " -> ");
        for(list< string >::iterator channel = rule->second.begin(); channel != rule->second.end(); channel++) {

            // handle a channel as internal place
            TRACE(TRACE_1, *channel + " ");
            Place * p  = rewriter->findPlace("E_" + *channel);
            assert(p != NULL);
            rewriter->newArc(t, p);
        }
        TRACE(TRACE_1, "\n");
        
        ruleNr++;
    }

    // garbage collection
    hiddenRules.clear();
}


void Adapter::expandRewriterWithObservableRules(PNapi::PetriNet* rewriter) {

    assert(rewriter != NULL);

    // This implements the idea of a partial communication flow:
    // Visible communication is only visible to the controller, but cannot be
    // influenced.
    unsigned int ruleNr = 0;
    for(list<adapterRule>::iterator rule = observableRules.begin(); rule != observableRules.end(); rule++) {

        Transition * t = rewriter->newTransition("XT_visibleRule" + intToString(ruleNr));

        // handle first channel list of current adapter rule
        TRACE(TRACE_1, "handling visible rule:\t");
        for(list<string>::iterator channel = rule->first.begin(); channel != rule->first.end(); channel++) {

            // handle a channel as internal place
            TRACE(TRACE_1, *channel + " ");
            Place * p = rewriter->findPlace("E_" + *channel);
            assert(p != NULL);
            rewriter->newArc(p, t);
        }

        // This is a visible rule, so the controller will be noticed after
        // finished execution with the help of an output interface place.
        PNapi::Place * p2 = rewriter->newPlace("F_visibleRule" + intToString(ruleNr), PNapi::OUT);
        rewriter->newArc(t, p2);

        // handle second channel list of current adapter rule
        TRACE(TRACE_1, " -> ");
        for(list< string >::iterator channel = rule->second.begin(); channel != rule->second.end(); channel++) {

            // handle a channel as internal place
            TRACE(TRACE_1, *channel + " ");
            Place * p  = rewriter->findPlace("E_" + *channel);
            assert(p != NULL);
            rewriter->newArc(t, p);
        }
        TRACE(TRACE_1, "\n");
        
        ruleNr++;
    }

    // garbage collection
    observableRules.clear();
}


void Adapter::expandRewriterWithControllableRules(PNapi::PetriNet* rewriter) {

    assert(rewriter != NULL);

    // This implements the idea of a partial communication flow:
    // Activatable communication is invisible to the controller, but can be
    // influenced.
    unsigned int ruleNr = 0;
    for(list<adapterRule>::iterator rule = controllableRules.begin(); rule != controllableRules.end(); rule++) {

        Transition * t = rewriter->newTransition("XT_activatableRule" + intToString(ruleNr));

        // handle first channel list of current adapter rule
        TRACE(TRACE_1, "handling activatable rule:\t");
        for(list<string>::iterator channel = rule->first.begin(); channel != rule->first.end(); channel++) {

            // handle a channel as internal place
            TRACE(TRACE_1, *channel + " ");
            Place * p = rewriter->findPlace("E_" + *channel);
            assert(p != NULL);
            rewriter->newArc(p, t);
        }

        // This is a visible rule, so the controller will be noticed after
        // finished execution with the help of an output interface place.
        PNapi::Place * p1 = rewriter->newPlace("F_activatableRule" + intToString(ruleNr), PNapi::IN);
        rewriter->newArc(p1, t);

        // handle second channel list of current adapter rule
        TRACE(TRACE_1, " -> ");
        for(list< string >::iterator channel = rule->second.begin(); channel != rule->second.end(); channel++) {

            // handle a channel as internal place
            TRACE(TRACE_1, *channel + " ");
            Place * p  = rewriter->findPlace("E_" + *channel);
            assert(p != NULL);
            rewriter->newArc(t, p);
        }
        TRACE(TRACE_1, "\n");
        
        ruleNr++;
    }

    // garbage collection
    controllableRules.clear();
}


//! \brief read given rules from given *.ar file and expand given rewriter
//! \param a petrinet pointer to a net which contains the current adapter rewriter
//! \param an integer representing the optimization level that should be used
void Adapter::expandRewriterWithTotalRules(PNapi::PetriNet * rewriter, int level) {

    assert(rewriter != NULL);

    // We expand the given rewriter with one pattern per read rule, that
    // means one transition and the corresponding places (for communication
    // with the controller).
    // Here is the moment where some optimizations fit in, this is because the
    // less interface places the rewriter has the less time we need to compute
    // the corresponding controller for the adapter.
    // Optimization:
    // A rule is conflictfree iff it has a non-empty first channel list and all
    // channels in the first channel list appear in maximal one rule.
    // Then there is no need to control the execution of this rule, that means
    // we can abandon the corresponding interface places.
    unsigned int ruleNr = 0;
    for(list<adapterRule>::iterator rule = totalRules.begin(); rule != totalRules.end(); rule++) {

        // WARNING - Handle internal name creation with care:
        // - all service places are disjoint and we added a prefix
        //   consisting of a non-negative number and '_' (eg. "0_")
        // - all internal rewriter places get a prefix "E_" by definition
        //
        // - all interface rewriter places which are used to communicate with
        //   the adapter controller about sending to/receiving from the given
        //   services get a prefix "S_" or "R_" by definition
        // - all transitions from/to interface rewriter places mentioned above
        //   get a prefix "ST_" or "RT_" by definition
        //
        // - all interface rewriter places which are used to communicate with
        //   the adapter controller about the execution/finished execution of
        //   given rules get a name "X_RuleN" or "F_RuleN" by definition (N >= 0)
        // - all transition from/to interface rewriter places mentioned above
        //   get a name "XT_RuleN" by definition (N >= 0)
        //
        // - all interface rewriter places which are used to communicate with
        //   the given services keep their name (because they will be composed)
        Transition * t = rewriter->newTransition("XT_standardRule" + intToString(ruleNr));

        // Optimization
        bool conflictfree = (rule->first.size() > 0) ? true : false;


        // handle first channel list of current adapter rule
        TRACE(TRACE_1, "handling rule:\t");
        for(list<string>::iterator channel = rule->first.begin(); channel != rule->first.end(); channel++) {

            // Optimization
            conflictfree = conflictfree && (consumeRulesPerChannel[*channel] <= 1);
            //cerr << "\nDEBUG consumeRulesPerChannel[" << *channel << "] = " << consumeRulesPerChannel[*channel] << endl;

            // handle a channel as internal place
            TRACE(TRACE_1, *channel + " ");
            Place * p = rewriter->findPlace("E_" + *channel);
            if (p == NULL) {

                p = rewriter->newPlace("E_" + *channel, PNapi::INTERNAL, messages_manual);
                //cerr << "DEBUG created place E_ " << *channel << endl;
            }
            rewriter->newArc(p, t);
        }


        // We give control about the execution of this rule to the adapter
        // controller with the help of two interface places.
        //cerr << "\nDEBUG conflictfree = " << conflictfree << endl;
        if (!conflictfree) {
            PNapi::Place * p1 = rewriter->newPlace("X_standardRule" + intToString(ruleNr), PNapi::IN);
            PNapi::Place * p2 = rewriter->newPlace("F_standardRule" + intToString(ruleNr), PNapi::OUT);
            rewriter->newArc(p1, t);
            rewriter->newArc(t, p2);
        }


        // handle second channel list of current adapter rule
        TRACE(TRACE_1, " -> ");
        for(list< string >::iterator channel = rule->second.begin(); channel != rule->second.end(); channel++) {

            // handle a channel as internal place
            TRACE(TRACE_1, *channel + " ");
            Place * p  = rewriter->findPlace("E_" + *channel);
            if (p == NULL) {
                p = rewriter->newPlace("E_" + *channel, PNapi::INTERNAL, messages_manual);
                //cerr << "DEBUG created place E_ " << *channel << endl;
            }
            rewriter->newArc(t, p);
        }
        TRACE(TRACE_1, "\n");

        ruleNr++;
    }

    // garbage collection
    totalRules.clear();
    ruleTypePerChannel.clear();
    consumeRulesPerChannel.clear();
    //produceRulesPerChannel.clear();
}


//! \brief main function for generating adapter
void Adapter::generate()
{
    TRACE(TRACE_5, "Adapter::generate() : start\n");

    // without output files we cannot compute the controller
    assert(!options[O_NOOUTPUTFILES]);

    extern list<string> netfiles;          // list of all input (owfn) files
    extern string adapterRulesFile;        // name of the rules file
    time_t secondsBefor, secondsAfter;     // used to measure time
    string outputPrefix = "";              // prefixes seperated by _
    list< PNapi::PetriNet * > serviceList; // list of the services
    PNapi::PetriNet * adapterRewriter = new PNapi::PetriNet(); // rewriter (rules part) of adapter


    // First we read all given oWFNs and store them in our service list.
    // Furthermore they have to be renamend in order to avoid confusion
    // between newly created places/transitions in the adapter and
    // places/transitions from the read oWFNs. Therefor the read oWFNs (this
    // includes all places/transitions) get an integer from 0 to k as prefix
    // (seperated by _ ) and the adapter gets an E as prefix (seperated by _ ).
    // Additionally outputPrefix stores the prefix of the output files which
    // is a combination of all given oWFNs or the given prefix if Fiona was
    // called with option -o.
    TRACE(TRACE_1, "=================================================================\n");
    TRACE(TRACE_1, "reading service(s)\n");
    TRACE(TRACE_1, "=================================================================\n");
    secondsBefor = time(NULL);

    // iterate over all given oWFNs
    unsigned int prefix = 0;
    for(list<string>::iterator netiter = netfiles.begin(); netiter != netfiles.end(); netiter++)
    {
        // read a net and it's name
    	string currentName = "";
        PNapi::PetriNet * currentPN = readService( *netiter, currentName );

        // combine the names in outputPrefix, add a unique prefix to the oWFN
        // and add it to the service list
        outputPrefix += ( currentName + "_" );
        currentPN->addPrefix(intToString(prefix) + '_', true);
        serviceList.push_back(currentPN);

        // debug information
        //if ( debug_level >= 4 ) {
        //    outputPNasPNG( currentPN, intToString(prefix) + '_' + currentName );
        //    if ( !options[O_OUTFILEPREFIX] ) {
        //        TRACE(TRACE_4, "outputPrefix:\t" + outputPrefix + "\n\n");
        //    }
        //}

        prefix++;
    }

    // implements option -o, outfilePrefix comes from options.h
    if (options[O_OUTFILEPREFIX]) {
        outputPrefix = outfilePrefix; 
        if ( debug_level >= 4 ) {
                TRACE(TRACE_4, "outputPrefix:\t" + outputPrefix + "\n\n");
        }
    }
    secondsAfter = time(NULL);
    cerr << endl << difftime(secondsAfter, secondsBefor) << " s consumed for reading services" << endl << endl;


    // The second step is to build the rewriter part of the adapter.
    // First we open and parse the given rules file.
    // This gives us all rules in a list of adapter rules (totalRules) and
    // furthermore some information for optimizations, such as the rules per
    // channel (consumeRulesPerChannel).
    // Then we add pattern for each interface place of each read oWFN and
    // expand the rewriter with the help of the rules.
    TRACE(TRACE_1, "=================================================================\n");
    TRACE(TRACE_1, "building rewriter\n");
    TRACE(TRACE_1, "=================================================================\n");
    secondsBefor = time(NULL);

    // initialize globals for the rule file parser
    hiddenRules.clear();
    observableRules.clear();
    controllableRules.clear();
    totalRules.clear();
    ruleTypePerChannel.clear();
    consumeRulesPerChannel.clear();
    //produceRulesPerChannel.clear();

    // read the rule file
    adapt_rules_yyin = fopen(adapterRulesFile.c_str(), "r");
    if (!adapt_rules_yyin) {
        cerr << "cannot open rules file '" << adapterRulesFile << "' for reading'\n" << endl;
        exit(EC_NO_RULES_FILE);
    }
    adapt_rules_yyparse();
    //cerr << "DEBUG hiddenRules.size " << hiddenRules.size() << endl;
    //cerr << "DEBUG observableRules.size " << observableRules.size() << endl;
    //cerr << "DEBUG controllableRules.size " << controllableRules.size() << endl;
    //cerr << "DEBUG totalRules.size " << totalRules.size() << endl;
    //cerr << "DEBUG ruleTypePerChannel.size " << ruleTypePerChannel.size() << endl;
    //cerr << "DEBUG consumeRulesPerChannel.size " << consumeRulesPerChannel.size() << endl;
    //cerr << "DEBUG produceRulesPerChannel.size " << produceRulesPerChannel.size() << endl << endl;

    // iterate over all stored oWFNs
    prefix = 0;
    for (list<PetriNet*>::const_iterator service = serviceList.begin(); service != serviceList.end(); service++) {
        
        // expand the rewriter with patterns for the interface of the read net
        expandRewriterWithInterface(adapterRewriter, *service);

        // debug information
        //if ( debug_level >= 4 ) {
        //    outputPNasOWFN(adapterRewriter, intToString(prefix) + '_' + "rewriter");
        //    outputPNasPNG( adapterRewriter, intToString(prefix) + '_' + "rewriter");
        //}

        prefix++;
    }
	
    // expand the rewriter and use optimizations
    if (options[O_ADAPTER_FILE]) {
        expandRewriterWithHiddenRules( adapterRewriter );
        expandRewriterWithObservableRules( adapterRewriter );
        expandRewriterWithControllableRules( adapterRewriter );
        expandRewriterWithTotalRules( adapterRewriter, 0 );
    }
    secondsAfter = time(NULL);
    cerr << endl << difftime(secondsAfter, secondsBefor) << " s consumed for building rewriter" << endl << endl;

    // debug information
    //if ( debug_level >= 4 ) {
    //    outputPNasOWFN(adapterRewriter, intToString(prefix) + '_' + "rewriter");
    //    outputPNasPNG( adapterRewriter, intToString(prefix) + '_' + "rewriter");
    //}

    // reduce complexity with pnapi reduction methods
    adapterRewriter->reduce(5);
    
    // debug information
    //if ( debug_level >= 4 ) {
    //    outputPNasOWFN(adapterRewriter, intToString(prefix) + intToString(prefix) + '_' + "rewriter");
    //    outputPNasPNG( adapterRewriter, intToString(prefix) + intToString(prefix) + '_' + "rewriter");
    //}
    
    // reduce complexity with adapter rewriter reduction methods
    // will be implemented in new PNApi version
    //reduceRewriter( adapterRewriter, 6 );
    
    // debug information
    //if ( debug_level >= 4 ) {
    //    outputPNasOWFN(adapterRewriter, intToString(prefix) + intToString(prefix) + intToString(prefix) + '_' + "rewriter");
    //    outputPNasPNG( adapterRewriter, intToString(prefix) + intToString(prefix) + intToString(prefix) + '_' + "rewriter");
    //}
    
    // We have read all given services and constructed the rewriter part of the
    // adapter, so now it is time to build the adapter controller. This ist done
    // by existing technology: the partner synthesis implemented in fiona.
    // The adapter controller is the partner for the composition of the adapter
    // rewriter and all given services.
    TRACE(TRACE_1, "=================================================================\n");
    TRACE(TRACE_1, "computing controller\n");
    TRACE(TRACE_1, "=================================================================\n");
    secondsBefor = time(NULL);
	
    // iterate over all stored oWFNs
    PetriNet* adapterComposed = new PetriNet(*adapterRewriter);
    prefix = 0;
    for (list<PetriNet*>::const_iterator service = serviceList.begin(); service != serviceList.end(); service++) {

        // compose current net with former composed nets
    	adapterComposed->compose(**service);

        // debug information
        //if ( debug_level >= 4 ) {
        //    outputPNasOWFN(adapterComposed, intToString(prefix) + '_' + "composed");
        //    outputPNasPNG( adapterComposed, intToString(prefix) + '_' + "composed");
        //}

        prefix++;
    }

    // reduce complexity of composition and reenumerate nodes
    adapterComposed->reduce(5);
    adapterComposed->reenumerate();

    // output composition
    outputPNasOWFN(adapterComposed, outputPrefix + "composed");
    //if ( debug_level >= 4 ) {
    //    outputPNasPNG(adapterComposed, outputPrefix + "composed");
    //}

    // compute partner (adapter controller) with fiona
	string systemcall = (parameters[P_SMALLADAPTER]) ?
		"fiona -t smallpartner -r -m" + toString(messages_manual) + " " + outputPrefix + "composed.owfn" :
		"fiona -t mostpermissivepartner -m" + toString(messages_manual) + " " + outputPrefix + "composed.owfn";
    TRACE(TRACE_1, "Invoking fiona with the following options:\n");
    TRACE(TRACE_1, systemcall + "\n\n");
    system(systemcall.c_str());


    delete adapterComposed;
    secondsAfter = time(NULL);
    cerr << endl << difftime(secondsAfter, secondsBefor) << " s consumed for computing controller" << endl << endl;


    // Fiona computed the adapter controller, so all we have to do is to read
    // the result and compose it with our constructed rewriter. The computed
    // partner is identifiable by the suffix "-partner" it gets from Fiona.
    // Furthermore we output the complete adapter together with some
    // information about it.
    TRACE(TRACE_1, "=================================================================\n");
    TRACE(TRACE_1, "building complete adapter\n");
    TRACE(TRACE_1, "=================================================================\n");
    secondsBefor = time(NULL);

    string controlName = "";
    PetriNet* adapterControl = readService(outputPrefix + "composed-partner.owfn", controlName);

    // rename and compose controller and rewriter
    // necessary because compose-method expects different prefixes for interface
    adapterRewriter->addPrefix("rewriter_");
    adapterControl->addPrefix("controller_");
    adapterRewriter->compose(*adapterControl);

    // reduce complexity of composition and reenumerate nodes
    adapterRewriter->reduce(5);
    adapterRewriter->reenumerate();
    
#ifndef NDEBUG    
    // output a composition of all given services and the complete computed
    // adapter which can be used for testing the adapter synthesis algorithm.
    // TODO: this is a HACK for testing purposes
    if ( debug_level >= TRACE_3 ) {

        PetriNet* adapterTest = new PetriNet( *adapterRewriter );
        for (list<PetriNet*>::const_iterator service = serviceList.begin(); service != serviceList.end(); service++) {

            adapterTest->compose(**service);
        }
        outputPNasOWFN(adapterTest, outputPrefix + "adaptertest");
        //outputPNasPNG( adapterTest, outputPrefix + "adaptertest");

        delete adapterTest;
    }
#endif    

    // output final adapter
    // TODO BUG: remove prefixes from interface places
    // like adapterRewriter->addPrefix("", true); but using methods from new PNApi
    outputPNasOWFN(adapterRewriter, outputPrefix + "adapter");
    outputPNasPNG( adapterRewriter, outputPrefix + "adapter");
    cerr << adapterRewriter->information() << endl;

    secondsAfter = time(NULL);
    cerr << endl << difftime(secondsAfter, secondsBefor) << " s consumed for building complete adapter" << endl << endl;

    // garbage collection
    for (list<PetriNet*>::const_iterator service = serviceList.begin(); service != serviceList.end(); service++) {
        delete *service;
    }
    delete adapterControl;
    delete adapterRewriter;
    serviceList.clear();

    TRACE(TRACE_5, "Adapter::generate() : end\n");
}
