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

#ifndef PORT_H
#define PORT_H

#include "component.h"
#include "interface.h"
#include <string>
#include <vector>

namespace pnapi
{

  // forward declarations
  class Label;

  class Port
  {
  public:

    /// standard constructor (e.g. for global port)
    Port();

    /// standard copy-constructor
    Port(const Port &);

    /// standard destructor
    virtual ~Port();

    Label & addLabel(Label &l);

    /// adding a receive label
    Label & addInputLabel(const std::string &label);

    /// adding a send label
    Label & addOutputLabel(const std::string &label);

    /// adding a synchronous label
    Label & addSynchronousLabel(const std::string &label);

    /// returning the vector of input labels
    const std::vector<Label *> inputLabels() const;

    /// returning the vector of output labels
    const std::vector<Label *> outputLabels() const;

    /// returning the vector of synchronous label
    const std::vector<Label *> synchronousLabels() const;

    /// returning the port string
    const std::string portString() const;

    /// checks whether the port is empty
    bool isEmpty() const;

    Label * findLabel(const std::string &label) const;

  private:
    /// returning the port's input label string
    const std::string inputLabelString() const;

    /// returning the port's output label string
    const std::string outputLabelString() const;

    /// returning the port's synchronous label string
    const std::string synchronousLabelString() const;

  private:
    /// asynchronous receive
    std::vector<Label *> input_;

    /// asynchronous send
    std::vector<Label *> output_;

    /// synchronous
    std::vector<Label *> synchronous_;

  };


  class Label
  {
  public:
    /// communication type
    enum Type { INPUT, OUTPUT, SYNCHRONOUS };

    /// standard constructor (to be private)
    Label();

    /// standard constructor
    Label(const std::string &id, Type type);

    /// standard copy constructor
    Label(const Label &);

    /// standard destructor
    virtual ~Label();

    /// returning the label's identifier
    const std::string label() const;

    /// returing the label's type
    Type type() const;

    /// returning the label's typed string (e.g. for input a = ?a)
    const std::string typedString() const;

    /// adding a transition to the attached transitions
    void addTransition(Transition *t);

    /// removing a transition from the attached transitions
    void removeTransition(const Transition *t);

  private:
    /// the label name
    std::string id_;

    /// the label type
    Type type_;

    /// set of transitions
    std::vector<Transition *> transitions_;

  };

} /* namespace pnapi */

#endif /* PORT_H */
