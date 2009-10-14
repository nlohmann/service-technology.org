/*!
 * \file    errorhandling.cc
 *
 * \brief   Collection of exception classes
 *
 * \author  Christian Sura <christian.sura@uni-rostock.de>,
 *          last changes of: \$Author: gierds $
 *
 * \since   2009-08-04
 *
 * \date    $Date: 2009-08-05 10:20:55 +0200 (Mi, 05. Aug 2009) $
 *
 * \version $Revision: 4517 $
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

