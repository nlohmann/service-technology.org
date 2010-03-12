/*!
 * \file  state.cc
 */

#include "config.h"

#include <iostream>
#include <sstream>
#include <map>

#include "automaton.h"
#include "marking.h"
#include "state.h"
#include "util.h"


using std::map;


namespace pnapi
{


/*** class State ***/


/*!
 * \brief standard constructor
 * 
 * The state's name, marking, and hash value are set to default values. The
 * name is unique within the whole automaton class.
 * Template for unique names of states: sX where X is a number.
 * 
 */
State::State(unsigned int * counter) :
  name_((*counter)++), isFinal_(false), isInitial_(false), m_(NULL),
  hashValue_(0)
{
}


/*!
 * \brief constructor
 */
State::State(const unsigned int name) :
  name_(name), isFinal_(false), isInitial_(false), m_(NULL), hashValue_(0)
{
}


/*!
 * \brief standard constructor service automaton
 */
State::State(Marking & m, std::map<const Place *, unsigned int> * pw,
             unsigned int & counter) :
  name_(counter++), isFinal_(false), isInitial_(false), m_(&m)
{
  setHashValue(pw);
}


/*!
 * \brief copy constructor
 */
State::State(const State & s) :
  name_(s.name_), preset_(s.preset_), postset_(s.postset_),
  isFinal_(s.isFinal_), isInitial_(s.isInitial_), m_(NULL),
  hashValue_(s.hashValue_)
{
  if (s.m_ != NULL)
  {
    m_ = new Marking(*s.m_);
  }
}

/*!
 * \brief copy a state from one automaton to another one
 */
State::State(const State & s, PetriNet * net,
             std::map<const Place *, const Place *> * placeMap) :
  name_(s.name_), isFinal_(s.isFinal_), isInitial_(s.isInitial_),
  hashValue_(s.hashValue_), m_(NULL) 
{
  if(s.m_ != NULL)
  {
    m_ = new Marking(*(s.m_), net, *placeMap);
  }
}

/*!
 * \brief destructor
 */
State::~State()
{
  delete m_;
}

/*!
 * \brief method returning the state's name
 */
unsigned int State::getName() const
{
  return name_;
}


/*!
 * \brief method returning the state's preset
 */
const std::set<State *> & State::getPreset() const
{
  return preset_;
}


/*!
 * \brief method returning the state's postset
 */
const std::set<State *> & State::getPostset() const
{
  return postset_;
}


/*!
 * \brief method returning the state's postset edges
 */
const std::set<Edge *> & State::getPostsetEdges() const
{
  return postsetEdges_;
}


/*!
 * \brief method returning whether the state is a final state
 */
bool State::isFinal() const
{
  return isFinal_;
}


/*!
 * \brief make this state a final state
 */
void State::setFinal()
{
  isFinal_ = true;
}


/*!
 * \brief method returning whether the state is an initial state
 */
bool State::isInitial() const
{
  return isInitial_;
}


/*!
 * \brief make this state an initial state
 */
void State::setInitial()
{
  isInitial_ = true;
}


/*!
 * \brief checks if 2 states are equal (by name, preset, and postset, or marking)
 * 
 * First, checks if a marking is given for this state and if so, the
 * equality of both markings is the result of the check. If no marking
 * is given, the equality is evaluated by the equality of both names.
 *
 * \return    true iff both markings are equal or their names are equal,
 *            else false.
 */
bool State::operator==(const State & s2) const
{
  if (m_ != NULL)
    return ((*m_) == (*s2.m_));
  else
    return (name_ == s2.name_);
}


/*!
 * \brief returns the state's marking
 */
Marking * State::getMarking() const
{
  return m_;
}


/*!
 * \brief returns the state's hash value (only needed by service automaton)
 */
unsigned int State::getHashValue() const
{
  return hashValue_;
}


/*!
 * \brief returns the size of the represented marking
 */
unsigned int State::size() const
{
  if (m_ != NULL)
    return m_->size();
  
  return 0;
}


/*!
 * \brief set the hash value
 */
void State::setHashValue(std::map<const Place *, unsigned int> * pw)
{
  if (m_ != NULL)
  {
    unsigned int hash = 0;
    PNAPI_FOREACH(i, m_->getMap())
    {
      hash += (i->second * (*pw)[i->first]) % Automaton::HASH_SIZE;
    }
    
    hashValue_ = hash % Automaton::HASH_SIZE;
  }
}


/*!
 * \brief adding a state to the preset
 */
void State::addPreState(State & pre)
{
  preset_.insert(&pre);
}


/*!
 * \brief adding a state to the postset
 */
void State::addPostState(State & post)
{
  postset_.insert(&post);
}


/*!
 * \brief adding an edge to the postset edges
 */
void State::addPostEdge(Edge & e)
{
  postsetEdges_.insert(&e);
}


/*** class Edge ***/


/*!
 * \brief constructor
 */
Edge::Edge(State & source, State & destination, const std::string & label, const Type type) :
   label_(label), source_(source), destination_(destination), type_(type)
{
  source_.addPostState(destination_);
  destination_.addPreState(source_);
  source_.addPostEdge(*this);
}


/*!
 * \brief destructor
 */
Edge::~Edge()
{
}


/*!
 * \brief method returning the edge's label
 */
const std::string & Edge::getLabel() const
{
  return label_;
}


/*!
 * \brief method returning the egde's source state
 */
State & Edge::getSource() const
{
  return source_;
}


/*!
 * \brief method returning the edge's destination state
 */
State & Edge::getDestination() const
{
  return destination_;
}


/*!
 * \brief method returning the type according to the edge (sa)
 */
Edge::Type Edge::getType() const
{
  return type_;
}

} /* END OF NAMESPACE pnapi */

