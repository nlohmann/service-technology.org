/*!
 * \file    errorhandling.cc
 *
 * \brief   Collection of exception classes
 *
 * \author  Christian Sura <christian.sura@uni-rostock.de>,
 *          last changes of: \$Author: niels $
 *
 * \since   2009-08-04
 *
 * \date    $Date: 2009-10-14 11:30:09 +0200 (Mi, 14. Okt 2009) $
 *
 * \version $Revision: 4827 $
 */

#include "config.h"
#include "petrinet.h"

#include <string>

using std::string;

namespace pnapi
{
namespace exceptions
{

GeneralException::GeneralException(string msg) : 
  msg_(msg)
  {
  }

ComposeError::ComposeError(string msg) :
  GeneralException(msg)
  {
  }

UnknownTransitionError::UnknownTransitionError() :
  GeneralException("")
  {
  }

} /* namespace exceptions */
} /* namespace pnapi */

