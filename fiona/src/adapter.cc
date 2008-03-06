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
#include "pnapi/pnapi.h"
#include "owfn.h"
#include "state.h"

#include <cassert>
#include <fstream>
#include <iostream>
#include <list>
#include <string>

using namespace std;

namespace {
ostream *openOutput(string name);
void closeOutput(ostream *file);

/*!
 * \brief open output file
 *
 * \param name filename of the output file
 * \return output file stream to the output file
 *
 * \post output file with filename name opened and bound to stream file
 */
ostream *openOutput(string name)
{
  ofstream *file = new ofstream(name.c_str(), ofstream::out | ofstream::trunc | ofstream::binary);

  if (!file->is_open())
    trace(TRACE_0, "File \"" + name + "\" could not be opened for writing access!\n");

  return file;
}

/*!
 * \brief close output file
 *
 * \param file file stream of the output file
 *
 * \post out file addressed by output file stream #file closed
 */
void closeOutput(ostream *file)
{
  if ( file != NULL )
  {
    (*file) << flush;
    (static_cast<ofstream*>(file))->close();
    delete(file);
    file = NULL;
  }
}

} /* namespace */

void generateAdapter()
{
    trace(TRACE_5, "generateAdapter() : start\n");

    cerr << "Generating adapter!" << endl;

    using PNapi::Place;
    using PNapi::Transition;
    using PNapi::IN;
    using PNapi::OUT;
    using PNapi::FORMAT_DOT;

    // list of all input (owfn) files
    extern list<std::string> netfiles;
    
    // function to read oWFN from file
    extern void readnet(const std::string& owfnfile);
    // function to print report about a net
    extern void reportNet();

    // pointer to oWFN PN
    extern oWFN * PN;

    // pointer to output stream
    ostream *output = NULL;

    oWFN * serviceA = NULL;
    PNapi::PetriNet * PnA = NULL;

    oWFN * serviceB = NULL;
    PNapi::PetriNet * PnB = NULL;

    PNapi::PetriNet * adapterPattern = new PNapi::PetriNet();

    // for the start, we want to adapt exactly 2 services
    assert(netfiles.size() == 2);

    list<std::string>::iterator netiter = netfiles.begin();

    {
        std::string currentowfnfile = *netiter;
        assert(currentowfnfile != "");
        
        // prepare getting the net
        PlaceTable = new SymbolTab<PlSymbol>;
        
        // get the net into variable PN
        readnet(currentowfnfile);
        trace(TRACE_0, "=================================================================\n");
        trace(TRACE_0, "processing net " + currentowfnfile + "\n");
        reportNet();
        delete PlaceTable;
    }

    serviceA = PN;
    PnA = serviceA->returnPNapiNet();
    PnA->addPrefix(*netiter);

    PN = NULL;

    netiter++;

    {
        std::string currentowfnfile = *netiter;
        assert(currentowfnfile != "");
        
        // prepare getting the net
        PlaceTable = new SymbolTab<PlSymbol>;
        
        // get the net into variable PN
        readnet(currentowfnfile);
        trace(TRACE_0, "=================================================================\n");
        trace(TRACE_0, "processing net " + currentowfnfile + "\n");
        reportNet();
        delete PlaceTable;
    }

    serviceB = PN;
    PnB = serviceB->returnPNapiNet();
    PnB->addPrefix(*netiter);

    PN = NULL;

    // create Interface to services
    set< Place * > ifPlaces = PnA->getInterfacePlaces();

    for (set< Place * >::iterator place = ifPlaces.begin(); place != ifPlaces.end(); place++)
    {
        if ((*place)->type == IN && adapterPattern->findPlace((*place)->nodeFullName()) == NULL)
        {
            Place * p           = adapterPattern->newPlace((*place)->nodeFullName(), OUT);
            Place * p_send      = adapterPattern->newPlace("send_" + (*place)->nodeFullName(), IN);
            Place * p_sent      = adapterPattern->newPlace("sent_" + (*place)->nodeFullName(), OUT);

            Place * p_internal  = adapterPattern->findPlace((*place)->nodeFullName() + "_internal");
                    
            if (! p_internal) 
            {
                p_internal = adapterPattern->newPlace((*place)->nodeFullName() + "_internal");
            }
            
            Transition * t      = adapterPattern->newTransition("send_" + (*place)->nodeFullName());

            adapterPattern->newArc(t, p);
            adapterPattern->newArc(p_internal, t);
            adapterPattern->newArc(t, p_sent);
            adapterPattern->newArc(p_send, t);
        }
        if ((*place)->type == OUT && adapterPattern->findPlace((*place)->nodeFullName()) == NULL)
        {
            Place * p           = adapterPattern->newPlace((*place)->nodeFullName(), IN);
            Place * p_received  = adapterPattern->newPlace("reveived_" + (*place)->nodeFullName(), OUT);
            Place * p_internal  = adapterPattern->findPlace((*place)->nodeFullName() + "_internal");
                    
            if (! p_internal) 
            {
                p_internal = adapterPattern->newPlace((*place)->nodeFullName() + "_internal");
            }
            
            Transition * t      = adapterPattern->newTransition("receive_" + (*place)->nodeFullName());

            adapterPattern->newArc(p, t);
            adapterPattern->newArc(t, p_internal);
            adapterPattern->newArc(t, p_received);
        }
    }

    // create Interface to services
    ifPlaces = PnB->getInterfacePlaces();

    for (set< Place * >::iterator place = ifPlaces.begin(); place != ifPlaces.end(); place++)
    {
        if ((*place)->type == IN && adapterPattern->findPlace((*place)->nodeFullName()) == NULL)
        {
            Place * p           = adapterPattern->newPlace((*place)->nodeFullName(), OUT);
            Place * p_send      = adapterPattern->newPlace("send_" + (*place)->nodeFullName(), IN);
            Place * p_sent      = adapterPattern->newPlace("sent_" + (*place)->nodeFullName(), OUT);

            Place * p_internal  = adapterPattern->findPlace((*place)->nodeFullName() + "_internal");
                    
            if (! p_internal) 
            {
                p_internal = adapterPattern->newPlace((*place)->nodeFullName() + "_internal");
            }
            
            Transition * t      = adapterPattern->newTransition("send_" + (*place)->nodeFullName());

            adapterPattern->newArc(t, p);
            adapterPattern->newArc(p_internal, t);
            adapterPattern->newArc(t, p_sent);
            adapterPattern->newArc(p_send, t);
        }
        if ((*place)->type == OUT && adapterPattern->findPlace((*place)->nodeFullName()) == NULL)
        {
            Place * p           = adapterPattern->newPlace((*place)->nodeFullName(), IN);
            Place * p_received  = adapterPattern->newPlace("reveived_" + (*place)->nodeFullName(), OUT);
            Place * p_internal  = adapterPattern->findPlace((*place)->nodeFullName() + "_internal");
                    
            if (! p_internal) 
            {
                p_internal = adapterPattern->newPlace((*place)->nodeFullName() + "_internal");
            }
            
            Transition * t      = adapterPattern->newTransition("receive_" + (*place)->nodeFullName());

            adapterPattern->newArc(p, t);
            adapterPattern->newArc(t, p_internal);
            adapterPattern->newArc(t, p_received);
        }
    }

//    PnA->addPrefix("A.");
    PnB->addPrefix("B.");
    adapterPattern->addPrefix("adapter.");

    PnA->compose(*adapterPattern);
    PnA->compose(*PnB);

//    PnA->set_format(FORMAT_OWFN);
  
    output = openOutput("adapter.dot");
    trace(TRACE_1, "-> Printing Petri net for dot ...\n");
    PnA->set_format(FORMAT_DOT);
    (*output) << *PnA;

    closeOutput(output);
    output = NULL;
        
    string systemcall = "dot -q -Tpng -o\"adapter.png\" \"adapter.dot\"";
    trace(TRACE_1, "Invoking dot with the following options:\n");
    trace(TRACE_1, systemcall + "\n\n");
    system(systemcall.c_str());
        
    delete serviceA;
    delete serviceB;
    delete PnA;
    delete PnB;
    delete adapterPattern;

    trace(TRACE_5, "generateAdapter() : end\n");

}
