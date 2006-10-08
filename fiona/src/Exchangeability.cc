#include "mynew.h"
#include "options.h" 
#include "debug.h"
#include <iostream>
#include <string.h>
#include "Exchangeability.h"

DdManager* Exchangeability::mgrMp = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
DdManager* Exchangeability::mgrAnn = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
 
Exchangeability::Exchangeability(char* filename){
	trace(TRACE_5, "Exchangeability::Exchangeability(char* filename): begin\n");		   
    
    names = NULL;
    nbrVarAnn = 0;
	loadBdd(filename);
	 
	for (int i = 0; i < nbrVarAnn; ++i){
        if (names[i][0] == '!' || names[i][0] == '?'){
           (labelList).push_back(names[i]);
        }
    }
    
/* list<char*>::iterator list_iter;
	cout << "Liste ausgeben ";
   	for( list_iter = labelList.begin(); list_iter != labelList.end(); ++list_iter) {
     	cout << *list_iter;
   	}
	cout << endl;           
*/
	trace(TRACE_5, "Exchangeability::Exchangeability(char* filename): end\n");
}

void Exchangeability::loadBdd(char* filename){
    trace(TRACE_5, "Exchangeability::loadBdds(char* filename): begin\n");		   		   
    cout << "load BDD-representation of the operating guideline of " << filename << endl;
    
	//open cudd-files
	char bufferMp[256]; 
	char bufferAnn[256];

	sprintf(bufferMp, "%s.a.OG.BDD_MP.cudd", filename);
    sprintf(bufferAnn, "%s.a.OG.BDD_ANN.cudd", filename); 
    
    FILE* fpMp;
    fpMp = fopen(bufferMp, "r");
    
    FILE* fpAnn;
    fpAnn = fopen(bufferAnn, "r");
    
    if (fpMp == NULL || fpAnn == NULL){
    	sprintf(bufferMp, "%s.OG.BDD_MP.cudd", filename);
    	sprintf(bufferAnn, "%s.OG.BDD_ANN.cudd", filename); 
    	
    	fpMp = fopen(bufferMp, "r");
    	fpAnn = fopen(bufferAnn, "r");
    	
    	if (fpMp == NULL || fpAnn == NULL){
    		cerr << "cannot open cudd-files of " << filename << "\n";
            exit(4);
    	}
	}
	
	//load header of bddAnn
    int* permids = NULL;    //optimal variable ordering      
    loadHeader(fpAnn, &names, &nbrVarAnn, &permids);
    
    fseek(fpAnn, 0, SEEK_SET); //File-Pointer auf Anfang der Datei setzen, damit später BDD geladen werden kann;      
    
    //load bddAnn
    bddAnn = loadDiagram(fpAnn, mgrAnn, nbrVarAnn, permids);
    fclose(fpAnn);
    
    int nbrVarMp = 0;     //number of variables in bddMp
    
    delete [] permids;
    permids = NULL;
    
    //load header of bddMp
    loadHeader(fpMp, NULL, &nbrVarMp, &permids);
    fseek(fpMp, 0, SEEK_SET); //File-Pointer auf Anfang der Datei setzen, damit Bdd geladen werden kann;
    
    //load bddMp 
    bddMp = loadDiagram(fpMp, mgrMp, nbrVarMp, permids);
    fclose(fpMp);
    
    trace(TRACE_5, "Exchangeability::loadBdds(char* filename): end\n");		   
}


void Exchangeability::loadHeader(FILE* fp, char*** names, int* nVars, int** permids){
	trace(TRACE_5,"Exchangeability::loadHeader(FILE* fp, char*** names, int* nVars, int** permids): begin\n");
    Dddmp_DecompType ddType; //possible Values: DDDMP_BDD,DDDMP_ADD,DDDMP_CNF,DDDMP_NONE
    int nsuppvars;
    char** orderedVarNames;
    char** suppVarNames;
    int* ids;
    int* auxids; //Hilfsvariablen?
    int nRoots;

    Dddmp_cuddHeaderLoad (
    &ddType         /* OUT: selects the proper decomp type */,
    nVars           /* OUT: number of DD variables */,
    &nsuppvars      /* OUT: number of support variables */,
    &suppVarNames   /* OUT: array of support variable names */,
    &orderedVarNames/* OUT: array of variable names */,
    &ids            /* OUT: array of variable ids */,
    permids         /* OUT: array of permids ids */,
    &auxids         /* OUT: array of variable aux ids */,
    &nRoots         /* OUT: number of root in the file */,
    NULL            /* IN: file name */,
    fp              /* IN: file pointer */
    );
    
    if (ddType != DDDMP_BDD) {cout << "\nFehler beim Laden des BDDs: DD-Typ falsch"; exit(1);}
    if (nRoots != 1) {cout << "\nFehler beim Laden des BDDs: in Datei muss genau ein BDD enthalten sein"; exit(1);}
    if (names != NULL){
        if (suppVarNames == NULL) {cout << "FEHLER suppVarNames == NULL"; exit(1);}
        *names = suppVarNames;
    }
    if (nVars == 0) {cout << "FEHLER nVars == 0"; exit(1);}
    trace(TRACE_5,"Exchangeability::loadHeader(FILE* fp, char*** names, int* nVars, int** permids): end\n");
}


DdNode* Exchangeability::loadDiagram(FILE* fp, DdManager* mgr, int size, int* permids){
	trace(TRACE_5,"Exchangeability::loadDiagram(FILE* fp, DdManager* mgr, int size, int* permids): begin\n");
    DdNode* bdd = Dddmp_cuddBddLoad (
            mgr                 /* IN: DD Manager */,
            DDDMP_VAR_MATCHIDS  /* IN: storing mode selector */,
            NULL                /* IN: array of variable names - by IDs */,
            NULL                /* IN: array of variable auxids - by IDs */,
            NULL                /* IN: array of new ids accessed - by IDs */,
            DDDMP_MODE_TEXT     /* IN: requested input file format */,
            NULL                /* IN: file name */,
            fp                  /* IN: file pointer */
    );
    
    // optimale Reihenfolge der Variablen im neuen Manager wieder bilden
    int idOrder[size];
    for (int i = 0; i < size; ++i){
        idOrder[permids[i]] = i;
    }
    Cudd_ShuffleHeap(mgr,idOrder);
    trace(TRACE_5,"Exchangeability::loadDiagram(FILE* fp, DdManager* mgr, int size, int* permids): end\n");
    return (bdd);
}


void Exchangeability::printDotFile(char* filename, char** varNames, DdNode* bddMp, DdNode* bddAnn){
	trace(TRACE_5,"Exchangeability::printDotFile(char* filename, char** varNames, DdNode* bddMp, DdNode* bddAnn): begin\n");
	if ((Cudd_DagSize(bddMp) < 200000) && (Cudd_DagSize(bddAnn) < 200000) ) {
	
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
            sprintf(bufferMp, "dot -Tpng %sexchangeability.BDD_MP.out -o %sexchangeability.BDD_MP.png", filename, filename);
            sprintf(bufferAnn, "dot -Tpng %sexchangeability.BDD_ANN.out -o %sexchangeability.BDD_ANN.png", filename, filename);

            system(bufferMp);
            system(bufferAnn);
         
        } 
    }
	trace(TRACE_5,"Exchangeability::printDotFile(char* filename, char** varNames, DdNode* bddMp, DdNode* bddAnn): end\n");     
}      


bool Exchangeability::check(Exchangeability* bdd){
	trace(TRACE_5,"Exchangeability::check(Exchangeability* bdd): begin\n");
	if (this->labelList.size() != bdd->labelList.size()){
		return (false);
	}
	
	list<char*>::const_iterator thislist_iter = this->labelList.begin();
	list<char*>::const_iterator bddlist_iter = bdd->labelList.begin();
	while (thislist_iter != this->labelList.end()){ 	//this->labelList.size() is equal to bdd->labelList.size()
		//cout << *thislist_iter << *bddlist_iter << endl;
		if(strcmp(*thislist_iter, *bddlist_iter)){
			return(false);
		}
		++thislist_iter;
		++bddlist_iter;
	}
	
	if (Cudd_bddLeq(mgrMp, bddMp, bdd->bddMp) == 1  && Cudd_bddLeq(mgrAnn, bddAnn, bdd->bddAnn) == 1 ){
		return(true);
	}else{
		return(false);
	}
	trace(TRACE_5,"Exchangeability::check(Exchangeability* bdd): end\n");
}
