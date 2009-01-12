#include <cassert>
#include <fstream>

#include "link.h"
#include "parser.h"

using std::ifstream;

using namespace pnapi::formula;

extern istream * pnapi_owfn_istream;
extern pnapi::parser::Node<pnapi::parser::owfn::Node> * pnapi_owfn_ast;
extern int pnapi_owfn_parse();

extern istream * pnapi_petrify_istream;
extern pnapi::parser::Node<pnapi::parser::petrify::Node> * pnapi_petrify_ast;
extern int pnapi_petrify_parse();

namespace pnapi
{

  namespace parser
  {

    namespace owfn
    {

      Node::Node(Type type, Node * node) : 
	type(type), petriNet(NULL), value(0)
      {
	if (node != NULL)
	  addChild(*node);
      }
	
      Node::Node(Type type, Node * node1, Node * node2) : 
	type(type), petriNet(NULL), value(0)
      {
	if (node1 != NULL)
	  addChild(*node1);
	if (node2 != NULL)
	  addChild(*node2);
      }

      Node::Node(Type type, PetriNet * net, Node * node) : 
	type(type), petriNet(net), value(0)
      {
	if (node != NULL)
	  addChild(*node);
      }

      Node::Node(Type type, const string * str, int i) : 
	type(type), petriNet(NULL), value(i), identifier(*str)
      {
      }

      Node::~Node()
      {
	if (petriNet != NULL)
	  delete petriNet;
      }

      Parser::Parser() :
	parser::Parser<Node>(::pnapi_owfn_istream, ::pnapi_owfn_ast,
			     &::pnapi_owfn_parse)
      {
      }

      void Visitor::beforeChildren(const Node & node)
      {
	Place * place;
	int nTokens;
	Formula * formula;

	switch (node.type) 
	  {
	  case Node::PETRINET:
	    net_ = *node.petriNet; // copy the net, the AST might destroy it
	    break;

	  case Node::FORMULA_EQ:
	  case Node::FORMULA_NE:
	  case Node::FORMULA_LT:
	  case Node::FORMULA_GT:
	  case Node::FORMULA_GE:
	  case Node::FORMULA_LE:
	    place = net_.findPlace(node.identifier);
	    nTokens = node.value;
	    switch (node.type)
	      {
	      case Node::FORMULA_EQ:
		formula = new FormulaEqual(place, nTokens); break;
	      case Node::FORMULA_NE:
		formula = new FormulaNot(new FormulaEqual(place, nTokens)); 
		break;
	      case Node::FORMULA_LT:
		formula = new FormulaLess(place, nTokens); break;
	      case Node::FORMULA_GT:
		formula = new FormulaGreater(place, nTokens); break;
	      case Node::FORMULA_GE:
		formula = new FormulaGreaterEqual(place, nTokens); break;
	      case Node::FORMULA_LE:
		formula = new FormulaLessEqual(place, nTokens); break;
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
	  case Node::FORMULA_NOT:
	    if (formulas_.size() < 1)
	      throw string("operand for unary NOT operator expected");
	    formulas_.push_back(new FormulaNot(formulas_.front()));
	    formulas_.pop_front();
	    break;
	  case Node::FORMULA_AND:
	  case Node::FORMULA_OR:
	    {
	      if (formulas_.size() < 2)
		throw string("two operands for binary AND/OR operator expected");
	      Formula * op1 = formulas_.front(); formulas_.pop_front();
	      Formula * op2 = formulas_.front(); formulas_.pop_front();
	      Formula * f;
	      if (node.type == Node::FORMULA_AND)
		f = new FormulaAnd(op1, op2);
	      else
		f = new FormulaOr(op1, op2);
	      formulas_.push_back(f);
	      break;
	    }

	  case Node::FORMULA_AAOPE:
	  case Node::FORMULA_AAOIPE:
	  case Node::FORMULA_AAOEPE:
	    assert(false) /* FIXME: formula constructs missing */;
	    break;

	  case Node::PETRINET:
	    if (!formulas_.empty())
	      {
		//FIXME: net_.setFinalCondition(*formulas_.front());
		formulas_.pop_front();
	      }
	    assert(formulas_.empty());
	    break;

	  default: /* empty */ ;
	  }
      }

    }
    

    namespace petrify
    {
    
      Node::Node(Type type) : type(type) {}
    
      Node::Node(Type type, Node *child1, Node *child2, Node *child3, Node *child4) :
        type(type)
      {
        if(child1!=NULL)
        {
          addChild(*child1);
        }
        
        Node *n1 = new Node(CTRL1);
        addChild(*n1);
        
        if(child2!=NULL)
        {
          addChild(*child2);
        }
        if(child3!=NULL)
        {
          addChild(*child3);
        }
        
        Node *n2 = new Node(CTRL2);
        addChild(*n2);
        
        if(child4!=NULL)
        {
          addChild(*child4);
        }
      }
      
      Node::Node(Type type, string data, Node *child) :
        type(type), data(data)
      {
        if(child != NULL)
        {
          addChild(*child);
        }
      }
      
      Node::Node(Type type, string data, Node *child1, Node *child2) :
        type(type), data(data)
      {
        if(child1 != NULL)
        {
          addChild(*child1);
        }
        if(child2 != NULL)
        {
          addChild(*child2);
        }
      }

      Visitor::Visitor()
      {
        in_marking_list=false;
        in_arc_list=false;
      }
      
      Parser::Parser() : 
        parser::Parser<Node>(::pnapi_petrify_istream, ::pnapi_petrify_ast, &::pnapi_petrify_parse)
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
