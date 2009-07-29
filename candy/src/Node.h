#ifndef Node_H_
#define Node_H_

#include <list>
#include <string>
#include <map>
#include "Formula.h"
#include "Event.h"


using std::list;
using std::pair;
using std::string;
using std::map;


class Node {

    public:

    	/*-----------.
		| attributes |
		`-----------*/

        /// successor nodes together with their corresponding events
        map< Node*, list<Event*> > successors;

        // this node's formula
        Formula* formula;

        bool final;
        list< Node* > efficientSuccessors;

        /*--------.
		| methods |
		`--------*/

        /// basic constructor
        explicit Node(unsigned int);

        /// basic destructor
        ~Node();

        // getter and setter for id
        unsigned int getID() const { return id; }
        void setID(unsigned int newID) { id = newID; }

        /// TODO comment
        void recolorInefficientSuccessors(map< Node*, list<Node*> >& );

        /// TODO comment
        unsigned int computeEfficientSuccessors();

        /// TODO comment
        unsigned int getCostMinimalAssignments(
        		list< pair< pair< Node*, Event*>, unsigned int> >,
        		list< FormulaAssignment >& );

        /// TODO comment
        void getCostMinimalAssignmentsRecursively(
        		list< pair< pair<Node*, Event*>, unsigned int> >,
        		unsigned int,
        		FormulaAssignment,
        		unsigned int&,
        		list< FormulaAssignment >&);

        /// print information about this node on std::cout
        void printToStdout();
        void printToStdoutRecursively();
        void output(std::ostream&);


    private:

        unsigned int id;
};

#endif /* Node_H_ */
