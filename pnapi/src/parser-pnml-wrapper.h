// -*- C++ -*-

/*!
 * \file    parser-pnmlwrapper.h
 * \brief   Parser Related Structures
 *
 * Since only one bison-generated parser can be included,
 * we have to wrap each parser in a separate file.
 */

#ifndef PNAPI_PARSER_PNML_WRAPPER_H
#define PNAPI_PARSER_PNML_WRAPPER_H

#include "config.h"

#ifndef __FLEX_LEXER_H
// do not include twice in lexer source files
#define yyFlexLexer PnmlFlexLexer
#include <FlexLexer.h>
#endif /* __FLEX_LEXER_H */

#include "parser.h"
#include "parser-pnml.h"

/*************************************************************************
 ***** PNML Parser
 *************************************************************************/

namespace pnapi
{

namespace parser
{

/*!
 * \brief   PNML Parser
 *
 * Instantiation of the parser framework for parsing PNML files.
 */
namespace pnml
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
class Lexer : public AbstractLexer<Parser, BisonParser::semantic_type, PnmlFlexLexer>
{
  /* friend methods */
  friend int yylex(BisonParser::semantic_type *, BisonParser::location_type *, Parser &);
  
private: /* private static variables */
  /// To store start condition
  static int keep;

public: /* public static methods */
  /// get the name of xml tags
  static char * word(char *);
  
public: /* public methods */
  /// contructor
  Lexer(Parser &);
  /// actual lex-function
  int yylex();
};

} /* namespace yy */


/*!
 * \brief   Encapsulation of the flex/bison PNML parser
 *
 * Connects to the flex/bison implementation for parsing.
 */
class Parser : public AbstractParser<yy::BisonParser, yy::Lexer, Parser>
{
  /* friend functions and classes */
  friend int yy::yylex(yy::BisonParser::semantic_type *, yy::BisonParser::location_type *, Parser &);
  friend class yy::BisonParser;
  friend void AbstractLexer<Parser, yy::BisonParser::semantic_type, PnmlFlexLexer>::LexerError(const char *);
    
public: /* public types */
  /// type of read element
  enum elementTypes
  {
    T_PNML,
    T_NET,
    T_ARC,
    T_PLACE,
    T_TRANSITION,
    T_INSCRIPTION,
    T_INITIALMARKING,
    T_ANNOTATION,
    T_TEXT,
    T_MODULE,
    T_PORTS,
    T_PORT,
    T_INPUT,
    T_OUTPUT,
    T_SYNCHRONOUS,
    T_RECEIVE,
    T_SEND,
    T_SYNCHRONIZE,
    T_FINALMARKINGS,
    T_MARKING,
    T_PAGE,
    T_NAME,
    T_NONE
  };
  
private: /* private static constants */
  // we only care about these elements
  static const char * whitelist[];
  
private: /* private variables */
  /// a mapping to store the attributes, accessible by the parse depth
  std::map<unsigned int, std::map<std::string, std::string> > pnml_attributes;
  /// a mapping to store created Petri net nodes, accessible by the "id"
  std::map<std::string, Node *> pnml_id2node;
  /// a mapping to store created interface labels accessible by the "id"
  std::map<std::string, Label *> pnml_id2label;
  /// a mapping to store created Petri net names, accessible by the "id"
  std::map<std::string, std::string> pnml_id2name;
  // a marking we build during parsing
  pnapi::Marking * currentMarking;
  /// depth in XML structure
  unsigned int current_depth;
  /// the depth of the last XML element that should be parsed (i.e., is interesting)
  unsigned int last_interesting_depth;
  /// whether we are currently ignoring XML elements (and wait for the depth to be interesting again)
  bool ignoring;
  /// the part of the file we are currently parsing
  unsigned int file_part;
  /// stack of elements
  std::vector<unsigned int> elementStack;
  
public: /* public methods */
  /// constructor
  Parser();

protected: /* protected methods */
  /// make this class concrete
  void implementMe() {};
  
private: /* private methods */
  /// whether we care about a given element
  bool is_whitelisted(char *);
  /// opens an element
  void open_element(char *);
  /// removes whitespace around given string
  std::string sanitize(char *);
  /// close an element
  void close_element();
  /// store attributes
  void store_attributes(char *, char *);
  /// store data
  void store_data(char *);
  /// throw an error
  void error(const std::string &);
};

} /* namespace pnml */

} /* namespace parser */

} /* namespace pnapi */

#endif /* PNAPI_PARSER_PNML_WRAPPER_H */
