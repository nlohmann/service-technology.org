#include "Node.h"
#include <iostream>
//#include <cassert>

using std::cout;
using std::endl;


//! \brief constructor (three parameters)
//! \param _number the number of this node
Node::Node(unsigned int _id) :
    id(_id) {
}


//! \brief destructor
Node::~Node() {

    successors.clear();
}

void Node::printToStdout() {

    cout << "node id '" << id << "' with successors" << endl;
    for ( list< pair<Node*, string> >::iterator iter = successors.begin();
          iter != successors.end(); ++iter ) {

        Node* currentNode = iter->first;
        if ( currentNode != NULL ) {
            cout << "\tnode id '" << currentNode->id << "' with edge label '" << iter->second << "'" << endl;
        } else {
            cout << "\tnode NULL" << endl;
        }
    }
}
