// -*- C++ -*-

/*!
 * \file    parser-sa-wrapper.h
 * \brief   Parser Related Structures
 *
 * Since only one bison-generated parser can be included,
 * we have to wrap each parser in a separate file.
 */

#ifndef PNAPI_PARSER_SA_WRAPPER_H
#define PNAPI_PARSER_SA_WRAPPER_H

#include "config.h"

#ifndef __FLEX_LEXER_H
// do not include twice in lexer source files
#define yyFlexLexer SaFlexLexer
#include "FlexLexer.h"
#endif /* __FLEX_LEXER_H */

#include "parser.h"
#include "parser-sa.h"
#include "automaton.h"

/*************************************************************************
 ***** SA Parser
 *************************************************************************/

namespace pnapi
{

namespace parser
{

namespace sa
{

/// forward declaration
class Parser;

namespace yy
{

/// wrapper funktion to link Bison parser with Flex lexer
int yylex(BisonParser::semantic_type *, BisonParser::location_type *, Parser &);

/*!
 * \brief Lexer class
 */
class Lexer : public AbstractLexer<Parser, BisonParser::semantic_type, SaFlexLexer>
{
  /* friend methods */
  friend int yylex(BisonParser::semantic_type *, BisonParser::location_type *, Parser &);
  
public: /* public methods */
  /// contructor
  Lexer(Parser &);
  /// actual lex-function
  int yylex();
};

} /* namespace yy */


/*!
 * \brief   Encapsulation of the flex/bison SA parser
 *
 * Connects to the flex/bison implementation for parsing.
 */
class Parser : public AbstractParser<yy::BisonParser, yy::Lexer, Parser>
{
  /* friend functions and classes */
  friend int yy::yylex(yy::BisonParser::semantic_type *, yy::BisonParser::location_type *, Parser &);
  friend class yy::BisonParser;
  friend void AbstractLexer<Parser, yy::BisonParser::semantic_type, SaFlexLexer>::LexerError(const char *);
    
private: /* private variables */
  /// generated automaton
  Automaton automaton_;
  /// temporary list of idents
  std::vector<std::string> identlist;
  /// input labels
  std::set<std::string> input_;
  /// output labels
  std::set<std::string> output_;
  /// synchronous labels
  std::set<std::string> synchronous_;

  /// temporary state pointer
  State * state_;
  /// states by name
  std::map<int, State *> states_;
  /// whether recent state is final
  bool final_;
  /// whether recent state is initial
  bool initial_;
  /// successors of recent state
  std::vector<unsigned int> succState_;
  /// labels of arcs to the successors of recent state
  std::vector<std::string> succLabel_;
  /// types of arcs to the successors of recent state
  std::vector<Edge::Type> succType_;
  /// label of recent edge
  std::string edgeLabel_;
  /// type of recent edge
  Edge::Type edgeType_;
  
public: /* public methods */
  /// constructor
  Parser();
  /// parses stream contents with the associated parser
  const Automaton & parse(std::istream &);
  
protected: /* protected methods */
  /// make this class concrete
  void implementMe() {};
};

} /* namespace sa */

} /* namespace parser */

} /* namespace pnapi */

#endif /* PNAPI_PARSER_SA_WRAPPER_H */
