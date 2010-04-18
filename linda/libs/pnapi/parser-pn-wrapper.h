// -*- C++ -*-

/*!
 * \file    parser-pn-wrapper.h
 * \brief   Parser Related Structures
 *
 * Since only one bison-generated parser can be included,
 * we have to wrap each parser in a separate file.
 */

#ifndef PNAPI_PARSER_PN_WRAPPER_H
#define PNAPI_PARSER_PN_WRAPPER_H

#include "config.h"

#ifndef __FLEX_LEXER_H
// do not include twice in lexer source files
#define yyFlexLexer PnFlexLexer
#include "FlexLexer.h"
#endif /* __FLEX_LEXER_H */

#include "parser.h"
#include "parser-pn.h"
#include "petrinet.h"

/*************************************************************************
 ***** Petrify/Genet Parser
 *************************************************************************/

namespace pnapi
{

namespace parser
{

/*!
 * \brief   Encapsulation of the flex/bison PN parser
 *
 * Connects to the flex/bison implementation for parsing.
 * Parsing Genet/Petrify/etc. output.
 */
namespace pn
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
class Lexer : public AbstractLexer<Parser, BisonParser::semantic_type, PnFlexLexer>
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
 * \brief   Encapsulation of the flex/bison PN parser
 *
 * Connects to the flex/bison implementation for parsing.
 */
class Parser : public AbstractParser<yy::BisonParser, yy::Lexer, Parser>
{
  /* friend functions and classes */
  friend int yy::yylex(yy::BisonParser::semantic_type *, yy::BisonParser::location_type *, Parser &);
  friend class yy::BisonParser;
  friend void AbstractLexer<Parser, yy::BisonParser::semantic_type, PnFlexLexer>::LexerError(const char *);
  /// additional friend is needed for this special parser
  friend class pnapi::PetriNet;
    
private: /* private variables */
  /// set of transition names
  std::set<std::string> transitions_;
  /// set of place names
  std::set<std::string> places_;
  /// initial marking
  std::map<std::string, unsigned int> initialMarked_;
  /// interface labels
  std::set<std::string> interface_;
  /// parsed arcs
  std::map<std::string, std::map<std::string, unsigned int> > arcs_;
  /// temporary node mapping to parse arcs
  std::map<std::string, unsigned int> tempNodeMap_;
  /// whether parser is in a marking list
  bool in_marking_list;
  /// whether parser is in an arc list
  bool in_arc_list;
  
public: /* public methods */
  /// constructor
  Parser();
  
protected: /* protected methods */
  /// make this class concrete
  void implementMe() {};
};

} /* namespace pn */

} /* namespace parser */

} /* namespace pnapi */

#endif /* PNAPI_PARSER_PN_WRAPPER_H */
