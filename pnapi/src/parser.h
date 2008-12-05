/* -*- mode: c++ -*- */

/*!
 * \file    parser.h
 * \brief   Parser Related Structures
 *
 * Includes a C++ parser framework (PNapi::parser) and parsers for various
 * Petri net input formats (e.g. OWFN (PNapi::parser::owfn)).
 */

#ifndef PNAPI_PARSER_H
#define PNAPI_PARSER_H

#include <istream>
#include <vector>
#include <deque>

#include "petrinet.h"

using std::istream;
using std::vector;
using std::deque;

namespace PNapi
{

  /*!
   * \brief   Generic Parser Framework
   *
   * Provides C++ classes encapsulating flex/bison parsers. Documentation is
   * available under <https://ikaria/trac/mesu/wiki/ParserFramework>.
   */
  namespace parser
  {

    // forward declaration
    template <typename T> class Visitor;


    /*!
     * \brief   Node of an Abstract Syntax Tree (AST)
     * \param   T An instance (class) of the Node template.
     *
     * Provides tree structures and part of a visitor pattern (class Visitor).
     */
    template <typename T> class Node
    {
    public:

      /// destructor
      ~Node();
      
      /// receives a visitor for visiting the subtree rooted at this node
      void visit(Visitor<T> &) const;

      /// adds another node as a child of this one
      void addChild(Node<T> &);

    private:
      vector<Node<T> *> children_;
    };


    /*!
     * \brief   Generic Parser Capsule for flex/bison Parsers
     * \param   T An instance (class) of the Node template.
     *
     * All the functionality is provided by traditional C-style lexers and 
     * parsers. To work with a parser, first call #parse() and then #visit().
     */
    template <typename T> class Parser
    {
    public:

      /// destructor
      ~Parser();

      /// receives a visitor for visiting the nodes of the AST
      void visit(Visitor<T> &) const;

      /// parses stream contents with the associated parser
      Parser<T> & parse(istream &);

    protected:

      /// constructor to be used in derived classes (framework instances)
      Parser(istream * &, Node<T> * &, int (*)());

    private:
      istream * & flexStream_;
      Node<T> * & parseResult_;
      int (*yaccParse_)();
      Node<T> * rootNode_;
    };


    /*!
     * \brief   Simple Visitor Pattern
     * \param   T An instance (class) of the Node template.
     *
     * Implemenation of a very basic visitor pattern. Visitor instances walk
     * along the abstract syntax tree (AST) and collect data to calculate the
     * result (e.g. a PetriNet object).
     */
    template <typename T> class Visitor
    {
    public:

      /*! 
       * \brief   called before traversing the children of a Node
       * 
       * Implementing only this method would result in a preorder traversal.
       */
      virtual void beforeChildren(const T &) =0;

      /*!
       * \brief   called after traversing the children of a Node
       *
       * Implementing only this method would result in a postorder traversal.
       */
      virtual void afterChildren(const T &) =0;
    };


    /*!
     * \brief   OWFN Parser
     *
     * Instantiation of the parser framework for parsing OWFN files.
     */
    namespace owfn
    {

      /*!
       * \brief   Node of an OWFN AST
       *
       * Each Node has a Type which is the first parameter in a variety of 
       * constructors. Furthermore other data collected during the parsing
       * process (a #petriNet, an #identifier and a #value) may be stored. Node
       * parameters in a constructor result in the nodes being added as 
       * children.
       */
      class Node : public parser::Node<Node>
      {
      public:

	enum Type { 
	  PETRINET, FORMULA_NOT, FORMULA_OR, FORMULA_AND, FORMULA_AAOPE, 
	  FORMULA_AAOIPE, FORMULA_AAOEPE, FORMULA_EQ, FORMULA_NE, FORMULA_LT, 
	  FORMULA_GT, FORMULA_GE, FORMULA_LE };

	Node(Type, Node *);
	Node(Type, Node *, Node *);
	Node(Type, PetriNet *, Node *);
	Node(Type, const char *, int);

	~Node();

	const Type type;
	PetriNet * const petriNet;
	const string identifier;
	const int value;
      };


      /*!
       * \brief   Encapsulation of the flex/bison OWFN parser
       *
       * Connects to the flex/bison implementation for parsing (#parse()) and 
       * result retrieval (#visit()). Call the two functions in this order.
       */
      class Parser : public parser::Parser<Node>
      {
      public:
	Parser();
      };


      /*!
       * \brief   Visitor for OWFN AST nodes
       *
       * Constructs a PetriNet (#getPetriNet()) during traversal via 
       * Parser::visit().
       */
      class Visitor : public parser::Visitor<Node>
      {
      public:
	inline const PetriNet & getPetriNet() const;
	
	void beforeChildren(const Node &);
	void afterChildren(const Node &);

      private:
	PetriNet net_;
	deque<Formula *> formulas_;
      };


      // inline member function
      const PetriNet & Visitor::getPetriNet() const
      {
	return net_;
      }

    }


    /*!
     * \brief   destructor
     *
     * Recursively destroys all children.
     */
    template <typename T> Node<T>::~Node()
    {
      for (typename vector<Node<T> *>::const_iterator it = children_.begin(); 
	   it != children_.end(); ++it)
	delete *it;
    }

    /*!
     * \brief   adds another node as a child of this one
     * \param   node  the child reference to be added
     * 
     * Should be used to recursively construct the AST during parsing (in the 
     * C style bison parser).
     */
    template <typename T> void Node<T>::addChild(Node<T> & node)
    {
      children_.push_back(&node);
    }

    /*!
     * \brief   receives a visitor for visiting the subtree rooted at this node
     * \param   visitor  the visitor
     *
     * First calls Visitor::beforeChildren(), then visits all children and at 
     * last calls Visitor::afterChildren().
     */
    template <typename T> void Node<T>::visit(Visitor<T> & visitor) const
    {
      visitor.beforeChildren(*static_cast<const T *>(this));
      for (typename vector<Node<T> *>::const_iterator it = children_.begin(); 
	   it != children_.end(); ++it)
	(*it)->visit(visitor);
      visitor.afterChildren(*static_cast<const T *>(this));
    }

    /*!
     * \brief   destructor
     *
     * Destroys AST if available.
     */
    template <typename T> Parser<T>::~Parser()
    {
      delete rootNode_;
    }

    /*!
     * \brief   constructor to be used in derived classes (framework instances)
     * \param   flexStream  a reference to a pointer variable holding the input
     *                      stream the lexer will use during scanning
     * \param   parseResult a reference to a pointer variable that will contain
     *                      the resulting AST after parsing
     * \param   yaccParse   pointer to the parse() function of the bison parser
     *
     * Connects to the flex/bison parser. This constructor should be called 
     * from within the initialization section of specialized parser classes
     * (see owfn::Parser::Parser() for an example).
     */
    template <typename T> 
    Parser<T>::Parser(istream * & flexStream, Node<T> * & parseResult, 
		      int (*yaccParse)()) :
      flexStream_(flexStream),
      yaccParse_(yaccParse),
      parseResult_(parseResult),
      rootNode_(NULL)
    {
    }

    /*! 
     * \brief   parses stream contents with the associated parser
     * \param   is  an input stream
     * \return the current parser instance
     *
     * The resulting AST can be traversed using the #visit() function.
     */
    template <typename T> Parser<T> & Parser<T>::parse(istream & is)
    {
      // possibly clean up old AST
      if (rootNode_ != NULL)
	delete rootNode_;

      // assign lexer input stream
      flexStream_ = &is;
      
      // call the parser
      if ((*yaccParse_)() != 0)
	throw string("yacc parser failed");

      // copy result pointer
      rootNode_ = parseResult_;

      // cleanup and return
      flexStream_ = NULL;
      parseResult_ = NULL;
      return *this;
    }

    /*!
     * \brief   receives a visitor for visiting the nodes of the AST
     * \param   visitor  a visitor
     *
     * Traverses the AST using Node::visit().
     */
    template <typename T> void Parser<T>::visit(Visitor<T> & visitor) const
    {
      rootNode_->visit(visitor);
    }

  }

}

#endif
