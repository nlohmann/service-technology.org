#include <iostream>
#include <fstream>
#include <map>
#include <set>
#include <string>

//#include "owfn.h"
#include "CommunicationGraph.h"
#include "pnapi/pnapi.h"

using std::map;
using std::set;
using std::string;
using std::cerr;
using std::endl;
using std::ofstream;
using namespace PNapi;

// globals used in STG parser
set<string> transitions;
set<string> places;
set<string> initialMarked;
map<string, set<string> > arcs;


PetriNet STG2oWFN_init() {
    extern int stg_yyparse();
    extern FILE *stg_yyin;

    // call STG parser
    stg_yyparse();
    fclose(stg_yyin);

    // create a Petri Net object
    PetriNet STGPN = PetriNet();

    // create places
    for (set<string>::iterator p = places.begin(); p != places.end(); p++)
        STGPN.newPlace(*p);

    //Place *finalPlace = STGPN.newPlace("final");
    //finalPlace->isFinal = true;

    // initially mark places
    for (set<string>::iterator p = initialMarked.begin();
         p != initialMarked.end(); p++) {
        STGPN.findPlace(*p)->mark();
    }

    // create transitions
    for (set<string>::iterator t = transitions.begin(); t != transitions.end(); t++) {
        string transitionName = *t;
        string placeName = transitionName.substr(0, transitionName.find_first_of("/"));

        if (transitionName.substr(0, transitionName.find_first_of(".")) == "in") {
            Place *inPlace = STGPN.findPlace(placeName);
            if (inPlace == NULL)
                inPlace = STGPN.newPlace(placeName, IN);

            transitionName = "t" + transitionName;

            Transition *inTransition = STGPN.findTransition(transitionName);
            if (inTransition == NULL)
                inTransition = STGPN.newTransition(transitionName);

            STGPN.newArc(inPlace, inTransition);
        } else if (transitionName.substr(0, transitionName.find_first_of("."))
                == "out") {
            Place *outPlace = STGPN.findPlace(placeName);
            if (outPlace == NULL)
                outPlace = STGPN.newPlace(placeName, OUT);

            transitionName = "t" + transitionName;

            Transition *outTransition = STGPN.findTransition(transitionName);
            if (outTransition == NULL)
                outTransition = STGPN.newTransition(transitionName);

            STGPN.newArc(outTransition, outPlace);
        }
        /*        else
         if (transitionName == "finalize")
         {
         Transition *finalTransition = STGPN.newTransition("t" + transitionName);
         STGPN.newArc(finalTransition, finalPlace);
         }
         */
        // arcs t->p
        for (set<string>::iterator p = arcs[*t].begin(); p != arcs[*t].end(); p++) {
            STGPN.newArc(STGPN.findTransition(transitionName), STGPN.findPlace(*p));
        }
    }

    // arcs: p->t
    for (set<string>::iterator p = places.begin(); p != places.end(); p++)
        for (set<string>::iterator t = arcs[*p].begin(); t != arcs[*p].end(); t++)
            STGPN.newArc(STGPN.findPlace(*p), STGPN.findTransition("t" + *t));

    return STGPN;
}


void STG2oWFN_main() {

    string netfile = PN->filename;

    PetriNet STGPN = STG2oWFN_init();
    std::cerr << STGPN.information() << endl;

    string filename = netfile.substr(0, string(netfile).length()-5) + "-partner.owfn";
    cerr << "writing partner oWFN to file `"<< filename << "'"<< endl;
    ofstream *file = new ofstream(filename.c_str(), ofstream::out | ofstream::trunc | ofstream::binary);

    STGPN.set_format(FORMAT_OWFN);
    (*file) << STGPN;

    file->close();

    // also generate the png file
    filename = netfile.substr(0, string(netfile).length()-5) + "-partner.dot";
    file = new ofstream(filename.c_str(), ofstream::out | ofstream::trunc | ofstream::binary);

    STGPN.set_format(FORMAT_DOT);
    (*file) << STGPN;

    file->close();

    // Make a systemcall to dot in order to create the png
    string systemcall = "dot -q -Tpng -o\"" + netfile.substr(0, string(netfile).length()-5) + "-partner.png\" "+ filename;
    system(systemcall.c_str());
}
