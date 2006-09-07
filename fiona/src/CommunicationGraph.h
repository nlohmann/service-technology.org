/*!
 *  \class communicationGraph
    \brief abstract class for the generation of a reachability graph of oWFNs

    \author Daniela Weinberg
    \par History
        - 2005-09-07 creation
        - 2005-12-03 added comments
 */

#ifndef GRAPH_H_
#define GRAPH_H_

#include "mynew.h"
#include "vertex.h"
#include "enums.h"
#include <string>
#include <fstream>
#include <iostream>

#include <set>

using namespace std;

class oWFN;
class State;

struct compareVertices {
  bool operator() (vertex const * left, vertex const * right) {
    return (*left < *right);
  }
}; // compareVertices

typedef std::multiset<unsigned int> messageMultiSet;

struct compareMessageMultiSets {
  bool operator() (messageMultiSet const left, messageMultiSet const right) {
    if (left.size() < right.size()) {
        return true;
    }
    if (left.size() > right.size()) {
        return false;
    }
    for (std::multiset<unsigned int>::iterator s1 = left.begin(); s1 != left.end(); s1++) {
        for (std::multiset<unsigned int>::iterator s2 = right.begin(); s2 != right.end(); s2++) {
            if (*s1 < *s2) {
                return true;
            }
            if (*s1 > *s2) {
                return false;
            }
        }
    }
    return false;
  }
}; // compareMessageMultiSets

typedef std::set<vertex*, compareVertices> vertexSet;

typedef std::set<messageMultiSet, compareMessageMultiSets> setOfMessages;


/* reachability graph */
class communicationGraph {
protected:
   vertex * currentVertex;          //!< the vertex we are working on at a certain time
   vertex * root;                   //!< the root node of the graph

   int numberOfVertices;            //!< the number of vertices of the graph

   unsigned int numberBlackNodes;   //!< number of black nodes in the graph
   unsigned int numberBlueNodes;    //!< number of blue nodes in the graph
   unsigned int numberBlueEdges;    //!< number of blue edges in the graph

   oWFN * PN;                   	//!< pointer to the underlying petri net

   int actualDepth; 				//!< actual depth in graph

  
   unsigned int numberOfStatesAllNodes;

public:
    communicationGraph (oWFN *);
    ~communicationGraph ();

   int numberOfEdges;               //!< the number of edges of the graph


   vertexSet setOfVertices;

    void calculateRootNode();

    vertex * getRoot() const;
    
    unsigned int getNumberOfEdges () const;
    unsigned int getNumberOfVertices () const;

    vertex * findVertexInSet(vertex *);

    int AddVertex (vertex *, unsigned int, edgeType);
    int AddVertex (vertex *, messageMultiSet, edgeType);

    void buildGraphRandom();

    bool calculateSuccStatesInput(unsigned int, vertex *, vertex *);
    void calculateSuccStatesOutput(unsigned int, vertex *, vertex *);

    void calculateSuccStatesInput(messageMultiSet, vertex *, vertex *);
    void calculateSuccStatesOutput(messageMultiSet, vertex *, vertex *);

    void printGraphToDot(vertex * v, fstream& os, bool[]);
    void printDotFile();

    bool stateActivatesOutputEvents(State *);

    analysisResult analyseNode(vertex *, bool);
};

#endif /*GRAPH_H_*/
