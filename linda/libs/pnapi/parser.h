// -*- C++ -*-

/*!
 * \file    parser.h
 * \brief   Parser Related Structures
 *
 * Includes a C++ parser framework (pnapi::parser) and parsers for various
 * Petri net input formats (e.g. OWFN (pnapi::parser::owfn)).
 */

#ifndef PNAPI_PARSER_H
#define PNAPI_PARSER_H

#include "config.h"

#include "petrinet.h"
#include "port.h"
#include "state.h"

#include <sstream>
#include <vector>

namespace pnapi
{

/*!
 * \brief   Generic Parser Framework
 *
 * Provides C++ classes encapsulating flex/bison parsers. Documentation is
 * available under <https://ikaria/trac/mesu/wiki/ParserFramework>.
 * 
 * \todo check documentation availability; may move to API documentation
 */
namespace parser
{

/// called by generated lexers/parsers
void error(std::istream &, unsigned int, const std::string &, const std::string &);

/*!
 * \brief parent parser class with common structures
 */
template <class P, class L, class C>
class AbstractParser
{
protected: /* protected variables */
  /// the actual parser
  P parser_;
  /// the lexer
  L lexer_;
  /// input stream
  std::istream * is_; 
  /// generated petrinet
  PetriNet net_;
  
public: /* public methods */
  /// constructor
  AbstractParser();
  /// "assertion"
  void check(bool, const std::string &);
  /// parses stream contents with the associated parser
  const PetriNet & parse(std::istream &);
  
protected: /* protected methods */
  /// make this class abstract
  virtual void implementMe() = 0;
};

/*!
 * \brief parent lexer class with common structures
 */
template <class P, class Bst, class F>
class AbstractLexer : public F
{ 
protected: /* protected variables */
  /// the overlying parser
  P & parser_;
  /// parser's semantic value
  Bst * yylval;
  
public: /* public methods */
  /// contructor
  AbstractLexer(P &);
  /// actual lex-function
  virtual int yylex() = 0;
  /// overwrite YY_FATAL_ERROR behavior
  void LexerError(const char *);
};

namespace lola
{
/// parses stream contents with the associated parser
PetriNet parse(std::istream &);
} /* namespace lola */

namespace owfn
{
/// parses stream contents with the associated parser
PetriNet parse(std::istream &);
} /* namespace owfn */

namespace pnml
{
/// parses stream contents with the associated parser
PetriNet parse(std::istream &);
} /* namespace pnml */

namespace sa
{
/// parses stream contents with the associated parser
Automaton parse(std::istream &);
} /* namespace sa */

namespace woflan
{
/// parses stream contents with the associated parser
PetriNet parse(std::istream &);
} /* namespace woflan */

} /* namespace parser */

} /* namespace pnapi */


/*********************************\
 * template class implementation *
\*********************************/

namespace pnapi
{

namespace parser
{

/*!
 * \brief constructor
 */
template <class P, class L, class C>
AbstractParser<P, L, C>::AbstractParser() :
  parser_(*static_cast<C *>(this)), lexer_(*static_cast<C *>(this)), is_(NULL)
{
}

/*!
 * \brief "assertion"
 */
template <class P, class L, class C>
void AbstractParser<P, L, C>::check(bool condition, const std::string & msg)
{
  if (!condition)
  {
    throw exception::InputError(exception::InputError::SEMANTIC_ERROR, 
                                io::util::MetaData::data(*is_)[io::INPUTFILE],
                                lexer_.lineno(), lexer_.YYText(), msg);
  }
}

/*!
 * \brief parses stream contents with the associated parser
 */
template <class P, class L, class C>
const PetriNet & AbstractParser<P, L, C>::parse(std::istream & is)
{
  // assign lexer input stream
  is_ = &is;
  lexer_.switch_streams(is_);

  // call the parser
  parser_.parse();

  return net_;
}


/*!
 * \brief constructor
 */
template <class P, class Bst, class F>
AbstractLexer<P, Bst, F>::AbstractLexer(P & p) :
  F(), yylval(NULL), parser_(p)
{
}

/*!
 * \brief overwrite YY_FATAL_ERROR behavior
 */
template <class P, class Bst, class F>
void AbstractLexer<P, Bst, F>::LexerError(const char * msg)
{
  parser::error(*(parser_.is_), F::lineno(), F::YYText(), msg);
}


} /* namespace parser */

} /* namespace pnapi */

#endif /* PNAPI_PARSER_H */
