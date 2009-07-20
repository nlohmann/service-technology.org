#include <sstream>
#include "automaton.h"
#include "marking.h"
#include "state.h"

#include <iostream>


namespace pnapi
{


  /*** class State ***/


  /*!
   * The state's name, marking, and hash value are set to default values. The
   * name is unique within the whole automaton class. Template for unique names
   * of states: sX where X is a number.
   */
  State::State(unsigned int *counter) :
    name_((*counter)++), isFinal_(false), isInitial_(false), m_(NULL),
    hashValue_(0)
  {
  }


  /*!
   *
   */
  State::State(const unsigned int name) :
    name_(name), isFinal_(false), isInitial_(false), m_(NULL), hashValue_(0)
  {
  }


  /*!
   */
  State::State(Marking &m, std::map<const Place *, unsigned int> *pw,
      unsigned int *counter) :
    name_((*counter)++), isFinal_(false), isInitial_(false), m_(&m)
  {
    setHashValue(pw);
  }


  /*!
   */
  State::State(const State &s) :
    name_(s.name_), preset_(s.preset_), postset_(s.postset_),
    isFinal_(s.isFinal_), isInitial_(s.isInitial_), m_(NULL),
    hashValue_(s.hashValue_)
  {
    if (s.m_ != NULL)
      m_ = new Marking(*s.m_);
  }


  /*!
   */
  const unsigned int State::name() const
  {
    return name_;
  }


  /*!
   */
  const std::set<State *> State::preset() const
  {
    return preset_;
  }


  /*!
   */
  const std::set<State *> State::postset() const
  {
    return postset_;
  }


  /*!
   *
   */
  const std::set<Edge *> State::postsetEdges() const
  {
    return postsetEdges_;
  }


  /*!
   */
  bool State::isFinal() const
  {
    return isFinal_;
  }


  /*!
   */
  void State::final()
  {
    isFinal_ = true;
  }


  /*!
   *
   */
  bool State::isInitial() const
  {
    return isInitial_;
  }


  /*!
   *
   */
  void State::initial()
  {
    isInitial_ = true;
  }


  /*!
   * First, checks if a marking is given for this state and if so, the
   * equality of both markings is the result of the check. If no marking
   * is given, the equality is evaluated by the equality of both names.
   *
   * \return    true iff both markings are equal or their names are equal,
   *            else false.
   */
  bool State::operator ==(const State &s2) const
  {
    if (m_ != NULL)
      return (*m_) == (*s2.m_);
    else
      return name_ == s2.name_;
  }


  /*!
   */
  Marking * State::marking() const
  {
    return m_;
  }


  /*!
   */
  const unsigned int State::hashValue()
  {
    return hashValue_;
  }


  /*!
   */
  const unsigned int State::size() const
  {
    if (m_ != NULL)
      return m_->size();

    return 0;
  }


  /*!
   */
  void State::setHashValue(std::map<const Place *, unsigned int> *pw)
  {
    if (m_ != NULL)
    {
      unsigned int hash = 0;
      for (std::map<const Place *, unsigned int>::const_iterator i =
          m_->getMap().begin(); i != m_->getMap().end(); i++)
        hash += (i->second * (*pw)[i->first]) % Automaton::HASH_SIZE;

      hashValue_ = hash % Automaton::HASH_SIZE;
    }
  }


  /*!
   *
   */
  void State::addPre(State &pre)
  {
    preset_.insert(&pre);
  }


  /*!
   *
   */
  void State::addPost(State &post)
  {
    postset_.insert(&post);
  }


  /*!
   *
   */
  void State::addPostEdge(Edge &e)
  {
    postsetEdges_.insert(&e);
  }



  /*** class Edge ***/


  /*!
   */
  Edge::Edge(State &source, State &destination, const std::string label, const Automaton::Type type) :
    label_(label), source_(source), destination_(destination), type_(type)
  {
    source_.addPost(destination_);
    destination_.addPre(source_);
    source_.addPostEdge(*this);
  }


  /*!
   */
  Edge::~Edge()
  {
  }


  /*!
   * \return    std::string label_
   */
  const std::string Edge::label() const
  {
    return label_;
  }


  /*!
   * \return    State &source_ state of the edge
   */
  State & Edge::source() const
  {
    return source_;
  }


  /*!
   * \return    State &destination_ state of the edge
   */
  State & Edge::destination() const
  {
    return destination_;
  }


  /*!
   * \return    Node::Type type_
   */
  Automaton::Type Edge::type() const
  {
    return type_;
  }

} /* END OF NAMESPACE pnapi */

