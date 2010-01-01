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

            for ( map< Node*, list<Event*> >::iterator i = successors.begin();
                  i != successors.end(); ++i ) {
            	(*i).second.clear();
            }
            successors.clear();
        };

        /// getter for id
        unsigned int getID() const {
        	return id;
        }

        /// set flag for this node and all successor nodes
        void setFlagRecursively(bool);

        /// compute node's cost and cut connections to inefficient successor nodes
        unsigned int computeCost();

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

        /// output this node as og node
        void outputOG(std::ostream&, map<Node*, bool>&, bool);

        /// output this node as sa node
		void outputSA(std::ostream&, map<Node*, bool>&, bool);


    private:

    	/*-----------.
		| attributes |
		`-----------*/

        /// the node's ID
        unsigned int id;
};

#endif /* Node_H_ */
