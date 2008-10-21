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
 * \file    Exchangeability.h
 *
 * \brief   functions for comparing two BDD-represented OGs for equality
 *
 * \author  responsible: Kathrin Kaschner <kathrin.kaschner@informatik.uni-rostock.de>
 */

#ifndef EXCHANGEABILITY_H_
#define EXCHANGEABILITY_H_

#include "mynew.h"
#include <list>
//#include <vector>
//#include <map>

//#include "AnnotatedGraphNode.h"
//#include "CNF.h"

#include "util.h"
#include "cudd.h"
#include "cuddInt.h"
#include "dddmp.h"

using namespace std;
//typedef vector<bool> BitVector;


class Exchangeability {
    public:
        /// constructor (1 parameter)
        Exchangeability(string filename,
                        Cudd_ReorderingType heuristic = CUDD_REORDER_SIFT);

        /// deconstructor
        ~Exchangeability();

        /// checks the equality of two BDD representations
        bool check(Exchangeability* bdd);
        
        /// prints bdd_Mp and bdd_Ann
        void print();
        
        /// prints how much memory is currently in use
        void printMemoryInUse();
        
        /// reorders the bdd representation by another reordering type
        void reorder(Cudd_ReorderingType heuristic = CUDD_REORDER_SIFT);

        /// Provides user defined operator new. Needed to trace all new
        /// operations on this class.
#undef new
        NEW_OPERATOR(Exchangeability)
#define new NEW_NEW

        private:
        static DdManager* mgrMp;
        static DdManager* mgrAnn;
        
        /// Number of represented operating guidelines
        static int nbrBdd; 

        DdNode* bddMp;
        DdNode* bddAnn;
        
        /// varibles names of bddAnn
        char** names; 
        
        /// size of array names
        int nbrVarAnn; 
        list<char*> labelList;

        /// DESCRIPTION
        void loadBdd(std::string filename);
        
        /// DESCRIPTION
        void loadHeader(FILE* fp, char*** names, int* nVars, int** permids, int* nSuppVars);
        
        /// DESCRIPTION
        void loadOptimalOrder(DdManager* mgr, int size, int* permids, int maxId);
        
        /// DESCRIPTION
        DdNode* loadDiagram(FILE* fp, DdManager* mgr);
        
        /// DESCRIPTION
        void printDotFile(char* filename, char** varNames, DdNode* bddMp, DdNode* bddAnn);
        
        //void checkManager(DdManager* mgr, char* table);
    };

#endif /*EXCHANGEABILIY_H_*/
