// -*- C++ -*-

/*!
 * \file    exception.h
 *
 * \brief   exception classes
 *
 * \author  Niels Lohmann <nlohmann@informatik.hu-berlin.de>,
 *          Christian Gierds <gierds@informatik.hu-berlin.de>,
 *          last changes of: $Author$
 *
 * \since   2005/11/11
 *
 * \date    $Date$
 *
 * \version $Revision$
 */

#ifndef PNAPI_EXCEPTION_H
#define PNAPI_EXCEPTION_H

#include "config.h"

#include <string>
#include <iostream>

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
  /// output method
  virtual std::ostream & output(std::ostream &) const;
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
  /// output method
  virtual std::ostream & output(std::ostream &) const;
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
  /// output method
  virtual std::ostream & output(std::ostream &) const;
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
    UE_ARC_CONFLICT,
    UE_COMPOSE_ERROR,
    UE_MARKING_ERROR,
    UE_OMEGA_CAST_ERROR
  };
  
public: /* public constants */
  /// user error type
  const UE_Type type;
  
public: /* public methods */
  /// constructor
  UserCausedError(UE_Type, const std::string &);
  /// output method
  virtual std::ostream & output(std::ostream &) const;
};

template <class T>
class MarkingArithmeticError : public UserCausedError
{
private: /* private variables */
  /// faulty marking
  T faultyMarking_;
  
public: /* public methods */
  /// constructor
  MarkingArithmeticError(const T &);
  /// get faulty marking
  T getMarking() const;
};


/****************************************************************************
 *** Template Implementation
 ***************************************************************************/

/*!
 * \brief constructor
 */
template <class T> MarkingArithmeticError<T>::MarkingArithmeticError(const T & m) :
  UserCausedError(UE_MARKING_ERROR, "subtraction result negative"), faultyMarking_(m)
{
}

/*!
 * \brief get faulty marking
 */
template <class T> T MarkingArithmeticError<T>::getMarking() const
{
  return faultyMarking_;
}

} /* namespace exception */

} /* namespace pnapi */

#endif /* PNAPI_EXCEPTION_H */
