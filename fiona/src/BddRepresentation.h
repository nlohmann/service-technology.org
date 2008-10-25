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
 * \file    BddRepresentation.h
 *
 * \brief   functions for representing an OG as a BDD
 *
 * \author  responsible: Kathrin Kaschner <kathrin.kaschner@informatik.uni-rostock.de>
 */

#ifndef BDDREPRESENTATION_H_
#define BDDREPRESENTATION_H_

#include "mynew.h"
#include <vector>
#include <map>
#include <list>

#include "AnnotatedGraphNode.h"

#include "util.h"
#include "cudd.h"
#include "cuddInt.h"
#include "dddmp.h"

using namespace std;
typedef vector<bool> BitVector;

class BddLabelTab;


class BddRepresentation {
    public:
        BddRepresentation(unsigned int numberOfLabels, Cudd_ReorderingType heuristic = CUDD_REORDER_SIFT);
        ~BddRepresentation();

        void convertRootNode(AnnotatedGraphNode* root);
        void generateRepresentation(AnnotatedGraphNode* v, std::map<AnnotatedGraphNode*, bool>&);
        void addOrDeleteLeavingEdges(AnnotatedGraphNode* v);
        void reorder(Cudd_ReorderingType heuristic = CUDD_REORDER_SIFT);
        void printDotFile(char** names = NULL, char* option = "OG");
        void print();
        void save(char const* option = "OG");
        void printMemoryInUse();

        // Provides user defined operator new. Needed to trace all new
        // operations on this class.
#undef new
        NEW_OPERATOR(BddRepresentation)
#define new NEW_NEW

     private:
        DdManager* mgrMp;
        DdManager* mgrAnn;
        DdNode* bddMp;
        DdNode* bddRed;
        DdNode* bddAnn;
        int maxNodeBits;
        int maxLabelBits;
        unsigned int maxNodeNumber;
        int nbrLabels;
        map<unsigned int, unsigned int> nodeMap;
        BddLabelTab * labelTable;

        /**
         * Returns the BDD of a label (given as integer).
         */
        DdNode* labelToBddMp(const std::string& label);

        DdNode* nodesToBddMp(unsigned int node1, unsigned int node2);
        DdNode* annotationToBddAnn(AnnotatedGraphNode* v);
        DdNode* clauseToBddAnn(const GraphFormulaMultiaryOr* cl);
        unsigned int getBddNumber(unsigned int node);
        void addBddVars(unsigned int max);
        BitVector numberToBin(unsigned int number, int cntBits);
        int nbrBits(unsigned int i);
        string myitoa(unsigned int value, int base);
        void checkManager(DdManager* mgr, char* table);

};

#endif /*BDDREPRESENTATION_H_*/
