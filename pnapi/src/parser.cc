/*!
 * \file parser.cc
 */

#include "config.h"


#include <cstring>
#include <fstream>
#include <sstream>
#include <iostream>

#include "parser.h"
#include "myio.h"
#include "formula.h"
#include "state.h"

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

namespace pnapi
{

namespace parser
{

/// input stream for lexers
std::istream * stream;
/// last read lexer token
char * token;
/// line number in input
int line;

/*!
 * \brief called by generated lexers/parsers
 */
void error(const std::string & msg)
{
  throw io::InputError(io::InputError::SYNTAX_ERROR,
                       io::util::MetaData::data(*parser::stream)[io::INPUTFILE],
                       parser::line, parser::token, msg);
}


namespace owfn
{
/******************************************\
 *  "global" variables for flex and bison *
\******************************************/

/// generated petrinet
PetriNet pnapi_owfn_yynet;

/// mapping of names to places
std::map<std::string, Place *> places_;
/// recently read transition
Transition * transition_ = NULL;
/// all purpose place pointer
Place * place_ = NULL;
/// target of an arc
Node * * target_ = NULL;
/// source of an arc
Node * * source_ = NULL;
/// converts NUMBER and IDENT in string
std::stringstream nodeName_;

/// recent port
Port * port_ = NULL;
/// recent interface label
Label * label_ = NULL;
/// mapping from names to interface labels
std::map<std::string, Label *> labels_;
/// type of recent labels
Label::Type labelType_;

/// read capacity
int capacity_ = 0;
/// constrains
std::map<Transition *, std::set<std::string> > constrains_;
/// whether read marking is the initial marking or a final marking
bool markInitial_;
/// pointer to a final marking
Marking * finalMarking_ = NULL;

/// whether pre- or postset is read
bool placeSetType_;
/// precet/postset for fast checks
set<std::string> placeSet_;

/// wildcard ALL[_OTHER]_PLACES_EMPTY given
bool wildcardGiven_ = false;

/*!
 * \brief "assertion"
 */
void check(bool condition, const std::string & msg)
{
  if (!condition)
  {
    throw io::InputError(io::InputError::SEMANTIC_ERROR, 
                         io::util::MetaData::data(*parser::stream)[io::INPUTFILE],
                         parser::line, parser::token, msg);
  }
}

/*!
 * \brief Constructor
 */
Parser::Parser()
{
}

/*!
 * \brief Destructor
 * 
 * \note  Just used to call clean() automaticly
 */
Parser::~Parser()
{
  clean();
}

/*!
 * \brief Overwrites read net with empty net to free memory.
 */
void Parser::clean()
{
  pnapi_owfn_yynet = PetriNet();
}

/*!
 * \brief parses stream contents with the associated parser
 */
const PetriNet & Parser::parse(std::istream & is)
{
  // assign lexer input stream
  stream = &is;

  // reset line counter
  line = 1;

  // clear net
  pnapi_owfn_yynet = PetriNet();

  // initialize port with global port
  port_ = pnapi_owfn_yynet.getInterface().getPort();
  
  // call the parser
  owfn::parse();

  // clean up lexer
  owfn::lex_destroy();
  
  // clean up global variables
  places_.clear();
  labels_.clear();
  transition_ = NULL;
  place_ = NULL;
  source_ = target_ = NULL;
  port_ = NULL;
  label_ = NULL;
  constrains_.clear();
  placeSet_.clear();

  return pnapi_owfn_yynet;
}

} /* namespace owfn */

namespace pnml
{
/******************************************\
 *  "global" variables for flex and bison *
\******************************************/

/// generated petrinet
PetriNet pnapi_pnml_yynet;

/// a mapping to store the attributes, accessible by the parse depth
std::map<unsigned int, std::map<std::string, std::string> > pnml_attributes;
/// a mapping to store created Petri net nodes, accessible by the "id"
std::map<std::string, Node *> pnml_id2node;
/// a mapping to store created interface labels accessible by the "id"
std::map<std::string, Label *> pnml_id2label;
/// a mapping to store created Petri net names, accessible by the "id"
std::map<std::string, std::string> pnml_id2name;
/// a marking we build during parsing
Marking * currentMarking = NULL;
/// depth in XML structure
unsigned int current_depth = 0;
/// the depth of the last XML element that should be parsed (i.e., is interesting)
unsigned int last_interesting_depth = 0;
/// whether we are currently ignoring XML elements (and wait for the depth to be interesting again)
bool ignoring = false;
/// the part of the file we are currently parsing
unsigned int file_part = T_NONE;
/// stack of elements
std::vector<unsigned int> elementStack;

// we only care about these elements
const char * whitelist[] = { "pnml", "net", "arc", "place", "transition", "inscription",
                             "initialMarking", "annotation", "text", "module", "ports",
                             "port", "input", "output", "synchronous", "receive", "send",
                             "synchronize", "finalmarkings", "marking", "page", "name", NULL };



/*!
 * \brief "assertion"
 */
void check(bool condition, const std::string & msg)
{
  if (!condition)
  {
    throw io::InputError(io::InputError::SEMANTIC_ERROR, 
                         io::util::MetaData::data(*parser::stream)[io::INPUTFILE],
                         parser::line, parser::token, msg);
  }
}


/*!
 * \brief whether we care about a given element
 */
bool is_whitelisted(char * s)
{
  unsigned int i = 0;
  while(whitelist[i])
  {
    if(!strcmp(s, whitelist[i++]))
    {
      // remember which part of the net we are parsing
      switch(i-1)
      {
      case T_PORTS:
      case T_NET:
      case T_FINALMARKINGS:
        file_part = i - 1;
        break;
      case T_MARKING:
        currentMarking = new Marking(pnapi_pnml_yynet);
        break;
      }
      
      elementStack.push_back(i - 1);
      return true;
    }
  }
  
  return false;
}

/*!
 * \brief opens an element
 */
void open_element(char * s)
{    
  // first check if this element is whitelisted
  if(!ignoring && !is_whitelisted(s))
  {
    // element is blacklistet -- remember this!
    last_interesting_depth = current_depth;
    ignoring = true;
  }

  // we traverse a new element
  ++current_depth;
  pnml_attributes[current_depth].clear();
}

/*!
 * \brief removes whitespace around given string
 */
std::string sanitize(char * s)
{
  std::string str(s);
  char const * delims = " \t\r\n";

  // trim leading whitespace
  std::string::size_type notwhite = str.find_first_not_of(delims);
  str.erase(0, notwhite);

  // trim trailing whitespace
  notwhite = str.find_last_not_of(delims); 
  str.erase(notwhite + 1);

  return str;
}

/*!
 * \brief close an element
 */
void close_element()
{
  unsigned int myType = T_NONE;

  if(!ignoring)
  {
    myType = elementStack[elementStack.size() - 1];
    elementStack.pop_back();
  }

  switch(file_part)
  {
  case(T_PORTS):
  {
    // TODO: sort interface labels to their ports
    
    const std::string id = pnml_attributes[current_depth]["id"];
    const std::string name = pnml_attributes[current_depth]["__name__"];
    const std::string usedName = id; // = name.empty() ? id : name;
    
    switch(myType)
    {
    case(T_INPUT):
    {
      Label & l = pnapi_pnml_yynet.getInterface().addInputLabel(usedName);
      pnml_id2label[id] = &l;
      break;
    }
    case(T_OUTPUT):
    {
      Label & l = pnapi_pnml_yynet.getInterface().addOutputLabel(usedName);
      pnml_id2label[id] = &l;
      break;
    }
    case(T_SYNCHRONOUS):
    {
      Label & l = pnapi_pnml_yynet.getInterface().addSynchronousLabel(usedName);
      pnml_id2label[id] = &l;
      break;
    }
    case(T_TEXT):
    {
      // check the parent
      if(elementStack[elementStack.size() - 1] == T_NAME)
      {
        // store data as parent's attribute
        pnml_attributes[current_depth - 2]["__name__"] = pnml_attributes[current_depth]["__data__"];
      }
      break;
    }
    } /* myType */
    break;
  }
  case(T_NET):
  {
    switch(myType)
    {
    case(T_PLACE):
    {
      std::string id = pnml_attributes[current_depth]["id"];
      std::string name = pnml_attributes[current_depth]["__name__"];
      std::string usedName = id; // = name.empty() ? id : name; 
      pnml_id2node[id] = &pnapi_pnml_yynet.createPlace(usedName, atoi(pnml_attributes[current_depth]["__initialMarking__"].c_str()));
      break;
    }
    case(T_TRANSITION):
    {
      std::string id = pnml_attributes[current_depth]["id"];
      std::string name = pnml_attributes[current_depth]["__name__"];
      std::string usedName = id; // = name.empty() ? id : name; 
      Transition * currentTransition = &pnapi_pnml_yynet.createTransition(usedName);
      pnml_id2node[id] = currentTransition;

      if(!pnml_attributes[current_depth]["__send__"].empty())
      {
        Label * outputLabel = pnml_id2label[pnml_attributes[current_depth]["__send__"]];
        if (!outputLabel)
        {
          error(std::string("output place with id '" + pnml_attributes[current_depth]["__send__"] + "' not found").c_str());
        }
        currentTransition->addLabel(*outputLabel);
      }
      if(!pnml_attributes[current_depth]["__receive__"].empty())
      {
        Label * inputLabel = pnml_id2label[pnml_attributes[current_depth]["__receive__"]];
        if(!inputLabel)
        {
          error(std::string("input place with id '" + pnml_attributes[current_depth]["__receive__"] + "' not found").c_str());
        }
        currentTransition->addLabel(*inputLabel);
      }
      if(!pnml_attributes[current_depth]["__synchronize__"].empty())
      {
        Label * synchLabel = pnml_id2label[pnml_attributes[current_depth]["__synchronize__"]];
        if(!synchLabel)
        {
          error(std::string("synchronous label with id '" + pnml_attributes[current_depth]["__synchronize__"] + "' not found").c_str());
        }
        currentTransition->addLabel(*synchLabel);
      }
      break;
    }
    case(T_ARC):
    {
      // TODO: make sure, no interface places are concerned by an arc or insert label handling here
      Node * source = pnml_id2node[pnml_attributes[current_depth]["source"]];
      if(!source)
      {
        error(std::string("source node '" + pnml_attributes[current_depth]["source"] + "' not found").c_str());
      }
      Node * target = pnml_id2node[pnml_attributes[current_depth]["target"]];
      if(!target)
      {
        error(std::string("target node '" + pnml_attributes[current_depth]["target"] + "' not found").c_str());
      }
      const unsigned int weight = ((pnml_attributes[current_depth]["__inscription__"].empty()) ? 1 : atoi(pnml_attributes[current_depth]["__inscription__"].c_str()));
      pnapi_pnml_yynet.createArc(*source, *target, weight);
      break;
    }
    case(T_TEXT):
    {
      // check the parent
      if(elementStack[elementStack.size() - 1] == T_INITIALMARKING)
      {
        // store data as parent's attribute
        pnml_attributes[current_depth - 2]["__initialMarking__"] = pnml_attributes[current_depth]["__data__"];
      }
      if(elementStack[elementStack.size() - 1] == T_INSCRIPTION)
      {
        // store data as parent's attribute
        pnml_attributes[current_depth - 2]["__inscription__"] = pnml_attributes[current_depth]["__data__"];
      }
      // check the parent
      if(elementStack[elementStack.size() - 1] == T_NAME)
      {
        // store data as parent's attribute
        pnml_attributes[current_depth - 2]["__name__"] = pnml_attributes[current_depth]["__data__"];
      }
      break;
    }
    case(T_RECEIVE):
    {
      if(elementStack[elementStack.size() - 1] == T_TRANSITION)
      {
        pnml_attributes[current_depth - 1]["__receive__"] = pnml_attributes[current_depth]["idref"];
      }
      break;
    }
    case(T_SEND):
    {
      if(elementStack[elementStack.size() - 1] == T_TRANSITION)
      {
        pnml_attributes[current_depth - 1]["__send__"] = pnml_attributes[current_depth]["idref"];
      }
      break;
    }
    case(T_SYNCHRONIZE):
    {
      if(elementStack[elementStack.size() - 1] == T_TRANSITION)
      {
        pnml_attributes[current_depth - 1]["__synchronize__"] = pnml_attributes[current_depth]["idref"];
      }
      break;
    }
    } /* myType */
    break;
  }
  case(T_FINALMARKINGS):
  {
    switch(myType)
    {
    case(T_MARKING):
    {
      pnapi_pnml_yynet.getFinalCondition().addMarking(*currentMarking);
      delete currentMarking; // TODO: currentMarking = NULL; ?
      break;
    }
    case(T_PLACE):
    {
      const unsigned int tokens = atoi(pnml_attributes[current_depth]["__finalMarking__"].c_str());
      Node * place = pnml_id2node[pnml_attributes[current_depth]["idref"]];
      if(!place)
      {
        error(std::string("place with id '" + pnml_attributes[current_depth]["idref"] + "' not found").c_str());
      }
      (*currentMarking)[*static_cast<const Place *>(place)] = tokens;
    }
    case(T_TEXT):
    {
      // check the parent
      if(elementStack[elementStack.size() - 1] == T_PLACE)
      {
        // store data as parent's attribute
        pnml_attributes[current_depth - 1]["__finalMarking__"] = pnml_attributes[current_depth]["__data__"];
      }
    }
    break;
    } /* myType */
    break;
  }
  
  default: /* do nothing */ ;
  } /* file_part */

  --current_depth;

  // return form ignoring elements
  if(current_depth == last_interesting_depth)
  {
    last_interesting_depth = 0;
    ignoring = false;
  }
}

/*!
 * \brief store attributes
 */
void store_attributes(char * a1, char * a2)
{
  if(!ignoring)
  {
    // store attribute (strip quotes from a2)
    pnml_attributes[current_depth][a1] = std::string(a2).substr(1, strlen(a2) - 2);
  }
}

/*!
 * \brief store data
 */
void store_data(char * s)
{
  if(!ignoring)
  {
    // store current data as attribute of surrouding element
    pnml_attributes[current_depth]["__data__"] = sanitize(s);
  }
}


/*!
 * \brief Constructor
 */
Parser::Parser()
{
}

/*!
 * \brief Destructor
 * 
 * \note  Just used to call clean() automaticly
 */
Parser::~Parser()
{
  clean();
}

/*!
 * \brief Overwrites read net with empty net to free memory.
 */
void Parser::clean()
{
  pnapi_pnml_yynet = PetriNet();
}

/*!
 * \brief parses stream contents with the associated parser
 */
const PetriNet & Parser::parse(std::istream & is)
{
  // assign lexer input stream
  stream = &is;

  // reset line counter
  line = 1;

  // clean net
  pnapi_pnml_yynet = PetriNet();

  // call the parser
  pnml::parse();

  // clean up lexer
  pnml::lex_destroy();
  
  return pnapi_pnml_yynet;
}

} /* namespace pnml */


namespace lola
{

/// "assertion"
void check(bool condition, const std::string & msg)
{
  if (!condition)
    throw io::InputError(io::InputError::SEMANTIC_ERROR, 
        io::util::MetaData::data(*parser::stream)[io::INPUTFILE],
        parser::line, parser::token, msg);
}

/******************************************\
 *  "global" variables for flex and bison *
\******************************************/

/// generated petrinet
PetriNet pnapi_lola_yynet;

/// mapping of names to places
map<std::string, Place *> places_;
/// recently read transition
Transition * transition_ = NULL;
/// all purpose place pointer
Place * place_ = NULL;
/// target of an arc
Node * * target_ = NULL;
/// source of an arc
Node * * source_ = NULL;
/// converts NUMBER and IDENT in string
std::stringstream nodeName_;
/// read capacity
int capacity_ = 0;


/*!
 * \brief Constructor
 */
Parser::Parser()
{
}

/*!
 * \brief Destructor
 * \note  Just used to call clean() automaticly
 */
Parser::~Parser()
{
  clean();
}

/*!
 * \brief Overwrites read net with empty net to free memory.
 */
void Parser::clean()
{
  pnapi_lola_yynet = PetriNet();
}

/*!
 * \brief parses stream contents with the associated parser
 */
const PetriNet & Parser::parse(std::istream & is)
{
  // assign lexer input stream
  stream = &is;

  // reset line counter
  line = 1;

  pnapi_lola_yynet = PetriNet();

  // call the parser
  lola::parse();

  // clean up lexer
  lola::lex_destroy();

  // clean up global variables
  places_.clear();
  transition_ = NULL;
  place_ = NULL;
  source_ = target_ = NULL;
  nodeName_.clear();

  return pnapi_lola_yynet;
}

} /* namespace lola */



namespace sa
{

/* Global variables for flex+bison */

/// generated automaton
Automaton pnapi_sa_yyautomaton;

/// temporary list of idents
std::vector<std::string> identlist;
/// input labels
std::set<std::string> input_;
/// output labels
std::set<std::string> output_;
/// synchronous labels
std::set<std::string> synchronous_;

/// temporary state pointer
State * state_ = NULL;
/// states by name
std::map<int, State *> states_;
/// whether recent state is final
bool final_ = false;
/// whether recent state is initial
bool initial_ = false;
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


/*!
 * \brief constructor
 */
Parser::Parser()
{
}

/*!
 * \brief destructor
 * 
 * \note only used to call clean() automatically
 */
Parser::~Parser()
{
  clean();  
}

/*!
 * \brief frees memory
 */
void Parser::clean()
{
  pnapi_sa_yyautomaton = Automaton();
}

/*!
 * \brief parses stream contents with the associated parser
 */
const Automaton & Parser::parse(std::istream & is)
{
  // set input stream
  stream = &is;

  // reset line counter
  line = 1;

  // clear automaton
  pnapi_sa_yyautomaton = Automaton();

  // call actual parser
  sa::parse();

  // clean up lexer
  sa::lex_destroy();

  // clean up global variables
  identlist.clear();
  input_.clear();
  output_.clear();
  synchronous_.clear();
  state_ = NULL;
  states_.clear();
  initial_ = final_ = false;
  succState_.clear();
  succLabel_.clear();
  succType_.clear();

  return pnapi_sa_yyautomaton;
}

} /* namespace sa */


namespace pn
{
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
bool in_marking_list = false;
/// whether parser is in an arc list
bool in_arc_list = false;

/*!
 * \brief Constructor
 */
Parser::Parser()
{
}

/*!
 * \brief Destructor
 * 
 * \note  Just used to call clean() automaticly
 */
Parser::~Parser()
{
  clean();
}

/*!
 * \brief parses stream contents with the associated parser
 */
void Parser::parse(std::istream & is)
{
  // assign lexer input stream
  stream = &is;

  // reset line counter
  line = 1;

  // call the parser
  pn::parse();

  // clean up lexer
  pn::lex_destroy();
}

/*!
 * \brief cleans used variables
 */
void Parser::clean()
{
  transitions_.clear();
  places_.clear();
  initialMarked_.clear();
  interface_.clear();
  arcs_.clear();
  in_marking_list = false;
  in_arc_list = false;
  tempNodeMap_.clear();
}

} /* namespace pn */

} /* namespace parser */

} /* namespace pnapi */
