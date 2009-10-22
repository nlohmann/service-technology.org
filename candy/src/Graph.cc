#include "settings.h"
#include "Graph.h"

// program invocation declared in main.cc
extern string invocation;



//! \brief a basic constructor of Graph
Graph::Graph() :
    root(NULL) {
}


//! \brief a basic destructor of Graph
Graph::~Graph() {
    root = NULL;
    nodes.clear();
}


// use this after computing efficient nodes
void Graph::removeInefficientNodesAndEvents() {

    // flag all efficient nodes and events
    root->setFlagRecursively(true);

    // remove unused nodes
    // ATTENTION: increment inside because of erase
    for ( map< unsigned int, Node* >::iterator i = nodes.begin(); i != nodes.end(); ) {

        // remove non-flaged node
        if ( not (i->second)->flag ) {
            // the erase returns incremented iterator, which is invalid for last element
            nodes.erase( i++ );
        } else {
            ++i;
        }
    }
}


void Graph::outputDebug(std::ostream& file) {

	file << "graph has " << nodes.size() << " nodes and "
         << (events.size()) << " events\n" << endl;

    for (map<string, Event*>::const_iterator iter = events.begin();
         iter != events.end(); ++iter) {
    	Event* currentEvent = iter->second;
    	currentEvent->outputDebug(file);
    }

    file << "\nroot node is node id '";
    if ( root == NULL ) file << "NULL'" << endl; else file << root->getID() << "'\n" << endl;
    for (map<unsigned int, Node*>::iterator iter = nodes.begin();
         iter != nodes.end(); ++iter) {

        Node* currentNode = iter->second;
        currentNode->outputDebug(file);
    }
}


void Graph::output(std::ostream& file) {

    unsigned int send = 0;
    unsigned int receive = 0;
    unsigned int synchronous = 0;

    for ( map< string, Event* >::const_iterator i = events.begin();
          i != events.end(); ++i) {

        if ( (i->second)->type == T_INPUT ) {
            ++receive;
        } else if ( (i->second)->type == T_OUTPUT ) {
            ++send;
        } else if ( (i->second)->type == T_SYNC ) {
            ++synchronous;
        } else {
            assert(false);
        }
    }

    file << "{\n"
         << "  generator:    " << PACKAGE_STRING << " (" << CONFIG_BUILDSYSTEM << ")\n"
         << "  invocation:   " << invocation << "\n"
         << "  events:       " << send << " send, "
                               << receive << " receive, "
                               << synchronous << " synchronous\n"
         << "  statistics:   " << nodes.size() << " nodes\n"
         << "}\n\n";

    file << "INTERFACE\n";

    // output input events
    file << "  INPUT\n    ";
    bool first = true;
    for ( map<string, Event*>::const_iterator i = events.begin();
        i != events.end(); ++i ) {

        if ( (i->second)->type == T_INPUT ) {
            if ( not first ) {
                file << ", ";
            }
            first = false;
            file << (i->second)->name;
        }
    }
    file << ";\n";

    // output output events
    file << "  OUTPUT\n    ";
    first = true;
    for ( map<string, Event*>::const_iterator i = events.begin();
        i != events.end(); ++i ) {

        if ( (i->second)->type == T_OUTPUT ) {
            if ( not first ) {
                file << ", ";
            }
            first = false;
            file << (i->second)->name;
        }
    }
    file << ";\n";

    // output synchronous events
    file << "  SYNCHRONOUS\n    ";
    first = true;
    for ( map<string, Event*>::const_iterator i = events.begin();
        i != events.end(); ++i ) {

        if ( (i->second)->type == T_SYNC ) {
            if ( not first ) {
                file << ", ";
            }
            first = false;
            file << (i->second)->name;
        }
    }
    file << ";\n";

    // print all nodes beginning from the root
    file << "\nNODES\n";
    map<Node*, bool> printed;
    root->output(file, printed, true);
    printed.clear();
}
