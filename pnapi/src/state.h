// -*- C++ -*-

#ifndef PNAPI_STATE_H
#define PNAPI_STATE_H

#include <set>
#include <string>

#include "marking.h"
#include "component.h"

using std::set;
using std::string;

namespace pnapi
{

  template <class T>
  class Edge
  {
  public:
    /// standard constructor
    Edge(T &source, T &destination, const string label, pnapi::Node::Type type);
    /// standard destructor
    virtual ~Edge() {}

    /// returns the label
    string getLabel() const;
    /// returns the source node (state)
    T & getSource() const;
    /// returns the destination node (state)
    T & getDestination() const;
    /// returns the type of the edge
    Node::Type getType() const;

  private:
    string label_;
    T &source_;
    T &destination_;

    Node::Type type_;
  };


  class State
  {
  public:
    // standard constructor
    State(string name = "", bool isFinal = false);
    // standard destructor
    virtual ~State();

    // returns the name
    const string & getName() const;
    // returns the preset
    set<State *> getPreset() const;
    // returns the postset
    set<State *> getPostset() const;

    void final();
    bool isFinal() const;

    // comparison operator for states
    virtual bool operator ==(const State &m) const;

  protected:
    // name of the state
    string name_;
    // preset
    set<State *> preset_;
    // postset
    set<State *> postset_;
    // final
    bool isFinal_;
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


  /*** Edge<T> implementation ***/

  /*!
   * \brief
   */
  template <class T>
  Edge<T>::Edge(T &source, T &destination, const string label,
      pnapi::Node::Type type) :
    source_(source), destination_(destination), label_(label), type_(type)
  {
    source_.getPostset().insert(&destination_);
    destination_.getPreset().insert(&source_);
  }


  /*!
   * \brief
   */
  template <class T>
  string Edge<T>::getLabel() const
  {
    return label_;
  }


  /*!
   * \brief
   */
  template <class T>
  T & Edge<T>::getSource() const
  {
    return source_;
  }


  /*!
   * \brief
   */
  template <class T>
  T & Edge<T>::getDestination() const
  {
    return destination_;
  }


  template <class T>
  Node::Type Edge<T>::getType() const
  {
    return type_;
  }


}

#endif /* State_H */
