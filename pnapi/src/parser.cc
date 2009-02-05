#include <cassert>
#include <sstream>
#include <iostream>

#include "parser.h"
#include "io.h"

using std::stringstream;

namespace pnapi
{

  namespace parser
  {

    istream * stream;
    
    char * token;
    
    int line;

    void error(const string & msg)
    {
      throw io::InputError(io::InputError::SYNTAX_ERROR, 
			   io::FileIO::getFilename(*parser::stream), 
			   parser::line, parser::token, msg);
    }


    namespace owfn
    {

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

      Node::Node(Type type, Node * data, Node * list) :
	type(type)
      {
	mergeData(data);
	mergeChildren(list);
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

	switch (type)
	  {
	  case PLACE:
	  case TRANSITION:
	  case MARK:
	  case ARC:
	    if (node->type != DATA_NUMBER)
	      identifier = node->identifier;
	    else
	      { stringstream ss; ss << node->number; identifier = ss.str(); }
	    break;

	  case CAPACITY: *this = *node; break;

	  default: assert(false);
	  }
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

      PetriNet Visitor::getPetriNet() const
      {
	return net_;
      }

      void Visitor::beforeChildren(const Node & node)
      {
	Place * place;
	int nTokens;
	Formula * formula = NULL;

	switch (node.type)
	  {
	  case INPUT:    placeType_ = Place::INPUT;    break;
	  case OUTPUT:   placeType_ = Place::OUTPUT;   break;
	  case INTERNAL: placeType_ = Place::INTERNAL; break;

	  case PLACE: 
	    node.check(places_.find(node.identifier) == places_.end(),
		       node.identifier, "node name already used");
	    places_[node.identifier].type = placeType_;     
	    break;
	  case MARK:  
	    node.check(places_.find(node.identifier) != places_.end(),
		       node.identifier, "unknown place");
	    places_[node.identifier].marking = node.number; 
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

	  case FORMULA_EQ:
	  case FORMULA_NE:
	  case FORMULA_LT:
	  case FORMULA_GT:
	  case FORMULA_GE:
	  case FORMULA_LE:
	    place = net_.findPlace(node.identifier);
	    nTokens = node.number;
	    switch (node.type)
	      {
	      case FORMULA_EQ:
		formula = new FormulaEqual(*place, nTokens); break;
	      case FORMULA_NE:
		formula = new FormulaNot(new FormulaEqual(*place, nTokens));
		break;
	      case FORMULA_LT:
		formula = new FormulaLess(*place, nTokens); break;
	      case FORMULA_GT:
		formula = new FormulaGreater(*place, nTokens); break;
	      case FORMULA_GE:
		formula = new FormulaGreaterEqual(*place, nTokens); break;
	      case FORMULA_LE:
		formula = new FormulaLessEqual(*place, nTokens); break;
	      default: /* empty */ ;
	      }
	    formulas_.push_back(formula);
	    break;

	  default: /* empty */ ;
	  }
      }

      void Visitor::afterChildren(const Node & node)
      {
	switch (node.type)
	  {
	  case INITIAL:
	    for (map<string, PlaceAttributes>::iterator it = places_.begin();
		 it != places_.end(); ++it)
	      net_.createPlace(it->first, it->second.type, it->second.marking);
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
	    if (formulas_.size() < 1)
	      assert(false); // FIXME
	      //throw string("operand for unary NOT operator expected");
	    formulas_.push_back(new FormulaNot(formulas_.front()));
	    formulas_.pop_front();
	    break;
	  case FORMULA_AND:
	  case FORMULA_OR:
	    {
	      if (formulas_.size() < 2)
		assert(false); // FIXME
		//throw string("two operands for binary AND/OR operator expected");
	      Formula * op1 = formulas_.front(); formulas_.pop_front();
	      Formula * op2 = formulas_.front(); formulas_.pop_front();
	      Formula * f;
	      if (node.type == FORMULA_AND)
		f = new FormulaAnd(op1, op2);
	      else
		f = new FormulaOr(op1, op2);
	      formulas_.push_back(f);
	      break;
	    }

	  case FORMULA_AAOPE:
	  case FORMULA_AAOIPE:
	  case FORMULA_AAOEPE:
	    assert(false) /* FIXME: formula constructs missing */;
	    break;

	  default: /* empty */ ;
	  }
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

  }

}
