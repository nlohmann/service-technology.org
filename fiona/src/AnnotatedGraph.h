/*****************************************************************************
 * Copyright 2005, 2006, 2007 Jan Bretschneider, Peter Massuthe, Leonard Kern*
 *                                                                           *
 * Copyright 2008                                                            *
 *   Peter Massuthe, Daniela Weinberg                                        *
 *                                                                           *
 * This file is part of Fiona.                                               *
 *                                                                           *
 * Fiona is free software; you can redistribute it and/or modify it          *
 * under the terms of the GNU General Public License as published by the     *
 * Free Software Foundation; either version 2 of the License, or (at your    *
 * option) any later version.                                                *
 *                                                                           *
 * Fiona is distributed in the hope that it will be useful, but WITHOUT      *
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or     *
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for  *
 * more details.                                                             *
 *                                                                           *
 * You should have received a copy of the GNU General Public License along   *
 * with Fiona; if not, write to the Free Software Foundation, Inc., 51       *
 * Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.                      *
 *****************************************************************************/

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
 *
 * \note    This file is part of the tool Fiona and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universitt zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
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


class AnnotatedGraph : public Graph {

    // Public Typedefs
    public:

        /// Type of container passed to AnnotatedGraph::product().
        typedef std::list<AnnotatedGraph*> ogs_t;

        /// Type of container passed to AnnotatedGraph::getProductOGFilePrefix().
        typedef std::list<std::string> ogfiles_t;

        typedef set<std::string> EdgeSet;

        /// Type of container of transitions associated to each label
        typedef map<string, EdgeSet> TransitionMap;

        /// Type of container of nodes stored in this graph
        typedef std::vector<AnnotatedGraphNode*> nodes_t;

        /// Type of the predecessor map
        typedef std::map< AnnotatedGraphNode*, AnnotatedGraphNode::LeavingEdges > predecessorMap;

    // Protected Methods and typedefs    
    protected:

        /// Iterator types for nodes
        typedef nodes_t::const_iterator nodes_const_iterator;
        typedef nodes_t::iterator nodes_iterator;

        /// name of the file that was source of the graph
        std::string filename;

        /// A pointer to the root node
        AnnotatedGraphNode* root;

        GraphFormulaCNF* covConstraint;

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

        /// checks, whether the part of an AnnotatedGraph below myNode simulates
        /// the part of an AnnotatedGraph below simNode while covering all interface transitions
        bool covSimulatesRecursive(AnnotatedGraphNode* myNode,
                                   AnnotatedGraphNode* simNode,
                                   set<pair<AnnotatedGraphNode*, AnnotatedGraphNode*> >& visitedNodes,
                                   GraphFormulaCNF* myCovConstraint,
                                   GraphFormulaCNF* simCovConstraint);

        /// filters the current OG through a given OG below myNode (rhsNode respectively)
        /// in such a way, that the complete OG given as the operand simulates the current OG
        void filterRecursive(AnnotatedGraphNode* myNode,
                             AnnotatedGraphNode* rhsNode,
                             set<AnnotatedGraphNode*>* VisitedNodes);

        //! Create the formula describing the structure of the subgraph under the given node through events
        //! NOTE: graph has to be acyclic!
        GraphFormulaMultiaryAnd* createStructureFormulaRecursively(AnnotatedGraphNode*);

        set<std::string> sendEvents;
        set<std::string> recvEvents;

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

        /// Public View Generation only:
        /// transforms the public views annotated graph to a non annotated Graph
        void transformOGToService( Graph* cleanPV);

        /// recursive function for computing the number of services
        unsigned int numberOfServicesRecursively(set<AnnotatedGraphNode*> activeNodes,
                                                 map<AnnotatedGraphNode*, unsigned int>& followers,
                                                 map<AnnotatedGraphNode*, list <set<AnnotatedGraphNode*> > >& validFollowerCombinations,
                                                 map<set<AnnotatedGraphNode*>, unsigned int>& eliminateRedundantCounting,
                                                 unsigned int& instances);

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
                                                 std::map<AnnotatedGraphNode*, bool>& visitedNodes, 
                                                 bool onABluePath);

        /// The total number of all states stored in all nodes in this graph.
        /// Is computed by computeNumberOfStatesAndEdges().
        unsigned int nStoredStates;

        /// The number of all edges in this graph.
        /// Is computed by computeNumberOfStatesAndEdges().
        unsigned int nEdges;

        /// The number of blue to be shown nodes in this graph.
        /// Is computed by computeNumberOfBlueNodesEdges().
        unsigned int nBlueNodes;

        /// The number of blue to be shown edges in this graph.
        /// Is computed by computeNumberOfBlueNodesEdges().
        unsigned int nBlueEdges;


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

    // Public methods
    public:

        /// basic constructor
        AnnotatedGraph();

        /// basic deconstructor
        virtual ~AnnotatedGraph();

        /// adds a node to the graph (directly to the node set)
        virtual void addNode(AnnotatedGraphNode* node);

        /// creates a new node in the graph
        AnnotatedGraphNode* addNode(const std::string& nodeName,
                                    GraphFormula* annotation,
                                    GraphNodeColor color = BLUE);

        /// creates a new edge in the graph
        void addEdge(const std::string& srcName,
                     const std::string& dstName,
                     const std::string& label);


        /// creates a new edge in the graph
        void addEdge(AnnotatedGraphNode * src,
                     AnnotatedGraphNode * dst,
                     const std::string& label);

        /// Returns the number of nodes in this graph.
        virtual unsigned int getNumberOfNodes() const;

        /// returns true if a node with the given name was found
        bool hasNodeWithName(const std::string& nodeName) const;

        /// returns a pointer to the root node
        AnnotatedGraphNode* getRoot() const;

        /// sets a new root node
        void setRoot(AnnotatedGraphNode* newRoot);

        /// sets the root node to the one with the given name
        void setRootToNodeWithName(const std::string& nodeName);

        /// returns a node with the given name, or NULL else
        AnnotatedGraphNode* getNodeWithName(const std::string& nodeName) const;

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
        std::string getFilename();

        /// sets the name of the source file for the Graph
        void setFilename(std::string filename);

        /// Returns the product OG of all given OGs.
        static AnnotatedGraph* product(const ogs_t& ogs);

        /// Returns the product OG of this OG and the passed one
        AnnotatedGraph* product(const AnnotatedGraph* rhs);

        /// Creates and returns the annotation for the product node of the given two nodes.
        GraphFormulaCNF* createProductAnnotation(const AnnotatedGraphNode* lhs,
                                                 const AnnotatedGraphNode* rhs) const;

        /// Produces the default prefix of the product OG output file.
        static std::string getProductOGFilePrefix(const ogfiles_t& ogfiles);

        /// Strips the OG file suffix from filename and returns the result.
        static std::string stripOGFileSuffix(const std::string& filename);

        /// dfs through the graph printing each node and edge to the output stream
        void printGraphToDot(AnnotatedGraphNode* v,
                             fstream& os,
                             std::map<AnnotatedGraphNode*, bool>&) const;

        /// creates a dot output of the graph and calls dot to create an image from it
        void printDotFile(const std::string& filenamePrefix) const;

        /// creates a dot output of the graph and calls dot to create an image from it
        void printDotFile(const std::string& filenamePrefix,
                          const std::string& dotGraphTitle) const;

        /// Prints this OG in OG file format
        void printOGFile(const std::string& filenamePrefix) const;

        /// Adds the suffix for OG files to the given file name prefix
        static std::string addOGFileSuffix(const std::string& filePrefix);

        /// checks, whether this AnnotatedGraph simulates the given simulant
        bool simulates(AnnotatedGraph* smallerOG);

        /// checks, whether this AnnotatedGraph is equivalent to the secondOG
        bool isEquivalent(AnnotatedGraph* secondOG);

        /// checks, whether two nodes of this AnnotatedGraph are equivalent
        bool isEquivalent(AnnotatedGraphNode* leftNode,
                          AnnotatedGraphNode* rightNode);

        /// checks, whether this AnnotatedGraph simulates the given simulant while covering all interface transitions
        bool covSimulates(AnnotatedGraph* smallerOG);

        /// filters the current OG through a given OG in such a way,
        /// that the filtered current OG simulates the opernad og;
        void filter(AnnotatedGraph* rhsOG);

        /// computes the number of Services determined by this OG
        unsigned int numberOfServices();

        /// Minimizes the graph
        void minimizeGraph();

        /// tests if this OG is acyclic
        bool isAcyclic();

        /// a function needed for successful deletion of the graph
        void clearNodeSet();

        /// Get all transitions from the graph, each associated to a specific label
        TransitionMap getTransitionMap();

        /// Get all transitions from the graph with a label from the given set, each associated to a specific label
        /// NULL refers to covering the whole interface set
        TransitionMap getTransitionMap(set<string>* labels);

        /// Create the formula describing the coverability criteria when covering labels in the given set.
        void createCovConstraint(set<string>* labels = NULL);

        //! Create the formula describing the structure of the complete graph through events
        //! NOTE: graph has to be acyclic!
        GraphFormulaMultiaryAnd* createStructureFormula();

        /// Computes and prints the statistics for this graph.
        void computeAndPrintGraphStatistics();

        /// Computes statistics about this graph
        void computeGraphStatistics();

        /// Prints statistics about this graph. May only be called after
        /// computeGraphStatistics().
        void printGraphStatistics();

// CODE FROM PL
        /// transforms the graph to the public view
        void transformToPublicView(Graph* cleanPV, bool fromOWFN);
// END OF CODE FROM PL

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
