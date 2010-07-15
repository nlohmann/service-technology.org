/*!
 * \file parser-pnml-wrapper.cc
 */

#include "config.h"

#include "parser-pnml-wrapper.h"

#include "automaton.h"
#include "marking.h"
#include "myio.h"
#include "petrinet.h"

#include <cstring>
#include <sstream>

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

namespace pnapi { namespace parser { namespace pnml
{

/*!
 * \brief parses stream contents with the associated parser
 */
PetriNet parse(std::istream & is)
{
  Parser myParser;
  return myParser.parse(is);
}

namespace yy
{

/*!
 * \brief Wrapper funktion to link Bison parser with Flex lexer
 */
int yylex(BisonParser::semantic_type * yylval, BisonParser::location_type *, Parser & p)
{
  p.lexer_.yylval = yylval;
  return p.lexer_.yylex();
}

/*!
 * \brief constructor
 */
Lexer::Lexer(Parser & p) :
  AbstractLexer<Parser, BisonParser::semantic_type, PnmlFlexLexer>(p)
{
}

/*!
 * \brief get the name of xml tags
 */
char * Lexer::word(char * s)
{
  char * buf;
  int i, k;
  for(k = 0; (isspace(s[k]) || (s[k] == '<')); ++k);
  for(i = k; (s[i] && (!isspace(s[i]))); ++i);
  buf = (char*)malloc((i - k + 1) * sizeof(char));
  strncpy(buf, &s[k], i - k);
  buf[i - k] = '\0';
  return buf;
}

/*!
 * \brief implement parser error handling
 */
void BisonParser::error(const location_type & loc, const std::string & msg)
{
  parser::error(*(parser_.is_), parser_.lexer_.lineno(), parser_.lexer_.YYText(), msg);
}

} /* namespace yy */


// we only care about these elements
const char * Parser::whitelist[] = { "pnml", "net", "arc", "place", "transition",
                                     "inscription", "initialMarking", "annotation",
                                     "text", "module", "ports", "port", "input", "output",
                                     "synchronous", "receive", "send", "synchronize",
                                     "finalmarkings", "marking", "page", "name", NULL };


/*!
 * \brief Constructor
 */
Parser::Parser() :
  AbstractParser<yy::BisonParser, yy::Lexer, Parser>(),
  currentMarking(NULL), current_depth(0), last_interesting_depth(0),
  ignoring(false), file_part(T_NONE)
{
}

/*!
 * \brief throw an error
 */
void Parser::error(const std::string & msg)
{
  parser::error(*is_, lexer_.lineno(), lexer_.YYText(), msg);
}

/*!
 * \brief whether we care about a given element
 */
bool Parser::is_whitelisted(char * s)
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
        currentMarking = new Marking(net_);
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
void Parser::open_element(char * s)
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
std::string Parser::sanitize(char * s)
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
void Parser::close_element()
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
      Label & l = net_.getInterface().addInputLabel(usedName);
      pnml_id2label[id] = &l;
      break;
    }
    case(T_OUTPUT):
    {
      Label & l = net_.getInterface().addOutputLabel(usedName);
      pnml_id2label[id] = &l;
      break;
    }
    case(T_SYNCHRONOUS):
    {
      Label & l = net_.getInterface().addSynchronousLabel(usedName);
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
      pnml_id2node[id] = &net_.createPlace(usedName, atoi(pnml_attributes[current_depth]["__initialMarking__"].c_str()));
      break;
    }
    case(T_TRANSITION):
    {
      std::string id = pnml_attributes[current_depth]["id"];
      std::string name = pnml_attributes[current_depth]["__name__"];
      std::string usedName = id; // = name.empty() ? id : name; 
      Transition * currentTransition = &net_.createTransition(usedName);
      pnml_id2node[id] = currentTransition;

      if(!pnml_attributes[current_depth]["__send__"].empty())
      {
        Label * outputLabel = pnml_id2label[pnml_attributes[current_depth]["__send__"]];
        if (!outputLabel)
        {
          error(std::string("output place with id '" + pnml_attributes[current_depth]["__send__"] + "' not found"));
        }
        currentTransition->addLabel(*outputLabel);
      }
      if(!pnml_attributes[current_depth]["__receive__"].empty())
      {
        Label * inputLabel = pnml_id2label[pnml_attributes[current_depth]["__receive__"]];
        if(!inputLabel)
        {
          error(std::string("input place with id '" + pnml_attributes[current_depth]["__receive__"] + "' not found"));
        }
        currentTransition->addLabel(*inputLabel);
      }
      if(!pnml_attributes[current_depth]["__synchronize__"].empty())
      {
        Label * synchLabel = pnml_id2label[pnml_attributes[current_depth]["__synchronize__"]];
        if(!synchLabel)
        {
          error(std::string("synchronous label with id '" + pnml_attributes[current_depth]["__synchronize__"] + "' not found"));
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
        error(std::string("source node '" + pnml_attributes[current_depth]["source"] + "' not found"));
      }
      Node * target = pnml_id2node[pnml_attributes[current_depth]["target"]];
      if(!target)
      {
        error(std::string("target node '" + pnml_attributes[current_depth]["target"] + "' not found"));
      }
      const unsigned int weight = ((pnml_attributes[current_depth]["__inscription__"].empty()) ? 1 : atoi(pnml_attributes[current_depth]["__inscription__"].c_str()));
      net_.createArc(*source, *target, weight);
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
      net_.getFinalCondition().addMarking(*currentMarking);
      delete currentMarking; // TODO: currentMarking = NULL; ?
      break;
    }
    case(T_PLACE):
    {
      const unsigned int tokens = atoi(pnml_attributes[current_depth]["__finalMarking__"].c_str());
      Node * place = pnml_id2node[pnml_attributes[current_depth]["idref"]];
      if(!place)
      {
        error(std::string("place with id '" + pnml_attributes[current_depth]["idref"] + "' not found"));
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
void Parser::store_attributes(char * a1, char * a2)
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
void Parser::store_data(char * s)
{
  if(!ignoring)
  {
    // store current data as attribute of surrouding element
    pnml_attributes[current_depth]["__data__"] = sanitize(s);
  }
}

} } } /* namespace pnapi::parser::pnml */
