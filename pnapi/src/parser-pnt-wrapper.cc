/*!
 * \file parser-pnt-wrapper.cc
 */

#include "config.h"

#include "parser-pnt-wrapper.h"

#include "automaton.h"
#include "marking.h"
#include "myio.h"
#include "petrinet.h"

#include <cstring>
#include <cstdlib>
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

namespace pnapi { namespace parser { namespace pnt
{

/*!
 * \brief parses stream contents with the associated parser
 */
PetriNet parse(std::istream & is)
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
  AbstractLexer<Parser, BisonParser::semantic_type, PntFlexLexer>(p),
  atCounter_(0)
{
}

/*!
 * \brief implement parser error handling
 */
void BisonParser::error(const location_type &, const std::string & msg)
{
  parser::error(*(parser_.is_), parser_.lexer_.lineno(), parser_.lexer_.YYText(), msg);
}

} /* namespace yy */

/*!
 * \brief Constructor
 */
Parser::Parser() :
  AbstractParser<yy::BisonParser, yy::Lexer, Parser>(),
  currentPlace_(0), currentArcList_(NULL), arc_(NULL)
{
}

/*!
 * \brief create Arcs
 */
void Parser::completeNet()
{
  while(preset_.size() > 0) // while not empty
  {
    arc_ = preset_.back(); // get next arc
    preset_.pop_back(); // remove arc from presets
    if (places_.find(arc_[0]) == places_.end()) // if place does not exist
    {
      // throw error
      missingNode(arc_[0], true);
    }
    if (transitions_.find(arc_[1]) == transitions_.end()) // if transition does not exist
    {
      // throw error
      missingNode(arc_[1], false);
    }
    net_.createArc(*transitions_[arc_[1]], *places_[arc_[0]], arc_[2]); // create arc
    free(arc_); // free memory
  }

  while(postset_.size() > 0) // while not empty
  {
    arc_ = postset_.back(); // get next arc
    postset_.pop_back(); // remove arc from postsets
    if (places_.find(arc_[0]) == places_.end()) // if place does not exist
    {
      // throw error
      missingNode(arc_[0], true);
    }
    if (transitions_.find(arc_[1]) == transitions_.end()) // if transition does not exist
    {
      // throw error
      missingNode(arc_[1], false);
    }
    net_.createArc(*places_[arc_[0]], *transitions_[arc_[1]], arc_[2]); // create arc
    free(arc_); // free memory
  }
}

/*!
 * \brief missing node error
 */
void Parser::missingNode(int id, bool isPlace)
{
  stringstream ss("missing ");
  if (isPlace)
  {
    ss << "place";
  }
  else
  {
    ss << "transition";
  }
  ss << " with ID '" << id << "'";
  check(false, ss.str());
}


} } } /* namespace pnapi::parser::pnt */
