/*!
 * \file    errorhandling.cc
 *
 * \brief   Collection of exception classes
 *
 * \author  Christian Sura <christian.sura@uni-rostock.de>,
 *          last changes of: \$Author: cas $
 *
 * \since   2009-08-04
 *
 * \date    $Date: 2010-01-22 00:23:39 +0100 (Fri, 22 Jan 2010) $
 *
 * \version $Revision: 5258 $
 */

#include "config.h"
#include "petrinet.h"

#include <string>

using std::string;

namespace pnapi
{
namespace exceptions
{

GeneralException::GeneralException(std::string msg) : 
  msg_(msg)
  {
  }

ComposeError::ComposeError(std::string msg) :
  GeneralException(msg)
  {
  }

UnknownTransitionError::UnknownTransitionError() :
  GeneralException("")
  {
  }

} /* namespace exceptions */
} /* namespace pnapi */

