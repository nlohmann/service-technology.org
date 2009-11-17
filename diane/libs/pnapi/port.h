/*!
 * \file    port.h
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

#ifndef PORT_H
#define PORT_H

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

    /// adding a receive label
    void addInputLabel(const std::string &label);

    /// adding a send label
    void addOutputLabel(const std::string &label);

    /// adding a synchronous label
    void addSynchronousLabel(const std::string &label);

    /// returning the vector of input labels
    const std::vector<Label *> inputLabels() const;

    /// returning the vector of output labels
    const std::vector<Label *> outputLabels() const;

    /// returning the vector of synchronous label
    const std::vector<Label *> synchronousLabel() const;

    /// returning the port string
    const std::string portString() const;

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
    const Type type() const;

    /// returning the label's typed string (e.g. for input a = ?a)
    const std::string typedString() const;

  private:
    /// the label name
    std::string id_;

    /// the label type
    Type type_;

  };

} /* namespace pnapi */

#endif /* PORT_H */
