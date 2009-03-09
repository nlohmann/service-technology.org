#include <cmath>
#include <map>
#include <sstream>
#include <string>

#include "state.h"

using std::map;
using std::string;
using std::stringstream;


namespace pnapi
{

  /******************************************************************
   *                      Basic Class State                         *
   ******************************************************************/

  /*!
   * \brief   standard constructor
   */
  State::State(string name, bool isFinal) :
    preset_(), postset_(), isFinal_(isFinal)
  {
    static unsigned int number = 1;
    string snumber;
    stringstream sstream;
    sstream << number++;
    sstream >> snumber;

    if (name == "")
      name_ = "s" + snumber;
    else
      name_ = name;
  }


  /*!
   * \brief   destructor
   */
  State::~State()
  {
    preset_.clear();
    postset_.clear();
  }


  /*!
   * \brief
   */
  const string & State::getName() const
  {
    return name_;
  }


  /*!
   * \brief   returns the preset
   */
  set<State *> State::getPreset() const
  {
    return preset_;
  }


  /*!
   * \brief   returns the postset
   */
  set<State *> State::getPostset() const
  {
    return postset_;
  }


  /*!
   * \brief   toggles the isFinal_ property
   */
  void State::final()
  {
    isFinal_ = !isFinal_;
  }


  /*!
   * \brief   Returns the final value of the state
   */
  bool State::isFinal() const
  {
    return isFinal_;
  }


  /*!
   * \brief   Comparison operater ==
   *
   * \note    I don't know if two states are equal if all the
   *          attributes are equal. Maybe it's enough to compare
   *          the post- and presets.
   */
  bool State::operator ==(const State &m) const
  {
    if (name_ == m.name_ && preset_ == m.preset_ && postset_ == m.postset_)
      return true;
    else
      return false;
  }

  /******************************************************************
   *                      State Class Build                         *
   ******************************************************************/


  /*!
   * \brief   Standard constructor taking a marking
   */
  StateB::StateB(Marking &m) :
    m_(m), hashValue_(NULL)
  {
  }


  /*!
   * \brief   Copy constructor
   */
  StateB::StateB(const StateB &s) :
    m_(s.m_)
  {
    hashValue_ = new unsigned int(*s.hashValue_);
  }


  /*!
   * \brief   Standard destructor
   */
  StateB::~StateB()
  {
  }


  /*!
   * \brief   Returns the representing marking
   */
  Marking & StateB::getMarking() const
  {
    return m_;
  }


  /*!
   * \brief   Returns the size of one state
   *
   * The state's size depends on the size of the marking vector.
   */
  unsigned int StateB::size() const
  {
    return m_.size();
  }


  /*!
   * \brief   Returns the hash value of one state
   *
   * If there is already set a hash value, the method just returns
   * it. If not, the method calculates one and writes it to the property
   * hashValue_. Afterwards, it will be returned.
   */
  unsigned int StateB::getHashValue()
  {
    if (hashValue_ == NULL)
    {
      unsigned int hash = 0;
      for (map<const Place *, unsigned int>::const_iterator i = m_.getMap().begin();
          i != m_.getMap().end(); i++)
        hash += i->second;

      hashValue_ = new unsigned int(hash);
    }

    return *hashValue_;
  }


  /*!
   * \brief   Checks if two StateBs are equal
   *
   * Two StateBs i and j are equal iff their markings
   * they represent are equal.
   *
   * \param   StateB j
   *
   * \return  TRUE iff the condition is fulfilled.
   */
  bool StateB::operator ==(const StateB &j) const
  {
    return m_ == j.m_;
  }


  /******************************************************************
   *                      OG State Class                            *
   ******************************************************************/


  /*!
   * \brief
   */
  StateOG::StateOG()
  {
  }


  /*!
   * \brief
   */
  StateOG::~StateOG()
  {
  }


  /*!
   * \brief
   */
  bool StateOG::operator ==(const StateOG &m) const
  {
    return false;
  }


}
