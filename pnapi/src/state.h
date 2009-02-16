// -*- C++ -*-

#ifndef PNAPI_STATE_H
#define PNAPI_STATE_H

#include <set>
#include <string>

#include "marking.h"

using std::set;
using std::string;

namespace pnapi
{


  template <class T>
  class Edge
  {
  public:
    /*!
     * \brief
     */
    Edge(T &source, T &destination, const string label) :
      source_(source), destination_(destination), label_(label)
    {
      source_.getPostset().insert(&destination_);
      destination_.getPreset().insert(&source_);
    }


    /*!
     * \brief
     */
    virtual ~Edge()
    {
    }


    /*!
     * \brief
     */
    string getLabel() const
    {
      return label_;
    }


    /*!
     * \brief
     */
    T & getSource() const
    {
      return source_;
    }


    /*!
     * \brief
     */
    T & getDestination() const
    {
      return destination_;
    }


  private:
    string label_;
    T &source_;
    T &destination_;
  };


  class State
  {
  public:
    // standard constructor
    State(string name = "");
    // standard destructor
    virtual ~State();

    // returns the name
    const string & getName() const;
    // returns the preset
    set<State *> getPreset() const;
    // returns the postset
    set<State *> getPostset() const;

    // comparison operator for states
    virtual bool operator ==(const State &m) const;

  protected:
    // name of the state
    string name_;
    // preset
    set<State *> preset_;
    // postset
    set<State *> postset_;
  };


  class StateB : public State
  {
  public:
    /// Constructors & Destructor
    StateB(Marking &m);
    StateB(const StateB &s);
    virtual ~StateB();

    /// returns the marking represented by this StateB
    Marking & getMarking() const;

    /// returns the marking's size
    unsigned int size() const;

    unsigned int getHashValue();

    bool operator ==(const StateB &m) const;

  private:
    /// the represented marking
    Marking &m_;

    /// pointer to the hash value
    unsigned int *hashValue_;
  };


  class StateOG : public State
  {
  public:
    // standard constructor
    StateOG();
    // standard destructor
    virtual ~StateOG();

    // comparison operator
    bool operator ==(const StateOG &m) const;

  private:
    /// some type of formula

  };


}

#endif /* State_H */
