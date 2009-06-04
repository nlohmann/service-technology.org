#ifndef Node_H_
#define Node_H_

//#include "state.h"
//#include "SinglyLinkedList.h"
//#include "GraphFormula.h"
//#include "GraphEdge.h"
//#include "mynew.h"
//#include "debug.h"
//#include "options.h"
//#include <cassert>
#include <list>
#include <string>

using std::list;
using std::pair;
using std::string;


class Node {

    public:

        unsigned int id;

        /// successor nodes together with their corresponding edge labels
        list< pair<Node*, string> > successors;

        // TODO this node's formula

        /// constructor
        Node(unsigned int);

        /// destructor
        ~Node();

        void printToStdout();
};

#endif /* Node_H_ */
