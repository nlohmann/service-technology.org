/*!
 * \file    interface.h
 *
 * \brief   Class Interface
 *
 * \author  Niels Lohmann <nlohmann@informatik.hu-berlin.de>,
 *          Christian Gierds <gierds@informatik.hu-berlin.de>,
 *          Martin Znamirowski <znamirow@informatik.hu-berlin.de>,
 *          Robert Waltemath <robert.waltemath@uni-rostock.de>,
 *          Stephan Mennicke <stephan.mennicke@uni-rostock.de>,
 *          last changes of: $Author: $
 *
 * \since   2005/10/18
 *
 * \date    $Date: $
 *
 * \version $Revision: $
 */

#ifndef INTERFACE_H
#define INTERFACE_H

#include <map>
#include <string>
#include <vector>
#include "port.h"

namespace pnapi
{

  class Port;
  class Label;

  class Interface
  {
  public:
    /// standard constructor
    Interface();

    /// standard copy constructor
    Interface(const Interface &);

    /// standard destructor
    virtual ~Interface();

    /// adding a single port
    Port & addPort(const std::string &name);

    /// adding an input label
    Label & addInputLabel(const std::string &label, Port *port = NULL);

    /// adding an input label
    Label & addInputLabel(const std::string &label, const std::string &port);

    /// adding an output label
    Label & addOutputLabel(const std::string &label, Port *port = NULL);

    /// adding an input label
    Label & addOutputLabel(const std::string &label, const std::string &port);

    /// adding a synchronous label
    Label & addSynchronousLabel(const std::string &label, Port *port = NULL);

    /// adding an input label
    Label & addSynchronousLabel(const std::string &label, const std::string &port);

    /// returning the port that belongs to the given name
    const Port & getPort(const std::string &port = "") const;

    /// returning input labels
    const std::vector<Label *> getInputLabels(Port *port = NULL) const;

    /// returning input labels
    const std::vector<Label *> getInputLabels(const std::string &port = "") const;

    /// returning output labels
    const std::vector<Label *> getOutputLabels(Port *port = NULL) const;

    /// returning output labels
    const std::vector<Label *> getOutputLabels(const std::string &port = "") const;

    /// returning synchronous labels
    const std::vector<Label *> getSynchronousLabels(Port *port = NULL) const;

    /// returning synchronous labels
    const std::vector<Label *> getSynchronousLabels(const std::string &port = "") const;

    Label * findLabel(const std::string &label) const;

    const std::string interfaceString() const;

    bool isUsed() const;

  private:
    /// the global port
    Port *global_;

    /// the other ports
    std::map<std::string, Port *> ports_;

  };

} /* namespace pnapi */

#endif /* INTERFACE_H */
