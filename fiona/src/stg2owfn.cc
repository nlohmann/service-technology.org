#include <iostream>
#include <fstream>
#include <map>
#include <set>
#include <string>
#include <sstream>

//#include "owfn.h"
#include "CommunicationGraph.h"
#include "pnapi/pnapi.h"

// #defines YY_FLEX_HAS_YYLEX_DESTROY if we can call yylex_destroy()
#include "lexer_owfn_wrap.h"

#ifdef YY_FLEX_HAS_YYLEX_DESTROY
extern int stg_yylex_destroy();
#endif

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
set<string> interface;
map<string, set<string> > arcs;


//! \brief helper function for STG2oWFN
//! \param edge mapped label, e.g. t1 (created by printGraphToSTG) or t1/1 (created by petrify)
//! \param edgeLabels a vector of strings containing the former labels (created by printGraphToSTG)
//! \return a string containing the remapped label, e.g. if !Euro was mapped to t0, then t0 is remapped to !Euro
//!         labels created by petrify need special handling, e.g. t0/1 is remapped to !Euro/1
string remap(string edge, vector<string> & edgeLabels) {
    string affix = (edge.find("/") != string::npos) ? edge.substr( edge.find("/") ) : "";    // read affix
    string indexString = edge.substr( 1, edge.size() - affix.size() - 1 );                    // read index

    istringstream indexStream; // read index as integer
    int index;
    indexStream.str( indexString );
    indexStream >> index;

    //cout << "transitionName: " << edge << " remapped: " << edgeLabels.at(index) << affix << endl;
    //cout << "affix: " << affix << ", indexString: " << indexString << ", read index: " << index << endl;

    return edgeLabels.at(index) + affix;
}


//! \brief helper function for STG2oWFN
PetriNet STG2oWFN_init(vector<string> & edgeLabels, string PNFileName) {

    extern int stg_yyparse();
    extern FILE *stg_yyin;

    // call STG parser
    trace(TRACE_1, "========== starting STG parser\n");
    stg_yyin = fopen(PNFileName.c_str(), "r");
    stg_yyparse();
    fclose(stg_yyin);
#ifdef YY_FLEX_HAS_YYLEX_DESTROY
    // Destroy buffer of OG parser.
    // Must NOT be called before fclose(og_yyin);
    stg_yylex_destroy();
#endif


    // create a petrinet object and create places 
    trace(TRACE_1, "========== create places\n");
    PetriNet STGPN = PetriNet();
    for (set<string>::iterator p = places.begin(); p != places.end(); p++) {
        STGPN.newPlace(*p);
    }
    

    // initially mark places
    trace(TRACE_1, "========== initially mark places\n");
    
    for (set<string>::iterator p = initialMarked.begin(); p != initialMarked.end(); p++) {
        STGPN.findPlace(*p)->mark();
    }

    
    // create interface places out of dummy transitions
    trace(TRACE_1, "========== create interface places\n");

    for (set<string>::iterator t = interface.begin(); t != interface.end(); t++) {
        string remapped = remap(*t, edgeLabels);
    
    if (remapped.substr(0,5) != "FINAL") {
    
        //cout << "original remapped: " << remapped << endl;
        assert( remapped.find("/") == remapped.npos ); // petrify should not rename/create dummy transitions

            do {
                // PRECONDITION: transitions are separated by ", "
        string transitionName = remapped.substr( 0, remapped.find(",") );
        //cout << "transitionName: \"" << transitionName << "\" remapped: \"" << remapped << "\"" << endl;
        remapped = (remapped.find(",") != remapped.npos) ? remapped.substr( transitionName.size() + 2 ) : "";
        //cout << "transitionName: \"" << transitionName << "\" remapped: \"" << remapped << "\"" << endl;

        string placeName = transitionName.substr( 1 );
        if ( transitionName[0] == '?' ) {
            Place *inPlace = STGPN.findPlace(placeName);
            if (inPlace == NULL) 
                                inPlace = STGPN.newPlace(placeName, IN);
        } else if ( transitionName[0] == '!' ) {
            Place *outPlace = STGPN.findPlace(placeName);
            if (outPlace == NULL) 
                                outPlace = STGPN.newPlace(placeName, OUT);
        } else {
                    cerr << "possible error in stg2owfn_init: found transition without ! or ? as first symbol" << endl;
        }
            } while ( remapped != "" );
    }
    }


    // create transitions and arcs from/to interface places
    trace(TRACE_1, "========== create transitions\n");
    
    for (set<string>::iterator t = transitions.begin(); t != transitions.end(); t++) {

    // create transition if necessary
    string remapped = remap(*t, edgeLabels);
    Transition * transition = STGPN.findTransition("t" + remapped);
    //cout << "\t" << remapped << endl;    
    if (transition == NULL && remapped.substr(0,5) != "FINAL") transition = STGPN.newTransition("t" + remapped);

    // create arcs t->p
    if (transition != NULL) {
        for (set<string>::iterator p = arcs[*t].begin(); p != arcs[*t].end(); p++) {
            STGPN.newArc( transition, STGPN.findPlace(*p) );
        }
        
                // create arcs t->interface and interface->t
        //cout << "original remapped: \"" << remapped << "\"" << endl;
        do {
                    // PRECONDITION: transitions are separated by ", "
            string placeName = remapped.substr( 0, remapped.find(",") );    // take next placename
            placeName = placeName.substr( 1 );                                                        // remove first ! or ?
            placeName = placeName.substr( 0, placeName.find("/") );                // remove possible /

            //cout << "placeName: \"" << placeName << "\" remapped: \"" << remapped << "\"" << endl;
            Place * place = STGPN.findPlace(placeName);

                    if ( remapped[0] == '?' ) {
                if (place == NULL) place = STGPN.newPlace(placeName, IN);
                STGPN.newArc(place, transition); 
                    } else if ( remapped[0] == '!' ) {
            if (place == NULL) place = STGPN.newPlace(placeName, OUT);
                STGPN.newArc(transition, place);
            } else {
                cerr << "possible error in stg2owfn_init: found label without ! or ? as first symbol" << endl;
            }

            // remove first symbol (! or ?), placename (read above) and separators (", ") from remapped
            remapped = (remapped.find(",") != remapped.npos) ? remapped.substr( remapped.find(",") + 2 ) : "";
            //cout << "placeName: \"" << placeName << "\" remapped: \"" << remapped << "\"" << endl;
        } while ( remapped != "" );
        }
    }


    // create arcs p->t
    trace(TRACE_1, "========== create arcs\n");
    
    // Create a map of string sets for final condition creation. 
    map<string, set<string> > finalCondMap; 
        
    for (set<string>::iterator p = places.begin(); p != places.end(); p++) {
        for (set<string>::iterator t = arcs[*p].begin(); t != arcs[*p].end(); t++) {
                string transitionName = remap(*t, edgeLabels);
                if (transitionName.substr(0,5) != "FINAL") {
                STGPN.newArc(STGPN.findPlace(*p), STGPN.findTransition("t" + transitionName));
                } else {
                    // This place is the result of a final node
                    // cerr << "Found a final marking place " << *p << endl;
                    finalCondMap[transitionName].insert(*p);                                                                    
                }
        }
    }

        
    // Get a reference to the final condition of the petri net.     
    list< set< pair<Place *, unsigned int > > >& finalCondSet = STGPN.final_set_list;
     

    // For each transition found to be a final transition...    
    for (map<string, set<string> >::iterator transIt = finalCondMap.begin(); transIt != finalCondMap.end(); ++transIt) {
            
        // Create a set for the places having this transition in their post set.
        set< pair<Place *, unsigned int> > nextTrans;
            
        // For each place in the post set...
        for (set<string>::iterator placesIt = (*transIt).second.begin(); placesIt != (*transIt).second.end(); ++placesIt) {
                
            // Insert this place in the post set.
            nextTrans.insert(pair<Place *, unsigned int>(STGPN.findPlace(*placesIt), 1));
                
        }      

        // Add this clause to the final condition
        finalCondSet.push_back(nextTrans);
            
    }

    return STGPN;
}


void STG2oWFN_main(vector<string> & edgeLabels, string PNFileName) {

    cout << "creating the oWFN file out of " << PNFileName << endl;

    PetriNet STGPN = STG2oWFN_init( edgeLabels, PNFileName );
    std::cerr << STGPN.information() << endl;

    string netfile = PNFileName.substr(0, PNFileName.find(".owfn") );
    string filename = netfile + "-partner.owfn";
    cerr << "writing partner oWFN to file `"<< filename << "'"<< endl;
    ofstream *file = new ofstream(filename.c_str(), ofstream::out | ofstream::trunc | ofstream::binary);

    STGPN.set_format(FORMAT_OWFN);
    (*file) << STGPN;

    file->close();
    delete file;

    // generate the png file
    //cerr << "=== generate png file" << endl;
    filename = netfile + "-partner.dot";
    file = new ofstream(filename.c_str(), ofstream::out | ofstream::trunc | ofstream::binary);

    STGPN.set_format(FORMAT_DOT);
    (*file) << STGPN;

    file->close();
    delete file;

    // Make a systemcall to dot in order to create the png
    string systemcall = "dot -q -Tpng -o\"" + netfile + "-partner.png\" "+ filename;
    system(systemcall.c_str());
}
