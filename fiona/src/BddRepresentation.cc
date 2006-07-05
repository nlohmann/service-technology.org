#include "symboltab.h"

#include <iostream>
#include <string>
#include "math.h"

#include "BddRepresentation.h" 
#include "stateList.h"
#include "graphEdge.h"
#include "symboltab.h"
#include "owfn.h"

#include "options.h"
#include "debug.h"
 
//extern char* netfile;  
 
//! \fn BddRepresentation::BddRepresentation(vertex * root, int nbrLabels, Cudd_ReorderingType heuristic)
//! \brief constructor
BddRepresentation::BddRepresentation(vertex * root, int nbrLabels, Cudd_ReorderingType heuristic){
	maxLabelBits = nbrBits(nbrLabels-1);
	maxNodeBits = 1;
	
	int sizeMp = 2 * maxNodeBits + maxLabelBits;
    int sizeAnn = maxNodeBits + nbrLabels;
    
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
    
    InterfaceTable = new SymbolTab(PN->placeInputCnt + PN->placeOutputCnt);
    unsigned int i = 0;
    
    //add interface places to InterfaceTable
    owfnPlace * P;
    ISymbol * IPS; //Interface-Place-Symbol
    for (i = 0; i < PN->placeInputCnt; ++i){
    	P = new owfnPlace(PN->Places[PN->inputPlacesArray[i]]->name, PN->Places[PN->inputPlacesArray[i]]->getType(), PN);    	
    	P->index = PN->Places[PN->inputPlacesArray[i]]->index;
    	IPS = new ISymbol(P);
    }
    
    for (i = 0; i < PN->placeOutputCnt; ++i){
		P = new owfnPlace(PN->Places[PN->outputPlacesArray[i]]->name, PN->Places[PN->outputPlacesArray[i]]->getType(), PN);
    	P->index = PN->Places[PN->outputPlacesArray[i]]->index;
    	IPS = new ISymbol(P);
    }
        
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
}

//! \fn void BddRepresentation::generateRepresentation(vertex* v, bool visitedNodes[])
//! \brief generate BDD representation
void BddRepresentation::generateRepresentation(vertex* v, bool visitedNodes[]){
	v->resetIteratingSuccNodes();
	if (v->getColor() == BLUE) {	
		if (v->getStateList()->setOfReachGraphStates.size() != 0){
			visitedNodes[v->getNumber()] = 1;
			graphEdge* element;
			
			while((element = v->getNextEdge()) != NULL){
				
				vertex* vNext = element->getNode();
				
				if (vNext->getColor() == BLUE && 
				    vNext->getStateList()->setOfReachGraphStates.size() != 0 &&
				    vNext != NULL ) {
				    						
				    //label						
		            DdNode * label = labelToBddMp(element->getLabel()); 
//TODO              //if (label == NULL) exit(1);
		            		
		            //nodes		    
		            DdNode * nodes = nodesToBddMp(v->getNumber(), vNext->getNumber());
		            
		            //edge
		            DdNode * edge = Cudd_bddAnd(mgrMp, label, nodes); 
//TODO              //if (edge == NULL) exit(1);
		            Cudd_Ref(edge);
		            Cudd_RecursiveDeref(mgrMp, label);
		            Cudd_RecursiveDeref(mgrMp, nodes);
					/*
		            cout << "bddMp " << v->getNumber() << " [" << element->getLabel() << "> "
		                 << vNext->getNumber() << " :\t"; Cudd_PrintMinterm(this->mgrMp, edge);
		            cout << "--------------------------------\n";
					*/
		            DdNode* tmp = Cudd_bddOr(mgrMp, edge, bddMp);
//TODO              //if (tmp == NULL) exit(1);
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
}

//! \fn DdNode*  BddRepresentation::labelToBddMp(char* label)
//! \brief returns the BDD of a label (given as integer)
DdNode*  BddRepresentation::labelToBddMp(char* label){
	Symbol * s = InterfaceTable->lookup(label);
    unsigned int number = ((ISymbol*)s)->place->index;
	
	BitVector* assignment = numberToBin(number, maxLabelBits);
		
	DdNode*  f = Cudd_ReadOne(mgrMp);
    Cudd_Ref(f);
    DdNode* tmp;

	//assignment to BDD
	for (int i = 0; i < maxLabelBits; ++i){     //TODO: Schleife Absteigend durchlaufen (--i)
        if ((*assignment)[i] == false){
            tmp = Cudd_bddAnd(mgrMp, Cudd_Not(Cudd_bddIthVar(mgrMp,i)), f);
        }
        else{
            tmp = Cudd_bddAnd(mgrMp, Cudd_bddIthVar(mgrMp,i), f);
        }
        Cudd_Ref(tmp);
        Cudd_RecursiveDeref(mgrMp, f);
        f = tmp;
    }
    return (f);
}

//! \fn 
//! \param 
//! \brief
unsigned int BddRepresentation::getBddNumber(unsigned int node){
    map<unsigned int, unsigned int>::const_iterator map_iter;
    map_iter = nodeMap.find(node);    //search node in nodeMap
    
    unsigned int bddNumber;
    if (map_iter == nodeMap.end()){ //if node not found add node with a new bddNumber
        pair<map<unsigned int, unsigned int>::iterator, bool> success;
        bddNumber = nodeMap.size();
        success = nodeMap.insert(make_pair(node, bddNumber));
//TODO  //if (!(success.second)){exit(1);}
    }
    else {
        bddNumber = map_iter -> second;
    }
    
    return(bddNumber);
}


//! \fn DdNode* BddRepresentation::nodesToBddMp(unsigned int node1, unsigned int node2)
//! \brief returns the BDD of the nodes (given as integer) of an edge
DdNode* BddRepresentation::nodesToBddMp(unsigned int node1, unsigned int node2){
   
    unsigned int bddNumber1 = getBddNumber(node1);   
    unsigned int bddNumber2 = getBddNumber(node2);
    
	unsigned int maxBddNumber;	
	if (bddNumber1 > bddNumber2){
		maxBddNumber = bddNumber1;
	}else {maxBddNumber = bddNumber2;}
		
	int neededNodeBits = nbrBits(maxBddNumber); 
	/*cout << "maxBddNumber: " << maxBddNumber << endl <<
		"neededNodeBits: " << neededNodeBits << 
        "  maxNodeBits(old): " << maxNodeBits << endl;
    */
    if (neededNodeBits > maxNodeBits){
    	//add necessary BDD variables	    	
    	DdNode* tmp1 = Cudd_ReadOne(mgrMp);
    	Cudd_Ref(tmp1);
    	DdNode* tmp2;
    	DdNode* var;
    	
    	for(int i = 0; i < 2*(neededNodeBits - maxNodeBits); ++i){
    		var = Cudd_bddNewVar(mgrMp);
//TODO		//if (var == NULL ){ FEHLER };
    		tmp2 = Cudd_bddAnd(mgrMp, tmp1, Cudd_Not(var));
    		Cudd_Ref(tmp2);
    		Cudd_RecursiveDeref(mgrMp, tmp1);
    		tmp1 = tmp2;		    		
    	}
    	 
    	maxNodeBits = neededNodeBits;
		tmp2 = Cudd_bddAnd(mgrMp, tmp1, bddMp);
		Cudd_Ref(tmp2);
		Cudd_RecursiveDeref(mgrMp, tmp1); 
    	Cudd_RecursiveDeref(mgrMp, bddMp);
    	bddMp = tmp2;
    	//cout << "maxNodeBits(new): " << maxNodeBits << endl;
    	//Cudd_PrintMinterm(mgrMp, bddMp);
			    	
    }	
	
	BitVector* assignment1 = numberToBin(bddNumber1, maxNodeBits);
	BitVector* assignment2 = numberToBin(bddNumber2, maxNodeBits);

    //calculate the BDD for assignment1 and assignment2
    DdNode* f = Cudd_ReadOne(mgrMp);
    Cudd_Ref(f);
    DdNode* tmp;
    
    int j;
    for (int i = 0; i < maxNodeBits; ++i){     //TODO: schleife Absteigend durchlaufen (--i)
    	j = maxNodeBits-1-i;
        if ((*assignment1)[j] == false){
            tmp = Cudd_bddAnd(mgrMp, Cudd_Not(Cudd_bddIthVar(mgrMp, maxLabelBits+(2*i))), f);
        }
        else{
            tmp = Cudd_bddAnd(mgrMp, Cudd_bddIthVar(mgrMp, maxLabelBits+(2*i)), f);
        }
        Cudd_Ref(tmp);
        Cudd_RecursiveDeref(mgrMp, f);
        f = tmp; 
        
        if ((*assignment2)[j] == false){
            tmp = Cudd_bddAnd(mgrMp, Cudd_Not(Cudd_bddIthVar(mgrMp, maxLabelBits+1+(2*i))), f);
        }
        else{
            tmp = Cudd_bddAnd(mgrMp, Cudd_bddIthVar(mgrMp, maxLabelBits+1+(2*i)), f);
        }
        Cudd_Ref(tmp);
        Cudd_RecursiveDeref(mgrMp, f);
        f = tmp;
        
    }
    return (f);
}

//! \fn BitVector* BddRepresentation::numberToBin(unsigned int number, int count)
//! \brief returns the binary representation of a number **/
BitVector* BddRepresentation::numberToBin(unsigned int number, int count){
	BitVector* assignment = new BitVector(count);
    int base = 2;

    //calculate the binary representation
    int index = count - 1;
    do {
        (*assignment)[index--] = number % base;
        number /= base;
    } while ( number );

    //add leading zeros
    for (int i = index; i >= 0; --i){
        (*assignment)[i] = false;
    }
    
    return (assignment);
	
}

//! \fn int BddRepresentation::nbrBits(unsigned int i)
//! \brief returns the number of bits to represent a number i
int BddRepresentation::nbrBits(unsigned int i){
    switch (i){
        case(0): return(0);
        case(1): return(1);
        default:{
            const int max = (int)ceil(log((double)(i+1))/log((double)2));
            return (max);
        }
    }
}

//! \fn void BddRepresentation::reorder(Cudd_ReorderingType heuristic)
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
    cout << "\nBDD_MP: number of nodes: " << Cudd_DagSize(bddMp)<<endl<<endl;
    //cout << "\t" << Cudd_ReadReorderingTime(this->mgrMp) << " ms consumed for variable reordering" << endl;
    
    //Cudd_ReduceHeap(this->mgrAnn, heuristic, 0);
    //cout << "\nBDD_ANN: number of nodes: "  << Cudd_DagSize(this->bddAnn)<<endl;
    //cout << "\t" << Cudd_ReadReorderingTime(this->mgrAnn) << " ms consumed for variable reordering" << endl;
}
 
/*
//Für Annotationen
DdNode* BddRepresentation::labelToBdd(vector<char*> v_channel){
    bool gefunden = false;
    int pos = 0;

    while (!gefunden && (pos < v_channel.size())){
        if (strcmp(v_channel[pos],channel)== 0){  //Channel gefunden
            gefunden = true;
            return (numberToBdd(mgr, pos, first, count));   //Channelnummer als BDD zurückgeben
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
//TODO für Ausgabe (zur leichteren Lesbarkeit) Variablenordnung verändern: node1 label node2	
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

