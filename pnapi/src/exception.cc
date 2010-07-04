/*!
 * \file    exception.cc
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

#include "exception.h"

#include <fstream>

using std::string;
using std::ifstream;
using std::endl;

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
 * \brief output method
 */
std::ostream & Error::output(std::ostream & os) const
{
  return (os << message);
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

/*!
 * \brief output method
 */
std::ostream & InputError::output(std::ostream & os) const
{
  os << filename;
  if (line > 0)
  {
    os << ":" << line;
  }
  os << ": error";
  if (!token.empty())
    switch(type)
    {
    case exception::InputError::SYNTAX_ERROR:
      os << " near '" << token << "'";
      break;
    case exception::InputError::SEMANTIC_ERROR:
      os << ": '" << token << "'";
      break;
    default: /* do nothing */ ;
    }
  os << ": " << message;

  ifstream ifs(filename.c_str());
  if(ifs.good())
  {
    string line_, arrows;
    for(int i = 0; i < line; ++i)
    {
      getline(ifs, line_);
    }
    
    size_t firstpos(line_.find(token));
    
    for(unsigned int i = 0; i < firstpos; ++i)
    {
      arrows += " ";
    }
    for(unsigned int i = 0; i < token.size(); ++i)
    {
      arrows += "^";
    }
    
    os << endl << endl << line_ << endl << arrows << endl << endl;  
  }
  ifs.close();
  
  return os;
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
 * \brief output method
 */
std::ostream & AssertionFailedError::output(std::ostream & os) const
{
  return (os << file << ":" << line << ": assertion failed: " << message);
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
