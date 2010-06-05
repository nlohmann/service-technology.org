// -*- C++ -*-

/*!
 * \file    exception.h
 *
 * \brief   exception classes
 *
 * \author  Niels Lohmann <nlohmann@informatik.hu-berlin.de>,
 *          Christian Gierds <gierds@informatik.hu-berlin.de>,
 *          last changes of: $Author: cas $
 *
 * \since   2005/11/11
 *
 * \date    $Date: 2010-05-29 23:35:16 +0200 (Sat, 29 May 2010) $
 *
 * \version $Revision: 5779 $
 */

#ifndef PNAPI_EXCEPTION_H
#define PNAPI_EXCEPTION_H

#include "config.h"

#include <string>

namespace pnapi
{

/*!
 * \brief Petri Net API Exceptions
 */
namespace exception
{

/*!
 * \brief general exception class
 */
class Error
{
public: /* public constants */
  /// exception message
  const std::string message;
  
public: /* public methods */
  /// constructor
  Error(const std::string &);
};


/*!
 * \brief Exception class thrown by operator>>()
 */
class InputError : public Error 
{
public: /* public types */
  /// types of errors
  enum Type
  {
    SYNTAX_ERROR,
    SEMANTIC_ERROR
  };

public: /* public constants */
  /// type of error
  const Type type;
  /// last read token
  const std::string token;
  /// line number
  const int line;
  /// filename
  const std::string filename;
  
public: /* public methods */
  /// constructor
  InputError(Type, const std::string &, int, const std::string &,
             const std::string &);
};

/*!
 * \brief Exception for features to be implemented
 */
class NotImplementedError : public Error
{
public: /* public methods */
  /// constructor
  NotImplementedError(const std::string &); 
};

/*!
 * \brief Exception thrown when an internal assertion failed
 */
class AssertionFailedError : public Error
{
public: /* public constants */
  /// file the error occurred in
  const std::string file;
  /// line the error occurred at
  const unsigned int line;
  
public: /* public methods */
  /// constructor
  AssertionFailedError(const std::string &, unsigned int,
                       const std::string &);
};

/*!
 * \brief Exception thrown when user caused an error.
 */
class UserCausedError : public Error
{
public: /* public types */
  /// user error types
  enum UE_Type
  {
    UE_NONE,
    UE_NODE_NAME_CONFLICT,
    UE_LABEL_NAME_CONFLICT,
    UE_ARC_CONFLICT
  };
  
public: /* public constants */
  /// user error type
  const UE_Type type;
  
public: /* public methods */
  /// constructor
  UserCausedError(UE_Type, const std::string &);
};


} /* namespace exception */

} /* namespace pnapi */

#endif /* PNAPI_EXCEPTION_H */
