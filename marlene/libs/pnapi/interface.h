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
 *          last changes of: $Author:$
 *
 * \since   2005/10/18
 *
 * \date    $Date:$
 *
 * \version $Revision:$
 */

#ifndef INTERFACE_H
#define INTERFACE_H

#include <map>
#include <string>

namespace pnapi
{

  // forward declarations
  class Port;

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
    void addPort(const std::string &name);

    /// adding an input label
    void addInputLabel(const std::string &label,
        const std::string &port = "");

    /// adding an output label
    void addOutputLabel(const std::string &label,
        const std::string &port = "");

    /// adding a synchronous label
    void addSynchronousLabel(const std::string &label,
        const std::string &port = "");

  private:
    /// the global port
    Port *global_;

    /// the other ports
    std::map<std::string, Port *> ports_;

  };

} /* namespace pnapi */

#endif /* INTERFACE_H */
