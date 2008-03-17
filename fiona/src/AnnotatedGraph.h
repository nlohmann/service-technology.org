/*****************************************************************************
 * Copyright 2005, 2006, 2007 Jan Bretschneider, Peter Massuthe, Leonard Kern*
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
    public:

        /// Type of container passed to AnnotatedGraph::product().
        typedef std::list<AnnotatedGraph*> ogs_t;

        /// Type of container passed to AnnotatedGraph::getProductOGFilePrefix().
        typedef std::list<std::string> ogfiles_t;

        typedef set<std::string> EdgeSet;

        /// Type of container of transitions associated to each label
        typedef map<string, EdgeSet> TransitionMap;

    protected:
        /// name of the file that was source of the graph
        std::string filename;

        AnnotatedGraphNode* root;

        GraphFormulaCNF* covConstraint;

        typedef std::vector<AnnotatedGraphNode*> nodes_t;
        nodes_t setOfNodes; // needed for proper deletion of OG.

        nodes_t finalNodes; // final nodes of the graph

        typedef nodes_t::const_iterator nodes_const_iterator;
        typedef nodes_t::iterator nodes_iterator;

        /// remove all edges that have a given node as destination
        void removeEdgesToNodeFromAllOtherNodes(const AnnotatedGraphNode* nodeToDelete);

        /// remove all edges that have a given node as source
        void removeEdgesFromNodeToAllOtherNodes(AnnotatedGraphNode* nodeToDelete);

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

// CODE FROM PL
        set<std::string> sendEvents;
        set<std::string> recvEvents;
	
        /// remove a node from the annotated graph
        virtual void removeNode(AnnotatedGraphNode*);
        
        /// remove all nodes that have the annotation "true"
        void removeNodesAnnotatedWithTrue();
        
        /// constructs the dual service
        void constructDualService();
        
        /// applies 1st and 2nd fix to dual service
        void fixDualService();

        /// transforms the public view annotated graph to a non annotated Graph
        void transformOGToService( Graph* cleanPV);
// END OF CODE FROM PL


// CODE FROM CG FOR STRUCTURAL REDUCTION
        /// returns the vector, so that all nodes in the vector have leaving edges to node
        nodes_t getPredecessorNodes(AnnotatedGraphNode * node);

        /// returns the vector, so that node has leaving edges to all nodes in the vector
        nodes_t getSuccessorNodes(AnnotatedGraphNode * node);
// END OD CODE FROM CG


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

    public:

        /// basic constructor
        AnnotatedGraph();

        /// basic deconstructor
        virtual ~AnnotatedGraph();

        /// adds a node to the graph
        void addNode(AnnotatedGraphNode* node);

        /// creates a new node in the graph
        AnnotatedGraphNode* addNode(const std::string& nodeName,
                                    GraphFormula* annotation,
                                    GraphNodeColor color = BLUE);

        /// creates a new edge in the graph
        void addEdge(const std::string& srcName,
                     const std::string& dstName,
                     const std::string& label);

        /// returns true if a node with the given name was found
        bool hasNodeWithName(const std::string& nodeName) const;

        /// returns a pointer to the root node
        AnnotatedGraphNode* getRoot() const;

        /// sets a new root node
        void setRoot(AnnotatedGraphNode* newRoot);

        /// sets the root node to the one with the given name
        void setRootToNodeWithName(const std::string& nodeName);

        /// retruns a node with the given name, or NULL else
        AnnotatedGraphNode* getNodeWithName(const std::string& nodeName) const;

        /// retruns true if the graph's root node is NULL
        bool hasNoRoot() const;

        /// returns a vector containing all nodes that are always false
        void findFalseNodes(std::vector<AnnotatedGraphNode*>* falseNodes);

        /// removes all nodes that are always false
        virtual void removeFalseNodes();

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

        /// checks, whether this AnnotatedGraph simulates the given simulant while covering all interface transitions
        bool covSimulates(AnnotatedGraph* smallerOG);

        /// filters the current OG through a given OG in such a way,
        /// that the filtered current OG simulates the opernad og; 
        void filter(AnnotatedGraph* rhsOG);

        /// computes the number of Services determined by this OG
        unsigned int numberOfServices();

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

// CODE FROM PL
        /// transforms the graph to the public view
        void transformToPublicView(Graph* cleanPV);
// END OF CODE FROM PL

// CODE FROM CG FOR STRUCTURAL REDUCTION
        virtual void reduceStructurally();
// END OD CODE FROM CG

        // Provides user defined operator new. Needed to trace all new operations on this class.
#undef new
        NEW_OPERATOR(AnnotatedGraph)
#define new NEW_NEW
};

#endif
