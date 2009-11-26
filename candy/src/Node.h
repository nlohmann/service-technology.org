#ifndef Node_H_
#define Node_H_

#include "settings.h"
#include "Formula.h"
#include "Event.h"



class Node {

    public:

    	/*-----------.
		| attributes |
		`-----------*/

        /// successor nodes together with their corresponding events
        map< Node*, list<Event*> > successors;

        /// this node's formula
        Formula* formula;

        /// states if this is a final node or not
        bool final;

        /// a boolean flag used to speed up some algorithms
        bool flag;

        bool negation;

        bool computedCost;
        unsigned int cost;

        /*--------.
		| methods |
		`--------*/

        /// basic constructor
        explicit Node(unsigned int _id) : formula(NULL), id(_id), final(false),
            flag(false), negation(false), computedCost(false), cost(0) {};

        /// basic destructor
        virtual ~Node() {
            if ( formula != NULL ) {
                formula->clear();
                delete formula;
            }
            successors.clear();
        };

        /// getter and setter for id
        unsigned int getID() const { return id; }
        void setID(unsigned int newID) { id = newID; }

        /// set flag for this node and all successor nodes
        void setFlagRecursively(bool);

        /// cut connections to inefficient successor nodes
        unsigned int computeEfficientSuccessors();

        /// compute a list of cost minimal assignments for this node
        unsigned int getCostMinimalAssignments(
        		list< pair<Event*, unsigned int> >,
        		list< FormulaAssignment >& );

        /// helper function for getCostMinimalAssignments
        void getCostMinimalAssignmentsRecursively(
        		list< pair<Event*, unsigned int> >,
        		unsigned int,
        		FormulaAssignment,
        		unsigned int&,
        		list< FormulaAssignment >&);

        /// compute a list of cost minimal assignments for this node
        /// under the assumption that the node's formula does not
        /// contain a negation
        unsigned int getCostMinimalAssignmentsWithoutNegation(
        		list< pair<Event*, unsigned int> >,
        		list< FormulaAssignment >& );

        /// helper function for getCostMinimalAssignmentsWithoutNegation
        void getCostMinimalAssignmentsWithoutNegationRecursively(
        		list< pair<Event*, unsigned int> >,
        		unsigned int,
        		FormulaAssignment,
        		unsigned int&,
        		list< FormulaAssignment >&);


        /// print debug information about this node
        void outputDebug(std::ostream&);
        void outputDebugRecursively(std::ostream&, map<Node*, bool>&);

        /// print standard information about this node
        void output(std::ostream&, map<Node*, bool>&, bool);


    private:

        /// the node's ID
        unsigned int id;
};

#endif /* Node_H_ */
