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
#include <set>
#include "port.h"

namespace pnapi
{

// forwar declarations
class PetriNet;

/*!
 * \brief interface of a petrinet storing all labels
 * 
 * An interface stores a set of ports containing input, output
 * and synchronous labels.
 * Either each port has a name or there exists only an anonymous
 * global port with the empty string as name.
 * 
 * \todo shall the interface or the user take care of the naming constraints?
 * \todo ensure that there are no label duplicates
 */
class Interface
{
private: /* private variables */
  /// the ports
  std::map<std::string, Port *> ports_;
  /// the net the interface belongs to
  PetriNet & net_;
  
public: /* public methods */
  /*!
   * \name constructors and destructors
   */
  //@{
  /// standard constructor
  Interface(PetriNet &);
  /// compose constructor
  Interface(PetriNet &, const Interface &, const Interface &, std::map<Label *, Label *> &,
            std::map<Label *, Place *> &, std::set<Label *> &);
  /// standard destructor
  virtual ~Interface();
  //@}

  /*!
   * \name structural changes
   */
  //@{
  /// adding a single port
  Port & addPort(const std::string &);
  /// adding an input label
  Label & addInputLabel(const std::string &, Port &);
  /// adding an input label
  Label & addInputLabel(const std::string &, const std::string & = "");
  /// adding an output label
  Label & addOutputLabel(const std::string &, Port &);
  /// adding an output label
  Label & addOutputLabel(const std::string &, const std::string & = "");
  /// adding a synchronous label
  Label & addSynchronousLabel(const std::string &, Port &);
  /// adding a synchronous label
  Label & addSynchronousLabel(const std::string &, const std::string & = "");
  /// swaps input and output labels
  void mirror();
  /// removes all ports and their labels
  void clear();
  //@}

  /*!
   * \name getter
   */
  //@{
  /// returning the port that belongs to the given name
  Port * getPort(const std::string & = "") const;
  /// returning ports map
  const std::map<std::string, Port *> & getPorts() const;
  /// returning input labels
  const std::set<Label *> & getInputLabels(Port &) const;
  /// returning input labels
  std::set<Label *> getInputLabels(const std::string & = "") const;
  /// returning output labels
  const std::set<Label *> & getOutputLabels(Port &) const;
  /// returning output labels
  std::set<Label *> getOutputLabels(const std::string & = "") const;
  /// returning synchronous labels
  const std::set<Label *> & getSynchronousLabels(Port &) const;
  /// returning synchronous labels
  std::set<Label *> getSynchronousLabels(const std::string & = "") const;
  /// returning asynchronous labels (i.e. union of input and output places)
  std::set<Label *> getAsynchronousLabels(Port &) const;
  /// returning asynchronous labels (i.e. union of input and output places)
  std::set<Label *> getAsynchronousLabels(const std::string & = "") const;
  /// get the underlying petri net
  PetriNet & getPetriNet() const;
  /// find a certain label
  Label * findLabel(const std::string &) const;
  /// if there is no port or label defined
  bool isEmpty() const;
  //@}
  
private: /* private methods */
  /// no copying allowed
  Interface(const Interface &);
};

} /* namespace pnapi */

#endif /* INTERFACE_H */
