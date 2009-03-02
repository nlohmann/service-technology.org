#ifndef NDEBUG
#include <iostream>
using std::cout;
using std::endl;
#endif

#include <cassert>
#include <fstream>
#include <sstream>
#include <iostream>

#include "parser.h"
#include "io.h"
#include "formula.h"

using std::ifstream;
using std::stringstream;
using std::string;
using std::map;
using std::set;
using std::pair;

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

      typedef const set<const Place *> * Places;
      typedef pair<Formula *, const set<const Place *> *> WildcardFormula;

      Node * node;

      Node::Node() :
	BaseNode(), type(NO_DATA)
      {
      }

      Node::Node(Node * node) :
	BaseNode(node), type(NO_DATA)
      {
      }

      Node::Node(Node * node1, Node * node2) :
	BaseNode(node1, node2), type(NO_DATA)
      {
      }

      Node::Node(Node * node1, Node * node2, Node * node3) :
	BaseNode(node1, node2, node3), type(NO_DATA)
      {
      }

      Node::Node(int val) :
	type(DATA_NUMBER), number(val)
      {
      }

      Node::Node(string * str) :
	type(DATA_IDENTIFIER), identifier(*str)
      {
	delete str;
      }

      Node::Node(Type type) :
	type(type)
      {
      }

      Node::Node(Type type, Node * node) :
	type(type)
      {
	assert(node != NULL);

	if (node->type == DATA_NUMBER || node->type == DATA_IDENTIFIER)
	  mergeData(node);
	else if (node->type == NO_DATA)
	  mergeChildren(node);
	else
	  addChild(node);
      }

      Node::Node(Type type, Node * node, int number) :
	type(type), number(number)
      {
	mergeData(node);
      }

      Node::Node(Type type, Node * node1, Node * node2) :
	type(type)
      {
	if (type == FORMULA_OR || type == FORMULA_AND)
	  {
	    addChild(node1);
	    addChild(node2);
	  }
	else
	  {
	    mergeData(node1);
	    mergeChildren(node2);
	  }
      }

      Node::Node(Type type, Node * data, Node * node1, Node * node2) :
	BaseNode(node1, node2), type(type)
      {
	mergeData(data);
      }

      Node & Node::operator=(const Node & node)
      {
	number = node.number;
	identifier = node.identifier;
	return *this;
      }

      void Node::mergeData(Node * node)
      {
	assert(node != NULL);
	assert(node->type == DATA_NUMBER || node->type == DATA_IDENTIFIER);

	if (type == CAPACITY)
	  *this = *node;
	else
	  if (node->type != DATA_NUMBER)
	    identifier = node->identifier;
	  else
	    { stringstream ss; ss << node->number; identifier = ss.str(); }

	delete node;
      }

      void Node::mergeChildren(Node * node)
      {
	assert(node != NULL);
	assert(node->type == NO_DATA);

	children_ = node->children_;
	node->children_.clear();
	delete node;
      }

      Parser::Parser() :
	parser::Parser<Node>(node, owfn::parse)
      {
      }

      Visitor::Visitor() :
	finalMarking_(net_)
      {
      }

      PetriNet Visitor::getPetriNet() const
      {
	return net_;
      }

      void Visitor::beforeChildren(const Node & node)
      {
	switch (node.type)
	  {
	  case INPUT:    placeType_ = Place::INPUT;    break;
	  case OUTPUT:   placeType_ = Place::OUTPUT;   break;
	  case INTERNAL: placeType_ = Place::INTERNAL; break;

	  case CAPACITY: capacity_  = node.number;     break;
	  case PORT:     port_      = node.identifier; break;

	  case PLACE: 
	    node.check(places_.find(node.identifier) == places_.end(),
		       node.identifier, "node name already used");
	    places_[node.identifier].type = placeType_;     
	    places_[node.identifier].capacity = capacity_;     
	    if (!port_.empty())
	      {
		node.check(placeType_ != Place::INTERNAL, 
			   node.identifier, "interface place expected");
		node.check(places_[node.identifier].port.empty(),
			   node.identifier, "place already assigned to port '" +
			   places_[node.identifier].port + "'");
		places_[node.identifier].port = port_;
	      }
	    break;
	  case PORT_PLACE:
	    {
	      map<string, PlaceAttributes>::iterator p = 
		places_.find(node.identifier);
	      node.check(p != places_.end(),
			 node.identifier, "unknown place");
	      node.check(p->second.type != Place::INTERNAL, 
			 node.identifier, "interface place expected");
	      node.check(p->second.port.empty(),
			 node.identifier, "place already assigned to port '" +
			 p->second.port + "'");
	      p->second.port = port_;
	      break;
	    }

	  case INITIALMARKING: 
	    isInitial_ = true;  
	    break;
	  case FINALMARKING:   
	    isInitial_ = false; 
	    finalMarking_ = Marking(net_, false, true);
	    break;
	  case MARK:  
	    node.check(places_.find(node.identifier) != places_.end(),
		       node.identifier, "unknown place");
	    if (isInitial_)
	      places_[node.identifier].marking = node.number;
	    else
	      finalMarking_[*net_.findPlace(node.identifier)] = node.number;
	    break;

	  case PRESET:  isPreset_ = true;  break;
	  case POSTSET: isPreset_ = false; break;

	  case ARC: 
	    if (isPreset_)
	      {
		node.check(preset_.find(node.identifier) == preset_.end(),
			   node.identifier, "place already used in preset");
		preset_[node.identifier] = node.number; 
	      }
	    else
	      {
		node.check(postset_.find(node.identifier) == postset_.end(),
			   node.identifier, "place already used in postset");
		postset_[node.identifier] = node.number;
	      }
	    break;

	  case FORMULA_TRUE:  
	    formulas_.push_back(WildcardFormula(new FormulaTrue, NULL));  
	    break;
	  case FORMULA_FALSE: 
	    formulas_.push_back(WildcardFormula(new FormulaFalse, NULL)); 
	    break;

	  case FORMULA_EQ:
	  case FORMULA_NE:
	  case FORMULA_LT:
	  case FORMULA_GT:
	  case FORMULA_GE:
	  case FORMULA_LE:
	    {
	      Place * place;
	      unsigned int nTokens;
	      Formula * formula;
	      place = net_.findPlace(node.identifier);
	      nTokens = node.number;
	      switch (node.type)
		{
		case FORMULA_EQ:
		  formula = new FormulaEqual(*place, nTokens); break;
		case FORMULA_NE:
		  formula = new Negation(FormulaEqual(*place, nTokens));
		  break;
		case FORMULA_LT:
		  formula = new FormulaLess(*place, nTokens); break;
		case FORMULA_GT:
		  formula = new FormulaGreater(*place, nTokens); break;
		case FORMULA_GE:
		  formula = new FormulaGreaterEqual(*place, nTokens); break;
		case FORMULA_LE:
		  formula = new FormulaLessEqual(*place, nTokens); break;
		default: assert(false);
		}
	      formulas_.push_back(WildcardFormula(formula, NULL));
	      break;
	    }

	  default: /* empty */ ;
	  }
      }

      void Visitor::afterChildren(const Node & node)
      {
	const set<const Place *> * wcPlaces = NULL;

	switch (node.type)
	  {
	  case INITIALMARKING:
	    for (map<string, PlaceAttributes>::iterator it = places_.begin();
		 it != places_.end(); ++it)
	      net_.createPlace(it->first, it->second.type, it->second.marking,
			       it->second.capacity, it->second.port);
	    break;
	  case FINALMARKING:
	    net_.finalCondition().addMarking(finalMarking_);
	    break;

	  case TRANSITION:
	    {
	      node.check(!net_.containsNode(node.identifier), node.identifier,
			 "node name already used");
	      Transition & trans = net_.createTransition(node.identifier);
	      for (map<string, unsigned int>::iterator it = preset_.begin();
		   it != preset_.end(); ++ it)
		{
		  Place * place = net_.findPlace(it->first);
		  node.check(place != NULL, it->first, "unknown place");
		  net_.createArc(*place, trans, it->second);
		}
	      for (map<string, unsigned int>::iterator it = postset_.begin();
		   it != postset_.end(); ++ it)
		{
		  Place * place = net_.findPlace(it->first);
		  node.check(place != NULL, it->first, "unknown place");
		  net_.createArc(trans, *place, it->second);
		}
	      preset_.clear();
	      postset_.clear();
	      break;
	    }

	  case FORMULA_NOT:
	    {
	      assert(formulas_.size() > 0);
	      WildcardFormula wcf = formulas_.front();
	      Formula * iwcf = integrateWildcard(wcf);
	      Formula * f = new Negation(*iwcf); delete iwcf;
	      formulas_.push_back(WildcardFormula(f, NULL));
	      delete formulas_.front().first; formulas_.pop_front();
	      break;
	    }
	  case FORMULA_AND:
	  case FORMULA_OR:
	    {
	      assert(formulas_.size() > 1);
	      WildcardFormula op1 = formulas_.front(); formulas_.pop_front();
	      WildcardFormula op2 = formulas_.front(); formulas_.pop_front();
	      Formula * f;
	      if (node.type == FORMULA_AND)
		{
		  node.check(op1.second == NULL, "", 
			     "wildcard must be last AND operand");
		  f = new Conjunction(*op1.first, *op2.first);
		  formulas_.push_back(WildcardFormula(f, op2.second));
		}
	      else
		{
		  Formula * f1 = integrateWildcard(op1);
		  Formula * f2 = integrateWildcard(op2);
		  f = new Disjunction(*f1, *f2); delete f1; delete f2;
		  formulas_.push_back(WildcardFormula(f, NULL));
		}
	      delete op1.first;
	      delete op2.first;
	      break;
	    }

	  case FORMULA_AAOPE:
	    wcPlaces = wcPlaces ? wcPlaces : (Places)&net_.getPlaces();
	    // fallthrough intended
	  case FORMULA_AAOIPE:
	    wcPlaces = wcPlaces ? wcPlaces : (Places)&net_.getInternalPlaces();
	    // fallthrough intended
	  case FORMULA_AAOEPE:
	    wcPlaces = wcPlaces ? wcPlaces : (Places)&net_.getInterfacePlaces();
	    // fallthrough intended
	    {
	      assert(formulas_.size() > 0);
	      node.check(formulas_.front().second == NULL, "", 
			 "wildcard must be last AND operand");
	      Formula * f = new Conjunction(*formulas_.front().first);
	      formulas_.push_back(WildcardFormula(f, wcPlaces));
	      delete formulas_.front().first; formulas_.pop_front();
	    }
	    break;

	  case CONDITION:
	    {
	      assert(formulas_.size() == 1);
	      Formula * f = integrateWildcard(formulas_.front());
	      net_.finalCondition() = *f; delete f;
	      delete formulas_.front().first;
	      break;
	    }

	  default: /* empty */ ;
	  }
      }

      Formula * Visitor::integrateWildcard(WildcardFormula wcf)
      {
	Conjunction * c = dynamic_cast<Conjunction *>(wcf.first);
	if (c == NULL || wcf.second == NULL)
	  return wcf.first->clone();
	else
	  return new Conjunction(*c, *wcf.second);
      }

    }


    namespace petrify
    {

      Node * node;

      Node::Node(Type type) : type(type) {}

      Node::Node(Type type, Node *child1, Node *child2, Node *child3, Node *child4) :
        type(type)
      {
        if(child1!=NULL)
        {
          addChild(child1);
        }

        Node *n1 = new Node(CTRL1);
        addChild(n1);

        if(child2!=NULL)
        {
          addChild(child2);
        }
        if(child3!=NULL)
        {
          addChild(child3);
        }

        Node *n2 = new Node(CTRL2);
        addChild(n2);

        if(child4!=NULL)
        {
          addChild(child4);
        }
      }

      Node::Node(Type type, string data, Node *child) :
        type(type), data(data)
      {
        if(child != NULL)
        {
          addChild(child);
        }
      }

      Node::Node(Type type, string data, Node *child1, Node *child2) :
        type(type), data(data)
      {
        if(child1 != NULL)
        {
          addChild(child1);
        }
        if(child2 != NULL)
        {
          addChild(child2);
        }
      }

      Visitor::Visitor()
      {
        in_marking_list=false;
        in_arc_list=false;
      }

      Parser::Parser() :
        parser::Parser<Node>(node, petrify::parse)
      {
      }

      void Visitor::beforeChildren(const Node & node)
      {
        // erzeuge result_ aus Knoten hier ...
        switch(node.type)
        {
        case Node::TLIST:
          {
            if(in_arc_list)
            {
              tempNodeSet.insert(node.data);
              result_->transitions.insert(node.data);
            } else
            {
              result_->interface.insert(node.data);
            }
            break;
          }
        case Node::PLIST:
          {
            result_->places.insert(node.data);
            if (in_marking_list)
              result_->initialMarked.insert(node.data);
            else
              tempNodeSet.insert(node.data);
            break;
          }
        case Node::PT:
        case Node::TP:
          {
            tempNodeStack.push(tempNodeSet);
            tempNodeSet.clear();
            break;
          }
        case Node::CTRL1:
          {
            in_arc_list = true;
            break;
          }
        case Node::CTRL2:
          {
            in_marking_list = true;
            break;
          }
        default: break;

        }
      }

      void Visitor::afterChildren(const Node & node)
      {
        // ... und hier
        switch(node.type)
        {
        case Node::PT:
        case Node::TP:
        {
          result_->arcs[node.data] = tempNodeSet;
          tempNodeSet.clear();
          if(tempNodeStack.size() > 0)
          {
            tempNodeSet = tempNodeStack.top();
            tempNodeStack.pop();
          }
        }
        default: break;
        }
      }

    }


    namespace onwd
    {

      Node * node;

      
      Parser::Parser() :
	parser::Parser<Node>(node, onwd::parse)
      {
      }


      /* simple child adding */

      Node::Node() :
	BaseNode(), type(STRUCT)
      {
      }

      Node::Node(Node * node) :
	BaseNode(node), type(STRUCT)
      {
      }

      Node::Node(Node * node1, Node * node2) :
	BaseNode(node1, node2), type(STRUCT)
      {
      }

      Node::Node(Node * node1, Node * node2, Node * node3) :
	BaseNode(node1, node2, node3), type(STRUCT)
      {
      }


      /* data node construction */

      Node::Node(Type type, string * str1, string * str2) :
	type(type), string1(*str1), string2(*str2)
      {
	delete str1;
	delete str2;
      }


      /* typed node construction */

      Node::Node(Type type) :
	type(type)
      {
      }

      Node::Node(Type type, Node * node) :
	type(type)
      {
	assert(node != NULL);

	if (node->type == DATA)
	  mergeData(node);
	else if (node->type == STRUCT)
	  mergeChildren(node);
	else
	  addChild(node);
      }

      Node::Node(Type type, Node * node1, Node * node2) :
	BaseNode(node1, node2), type(type)
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

      void Visitor::beforeChildren(const Node & node)
      {
	switch (node.type)
	  {
	  case INSTANCE:
	    {
	      ifstream file(node.string2.c_str());
	      file >> io::owfn >> io::meta(io::INPUTFILE, node.string2) 
		   >> instances_[node.string1];
	    }
	  default: /* empty */ ;
	  }
      }

      void Visitor::afterChildren(const Node & node)
      {
	switch (node.type)
	  {
	  default: /* empty */ ;
	  }
      }

      const map<string, PetriNet> & Visitor::instances()
      { 
	return instances_; 
      }


    } /* namespace onwd */

  } /* namespace parser */

} /* namespace pnapi */
