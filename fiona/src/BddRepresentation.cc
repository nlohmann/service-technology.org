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
 * \file    BddRepresentation.cc
 *
 * \brief   functions for representing an OG as a BDD
 *
 * \author  responsible: Kathrin Kaschner <kathrin.kaschner@informatik.uni-rostock.de>
 */

#include "mynew.h"

#include <iostream>
#include <sstream>
#include "BddRepresentation.h"
#include "BddLabel.h"
#include "AnnotatedGraphEdge.h"
#include "GraphFormula.h"

#include "options.h"
#include "debug.h"

#include "owfn.h"
#include "state.h"
extern oWFN * PN;


//! \brief constructor
BddRepresentation::BddRepresentation(unsigned int numberOfLabels,
                                     Cudd_ReorderingType heuristic) {
    TRACE(TRACE_5, "BddRepresentation::BddRepresentation(unsigned int numberOfLabels, Cudd_ReorderingType heuristic): begin\n");

    nbrLabels = numberOfLabels + 1; //labels + final
    maxLabelBits = nbrBits(numberOfLabels-1);
    maxNodeBits = 1;
    maxNodeNumber = 0;

    int sizeMp = 2 * maxNodeBits + maxLabelBits;
    int sizeAnn = nbrLabels + maxNodeBits;

    //init managers
    //mgrMp = Cudd_Init(sizeMp, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    //mgrAnn = Cudd_Init(sizeAnn, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    mgrMp = Cudd_Init(sizeMp, 0, 1, 1, 1);
    mgrAnn = Cudd_Init(sizeAnn, 0, 1, 1, 1);

    //enable automatic dynamic reordering of the BDDs
    Cudd_AutodynEnable(mgrMp, heuristic);
    Cudd_AutodynEnable(mgrAnn, heuristic);

    //init BDDs
    bddMp = Cudd_Not(Cudd_ReadOne(mgrMp)); //BDDstructure
    Cudd_Ref(bddMp);
    bddRed = Cudd_Not(Cudd_ReadOne(mgrMp)); //BDDstructure
    Cudd_Ref(bddRed);
    bddAnn = Cudd_Not(Cudd_ReadOne(mgrAnn)); //BDDannotation
    Cudd_Ref(bddAnn);

    labelTable = new BddLabelTab(2 * (nbrLabels + 1));
    assert(PN->getInputPlaceCount() + PN->getOutputPlaceCount() <= pow(double(2), double(sizeof(int) * 8 - 1)) - 1); //PN->getInputPlaceCount() + PN->getOutputPlaceCount() <= 2^31 -1
    assert(nbrLabels == 1 + int(PN->getInputPlaceCount() + PN->getOutputPlaceCount())); //final + labels

    //add the labels and their bddNumber to the labelTable
    BddLabel * label;
    for (unsigned int i = 0; i < PN->getInputPlaceCount(); ++i) {
        //the labels must be coded in alphabetical order
        assert( i == 0 || (PN->getInputPlace(i - 1)->getLabelForCommGraph() < PN->getInputPlace(i)->getLabelForCommGraph()));
        label = new BddLabel(PN->getInputPlace(i)->getLabelForCommGraph(), i, labelTable);
        //cout << i << "  " << PN->getInputPlace(i)->name << endl;
    }

    for (unsigned int i = 0; i < PN->getOutputPlaceCount(); ++i) {
        //the labels must be coded in alphabetical order
        assert( i == 0 || (PN->getOutputPlace(i - 1)->getLabelForCommGraph() < PN->getOutputPlace(i)->getLabelForCommGraph()));
        label = new BddLabel(PN->getOutputPlace(i)->getLabelForCommGraph(), i + PN->getInputPlaceCount(), labelTable);
        //cout << i + PN->getInputPlaceCount() << "  " << PN->getOutputPlace(i)->name << endl;
    }

    //add the variable "final" to the labelTable
    //cout << PN->getOutputPlaceCount() + PN->getInputPlaceCount() << "  " << "final" << endl;
    label = new BddLabel(GraphFormulaLiteral::FINAL, PN->getOutputPlaceCount() + PN->getInputPlaceCount(), labelTable);

    /* BddLabel * temp;
     for (unsigned int i = 0; i < labelTable->size; i++) {
         BddLabel * temp;
         temp = labelTable->table[i];
         while (temp != 0){
             cout << i << "    " << temp->nbr <<"   " << temp->name << endl;
             temp = temp->next;
         }
     }
     */
    TRACE(TRACE_5, "BddRepresentation::BddRepresentation(unsigned int numberOfLabels, Cudd_ReorderingType heuristic): end\n");
}


//! \brief destructor
BddRepresentation::~BddRepresentation() {
    TRACE(TRACE_5, "BddRepresentation::~BddRepresentation(): begin\n");

    nodeMap.clear();

    Cudd_RecursiveDeref(mgrAnn, bddAnn);
    Cudd_RecursiveDeref(mgrMp, bddMp);

    //checkManager(mgrAnn, "mgrAnn");
    //checkManager(mgrMp, "mgrMp");

    Cudd_Quit(mgrAnn);
    Cudd_Quit(mgrMp);

    delete labelTable;

    TRACE(TRACE_5, "BddRepresentation::~BddRepresentation(): end\n");
}


void BddRepresentation::convertRootNode(AnnotatedGraphNode* root) {
    TRACE(TRACE_5, "void BddRepresentation::convertRootNode(AnnotatedGraphNode* root): begin\n");

    pair<map<unsigned int, unsigned int>::iterator, bool> success;
    success = nodeMap.insert(make_pair(root->getNumber(), 0));
    assert(success.second == true);

    TRACE(TRACE_5, "void BddRepresentation::convertRootNode(AnnotatedGraphNode* root): end\n");
}


//! \brief generate BDD representation
void BddRepresentation::generateRepresentation(AnnotatedGraphNode* v,
                                               std::map<AnnotatedGraphNode*, bool>& visitedNodes) {

    TRACE(TRACE_5, "BddRepresentation::generateRepresentation(AnnotatedGraphNode* v, bool visitedNodes[]): start\n");

    DdNode* tmp;
    //annotation
    DdNode * annotation = annotationToBddAnn(v);
//    cout << "annotation of node: " << v->getNumber() << " ("<< getBddNumber(v->getNumber()) << ")" << "\n";
//    Cudd_PrintMinterm(mgrAnn, annotation);
//    cout << "---------------------\n";
    tmp = Cudd_bddOr(mgrAnn, annotation, bddAnn);
    assert(tmp != NULL);
    Cudd_Ref(tmp);
    Cudd_RecursiveDeref(mgrAnn, annotation);
    Cudd_RecursiveDeref(mgrAnn, bddAnn);
    bddAnn = tmp;

    //cout << "current BDD_Ann\n"; Cudd_PrintMinterm(mgrAnn, bddAnn); cout << "=====================\n";

    //bddMp = nodesToBddMp(0, 1);

    if (v->getColor() == BLUE && v->reachGraphStateSet.size() != 0) {
            visitedNodes[v] = true;

            AnnotatedGraphNode::LeavingEdges::ConstIterator
                    edgeIter =v->getLeavingEdgesConstIterator();

            while (edgeIter->hasNext()) {
                AnnotatedGraphEdge* element = edgeIter->getNext();
                AnnotatedGraphNode* vNext = element->getDstNode();

                if (vNext->getColor() == BLUE &&
                    vNext->reachGraphStateSet.size() != 0 &&
                    vNext != NULL ) {

                    //label
                    DdNode * label = labelToBddMp(element->getLabel());
                    //nodes
                    DdNode * nodes = nodesToBddMp(v->getNumber(), vNext->getNumber());

                    //edge
                    DdNode * edge = Cudd_bddAnd(mgrMp, label, nodes);
                    assert(edge != NULL);
                    Cudd_Ref(edge);
                    Cudd_RecursiveDeref(mgrMp, label);
                    Cudd_RecursiveDeref(mgrMp, nodes);

//                    cout << "edge: " << v->getNumber() << " [" << element->getLabel() << "> " << vNext->getNumber() << endl;
//                    cout << "edge: " << getBddNumber(v->getNumber()) << " [" << element->getLabel() << "> "
//                    << getBddNumber(vNext->getNumber()) << " -new-\n";
//                    Cudd_PrintMinterm(mgrMp, edge);
//                    cout << "=====================\n";

                    tmp = Cudd_bddOr(mgrMp, edge, bddMp);
                    assert(tmp != NULL);
                    Cudd_Ref(tmp);
                    Cudd_RecursiveDeref(mgrMp, edge);
                    Cudd_RecursiveDeref(mgrMp, bddMp);
                    bddMp = tmp;
//                    cout << "current BDD_MP\n";
//                    Cudd_PrintMinterm(mgrMp, bddMp);
//                    cout << "=====================\n";
                    if ((vNext != v) && !visitedNodes[vNext]) {
                        generateRepresentation(vNext, visitedNodes);
                    }
                }
            } //end while
            delete edgeIter;
    }
    TRACE(TRACE_5, "BddRepresentation::generateRepresentation(AnnotatedGraphNode* v, bool visitedNodes[]): end\n");
}


//! \brief add blue edges to the BDD and delete red edges from the BDD (for the on the fly construction)
void BddRepresentation::addOrDeleteLeavingEdges(AnnotatedGraphNode* v) {

    TRACE(TRACE_5, "BddRepresentation::addOrDeleteLeavingEdges(AnnotatedGraphNode* v): start\n");

    if (v->getColor() == BLUE) { //add annotation
        DdNode * annotation = annotationToBddAnn(v);
        DdNode* tmp = Cudd_bddOr(mgrAnn, annotation, bddAnn);
        assert(tmp != NULL);
        Cudd_Ref(tmp);
        Cudd_RecursiveDeref(mgrAnn, annotation);
        Cudd_RecursiveDeref(mgrAnn, bddAnn);
        bddAnn = tmp;
    } else { //delete annotation
        DdNode * annotation = annotationToBddAnn(v);
        DdNode* tmp = Cudd_bddAnd(mgrAnn, annotation, Cudd_Not(bddAnn));
        assert(tmp != NULL);
        Cudd_Ref(tmp);
        Cudd_RecursiveDeref(mgrAnn, annotation);
        Cudd_RecursiveDeref(mgrAnn, bddAnn);
        bddAnn = tmp;
    }

    if (v->reachGraphStateSet.size() != 0) {

        AnnotatedGraphNode::LeavingEdges::ConstIterator
                edgeIter = v->getLeavingEdgesConstIterator();

        while (edgeIter->hasNext()) {
            AnnotatedGraphEdge* element = edgeIter->getNext();
            AnnotatedGraphNode* vNext = element->getDstNode();
            if (vNext != NULL) {

                //label
                DdNode* label = labelToBddMp(element->getLabel());

                //nodes
                DdNode* nodes = nodesToBddMp(v->getNumber(),
                                              vNext->getNumber());

                //edge
                DdNode* edge = Cudd_bddAnd(mgrMp, label, nodes);
                assert(edge != NULL);
                Cudd_Ref(edge);
                Cudd_RecursiveDeref(mgrMp, label);
                Cudd_RecursiveDeref(mgrMp, nodes);

                /*cout << "edge: " << v->getNumber() << " [" << element->getLabel() << "> " << vNext->getNumber() << endl;
                 cout << "edge: " << getBddNumber(v->getNumber()) << " [" << element->getLabel() << "> "
                 << getBddNumber(vNext->getNumber()) << " -new-\n";
                 Cudd_PrintMinterm(mgrMp, edge);    */
                if (v->getColor() == BLUE && vNext->getColor() == BLUE ) { //add blue edges
                    /*cout << "add current edge " << endl;
                     cout << "\n=====================\n\n";*/
                    DdNode* tmp = Cudd_bddOr(mgrMp, edge, bddMp);
                    assert(tmp != NULL);
                    Cudd_Ref(tmp);
                    Cudd_RecursiveDeref(mgrMp, edge);
                    Cudd_RecursiveDeref(mgrMp, bddMp);
                    bddMp = tmp;
                } else { //delete red edge
                    /*cout << "delete edge " << v->getNumber() << "->" << vNext->getNumber() << endl;
                     cout << "\n=====================\n\n";*/
                    DdNode* tmp = Cudd_bddAnd(mgrMp, bddMp, Cudd_Not(edge));
                    assert(tmp != NULL);
                    Cudd_Ref(tmp);
                    Cudd_RecursiveDeref(mgrMp, edge);
                    Cudd_RecursiveDeref(mgrMp, bddMp);
                    bddMp = tmp;
                }
            }
        } //end while
        delete edgeIter;
    }

    TRACE(TRACE_5, "BddRepresentation::addOrDeleteLeavingEdges(AnnotatedGraphNode* v): end\n");
}


//! \brief returns the BDD of the given label
DdNode* BddRepresentation::labelToBddMp(const std::string& label) {

    TRACE(TRACE_5, "BddRepresentation::labelToBddMp(const std::string& label): start\n");

    BddLabel * s = labelTable->lookup(label.c_str());
    unsigned int number = s->nbr;
    BitVector assignment = numberToBin(number, maxLabelBits);

    DdNode* f = Cudd_ReadOne(mgrMp);
    Cudd_Ref(f);
    DdNode* tmp;

    //assignment to BDD
    for (int i = 0; i < maxLabelBits; ++i) {
        if (assignment[i] == false) {
            tmp = Cudd_bddAnd(mgrMp, Cudd_Not(Cudd_bddIthVar(mgrMp, i)), f);
        } else {
            tmp = Cudd_bddAnd(mgrMp, Cudd_bddIthVar(mgrMp, i), f);
        }
        assert(tmp != NULL);
        Cudd_Ref(tmp);
        Cudd_RecursiveDeref(mgrMp, f);
        f = tmp;
    }
    //cout << "label: " << label << ": "; Cudd_PrintMinterm(mgrMp,f);
    TRACE(TRACE_5, "BddRepresentation::labelToBddMp(const std::string& label): end\n");
    return (f);
}


//! \brief returns the BDD of the nodes (given as integer) of an edge
DdNode* BddRepresentation::nodesToBddMp(unsigned int node1, unsigned int node2) {

    TRACE(TRACE_5, "BddRepresentation::nodesToBddMp(unsigned int node1, unsigned int node2): start\n");

    unsigned int bddNumber1 = getBddNumber(node1);
    unsigned int bddNumber2 = getBddNumber(node2);

    unsigned int max;
    if (bddNumber1 > bddNumber2) {
        max = bddNumber1;
    } else {
        max = bddNumber2;
    }

    // cout << "max: " << max  << "  maxNodeNumber-old-: " << maxNodeNumber << endl;

    if (max > maxNodeNumber) {
        addBddVars(max);
    }

    BitVector assignment1 = numberToBin(bddNumber1, maxNodeBits);
    BitVector assignment2 = numberToBin(bddNumber2, maxNodeBits);

    //calculate the BDD for assignment1 and assignment2
    DdNode* f = Cudd_ReadOne(mgrMp);
    Cudd_Ref(f);
    DdNode* tmp;

    int j;
    for (int i = 0; i < maxNodeBits; ++i) {
        j = maxNodeBits-1-i;
        assert(j >= 0);
        if (assignment1[j] == false) {
            tmp = Cudd_bddAnd(mgrMp,
                              Cudd_Not(Cudd_bddIthVar(mgrMp, maxLabelBits + (2*i))), f);
        } else {
            tmp = Cudd_bddAnd(mgrMp, Cudd_bddIthVar(mgrMp, maxLabelBits+(2*i)), f);
        }
        assert(tmp != NULL);
        Cudd_Ref(tmp);
        Cudd_RecursiveDeref(mgrMp, f);
        f = tmp;

        if (assignment2[j] == false) {
            tmp = Cudd_bddAnd(mgrMp,
                              Cudd_Not(Cudd_bddIthVar(mgrMp, maxLabelBits+1+(2 *i))), f);
        } else {
            tmp = Cudd_bddAnd(mgrMp, Cudd_bddIthVar(mgrMp, maxLabelBits+1+(2*i)), f);
        }
        assert(tmp != NULL);
        Cudd_Ref(tmp);
        Cudd_RecursiveDeref(mgrMp, f);
        f = tmp;
    }
    TRACE(TRACE_5, "BddRepresentation::nodesToBddMp(unsigned int node1, unsigned int node2): end\n");
    return (f);
}


//! \brief returns the BDD of the annotation of a given node
DdNode* BddRepresentation::annotationToBddAnn(AnnotatedGraphNode* v) {
    TRACE(TRACE_5, "DdNode* BddRepresentation::annotationToBddAnn(AnnotatedGraphNode * v): start\n");

//    cout << "----------------------------------\n";
//    cout << "node " << v->getNumber() << " ("<< getBddNumber(v->getNumber()) << ") : " << v->getAnnotation()->asString()<< endl;

    DdNode* tmp;
    DdNode* CNFTemp;

    DdNode* annotation = Cudd_ReadOne(mgrAnn);
    Cudd_Ref(annotation);

    GraphFormulaCNF* cnfFormula = v->getAnnotation();

    for (GraphFormulaCNF::const_iterator iClause = cnfFormula->begin(); iClause
            != cnfFormula->end(); ++iClause) {
        GraphFormulaMultiaryOr
                * clause =dynamic_cast<GraphFormulaMultiaryOr*>(*iClause);
        assert(clause != NULL);
        CNFTemp = clauseToBddAnn(clause);
        tmp = Cudd_bddAnd(mgrAnn, annotation, CNFTemp);
        assert(tmp != NULL);
        Cudd_Ref(tmp);
        Cudd_RecursiveDeref(mgrAnn, annotation);
        Cudd_RecursiveDeref(mgrAnn, CNFTemp);
        annotation = tmp;
    }

    unsigned int bddNumber = getBddNumber(v->getNumber());
    //cout << "bddNumber: " << bddNumber  << "  maxNodeNumber: " << maxNodeNumber << endl;

    if (bddNumber > maxNodeNumber) {
        addBddVars(bddNumber);
    }
    BitVector assignment = numberToBin(bddNumber, maxNodeBits);

    //calculate the BDD for assignment
    DdNode* f = Cudd_ReadOne(mgrAnn);
    Cudd_Ref(f);

    int j;
    for (int i = 0; i < maxNodeBits; ++i) {
        j = maxNodeBits-1-i;
        assert(j >= 0);
        if (assignment[j] == false) {
            tmp = Cudd_bddAnd(mgrAnn, Cudd_Not(Cudd_bddIthVar(mgrAnn, nbrLabels
                    +i)), f);
        } else {
            tmp = Cudd_bddAnd(mgrAnn, Cudd_bddIthVar(mgrAnn, nbrLabels+i), f);
        }
        assert(tmp != NULL);
        Cudd_Ref(tmp);
        Cudd_RecursiveDeref(mgrAnn, f);
        f = tmp;
    }

    tmp = Cudd_bddAnd(mgrAnn, annotation, f);
    assert(tmp != NULL);
    Cudd_Ref(tmp);
    Cudd_RecursiveDeref(mgrAnn, annotation);
    Cudd_RecursiveDeref(mgrAnn, f);
    annotation = tmp;

    TRACE(TRACE_5, "DdNode* BddRepresentation::annotationToBddAnn(AnnotatedGraphNode * v): end\n");
    return annotation;
}


DdNode* BddRepresentation::clauseToBddAnn(const GraphFormulaMultiaryOr* myclause) {
    DdNode* tmp;

    if (myclause->empty()) {
        // since the clause is empty we can't conclude anything
        tmp = Cudd_Not(Cudd_ReadOne(mgrAnn));
        Cudd_Ref(tmp);
        return tmp; //"(false)";
    }

    DdNode* clause1 = Cudd_Not(Cudd_ReadOne(mgrAnn));
    Cudd_Ref(clause1);

    for (GraphFormulaMultiaryOr::const_iterator iLiteral =myclause->begin(); iLiteral
            != myclause->end(); ++iLiteral) {
        GraphFormulaLiteral * literal =dynamic_cast<GraphFormulaLiteral*>(*iLiteral);
        assert(literal != NULL);
        BddLabel* label = labelTable->lookup(literal->asString());
        int i = label->nbr;
        tmp = Cudd_bddOr(mgrAnn, clause1, Cudd_bddIthVar(mgrAnn, i));
        assert(tmp != NULL);
        Cudd_Ref(tmp);
        Cudd_RecursiveDeref(mgrAnn, clause1);
        clause1 = tmp;
    }

    // cout<< "clause: \n"; Cudd_PrintMinterm(mgrAnn, clause1);
    return clause1;
}


//! \param node
unsigned int BddRepresentation::getBddNumber(unsigned int node) {

    TRACE(TRACE_5, "BddRepresentation::getBddNumber(unsigned int node): begin\n");

    map<unsigned int, unsigned int>::const_iterator map_iter;
    map_iter = nodeMap.find(node); //search node in nodeMap

    unsigned int bddNumber;
    if (map_iter == nodeMap.end()) { //if node not found add node with a new bddNumber
        pair<map<unsigned int, unsigned int>::iterator, bool> success;
        bddNumber = nodeMap.size();
        success = nodeMap.insert(make_pair(node, bddNumber));
        //if (!(success.second)){exit(1);}
    } else {
        bddNumber = map_iter -> second;
    }

    TRACE(TRACE_5, "BddRepresentation::getBddNumber(unsigned int node): end\n");
    return (bddNumber);
}


void BddRepresentation::addBddVars(unsigned int max) {
    //cout << "maxNodeBits-old-: " << maxNodeBits << "   maxNodeNumber-old-: " << maxNodeNumber << endl;
    int neededNodeBits = nbrBits(max);
    //cout << "neededNodeBits: " << neededNodeBits << "   maxNodeNumber-new-: " << max << endl;
    DdNode* tmp1 = Cudd_ReadOne(mgrMp);
    Cudd_Ref(tmp1);
    DdNode* tmp2;
    DdNode* var;
    //add necessary BDD variables to BddMp
    for (int i = 0; i < 2*(neededNodeBits - maxNodeBits); ++i) {
        var = Cudd_bddNewVar(mgrMp);
        assert(var != NULL);
        tmp2 = Cudd_bddAnd(mgrMp, tmp1, Cudd_Not(var));
        assert(tmp2 != NULL);
        Cudd_Ref(tmp2);
        Cudd_RecursiveDeref(mgrMp, tmp1);
        tmp1 = tmp2;
    }

    tmp2 = Cudd_bddAnd(mgrMp, tmp1, bddMp);
    assert(tmp2 != NULL);
    Cudd_Ref(tmp2);
    Cudd_RecursiveDeref(mgrMp, tmp1);
    Cudd_RecursiveDeref(mgrMp, bddMp);
    bddMp = tmp2;

    tmp1 = Cudd_ReadOne(mgrAnn);
    Cudd_Ref(tmp1);
    //add necessary BDD variables to BddAnn
    for (int i = 0; i < (neededNodeBits - maxNodeBits); ++i) {
        var = Cudd_bddNewVar(mgrAnn);
        assert(var != NULL);
        tmp2 = Cudd_bddAnd(mgrAnn, tmp1, Cudd_Not(var));
        assert(tmp2 != NULL);
        Cudd_Ref(tmp2);
        Cudd_RecursiveDeref(mgrAnn, tmp1);
        tmp1 = tmp2;
    }

    tmp2 = Cudd_bddAnd(mgrAnn, tmp1, bddAnn);
    assert(tmp2 != NULL);
    Cudd_Ref(tmp2);
    Cudd_RecursiveDeref(mgrAnn, tmp1);
    Cudd_RecursiveDeref(mgrAnn, bddAnn);
    bddAnn = tmp2;

    maxNodeBits = neededNodeBits;
    maxNodeNumber = (unsigned int) pow((double)2, (double)neededNodeBits)-1;
}


//! \brief returns the binary representation of a number
BitVector BddRepresentation::numberToBin(unsigned int number, int cntBits) {
    TRACE(TRACE_5, "BddRepresentation::numberToBin(unsigned int number, int cntBits): start\n");

    BitVector assignment = BitVector(cntBits);
    int base = 2;

    //calculate the binary representation
    int index = cntBits - 1;
    do {
        assert(index >= 0);
        assignment[index--] = number % base;
        number /= base;
    } while (number);

    //add leading zeros
    for (int i = index; i >= 0; --i) {
        assignment[i] = false;
    }
    TRACE(TRACE_5, "BddRepresentation::numberToBin(unsigned int number, int cntBits): end\n");
    return (assignment);
}


//! \brief returns the number of bits to represent a number i
int BddRepresentation::nbrBits(unsigned int i) {
    switch (i) {
        case (0):
            return (1);
        case (1):
            return (1);
        default: {
            //const int max = (int)floor(log((double)(i))/log((double)2))+1;
            unsigned int nbrBits = 0;
            unsigned int nbr = i;
            while (nbr > 0) {
                nbr = nbr >> 1;
                ++nbrBits;
                //cout << "nbrBits :" << nbrBits << endl;
            }
            //cout << "nbrBits of " << i << ":  " << nbrBits << endl;
            return (nbrBits);
        }
    }
}


//! \param mgr
//! \param table
void BddRepresentation::checkManager(DdManager* mgr, char* table) {
    cout << "\ncheck "<<table << ":\n";
    if (Cudd_CheckZeroRef(mgr) != 0) {
        cout << "WARNING: Cudd_CheckZeroRef(" << table << ") = "
             << Cudd_CheckZeroRef(mgr) << "\n";
    }
    cuddHeapProfile(mgr);
    Cudd_DebugCheck(mgr);
}


//! \param heuristic
void BddRepresentation::reorder(Cudd_ReorderingType heuristic) {
    Cudd_ReduceHeap(mgrMp, heuristic, 0);
    cout << "BDD_MP: number of nodes: " << Cudd_DagSize(bddMp);
    if (bddRed != Cudd_Not(Cudd_ReadOne(mgrMp))) {
        DdNode* bdd[2];
        bdd[0] = bddMp;
        bdd[1] = bddRed;
        cout << "\nBDD_RED: number of nodes: " << Cudd_DagSize(bddRed);
        cout << "\nBDD_MP + BDD_RED: number of nodes: " << Cudd_SharingSize(bdd, 2);
    }

    cout << "\t  " << Cudd_ReadReorderingTime(this->mgrMp)
         << " ms consumed for variable reordering" << endl;

    Cudd_ReduceHeap(this->mgrAnn, heuristic, 0);
    cout << "BDD_ANN: number of nodes: " << Cudd_DagSize(this->bddAnn);
    cout << "\t  " << Cudd_ReadReorderingTime(this->mgrAnn)
         << " ms consumed for variable reordering" << endl;
}


//! \brief creates dot files of the BDDs
// use dot -Tps fileName -o new.ps for drawing the BDD
void BddRepresentation::printDotFile(char** varNames, char* option) {
    if ((Cudd_DagSize(bddMp) < 20000) && (Cudd_DagSize(bddAnn) < 20000)) {

        char bufferMp[256];
        char bufferAnn[256];

        string modus = toLower(option);

        if (options[O_OUTFILEPREFIX]) {
            const char * prefix = outfilePrefix.c_str();
            if (options[O_CALC_ALL_STATES]) {
                sprintf(bufferMp, "%s.%s.BDD_MP.out", prefix, modus.c_str());
                sprintf(bufferAnn, "%s.%s.BDD_ANN.out", prefix, modus.c_str());
            } else {
                sprintf(bufferMp, "%s.R.%s.BDD_MP.out", prefix, modus.c_str());
                sprintf(bufferAnn, "%s.R.%s.BDD_ANN.out", prefix, modus.c_str());
            }
        } else {
            if (options[O_CALC_ALL_STATES]) {
                sprintf(bufferMp, "%s.%s.BDD_MP.out", PN->filename.c_str(), modus.c_str());
                sprintf(bufferAnn, "%s.%s.BDD_ANN.out", PN->filename.c_str(), modus.c_str());
            } else {
                sprintf(bufferMp, "%s.R.%s.BDD_MP.out", PN->filename.c_str(), modus.c_str());
                sprintf(bufferAnn, "%s.R.%s.BDD_ANN.out", PN->filename.c_str(), modus.c_str());
            }
        }

        FILE* fpMp;
        fpMp = fopen(bufferMp, "w");
        Cudd_DumpDot(mgrMp, 1, &bddMp, varNames, NULL, fpMp);
        fclose(fpMp);

        FILE* fpAnn;
        fpAnn = fopen(bufferAnn, "w");
        Cudd_DumpDot(mgrAnn, 1, &bddAnn, varNames, NULL, fpAnn);
        fclose(fpAnn);

        if ((Cudd_DagSize(bddMp) < 900) && (Cudd_DagSize(bddAnn) < 900)) {
            trace( "\ncreating the dot file of BDD_MP and BDD_ANN...\n");
            if (options[O_OUTFILEPREFIX]) {
                const char * prefix = outfilePrefix.c_str();
                if (options[O_CALC_ALL_STATES]) {
                    sprintf(bufferMp,
                            "%s -Tpng \"%s.og.BDD_MP.out\" -o \"%s.og.BDD_MP.png\"",
                            CONFIG_DOT, prefix, prefix);
                    sprintf(bufferAnn,
                            "%s -Tpng \"%s.og.BDD_ANN.out\" -o \"%s.og.BDD_ANN.png\"",
                            CONFIG_DOT, prefix, prefix);
                } else {
                    sprintf(bufferMp,
                            "%s -Tpng \"%s.R.og.BDD_MP.out\" -o \"%s.R.og.BDD_MP.png\"",
                            CONFIG_DOT, prefix, prefix);
                    sprintf(bufferAnn,
                            "%s -Tpng \"%s.R.og.BDD_ANN.out\" -o \"%s.R.og.BDD_ANN.png\"",
                            CONFIG_DOT, prefix, prefix);
                }
            } else {
                if (options[O_CALC_ALL_STATES]) {
                    sprintf(bufferMp,
                            "%s -Tpng \"%s.og.BDD_MP.out\" -o \"%s.og.BDD_MP.png\"",
                            CONFIG_DOT, PN->filename.c_str(), PN->filename.c_str());
                    sprintf(bufferAnn,
                            "%s -Tpng \"%s.og.BDD_ANN.out\" -o \"%s.og.BDD_ANN.png\"",
                            CONFIG_DOT, PN->filename.c_str(), PN->filename.c_str());
                } else {
                    sprintf(bufferMp,
                            "%s -Tpng \"%s.R.og.BDD_MP.out\" -o \"%s.R.og.BDD_MP.png\"",
                            CONFIG_DOT, PN->filename.c_str(), PN->filename.c_str());
                    sprintf(bufferAnn,
                            "%s -Tpng \"%s.R.og.BDD_ANN.out\" -o \"%s.R.og.BDD_ANN.png\"",
                            CONFIG_DOT, PN->filename.c_str(), PN->filename.c_str());
                }
            }

            trace( bufferMp);
            trace( "\n");
            trace( bufferAnn);
            trace( "\n");
            system(bufferMp);
            system(bufferAnn);

            if (bddRed != Cudd_Not(Cudd_ReadOne(mgrMp))) {
                char bufferMpRed[256];
                if (options[O_OUTFILEPREFIX]) {
                    const char * prefix = outfilePrefix.c_str();
                    if (options[O_CALC_ALL_STATES]) {
                        sprintf(bufferMpRed, "%s.%s.BDD_MPRED.out", prefix, modus.c_str());
                    } else {
                        sprintf(bufferMpRed, "%s.R.%s.BDD_MPRED.out", prefix, modus.c_str());
                    }
                } else {
                    if (options[O_CALC_ALL_STATES]) {
                        sprintf(bufferMpRed, "%s.%s.BDD_MPRED.out", PN->filename.c_str(), modus.c_str());
                    } else {
                        sprintf(bufferMpRed, "%s.R.%s.BDD_MPRED.out", PN->filename.c_str(), modus.c_str());
                    }
                }

                DdNode* bdd[2];
                bdd[0] = bddMp;
                bdd[1] = bddRed;
                FILE* fpMpRed;
                fpMpRed = fopen(bufferMpRed, "w");
                Cudd_DumpDot(mgrMp, 2, bdd, varNames, NULL, fpMpRed);
                fclose(fpMpRed);

                if (options[O_OUTFILEPREFIX]) {
                    const char * prefix = outfilePrefix.c_str();
                    if (options[O_CALC_ALL_STATES]) {
                        sprintf(bufferMpRed,
                                "%s -Tpng %s.og.BDD_MPRED.out -o %s.og.BDD_MPRED.png",
                                CONFIG_DOT, prefix, prefix);
                    } else {
                        sprintf(bufferMpRed,
                                "%s -Tpng %s.R.og.BDD_MPRED.out -o %s.R.og.BDD_MPRED.png",
                                CONFIG_DOT, prefix, prefix);
                    }
                } else {
                    if (options[O_CALC_ALL_STATES]) {
                        sprintf(bufferMpRed,
                                "%s -Tpng %s.og.BDD_MPRED.out -o %s.og.BDD_MPRED.png",
                                CONFIG_DOT, PN->filename.c_str(), PN->filename.c_str());
                    } else {
                        sprintf(bufferMpRed,
                                "%s -Tpng %s.R.og.BDD_MPRED.out -o %s.R.og.BDD_MPRED.png",
                                CONFIG_DOT, PN->filename.c_str(), PN->filename.c_str());
                    }
                }

                trace( bufferMpRed);
                trace( "\n");
                system(bufferMpRed);
            }

        } else {
            trace( "\nBDDs are too big to have dot create the graphics\n");
        }

    } else {
        trace( "\nBDDs are too big to create dot file\n");
    }
}


//! \brief print bdd_Mp and bdd_Ann
void BddRepresentation::print() {
    cout << "\nBDD_MP:\n";
    Cudd_PrintMinterm(mgrMp, bddMp);
    cout << "\nBDD_ANN:\n";
    Cudd_PrintMinterm(mgrAnn, bddAnn);
}


//! \brief save bddMp and bddAnn in file
void BddRepresentation::save(char const* option) {
    int size = nbrLabels + maxNodeBits;
    char** names = new char*[size];

    string modus = toLower(option);

    assert(PN->getInputPlaceCount() + PN->getOutputPlaceCount() <= pow(double(2), double(sizeof(int)*8-1)) - 1);
    //PN->getInputPlaceCount() + PN->getOutputPlaceCount() <= 2^31 -1

    for (unsigned int i = 0; i < PN->getInputPlaceCount(); ++i) {
        assert((int)i < nbrLabels + maxNodeBits);
        owfnPlace* place = PN->getInputPlace(i);
        // cout << "i: " << i << "   name: " << place->name << "   nbr: " << labelTable->lookup(place->getLabelForCommGraph())->nbr << endl;
        char* tmp = new char [place->getLabelForCommGraph().size() + 1];
        strcpy (tmp, place->getLabelForCommGraph().c_str());
        unsigned int nbr = labelTable->lookup(place->getLabelForCommGraph())->nbr;
        names[nbr] = tmp;
    }

    for (unsigned int i = 0; i < PN->getOutputPlaceCount(); ++i) {
        assert(int(i+PN->getInputPlaceCount()) < nbrLabels + maxNodeBits);
        owfnPlace* place = PN->getOutputPlace(i);
        //cout << "i: " << i << "   name: " << place->name << "   nbr: " << labelTable->lookup(place->getLabelForCommGraph())->nbr << endl;
        char* tmp = new char [place->getLabelForCommGraph().size() + 1];
        strcpy (tmp, place->getLabelForCommGraph().c_str());
        unsigned int nbr = labelTable->lookup(place->getLabelForCommGraph())->nbr;
        names[nbr] = tmp;
    }

    char* tmp = new char [GraphFormulaLiteral::FINAL.size() + 1];
    strcpy (tmp, GraphFormulaLiteral::FINAL.c_str());
    unsigned int nbr = labelTable->lookup(GraphFormulaLiteral::FINAL)->nbr;
    names[nbr] = tmp;

    assert((unsigned int)nbrLabels == PN->getInputPlaceCount() + PN->getOutputPlaceCount() + 1); // + 1 for 'final' variable
    assert(Cudd_ReadSize(mgrAnn) == nbrLabels + maxNodeBits);
    for (int i = nbrLabels; i < size; ++i) {
        assert(i < Cudd_ReadSize(mgrAnn));

        int varNumber = i-nbrLabels;
        assert(varNumber >= 0);
        char* buffer = new char[11];
        sprintf(buffer, "%d", varNumber);
        names[i] = buffer;
    }

    //    for (int i = 0; i < size; ++i){cout << names[i] << "  ";}

    char bufferMp[256];
    char bufferAnn[256];

    if (options[O_OUTFILEPREFIX]) {
        const char * prefix = outfilePrefix.c_str();
        if (options[O_CALC_ALL_STATES]) {
            sprintf(bufferMp, "%s.%s.BDD_MP.cudd", prefix, modus.c_str());
            sprintf(bufferAnn, "%s.%s.BDD_ANN.cudd", prefix, modus.c_str());
        } else {
            sprintf(bufferMp, "%s.R.%s.BDD_MP.cudd", prefix, modus.c_str());
            sprintf(bufferAnn, "%s.R.%s.BDD_ANN.cudd", prefix, modus.c_str());
        }
    } else {
        if (options[O_CALC_ALL_STATES]) {
            sprintf(bufferMp, "%s.%s.BDD_MP.cudd", PN->filename.c_str(), modus.c_str());
            sprintf(bufferAnn, "%s.%s.BDD_ANN.cudd", PN->filename.c_str(), modus.c_str());
        } else {
            sprintf(bufferMp, "%s.R.%s.BDD_MP.cudd", PN->filename.c_str(), modus.c_str());
            sprintf(bufferAnn, "%s.R.%s.BDD_ANN.cudd", PN->filename.c_str(), modus.c_str());
        }
    }

    cout << "\nsaving the BDDs... \n";
    FILE* fpMp = fopen(bufferMp, "w");
    Dddmp_VarInfoType varinfo = DDDMP_VARDEFAULT; //DDDMP_VARPERMIDS;
    Dddmp_cuddBddStore (mgrMp, /* DD Manager */
                        "bddMp", /* DD name (or NULL) */
                        bddMp, /* BDD root to be stored */
                        NULL, /* array of variable names (or NULL) */
                        NULL, //int *auxids /* array of converted var ids (optional) */
                        DDDMP_MODE_TEXT, /* storing mode selector */
                        varinfo, /* extra info for variables in text mode */
                        NULL, /* File name */
                        fpMp /* File pointer to the store file */
    );
    fclose(fpMp);

    FILE* fpAnn = fopen(bufferAnn, "w");
    Dddmp_cuddBddStore (mgrAnn, /* DD Manager */
                        "bddAnn", /* DD name (or NULL) */
                        bddAnn, /* BDD root to be stored */
                        names, /* array of variable names (or NULL) */
                        NULL, //int *auxids /* array of converted var ids (optional) */
                        DDDMP_MODE_TEXT, /* storing mode selector */
                        varinfo, /* extra info for variables in text mode */
                        NULL, /* File name */
                        fpAnn /* File pointer to the store file */
    );
    fclose(fpAnn);
    for (int i = 0; i < size; ++i) {
        delete[] names[i];
    }
    delete [] names;
    names = 0;
}


void BddRepresentation::printMemoryInUse() {
    cout << endl;
    //	cout << "Number of live nodes in mgrMp: " << Cudd_ReadNodeCount(mgrMp) << endl;
    //	cout << "Peak number of nodes in mgrMp: " << Cudd_ReadPeakNodeCount(mgrMp) << endl;
    //	cout << "Number of dead nodes in mgrMp: " << Cudd_ReadDead(mgrMp) << endl;
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

