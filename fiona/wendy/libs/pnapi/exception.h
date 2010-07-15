#ifndef PNAPI_EXCEPTION_H
#define PNAPI_EXCEPTION_H

#include "config.h"

#include <cassert>
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
 * \brief
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


} /* namespace exception */

} /* namespace pnapi */

/*******************\
 * assertion makro *
\*******************/

#ifdef PNAPI_USE_C_ASSERTS
#define PNAPI_ASSERT(expr) assert(expr)
#else

#ifdef NDEBUG
#define PNAPI_ASSERT(expr) assert(expr)
#else
#define PNAPI_ASSERT(expr) if(!(expr)) { throw pnapi::exception::AssertionFailedError( __FILE__ , __LINE__ , #expr ); }
#endif /* NDEBUG */

#endif /* PNAPI_USE_C_ASSERTS */

#endif /* PNAPI_EXCEPTION_H */
