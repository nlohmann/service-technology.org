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
 * \file    CommunicationGraph.h
 *
 * \brief   common functions for IG / OG
 *
 * \author  responsible: Daniela Weinberg <weinberg@informatik.hu-berlin.de>
 */

#ifndef COMMUNICATIONGRAPH_H_
#define COMMUNICATIONGRAPH_H_

#include "mynew.h"
#include "AnnotatedGraphEdge.h"
#include "GraphNode.h" // for GraphNodeDiagnosisColor_enum
#include "AnnotatedGraphNode.h"
#include "AnnotatedGraph.h" // parent class
#include "owfn.h"
#include "binDecision.h"
#include <fstream>
#include <set>

using namespace std;


extern double global_progress;
extern int show_progress;

/* communication graph */
class CommunicationGraph : public AnnotatedGraph {
    private:

        /// recursive helper function for diagnose()
        GraphNodeDiagnosisColor_enum diagnose_recursively(AnnotatedGraphNode* v,
                                                          std::map<AnnotatedGraphNode*, bool>& visitedNodes);

        /// Recursive helper for createDotFile; runs a dfs through the graph printing each node and edge to the output stream
        void createDotFileRecursively(AnnotatedGraphNode* v,
                                      fstream& os,
                                      std::map<AnnotatedGraphNode*, bool>&) const;

        /// recursive helper function for annotateGraphDistributedly()
        bool annotateGraphDistributedlyRecursively(AnnotatedGraphNode* v,
                                                   std::map<AnnotatedGraphNode*, bool>&);

        /// helper function for annotateGraphDistributedly()
        void removeLabeledSuccessor(AnnotatedGraphNode* v, string label);


    protected:

        /// Pointer to the underlying petri net.
        oWFN* PN;

        /// store temporarily calculated states in binDecision structure
        binDecision * tempBinDecision;

        /// this set contains all states of the newly calculated node
        StateSet setOfStatesStubbornTemp;

        /// A sorted set of the nodes in the graph. Should always contain the same nodes as the vector, though iteration order can differ.
        GraphNodeSet setOfSortedNodes;

        void addProgress(double);

    public:

        /// basic constructor
        CommunicationGraph(oWFN *);

        /// basic destructor
        virtual ~CommunicationGraph();



        /// Returns the number of nodes in this graph.
        virtual unsigned int getNumberOfNodes() const;

        /// Adds a node to the CommunicationGraph. The node is inserted in both sets.
        virtual void addNode(AnnotatedGraphNode*);

        /// remove a node from the annotated graph
        virtual void removeNode(AnnotatedGraphNode*);

        /// checks if the given node is in the graphs node set
        AnnotatedGraphNode* findGraphNodeInSet(AnnotatedGraphNode*);



        /// calculate a root node for the graph
        void calculateRootNode();

        /// function to annotate the OG for distributed controllability
        bool annotateGraphDistributedly();

        /// gives a diagnosis of the graph
        void diagnose();

        /// checks wether a state activates output events
        bool stateActivatesOutputEvents(State*);

        /// prints the current global progress value depending whether the value
        /// changed significantly and depending on the debug-level set
        void printProgressFirst();

        /// prints the current global progress value depending whether the value
        /// changed significantly and depending on the debug-level set
        void printProgress();

        /// Creates a dot output (.out) of the graph, using the filename as title. 
        virtual string createDotFile(string& filenamePrefix) const;

        /// Creates an image output (.png) of the graph by calling dot.
        virtual string createPNGFile(string& filenamePrefix,
                                    string& dotFileName) const;

        /// Creates an annotated dot output (.dot) of the graph by calling dot.
        virtual string createAnnotatedDotFile(string& filenamePrefix,
                                            string& dotFileName) const;
   

        /// fills the given sets with the names of the inputs and outputs
        /// needed for public view generation. The interfaces of the public view
        /// and the original netmust be the same, even if some output/inputs are 
        /// not used
        void returnInterface(set<string>& inputs, set<string>& outputs);

        /// deletes the corresponding oWFN
        void deleteOWFN();

        /// Neither implemented nor used somewhere:
        void buildGraphRandom();


        // Provides user defined operator new. Needed to trace all new operations on this class.
#undef new
        NEW_OPERATOR(CommunicationGraph)
#define new NEW_NEW
};

#endif /*GRAPH_H_*/
