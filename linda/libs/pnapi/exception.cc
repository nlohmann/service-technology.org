/*!
 * \file    exception.cc
 *
 * \brief   Collection of exception classes
 *
 * \author  Christian Sura <christian.sura@uni-rostock.de>,
 *          last changes of: \$Author: cas $
 *
 * \since   2009-08-04
 *
 * \date    $Date: 2010-03-14 14:22:21 +0100 (Sun, 14 Mar 2010) $
 *
 * \version $Revision: 5514 $
 */

#include "config.h"

#include "exception.h"

using std::string;

namespace pnapi
{

namespace exception
{

/*!
 * \brief General Exception Constructor
 */
Error::Error(const std::string & msg) : 
  message(msg)
{
}


/*!
 * \brief constructor of input error
 */
InputError::InputError(Type type, const std::string & filename, int line,
                       const std::string & token, const std::string & msg) :
  Error(msg),
  type(type), token(token), line(line), filename(filename)
{
}

/*
 * \brief constructor
 */
NotImplementedError::NotImplementedError(const std::string & msg) :
  Error(msg)
{
}
  

/// constructor
AssertionFailedError::AssertionFailedError(const std::string & file_, unsigned int line_,
                                           const std::string & assertion) :
  Error(assertion), file(file_), line(line_)
{
}


} /* namespace exception */

} /* namespace pnapi */
