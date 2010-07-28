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
 * \date    $Date: 2010-07-23 14:38:42 +0200 (Fri, 23 Jul 2010) $
 *
 * \version $Revision: 5949 $
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

/*!
 * \brief output method
 */
std::ostream & UserCausedError::output(std::ostream & os) const
{
  switch(type)
  {
  case UE_NODE_NAME_CONFLICT: os << "node name conflict: "; break;
  case UE_LABEL_NAME_CONFLICT: os << "label name conflict: "; break;
  case UE_ARC_CONFLICT: os << "arc conflict: "; break;
  case UE_COMPOSE_ERROR: os << "compose error: "; break;
  default: /* do nothing */;
  }
  
  return (os << message);
}

} /* namespace exception */

} /* namespace pnapi */
