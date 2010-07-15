// -*- C++ -*-

/*!
 * \file    parser-FORMAT-wrapper.h
 * \brief   Parser Related Structures
 *
 * Since only one bison-generated parser can be included,
 * we have to wrap each parser in a separate file.
 */

#ifndef PNAPI_PARSER_FORMAT_WRAPPER_H
#define PNAPI_PARSER_FORMAT_WRAPPER_H

#include "config.h"

#ifndef __FLEX_LEXER_H
// do not include twice in lexer source files
#define yyFlexLexer FORMATFlexLexer
#include "FlexLexer.h"
#endif /* __FLEX_LEXER_H */

#include "parser.h"
#include "parser-FORMAT.h"

/*************************************************************************
 ***** FORMAT Parser
 *************************************************************************/

namespace pnapi
{

namespace parser
{

/*!
 * \brief   FORMAT Parser
 *
 * Instantiation of the parser framework for parsing FORMAT files.
 */
namespace FORMAT
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
class Lexer : public AbstractLexer<Parser, BisonParser::semantic_type, FORMATFlexLexer>
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
 * \brief   Encapsulation of the flex/bison LOLA parser
 *
 * Connects to the flex/bison implementation for parsing.
 */
class Parser : public AbstractParser<yy::BisonParser, yy::Lexer, Parser>
{ 
  /* friend functions and classes */
  friend int yy::yylex(yy::BisonParser::semantic_type *, yy::BisonParser::location_type *, Parser &);
  friend class yy::BisonParser;
  friend void AbstractLexer<Parser, yy::BisonParser::semantic_type, FORMATFlexLexer>::LexerError(const char *);
  
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
  
public: /* public methods */
  /// constructor
  Parser();
  
protected: /* protected methods */
  /// make this class concrete
  void implementMe() {};
};

} /* namespace FORMAT */

} /* namespace parser */

} /* namespace pnapi */

#endif /* PNAPI_PARSER_FORMAT_WRAPPER_H */

