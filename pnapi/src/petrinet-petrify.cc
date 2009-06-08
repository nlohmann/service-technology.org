
/******************************************************************************
 * Headers
 *****************************************************************************/

#include <cassert>
#include <cstdlib>
#include <iostream>
#include <set>
#include <sstream>
#include <string>
#include <vector>

#include "automaton.h"
#include "petrinet.h"
#include "petrinet-petrify.h"
#include "config.h"         // needed to use petrify
#include "formula.h"
#include "util.h"

using std::cerr;
using std::endl;
using std::istringstream;
using std::pair;
using std::set;
using std::string;
using std::vector;


using namespace pnapi;


/*!
 * The constructor transforming an automaton to a Petri net.
 */
PetriNet::PetriNet(const Automaton &sa) :
  observer_(*this)
{
  std::vector<std::string> edgeLabels;
  std::string fname = sa.printToSTG(edgeLabels);
  std::set<std::string> input = sa.input();
  std::set<std::string> output = sa.output();

  createFromSTG(edgeLabels, fname, input, output);
}


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
void PetriNet::createFromSTG(vector<string> &edgeLabels,
                             const string &fileName,
                             std::set<std::string> &inputPlacenames,
                             std::set<std::string> &outputPlacenames)
{
  // preparing system call of petrify
  string pnFileName = fileName + ".pn"; // add .pn to the output file
  string systemcall = string(CONFIG_PETRIFY) + " " + fileName +
                      " -dead -ip -nolog -o " + pnFileName;

  // calling petrify if possible
  if (string(CONFIG_PETRIFY) != "not found")
  {
    int result = system(systemcall.c_str());
    assert(result == 0);
  }

  // parse generated file
  pnapi_petrify_yyin = fopen(pnFileName.c_str(), "r");
  if (pnapi_petrify_yyin == NULL)
    return; // TODO: error!!!
  pnapi_petrify_yyparse();
  fclose(pnapi_petrify_yyin);


  /* ---------------------------
   * BEGIN of STG2oWFN_init part
   * ---------------------------
   */

  for (set<string>::iterator p = pnapi_petrify_places.begin(); p != pnapi_petrify_places.end(); p++)
  {
    createPlace(*p);
  }

  // initially mark places
  for (set<string>::iterator p = pnapi_petrify_initialMarked.begin(); p != pnapi_petrify_initialMarked.end(); p++)
  {
    findPlace(*p)->mark(1);
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

        string placeName = transitionName;
        if ( inputPlacenames.count(transitionName) > 0 )
        {
          Place *inPlace = findPlace(placeName);
          if (inPlace == NULL) inPlace = &createPlace(placeName, Node::INPUT);
        }
        else if ( outputPlacenames.count(transitionName) > 0 )
        {
          Place *outPlace = findPlace(placeName);
          if (outPlace == NULL) outPlace = &createPlace(placeName, Node::OUTPUT);
        }
        else
        {
          //cerr << "possible error in createFromSTG: found transition without ! or ? as first symbol" << endl;
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
      Transition * transition = findTransition("t" + remapped);
      if (transition == NULL) transition = &createTransition("t" + remapped);

      // create arcs t->p
      for (set<string>::iterator p = pnapi_petrify_arcs[*t].begin(); p != pnapi_petrify_arcs[*t].end(); p++)
      {
        createArc( *transition, *findPlace(*p) );
      }

      // create arcs t->interface and interface->t
      do
      {
        // PRECONDITION: transitions are separated by ", "
        string placeName = remapped.substr( 0, remapped.find(",") ); // take next placename
        placeName = placeName.substr( 0, placeName.find("/") );      // remove possible /

        Place * place = findPlace(placeName);
//        assert(place != NULL);

        if ( inputPlacenames.count(placeName) > 0 )
        {
          if (place == NULL) place = &createPlace(placeName, Node::INPUT);
          createArc(*place, *transition);
        }
        else if ( outputPlacenames.count(placeName) > 0 )
        {
          if (place == NULL) place = &createPlace(placeName, Node::OUTPUT);
          createArc(*transition, *place);
        }
        else
        {
          //cerr << "possible error in stg2owfn_init: found label without ! or ? as first symbol" << endl;
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
        createArc(*findPlace(*p), *findTransition("t" + transitionName));
      }
      else
      {
        // This place is the result of a final node
        finalCondMap[transitionName].insert(*p);
      }
    }
  }


  // For each transition found to be a final transition...
  for (map<string, set<string> >::iterator transIt = finalCondMap.begin(); transIt != finalCondMap.end(); ++transIt)
  {

    // Create a set for the places having this transition in their post set.
    set<Place *> nextTrans;

    // For each place in the post set...
    for (set<string>::iterator placesIt = (*transIt).second.begin(); placesIt != (*transIt).second.end(); ++placesIt)
    {
      // Insert this place in the post set.
      nextTrans.insert(findPlace(*placesIt));
    }

    finalCondition() = false;
    pnapi::formula::Conjunction *fd = NULL;
    pnapi::formula::FormulaEqual *store = NULL;
    // create clause for the final condition
    for (set<Place *>::iterator p = nextTrans.begin(); p != nextTrans.end(); p++)
    {
      if (fd == NULL)
      {
        if (store == NULL)
        {
          store = new pnapi::formula::FormulaEqual(**p, 1);
        }
        else
        {
          fd = new pnapi::formula::Conjunction(*store, *new pnapi::formula::FormulaEqual(**p, 1));
        }
      }
      else
      {
        fd = new pnapi::formula::Conjunction(*fd, *new pnapi::formula::FormulaEqual(**p, 1));
      }
    }
    if (fd != NULL)
      finalCondition() = finalCondition().formula() || *fd;
    else
      if (store != NULL)
        finalCondition() = finalCondition().formula() || *store;

  }

  // All other places empty
  set<const Place *> concerning = finalCondition().concerningPlaces();
  set<const Place *> places;
  for (set<Place *>::iterator p = getPlaces().begin();
      p != getPlaces().end(); p++)
    places.insert(const_cast<Place *>(*p));
  set<const Place *> empty = util::setDifference(places, concerning);
  for (set<const Place *>::iterator p = empty.begin(); p != empty.end(); p++)
    finalCondition() = finalCondition().formula() && **p == 0;


  /* ---------------------------
   * END of STG2oWFN_init part
   * ---------------------------
   */

  // cleaning up generated file
  remove(pnFileName.c_str());
}
