/*****************************************************************************
 * Copyright 2005, 2006 Kathrin Kaschner                                     *
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
 * \file    BddRepresentation.cc
 *
 * \brief   functions for representing an OG as a BDD
 *
 * \author  responsible: Kathrin Kaschner <kathrin.kaschner@informatik.uni-rostock.de>
 *
 * \note    This file is part of the tool Fiona and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 */

#include "mynew.h"

#include <iostream>
#include <string>
#include "math.h"

#include "BddRepresentation.h" 
#include "BddLabel.h"
#include "graphEdge.h"
#include "symboltab.h" 
#include "owfn.h"

#include "options.h" 
#include "debug.h"
  
//comparison function object
struct cmp{
    bool operator()(char* x, char* y){
        return (strcmp(x,y)<0); // true falls x kleiner y, sonst false
    } 
};

 
//! \fn BddRepresentation::BddRepresentation(unsigned int numberOfLabels, Cudd_ReorderingType heuristic)
//! \param numberOfLabels
//! \param heuristic
//! \brief constructor
BddRepresentation::BddRepresentation(unsigned int numberOfLabels, Cudd_ReorderingType heuristic){
	trace(TRACE_5, "BddRepresentation::BddRepresentation(unsigned int numberOfLabels, Cudd_ReorderingType heuristic): begin\n");
	
	nbrLabels = numberOfLabels;
	maxLabelBits = nbrBits(numberOfLabels-1);
	maxNodeBits = 1;
	maxNodeNumber = 0;  
	 
	int sizeMp = 2 * maxNodeBits + maxLabelBits;
	int sizeAnn = nbrLabels + maxNodeBits;
	
	//init managers
	//mgrMp = Cudd_Init(sizeMp, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 30000000);   
	//mgrAnn = Cudd_Init(sizeAnn, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
	mgrMp = Cudd_Init(sizeMp, 0, 1000, 1000, 300000);   
	mgrAnn = Cudd_Init(sizeAnn, 0, 1000, 1000, 300000);
	//PrintMemoryInUse();
	
	//enable automatic dynamic reordering of the BDDs
	Cudd_AutodynEnable(mgrMp, heuristic);
	Cudd_AutodynEnable(mgrAnn, heuristic);		
	
	//init BDDs
	bddMp = Cudd_Not(Cudd_ReadOne(mgrMp)); //BDDstructure
	Cudd_Ref(bddMp);
	bddAnn = Cudd_Not(Cudd_ReadOne(mgrAnn)); //BDDannotation
	Cudd_Ref(bddAnn); 
	
	//nodeMap.insert(make_pair(root->getNumber(), 0));
	
	labelTable = new BddLabelTab(2*nbrLabels);
	
	assert(PN->placeInputCnt + PN->placeOutputCnt <= pow(double(2), double(sizeof(int)*8-1)) - 1); //PN->placeInputCnt + PN->placeOutputCnt <= 2^31 -1
	assert(nbrLabels ==  int(PN->placeInputCnt + PN->placeOutputCnt));
	
/*	//for a unigue coding of the labels
	list<char*> labelList;
	for (unsigned int i = 0; i < PN->placeInputCnt; ++i){
		//cout << i << "  " << PN->inputPlacesArray[i]->name << endl;
		labelList.push_back(PN->inputPlacesArray[i]->name);
	}
	
	for (unsigned int i = 0; i < PN->placeOutputCnt; ++i){
		//cout << i << "  " << PN->outputPlacesArray[i]->name << endl;
		labelList.push_back(PN->outputPlacesArray[i]->name);
	}
	
	labelList.sort(cmp());	
	
	
	labelTable = new BddLabelTab(2*nbrLabels);    
	BddLabel * label;
	//list<char*>::const_iterator list_iter = labelList.begin();
	
	//add labels and their bddNumber to labelTable
	for (unsigned int i = 0; i < PN->placeInputCnt + PN->placeOutputCnt; ++i){
		assert(list_iter != labelList.end());
		cout << i << "  " << *list_iter << endl;
		label = new BddLabel(*list_iter, i, labelTable);
		++list_iter;
	}
	
	labelList.clear();
*/
	
	//add the labels and their bddNumber to the labelTable
	BddLabel * label;
	for (unsigned int i = 0; i < PN->placeInputCnt; ++i){
		//cout << i << "  " << PN->inputPlacesArray[i]->name << endl;
		label = new BddLabel(PN->inputPlacesArray[i]->name, i, labelTable);
	}
	
	for (unsigned int i = 0; i < PN->placeOutputCnt; ++i){
		//cout << i + PN->placeInputCnt << "  " << PN->outputPlacesArray[i]->name << endl;
		label = new BddLabel(PN->outputPlacesArray[i]->name, i + PN->placeInputCnt, labelTable);
	}

/*	BddLabel * temp;
	for(unsigned int i = 0; i < labelTable->size;i++){
		BddLabel * temp;
		temp = labelTable->table[i];
		while(temp != 0){
			cout << i << "    " << temp->nbr <<"   " << temp->name << endl;
			temp = temp->next;	
		}
	}    
*/    
	trace(TRACE_5, "BddRepresentation::BddRepresentation(unsigned int numberOfLabels, Cudd_ReorderingType heuristic): end\n");
}

//! \fn BddRepresentation::~BddRepresentation()()
//! \brief destructor 
BddRepresentation::~BddRepresentation(){
	trace(TRACE_5, "BddRepresentation::~BddRepresentation(): begin\n");
	
	nodeMap.clear();
	
	Cudd_RecursiveDeref(mgrAnn, bddAnn);
	Cudd_RecursiveDeref(mgrMp, bddMp);
	
	//checkManager(mgrAnn, "mgrAnn");
	//checkManager(mgrMp, "mgrMp");
	
	Cudd_Quit(mgrAnn);
	Cudd_Quit(mgrMp);
	
	delete labelTable;
	
	trace(TRACE_5, "BddRepresentation::~BddRepresentation(): end\n"); 
}

void BddRepresentation::convertRootNode(vertex* root){
	trace(TRACE_5, "void BddRepresentation::convertRootNode(vertex* root): begin\n");
	
	nodeMap.insert(make_pair(root->getNumber(), 0));
	
	trace(TRACE_5, "void BddRepresentation::convertRootNode(vertex* root): end\n"); 
}

//! \fn void BddRepresentation::generateRepresentation(vertex* v, bool visitedNodes[])
//! \brief generate BDD representation
void BddRepresentation::generateRepresentation(vertex* v, bool visitedNodes[]){
	
	trace(TRACE_5, "BddRepresentation::generateRepresentation(vertex* v, bool visitedNodes[]): start\n");
	
	DdNode * annotation = annotationToBddAnn(v);
	DdNode* tmp = Cudd_bddOr(mgrAnn, annotation, bddAnn);
	//  if (tmp == NULL) exit(1);
	Cudd_Ref(tmp);
	Cudd_RecursiveDeref(mgrAnn, annotation);
	Cudd_RecursiveDeref(mgrAnn, bddAnn);
	bddAnn = tmp;	
	
	v->resetIteratingSuccNodes();
	if (v->getColor() == BLUE) {	
		if (v->reachGraphStateSet.size() != 0){
			visitedNodes[v->getNumber()] = 1;
			graphEdge* element;
			
			while((element = v->getNextEdge()) != NULL){;
				vertex* vNext = element->getNode();
				
				if (vNext->getColor() == BLUE && 
				    vNext->reachGraphStateSet.size() != 0 &&
				    vNext != NULL ) {
				    
					//cout << "edge: " << v->getNumber() << " [" << element->getLabel() << "> " << vNext->getNumber() << endl;

					//label	
					DdNode * label = labelToBddMp(element->getLabel().c_str()); 
		
					//nodes 
					DdNode * nodes = nodesToBddMp(v->getNumber(), vNext->getNumber());
					
					//edge
					DdNode * edge = Cudd_bddAnd(mgrMp, label, nodes); 
					// if (edge == NULL) exit(1);
					Cudd_Ref(edge);
					Cudd_RecursiveDeref(mgrMp, label);
					Cudd_RecursiveDeref(mgrMp, nodes);
					
					/*cout << "edge: " << v->getNumber() << " [" << element->getLabel() << "> " << vNext->getNumber() << endl;
					cout << "edge: " << getBddNumber(v->getNumber()) << " [" << element->getLabel() << "> "
						<< getBddNumber(vNext->getNumber()) << " -new-\n";
					Cudd_PrintMinterm(mgrMp, edge);
					cout << "--------------------------------\n";*/
						
					tmp = Cudd_bddOr(mgrMp, edge, bddMp);
					// if (tmp == NULL) exit(1);
					Cudd_Ref(tmp);
					Cudd_RecursiveDeref(mgrMp, edge);
					Cudd_RecursiveDeref(mgrMp, bddMp);
					bddMp = tmp;	
					if ((vNext != v) && !visitedNodes[vNext->getNumber()]){
						generateRepresentation(vNext, visitedNodes); 
					}   
				}
			}//end while
		}
	}
	trace(TRACE_5, "BddRepresentation::generateRepresentation(vertex* v, bool visitedNodes[]): end\n");	
}


//add blue edges to BDD and delete red edges from BDD
void BddRepresentation::addOrDeleteLeavingEdges(vertex* v){

	trace(TRACE_5, "BddRepresentation::addOrDeleteLeavingEdges(vertex* v): start\n");		
	
	//hier noch Annotation berechnen
	
	v->resetIteratingSuccNodes();
	
	if (v->reachGraphStateSet.size() != 0){
		
		graphEdge* element;
		
		while((element = v->getNextEdge()) != NULL){
			vertex* vNext = element->getNode();
			if (vNext != NULL){
				
				//cout << "current edge: " << v->getNumber() << " [" << element->getLabel() << "> " << vNext->getNumber() << endl;
								
				//label
				DdNode * label = labelToBddMp(element->getLabel().c_str()); 
				//if (label == NULL) exit(1)

				//nodes		    
				DdNode * nodes = nodesToBddMp(v->getNumber(), vNext->getNumber());
				// if (nodes == NULL) exit(1)		            
			
				//edge
				DdNode * edge = Cudd_bddAnd(mgrMp, label, nodes); 
				// if (edge == NULL) exit(1);
				Cudd_Ref(edge);
				Cudd_RecursiveDeref(mgrMp, label);
				Cudd_RecursiveDeref(mgrMp, nodes);
				
				cout << "edge: " << v->getNumber() << " [" << element->getLabel() << "> " << vNext->getNumber() << endl;
					cout << "edge: " << getBddNumber(v->getNumber()) << " [" << element->getLabel() << "> "
						<< getBddNumber(vNext->getNumber()) << " -new-\n";
					Cudd_PrintMinterm(mgrMp, edge);
					
					
				if (v->getColor() == BLUE && vNext->getColor() == BLUE ) { //add blue edges
					cout << "add current edge " << v->getNumber() << "->" << vNext->getNumber() << endl;
					cout << "--------------------------------\n";
					DdNode* tmp = Cudd_bddOr(mgrMp, edge, bddMp);
					// if (tmp == NULL) exit(1);
					Cudd_Ref(tmp);
					Cudd_RecursiveDeref(mgrMp, edge);
					Cudd_RecursiveDeref(mgrMp, bddMp);
					bddMp = tmp;	 
				} 
				else { //delete red edge (vermutl. unnötig, da keine roten Kanten im BDD)
					cout << "delete edge " << v->getNumber() << "->" << vNext->getNumber() << endl;
					cout << "--------------------------------\n";
					DdNode* tmp = Cudd_bddAnd(mgrMp, bddMp, Cudd_Not(edge));
					// if (tmp == NULL) exit(1);
					Cudd_Ref(tmp);
					Cudd_RecursiveDeref(mgrMp, edge);
					Cudd_RecursiveDeref(mgrMp, bddMp);
					bddMp = tmp;	   
				}
			} 
		}//end while 
	}
	trace(TRACE_5, "BddRepresentation::addOrDeleteLeavingEdges(vertex* v): end\n");
}


//! \fn DdNode*  BddRepresentation::labelToBddMp(char* label)
//! \brief returns the BDD of a label (given as integer)
DdNode* BddRepresentation::labelToBddMp(const char* label) {
	
	trace(TRACE_5, "BddRepresentation::labelToBddMp(char* label): start\n");
	 
	BddLabel * s = labelTable->lookup(label);
	unsigned int number = s->nbr;
	BitVector assignment = numberToBin(number, maxLabelBits);
		
	DdNode*  f = Cudd_ReadOne(mgrMp);
	Cudd_Ref(f);
	DdNode* tmp;

	//assignment to BDD
	for (int i = 0; i < maxLabelBits; ++i){
		if (assignment[i] == false){
			tmp = Cudd_bddAnd(mgrMp, Cudd_Not(Cudd_bddIthVar(mgrMp,i)), f);
		}
		else{
			tmp = Cudd_bddAnd(mgrMp, Cudd_bddIthVar(mgrMp,i), f);
		}
		// if (tmp == NULL) exit(1);
		Cudd_Ref(tmp);
		Cudd_RecursiveDeref(mgrMp, f);
		f = tmp;
	}

	trace(TRACE_5, "BddRepresentation::labelToBddMp(char* label): end\n");    
	return (f);
}


//! \fn DdNode* BddRepresentation::nodesToBddMp(unsigned int node1, unsigned int node2)
//! \brief returns the BDD of the nodes (given as integer) of an edge
DdNode* BddRepresentation::nodesToBddMp(unsigned int node1, unsigned int node2){
	
	trace(TRACE_5, "BddRepresentation::nodesToBddMp(unsigned int node1, unsigned int node2): start\n");
	
	unsigned int bddNumber1 = getBddNumber(node1);   
	unsigned int bddNumber2 = getBddNumber(node2);
    
	unsigned int max;	
	if (bddNumber1 > bddNumber2){
		max = bddNumber1;
	}
	else {
		max = bddNumber2;
	}
		
	//cout << "max: " << max  << "  maxNodeNumber-old-: " << maxNodeNumber << endl;
    
	if (max > maxNodeNumber){
		addBddVars(max);
	}

	BitVector assignment1 = numberToBin(bddNumber1, maxNodeBits);
	BitVector assignment2 = numberToBin(bddNumber2, maxNodeBits);

	//calculate the BDD for assignment1 and assignment2
	DdNode* f = Cudd_ReadOne(mgrMp);
	Cudd_Ref(f);
	DdNode* tmp;
	
	int j;
	for (int i = 0; i < maxNodeBits; ++i){
		j = maxNodeBits-1-i;
		assert(j >= 0);
		if (assignment1[j] == false){
			tmp = Cudd_bddAnd(mgrMp, Cudd_Not(Cudd_bddIthVar(mgrMp, maxLabelBits+(2*i))), f);
		}
		else{
			tmp = Cudd_bddAnd(mgrMp, Cudd_bddIthVar(mgrMp, maxLabelBits+(2*i)), f);
		}
		// if (tmp == NULL) exit(1);        
		Cudd_Ref(tmp);
		Cudd_RecursiveDeref(mgrMp, f);
		f = tmp; 
		
		if (assignment2[j] == false){
			tmp = Cudd_bddAnd(mgrMp, Cudd_Not(Cudd_bddIthVar(mgrMp, maxLabelBits+1+(2*i))), f);
		}
		else{
			tmp = Cudd_bddAnd(mgrMp, Cudd_bddIthVar(mgrMp, maxLabelBits+1+(2*i)), f);
		}
		// if (tmp == NULL) exit(1);
		Cudd_Ref(tmp);
		Cudd_RecursiveDeref(mgrMp, f);
		f = tmp;
	}
	trace(TRACE_5, "BddRepresentation::nodesToBddMp(unsigned int node1, unsigned int node2): end\n");	   
	return (f);
}


DdNode* BddRepresentation::annotationToBddAnn(vertex* v){
	trace(TRACE_5, "DdNode* BddRepresentation::annotationToBddAnn(vertex * v): start\n");
	
//	cout << "----------------------------------\n";
//	cout << "node " << v->getNumber() << " ("<< getBddNumber(v->getNumber()) << ") : " << v->getCNF()<< endl;
	
	DdNode* tmp;
	DdNode* CNFTemp;
	
	DdNode* annotation = Cudd_ReadOne(mgrAnn);		
	Cudd_Ref(annotation);
	
	CNF* cl = v->getAnnotation();
		
	while (cl) {
		CNFTemp = CNFtoBddAnn(cl);
		tmp = Cudd_bddAnd(mgrAnn, annotation, CNFTemp);
		// if (tmp == NULL) exit(1);
		Cudd_Ref(tmp);
		Cudd_RecursiveDeref(mgrAnn, annotation);
		Cudd_RecursiveDeref(mgrAnn, CNFTemp);
		annotation = tmp;
		
		cl = cl->nextElement;	
	}
		
	unsigned int bddNumber = getBddNumber(v->getNumber());   
	//cout << "bddNumber: " << bddNumber  << "  maxNodeNumber(old): " << maxNodeNumber << endl;
    
	if (bddNumber > maxNodeNumber){
		addBddVars(bddNumber);
	}
	BitVector assignment = numberToBin(bddNumber, maxNodeBits);

	//calculate the BDD for assignment
	DdNode* f = Cudd_ReadOne(mgrAnn);
	Cudd_Ref(f);
	
	int j;
	for (int i = 0; i < maxNodeBits; ++i){
		j = maxNodeBits-1-i;
		assert(j >= 0);
		if (assignment[j] == false){
			tmp = Cudd_bddAnd(mgrAnn, Cudd_Not(Cudd_bddIthVar(mgrAnn, nbrLabels+i)), f);
		}
		else{
			tmp = Cudd_bddAnd(mgrAnn, Cudd_bddIthVar(mgrAnn, nbrLabels+i), f);
		}
		// if (tmp == NULL) exit(1);        
		Cudd_Ref(tmp);
		Cudd_RecursiveDeref(mgrAnn, f);
		f = tmp;         
	}
		
	tmp = Cudd_bddAnd(mgrAnn, annotation, f);
	//  if (tmp == NULL) exit(1);	
	Cudd_Ref(tmp);
	Cudd_RecursiveDeref(mgrAnn, annotation);
	Cudd_RecursiveDeref(mgrAnn, f);
	annotation = tmp;
	
//	cout << "annotation: \n"; Cudd_PrintMinterm(mgrAnn, annotation);
	
	trace(TRACE_5, "DdNode* BddRepresentation::annotationToBddAnn(vertex * v): end\n");		 
	return annotation;	
}


DdNode* BddRepresentation::CNFtoBddAnn(CNF* cl){
	DdNode* tmp;
	
	if (cl == NULL) {		// since there is no clause we can't conclude anything
		tmp = Cudd_Not(Cudd_ReadOne(mgrAnn));
		Cudd_Ref(tmp);
		return tmp;  //"(false)";	
	}
	
	if (cl->isFinalState) {
		tmp = Cudd_ReadOne(mgrAnn);
		Cudd_Ref(tmp);
		return tmp;  //"(true)";
	}

	DdNode* clause1 = Cudd_Not(Cudd_ReadOne(mgrAnn));
    Cudd_Ref(clause1);
    
	clause* literal = cl->cl;  // get the first literal of the clause

    while (literal) {
        if (literal->edge != NULL && 
        	literal->edge->getNode() != NULL && 
        	literal->edge->getNode()->getColor() != RED && 
        	literal->edge->getNode()->reachGraphStateSet.size() > 0) {
        			
//            cout << "search for label " << literal->edge->getLabel() << " ...";
            BddLabel* label = labelTable->lookup(literal->edge->getLabel().c_str());
            //if (!l) { cout << "  Label not found\n"; exit(1);}
//            cout << "   found: nbr = " << label->nbr << endl;
            int i = label->nbr;
            tmp = Cudd_bddOr(mgrAnn, clause1, Cudd_bddIthVar(mgrAnn,i));
            Cudd_Ref(tmp);
    		Cudd_RecursiveDeref(mgrAnn, clause1);
    		clause1 = tmp;
        }   	
    	literal = literal->nextElement;	
    }
      
//	cout<< "clause: \n"; Cudd_PrintMinterm(mgrAnn, clause1);
	return clause1; 
}


//! \fn int BddRepresentation::getBddNumber(unsigned int node)
//! \param node
unsigned int BddRepresentation::getBddNumber(unsigned int node){
	
	trace(TRACE_5, "BddRepresentation::getBddNumber(unsigned int node): begin\n");
	
	map<unsigned int, unsigned int>::const_iterator map_iter;
	map_iter = nodeMap.find(node);    //search node in nodeMap
	
	unsigned int bddNumber;
	if (map_iter == nodeMap.end()){ //if node not found add node with a new bddNumber
	pair<map<unsigned int, unsigned int>::iterator, bool> success;
	bddNumber = nodeMap.size();
	success = nodeMap.insert(make_pair(node, bddNumber));
	//if (!(success.second)){exit(1);}
	}
	else {
        	bddNumber = map_iter -> second;
	}
	
	trace(TRACE_5, "BddRepresentation::getBddNumber(unsigned int node): end\n");
	return(bddNumber);
}



void BddRepresentation::addBddVars(unsigned int max){
	//cout << "void BddRepresentation::addBddVars(unsigned int max): begin\n";
	//cout << "maxNodeBits-old-: " << maxNodeBits << "   maxNodeNumber-old-: " << maxNodeNumber << endl;
	int neededNodeBits = nbrBits(max);
	//cout << "neededNodeBits: " << neededNodeBits << "   maxNodeNumber-new-: " << max << endl;
	DdNode* tmp1 = Cudd_ReadOne(mgrMp);
	Cudd_Ref(tmp1);
	DdNode* tmp2;
	DdNode* var;
	//add necessary BDD variables to BddMp
	for(int i = 0; i < 2*(neededNodeBits - maxNodeBits); ++i){
		var = Cudd_bddNewVar(mgrMp);
//		    if (var == NULL ) exit(1);
		tmp2 = Cudd_bddAnd(mgrMp, tmp1, Cudd_Not(var));
		Cudd_Ref(tmp2);
		Cudd_RecursiveDeref(mgrMp, tmp1);
		tmp1 = tmp2;		    		
	}
	 
	tmp2 = Cudd_bddAnd(mgrMp, tmp1, bddMp);
	Cudd_Ref(tmp2);
	Cudd_RecursiveDeref(mgrMp, tmp1); 
	Cudd_RecursiveDeref(mgrMp, bddMp);
	bddMp = tmp2;
	
	tmp1 = Cudd_ReadOne(mgrAnn);
	Cudd_Ref(tmp1);
	//add necessary BDD variables to BddAnn
	for(int i = 0; i < (neededNodeBits - maxNodeBits); ++i){
		var = Cudd_bddNewVar(mgrAnn);
//		    if (var == NULL ) exit(1);
		tmp2 = Cudd_bddAnd(mgrAnn, tmp1, Cudd_Not(var));
		Cudd_Ref(tmp2);
		Cudd_RecursiveDeref(mgrAnn, tmp1);
		tmp1 = tmp2;		    		
	}
	 
	tmp2 = Cudd_bddAnd(mgrAnn, tmp1, bddAnn);
	Cudd_Ref(tmp2);
	Cudd_RecursiveDeref(mgrAnn, tmp1); 
	Cudd_RecursiveDeref(mgrAnn, bddAnn);
	bddAnn = tmp2;
	
	maxNodeBits = neededNodeBits;
	maxNodeNumber = (unsigned int) pow((double)2, (double)neededNodeBits)-1;
	//cout << "maxNodeNumber-new-: " << maxNodeNumber << endl;
	//cout << "maxNodeBits-new-: " << maxNodeBits << endl;
	//Cudd_PrintMinterm(mgrMp, bddMp);
	//Cudd_PrintMinterm(mgrAnn, bddAnn);
	//cout << "void BddRepresentation::addBddVars(unsigned int max): end\n";
}	


//! \fn BitVector BddRepresentation::numberToBin(unsigned int number, int count)
//! \brief returns the binary representation of a number **/
BitVector BddRepresentation::numberToBin(unsigned int number, int cntBits){	
	trace(TRACE_5, "BddRepresentation::numberToBin(unsigned int number, int cntBits): start\n");
	//cout << "BddRepresentation::numberToBin(unsigned int number, int cntBits): start\n";
	//cout << "number: " << number << "   cntBits: " << cntBits << endl;
	
	BitVector assignment = BitVector(cntBits);
	int base = 2;

	//calculate the binary representation
	int index = cntBits - 1;
	do {
		assert(index >= 0);
		assignment[index--] = number % base;
		number /= base;
	} while ( number );
	
	//add leading zeros
	for (int i = index; i >= 0; --i){
		assignment[i] = false;
	}
	//cout << "BddRepresentation::numberToBin(unsigned int number, int cntBits): end\n";
	trace(TRACE_5, "BddRepresentation::numberToBin(unsigned int number, int cntBits): end\n");
	return (assignment);
	}

//! \fn int BddRepresentation::nbrBits(unsigned int i)
//! \brief returns the number of bits to represent a number i
int BddRepresentation::nbrBits(unsigned int i){
	switch (i){
		case(0): return(1);
		case(1): return(1);
		default:{
			//const int max = (int)floor(log((double)(i))/log((double)2))+1;
			unsigned int nbrBits = 0;
			unsigned int nbr = i;
			while (nbr > 0){
				nbr = nbr >> 1;
				++nbrBits;
				//cout << "nbrBits :" << nbrBits << endl;
			}
			//cout << "nbrBits of " << i << ":  " << nbrBits << endl;
			return (nbrBits);
		}
	}
}


//! \fn void BddRepresentation::checkManager(DdManager* mgr, char* table)
//! \param mgr
//! \param table
void BddRepresentation::checkManager(DdManager* mgr, char* table){
    cout << "\ncheck " <<table << ":\n";
    if (Cudd_CheckZeroRef(mgr) != 0){
        cout << "WARNING: Cudd_CheckZeroRef("<< table << ") = "
                << Cudd_CheckZeroRef(mgr) << "\n";
    }
    cuddHeapProfile(mgr);
    Cudd_DebugCheck(mgr);
}

//! \fn void BddRepresentation::reorder(Cudd_ReorderingType heuristic)
//! \param heuristic
void BddRepresentation::reorder(Cudd_ReorderingType heuristic){
/*
     if (filename == "bddMp.dot"){
         //Variablen im bddMp Gruppieren:
         //die Variablen für den zweiten Knoten sind im BDD immer unten => günstig für Restrict beim Matching
         Cudd_MakeTreeNode(mgrMp, 0 ,maxChannelBits + maxNodeBits, MTR_DEFAULT); //1. Gruppe: innerhalb der Gruppe ist die Variablenordnung beliebig
         Cudd_MakeTreeNode(mgrMp, maxChannelBits + maxNodeBits, maxNodeBits, MTR_DEFAULT); //2. Gruppe: innerhalb der Gruppe ist die Variablenordnung beliebig
         Cudd_MakeTreeNode(mgrMp, 0 ,maxNodeBits, MTR_FIXED);   //1. und 2. Gruppe sind wieder in einer Gruppe, in dieser ist aber die Reihenfolge der Elemente (hier die beiden Gruppen) fest
     }
*/
    Cudd_ReduceHeap(mgrMp, heuristic, 0);
    cout << "BDD_MP: number of nodes: " << Cudd_DagSize(bddMp);
    cout << "\t" << Cudd_ReadReorderingTime(this->mgrMp) << " ms consumed for variable reordering" << endl;
    
    Cudd_ReduceHeap(this->mgrAnn, heuristic, 0);
    cout << "BDD_ANN: number of nodes: "  << Cudd_DagSize(this->bddAnn);
    cout << "\t" << Cudd_ReadReorderingTime(this->mgrAnn) << " ms consumed for variable reordering" << endl;
}


//! \fn void BddRepresentation::printDotFile(char** varNames)
//! \brief creates dot files of the BDDs
//    mit dot -Tps fileName -o neu.ps kann das BDD graphisch dargestellt werden
void BddRepresentation::printDotFile(char** varNames){
	if ((Cudd_DagSize(bddMp) < 200000)&&(Cudd_DagSize(bddAnn) < 200000) ) {
	
		char bufferMp[256]; 
		char bufferAnn[256];

		if (options[O_CALC_ALL_STATES]) {
	                sprintf(bufferMp, "%s.a.OG.BDD_MP.out", netfile);
	                sprintf(bufferAnn, "%s.a.OG.BDD_ANN.out", netfile); 
	            } else {
	                sprintf(bufferMp, "%s.OG.BDD_MP.out", netfile);
	                sprintf(bufferAnn, "%s.OG.BDD_ANN.out", netfile);
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
            trace(TRACE_0, "\ncreating the dot file of BDD_MP and BDD_ANN...\n");
            if (options[O_CALC_ALL_STATES]) {
                sprintf(bufferMp, "dot -Tpng %s.a.OG.BDD_MP.out -o %s.a.OG.BDD_MP.png", netfile, netfile);
                sprintf(bufferAnn, "dot -Tpng %s.a.OG.BDD_ANN.out -o %s.a.OG.BDD_ANN.png", netfile, netfile);
            } else {
                sprintf(bufferMp, "dot -Tpng %s.OG.BDD_MP.out -o %s.OG.BDD_MP.png", netfile, netfile);
                sprintf(bufferAnn, "dot -Tpng %s.OG.BDD_ANN.out -o %s.OG.BDD_ANN.png", netfile, netfile);
            }
         
            trace(TRACE_0, bufferMp); trace(TRACE_0, "\n");
            trace(TRACE_0, bufferAnn); trace(TRACE_0, "\n");
            system(bufferMp);
            system(bufferAnn);
         
        } else {
            trace(TRACE_0, "\nBDDs are too big to have dot create the graphics\n");
        }
    } else {
        trace(TRACE_0, "\nBDDs are too big to create dot file\n");
    }      
}	

//! \fn void BddRepresentation::print()
//! \brief print bdd_Mp and bdd_Ann
void BddRepresentation::print(){
//TODO für Ausgabe (zur leichteren Lesbarkeit) Variablenordnung verändern: node1 label node2
	cout << "\nBDD_MP:\n";	
	Cudd_PrintMinterm(mgrMp, bddMp);
	cout << "\nBDD_ANN:\n"; 
	Cudd_PrintMinterm(mgrAnn, bddAnn);
}


//! \fn void BddRepresentation::save()
//! \brief save bddMp and bddAnn in file
void BddRepresentation::save(){
	int size = nbrLabels + maxNodeBits;
	char** names = new char*[size];

	assert(PN->placeInputCnt + PN->placeOutputCnt <= pow(double(2), double(sizeof(int)*8-1)) - 1); //PN->placeInputCnt + PN->placeOutputCnt <= 2^31 -1
	
    for (int i = 0; i < int(PN->placeInputCnt); ++i){
    	assert(i < nbrLabels + maxNodeBits);
    	//cout << "i: " << i << "   name: " << PN->inputPlacesArray[i]->name << "   nbr: " << labelTable->lookup(PN->inputPlacesArray[i]->name)->nbr << endl;
    	char* tmp = new char [PN->inputPlacesArray[i]->name.size() + 2];
    	strcpy (tmp,"!");
    	strcat (tmp, PN->inputPlacesArray[i]->name.c_str());
    	unsigned int nbr = labelTable->lookup(PN->inputPlacesArray[i]->name)->nbr;
        names[nbr] = tmp;
    }
    
    for (int i = 0; i < int(PN->placeOutputCnt); ++i){
		assert(int(i+PN->placeInputCnt) < nbrLabels + maxNodeBits);
		//cout << "i: " << i << "   name: " << PN->outputPlacesArray[i]->name << "   nbr: " << labelTable->lookup(PN->outputPlacesArray[i]->name)->nbr << endl;
    	char* tmp = new char [PN->outputPlacesArray[i]->name.size() + 2];
    	strcpy (tmp,"?");
    	strcat (tmp, PN->outputPlacesArray[i]->name.c_str());
    	unsigned int nbr = labelTable->lookup(PN->outputPlacesArray[i]->name)->nbr;
        names[nbr] = tmp;
    }
    

	assert((unsigned int)nbrLabels == PN->placeInputCnt+PN->placeOutputCnt);
	assert(Cudd_ReadSize(mgrAnn) == nbrLabels + maxNodeBits);
    for (int i = nbrLabels; i < size; ++i){
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
	
	if (options[O_CALC_ALL_STATES]) {
                sprintf(bufferMp, "%s.a.OG.BDD_MP.cudd", netfile);
                sprintf(bufferAnn, "%s.a.OG.BDD_ANN.cudd", netfile); 
    } else {
        sprintf(bufferMp, "%s.OG.BDD_MP.cudd", netfile);
        sprintf(bufferAnn, "%s.OG.BDD_ANN.cudd", netfile);
    }
    
   	cout << "\nsaving the BDDs... \n";
    FILE* fpMp = fopen(bufferMp, "w");
    Dddmp_VarInfoType varinfo = DDDMP_VARDEFAULT; //DDDMP_VARPERMIDS;
    Dddmp_cuddBddStore (
            mgrMp,              /* DD Manager */
            "bddMp",            /* DD name (or NULL) */
            bddMp,              /* BDD root to be stored */
            NULL,               /* array of variable names (or NULL) */
            NULL, //int *auxids /* array of converted var ids (optional) */
            DDDMP_MODE_TEXT,    /* storing mode selector */
            varinfo,            /* extra info for variables in text mode */
            NULL,               /* File name */
            fpMp                /* File pointer to the store file */
     );
     fclose(fpMp);

     FILE* fpAnn = fopen(bufferAnn, "w");
     Dddmp_cuddBddStore (
            mgrAnn,             /* DD Manager */
            "bddAnn",           /* DD name (or NULL) */
            bddAnn,             /* BDD root to be stored */
            names,              /* array of variable names (or NULL) */
            NULL, //int *auxids /* array of converted var ids (optional) */
            DDDMP_MODE_TEXT,    /* storing mode selector */
            varinfo,            /* extra info for variables in text mode */
            NULL,               /* File name */
            fpAnn               /* File pointer to the store file */
     );
     fclose(fpAnn);
     for (int i = 0; i < size; ++i){
     	delete[] names[i];	
     }
     delete [] names;
     names = 0;
}

void BddRepresentation::PrintMemoryInUse(){
	cout << "\nNumber of live nodes in mgrMp: " << Cudd_ReadNodeCount(mgrMp)<<endl;
	cout << "Peak number of nodes in mgrMp: " << Cudd_ReadPeakNodeCount(mgrMp)<<endl;
	cout << "Memory in use for mgrMp:  " << Cudd_ReadMemoryInUse(mgrMp)<< endl;
	cout << "Memory in use for mgrAnn: " << Cudd_ReadMemoryInUse(mgrAnn)<< endl;
	cout << "Memory in use for both BDD: " << Cudd_ReadMemoryInUse(mgrMp) + Cudd_ReadMemoryInUse(mgrAnn)<< endl;
}
