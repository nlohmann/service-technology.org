/*!
 * \file    interface.cc
 *
 * \brief   Interface class holding asynchronous and synchronous communication
 *
 * \author  Niels Lohmann <nlohmann@informatik.hu-berlin.de>,
 *          Christian Gierds <gierds@informatik.hu-berlin.de>,
 *          Martin Znamirowski <znamirow@informatik.hu-berlin.de>,
 *          Robert Waltemath <robert.waltemath@uni-rostock.de>,
 *          Stephan Mennicke <stephan.mennicke@uni-rostock.de>,
 *          last changes of: $Author$
 *
 * \since   2005-10-18
 *
 * \date    $Date$
 *
 * \version $Revision$
 */

#include "interface.h"
#include "port.h"
#include <cassert>
#include <sstream>

using std::map;
using std::string;
using std::stringstream;
using std::vector;

namespace pnapi
{

  Interface::Interface() :
    global_(new Port())
  {
  }

  Interface::Interface(const Interface &interface) :
    global_(interface.global_), ports_(interface.ports_)
  {
  }

  Interface::~Interface()
  {
    delete global_;
    for (map<string, Port *>::iterator port = ports_.begin(); port
        != ports_.end(); ++port)
      delete port->second;
    ports_.clear();
  }

  Port & Interface::addPort(const std::string &name)
  {
    assert(!ports_.count(name));

    Port *p = new Port();
    ports_[name] = p;
    return *p;
  }

  Label & Interface::addInputLabel(const std::string &label,
      Port *port)
  {
    if (port)
      return port->addInputLabel(label);
    else
      return global_->addInputLabel(label);
  }

  Label & Interface::addInputLabel(const std::string &label,
      const std::string &port)
  {
    assert(ports_.count(port));
    return addInputLabel(label, ports_.find(port)->second);
  }

  Label & Interface::addOutputLabel(const string &label, Port *port)
  {
    if (port)
      return port->addInputLabel(label);
    else
      return global_->addInputLabel(label);
  }

  Label & Interface::addOutputLabel(const string &label, const string &port)
  {
    assert(ports_.count(port));
    return addOutputLabel(label, ports_.find(port)->second);
  }

  Label & Interface::addSynchronousLabel(const string &label, Port *port)
  {
    if (port)
      return port->addInputLabel(label);
    else
      return global_->addInputLabel(label);
  }

  Label & Interface::addSynchronousLabel(const string &label, const string &port)
  {
    assert(ports_.count(port));
    return addOutputLabel(label, ports_.find(port)->second);
  }

  const Port & Interface::getPort(const string &port) const
  {
    if (ports_.count(port))
      return *ports_.find(port)->second;
    else
      return *global_;
  }

  const vector<Label *> Interface::getInputLabels(Port *port) const
  {
    return port->inputLabels();
  }

  const vector<Label *> Interface::getInputLabels(const string &port) const
  {
    if (port.compare(""))
      return getInputLabels(ports_.find(port)->second);
    else
      return global_->inputLabels();
  }

  const vector<Label *> Interface::getOutputLabels(Port *port) const
  {
    return port->outputLabels();
  }

  const vector<Label *> Interface::getOutputLabels(const string &port) const
  {
    if (port.compare(""))
      return getOutputLabels(ports_.find(port)->second);
    else
      return global_->outputLabels();
  }

  const vector<Label *> Interface::getSynchronousLabels(Port *port) const
  {
    return port->synchronousLabels();
  }

  const vector<Label *> Interface::getSynchronousLabels(const string &port) const
  {
    if (port.compare(""))
      return getSynchronousLabels(ports_.find(port)->second);
    else
      return global_->synchronousLabels();
  }

  typedef map<string, Port *> portmap;
  Label * Interface::findLabel(const string &label) const
  {
    Label *l = global_->findLabel(label);
    if (l)
      return l;
    for (portmap::const_iterator i = ports_.begin(); i != ports_.end(); ++i)
    {
      l = i->second->findLabel(label);
      if (l)
        return l;
    }
    return NULL;
  }

  const string Interface::interfaceString() const
  {
    stringstream str;

    str << "INTERFACE\n";
    if (ports_.empty())
    {
      str << global_->portString();
    }
    else
    {
      for (map<string, Port *>::const_iterator p = ports_.begin(); p
          != ports_.end(); ++p)
      {
        str << "  PORT " << p->first << "\n"
            << "  " << p->second->portString() << "\n";
      }
    }

    return str.str();
  }

} /* namespace pnapi */
