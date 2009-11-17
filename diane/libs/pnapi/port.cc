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

#include "port.h"

using std::string;
using std::vector;

namespace pnapi
{

  /*!
   * \class Port
   */

  Port::Port()
  {
  }

  Port::Port(const Port &port) :
    input_(port.input_.size()), output_(port.output_.size()), synchronous_(
        port.synchronous_.size())
  {
    for (int i = 0; i < (int) port.input_.size(); ++i)
      input_[i] = new Label(*port.input_[i]);
    for (int i = 0; i < (int) port.output_.size(); ++i)
      output_[i] = new Label(*port.output_[i]);
    for (int i = 0; i < (int) port.synchronous_.size(); ++i)
      synchronous_[i] = new Label(*port.synchronous_[i]);
  }

  Port::~Port()
  {
    for (int i = 0; i < (int) input_.size(); ++i)
      delete input_[i];
    for (int i = 0; i < (int) output_.size(); ++i)
      delete output_[i];
    for (int i = 0; i < (int) synchronous_.size(); ++i)
      delete synchronous_[i];
    input_.clear();
    output_.clear();
    synchronous_.clear();
  }

  void Port::addInputLabel(const string &label)
  {
    // TODO: maybe check whether label is already in this port
    input_.push_back(new Label(label, Label::INPUT));
  }

  void Port::addOutputLabel(const string &label)
  {
    // TODO: maybe check whether label is already in this port
    output_.push_back(new Label(label, Label::OUTPUT));
  }

  void Port::addSynchronousLabel(const string &label)
  {
    // TODO: maybe check whether label is already in this port
    synchronous_.push_back(new Label(label, Label::SYNCHRONOUS));
  }

  const vector<Label *> Port::inputLabels() const
  {
    return input_;
  }

  const vector<Label *> Port::outputLabels() const
  {
    return output_;
  }

  const vector<Label *> Port::synchronousLabel() const
  {
    return synchronous_;
  }

  /*!
   * \brief returning the port's interface string (oWFN)
   *
   * \return result string as described below
   *
   *   INPUT <comma-separated-list>;
   *
   *   OUTPUT <comma-separated-list>;
   *
   *   SYNCHRONOUS <comma-separated-list>;
   *
   * \note This method does _neither_ include the keyword PORT
   *       _nor_ the port's name!
   */
  const string Port::portString() const
  {
    string in = inputLabelString();
    string out = outputLabelString();
    string synch = synchronousLabelString();
    string result;
    result += in.empty() ? "" : "  INPUT " + in + ";\n";
    result += out.empty() ? "" : "  OUTPUT " + out + ";\n";
    result += synch.empty() ? "" : "  SYNCHRONOUS " + synch + ";\n";
    return result;
  }

  const string Port::inputLabelString() const
  {
    string result;
    for (int i = 0; i < (int) input_.size(); ++i)
      result += i == 0 ? input_[i]->label() : (", " + input_[i]->label());
    return result;
  }

  const string Port::outputLabelString() const
  {
    string result;
    for (int i = 0; i < (int) input_.size(); ++i)
      result += i == 0 ? input_[i]->label() : (", " + input_[i]->label());
    return result;
  }

  const string Port::synchronousLabelString() const
  {
    string result;
    for (int i = 0; i < (int) input_.size(); ++i)
      result += i == 0 ? input_[i]->label() : (", " + input_[i]->label());
    return result;
  }

  /*!
   * \class Label
   */

  Label::Label()
  {
  }

  Label::Label(const string &id, Type type) :
    id_(id), type_(type)
  {
  }

  Label::Label(const Label &label) :
    id_(label.id_), type_(label.type_)
  {
  }

  Label::~Label()
  {
  }

  const std::string Label::label() const
  {
    return id_;
  }

  const Label::Type Label::type() const
  {
    return type_;
  }

  const std::string Label::typedString() const
  {
    switch (type_)
    {
    case INPUT:
      return "?" + id_;
    case OUTPUT:
      return "!" + id_;
    case SYNCHRONOUS:
      return "#" + id_;
    }
  }

} /* namespace pnapi */
