#include "config.h"
#include <cassert>
#include <iostream>
#include <fstream>
#include "Graph.h"

// used namespaces
using std::map;
using std::cout;
using std::endl;
using std::ofstream;

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


void Graph::outputDebug(std::ostream& file) {

	file << "graph has " << nodes.size() << " nodes and "
         << (events.size()) << " events\n" << endl;

    for (map<string, Event*>::const_iterator iter = events.begin();
         iter != events.end(); ++iter) {
    	Event* currentEvent = iter->second;
    	currentEvent->printToStdout();
    }

    file << "\nroot node is node id '";
    if ( root == NULL ) file << "NULL'" << endl; else file << root->getID() << "'\n" << endl;
    for (map<unsigned int, Node*>::iterator iter = nodes.begin();
         iter != nodes.end(); ++iter) {

        Node* currentNode = iter->second;
        currentNode->printToStdout();
    }
}

void Graph::output(std::ostream& file) {

    file << "{\n  generator:    " << PACKAGE_STRING
         << " (" << CONFIG_BUILDSYSTEM << ")"
         << "\n  invocation:   " << invocation << "\n  events:       "
         //<< static_cast<unsigned int>(Label::send_events) << " send, "
         //<< static_cast<unsigned int>(Label::receive_events) << " receive, "
         //<< static_cast<unsigned int>(Label::sync_events) << " synchronous"
         << "\n  statistics:   " << nodes.size() << " nodes"
         << "\n}\n\n";

    file << "INTERFACE\n";

    if ( true) {
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
    }

    if ( true) {
        file << "  OUTPUT\n    ";
        bool first = true;
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
    }

    if ( true) {
        file << "  SYNCHRONOUS\n    ";
        bool first = true;
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
    }

    file << "\nNODES\n";

    // the root
    root->output(file);
}
