/*!
 * \file    port.cc
 *
 * \brief   Class Port and class Label
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

#include "config.h"

#include <iostream>
#include <string>

#include "port.h"
#include "petrinet.h"
#include "util.h"

using std::string;
using std::set;

namespace pnapi
{

/*** port class ***/

/*!
 * \brief constructor
 */
Port::Port(PetriNet & net, const std::string & name) :
  net_(net), name_(name)
{
}

/*!
 * \brief compose constructor
 */
Port::Port(PetriNet & net, const Port & port1, const Port & port2,
           std::map<Label *, Label *> & label2label, std::map<Label *, Place *> & label2place,
           std::set<Label *> & commonLabels) :
  net_(net), name_(port1.name_)
{
  // copy port1's input labels or compose them
  PNAPI_FOREACH(l1, port1.input_)
  {
    Label * l2 = port2.findLabel((*l1)->getName());
    if((l2 == NULL) || (l2->getType() != Label::OUTPUT)) // no matching label
    {
      // copy label
      Label * l = new Label(net_, *this, (*l1)->getName(), Label::INPUT);
      input_.insert(l);
      allLabels_.insert(l);
      label2label[*l1] = l;
    }
    else
    {
      // labels will be replaced by place
      Place * p = &(net_.createPlace(l2->getName()));
      label2place[*l1] = p;
      label2place[l2] = p;
    }
  }
  
  // copy port1's output labels or compose them
  PNAPI_FOREACH(l1, port1.output_)
  {
    Label * l2 = port2.findLabel((*l1)->getName());
    if((l2 == NULL) || (l2->getType() != Label::INPUT)) // no matching label
    {
      // copy label
      Label * l = new Label(net_, *this, (*l1)->getName(), Label::OUTPUT);
      input_.insert(l);
      allLabels_.insert(l);
      label2label[*l1] = l;
    }
    else
    {
      // labels will be replaced by place
      Place * p = &(net_.createPlace(l2->getName()));
      label2place[*l1] = p;
      label2place[l2] = p;
    }
  }
  
  // copy port2's input labels or compose them
  PNAPI_FOREACH(l1, port2.input_)
  {
    Label * l2 = port1.findLabel((*l1)->getName());
    if((l2 == NULL) || (l2->getType() != Label::OUTPUT)) // no matching label
    {
      // copy label
      Label * l = new Label(net_, *this, (*l1)->getName(), Label::INPUT);
      input_.insert(l);
      allLabels_.insert(l);
      label2label[*l1] = l;
    }
    // else already copied above
  }
  
  // copy port2's output labels or compose them
  PNAPI_FOREACH(l1, port2.output_)
  {
    Label * l2 = port1.findLabel((*l1)->getName());
    if((l2 == NULL) || (l2->getType() != Label::INPUT)) // no matching label
    {
      // copy label
      Label * l = new Label(net_, *this, (*l1)->getName(), Label::OUTPUT);
      input_.insert(l);
      allLabels_.insert(l);
      label2label[*l1] = l;
    }
    // else already copied above
  }
  
  // check synchronous labels
  PNAPI_FOREACH(l1, port1.synchronous_)
  {
    Label * l2 = port2.findLabel((*l1)->getName());
    if((l2 == NULL) || (l2->getType() != Label::SYNCHRONOUS)) // no matching label
    {
      // copy label
      Label * l = new Label(net_, *this, (*l1)->getName(), Label::SYNCHRONOUS);
      input_.insert(l);
      allLabels_.insert(l);
      label2label[*l1] = l;
    }
    else
    {
      commonLabels.insert(*l1);
      commonLabels.insert(l2);
    }
  }
  PNAPI_FOREACH(l1, port2.synchronous_)
  {
    Label * l2 = port1.findLabel((*l1)->getName());
    if((l2 == NULL) || (l2->getType() != Label::SYNCHRONOUS)) // no matching label
    {
      // copy label
      Label * l = new Label(net_, *this, (*l1)->getName(), Label::SYNCHRONOUS);
      input_.insert(l);
      allLabels_.insert(l);
      label2label[*l1] = l;
    }
    // else already done
  }
}

/*!
 * \brief destructor
 */
Port::~Port()
{
  PNAPI_FOREACH(l, allLabels_)
  {
    delete (*l);
  }
}

/*!
 * \brief adding a label
 * 
 * \todo remove me!
 */
Label & Port::addLabel(Label & label)
{
  switch (label.getType())
  {
  case Label::INPUT:
    input_.insert(&label);
    break;
  case Label::OUTPUT:
    output_.insert(&label);
    break;
  case Label::SYNCHRONOUS:
    synchronous_.insert(&label);
    break;
  default: /* do nothing */ ;
  }
  
  allLabels_.insert(&label);

  return label;
}

/*!
 * \brief adding a label with a given type
 */
Label & Port::addLabel(const std::string & label, Label::Type type)
{
  Label * l = new Label(net_, *this, label, type);
  return addLabel(*l);
}

/*!
 * \brief adding an input label
 */
Label & Port::addInputLabel(const std::string & label)
{
  Label * l = new Label(net_, *this, label, Label::INPUT);
  input_.insert(l);
  allLabels_.insert(l);
  return *l;
}

/*!
 * \brief adding an output label
 */
Label & Port::addOutputLabel(const std::string & label)
{
  Label * l = new Label(net_, *this, label, Label::OUTPUT);
  output_.insert(l);
  allLabels_.insert(l);
  return *l;
}

/*!
 * \brief adding a synchronous label
 */
Label & Port::addSynchronousLabel(const std::string & label)
{
  Label * l = new Label(net_, *this, label, Label::SYNCHRONOUS);
  synchronous_.insert(l);
  allLabels_.insert(l);
  return *l;
}

/*!
 * \brief deleting a label
 */
void Port::removeLabel(const std::string & label)
{
  Label * l = findLabel(label);
  if(l != NULL)
  {
    allLabels_.erase(l);
    switch(l->getType())
    {
    case Label::INPUT: input_.erase(l); break;
    case Label::OUTPUT: output_.erase(l); break;
    case Label::SYNCHRONOUS: synchronous_.erase(l); break;
    default: assert(false);
    }
    
    delete l;
  }
}

/*!
 * \brief swaps input and output labels
 */
void Port::mirror()
{
  set<Label *> tmp = input_;
  input_ = output_;
  output_ = input_;
  
  PNAPI_FOREACH(label, input_)
  {
    (*label)->mirror();
  }
  
  PNAPI_FOREACH(label, output_)
  {
    (*label)->mirror();
  }
}

/*!
 * \brief return the name of the port
 */
const std::string & Port::getName() const
{
  return name_;
}

/*!
 * \brief returning the set of input labels
 */
const std::set<Label *> & Port::getInputLabels() const
{
  return input_;
}

/*!
 * \brief returning the set of output labels
 */
const std::set<Label *> & Port::getOutputLabels() const
{
  return output_;
}

/*!
 * \brief returning the set of synchronous labels
 */
const std::set<Label *> & Port::getSynchronousLabels() const
{
  return synchronous_;
}

/*!
 * \brief returning the set of all labels
 */
const std::set<Label *> & Port::getAllLabels() const
{
  return allLabels_;
}

/*!
 * \brief get the underlying petri net
 */
PetriNet & Port::getPetriNet() const
{
  return net_;
}

/*!
 * \brief checks whether the port is empty
 */
bool Port::isEmpty() const
{
  return allLabels_.empty();
}

/*!
 * \brief seach for a certain label
 */
Label * Port::findLabel(const std::string & label) const
{
  PNAPI_FOREACH(l, allLabels_)
  {
    if(((*l)->getName()) == label)
    {
      return *l;
    }
  }
  
  return NULL;
}


/*** label class ***/


/*!
 * \brief constructor
 */
Label::Label(PetriNet & net, Port & port, const std::string & name, Type type) :
  net_(net), port_(port), name_(name), type_(type)
{
}


/*!
 * \brief destructor
 */
Label::~Label()
{
  PNAPI_FOREACH(t, transitions_)
  {
    (*t)->removeLabel(*this);    
  }
}

/*!
 * \brief swaps input and output labels
 * 
 * \note only mirror input or output labels
 */
void Label::mirror()
{
  switch(type_)
  {
  case INPUT: type_ = OUTPUT; break;
  case OUTPUT: type_ = INPUT; break;
  default: assert(false); // should not happen
  }
}

/*!
 * \brief returning the label's name
 */
const std::string & Label::getName() const
{
  return name_;
}

/*!
 * \brief returing the label's type
 */
Label::Type Label::getType() const
{
  return type_;
}

/*!
 * \brief get set of connected transitions
 */
const std::set<Transition *> & Label::getTransitions() const
{
  return transitions_;
}

/*!
 * \brief get the underlying petri net
 */
PetriNet & Label::getPetriNet() const
{
  return net_;
}

/*!
 * \brief get the assigned port
 */
Port & Label::getPort() const
{
  return port_;
}


/*!
 * \brief adding a transition to the attached transitions
 */
void Label::addTransition(Transition & t)
{
  transitions_.insert(&t);
}

/*!
 * \brief removing a transition from the attached transitions
 */
void Label::removeTransition(const Transition & t)
{
  transitions_.erase(const_cast<Transition *>(&t));
}

} /* namespace pnapi */
