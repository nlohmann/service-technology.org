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
 * \date    $Date: 2010-05-29 23:35:16 +0200 (Sat, 29 May 2010) $
 *
 * \version $Revision: 5779 $
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
  

/*!
 * \brief constructor
 */
AssertionFailedError::AssertionFailedError(const std::string & file_, unsigned int line_,
                                           const std::string & assertion) :
  Error(assertion), file(file_), line(line_)
{
}

/*!
 * \brief constructor
 */
UserCausedError::UserCausedError(UE_Type type_, const std::string & msg) :
  Error(msg), type(type_)
{
}


} /* namespace exception */

} /* namespace pnapi */
