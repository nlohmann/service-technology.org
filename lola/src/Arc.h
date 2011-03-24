#pragma once

class Node;
class Transition;
class Place;

/// Petri net arc
class Arc {
    public:
        static unsigned int cnt; ///< number of arcs
        Node* Source;  ///< the source node of arc (can be place or transition)
        Node* Destination;  ///< the sink node of arc (can be place or transition)
        Place* pl;  ///< the place connected to this arc (can be source or sink)
        Transition* tr;  ///< the transition connected to this arc (can be source of sink)
        unsigned int Multiplicity; ///< multiplicity attached to this arc
        Arc(Transition*, Place*, bool , unsigned int);  ///< constructor
        Node* Get(bool);  ///< false --> get source  true --> get destination
        void operator += (unsigned int);  ///< add i to multiplicity of arc
};

/// constructor
inline Arc::Arc(Transition* t, Place* p, bool totrans, unsigned int mult) {
    tr = t;
    pl = p;
    Source = totrans ? (Node*) p : (Node*) t;
    Destination = totrans ? (Node*) t : (Node*) p;
    Multiplicity = mult;
    cnt++;
}

/// get source (0) or destination (1) of node
inline Node* Arc::Get(bool dest) {
    return dest ? Destination : Source;
}

/// add i to multiplicity of node (used in parser, otherwise deprecated)
inline void Arc::operator += (unsigned int incr) {
    Multiplicity += incr;
}
