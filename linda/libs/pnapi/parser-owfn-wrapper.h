// -*- C++ -*-

/*!
 * \file    parser-owfn-wrapper.h
 * \brief   Parser Related Structures
 *
 * Since only one bison-generated parser can be included,
 * we have to wrap each parser in a separate file.
 */

#ifndef PNAPI_PARSER_OWFN_WRAPPER_H
#define PNAPI_PARSER_OWFN_WRAPPER_H

#include "config.h"

#ifndef __FLEX_LEXER_H
// do not include twice in lexer source files
#define yyFlexLexer OwfnFlexLexer
#include "FlexLexer.h"
#endif /* __FLEX_LEXER_H */

#include "parser.h"
#include "parser-owfn.h"

/*************************************************************************
 ***** OWFN Parser
 *************************************************************************/

namespace pnapi
{

namespace parser
{

/*!
 * \brief   OWFN Parser
 *
 * Instantiation of the parser framework for parsing OWFN files.
 */
namespace owfn
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
class Lexer : public AbstractLexer<Parser, BisonParser::semantic_type, OwfnFlexLexer>
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
 * \brief   Encapsulation of the flex/bison OWFN parser
 *
 * Connects to the flex/bison implementation for parsing.
 */
class Parser : public AbstractParser<yy::BisonParser, yy::Lexer, Parser>
{
  /* friend functions and classes */
  friend int yy::yylex(yy::BisonParser::semantic_type *, yy::BisonParser::location_type *, Parser &);
  friend class yy::BisonParser;
  friend void AbstractLexer<Parser, yy::BisonParser::semantic_type, OwfnFlexLexer>::LexerError(const char *);
  
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

  /// recent port
  Port * port_;
  /// recent interface label
  Label * label_;
  /// mapping from names to interface labels
  std::map<std::string, Label *> labels_;
  /// type of recent labels
  Label::Type labelType_;

  /// read capacity
  int capacity_;
  /// constrains
  std::map<Transition *, std::set<std::string> > constrains_;
  /// whether read marking is the initial marking or a final marking
  bool markInitial_;
  /// pointer to a final marking
  Marking * finalMarking_;

  /// preset/postset label for parse exception
  bool placeSetType_;
  /// precet/postset for fast checks
  std::set<std::string> placeSet_;

  /// wildcard ALL_PLACES_EMPTY given
  bool wildcardGiven_;
  
public: /* public methods */
  /// constructor
  Parser();
  
protected: /* protected methods */
  /// make this class concrete
  void implementMe() {};
};

} /* namespace owfn */

} /* namespace parser */

} /* namespace pnapi */

#endif /* PNAPI_PARSER_OWFN_WRAPPER_H */
