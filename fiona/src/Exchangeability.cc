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
 * \file    Exchangeability.cc
 *
 * \brief   functions for comparing two BDD-represented OGs for equality
 *
 * \author  responsible: Kathrin Kaschner <kathrin.kaschner@informatik.uni-rostock.de>
 */

#include "mynew.h"
#include "options.h"
#include "debug.h"
#include <iostream>
#include <string.h>
#include <stdlib.h>
#include "Exchangeability.h"


DdManager* Exchangeability::mgrMp = NULL;
DdManager* Exchangeability::mgrAnn = NULL;
int Exchangeability::nbrBdd = 0;

int comparestr( const void* a, const void* b){
    return strcmp( * (char**) a, *(char**) b);
}

//! \brief constructor
//! \param filename name of the fale to check exchangebility for
//! \param heuristic Cudd Reorderding type
Exchangeability::Exchangeability(string filename, Cudd_ReorderingType heuristic) {
    TRACE(TRACE_5, "Exchangeability::Exchangeability(char* filename): begin\n");
    // Init cudd package when first Exchangeability object is created.
    if (nbrBdd == 0) {
        //mgrMp = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        //mgrAnn = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
        mgrMp = Cudd_Init(0, 0, 1, 1, 1);
        mgrAnn = Cudd_Init(0, 0, 1, 1, 1);

        //enable automatic dynamic reordering of the BDDs
        Cudd_AutodynEnable(mgrMp, heuristic);
        Cudd_AutodynEnable(mgrAnn, heuristic);
    }

    names = NULL;
    nbrVarAnn = 0;
    loadBdd(filename);

    ++nbrBdd;

    for (int i = 0; i < nbrVarAnn; ++i) {
        if (names[i][0] == '!' || names[i][0] == '?') {
            (labelList).push_back(names[i]);
        }
    }

//     list<char*>::iterator list_iter;
//     for( list_iter = labelList.begin(); list_iter != labelList.end(); ++list_iter) {
//     cout << *list_iter;
//     }
//     cout << endl;
//
//     TRACE(TRACE_5, "Exchangeability::Exchangeability(char* filename): end\n");
}


//! \brief destructor
Exchangeability::~Exchangeability() {
    TRACE(TRACE_5, "Exchangeability::~Exchangeability(): begin\n");

    delete [] names;

    Cudd_RecursiveDeref(mgrAnn, bddAnn);
    Cudd_RecursiveDeref(mgrMp, bddMp);

    //checkManager(mgrAnn, "mgrAnn");
    //checkManager(mgrMp, "mgrMp");

    --nbrBdd;
    assert(nbrBdd >= 0);

    // Clean up cudd package when last Exchangeability object is destroyed.
    if (nbrBdd == 0) {
        Cudd_Quit(mgrAnn);
        mgrAnn = NULL;
        Cudd_Quit(mgrMp);
        mgrMp = NULL;
    }

    labelList.clear();

    TRACE(TRACE_5, "Exchangeability::~Exchangeability(): end\n");
}


//! \brief loads the BDD representation of the opereating guideline stored in filname
//! \param filename name of the og file
void Exchangeability::loadBdd(std::string filename) {
    TRACE(TRACE_5, "Exchangeability::loadBdds(string filename): begin\n");
    cout << "loading BDD-representation of the operating guideline of "
         << filename << endl;

    //open cudd-files
    std::string bufferMp;
    std::string bufferAnn;

    bufferMp = filename + ".og.BDD_MP.cudd";
    bufferAnn = filename + ".og.BDD_ANN.cudd";

    FILE* fpMp;
    fpMp = fopen(bufferMp.c_str(), "r");

    FILE* fpAnn;
    fpAnn = fopen(bufferAnn.c_str(), "r");

    if (fpMp == NULL || fpAnn == NULL) {
        bufferMp = filename + ".R.og.BDD_MP.cudd";
        bufferAnn = filename + ".R.og.BDD_ANN.cudd";

        fpMp = fopen(bufferMp.c_str(), "r");
        fpAnn = fopen(bufferAnn.c_str(), "r");

        if (fpMp == NULL || fpAnn == NULL) {
            cerr << "cannot open cudd-files of " << filename << "\n";
            setExitCode(EC_NO_CUDD_FILE);
        }
    }

    // load header of bddAnn
    int* permids = NULL; //optimal variable ordering
    int nbrSuppVarsAnn = 0; //size of array permids in BddAnn
    loadHeader(fpAnn, &names, &nbrVarAnn, &permids, &nbrSuppVarsAnn);
    fseek(fpAnn, 0, SEEK_SET); //for loading the BDD set the File-Pointer to the beginnig of the file

    //load bddAnn
    if (nbrBdd == 0) {
        loadOptimalOrder(mgrAnn, nbrSuppVarsAnn, permids, nbrVarAnn);
    }
    bddAnn = loadDiagram(fpAnn, mgrAnn);
    fclose(fpAnn);

    free(permids);
    permids = NULL;

    int nbrVarMp = 0; //size of array permids in BddMp
    int nbrSuppVarsMp = 0;

    //load header of bddMp
    loadHeader(fpMp, NULL, &nbrVarMp, &permids, &nbrSuppVarsMp);
    fseek(fpMp, 0, SEEK_SET); //for loading the BDD set the File-Pointer to the beginnig of the file

    //load bddMp
    if (nbrBdd == 0) {
        loadOptimalOrder(mgrMp, nbrSuppVarsMp, permids, nbrVarMp);
    }
    bddMp = loadDiagram(fpMp, mgrMp);
    fclose(fpMp);

    TRACE(TRACE_5, "Exchangeability::loadBdds(string filename): end\n");
}


//! \brief DESCRIPTION
//! \param names DESCRIPTION
//! \param nVars DESCRIPTION
//! \param permids DESCRIPTION
//! \param nSuppVars DESCRIPTION
void Exchangeability::loadHeader(FILE* fp,
                                 char*** names,
                                 int* nVars,
                                 int** permids,
                                 int* nSuppVars) {
    TRACE(TRACE_5, "Exchangeability::loadHeader(FILE* fp, char*** names, int* nVars, int** permids): begin\n");
    Dddmp_DecompType ddType; //possible Values: DDDMP_BDD,DDDMP_ADD,DDDMP_CNF,DDDMP_NONE
    char** orderedVarNames;
    char** suppVarNames;
    int* ids;
    int* auxids;
    int nRoots;

    Dddmp_cuddHeaderLoad (&ddType /* OUT: selects the proper decomp type */,
                          nVars /* OUT: number of DD variables */,
                          nSuppVars /* OUT: number of support variables */,
                          &suppVarNames /* OUT: array of support variable names */,
                          &orderedVarNames/* OUT: array of variable names */,
                          &ids /* OUT: array of variable ids */,
                          permids /* OUT: array of permids ids */,
                          &auxids /* OUT: array of variable aux ids */,
                          &nRoots /* OUT: number of root in the file */,
                          NULL /* IN: file name */, fp /* IN: file pointer */
    );

    if (ddType != DDDMP_BDD) {
        cout << "\nError while loading the BDDs: wrong DD-Type";
        setExitCode(EC_BDD_ERROR);
    }
    if (nRoots != 1) {
        cout
                << "\nError while loading the BDDs: the file must contain exactly one BDD";
        setExitCode(EC_BDD_ERROR);
    }

    if (nVars == 0) {
        cout << "Error nVars == 0";
        setExitCode(EC_BDD_ERROR);
    }

    if (names != NULL) {
        if (orderedVarNames== NULL) {
            cout << "Error orderedVarNames == NULL";
            setExitCode(EC_BDD_ERROR);
        }
        qsort((void*)orderedVarNames, *nVars, sizeof(char*), comparestr);
        *names = orderedVarNames; //suppVarNames;
    }
    TRACE(TRACE_5, "Exchangeability::loadHeader(FILE* fp, char*** names, int* nVars, int** permids): end\n");
}


//! \brief loads a good variable order to the BDD manager
//! \param permids gives the optimal order
//! \param size size of permids (=numer of BDD variables)
//! \param maxId highest possible number in permids
void Exchangeability::loadOptimalOrder(DdManager* mgr, int size, int* permids, int maxId) {
    int length;
    if (size < Cudd_ReadSize(mgr)) {
        length = Cudd_ReadSize(mgr);
    } else {
        length = size;
    }
    int idOrder[length]; //The size of idOrder should be equal or greater to the number of variables currently in use in mgr
    for (int i = 0; i < size; ++i) {
        assert(permids[i] < maxId);
        idOrder[permids[i]] = i;
    }
//     for (int i = size; i < length; ++i) {
//         idOrder[i] = i;
//     }

    assert (length >= Cudd_ReadSize(mgr));
    int res = Cudd_ShuffleHeap(mgr, idOrder);
    assert(res == 1);
}


//! \brief DESCRPITION
//! \param fp DESCRPITION
//! \param mgr DESCRPITION
//! \return DESCRPITION
DdNode* Exchangeability::loadDiagram(FILE* fp, DdManager* mgr) {
    TRACE(TRACE_5, "Exchangeability::loadDiagram(FILE* fp, DdManager* mgr, int size, int* permids): begin\n");

    DdNode* bdd = Dddmp_cuddBddLoad (mgr /* IN: DD Manager */,
                                     DDDMP_VAR_MATCHIDS /* IN: storing mode selector */,
                                     NULL /* IN: array of variable names - by IDs */,
                                     NULL /* IN: array of variable auxids - by IDs */,
                                     NULL /* IN: array of new ids accessed - by IDs */,
                                     DDDMP_MODE_TEXT /* IN: requested input file format */,
                                     NULL /* IN: file name */, fp /* IN: file pointer */
    );

    TRACE(TRACE_5, "Exchangeability::loadDiagram(FILE* fp, DdManager* mgr, int size, int* permids): end\n");
    return (bdd);
}


//! \brief DESCRPITION
//! \param filename DESCRPITION
//! \param varNames DESCRPITION
//! \param bddMp DESCRPITION
//! \param bddAnn DESCRPITION
//! \return DESCRPITION
void Exchangeability::printDotFile(char* filename,
                                   char** varNames,
                                   DdNode* bddMp,
                                   DdNode* bddAnn) {
    TRACE(TRACE_5, "Exchangeability::printDotFile(char* filename, char** varNames, DdNode* bddMp, DdNode* bddAnn): begin\n");
    if ((Cudd_DagSize(bddMp) < 200000) && (Cudd_DagSize(bddAnn) < 200000)) {

        char bufferMp[256];
        char bufferAnn[256];

        sprintf(bufferMp, "%s.Exchangeability.BDD_MP.out", filename);
        sprintf(bufferAnn, "%s.Exchangeability.BDD_ANN.out", filename);

        FILE* fpMp;
        fpMp = fopen(bufferMp, "w");
        Cudd_DumpDot(mgrMp, 1, &bddMp, varNames, NULL, fpMp);
        fclose(fpMp);

        FILE* fpAnn;
        fpAnn = fopen(bufferAnn, "w");
        Cudd_DumpDot(mgrAnn, 1, &bddAnn, varNames, NULL, fpAnn);
        fclose(fpAnn);

        if ((Cudd_DagSize(bddMp) < 900) && (Cudd_DagSize(bddAnn) < 900)) {
            sprintf(bufferMp,
                    "%s -Tpng \"%sexchangeability.BDD_MP.out\" -o \"%sexchangeability.BDD_MP.png\"",
                    CONFIG_DOT, filename, filename);
            sprintf(bufferAnn,
                    "%s -Tpng \"%sexchangeability.BDD_ANN.out\" -o \"%sexchangeability.BDD_ANN.png\"",
                    CONFIG_DOT, filename, filename);

            system(bufferMp);
            system(bufferAnn);
        }
    }
    TRACE(TRACE_5, "Exchangeability::printDotFile(char* filename, char** varNames, DdNode* bddMp, DdNode* bddAnn): end\n");
}


//! \brief print bdd_Mp and bdd_Ann
void Exchangeability::print() {
    cout << "\nBDD_MP:\n";
    Cudd_PrintMinterm(mgrMp, bddMp);
    cout << "\nBDD_ANN:\n";
    Cudd_PrintMinterm(mgrAnn, bddAnn);
}


//! \brief checks the equality of two BDD representations
//! \param bdd BDD representation to check with
bool Exchangeability::check(Exchangeability* bdd) {
    TRACE(TRACE_5, "Exchangeability::check(Exchangeability* bdd): begin\n");

    if (this->labelList.size() != bdd->labelList.size()) {
        return (false);
    }

    list<char*>::const_iterator thislist_iter = this->labelList.begin();
    list<char*>::const_iterator bddlist_iter = bdd->labelList.begin();
    while (thislist_iter != this->labelList.end()) { //this->labelList.size() is equal to bdd->labelList.size()
        //cout << *thislist_iter << *bddlist_iter << endl;
        if (strcmp(*thislist_iter, *bddlist_iter)) {
            return (false);
        }
        ++thislist_iter;
        ++bddlist_iter;
    }
    if (Cudd_bddLeq(mgrMp, bddMp, bdd->bddMp) == 1&&Cudd_bddLeq(mgrMp,
                                                                bdd->bddMp,
                                                                bddMp) == 1
            &&Cudd_bddLeq(mgrAnn, bddAnn, bdd->bddAnn) == 1
            &&Cudd_bddLeq(mgrAnn, bdd->bddAnn, bddAnn) == 1) {
        return (true);
    } else {
        return (false);
    }
    TRACE(TRACE_5, "Exchangeability::check(Exchangeability* bdd): end\n");
}


//! \brief reorders the variables of the BDDs according to a given heuristic
//! \param heuristic type of Cudd reordering
void Exchangeability::reorder(Cudd_ReorderingType heuristic) {

    Cudd_ReduceHeap(mgrMp, heuristic, 0);
    cout << "BDD_MP: number of nodes: " << Cudd_DagSize(bddMp);
    cout << "\t" << Cudd_ReadReorderingTime(this->mgrMp)
         << " ms consumed for variable reordering"<< endl;

    Cudd_ReduceHeap(this->mgrAnn, heuristic, 0);
    cout << "BDD_ANN: number of nodes: " << Cudd_DagSize(this->bddAnn);
    cout << "\t" << Cudd_ReadReorderingTime(this->mgrAnn)
         << " ms consumed for variable reordering" << endl;
}


//! \brief returns the memory in use by the BDD manager measured in bytes
void Exchangeability::printMemoryInUse() {
    cout << endl;
    // cout << "Number of live nodes in mgrMp: " << Cudd_ReadNodeCount(mgrMp) << endl;
    // cout << "Peak number of nodes in mgrMp: " << Cudd_ReadPeakNodeCount(mgrMp) << endl;
    // cout << "Number of dead nodes in mgrMp: " << Cudd_ReadDead(mgrMp) << endl;
    // cout << "Number of live nodes in mgrAnn: " << Cudd_ReadNodeCount(mgrAnn) << endl;
    cout << "Memory in use for mgrMp:  "
         << Cudd_ReadMemoryInUse(mgrMp)
         << " bytes" << endl;
    cout << "Memory in use for mgrAnn: "
         << Cudd_ReadMemoryInUse(mgrAnn)
         << " bytes" << endl;
    cout << "Memory in use for both BDD: "
         << Cudd_ReadMemoryInUse(mgrMp) + Cudd_ReadMemoryInUse(mgrAnn)
         << "  bytes ("
         << (Cudd_ReadMemoryInUse(mgrMp) + Cudd_ReadMemoryInUse(mgrAnn)) / 1024
         << " KB; "
         << (Cudd_ReadMemoryInUse(mgrMp) + Cudd_ReadMemoryInUse(mgrAnn)) / (1024*1024)
         << " MB)" << endl;
}
