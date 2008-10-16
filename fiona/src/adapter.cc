/*****************************************************************************
 * Copyright 2008 Christian Gierds                                           *
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
 * \file    adapter.cc
 *
 * \brief   all functionality for adapter generation
 *
 * \author  responsible: Christian Gierds <gierds@informatik.hu-berlin.de>
 *
 * \note    This file is part of the tool Fiona and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universitaet zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
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
#include <list>
#include <string>

using namespace std;


// global variables - used for parsing rules file
std::list< adapterRule > adapter_rules;			// from parser: list of all read rules
std::map< std::string, unsigned int > rulesPerChannel;	// from parser: number of rules for each read channel


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
	if ( !parameters[P_NOPNG] )
	{
	    ostream * output = openOutput(prefix + ".dot");
	    
	    TRACE(TRACE_1, "-> Printing Petri net " + prefix + " as PNG ...\n");
	    petrinet->set_format(PNapi::FORMAT_DOT);
	    (*output) << *petrinet;
	
	    closeOutput(output);
	    
		string systemcall = "dot -q -Tpng -o\"" + prefix + ".png\" \"" + prefix + ".dot\"";
		TRACE(TRACE_1, "Invoking dot with the following options:\n");
		TRACE(TRACE_1, systemcall + "\n\n");
		system(systemcall.c_str());
	}
}






//! \brief read service from file and combine current prefix with given prefixes
//! \param filename a string containing the filename of the service
//! \param netname a reference to current netname
//! \return a pointer to the read petrinet
PNapi::PetriNet * Adapter::readService(std::string filename, std::string & netname)
{
    extern void readnet(const std::string & owfnfile);	// extern function to read oWFN from file
    extern void reportNet();							// function to print report about a net
    extern oWFN * PN;									// pointer to gloabel oWFN PN
    
    
    // read current prefix and combine with former read prefixes
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
    assert(netname != "");
    TRACE(TRACE_1, "-> reading net:\t\t" + filename + "\n");
    TRACE(TRACE_1, "netname:\t\t" + netname + "\n");
    
    
    // get oWFN into variable PN, convert it to petrinet and stores it in serviceList
    PlaceTable = new SymbolTab<PlSymbol>;
    readnet(filename);
    delete PlaceTable;
    assert(PN != NULL);

    PNapi::PetriNet * currentPN = PN->returnPNapiNet();
    assert(currentPN != NULL);
    PN = NULL;
    
    return currentPN;
}


//! \brief read adapter control from fiona created file
//! \param composedNetName a string containing the composed net from adapter rewriter and all services
//! \return a pointer to a petrinet which contains the read adapter control
PNapi::PetriNet * Adapter::readControl(std::string composedNetName)
{
    extern void readnet(const std::string & owfnfile);	// extern function to read oWFN from file
    extern void reportNet();							// function to print report about a net
    extern oWFN * PN;									// pointer to global oWFN PN
    
    
    // compose control filename
    assert(composedNetName != "");
    std::string filename = composedNetName + "-partner.owfn";
    TRACE(TRACE_1, "-> reading net:\t\t" + filename + "\n");
    TRACE(TRACE_1, "composedNetName:\t" + composedNetName + "\n\n");
	
    
    // get control oWFN into variable PN and convert it to petrinet
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
void Adapter::expandRules(PNapi::PetriNet * rewriter,
        PNapi::PetriNet * service, std::string netname) {
    assert(rewriter != NULL);
    assert(service != NULL);

    // expand rewriter by creating input/output handling for each interface place of given service 
    set< PNapi::Place *> interface = service->getInterfacePlaces();
    for (set< PNapi::Place *>::iterator place = interface.begin(); place
            != interface.end(); place++) {
        assert(*place != NULL);

        std::string placeName = (*place)->nodeFullName();
        std::string internalName = placeName.substr(netname.length() + 1); // without oWFN name prefix

        // handle input place
        if ((*place)->type == PNapi::IN && rewriter->findPlace(placeName)
                == NULL) {
            PNapi::Place * p = rewriter->newPlace(placeName, PNapi::OUT);
            PNapi::Place * p_send = rewriter->newPlace("send_" + internalName,
                    PNapi::IN);
            PNapi::Place * p_internal = rewriter->findPlace(internalName
                    + "_internal");

            // conflict of sending transition and internal rule
            rulesPerChannel[internalName]++;

            if (!p_internal) {
                p_internal = rewriter->newPlace(internalName + "_internal",
                        PNapi::INTERNAL, messages_manual);
                internalPlaces.push_back(internalName + "_internal");
            }

            PNapi::Transition * t = rewriter->newTransition("t_send_"
                    + internalName);

            rewriter->newArc(t, p);
            rewriter->newArc(p_internal, t);
            rewriter->newArc(p_send, t);
        }
        // handle output place
        else if ((*place)->type == PNapi::OUT && rewriter->findPlace(placeName)
                == NULL) {
            PNapi::Place * p = rewriter->newPlace(placeName, PNapi::IN);
            PNapi::Place * p_received = rewriter->newPlace("received_"
                    + internalName, PNapi::OUT);
            PNapi::Place * p_internal = rewriter->findPlace(internalName
                    + "_internal");

            if (!p_internal) {
                p_internal = rewriter->newPlace(internalName + "_internal",
                        PNapi::INTERNAL, messages_manual);
                internalPlaces.push_back(internalName + "_internal");
            }

            PNapi::Transition * t = rewriter->newTransition("t_receive_"
                    + internalName);

            rewriter->newArc(p, t);
            rewriter->newArc(t, p_internal);
            rewriter->newArc(t, p_received);
        }
    }
}


//! \brief read given rules from given *.ar file and expand given rewriter
//! \param a petrinet pointer to a net which contains the current adapter rewriter
void Adapter::readRules(PNapi::PetriNet * rewriter)
{
    extern string adapterRulesFile; // name of the rules file
	
    
    // opt. read rules from given rule file
    assert(rewriter != NULL);
    if (options[O_ADAPTER_FILE])
    {
        adapt_rules_yyin = fopen(adapterRulesFile.c_str(), "r");
        if (!adapt_rules_yyin) {
            cerr << "cannot open rules file '" << adapterRulesFile << "' for reading'\n" << endl;
            exit(EC_NO_RULES_FILE);
        }
        adapt_rules_yyparse();
    }
    
    
    // create rule pattern and expand rewriter
    unsigned int ruleNr = 1;
    for(std::list< adapterRule >::iterator rule = adapter_rules.begin(); rule != adapter_rules.end(); rule++)
    {
    	// rules without first channels needs special handling for controllability
        bool conflictfree = (rule->first.size() > 0) ? true : false;
        PNapi::Transition * t = rewriter->newTransition("rule" + toString(ruleNr));
        
        TRACE(TRACE_1, "handling rule:\t");
        
        // handle first channel list of current adapter rule
        for(list< string >::iterator channel = rule->first.begin(); channel != rule->first.end(); channel++)
        {
            TRACE(TRACE_1, *channel + " ");
            if (rulesPerChannel[*channel] > 1)
            {
                conflictfree = false;
            }
            PNapi::Place * p  = rewriter->findPlace(*channel + "_internal");
                    
            if (! p) // place channel_internal wasn't found
            {
                p = rewriter->newPlace(*channel + "_internal", PNapi::INTERNAL, messages_manual);
                internalPlaces.push_back(*channel + "_internal");
            }

            rewriter->newArc(p, t);
        }

        if (!conflictfree)
        {
	    PNapi::Place * p1 = rewriter->newPlace("execute_rule" + toString(ruleNr), PNapi::IN);
	    PNapi::Place * p2 = rewriter->newPlace("executed_rule" + toString(ruleNr), PNapi::OUT);

            rewriter->newArc(p1, t);
            rewriter->newArc(t, p2);
        }

        
        // handle second channel list of current adapter rule
        TRACE(TRACE_1, " -> ");
        for(list< string >::iterator channel = rule->second.begin(); channel != rule->second.end(); channel++)
        {
            TRACE(TRACE_1, *channel + " ");
        	
	    PNapi::Place * p  = rewriter->findPlace(*channel + "_internal");
                    
            if (! p) 
            {
                p = rewriter->newPlace(*channel + "_internal", PNapi::INTERNAL, messages_manual);
                internalPlaces.push_back(*channel + "_internal");
            }

            rewriter->newArc(t, p);
        }
        
        TRACE(TRACE_1, "\n");
        
        ruleNr++;
    }
    TRACE(TRACE_1, "\n");
}


//! \brief main function for generating adapter
void Adapter::generateAdapter()
{
    TRACE(TRACE_5, "generateAdapter() : start\n");
    
    extern list<std::string> netfiles;		// list of all input (owfn) files
    
    PNapi::PetriNet * adapterRewriter = new PNapi::PetriNet(); // rules part of adapter
    PNapi::PetriNet * adapterComposed = NULL;	// composition of adapter rewriter and given services
    PNapi::PetriNet * adapterControl = NULL;	// controll part of adapter
    
    list< PNapi::PetriNet * > serviceList; 	// list of the services
    string combinedPrefixes = ""; 		// combined prefixes of all read oWFNs, prefixes seperated by _
    time_t secondsBefor, secondsAfter;		// measure time
    
    
    // read given oWFNs, rename & store them in serviceList and expand the adapter rewriter
    secondsBefor = time(NULL);
    for(list<std::string>::iterator netiter = netfiles.begin(); netiter != netfiles.end(); netiter++)
    {
    	TRACE(TRACE_1, "=================================================================\n");
    	TRACE(TRACE_1, "reading service\n");
    	TRACE(TRACE_1, "=================================================================\n");
    	
    	std::string currentNetname = "";
        PNapi::PetriNet * currentPN = readService( *netiter, currentNetname );
        assert(currentPN != NULL);
        assert(currentNetname != "");
        
        combinedPrefixes += ( currentNetname + "_" );
        TRACE(TRACE_1, "combinedPrefixes:\t" + combinedPrefixes + "\n\n");
        currentPN->addPrefix(currentNetname + "_", true);
        serviceList.push_back(currentPN);
//    	outputPNasPNG( currentPN, currentNetname );				// as test
        
        expandRules(adapterRewriter, currentPN, currentNetname);
    }
//    outputPNasPNG(adapterRewriter, combinedPrefixes + "rewriter-interface");	// as test
    outputPNasOWFN(adapterRewriter, combinedPrefixes + "rewriter-interface");	// as test
    
    
    // read given rules
    TRACE(TRACE_1, "=================================================================\n");
    TRACE(TRACE_1, "reading rules\n");
    TRACE(TRACE_1, "=================================================================\n");
	
    readRules(adapterRewriter);
//    outputPNasPNG(adapterRewriter, combinedPrefixes + "rewriter-complete");	// as test
    outputPNasOWFN(adapterRewriter, combinedPrefixes + "rewriter-complete");	// as test
    
    secondsAfter = time(NULL);
    cerr << endl << difftime(secondsAfter, secondsBefor) << " s consumed for reading services and building rewriter" << endl << endl;
    
    
    // compose adapter rules and given services for computing adapter control with fiona
    TRACE(TRACE_1, "=================================================================\n");
    TRACE(TRACE_1, "computing adapter\n");
    TRACE(TRACE_1, "=================================================================\n");
	
    adapterComposed = new PNapi::PetriNet(*adapterRewriter);
    adapterComposed->addPrefix("adapter_");
    for ( list< PNapi::PetriNet * >::const_iterator service = serviceList.begin(); service != serviceList.end(); service++)
    {
    	adapterComposed->compose(**service);
        delete *service;
    }
    serviceList.clear();
    adapterComposed->reduce(5); //necessary for reducing complexity
    adapterComposed->reenumerate();
    
//    outputPNasPNG(adapterComposed, combinedPrefixes + "composed");		// as test
    outputPNasOWFN(adapterComposed, combinedPrefixes + "composed");		// as test
	

    // compute partner to the current adapter with fiona
    secondsBefor = time(NULL);
	string systemcall = (parameters[P_SMALLADAPTER]) ?
		"fiona -t smallpartner -r -m" + toString(messages_manual) + " " + combinedPrefixes + "composed.owfn" :
		"fiona -t mostpermissivepartner -m" + toString(messages_manual) + " " + combinedPrefixes + "composed.owfn";
    TRACE(TRACE_1, "Invoking fiona with the following options:\n");
    TRACE(TRACE_1, systemcall + "\n\n");
    system(systemcall.c_str());
    secondsAfter = time(NULL);
    cerr << endl << difftime(secondsAfter, secondsBefor) << " s consumed for building controller" << endl << endl;
    
    
    // read computed controller (fiona output) and compose final adapter
    adapterControl = readControl(combinedPrefixes + "composed");

    adapterRewriter->addPrefix("rewriter_"); // necessary because compose-method expects different prefixes for interface
    adapterControl->addPrefix("controller_");
    
    adapterRewriter->compose(*adapterControl);
    adapterRewriter->reduce(5);
    adapterRewriter->reenumerate();
    
    
    // output final adapter
    outputPNasPNG(adapterRewriter, combinedPrefixes + "result");
    outputPNasOWFN(adapterRewriter, combinedPrefixes + "result");
    std::cerr << adapterRewriter->information() << endl;
    
    
    // garbage collection
    delete adapterRewriter;
    delete adapterComposed;
    delete adapterControl;

    TRACE(TRACE_5, "generateAdapter() : end\n");
}
