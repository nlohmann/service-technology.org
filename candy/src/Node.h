#ifndef Node_H_
#define Node_H_

//#include <cassert>
#include <list>
#include <string>
#include <map>
#include "Formula.h"


using std::list;
using std::pair;
using std::string;
using std::map;


class Node {

    public:

        /// successor nodes together with their corresponding edge labels
        list< pair<string, Node*> > successors;

        // this node's formula
        Formula* formula;

        /// constructor
        explicit Node(unsigned int);

        /// destructor
        ~Node();

        // getter and setter for id
        unsigned int getID() const { return id; }
        void setID(unsigned int newID) { id = newID; }

    void recolorInefficientSuccessors(map< Node*, list<Node*> >& );

    unsigned int computeInefficientSuccessors(map< Node*, list<Node*> >& );

    unsigned int getCostMinimalAssignments(
        list< pair<string, unsigned int> >,
        list< FormulaAssignment >& );

    void getCostMinimalAssignmentsRecursively(
        list< pair<string, unsigned int> > ,
        unsigned int,
        FormulaAssignment,
        unsigned int&,
        list< FormulaAssignment >&);

        // for debugging
        void printToStdout();

    private:

        unsigned int id;
};

#endif /* Node_H_ */
