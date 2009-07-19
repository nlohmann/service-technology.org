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
#include <cassert>

#include <vector>
#include <stack>
#include <istream>

#include "io.h"
#include "link.h"
#include "automaton.h"
#include "component.h"
#include "petrinet.h"
#include "marking.h"

namespace pnapi
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


    /// input stream for lexers
    extern std::istream * stream;

    /// last read lexer token
    extern char * token;

    /// line number in input
    extern int line;

    /// called by generated lexers/parsers
    void error(const std::string &);


    /*!
     * \brief   Node of an Abstract Syntax Tree (AST)
     * \param   T An instance (class) of the Node template.
     *
     * Provides tree structures and part of a visitor pattern (class Visitor).
     */
    template <typename T> class BaseNode
    {
    public:

      /// standard constructor
      BaseNode();

      /// constructor
      BaseNode(T *);

      /// constructor
      BaseNode(T *, T *);

      /// constructor
      BaseNode(T *, T *, T *);

      /// constructor
      BaseNode(T *, T *, T *, T *);

      /// destructor
      virtual ~BaseNode();

      /// adds another node as a child of this one
      T * addChild(T *);

      /// receives a visitor for visiting the subtree rooted at this node
      void visit(Visitor<T> &) const;

      /// causes an error if condition is false
      void check(bool, const std::string &, const std::string &) const;

    protected:

      /// children of this node
      std::vector<T *> children_;

    private:
      int line;
      std::string token;
      std::string filename;
    };


    /*!
     * \brief   Generic Parser Capsule for flex/bison Parsers
     * \param   T An instance (class) of the BaseNode template.
     *
     * All the functionality is provided by traditional C-style lexers and
     * parsers. To work with a parser, first call #parse() and then #visit().
     */
    template <typename T> class Parser
    {
    public:

      /// destructor
      virtual ~Parser();

      /// parses stream contents with the associated parser
      T & parse(std::istream &);

    protected:

      /// constructor to be used in derived classes (framework instances)
      Parser(T * &, int (*)());

    private:
      T * & parseResult_;
      int (*yaccParse_)();
      T * rootNode_;
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

      virtual ~Visitor() {};
    };

  }



  /*************************************************************************
   ***** OWFN Parser
   *************************************************************************/

  // forward declarations
  class PetriNet;
  class Place;
  class Marking;

  namespace parser
  {

    /*!
     * \brief   OWFN Parser
     *
     * Instantiation of the parser framework for parsing OWFN files.
     */
    namespace owfn
    {

      /// flex generated lexer function
      int lex();

      /// bison generated parser function
      int parse();

      /// "assertion"
      void check(bool, const std::string &);

      /******************************************\
       *  "global" variables for flex and bison *
      \******************************************/
      
      /// parsed ident
      extern std::string ident; 
      /// generated petrinet
      extern PetriNet pnapi_owfn_yynet;
      
      /// mapping of names to places
      extern std::map<std::string, Place*> places_;
      /// recently read transition
      extern Transition* transition_;
      /// all purpose place pointer
      extern Place* place_;
      /// target of an arc
      extern Node * * target_;
      /// source of an arc
      extern Node * * source_;
      /// converts NUMBER and IDENT in string
      extern std::stringstream nodeName_;
      /// type of recently read places
      extern Node::Type placeType_;
      /// labels for synchronous communication
      extern std::set<std::string> labels_;
      /// read capacity
      extern int capacity_;
      /// used port
      extern std::string port_;
      /// constrains
      extern std::map<Transition*, std::set<std::string> > constrains_;
      /// whether read marking is the initial marking or a final marking
      extern bool markInitial_;
      /// pointer to a final marking
      extern Marking* finalMarking_;
      /// preset/postset label for parse exception
      extern bool placeSetType_;
      /// precet/postset for fast checks
      extern std::set<Place*> placeSet_;
      /// whether to check labels
      extern bool checkLabels_;
      
      /*!
       * \brief   Encapsulation of the flex/bison OWFN parser
       *
       * Connects to the flex/bison implementation for parsing.
       */
      class Parser
      {
      public:
	      Parser();
	      
	      /// parses stream contents with the associated parser
        const PetriNet & parse(std::istream &);
      };
    };



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

      /// bison generated parser function
      int parse();

      /// "assertion"
      void check(bool, const std::string &);

      /******************************************\
       *  "global" variables for flex and bison *
      \******************************************/
      
      /// parsed ident
      extern std::string ident; 
      /// generated petrinet
      extern PetriNet pnapi_lola_yynet;
      
      /// mapping of names to places
      extern std::map<std::string, Place*> places_;
      /// recently read transition
      extern Transition* transition_;
      /// all purpose place pointer
      extern Place* place_;
      /// target of an arc
      extern Node * * target_;
      /// source of an arc
      extern Node * * source_;
      /// converts NUMBER and IDENT in string
      extern std::stringstream nodeName_;
      /// read capacity
      extern int capacity_;
      /// precet/postset for fast checks
      extern std::set<Place*> placeSet_;
      /// preset/postset label for parse exception
      extern bool placeSetType_;
      
      
      /*!
       * \brief   Encapsulation of the flex/bison OWFN parser
       *
       * Connects to the flex/bison implementation for parsing.
       */
      class Parser
      {
      public:
        Parser();
        
        /// parses stream contents with the associated parser
        const PetriNet & parse(std::istream &);
      };
    };



    /*************************************************************************
     ***** ONWD Parser
     *************************************************************************/

    /*!
     * \brief   ONWD Parser
     *
     * Instantiation of the parser framework for parsing ONWD files.
     */
    namespace onwd
    {

      // forward declarations
      class Node;
      class Visitor;


      /// BaseNode instantiation
      typedef BaseNode<Node> BaseNode;


      /// output node of parser
      extern Node * node;

      /// flex generated lexer function
      int lex();

      /// bison generated parser function
      int parse();


      /*!
       * \brief   Encapsulation of the flex/bison ONWD parser
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
       * \brief   Node types
       */
      enum Type
	{
	  STRUCT, DATA, //< for simple structuring resp. data nodes
	  INSTANCES, INSTANCE, ANY_WIRING, ALL_WIRING, PLACE
	};


      /*!
       * \brief   Node of an ONWD AST
       *
       * Each Node has a Type which is the first parameter in a variety of
       * constructors. Furthermore other data collected during the parsing
       * process may be stored. Node
       * parameters in a constructor result in the nodes being added as
       * children.
       */
      class Node : public BaseNode
      {
      public:

	const Type type;
	const std::string string1;
	const std::string string2;
	const int number;

	Node();
	Node(Node *);
	Node(Node *, Node *);
	Node(Node *, Node *, Node *);

	Node(Type, std::string *, int);
	Node(Type, std::string *, std::string *);
	Node(Type, std::string *, std::string *, int);

	Node(Type);
	Node(Type, Node *);
	Node(Type, Node *, Node *);

	void mergeData(Node *);
	void mergeChildren(Node *);
      };


      /*!
       * \brief   Visitor for ONWD AST nodes
       *
       * Constructs a set of PetriNets during traversal via
       * Parser::visit().
       */
      class Visitor : public parser::Visitor<Node>
      {
      public:
	Visitor(std::map<std::string, PetriNet *> &);

	void beforeChildren(const Node &);
	void afterChildren(const Node &);

	std::map<std::string, std::vector<PetriNet> > & instances();
	const std::map<Place *, LinkNode *> & wiring();

      private:
	struct PlaceDescription
	{
	  std::string netName;
	  PetriNet * instance;
	  std::string placeName;

	  PlaceDescription(const std::string & net, PetriNet & inst,
			   const std::string & place) :
	    netName(net), instance(&inst), placeName(place) {}
	};

	std::map<std::string, PetriNet *> & nets_;
	std::map<std::string, std::vector<PetriNet> > instances_;
	std::deque<std::vector<PlaceDescription> > places_;
	std::map<Place *, LinkNode *> wiring_;

	std::pair<Place *, bool> getPlace(const Node &, PlaceDescription &,
					  Place::Type);
	LinkNode * getLinkNode(Place &, LinkNode::Mode, bool);
	LinkNode::Mode getLinkNodeMode(Type);
      };

    } /* namespace onwd */



    /*************************************************************************
     ***** SA Parser
     *************************************************************************/

    namespace sa
    {

      // forward declaration
      class Node;

      /// BaseNode instantiation
      typedef BaseNode<Node> BaseNode;

      // parser node
      extern Node *node;

      // flex lexer
      int lex();

      // bison parser
      int parse();

      class Parser : public parser::Parser<Node>
      {
      public:
        Parser();
      };

      /* Node Types */
      enum Type
      {
        STATE, FINAL, INITIAL, INIT_FINAL, EDGE, NO_DATA, ID, NUM,
        INPUT, OUTPUT
      };

      /*
       * \brief Node of a SA AST
       */
      class Node : public BaseNode
      {
      public:
        const Type type_;
        const int number_;
        const std::string identifier_;

        Node();
        Node(Node *);
        Node(Node *, Node *, Node *);

        Node(int);
        Node(std::string *);

        Node(Type);
        Node(Type, Node *);
        Node(Type, Node *, Node *);
        Node(Type, Node *, Node *, Node *);
      };

      /*
       * \brief Visitor of a SA AST
       */
      class Visitor : public parser::Visitor<Node>
      {
      public:
        Visitor();

        const Automaton & getAutomaton() const;

        void beforeChildren(const Node &);
        void afterChildren(const Node &);

      private:
        Automaton *sa_;

        std::stack<sa::Type> stack_;

        // temporary objects
        struct AState
        {
          unsigned int name;
          bool isFinal;
          bool isInitial;
          std::vector<std::string> edgeLabels;
          std::vector<unsigned int> targetNodes;
        } newState_;
        std::vector<std::string> input_;
        std::vector<std::string> output_;

        Automaton::Type getType(std::string) const;
      };

    } /* namespace sa */



    /*************************************************************************
     ***** Template Implementation
     *************************************************************************/


    /*!
     */
    template <typename T>
    BaseNode<T>::BaseNode() :
      line(parser::line), token(parser::token),
      filename(io::util::MetaData::data(*parser::stream)[io::INPUTFILE])
    {
    }

    /*!
     */
    template <typename T>
    BaseNode<T>::BaseNode(T * node) :
      line(parser::line), token(parser::token),
      filename(io::util::MetaData::data(*parser::stream)[io::INPUTFILE])
    {
      assert(node != NULL);
      addChild(node);
    }

    /*!
     */
    template <typename T>
    BaseNode<T>::BaseNode(T * node1, T * node2) :
      line(parser::line), token(parser::token),
      filename(io::util::MetaData::data(*parser::stream)[io::INPUTFILE])
    {
      assert(node1 != NULL);
      assert(node2 != NULL);
      addChild(node1);
      addChild(node2);
    }

    /*!
     */
    template <typename T>
    BaseNode<T>::BaseNode(T * node1, T * node2, T * node3) :
      line(parser::line), token(parser::token),
      filename(io::util::MetaData::data(*parser::stream)[io::INPUTFILE])
    {
      assert(node1 != NULL);
      assert(node2 != NULL);
      assert(node3 != NULL);
      addChild(node1);
      addChild(node2);
      addChild(node3);
    }

    /*!
     */
    template <typename T>
    BaseNode<T>::BaseNode(T * node1, T * node2, T * node3, T * node4) :
      line(parser::line), token(parser::token),
      filename(io::util::MetaData::data(*parser::stream)[io::INPUTFILE])
    {
      assert(node1 != NULL);
      assert(node2 != NULL);
      assert(node3 != NULL);
      assert(node4 != NULL);
      addChild(node1);
      addChild(node2);
      addChild(node3);
      addChild(node4);
    }

    /*!
     * Recursively destroys all children.
     */
    template <typename T> BaseNode<T>::~BaseNode()
    {
      for (typename std::vector<T *>::const_iterator it =
	     children_.begin(); it != children_.end(); ++it)
	delete *it;
    }

    /*!
     * \brief   adds another node as a child of this one
     * \param   node  the child reference to be added
     *
     * Should be used to recursively construct the AST during parsing (in the
     * C style bison parser).
     */
    template <typename T> T * BaseNode<T>::addChild(T * node)
    {
      children_.push_back(node);
      return static_cast<T *>(this);
    }

    /*!
     * \brief   receives a visitor for visiting the subtree rooted at this node
     * \param   visitor  the visitor
     *
     * First calls Visitor::beforeChildren(), then visits all children and at
     * last calls Visitor::afterChildren().
     */
    template <typename T> void BaseNode<T>::visit(Visitor<T> & visitor) const
    {
      visitor.beforeChildren(*static_cast<const T *>(this));
      for (typename std::vector<T *>::const_iterator it =
	     children_.begin(); it != children_.end(); ++it)
	(*it)->visit(visitor);
      visitor.afterChildren(*static_cast<const T *>(this));
    }

    /*!
     */
    template <typename T>
    void BaseNode<T>::check(bool condition, const std::string & token,
			    const std::string & msg) const
    {
      if (!condition)
	throw io::InputError(io::InputError::SEMANTIC_ERROR, filename, line,
			     token, msg);
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
    Parser<T>::Parser(T * & parseResult,
		      int (*yaccParse)()) :
      parseResult_(parseResult),
      yaccParse_(yaccParse),
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
    template <typename T> T & Parser<T>::parse(std::istream & is)
    {
      // possibly clean up old AST
      if (rootNode_ != NULL)
	delete rootNode_;

      // assign lexer input stream
      stream = &is;

      // reset line counter
      line = 1;

      // call the parser
      if ((*yaccParse_)() != 0)
	assert(false); // we should never end up here because the first error
                       // already results in an exception (maybe change this
                       // behavior in the future)

      // copy result pointer
      rootNode_ = parseResult_;

      // cleanup and return
      stream = NULL;
      parseResult_ = NULL;
      return *rootNode_;
    }

  }

}

#endif
