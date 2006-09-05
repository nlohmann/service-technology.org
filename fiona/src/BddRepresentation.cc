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
#include <cassert> 
  
//extern char* netfile;  
 
//! \fn BddRepresentation::BddRepresentation(vertex * root, int nbrLabels, Cudd_ReorderingType heuristic)
//! \brief constructor
BddRepresentation::BddRepresentation(vertex * root, unsigned int numberOfLabels, Cudd_ReorderingType heuristic){
	nbrLabels = numberOfLabels;
	maxLabelBits = nbrBits(numberOfLabels-1);
	maxNodeBits = 1;
	maxNodeNumber = 0;
	
	int sizeMp = 2 * maxNodeBits + maxLabelBits;
    int sizeAnn = nbrLabels + maxNodeBits;
    
    //init managers
    mgrMp = Cudd_Init(sizeMp, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);   
    mgrAnn = Cudd_Init(sizeAnn, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0); 
    
    //enable automatic dynamic reordering of BDDs
    Cudd_AutodynEnable(mgrMp, heuristic);
    Cudd_AutodynEnable(mgrAnn, heuristic);
    
    //init BDDs
    bddMp = Cudd_Not(Cudd_ReadOne(mgrMp)); //BDDstructure
    Cudd_Ref(bddMp);
    bddAnn = Cudd_Not(Cudd_ReadOne(mgrAnn)); //BDDannotation
    Cudd_Ref(bddAnn); 
     
    nodeMap.insert(make_pair(root->getNumber(), 0));
    
    labelTable = new BddLabelTab(2*nbrLabels);
       
    //add labels and their bddNumber to labelTable
    BddLabel * label;
    for (unsigned int i = 0; i < PN->placeInputCnt; ++i){
    	//cout << i << "  " << PN->inputPlacesArray[i]->name << endl;
    	label = new BddLabel(PN->inputPlacesArray[i]->name, i, labelTable);
    }
    
    for (unsigned int i = 0; i < PN->placeOutputCnt; ++i){
    	//cout << i + PN->placeInputCnt << "  " << PN->outputPlacesArray[i]->name << endl;
    	label = new BddLabel(PN->outputPlacesArray[i]->name, i + PN->placeInputCnt, labelTable);
    }
/*
  	BddLabel * temp;
    for(unsigned int i = 0; i < labelTable->size;i++){
		BddLabel * temp;
		temp = labelTable->table[i];
		while(temp != 0){
			cout << i << "    " << temp->nbr <<"   " << temp->name << endl;
			temp = temp->next;			
		}	
	}
*/ 
}

//! \fn BddRepresentation::~BddRepresentation()()
//! \brief destructor 
BddRepresentation::~BddRepresentation(){

	nodeMap.clear();
	
    Cudd_RecursiveDeref(mgrAnn, bddAnn);
    Cudd_RecursiveDeref(mgrMp, bddMp);

    //checkManager(mgrAnn, "mgrAnn");
    //checkManager(mgrMp, "mgrMp");

    Cudd_Quit(mgrAnn);
    Cudd_Quit(mgrMp);
    
    delete labelTable; 
}


//add blue edges to BDD and delete red edges from BDD
void BddRepresentation::addOrDeleteLeavingEdges(vertex* v){
	
		trace(TRACE_5, "BddRepresentation::addOrDeleteLeavingEdges(vertex* v): start\n");		
		
		v->resetIteratingSuccNodes();
		
		if (v->reachGraphStateSet.size() != 0){
			
			graphEdge* element;
			
			while((element = v->getNextEdge()) != NULL){
				
				vertex* vNext = element->getNode();
				if (vNext != NULL){ //&& vNext->reachGraphStateSet.size() != 0 
			
				    //label						
		            DdNode * label = labelToBddMp(element->getLabel()); 
//                  if (label == NULL) exit(1)

		            //nodes		    
		            DdNode * nodes = nodesToBddMp(v->getNumber(), vNext->getNumber());
//                  if (nodes == NULL) exit(1)		            
		            
		            //edge
		            DdNode * edge = Cudd_bddAnd(mgrMp, label, nodes); 
//                  if (edge == NULL) exit(1);
		            Cudd_Ref(edge);
		            Cudd_RecursiveDeref(mgrMp, label);
		            Cudd_RecursiveDeref(mgrMp, nodes);
					/*
		            cout << "bddMp " << v->getNumber() << " [" << element->getLabel() << "> "
		                 << vNext->getNumber() << " :\t"; Cudd_PrintMinterm(this->mgrMp, edge);
		            cout << "--------------------------------\n";
					*/
					
					if (v->getColor() == BLUE && vNext->getColor() == BLUE ) { //add blue edges
		            	DdNode* tmp = Cudd_bddOr(mgrMp, edge, bddMp);
//                 	    if (tmp == NULL) exit(1);
		            	Cudd_Ref(tmp);
		           	 	Cudd_RecursiveDeref(mgrMp, edge);
		            	Cudd_RecursiveDeref(mgrMp, bddMp);
		            	bddMp = tmp;	 
		            
		            	//cout << "add edge " << v->getNumber() << "->" << vNext->getNumber() << endl;
	                             		 
					} else { //delete red edge (vermutl. unn�tig, da keine roten Kanten im BDD)
						DdNode* tmp = Cudd_bddAnd(mgrMp, bddMp, Cudd_Not(edge));
//                      if (tmp == NULL) exit(1);
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
			
			while((element = v->getNextEdge()) != NULL){
				
				vertex* vNext = element->getNode();
				
				if (vNext->getColor() == BLUE && 
				    vNext->reachGraphStateSet.size() != 0 &&
				    vNext != NULL ) {
				    						
				    //label						
		            DdNode * label = labelToBddMp(element->getLabel()); 

		            //nodes		    
		            DdNode * nodes = nodesToBddMp(v->getNumber(), vNext->getNumber());
		            
		            //edge
		            DdNode * edge = Cudd_bddAnd(mgrMp, label, nodes); 
//                  if (edge == NULL) exit(1);
		            Cudd_Ref(edge);
		            Cudd_RecursiveDeref(mgrMp, label);
		            Cudd_RecursiveDeref(mgrMp, nodes);
		            
		       /*   cout << "Kante: " << v->getNumber() << " [" << element->getLabel() << "> "
		                 << vNext->getNumber() << endl; 
		            cout << "Kante: " << getBddNumber(v->getNumber()) << " [" << element->getLabel() << "> "
		                 << getBddNumber(vNext->getNumber()) << " (neu)\n";
		            Cudd_PrintMinterm(mgrMp, edge);
		            cout << "--------------------------------\n";
			*/
		            tmp = Cudd_bddOr(mgrMp, edge, bddMp);
//                  if (tmp == NULL) exit(1);
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

//! \fn DdNode*  BddRepresentation::labelToBddMp(char* label)
//! \brief returns the BDD of a label (given as integer)
DdNode* BddRepresentation::labelToBddMp(char* label) {
	
	trace(TRACE_5, "BddRepresentation::labelToBddMp(char* label): start\n");
	 
	BddLabel * s = labelTable->lookup(label);
    unsigned int number = s->nbr;
	BitVector assignment = numberToBin(number, maxLabelBits);
		
	DdNode*  f = Cudd_ReadOne(mgrMp);
    Cudd_Ref(f);
    DdNode* tmp;

	//assignment to BDD
	for (int i = 0; i < maxLabelBits; ++i){     //TODO: Schleife Absteigend durchlaufen (--i)
        if (assignment[i] == false){
            tmp = Cudd_bddAnd(mgrMp, Cudd_Not(Cudd_bddIthVar(mgrMp,i)), f);
        }
        else{
            tmp = Cudd_bddAnd(mgrMp, Cudd_bddIthVar(mgrMp,i), f);
        }
//      if (tmp == NULL) exit(1);
        Cudd_Ref(tmp);
        Cudd_RecursiveDeref(mgrMp, f);
        f = tmp;
    }

	trace(TRACE_5, "BddRepresentation::labelToBddMp(char* label): end\n");    
    return (f);
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
            BddLabel* label = labelTable->lookup(literal->edge->getLabel());
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
//      if (tmp == NULL) exit(1);
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
    for (int i = 0; i < maxNodeBits; ++i){     //TODO: schleife Absteigend durchlaufen (--i)
    	j = maxNodeBits-1-i;
    	assert(j >= 0);
        if (assignment[j] == false){
            tmp = Cudd_bddAnd(mgrAnn, Cudd_Not(Cudd_bddIthVar(mgrAnn, nbrLabels+i)), f);
        }
        else{
            tmp = Cudd_bddAnd(mgrAnn, Cudd_bddIthVar(mgrAnn, nbrLabels+i), f);
        }
//      if (tmp == NULL) exit(1);        
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

//! \fn 
//! \param 
//! \brief
unsigned int BddRepresentation::getBddNumber(unsigned int node){
	
	trace(TRACE_5, "BddRepresentation::getBddNumber(unsigned int node): begin\n");
	
    map<unsigned int, unsigned int>::const_iterator map_iter;
    map_iter = nodeMap.find(node);    //search node in nodeMap
    
    unsigned int bddNumber;
    if (map_iter == nodeMap.end()){ //if node not found add node with a new bddNumber
        pair<map<unsigned int, unsigned int>::iterator, bool> success;
        bddNumber = nodeMap.size();
        success = nodeMap.insert(make_pair(node, bddNumber));
//      if (!(success.second)){exit(1);}
    }
    else {
        bddNumber = map_iter -> second;
    }
    trace(TRACE_5, "BddRepresentation::getBddNumber(unsigned int node): end\n");
    return(bddNumber);
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
	}else {max = bddNumber2;}
		
	//cout << "max: " << max  << "  maxNodeNumber(old): " << maxNodeNumber << endl;
    
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
    for (int i = 0; i < maxNodeBits; ++i){     //TODO: schleife Absteigend durchlaufen (--i)
    	assert(j >= 0);
    	j = maxNodeBits-1-i;
        if (assignment1[j] == false){
            tmp = Cudd_bddAnd(mgrMp, Cudd_Not(Cudd_bddIthVar(mgrMp, maxLabelBits+(2*i))), f);
        }
        else{
            tmp = Cudd_bddAnd(mgrMp, Cudd_bddIthVar(mgrMp, maxLabelBits+(2*i)), f);
        }
//      if (tmp == NULL) exit(1);        
        Cudd_Ref(tmp);
        Cudd_RecursiveDeref(mgrMp, f);
        f = tmp; 
        
        if (assignment2[j] == false){
            tmp = Cudd_bddAnd(mgrMp, Cudd_Not(Cudd_bddIthVar(mgrMp, maxLabelBits+1+(2*i))), f);
        }
        else{
            tmp = Cudd_bddAnd(mgrMp, Cudd_bddIthVar(mgrMp, maxLabelBits+1+(2*i)), f);
        }
//      if (tmp == NULL) exit(1);
        Cudd_Ref(tmp);
        Cudd_RecursiveDeref(mgrMp, f);
        f = tmp;
        
    }
	trace(TRACE_5, "BddRepresentation::nodesToBddMp(unsigned int node1, unsigned int node2): end\n");		   
    return (f);
}

void BddRepresentation::addBddVars(unsigned int max){
    int neededNodeBits = nbrBits(max); 	    	
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
	//cout << "maxNodeNumber(new): " << maxNodeNumber << endl;
	//cout << "maxNodeBits(new): " << maxNodeBits << endl;
	//Cudd_PrintMinterm(mgrMp, bddMp);
	//Cudd_PrintMinterm(mgrAnn, bddAnn);
}	


//! \fn BitVector BddRepresentation::numberToBin(unsigned int number, int count)
//! \brief returns the binary representation of a number **/
BitVector BddRepresentation::numberToBin(unsigned int number, int cntBits){
	
	trace(TRACE_5, "BddRepresentation::numberToBin(unsigned int number, int cntBits): start\n");
	
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
            const int max = (int)floor(log((double)(i))/log((double)2))+1;
            //cout << i << "  " << max << endl;
            return (max);
        }
    }
}

//! \fn void BddRepresentation::reorder(Cudd_ReorderingType heuristic)
void BddRepresentation::reorder(Cudd_ReorderingType heuristic){
/*
     if (filename == "bddMp.dot"){
         //Variablen im bddMp Gruppieren:
         //die Variablen f�r den zweiten Knoten sind im BDD immer unten => g�nstig f�r Restrict beim Matching
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
 
/*
//F�r Annotationen
DdNode* BddRepresentation::labelToBdd(vector<char*> v_channel){
    bool gefunden = false;
    int pos = 0;

    while (!gefunden && (pos < v_channel.size())){
        if (strcmp(v_channel[pos],channel)== 0){  //Channel gefunden
            gefunden = true;
            return (numberToBdd(mgr, pos, first, count));   //Channelnummer als BDD zur�ckgeben
        }
        ++pos;

    }

    if (!gefunden) {
        cout << "FEHLER: Channel " << channel << " NICHT gefunden" << endl;
        cout << "CONTROLLER PASST NICHT!\n\n";
        exit(1);
    }
}
*/



/**C++ version std::string style "itoa" by John Maloney
   (http://www.jb.man.ac.uk/~slowe/cpp/itoa.html)
**/
string BddRepresentation::myitoa(unsigned int value, int base) {
    enum { kMaxDigits = 35 };
    std::string buf;
    buf.reserve( kMaxDigits ); // Pre-allocate enough space.

    // check that the base if valid
    if (base < 2 || base > 16) return buf;
    int quotient = value;

    // Translating number to string with base:
    do {
        buf += "0123456789abcdef"[ std::abs( quotient % base ) ];
        quotient /= base;
    } while ( quotient );

    // Append the negative sign for base 10
    if ( value < 0 && base == 10) buf += '-';
    std::reverse( buf.begin(), buf.end() );

    return buf;
}

//! \fn void BddRepresentation::checkManager(DdManager* mgr, char* table)
void BddRepresentation::checkManager(DdManager* mgr, char* table){
    cout << "\ncheck " <<table << ":\n";
    if (Cudd_CheckZeroRef(mgr) != 0){
        cout << "WARNING: Cudd_CheckZeroRef("<< table << ") = "
                << Cudd_CheckZeroRef(mgr) << "\n";
    }
    cuddHeapProfile(mgr);
    Cudd_DebugCheck(mgr);
}

//! \fn void BddRepresentation::print()
//! \brief  print bdd_Mp and bdd_Ann
void BddRepresentation::print(){
//TODO f�r Ausgabe (zur leichteren Lesbarkeit) Variablenordnung ver�ndern: node1 label node2	
	Cudd_PrintMinterm(mgrMp, bddMp); 
	//Cudd_PrintMinterm(this->mgrAnn, this->bddAnn);
}

//! \fn void BddRepresentation::printDotFile(char** varNames)
//! \brief creates dot files of the BDDs
//    mit dot -Tps fileName -o neu.ps kann das BDD graphisch dargestellt werden
void BddRepresentation::printDotFile(char** varNames){
	if ((Cudd_DagSize(bddMp) < 200000)&&(Cudd_DagSize(bddAnn) < 200000) ) {
	
		char bufferMp[256]; 
		char bufferAnn[256];
		if (parameters[P_CALC_ALL_STATES]) {
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
            trace(TRACE_0, "creating the dot file of BDD_MP and BDD_ANN...\n");
            if (parameters[P_CALC_ALL_STATES]) {
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
            trace(TRACE_0, "BDDs are too big to have dot create the graphics\n");
        }
    } else {
        trace(TRACE_0, "BDDs are too big to create dot file\n");
    }
            
}	

