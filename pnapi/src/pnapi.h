// -*- C++ -*-

/*!
 * \file    pnapi.h
 *
 * \brief   Petri Net API main header
 *
 * \since   2008/12/09
 *
 * \date    $Date$
 *
 * \version $Revision$
 */

#ifndef PNAPI_PNAPI_H
#define PNAPI_PNAPI_H

#include "io.h"
#include "condition.h"
#include "petrinet.h"
#include "automaton.h"
#include "state.h"

// overloaded operators should be available globally
using pnapi::io::operator<<;
using pnapi::io::operator>>;
using pnapi::operator==;
using pnapi::operator>;
using pnapi::operator>=;
using pnapi::operator<;
using pnapi::operator<=;
using pnapi::operator!=;
using pnapi::operator!;
using pnapi::operator||;
using pnapi::operator&&;


/*!
 * \mainpage
 * 
 * The Petri Net API provides structures for working with Petri nets.
 *
 * For using it, simply include the header file pnapi.h in your code:
 * \code
 * #include "pnapi.h"
 * \endcode
 *
 * Everything you will use can be found in the following namespaces:
 * - pnapi
 * - pnapi::formula
 * - pnapi::io
 *
 * The classes pnapi::PetriNet and pnapi::Automaton provide the main 
 * functionality and can serve as
 * an entry point for getting an overview of the API's functionality.
 */


/*!
 * \brief   Petri Net API
 *
 * Structures for working with Petri nets (PetriNet) and service automata (Automaton).
 */
namespace pnapi
{
}

#endif

