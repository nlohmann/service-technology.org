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
 * \file    ConstraintOG.h
 *
 * \brief   Holds an operating guideline with global constraint that is read
 *          from an .covog file.
 *          An OG with global constraint extends an OG by a global constraint. 
 *          It restricts the set of strategies characterized by the underlying
 *          operating guideline to those strategies that satisfy the constraint.
 *          OGs with global constraint can be used to cover places and 
 *          transitions of an oWFN. That means, in the composition of a 
 *          strategy and the oWFN there exists a run such that a place 
 *          (transition) to be covered can be marked (may fire).
 *          The notion of an OG ordinary OGs by the following functionality: 
 *          	- it is stored which oWFN nodes have to be covered
 *          	- a (global) constraint in CNF
 *          	- for each OG node, we store the set of places and transitions 
 *          	of the nodes' knowledge. 
 *          In addition an output format for OGs with global constraint has 
 *          been added which can be used to output the OG, the constraint and 
 *          the set of oWFN nodes that are covered in each OG node.
 *
 * \author  responsible: Robert Danitz <danitz@informatik.hu-berlin.de>
 *
 */

#ifndef CONSTRAINTOG_H_
#define CONSTRAINTOG_H_

#include "OG.h"

#undef TRUE
#undef FALSE

using namespace std;


class ConstraintOG : public OG {
    public:
        /// stores the names of the oWFN transitions to be covered 
        set<string> transitionsToCover;

        /// stores the names of the oWFN places to be covered
        /// (internal and external)
        set<string> placesToCover;

        /// associates OG nodes to the set of all markable oWFN places
        map<AnnotatedGraphNode*, set<string> > knownPlaces;

        /// associates OG nodes to the set of all fireable oWFN transitions 
        map<AnnotatedGraphNode*, set<string> > knownTransitions;

    private:

        /// constraint in CNF, describing which OG nodes have to be covered
        /// in order to meet the requested coverage of both oWFN places
        /// and transitions
        GraphFormulaCNF* covConstraint;

        /// dfs through the graph printing each node and edge 
        /// to the output stream
        virtual void createDotFileRecursively(
                AnnotatedGraphNode* v,
                fstream& os,
                std::map<AnnotatedGraphNode*, bool>& visitedNodes) const; 
    public:

        /// constructor
        ConstraintOG(oWFN *);

        /// destructor
        ~ConstraintOG();

        /// Builds the ConstraintOG of the associated PN
        virtual void buildGraph();
                
        /// reads the places and transitions to cover from the given file
        void readCovConstraint(const string& filename);

        /// computes the coverability formula in CNF and stores it
        void computeCovConstraint();

        /// delivers the coverability constraint 
        GraphFormulaCNF* getCovConstraint() const;
        
        /// Print this ConstraintOG in ConstraintOG file format (.covog)
        /// to the file with the given prefix.
        virtual string createOGFile(const std::string& filenamePrefix, 
                                    bool hasOWFN) const;

        /// Adds the suffix for OG files to the given file name prefix
        static std::string addOGFileSuffix(const std::string& filePrefix);

        /// computes which oWFN places and transitions cannot be covered
        set<string> computeUncoverables();
};

#endif

