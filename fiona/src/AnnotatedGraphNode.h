/*****************************************************************************
 * Copyright 2005, 2006, 2007 Peter Massuthe, Daniela Weinberg,              *
 *           Jan Bretschneider, Niels Lohmann, Leonard Kern                  *
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
 * \file    AnnotatedGraphNode.h
 *
 * \brief   functions for handling of nodes of IG / OG
 *
 * \author  responsible: Daniela Weinberg <weinberg@informatik.hu-berlin.de>
 *
 * \note    This file is part of the tool Fiona and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universitt zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 */

#ifndef AnnotatedGraphNode_H_
#define AnnotatedGraphNode_H_

#include "state.h"
#include "SinglyLinkedList.h"
#include "GraphFormula.h"
#include "AnnotatedGraphEdge.h"
#include "GraphNode.h"
#include "mynew.h"
#include "debug.h"
#include "options.h"
#include <set>
#include <cassert>


typedef std::set<State*> StateSet;

using namespace std;

/*******************
* class AnnotatedGraphNode *
*******************/

class AnnotatedGraphNode : public GraphNode {

    public:

        /// Type of the container that holds all leaving edges of this GraphNode.
        typedef SList<AnnotatedGraphEdge*> LeavingEdges;

    private:

        /// Annotation of this node as a formula (a CNF).
        GraphFormulaCNF* annotation;

        /// Contains all leaving edges.
        LeavingEdges leavingEdges;

        /// Annotation of this node as a formula in CNF used for Coverability
        GraphFormulaCNF* covAnnotation;

    public:

        /// constructor (no parameters)
        AnnotatedGraphNode();

        /// constructor (four parameters)
        AnnotatedGraphNode(const std::string& _name,
                           GraphFormula* _annotation,
                           GraphNodeColor _color = BLUE,
                           unsigned int number = 12345678);

        /// Destroys this AnnotatedGraphNode.
        virtual ~AnnotatedGraphNode();

        /// get the annotation
        GraphFormulaCNF* getAnnotation() const;

        /// get annotation of this node in CNF for coverability uses
        GraphFormulaCNF* getCovAnnotation() const;

        /// set the annotation
        void setAnnotation(GraphFormulaCNF*);

        /// get the annotation as a string
        std::string getAnnotationAsString() const;

        /// Adds a leaving edge to this node.
        void addLeavingEdge(AnnotatedGraphEdge* edge);

        /// Returns an iterator for this node's leaving edges.
        LeavingEdges::Iterator getLeavingEdgesIterator();

        /// Returns a constant iterator for this node's leaving edges.
        LeavingEdges::ConstIterator getLeavingEdgesConstIterator() const;

        /// Returns the number of leaving edges.
        unsigned int getLeavingEdgesCount() const;

        /// returns true iff a colored successor of v can be avoided
        bool coloredSuccessorsAvoidable(GraphNodeDiagnosisColor_enum color) const;

        /// returns true iff edge e is possible in every state
        bool edgeEnforcable(AnnotatedGraphEdge* e) const;

        /// returns true iff e changes the color of the common successors
        bool changes_color(AnnotatedGraphEdge* e) const;

        /// determines whether this node has a edge with the given label
        bool hasEdgeWithLabel(const std::string&) const;

        /// determines whether this node has a blue edge with a given label
        bool hasBlueEdgeWithLabel(const std::string&) const;

        /// returns a edge of this node with the given label
        AnnotatedGraphEdge* getEdgeWithLabel(const std::string&) const;

        /// returns the destination node of the edge with the given label
        AnnotatedGraphNode* followEdgeWithLabel(const std::string&);

        /// removes all edge from this node to the given one
        void removeEdgesToNode(const AnnotatedGraphNode*);

        /// adds a new clause to the CNF formula of the node
        void addClause(GraphFormulaMultiaryOr*);

        /// analyses the node and sets its color
        void analyseNode();

        /// removes the given literal from this node's annotation
        void removeLiteralFromAnnotation(const std::string& literal);

        /// remove the given literal from this node's annotation
        /// in that way that the literal is absorbed
        void removeLiteralFromAnnotationByHiding(const std::string& literal);

        /// Removes unneeded literals from the node's annotation. Labels of
        /// edges to red nodes are unneeded.
        void removeUnneededLiteralsFromAnnotation();

        /// returns true iff node should be shown according to the "show" parameter
        bool isToShow(const AnnotatedGraphNode* rootOfGraph) const;

        /// determines whether an assinment satisfies this node's annotation
        bool assignmentSatisfiesAnnotation(const GraphFormulaAssignment&) const;

        /// returns the assignment that is imposed by present or absent arcs
        /// leaving the node
        GraphFormulaAssignment* getAssignment() const;

        /// create the annotation in CNF for coverability uses
        /// through a given global constraint, describing the coverability criteria
        void createCovAnnotation(GraphFormula* global_constraint);

#undef new
        /// Provides user defined operator new. Needed to trace all new operations
        /// on this class.
        NEW_OPERATOR(AnnotatedGraphNode)
#define new NEW_NEW
};


class PriorityMap {
    public:

        typedef owfnPlace* KeyType;

        /// Fills the priority map according to the given annotation with interface places.
        /// NOTE: All interface places will be considered; places not in the 
        /// annotation will have a minimal priority.
        /// \param annotation the annotation, from which the priority map will be extracted. 
        void fill(oWFN * PN, GraphFormulaCNF *annotation);
        void fillForIG(setOfMessages&, oWFN*, GraphFormulaCNF*);

        /// Delivers the element from the priority map with the highest priority.
        /// This element will be removed afterwards.
        KeyType pop();

        /// DESCRIPTION
        messageMultiSet popIG();

        /// Returns true iff the priority map is empty.
        bool empty() const;

        /// DESCRIPTION
        bool emptyIG() const;

    private:

        /// Type of priority map.
        /// The first element of number represents the minimal length of a clause containing the key element.
        /// The second element of number represents the maximal occurence of the key element throughout the annotation.
        typedef map<KeyType, pair<int, int> > MapType;

        /// Type of priority map for the IG.
        /// as the first element of the map we use a multiset of messages
        /// The first element of int represents the minimal length of a clause containing the key element.
        /// The second element of int represents the maximal occurence of the key element throughout the annotation. 
        typedef map<messageMultiSet, pair<int, int> > MapTypeIG;

        /// Underlying representation of association between interface places and their priority.
        MapType pm;

        /// Underlying representation of association between a set of messages and their priority.
        MapTypeIG pmIG;
};


#endif /*AnnotatedGraphNode_H_*/
