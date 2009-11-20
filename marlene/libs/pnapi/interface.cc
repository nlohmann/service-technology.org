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
 *          last changes of: $Author:$
 *
 * \since   2005-10-18
 *
 * \date    $Date:$
 *
 * \version $Revision:$
 */

#include "interface.h"
#include "port.h"
#include <cassert>

using std::map;
using std::string;

namespace pnapi
{

  Interface::Interface() :
      global_()
  {
  }

  Interface::Interface(const Interface &interface) :
      global_(interface.global_), ports_(interface.ports_)
  {
  }

  Interface::~Interface()
  {
    delete global_;
    for (map<string, Port *>::iterator port = ports_.begin();
        port != ports_.end(); ++port)
      delete port->second;
    ports_.clear();
  }

  void Interface::addPort(const string &name)
  {
    assert(!ports_.count(name));

    ports_[name] = new Port();
  }

  void Interface::addInputLabel(const string &label,
      const string &port)
  {
    if (!port.compare(""))
    {
      global_->addInputLabel(label);
    }
    else
    {
      if (!ports_.count(port))
      {
        // TODO: maybe throw exception
        addPort(port);
      }
      ports_[port]->addInputLabel(label);
    }
  }

  void Interface::addOutputLabel(const string &label,
      const string &port)
  {
    if (!port.compare(""))
    {
      global_->addOutputLabel(label);
    }
    else
    {
      if (!ports_.count(port))
      {
        // TODO: maybe throw exception
        addPort(port);
      }
      ports_[port]->addOutputLabel(label);
    }
  }

  void Interface::addSynchronousLabel(const string &label,
      const string &port)
  {
    if (!port.compare(""))
    {
      global_->addSynchronousLabel(label);
    }
    else
    {
      if (!ports_.count(port))
      {
        // TODO: maybe throw exception
        addPort(port);
      }
      ports_[port]->addSynchronousLabel(label);
    }
  }

} /* namespace pnapi */
