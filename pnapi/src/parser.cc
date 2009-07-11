#include "config.h"
#include <cassert>

#ifndef NDEBUG
#include <iostream>
using std::cout;
using std::cerr;
using std::endl;
#endif

#include <fstream>
#include <sstream>
#include <iostream>

#include "parser.h"
#include "io.h"
#include "formula.h"
#include "state.h"

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

    std::istream * stream;

    char * token;

    int line;

    void error(const string & msg)
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
      
      /// parsed ident
      string ident; 
      /// generated petrinet
      PetriNet pnapi_owfn_yynet;
      
      /// mapping of names to places
      std::map<std::string, Place*> places_;
      /// recently read transition
      Transition* transition_;
      /// all purpose place pointer
      Place* place_;
      /// target of an arc
      Node * * target_;
      /// source of an arc
      Node * * source_;
      /// converts NUMBER and IDENT in string
      std::stringstream nodeName_;
      /// type of recently read places
      Node::Type placeType_;
      /// labels for synchronous communication
      std::set<std::string> labels_;
      /// read capacity
      int capacity_;
      /// used port
      std::string port_;
      /// constrains
      std::map<Transition*, std::set<std::string> > constrains_;
      /// whether read marking is the initial marking or a final marking
      bool markInitial_;
      /// pointer to a final marking
      Marking* finalMarking_;
      /// whether pre- or postset is read
      bool placeSetType_;
      /// precet/postset for fast checks
      set<Place*> placeSet_;
      /// whether to check labels
      bool checkLabels_;
        
      /// "assertion"
      void check(bool condition, const std::string & msg)
      {
        if (!condition)
          throw io::InputError(io::InputError::SEMANTIC_ERROR, 
              io::util::MetaData::data(*parser::stream)[io::INPUTFILE],
                       parser::line, parser::token, msg);
      }
    
      /*!
       * \brief Constructor
       */
      Parser::Parser()
      {
      }
      
      const PetriNet & Parser::parse(istream & is)
      {
        // assign lexer input stream
        stream = &is;

        // reset line counter
        line = 1;
        
        pnapi_owfn_yynet = PetriNet();
        
        // call the parser
        owfn::parse();
        
        // clean up global variables
        ident.clear();
        places_.clear();
        nodeName_.clear();
        labels_.clear();
        constrains_.clear();
        placeSet_.clear();

        return pnapi_owfn_yynet;
      }
    } /* namespace owfn */



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
      
      /// parsed ident
      string ident; 
      /// generated petrinet
      PetriNet pnapi_lola_yynet;
      
      /// mapping of names to places
      map<std::string, Place*> places_;
      /// recently read transition
      Transition* transition_;
      /// all purpose place pointer
      Place* place_;
      /// target of an arc
      Node * * target_;
      /// source of an arc
      Node * * source_;
      /// converts NUMBER and IDENT in string
      std::stringstream nodeName_;
      /// read capacity
      int capacity_;
      /// precet/postset for fast checks
      set<Place*> placeSet_;
      /// preset/postset label for parse exception
      bool placeSetType_;
      
      
      /*!
       * \brief Constructor
       */
      Parser::Parser()
      {
      }
      
      const PetriNet & Parser::parse(istream & is)
      {
        // assign lexer input stream
        stream = &is;

        // reset line counter
        line = 1;
        
        pnapi_lola_yynet = PetriNet();
        
        // call the parser
        lola::parse();
        
        // clean up global variables
        ident.clear();
        places_.clear();
        nodeName_.clear();
        placeSet_.clear();

        return pnapi_lola_yynet;
      }

    } /* namespace lola */


    namespace onwd
    {

      Node * node;


      Parser::Parser() :
	parser::Parser<Node>(node, onwd::parse)
      {
      }


      /* simple child adding */

      Node::Node() :
	BaseNode(), type(STRUCT), number(0)
      {
      }

      Node::Node(Node * node) :
	BaseNode(node), type(STRUCT), number(0)
      {
      }

      Node::Node(Node * node1, Node * node2) :
	BaseNode(node1, node2), type(STRUCT), number(0)
      {
      }

      Node::Node(Node * node1, Node * node2, Node * node3) :
	BaseNode(node1, node2, node3), type(STRUCT), number(0)
      {
      }


      /* data node construction */

      Node::Node(Type type, string * str1, int number) :
	type(type), string1(*str1), number(number)
      {
	delete str1;
      }

      Node::Node(Type type, string * str1, string * str2) :
	type(type), string1(*str1), string2(*str2), number(0)
      {
	delete str1;
	delete str2;
      }

      Node::Node(Type type, string * str1, string * str2, int number) :
	type(type), string1(*str1), string2(*str2), number(number)
      {
	delete str1;
	delete str2;
      }


      /* typed node construction */

      Node::Node(Type type) :
	BaseNode(), type(type), number(0)
      {
      }

      Node::Node(Type type, Node * node) :
	BaseNode(node), type(type), number(0)
      {
      }

      Node::Node(Type type, Node * node1, Node * node2) :
	BaseNode(node1, node2), type(type), number(0)
      {
      }


      void Node::mergeData(Node * node)
      {
	assert(node != NULL);
	assert(node->type == DATA);

	// FIXME
	assert(false);

	delete node;
      }

      void Node::mergeChildren(Node * node)
      {
	assert(node != NULL);
	assert(node->type == STRUCT);

	children_ = node->children_;
	node->children_.clear();
	delete node;
      }


      /* visiting nodes */

      Visitor::Visitor(map<string, PetriNet *> & nets) :
	nets_(nets)
      {
      }

      void Visitor::beforeChildren(const Node & node)
      {
	switch (node.type)
	  {
	  case INSTANCE:
	    {
	      string name = node.string1;
	      PetriNet net;
	      if (nets_.find(name) != nets_.end())
		net = *nets_[name];
	      for (int i = 0; i < node.number; i++)
		instances_[name].push_back(net);
	      break;
	    }

	  case PLACE:
	    {
	      string netName = node.string1;
	      string placeName = node.string2;
	      vector<PlaceDescription> places;
	      for (vector<PetriNet>::iterator it = instances_[netName].begin();
		   it != instances_[netName].end(); ++it)
		places.push_back(PlaceDescription(netName, *it, placeName));
	      places_.push_back(places);
	      break;
	    }

	  default: /* empty */ ;
	  }
      }

      void Visitor::afterChildren(const Node & node)
      {
	switch (node.type)
	  {
	  case INSTANCES:
	    for (map<string, PetriNet *>::iterator it = nets_.begin();
		 it != nets_.end(); ++it)
	      node.check(instances_.find(it->first) != instances_.end(),
			 it->first, "Petri net not declared as instance");
	    break;

	  case ANY_WIRING:
	  case ALL_WIRING:
	    {
	      assert(places_.size() == 2);
	      vector<PlaceDescription> p1s =
		places_.front(); places_.pop_front();
	      vector<PlaceDescription> p2s =
		places_.front(); places_.pop_front();
	      for (vector<PlaceDescription>::iterator p1i =
		     p1s.begin(); p1i != p1s.end(); ++p1i)
		for (vector<PlaceDescription>::iterator p2i =
		       p2s.begin(); p2i != p2s.end(); ++p2i)
		  {
		    pair<Place *, bool> p1 =
		      getPlace(node, *p1i, Place::OUTPUT);
		    pair<Place *, bool> p2 =
		      getPlace(node, *p2i, Place::INPUT);
		    LinkNode * n1 =
		      getLinkNode(*p1.first, getLinkNodeMode(node.type),
				  !p1.second);
		    LinkNode * n2 =
		      getLinkNode(*p2.first, getLinkNodeMode(node.type),
				  !p2.second);
		    n1->addLink(*n2);
		    wiring_[p1.first] = n1;
		    wiring_[p2.first] = n2;
		  }
	      break;
	    }

	  default: /* empty */ ;
	  }
      }

      pair<Place *, bool> Visitor::getPlace(const Node & node,
					    PlaceDescription & pd,
					    Place::Type type)
      {
	bool isNetGiven = nets_.find(pd.netName) != nets_.end();
	Place * p = pd.instance->findPlace(pd.placeName);
	bool createPlace = !isNetGiven && p == NULL;
	if (createPlace)
	  {
	    p = &pd.instance->createPlace(pd.placeName, type);
	    pd.instance->finalCondition().addProposition(*p == 0);
	  }
	node.check(p != NULL && p->getType() == type,
		   pd.netName + "." + pd.placeName,
		   (string)(type == Place::INPUT ? "input" : "output") +
		   (string)" place expected");
	return pair<Place *, bool>(p, createPlace);
      }

      LinkNode * Visitor::getLinkNode(Place & p, LinkNode::Mode mode,
				      bool internalize)
      {
	map<Place *, LinkNode *>::iterator it = wiring_.find(&p);
	if (it != wiring_.end())
	  return (*it).second;
	else
	  return new LinkNode(p, mode, internalize);
      }

      LinkNode::Mode Visitor::getLinkNodeMode(Type type)
      {
	assert(type == ANY_WIRING || type == ALL_WIRING);

	if (type == ANY_WIRING)
	  return LinkNode::ANY;
	else
	  return LinkNode::ALL;
      }

      map<string, vector<PetriNet> > & Visitor::instances()
      {
	return instances_;
      }

      const map<Place *, LinkNode *> & Visitor::wiring()
      {
	return wiring_;
      }


    } /* namespace onwd */


    namespace sa
    {
      Parser::Parser()
      {
      }

      /*!
       * Global variables for flex+bison
       */
      Automaton pnapi_sa_yyautomaton;

      std::vector<std::string> identlist;

      State *state_;
      bool final_;
      bool initial_;
      std::vector<unsigned int> succState_;
      std::vector<std::string> succLabel_;
      std::vector<Automaton::Type> succType_;

      State *edgeState_;
      std::string edgeLabel_;
      Automaton::Type edgeType_;

      std::map<int, State *> states_;

      const Automaton & Parser::parse(std::istream &is)
      {
        stream = &is;

        line = 1;

        pnapi_sa_yyautomaton = Automaton();

        sa::parse();

        // clean up global variables

        return pnapi_sa_yyautomaton;
      }

    } /* namespace sa */

  } /* namespace parser */

} /* namespace pnapi */
