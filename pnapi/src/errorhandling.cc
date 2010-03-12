/*!
 * \file    errorhandling.cc
 *
 * \brief   Collection of exception classes
 *
 * \author  Christian Sura <christian.sura@uni-rostock.de>,
 *          last changes of: \$Author$
 *
 * \since   2009-08-04
 *
 * \date    $Date$
 *
 * \version $Revision$
 */

#include "config.h"
#include "petrinet.h"

#include <string>

using std::string;

namespace pnapi
{

namespace exceptions
{

/*!
 * \brief General Exception Constructor
 */
GeneralException::GeneralException(const std::string & msg) : 
  msg_(msg)
{
}

/*!
 * \brief Compose Error Constructor
 */
ComposeError::ComposeError(const std::string & msg) :
  GeneralException(msg)
{
}

/*!
 * \brief Unknown Transition Error Constructor
 */
UnknownTransitionError::UnknownTransitionError() :
  GeneralException("")
{
}

} /* namespace exceptions */

} /* namespace pnapi */
