
/******************************************************************************
 * Headers
 *****************************************************************************/

#include <cassert>
#include <iostream>
#include <fstream>
#include <map>
#include <set>
#include <sstream>
#include <string>

#include "petrinet.h"
#include "parser.h"
#include "config.h"         // needed to use petrify
 
using std::map;
using std::set;
using std::string;
using std::cerr;
using std::endl;
using std::ofstream;
using std::ifstream;
using std::istringstream;
using namespace PNapi;


/*!
 * \brief helper function for STG2oWFN
 * \param edge mapped label, e.g. t1 (created by printGraphToSTG) or t1/1 (created by petrify)
 * \param edgeLabels a vector of strings containing the former labels (created by printGraphToSTG)
 * \return a string containing the remapped label, e.g. if !Euro was mapped to t0, then t0 is remapped to !Euro
 *         labels created by petrify need special handling, e.g. t0/1 is remapped to !Euro/1  
 */
string PetriNet::remap(string edge, vector<string> & edgeLabels) 
{
    string affix = (edge.find("/") != string::npos) ? edge.substr( edge.find("/") ) : ""; // read affix
    string indexString = edge.substr( 1, edge.size() - affix.size() - 1 );                // read index

    istringstream indexStream; // read index as integer
    int index;
    indexStream.str( indexString );
    indexStream >> index;


    return edgeLabels.at(index) + affix;
}



/*!
 * \brief   creates a petri net from an STG file by overwriting the existing one
 * 
 * \param   fileName location of the STG file
 * 
 * \note    - this function has been outsourced from fiona and recently only needed by fiona
 *          - petrify is needed to call this function
 * 
 */
void PetriNet::createFromSTG(vector<string> & edgeLabels, 
                             const string & fileName, 
                             set<string>& inputPlacenames, 
                             set<string>& outputPlacenames )
{
  // preparing systemcall of petrify
  string pnFileName = fileName + ".pn"; // add .pn to the output file
  string systemcall = string(CONFIG_PETRIFY) + " " + fileName + " -dead -ip -nolog -o " + pnFileName;
  
  // calling petrify if possible
  if (CONFIG_PETRIFY != "not found") 
  {
    system(systemcall.c_str());
  }
  
  
  // parse generated file
  
  parser::petrify::Parser parser;
  parser::petrify::Visitor visitor;
  ifstream is;
  is.open(pnFileName.c_str(), ifstream::in);
  parser.parse(is).visit(visitor);
  is.close();
  PetrifyResult result = visitor.getPetrifyResult();
  
  set<string> pnapi_petrify_transitions = result.transitions;
  set<string> pnapi_petrify_places = result.places;
  set<string> pnapi_petrify_initialMarked = result.initialMarked;
  set<string> pnapi_petrify_interface = result.interface;
  map<string, set<string> > pnapi_petrify_arcs = result.arcs;
  
  
  /* ---------------------------
   * BEGIN of STG2oWFN_init part
   * ---------------------------
   */  
  
  
  // create a petrinet object and create places 
  PetriNet STGPN = PetriNet();
  for (set<string>::iterator p = pnapi_petrify_places.begin(); p != pnapi_petrify_places.end(); p++) 
  {
    STGPN.newPlace(*p);
  }

  // initially mark places
  for (set<string>::iterator p = pnapi_petrify_initialMarked.begin(); p != pnapi_petrify_initialMarked.end(); p++) 
  {
    STGPN.findPlace(*p)->mark();
  }

  // create interface places out of dummy transitions
  for (set<string>::iterator t = pnapi_petrify_interface.begin(); t != pnapi_petrify_interface.end(); t++) 
  {
    string remapped = remap(*t, edgeLabels);
    
    if (remapped.substr(0,5) != "FINAL") 
    {

      assert( remapped.find("/") == remapped.npos ); // petrify should not rename/create dummy transitions
  
      do 
      {
        // PRECONDITION: transitions are separated by ", "
        string transitionName = remapped.substr( 0, remapped.find(",") );
        remapped = (remapped.find(",") != remapped.npos) ? remapped.substr( transitionName.size() + 2 ) : "";
      
        string placeName = transitionName.substr( 1 );
        if ( transitionName[0] == '?' ) 
        {
          Place *inPlace = STGPN.findPlace(placeName);
          if (inPlace == NULL) inPlace = STGPN.newPlace(placeName, IN);
        } 
        else if ( transitionName[0] == '!' ) 
        {
          Place *outPlace = STGPN.findPlace(placeName);
          if (outPlace == NULL) outPlace = STGPN.newPlace(placeName, OUT);
        } 
        else 
        {
          cerr << "possible error in createFromSTG: found transition without ! or ? as first symbol" << endl;
        }
      } 
      while ( remapped != "" );
    }
  }
   
  for (set<string>::iterator t = pnapi_petrify_transitions.begin(); t != pnapi_petrify_transitions.end(); t++) 
  {
    
    string remapped = remap(*t, edgeLabels);
   
    if (remapped.substr(0, 5) != "FINAL") 
    {
      // create transition if necessary
      Transition * transition = STGPN.findTransition("t" + remapped);
      if (transition == NULL) transition = STGPN.newTransition("t" + remapped);
  
      // create arcs t->p
      for (set<string>::iterator p = pnapi_petrify_arcs[*t].begin(); p != pnapi_petrify_arcs[*t].end(); p++) 
      {
        STGPN.newArc( transition, STGPN.findPlace(*p) );
      }
           
      // create arcs t->interface and interface->t
      do 
      {
        // PRECONDITION: transitions are separated by ", "
        string placeName = remapped.substr( 0, remapped.find(",") ); // take next placename
        placeName = placeName.substr( 1 );                           // remove first ! or ?
        placeName = placeName.substr( 0, placeName.find("/") );      // remove possible /
        
        Place * place = STGPN.findPlace(placeName);
        
        if ( remapped[0] == '?' ) 
        {
          if (place == NULL) place = STGPN.newPlace(placeName, IN);
          STGPN.newArc(place, transition); 
        } 
        else if ( remapped[0] == '!' ) 
        {
          if (place == NULL) place = STGPN.newPlace(placeName, OUT);
          STGPN.newArc(transition, place);
        } 
        else 
        {
          cerr << "possible error in stg2owfn_init: found label without ! or ? as first symbol" << endl;
        }
        
        // remove first symbol (! or ?), placename (read above) and separators (", ") from remapped
        remapped = (remapped.find(",") != remapped.npos) ? remapped.substr( remapped.find(",") + 2 ) : "";
      } 
      while ( remapped != "" );
    }
  }
  
  
   
  // create arcs p->t
       
  // Create a map of string sets for final condition creation. 
  map<string, set<string> > finalCondMap; 
           
  for (set<string>::iterator p = pnapi_petrify_places.begin(); p != pnapi_petrify_places.end(); p++) 
  {
    for (set<string>::iterator t = pnapi_petrify_arcs[*p].begin(); t != pnapi_petrify_arcs[*p].end(); t++) 
    {
      string transitionName = remap(*t, edgeLabels);
       
      if (transitionName.substr(0,5) != "FINAL") 
      {
        STGPN.newArc(STGPN.findPlace(*p), STGPN.findTransition("t" + transitionName));
      } 
      else 
      {
        // This place is the result of a final node
        finalCondMap[transitionName].insert(*p);                                                                    
      }
    }
  }
   
          
  // Get a reference to the final condition of the petri net.     
  list< set< pair<Place *, unsigned int > > >& finalCondSet = STGPN.final_set_list;
  
   
  // For each transition found to be a final transition...    
  for (map<string, set<string> >::iterator transIt = finalCondMap.begin(); transIt != finalCondMap.end(); ++transIt) 
  {
  
    // Create a set for the places having this transition in their post set.
    set< pair<Place *, unsigned int> > nextTrans;
                 
    // For each place in the post set...
    for (set<string>::iterator placesIt = (*transIt).second.begin(); placesIt != (*transIt).second.end(); ++placesIt) 
    {
                       
      // Insert this place in the post set.
      nextTrans.insert(pair<Place *, unsigned int>(STGPN.findPlace(*placesIt), 1));
                       
    }      
   
    // Add this clause to the final condition
    finalCondSet.push_back(nextTrans);
                 
  }
  
  
  /* ---------------------------
   * END of STG2oWFN_init part
   * ---------------------------
   */ 
  
  /* ---------------------------
   * BEGIN of STG2oWFN_main part
   * ---------------------------
   */ 
  
  
  // complete interface with interface information from IG/OG
   for (set<string>::const_iterator iter = inputPlacenames.begin(); iter != inputPlacenames.end(); iter++) 
   {
       Place *inPlace = STGPN.findPlace(*iter);
       if (inPlace == NULL) 
       {
           STGPN.newPlace(*iter, IN);
       }
   }
   for (set<string>::const_iterator iter = outputPlacenames.begin(); iter != outputPlacenames.end(); iter++) 
   {
       Place *outPlace = STGPN.findPlace(*iter);
       if (outPlace == NULL) 
       {
           STGPN.newPlace(*iter, OUT);
       }
   }
  
   /* note: STG2oWFN_main would now write the STGPN to a file
    * this can still be done there with the result of this function
    * so it will not be done here.
    */  
  
   /* ---------------------------
    * BEGIN of STG2oWFN_main part
    * ---------------------------
    */ 
   
  
  // cleaning up generated file
  remove(pnFileName.c_str());
  
  // apply result
  (*this) = STGPN;
}
