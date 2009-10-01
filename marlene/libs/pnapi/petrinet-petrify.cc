
/******************************************************************************
 * Headers
 *****************************************************************************/

#include "config.h"
#include <cassert>

#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <set>
#include <sstream>
#include <string>
#include <vector>

#include "automaton.h"
#include "petrinet.h"
#include "parser.h"
#include "formula.h"
#include "util.h"
#include "Output.h"

using std::cerr;
using std::endl;
using std::istringstream;
using std::ifstream;
using std::pair;
using std::set;
using std::string;
using std::vector;


using namespace pnapi;


/*!
 * The constructor transforming an automaton to a Petri net.
 */
PetriNet::PetriNet(const Automaton &sa, std::string petrify) :
  observer_(*this), pathToPetrify(petrify)
{
  util::Output out;
  std::vector<std::string> edgeLabels;
  
  sa.printToSTG(edgeLabels, out);
  
  std::set<std::string> input = sa.input();
  std::set<std::string> output = sa.output();
  std::set<std::string> synchronous = sa.getSynchronousLabels();

  createFromSTG(edgeLabels, out.name(), input, output, synchronous);
  setSynchronousLabels(synchronous);
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


    return edgeLabels[index] + affix;
}



/*!
 * \brief   creates a petri net from an STG file
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
                             std::set<std::string> &outputPlacenames,
                             std::set<std::string> &synchronizeLabels)
{
  // preparing system call of petrify
  string pnFileName = fileName + ".pn"; // add .pn to the output file
  string systemcall = string(pathToPetrify) + " " + fileName +
                      " -dead -ip -nolog -o " + pnFileName;

  // calling petrify if possible
  if (pathToPetrify != "not found")
  {
    int result = system(systemcall.c_str());
    assert(result == 0);
  }

  // parse generated file
  ifstream ifs(pnFileName.c_str(), ifstream::in);
  if (!ifs.good())
    return; // TODO: error!!!
  parser::petrify::Parser parser;
  parser.parse(ifs);
  ifs.close();

  using parser::petrify::places_;
  using parser::petrify::transitions_;
  using parser::petrify::arcs_;
  using parser::petrify::interface_;
  using parser::petrify::initialMarked_;

  // create places
  for (set<string>::iterator p = places_.begin(); p != places_.end(); ++p)
  {
    createPlace(*p, Node::INTERNAL, initialMarked_[*p]);
  }

  // create interface places out of dummy transitions
  for (set<string>::iterator t = interface_.begin(); t != interface_.end(); ++t)
  {
    string remapped = remap(*t, edgeLabels);

    if (remapped.substr(0,5) != "FINAL")
    {
      assert( remapped.find("/") == remapped.npos ); // petrify should not rename/create dummy transitions

      if ( inputPlacenames.count(remapped) > 0 )
      {
        Place *inPlace = findPlace(remapped);
        if (inPlace == NULL) 
          createPlace(remapped, Node::INPUT);
      }
      else if ( outputPlacenames.count(remapped) > 0 )
      {
        Place *outPlace = findPlace(remapped);
        if (outPlace == NULL) 
          createPlace(remapped, Node::OUTPUT);
      }
      // else must be TAU or a synchronize label
    }
  }

  // create transitions
  for (set<string>::iterator t = transitions_.begin(); t != transitions_.end(); ++t)
  {
    string remapped = remap(*t, edgeLabels);

    if (remapped.substr(0, 5) != "FINAL")
    {
      // create transition if necessary
      Transition * transition = findTransition("t" + remapped);
      if (transition == NULL) 
        transition = &createTransition("t" + remapped);

      // create arcs t->p
      for (set<string>::iterator p = arcs_[*t].begin(); p != arcs_[*t].end(); ++p)
      {
        createArc(*transition, *findPlace(*p));
      }

      // create arcs t->interface and interface->t
      string placeName = remapped.substr( 0, remapped.find("/") );      // remove possible /

      Place * place = findPlace(placeName);

      if (inputPlacenames.count(placeName) > 0)
      {
        if (place == NULL) 
          place = &createPlace(placeName, Node::INPUT);
        createArc(*place, *transition);
      }
      else if (outputPlacenames.count(placeName) > 0)
      {
        if (place == NULL) 
          place = &createPlace(placeName, Node::OUTPUT);
        createArc(*transition, *place);
      }
      else if (synchronizeLabels.count(placeName) > 0)
      {
        // this is not an interface place but a synchronize label
        set<string> labelSet;
        labelSet.insert(placeName);
        transition->setSynchronizeLabels(labelSet);
      }
      // else it was a TAU transition
    }
  }

  // create arcs p->t

  // Create a map of string sets for final condition creation.
  map<string, set<string> > finalCondMap;

  for (set<string>::iterator p = places_.begin(); p != places_.end(); ++p)
  {
    for (set<string>::iterator t = arcs_[*p].begin(); t != arcs_[*p].end(); ++t)
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

  // fill interface places not occurring in the automaton
  for (set<string>::iterator input = inputPlacenames.begin(); 
         input != inputPlacenames.end(); ++input)
    if (findPlace(*input) == NULL)
      createPlace(*input, Node::INPUT);
  for (set<string>::iterator output = outputPlacenames.begin(); 
         output != outputPlacenames.end(); ++output)
    if (findPlace(*output) == NULL)
      createPlace(*output, Node::OUTPUT);

  // initialize final condition
  finalCondition() = false;
  
  // For each transition found to be a final transition...
  for (map<string, set<string> >::iterator transIt = finalCondMap.begin(); 
         transIt != finalCondMap.end(); ++transIt)
  {
    // Create a set for the places having this transition in their post set.
    set<Place *> nextTrans;

    // For each place in the preset...
    for (set<string>::iterator placesIt = transIt->second.begin(); 
           placesIt != transIt->second.end(); ++placesIt)
    {
      // Insert this place in the preset.
      nextTrans.insert(findPlace(*placesIt));
    }

    pnapi::formula::Conjunction *fd = NULL;
    pnapi::formula::FormulaEqual *store = NULL;
    // create clause for the final condition
    for (set<Place *>::iterator p = nextTrans.begin(); p != nextTrans.end(); ++p)
    {
      if (fd == NULL)
      {
        if (store == NULL)
        {
          store = new pnapi::formula::FormulaEqual(**p, 1);
        }
        else
        {
          pnapi::formula::FormulaEqual * tmpF = new pnapi::formula::FormulaEqual(**p, 1);
          fd = new pnapi::formula::Conjunction(*store, *tmpF);
          delete tmpF;
          delete store;
          store = NULL;
        }
      }
      else
      {
        pnapi::formula::FormulaEqual * tmpF1 = new pnapi::formula::FormulaEqual(**p, 1);
        pnapi::formula::Conjunction * tmpF2 = new pnapi::formula::Conjunction(*fd, *tmpF1);
        delete fd;
        delete tmpF1;
        fd = tmpF2;
      }
    }
    if (fd != NULL)
    {
      finalCondition() = finalCondition().formula() || *fd;
      delete fd;
    }
    else
    {
      if (store != NULL)
      {
        finalCondition() = finalCondition().formula() || *store;
        delete store;
      }
    }
  }

  // make interface places empty
  for(set<Place*>::iterator p = interfacePlaces_.begin();
       p != interfacePlaces_.end(); ++p)
  {
    finalCondition() = finalCondition().formula() && (**p == 0); 
  }
 
  // cleaning up
  remove(pnFileName.c_str());
}
