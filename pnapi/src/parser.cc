#include <cassert>

#include "parser.h"

extern istream * pnapi_owfn_istream;
extern PNapi::parser::Node<PNapi::parser::owfn::Node> * pnapi_owfn_ast;
extern int pnapi_owfn_parse();

namespace PNapi
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

      Node::Node(Type type, const char * str, int i) : 
	type(type), petriNet(NULL), value(i), identifier(str)
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
	      }
	    formulas_.push_back(formula);
	    break;
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
		net_.setFinalCondition(formulas_.front());
		formulas_.pop_front();
	      }
	    assert(formulas_.empty());
	    break;
	  }
      }

    }

  }

}
