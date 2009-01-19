/*****************************************************************************\
 Copyright (c) 2008  Robert Danitz, Christian Gierds, Niels Lohmann,
                     Peter Massuthe, Richard Mu"ller, Janine Ott,
                     Jan Su"rmeli, Daniela Weinberg, Karsten Wolf,
                     Martin Znamirowski

 Copyright (c) 2007  Jan Bretschneider, Robert Danitz, Christian Gierds,
                     Kathrin Kaschner, Leonard Kern, Niels Lohmann,
                     Peter Massuthe, Daniela Weinberg, Karsten Wolf

 Copyright (c) 2006  Kathrin Kaschner, Peter Massuthe, Daniela Weinberg,
                     Karsten Wolf

 This file is part of Fiona, see <http://service-technology.org/fiona>.

 Fiona is free software; you can redistribute it and/or modify it under the
 terms of the GNU General Public License as published by the Free Software
 Foundation; either version 3 of the License, or (at your option) any later
 version.

 Fiona is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 A PARTICULAR PURPOSE. See the GNU General Public License for more details.

 You should have received a copy of the GNU General Public License along with
 Fiona (see file COPYING). If not, see <http://www.gnu.org/licenses/>.
\*****************************************************************************/

/*!
 * \file    AnnotatedGraph.h
 *
 * \brief   Holds an Operating Guidelines (OG) read from a OG file. We do not
 *          use the class OG for storing OGs read from a file because the class
 *          OG is tailored for computing an OG and therefore heavily depends
 *          on the existence of an underlying oWFN. If we used the class OG for
 *          storing an already computed OG, we would have to reimplement most
 *          methods anyway and the possibility of reuse would be minimal.
 *
 * \author  responsible: Jan Bretschneider <bretschn@informatik.hu-berlin.de>
 */

#ifndef ANNOTATEDGRAPH_H_
#define ANNOTATEDGRAPH_H_

#include <fstream>
#include <string>
#include <set>
#include <utility>
#include <map>
#include <list>
#include "AnnotatedGraphNode.h"
#include "Graph.h"

//using namespace std;

/// when counting nodes/edges/states and blue nodes/blue edges
/// we need to remember that we have visited a node already and if
/// we have counted the node as a blue node or not
/// (we may reach a blue node from a red path and later on from a blue path!)
enum typeOfVisit {
	VISITED_COUNTED_AS_BLUE,	/// node has been visited and counted as blue
	VISITED_NOT_COUNTED			/// node has been visited but not (yet) counted
};

class AnnotatedGraph : public Graph {

	// private enumeration
	private:

		typedef typeOfVisit t_typeOfVisit;

    // Public Typedefs
    public:

        /// Type of container passed to AnnotatedGraph::product().
        typedef std::list<AnnotatedGraph*> ogs_t;

        /// Type of container passed to AnnotatedGraph::getProductOGFilePrefix().
        typedef std::list<string> ogfiles_t;

        /// Type of container of nodes stored in this graph
        typedef std::vector<AnnotatedGraphNode*> nodes_t;

        /// Type of the predecessor map
        typedef std::map< AnnotatedGraphNode*, AnnotatedGraphNode::LeavingEdges > predecessorMap;

        string getSuffix() const;

    // Protected Methods and typedefs
    protected:

        /// Iterator types for nodes
        typedef nodes_t::const_iterator nodes_const_iterator;
        typedef nodes_t::iterator nodes_iterator;

        /// name of the file that was source of the graph
        string filename;

        /// A pointer to the root node
        AnnotatedGraphNode* root;

        /// The nodes stored in this graph. Not a set on this level but a vector.
        nodes_t setOfNodes; // needed for proper deletion of OG.

        /// remove all edges that have a given node as destination
        void removeEdgesToNodeFromAllOtherNodes(const AnnotatedGraphNode* nodeToDelete);

        /// remove all edges that have a given node as source
        void removeEdgesFromNodeToAllOtherNodes(AnnotatedGraphNode* nodeToDelete);

        /// assign the final nodes of the graph according to Gierds 2007
        void assignFinalNodes();

        /// Recursive coordinated dfs through OG and rhs OG.
        void buildProductOG(AnnotatedGraphNode* currentOGNode,
                            AnnotatedGraphNode* currentRhsNode,
                            AnnotatedGraph* productOG);

        /// checks, whether the part of an AnnotatedGraph below myNode simulates
        /// the part of an AnnotatedGraph below simNode
        bool simulatesRecursive(AnnotatedGraphNode* myNode,
                                AnnotatedGraphNode* simNode,
                                set<pair<AnnotatedGraphNode*, AnnotatedGraphNode*> >& visitedNodes,
                                AnnotatedGraph* greaterOG,
                                AnnotatedGraph* smallerOG);

        /// checks, whether the part of an AnnotatedGraph below leftNode is equivalent
        /// to the part of an AnnotatedGraph below rightNode
        bool isEquivalentRecursive(AnnotatedGraphNode* leftNode,
                                   AnnotatedGraphNode* rightNode,
                                   set<pair<AnnotatedGraphNode*, AnnotatedGraphNode*> >& visitedNodes,
                                   AnnotatedGraph* leftOG,
                                   AnnotatedGraph* rightOG);

        set<string> sendEvents;
        set<string> recvEvents;

        /// remove a node from the annotated graph
        virtual void removeNode(AnnotatedGraphNode*);

        /// Public View Generation only:
        /// constructs the dual service, by transforming all sending
        /// events into receiving events and vice versa
        void constructDualService();

        /// Public View Generation only:
        /// removed the emoty node only in case, that this og was created
        /// from an owfn. In case that the OG was read from a file, all
        /// true annotated nodes will be removed
        void removeTrueNodes(bool fromOWFN);

        /// Public View Generation only:
        /// applies various fixes to the dual service to ensure
        /// equivalence of the public view's and the original process' OGs
        void fixDualService(bool fromOWFN);


public:
// CODE FROM PL
        /// Public View Generation only:
        /// Transforms this OG to a Public View.
        /// WARNING: This directly changes the object itself, no copy is made.
        void transformToPV(bool fromOWFN);
// END OF CODE FROM PL


        /// Makes a deep copy of the object into another AnnotatedGraph object.
        void toAnnotatedGraph(AnnotatedGraph* destination);

        /// Makes a deep copy of the object into a Graph object, losing
        /// additional information like annotations.
        void toGraph(Graph* destination);

        // END: NEW PUBLIC VIEW METHODS

protected:
        /// recursive function for computing the number of matching tree service automata
        long double numberOfStrategiesRecursively(AnnotatedGraphNode* activeNode,
                                                 map<AnnotatedGraphNode*, list <list<AnnotatedGraphNode*> > >& validFollowerCombinations,
                                                 map<AnnotatedGraphNode*, long double>& eliminateRedundantCounting);

        /// recursive function for computing the number of true assignments for a node
        unsigned int processAssignmentsRecursively(set<string> labels,
                                                   GraphFormulaAssignment possibleAssignment,
                                                   AnnotatedGraphNode* testNode,
                                                   list<GraphFormulaAssignment>& assignmentList);



        /// Computes the total number of all states stored in all nodes and the
        /// number of all nodes and edges in this graph. Furthermore computes the
        /// number of blue nodes and edges.
        /// Uses computeNumberOfNodesAndStatesAndEdgesHelper to do a dfs.
        void computeNumberOfNodesAndStatesAndEdges();

        /// Helps computeNumberOfStatesAndEdges to computes the total number of all
        /// states stored in all nodes and the number of all nodes and edges in this graph.
        /// Furthermore computes the number of blue nodes and edges.
        void computeNumberOfNodesAndStatesAndEdgesHelper(AnnotatedGraphNode* v,
                                                 std::map<AnnotatedGraphNode*, t_typeOfVisit>& visitedNodes,
                                                 bool onABluePath);

        /// The total number of all states stored in all nodes in this graph.
        /// Is computed by computeNumberOfStatesAndEdges().
        unsigned int nStoredStates;

        /// The total number of all states stored in blue nodes in this graph.
        /// Is computed by computeNumberOfStatesAndEdges().
        unsigned int nStoredStatesBlueNodes;

        /// The number of all edges in this graph.
        /// Is computed by computeNumberOfStatesAndEdges().
        unsigned int nEdges;

        /// The number of blue to be shown nodes in this graph.
        /// Is computed by computeNumberOfBlueNodesEdges().
        unsigned int nBlueNodes;

        /// The number of blue to be shown edges in this graph.
        /// Is computed by computeNumberOfBlueNodesEdges().
        unsigned int nBlueEdges;

        /// Returns the total number of states stored in blue nodes in this
        /// graph. May only be called after computeGraphStatistics().
        unsigned int getNumberOfStoredStatesBlueNodes() const;

        /// Returns the total number of all states stored in all nodes in this
        /// graph. May only be called after computeGraphStatistics().
        unsigned int getNumberOfStoredStates() const;

        /// Returns the number of all edges in this graph. May only be called after
        /// computeGraphStatistics().
        unsigned int getNumberOfEdges() const;

        /// Returns the number of all blue to be shown nodes in this graph. May only
        /// be called after computeGraphStatistics().
        unsigned int getNumberOfBlueNodes() const;

        /// Returns the number of all blue to be shown edges in this graph. May only
        /// be called after computeGraphStatistics().
        unsigned int getNumberOfBlueEdges() const;

		/// recursive helper function to printGraphToSTG()
		void printGraphToSTGRecursively(AnnotatedGraphNode * v,
									   ostringstream & os,
									   std::map<AnnotatedGraphNode*, bool> & visitedNodes,
									   std::vector<string> & edgeLabels);

        /// recursive helper function to computeInterfaceFromGraph()
        void computeInterfaceRecursively(AnnotatedGraphNode* v,
                                         std::map<AnnotatedGraphNode*, bool>& visitedNodes);

        /// dfs through the graph printing each node and edge to the output stream
        virtual void createDotFileRecursively(
                AnnotatedGraphNode* v,
                fstream& os,
                std::map<AnnotatedGraphNode*, bool>&) const;


    // Public methods
    public:

        /// basic constructor
        AnnotatedGraph();

        /// basic deconstructor
        virtual ~AnnotatedGraph();

        /// stores interface information from associated oWFN(s)
        set<string> inputPlacenames;
        set<string> outputPlacenames;

        /// computes above placenames from the graph
        void computeInterfaceFromGraph();

        /// adds a node to the graph (directly to the node set)
        virtual void addNode(AnnotatedGraphNode* node);

        /// creates a new node in the graph
        AnnotatedGraphNode* addNode(const string& nodeName,
                                    GraphFormula* annotation,
                                    GraphNodeColor color = BLUE);

        /// creates a new edge in the graph
        void addEdge(const string& srcName,
                     const string& dstName,
                     const string& label);


        /// creates a new edge in the graph
        void addEdge(AnnotatedGraphNode * src,
                     AnnotatedGraphNode * dst,
                     const string& label);

        /// Returns the number of nodes in this graph.
        virtual unsigned int getNumberOfNodes() const;

        /// returns true if a node with the given name was found
        bool hasNodeWithName(const string& nodeName) const;

        /// returns a pointer to the root node
        AnnotatedGraphNode* getRoot() const;

        /// sets a new root node
        void setRoot(AnnotatedGraphNode* newRoot);

        /// sets the root node to the one with the given name
        void setRootToNodeWithName(const string& nodeName);

        /// returns a node with the given name, or NULL else
        AnnotatedGraphNode* getNodeWithName(const string& nodeName) const;

        /// retruns true if the graph's root node is NULL
        bool hasNoRoot() const;

        /// returns a vector containing all nodes that are always false
        void findFalseNodes(std::vector<AnnotatedGraphNode*>* falseNodes);

        /// removes all nodes that are always false
        virtual void removeReachableFalseNodes();

        /// removes all nodes that are always false
        virtual void removeReachableFalseNodesRecursively(set<AnnotatedGraphNode*>& candidates, map<AnnotatedGraphNode*, set<AnnotatedGraphNode*> >& parentNodes);

        /// removes all nodes that are always false
        virtual void removeReachableFalseNodesInit(AnnotatedGraphNode* currentNode, set<AnnotatedGraphNode*>& redNodes, map<AnnotatedGraphNode*, set<AnnotatedGraphNode*> >& parentNodes, set<AnnotatedGraphNode*>& visitedNodes);

        /// removes all nodes that have been disconnected from the root
        /// node due to other node removals
        void removeUnreachableNodes();

        /// deletes all visited nodes from the set of assumedly unreachable nodes
        void removeUnreachableNodesRecursively(AnnotatedGraphNode* currentNode,
                                                set<AnnotatedGraphNode*>& unreachableNodes);

        /// returns the name of the source file for the Graph
        string getFilename();

        /// sets the name of the source file for the Graph
        void setFilename(string filename);

        /// Returns the product OG of all given OGs.
        static AnnotatedGraph* product(const ogs_t& ogs);

        /// Returns the product OG of this OG and the passed one
        AnnotatedGraph* product(const AnnotatedGraph* rhs);

        /// Creates and returns the annotation for the product node of the given two nodes.
        GraphFormulaCNF* createProductAnnotation(const AnnotatedGraphNode* lhs,
                                                 const AnnotatedGraphNode* rhs) const;

        /// Produces the default prefix of the product OG output file.
        static string getProductOGFilePrefix(const ogfiles_t& ogfiles);

        /// Strips the OG file suffix from filename and returns the result.
        static string stripOGFileSuffix(const std::string& filename);

        /// Creates a dot output (.out) of the graph, using the filename as title.
        virtual string createDotFile(string& filenamePrefix) const;

        /// Creates a dot output (.out) of the graph, using a specified title.
        virtual string createDotFile(string& filenamePrefix,
                                     const string& dotGraphTitle) const;

        /// Creates an image output (.png) of the graph by calling dot.
        virtual string createPNGFile(string& filenamePrefix,
                                     string& dotFileName) const;

        /// Creates an annotated dot output (.dot) of the graph by calling dot.
        virtual string createAnnotatedDotFile(string& filenamePrefix,
                                              string& dotFileName) const;


        /// Creates an og output file (.og) of the graph.
        virtual string createOGFile(const string& filenamePrefix, bool hasOWFN) const;

        /// Adds the suffix for OG files to the given file name prefix,
        /// simply adds ".og" to the given string.
        static string addOGFileSuffix(const std::string& filePrefix);

        /// checks, whether this AnnotatedGraph simulates the given simulant
        bool simulates(AnnotatedGraph* smallerOG);

        /// checks, whether this AnnotatedGraph is equivalent to the secondOG
        bool isEquivalent(AnnotatedGraph* secondOG);

        /// checks, whether two nodes of this AnnotatedGraph are equivalent
        bool isEquivalent(AnnotatedGraphNode* leftNode,
                          AnnotatedGraphNode* rightNode);

        /// computes the number of matching tree service automata for this OG
        long double numberOfStrategies();

        /// Minimizes the graph
        void minimizeGraph();

        /// tests if this OG is acyclic
        bool isAcyclic();

        /// a function needed for successful deletion of the graph
        void clearNodeSet();

        /// Computes and prints the statistics for this graph.
        void computeAndPrintGraphStatistics();

        /// Computes statistics about this graph
        void computeGraphStatistics();

        /// Prints statistics about this graph. May only be called after
        /// computeGraphStatistics().
        void printGraphStatistics();

		/// creates a STG file of the graph
		string printGraphToSTG(vector<string>& edgeLabels);


// CODE FROM CG FOR STRUCTURAL REDUCTION
        virtual void reduceStructurally();

        /// function that returns a map from nodes to a vector of nodes
        /// so that every node in the vector is a predecessor of the
        /// corresponding node
        void getPredecessorRelation(AnnotatedGraph::predecessorMap& resultMap);

// END OD CODE FROM CG

// Provides user defined operator new. Needed to trace all new operations on this class.
#undef new
        NEW_OPERATOR(AnnotatedGraph)
#define new NEW_NEW
};

#endif
