#include "mynew.h"
#include"graph.h"
#include"dimensions.h"
#include"debug.h"
#include<fstream>
#include<string.h>
#include<stdio.h>

using namespace std;

unsigned int currentdfsnum = 1;
unsigned int pos;
//unsigned int State::card = 0;

	int bin_p; 				// (=place); index in MARKINGVECTOR
	int bin_pb;				// next bit of place to be processed;
	unsigned char bin_byte; // byte to be matched against tree vector; constructed from MARKINGVECTOR
	int bin_t;				// index in tree vector
	unsigned char * bin_v;	// current tree vector
	int bin_s;				// nr of bits pending in byte from previous iteration
	int bin_d;				// difference position
	int bin_b;				// bit nr at start of byte
	int bin_dir;			// bit nr at start of byte
	binDecision * fromdec, * todec, * vectordec;

char rapportstring[] = "search";

  unsigned int NrOfStates;
  unsigned int Edges;
  
void statistics(unsigned int s, unsigned int e, unsigned int h) {
	cout << "\n\n>>>>> " << s << " States, " << e << " Edges, " << h << " Hash table entries\n\n";
}

State * binSearch();
unsigned int MinBookmark; // MIN number of the first closed marking
                          // in the currently or last recently processed TSCC


// decodes State in bintree and returns the corresponding marking
void State::decode(oWFN * PN) {
	
	trace(TRACE_5, "void State::decode(int * v, oWFN * PN):start\n");
		
//	for (int i = 0; i < PN->getPlaceCnt(); i++) {
//		cout << PN->Places[i]->name << " (" << PN->Places[i]->nrbits << ")" << endl;
//	}

    binDecision * currentbindec;                // the considered part of bintree
    currentbindec = my_entry;

    unsigned char * currentvector;              // the corresponding bitvector
    currentvector = my_entry -> vector;

    int vectorsfirstbit;                        // offset where first bit of currentvector starts
    vectorsfirstbit = my_entry -> bitnr + 1;    // bit of decision not represented in vector

    int currentplacenr;                         // iterates through all places of the oWFN
    currentplacenr = PN -> getPlaceCnt() - 1;   // starting with the last one

    int pfirst;                                 // first bit of current place in bintree
    pfirst = PN->Places[currentplacenr]->startbit;

    int plast;                                  // last  bit of place in bintree
    plast = pfirst + PN->Places[currentplacenr]->nrbits - 1;

    int currentbyte;                            // current byte in currentvector of bintree
    currentbyte = ((PN->BitVectorSize-1) - vectorsfirstbit)/8 ;
        
    
    int cfirst;                                 // first bit of current byte in currentvector of bintree
    cfirst = vectorsfirstbit + 8*currentbyte;

    int clast;                                  // last  bit of current byte in currentvector of bintree
    clast = PN->BitVectorSize - 1;

    long int cutplace;                          // the value decoded for the current place
    cutplace = currentvector[currentbyte];

    // wenn currentbyte nicht ganz ausgefüllt (clast - cfirst != 7), dann rechts nullen
    cutplace = cutplace >> (7-(clast -cfirst));
    cutplace = cutplace << (7-(clast -cfirst));

	while(1) {		
        if(cfirst < pfirst) {
            // vorn abschneiden = verunden mit 00011111 wenn differenz 3
            cutplace &= (1 << (cfirst + 8 - pfirst)) - 1;
        }

		// wenn currentbyte nicht ganz ausgefüllt (clast - cfirst != 7), dann rechts ausrichten
		cutplace = cutplace >> (7 - (clast - cfirst));

        if(clast > plast) {
            // noch hinten abschneiden
            cutplace = cutplace >> (clast - plast);
        }


		// eintragen
		if(plast > clast) {
            // always preceded by the else-branch at prior currentbyte
			PN->CurrentMarking[currentplacenr] += cutplace << (plast - clast); 		
		} else {
			PN->CurrentMarking[currentplacenr] = cutplace;
		}


		// erster Fall: neuer Platz und neues Byte
		if(cfirst == pfirst) {
			if(currentplacenr == 0) {
				// all places decoded - finishing
				PN->placeHashValue = placeHashValue;
				PN->initializeTransitions();

				trace(TRACE_5, "void State::decode(int * v, oWFN * PN):end\n");
				
				return;
			}		
			
			// take new place
			currentplacenr--;

			pfirst = PN->Places[currentplacenr]->startbit;
			plast = pfirst + PN->Places[currentplacenr]->nrbits - 1;

			if(currentbyte == 0) {
				// go to predecessing vector in bintree
				currentbindec = currentbindec -> prev;
				currentvector = currentbindec -> vector;
				vectorsfirstbit = currentbindec -> bitnr + 1;
				currentbyte = (cfirst - 1 - vectorsfirstbit) / 8;
				clast = cfirst - 1;
				cfirst = vectorsfirstbit + 8 * currentbyte;
				
//				cout << "\t\tnew place " << PN->Places[currentplacenr]->name << " in new byte " << currentbyte << endl;
//				cout << "\t\t\t(bindecision changed)" << endl;
				
				cutplace =	currentvector[currentbyte];
				// bit of decision not represented in vector -> so XOR with 1
				cutplace = cutplace ^ (1 << ( 7 - (clast -cfirst)));
				// wenn currentbyte nicht ganz ausgefüllt (clast - cfirst != 7), dann rechts nullen
				cutplace = cutplace >> (7-(clast -cfirst));
				cutplace = cutplace << (7-(clast -cfirst));
			} else {
				// go to predecessing byte in current vector
				currentbyte--;
				cfirst -= 8;
				clast = cfirst + 7;

//				cout << "\t\tnew place " << PN->Places[currentplacenr]->name << " in new byte " << currentbyte << endl;
//				cout << "\t\t\t(in same bindecision)" << endl;

				cutplace = currentvector[currentbyte];
			}
			
			continue;
		}
		
		// zweiter Fall: neuer Platz, gleiches Byte
		if(cfirst < pfirst) {
			currentplacenr--;
			
			pfirst = PN->Places[currentplacenr]->startbit;
			plast = pfirst + PN->Places[currentplacenr]->nrbits - 1;

//			cout << "\t\tnew place " << PN->Places[currentplacenr]->name << " in same byte " << currentbyte << endl;
			
			cutplace = currentvector[currentbyte];
			
			continue;
		}
		
		// hier implizit pfirst < cfirst, also
		// dritter Fall: gleicher Platz in neuem Byte
		if(currentbyte == 0) {
			// new vector
			currentbindec = currentbindec -> prev;
			currentvector = currentbindec -> vector;
			vectorsfirstbit = currentbindec -> bitnr + 1;
			currentbyte = (cfirst - 1 - vectorsfirstbit) / 8;
			clast = cfirst - 1;
			cfirst = vectorsfirstbit + 8 * currentbyte;

//			cout << "\t\tsame place " << PN->Places[currentplacenr]->name << " in new byte " << currentbyte << endl;
//			cout << "\t\t\t(bindecision changed)" << endl;

			cutplace =	currentvector[currentbyte];
			// bit of decision not represented in vector -> so XOR with 1
			cutplace = cutplace ^ (1 << ( 7 - (clast -cfirst)));
			// wenn currentbyte nicht ganz ausgefüllt (clast - cfirst != 7), dann rechts nullen
			cutplace = cutplace >> (7-(clast -cfirst));	
			cutplace = cutplace << (7-(clast -cfirst));	
		} else {
			currentbyte--;
			cfirst -= 8;
			clast = cfirst + 7;

//			cout << "\t\tsame place " << PN->Places[currentplacenr]->name << " in new byte " << currentbyte << endl;
//			cout << "\t\t\t(in same bindecision)" << endl;

			cutplace = currentvector[currentbyte];
		}
	}

	cerr << "this line is not reachable" << endl;
	cerr << "if this happens please contact the authors" << endl;	
	cerr << "\tvoid State::decode(oWFN * PN) in graph.cc" << endl;
	
}

// decodes State in bintree and returns the corresponding marking
void State::decodeShowOnly(oWFN * PN) {
	
	trace(TRACE_5, "void State::decodeShowOnly(int * v, oWFN * PN):start\n");
		
//	for (int i = 0; i < PN->getPlaceCnt(); i++) {
//		cout << PN->Places[i]->name << " (" << PN->Places[i]->nrbits << ")" << endl;
//	}

    binDecision * currentbindec;                // the considered part of bintree
    currentbindec = my_entry;

    unsigned char * currentvector;              // the corresponding bitvector
    currentvector = my_entry -> vector;

    int vectorsfirstbit;                        // offset where first bit of currentvector starts
    vectorsfirstbit = my_entry -> bitnr + 1;    // bit of decision not represented in vector

    int currentplacenr;                         // iterates through all places of the oWFN
    currentplacenr = PN -> getPlaceCnt() - 1;   // starting with the last one

    int pfirst;                                 // first bit of current place in bintree
    pfirst = PN->Places[currentplacenr]->startbit;

    int plast;                                  // last  bit of place in bintree
    plast = pfirst + PN->Places[currentplacenr]->nrbits - 1;

    int currentbyte;                            // current byte in currentvector of bintree
    currentbyte = ((PN->BitVectorSize-1) - vectorsfirstbit)/8 ;
        
    
    int cfirst;                                 // first bit of current byte in currentvector of bintree
    cfirst = vectorsfirstbit + 8*currentbyte;

    int clast;                                  // last  bit of current byte in currentvector of bintree
    clast = PN->BitVectorSize - 1;

    long int cutplace;                          // the value decoded for the current place
    cutplace = currentvector[currentbyte];

    // wenn currentbyte nicht ganz ausgefüllt (clast - cfirst != 7), dann rechts nullen
    cutplace = cutplace >> (7-(clast -cfirst));
    cutplace = cutplace << (7-(clast -cfirst));

	while(1) {		
        if(cfirst < pfirst) {
            // vorn abschneiden = verunden mit 00011111 wenn differenz 3
            cutplace &= (1 << (cfirst + 8 - pfirst)) - 1;
        }

		// wenn currentbyte nicht ganz ausgefüllt (clast - cfirst != 7), dann rechts ausrichten
		cutplace = cutplace >> (7 - (clast - cfirst));

        if(clast > plast) {
            // noch hinten abschneiden
            cutplace = cutplace >> (clast - plast);
        }


		// eintragen
		if(plast > clast) {
            // always preceded by the else-branch at prior currentbyte
			PN->CurrentMarking[currentplacenr] += cutplace << (plast - clast); 		
		} else {
			PN->CurrentMarking[currentplacenr] = cutplace;
		}


		// erster Fall: neuer Platz und neues Byte
		if(cfirst == pfirst) {
			if(currentplacenr == 0) {
				// all places decoded - finishing
				trace(TRACE_5, "void State::decode(int * v, oWFN * PN):end\n");
				
				return;
			}		
			
			// take new place
			currentplacenr--;

			pfirst = PN->Places[currentplacenr]->startbit;
			plast = pfirst + PN->Places[currentplacenr]->nrbits - 1;

			if(currentbyte == 0) {
				// go to predecessing vector in bintree
				currentbindec = currentbindec -> prev;
				currentvector = currentbindec -> vector;
				vectorsfirstbit = currentbindec -> bitnr + 1;
				currentbyte = (cfirst - 1 - vectorsfirstbit) / 8;
				clast = cfirst - 1;
				cfirst = vectorsfirstbit + 8 * currentbyte;
				
//				cout << "\t\tnew place " << PN->Places[currentplacenr]->name << " in new byte " << currentbyte << endl;
//				cout << "\t\t\t(bindecision changed)" << endl;
				
				cutplace =	currentvector[currentbyte];
				// bit of decision not represented in vector -> so XOR with 1
				cutplace = cutplace ^ (1 << ( 7 - (clast -cfirst)));
				// wenn currentbyte nicht ganz ausgefüllt (clast - cfirst != 7), dann rechts nullen
				cutplace = cutplace >> (7-(clast -cfirst));
				cutplace = cutplace << (7-(clast -cfirst));
			} else {
				// go to predecessing byte in current vector
				currentbyte--;
				cfirst -= 8;
				clast = cfirst + 7;

//				cout << "\t\tnew place " << PN->Places[currentplacenr]->name << " in new byte " << currentbyte << endl;
//				cout << "\t\t\t(in same bindecision)" << endl;

				cutplace = currentvector[currentbyte];
			}
			
			continue;
		}
		
		// zweiter Fall: neuer Platz, gleiches Byte
		if(cfirst < pfirst) {
			currentplacenr--;
			
			pfirst = PN->Places[currentplacenr]->startbit;
			plast = pfirst + PN->Places[currentplacenr]->nrbits - 1;

//			cout << "\t\tnew place " << PN->Places[currentplacenr]->name << " in same byte " << currentbyte << endl;
			
			cutplace = currentvector[currentbyte];
			
			continue;
		}
		
		// hier implizit pfirst < cfirst, also
		// dritter Fall: gleicher Platz in neuem Byte
		if(currentbyte == 0) {
			// new vector
			currentbindec = currentbindec -> prev;
			currentvector = currentbindec -> vector;
			vectorsfirstbit = currentbindec -> bitnr + 1;
			currentbyte = (cfirst - 1 - vectorsfirstbit) / 8;
			clast = cfirst - 1;
			cfirst = vectorsfirstbit + 8 * currentbyte;

//			cout << "\t\tsame place " << PN->Places[currentplacenr]->name << " in new byte " << currentbyte << endl;
//			cout << "\t\t\t(bindecision changed)" << endl;

			cutplace =	currentvector[currentbyte];
			// bit of decision not represented in vector -> so XOR with 1
			cutplace = cutplace ^ (1 << ( 7 - (clast -cfirst)));
			// wenn currentbyte nicht ganz ausgefüllt (clast - cfirst != 7), dann rechts nullen
			cutplace = cutplace >> (7-(clast -cfirst));	
			cutplace = cutplace << (7-(clast -cfirst));	
		} else {
			currentbyte--;
			cfirst -= 8;
			clast = cfirst + 7;

//			cout << "\t\tsame place " << PN->Places[currentplacenr]->name << " in new byte " << currentbyte << endl;
//			cout << "\t\t\t(in same bindecision)" << endl;

			cutplace = currentvector[currentbyte];
		}
	}

	cerr << "this line is not reachable" << endl;
	cerr << "if this happens please contact the authors" << endl;	
	cerr << "\tvoid State::decode(oWFN * PN) in graph.cc" << endl;
	
}

