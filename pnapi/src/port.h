/*!
 * \file    port.h
 *
 * \brief   Class Port
 *
 * \author  Niels Lohmann <nlohmann@informatik.hu-berlin.de>,
 *          Christian Gierds <gierds@informatik.hu-berlin.de>,
 *          Martin Znamirowski <znamirow@informatik.hu-berlin.de>,
 *          Robert Waltemath <robert.waltemath@uni-rostock.de>,
 *          Stephan Mennicke <stephan.mennicke@uni-rostock.de>,
 *          last changes of: $Author:$
 *
 * \since   2005/10/18
 *
 * \date    $Date:$
 *
 * \version $Revision:$
 */

#ifndef PNAPI_PORT_H
#define PNAPI_PORT_H

#include <map>
#include <set>
#include <string>

namespace pnapi
{

// forward declarations
class Interface;
class PetriNet;
class Place;
class Port;
class Transition;

/*!
 * \brief interface labels
 */
class Label
{
public: /* public types */
  /// communication type
  enum Type
  {
    INPUT,
    OUTPUT,
    SYNCHRONOUS
  };

private: /* private variables */
  /// the petri net this label belongs to
  PetriNet & net_;
  /// assigned port
  Port & port_;
  /// the label name
  std::string name_;
  /// the label type
  Type type_;
  /// set of transitions associated to this label
  std::set<Transition *> transitions_;
  
public: /* public methods */
  /// constructor
  Label(PetriNet &, Port &, const std::string &, Type);
  /// standard destructor
  virtual ~Label();

  /// adding a transition to the attached transitions
  void addTransition(Transition &);
  /// removing a transition from the attached transitions
  void removeTransition(const Transition &);
  /// swaps input and output labels
  void mirror();
  
  /// returning the label's name
  const std::string & getName() const;
  /// returing the label's type
  Type getType() const;
  /// get set of connected transitions
  const std::set<Transition *> & getTransitions() const;
  /// get the underlying petri net
  PetriNet & getPetriNet() const;
  /// get the assigned port
  Port & getPort() const; 

private: /* private methods */
  /// no copying allowed
  Label(const Label &);
};


/*!
 * \brief port class storing interface labels
 */
class Port
{
private: /* private variables */
  /// the net the port belongs to
  PetriNet & net_;
  /// name of the port
  std::string name_;
  /// asynchronous receive
  std::set<Label *> input_;
  /// asynchronous send
  std::set<Label *> output_;
  /// synchronous
  std::set<Label *> synchronous_;
  /// all labels for faster processing
  std::set<Label *> allLabels_;
    
public: /* public methods */
  /*!
   * \name constructors and destructors
   */
  //@{
  /// standard constructor (e.g. for global port)
  Port(PetriNet &, const std::string &);
  /// compose constructor
  Port(PetriNet &, const Port &, const Port &, std::map<Label *, Label *> &,
       std::map<Label *, Place *> &, std::set<Label *> &); 
  /// standard destructor
  virtual ~Port();
  //@}

  /*!
   * \name structural changes
   */
  //@{
  /// adding a label
  Label & addLabel(Label &);
  /// adding a label with a given type
  Label & addLabel(const std::string &, Label::Type);
  /// adding an input label
  Label & addInputLabel(const std::string &);
  /// adding an output label
  Label & addOutputLabel(const std::string &);
  /// adding a synchronous label
  Label & addSynchronousLabel(const std::string &);
  /// deleting a label
  void removeLabel(const std::string &);
  /// swaps input and output labels
  void mirror();
  //@}

  /*!
   * \brief getter
   */
  //@{
  /// return the name of the port
  const std::string & getName() const;
  /// returning the set of input labels
  const std::set<Label *> & getInputLabels() const;
  /// returning the set of output labels
  const std::set<Label *> & getOutputLabels() const;
  /// returning the set of synchronous label
  const std::set<Label *> & getSynchronousLabels() const;
  /// returning the set of all labels
  const std::set<Label *> & getAllLabels() const;
  /// returning the port string
  const std::string getPortString() const;
  /// checks whether the port is empty
  bool isEmpty() const;
  /// seach for a certain label
  Label * findLabel(const std::string &) const;
  /// get the underlying petri net
  PetriNet & getPetriNet() const;
  //@}

private: /* private methods */
  /// no copying allowed
  Port(const Port &);
};

} /* namespace pnapi */

#endif /* PNAPI_PORT_H */
