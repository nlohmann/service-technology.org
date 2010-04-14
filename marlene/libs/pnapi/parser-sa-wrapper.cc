/*!
 * \file parser-sa-wrapper.cc
 */

#include "config.h"

#include "parser-sa-wrapper.h"

#include "automaton.h"
#include "marking.h"
#include "myio.h"
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

namespace pnapi { namespace parser { namespace sa
{

/*!
 * \brief parses stream contents with the associated parser
 */
Automaton parse(std::istream & is)
{
  Parser myParser;
  return myParser.parse(is);
}

namespace yy
{

/*!
 * \brief Wrapper funktion to link Bison parser with Flex lexer
 */
int yylex(BisonParser::semantic_type * yylval, BisonParser::location_type *, Parser & p)
{
  p.lexer_.yylval = yylval;
  return p.lexer_.yylex();
}

/*!
 * \brief constructor
 */
Lexer::Lexer(Parser & p) :
  AbstractLexer<Parser, BisonParser::semantic_type, SaFlexLexer>(p)
{
}

/*!
 * \brief implement parser error handling
 */
void BisonParser::error(const location_type & loc, const std::string & msg)
{
  parser::error(*(parser_.is_), parser_.lexer_.lineno(), parser_.lexer_.YYText(), msg);
}

} /* namespace yy */

/*!
 * \brief constructor
 */
Parser::Parser() :
  AbstractParser<yy::BisonParser, yy::Lexer, Parser>(),
  state_(NULL), final_(false), initial_(false)
{
}

/*!
 * \brief parses stream contents with the associated parser
 */
const Automaton & Parser::parse(std::istream & is)
{
  AbstractParser<yy::BisonParser, yy::Lexer, Parser>::parse(is);

  return automaton_;
}

} } } /* namespace pnapi::parser::sa */
