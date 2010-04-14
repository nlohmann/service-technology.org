/*!
 * \file parser.cc
 */

#include "config.h"

#include "automaton.h"
#include "marking.h"
#include "myio.h"
#include "parser.h"
#include "petrinet.h"

#include <cstring>
#include <sstream>

using std::cout;
using std::cerr;
using std::endl;
using std::ifstream;
using std::istream;
using std::stringstream;
using std::string;
using std::map;
using std::set;
using std::pair;
using std::vector;

using namespace pnapi::formula;
using pnapi::io::operator>>;

namespace pnapi
{

namespace parser
{

/*!
 * \brief called by generated lexers/parsers
 */
void error(std::istream & is, unsigned int line, const std::string & token, const std::string & msg)
{
  throw exception::InputError(exception::InputError::SYNTAX_ERROR,
                              io::util::MetaData::data(is)[io::INPUTFILE],
                              line, token, msg);
}

} /* namespace parser */

} /* namespace pnapi */
