// -*- C++ -*-

/*!
 * \file    parser-pnt-wrapper.h
 * \brief   Parser Related Structures
 *
 * Since only one bison-generated parser can be included,
 * we have to wrap each parser in a separate file.
 */

#ifndef PNAPI_PARSER_PNT_WRAPPER_H
#define PNAPI_PARSER_PNT_WRAPPER_H

#include "config.h"

#ifndef __FLEX_LEXER_H
// do not include twice in lexer source files
#define yyFlexLexer PntFlexLexer
#include "FlexLexer.h"
#endif /* __FLEX_LEXER_H */

#include "parser.h"
#include "parser-pnt.h"
#include <deque>

/*************************************************************************
 ***** INA Parser (*.pnt)
 *************************************************************************/

namespace pnapi
{

namespace parser
{

/*!
 * \brief   INA Parser (*.pnt)
 *
 * Instantiation of the parser framework for parsing INA files.
 */
namespace pnt
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
class Lexer : public AbstractLexer<Parser, BisonParser::semantic_type, PntFlexLexer>
{
  /* friend methods */
  friend int yylex(BisonParser::semantic_type *, BisonParser::location_type *, Parser &);
  
private: /* private variables */
  /// counts occuring "@"
  int atCounter_;

public: /* public methods */
  /// contructor
  Lexer(Parser &);
  /// actual lex-function
  int yylex();
};

} /* namespace yy */


/*!
 * \brief   Encapsulation of the flex/bison INA parser
 *
 * Connects to the flex/bison implementation for parsing INA files (*.pnt).
 */
class Parser : public AbstractParser<yy::BisonParser, yy::Lexer, Parser>
{
  /* friend functions and classes */
  friend int yy::yylex(yy::BisonParser::semantic_type *, yy::BisonParser::location_type *, Parser &);
  friend class yy::BisonParser;
  friend void AbstractLexer<Parser, yy::BisonParser::semantic_type, PntFlexLexer>::LexerError(const char *);
    
private: /* private variables */
  /// mapping from places to their marking
  std::map<int, int> marking_;
  /// mapping from places to their preset
  std::deque<int *> preset_;
  /// mapping from places to their postset
  std::deque<int *> postset_;
  /// mapping from places to their addresses
  std::map<int, Place *> places_;
  /// mapping from transitions to their addresses
  std::map<int, Transition *> transitions_;
  /// current place
  int currentPlace_;
  /// current arc list
  std::deque<int *> * currentArcList_;
  /// temporary int array
  int * arc_;
  
public: /* public methods */
  /// constructor
  Parser();
  
protected: /* protected methods */
  /// make this class concrete
  void implementMe() {}

private: /* private methods */
  /// create Arcs
  void completeNet();
  /// missing node error
  void missingNode(int, bool);
};

} /* namespace pnt */

} /* namespace parser */

} /* namespace pnapi */

#endif /* PNAPI_PARSER_PNT_WRAPPER_H */
