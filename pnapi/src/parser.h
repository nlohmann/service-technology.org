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

#include <vector>
#include <stack>
#include <string>
#include <istream>

#include "myio.h"
#include "automaton.h"
#include "component.h"
#include "petrinet.h"
#include "marking.h"

namespace pnapi
{

// forward declarations
class PetriNet;
class Place;
class Marking;

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

/// input stream for lexers
extern std::istream * stream;
/// last read lexer token
extern char * token;
/// line number in input
extern int line;
/// called by generated lexers/parsers
void error(const std::string &);


/*************************************************************************
 ***** OWFN Parser
 *************************************************************************/

/*!
 * \brief   OWFN Parser
 *
 * Instantiation of the parser framework for parsing OWFN files.
 */
namespace owfn
{

/// flex generated lexer function
int lex();
/// flex generated lexer cleanup function
int lex_destroy();
/// bison generated parser function
int parse();
/// "assertion"
void check(bool, const std::string &);

/******************************************\
 *  "global" variables for flex and bison *
\******************************************/

/// generated petrinet
extern PetriNet pnapi_owfn_yynet;

/// mapping of names to places
extern std::map<std::string, Place *> places_;
/// recently read transition
extern Transition * transition_;
/// all purpose place pointer
extern Place * place_;
/// target of an arc
extern Node * * target_;
/// source of an arc
extern Node * * source_;
/// converts NUMBER and IDENT in string
extern std::stringstream nodeName_;

/// recent port
extern Port * port_;
/// recent interface label
extern Label * label_;
/// mapping from names to interface labels
extern std::map<std::string, Label *> labels_;
/// type of recent labels
extern Label::Type labelType_;

/// read capacity
extern int capacity_;
/// constrains
extern std::map<Transition *, std::set<std::string> > constrains_;
/// whether read marking is the initial marking or a final marking
extern bool markInitial_;
/// pointer to a final marking
extern Marking * finalMarking_;

/// preset/postset label for parse exception
extern bool placeSetType_;
/// precet/postset for fast checks
extern std::set<std::string> placeSet_;

/// wildcard ALL_PLACES_EMPTY given
extern bool wildcardGiven_;


/*!
 * \brief   Encapsulation of the flex/bison OWFN parser
 *
 * Connects to the flex/bison implementation for parsing.
 */
class Parser
{
public: /* public methods */
  /// constructor
  Parser();
  /// destructor
  ~Parser();
  /// cleans global net
  void clean();
  /// parses stream contents with the associated parser
  const PetriNet & parse(std::istream &);
};

} /* namespace owfn */



/*************************************************************************
 ***** PNML Parser
 *************************************************************************/


/*!
 * \brief   PNML Parser
 *
 * Instantiation of the parser framework for parsing PNML files.
 */
namespace pnml
{

/// flex generated lexer function
int lex();
/// flex generated lexer cleanup function
int lex_destroy();
/// bison generated parser function
int parse();
/// "assertion"
void check(bool, const std::string &);


/*!
 * \brief type of read element
 */
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


/// whether we care about a given element
bool is_whitelisted(char *);
/// opens an element
void open_element(char *);
/// ???
std::string sanitize(char *);
/// close an element
void close_element();
/// store attributes
void store_attributes(char *, char *);
/// store data
void store_data(char *);


/******************************************\
 *  "global" variables for flex and bison *
\******************************************/

/// generated petrinet
extern PetriNet pnapi_pnml_yynet;

/// a mapping to store the attributes, accessible by the parse depth
extern std::map<unsigned int, std::map<std::string, std::string> > pnml_attributes;
/// a mapping to store created Petri net nodes, accessible by the "id"
extern std::map<std::string, Node *> pnml_id2node;
/// a mapping to store created interface labels accessible by the "id"
extern std::map<std::string, Label *> pnml_id2label;
/// a mapping to store created Petri net names, accessible by the "id"
extern std::map<std::string, std::string> pnml_id2name;
// we only care about these elements
extern const char * whitelist[];
// a marking we build during parsing
extern pnapi::Marking * currentMarking;
/// depth in XML structure
extern unsigned int current_depth;
/// ???
extern unsigned int last_interesting_depth;
/// whether to ignore something ???
extern bool ignoring;
/// ???
extern unsigned int file_part;
/// stack of elements
extern std::vector<unsigned int> elementStack;


/*!
 * \brief   Encapsulation of the flex/bison PNML parser
 *
 * Connects to the flex/bison implementation for parsing.
 */
class Parser
{
public: /* public methods */
  /// constructor
  Parser();
  /// destructor
  ~Parser();
  /// cleans global net
  void clean();
  /// parses stream contents with the associated parser
  const PetriNet & parse(std::istream &);
};

} /* namespace pnml */


/*************************************************************************
 ***** LOLA Parser
 *************************************************************************/

/*!
 * \brief   LOLA Parser
 *
 * Instantiation of the parser framework for parsing LOLA files.
 */
namespace lola
{

/// flex generated lexer function
int lex();
/// flex generated lexer cleanup function
int lex_destroy();
/// bison generated parser function
int parse();
/// "assertion"
void check(bool, const std::string &);

/******************************************\
 *  "global" variables for flex and bison *
\******************************************/

/// generated petrinet
extern PetriNet pnapi_lola_yynet;

/// mapping of names to places
extern std::map<std::string, Place *> places_;
/// recently read transition
extern Transition * transition_;
/// all purpose place pointer
extern Place * place_;
/// target of an arc
extern Node * * target_;
/// source of an arc
extern Node * * source_;
/// converts NUMBER and IDENT in string
extern std::stringstream nodeName_;
/// read capacity
extern int capacity_;


/*!
 * \brief   Encapsulation of the flex/bison LOLA parser
 *
 * Connects to the flex/bison implementation for parsing.
 */
class Parser
{
public: /* public methods */
  /// constructor
  Parser();
  /// destructor
  ~Parser();
  /// cleans global net
  void clean();
  /// parses stream contents with the associated parser
  const PetriNet & parse(std::istream &);
};

} /* namespace lola */


/*************************************************************************
 ***** SA Parser
 *************************************************************************/

namespace sa
{

// flex lexer
int lex();
/// flex generated lexer cleanup function
int lex_destroy();
// bison parser
int parse();

/// generated automaton
extern Automaton pnapi_sa_yyautomaton;
/// temporary list of idents
extern std::vector<std::string> identlist;
/// input labels
extern std::set<std::string> input_;
/// output labels
extern std::set<std::string> output_;
/// synchronous labels
extern std::set<std::string> synchronous_;

/// temporary state pointer
extern State * state_;
/// states by name
extern std::map<int, State *> states_;
/// whether recent state is final
extern bool final_;
/// whether recent state is initial
extern bool initial_;
/// successors of recent state
extern std::vector<unsigned int> succState_;
/// labels of arcs to the successors of recent state
extern std::vector<std::string> succLabel_;
/// types of arcs to the successors of recent state
extern std::vector<Edge::Type> succType_;
/// label of recent edge
extern std::string edgeLabel_;
/// type of recent edge
extern Edge::Type edgeType_;


/*!
 * \brief   Encapsulation of the flex/bison SA parser
 *
 * Connects to the flex/bison implementation for parsing.
 */
class Parser
{
public: /* public methods */
  /// constructor
  Parser();
  /// destructor
  ~Parser();
  /// parses stream contents with the associated parser
  const Automaton & parse(std::istream &);
  /// cleans global variables
  void clean();
};

} /* namespace sa */


/*************************************************************************
 ***** Petrify Parser
 *************************************************************************/

/*!
 * \brief   Encapsulation of the flex/bison PN parser
 *
 * Connects to the flex/bison implementation for parsing.
 * Parsing Genet/Petrify/etc. output.
 */
namespace pn
{

// flex lexer
int lex();
/// flex generated lexer cleanup function
int lex_destroy();
// bison parser
int parse();

/// set of transition names
extern std::set<std::string> transitions_;
/// set of place names
extern std::set<std::string> places_;
/// initial marking
extern std::map<std::string, unsigned int> initialMarked_;
/// interface labels
extern std::set<std::string> interface_;
/// parsed arcs
extern std::map<std::string, std::map<std::string, unsigned int> > arcs_;
/// temporary node mapping to parse arcs
extern std::map<std::string, unsigned int> tempNodeMap_;
/// whether parser is in a marking list
extern bool in_marking_list;
/// whether parser is in an arc list
extern bool in_arc_list;

/*!
 * \brief   Encapsulation of the flex/bison PN parser
 *
 * Connects to the flex/bison implementation for parsing.
 */
class Parser
{
public: /* public methods */
  /// constructor
  Parser();
  /// destructor
  ~Parser();
  /// parses stream contents with the associated parser
  void parse(std::istream &);
  /// cleans global variables
  void clean();
};

} /* namespace pn */

} /* namespace parser */

} /* namespace pnapi */

#endif /* PNAPI_PARSER_H */
