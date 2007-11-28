/*****************************************************************************
 * Copyright 2006, 2007 Kathrin Kaschner, Peter Massuthe                     *
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
 * \file    Exchangeability.h
 *
 * \brief   functions for comparing two BDD-represented OGs for equality
 *
 * \author  responsible: Kathrin Kaschner <kathrin.kaschner@informatik.uni-rostock.de>
 *
 * \note    This file is part of the tool Fiona and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universit� zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
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
        Exchangeability(string filename,
                        Cudd_ReorderingType heuristic = CUDD_REORDER_SIFT);
        ~Exchangeability();

        bool check(Exchangeability* bdd);
        void print();
        void printMemoryInUse();
        void reorder(Cudd_ReorderingType heuristic = CUDD_REORDER_SIFT);

        // Provides user defined operator new. Needed to trace all new
        // operations on this class.
#undef new
        NEW_OPERATOR(Exchangeability)
#define new NEW_NEW

        private:
        static DdManager* mgrMp;
        static DdManager* mgrAnn;
        static int nbrBdd; //Number of represented operating guidelines

        DdNode* bddMp;
        DdNode* bddAnn;
        char** names; //varibles names of bddAnn
        int nbrVarAnn; //size of array names
        list<char*> labelList;

        void loadBdd(std::string filename);
        void loadHeader(FILE* fp, char*** names, int* nVars, int** permids, int* nSuppVars);
        void loadOptimalOrder(DdManager* mgr, int size, int* permids, int maxId);
        DdNode* loadDiagram(FILE* fp, DdManager* mgr);
        void printDotFile(char* filename, char** varNames, DdNode* bddMp, DdNode* bddAnn);
        //void checkManager(DdManager* mgr, char* table);
    };

#endif /*EXCHANGEABILIY_H_*/
