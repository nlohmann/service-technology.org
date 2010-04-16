// -*- C++ -*-

/*!
 * \file    parser-woflan-wrapper.h
 * \brief   Parser Related Structures
 *
 * Since only one bison-generated parser can be included,
 * we have to wrap each parser in a separate file.
 */

#ifndef PNAPI_PARSER_WOFLAN_WRAPPER_H
#define PNAPI_PARSER_WOFLAN_WRAPPER_H

#include "config.h"

#ifndef __FLEX_LEXER_H
// do not include twice in lexer source files
#define yyFlexLexer WoflanFlexLexer
#include "FlexLexer.h"
#endif /* __FLEX_LEXER_H */

#include "parser.h"
#include "parser-woflan.h"

/*************************************************************************
 ***** Woflan Parser
 *************************************************************************/

namespace pnapi
{

namespace parser
{

/*!
 * \brief   Woflan Parser
 *
 * Instantiation of the parser framework for parsing Woflan files.
 */
namespace woflan
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
class Lexer : public AbstractLexer<Parser, BisonParser::semantic_type, WoflanFlexLexer>
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
 * \brief   Encapsulation of the flex/bison Woflan parser
 *
 * Connects to the flex/bison implementation for parsing.
 */
class Parser : public AbstractParser<yy::BisonParser, yy::Lexer, Parser>
{
  /* friend functions and classes */
  friend int yy::yylex(yy::BisonParser::semantic_type *, yy::BisonParser::location_type *, Parser &);
  friend class yy::BisonParser;
  friend void AbstractLexer<Parser, yy::BisonParser::semantic_type, WoflanFlexLexer>::LexerError(const char *);
    
private: /* private variables */
  /// mapping of names to places
  std::map<std::string, Place *> places_;
  /// recently read transition
  Transition * transition_;
  /// all purpose place pointer
  Place * place_;
  /// target of an arc
  Node * * target_;
  /// source of an arc
  Node * * source_;
  /// converts NUMBER and IDENT in string
  std::stringstream nodeName_;
  /// read capacity
  int capacity_;
  
  /// if a label is needed
  bool needLabel;
  /// name of a transition
  std::string transName;
  
public: /* public methods */
  /// constructor
  Parser();
  
protected: /* protected methods */
  /// make this class concrete
  void implementMe() {};
};

} /* namespace woflan */

} /* namespace parser */

} /* namespace pnapi */

#endif /* PNAPI_PARSER_WOFLAN_WRAPPER_H */
