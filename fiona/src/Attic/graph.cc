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


// decodes state and returns the corresponding marking
void State::decode(unsigned int * v, oWFN * PN) {
	
	trace(TRACE_5, "void State::decode(int * v, oWFN * PN):start\n");
	
	long int cutplace;
	int currentbyte;
	binDecision * currentbindec;
	unsigned char * currentvector;
	int byteincurrentvector;
	int currentplacenr;
	int vectorsfirstbit;
	int pfirst;
	int plast;
	int cfirst;
	int clast;

	currentplacenr = PN -> getPlaceCnt() - 1;
	currentbindec = my_entry;
	currentvector = my_entry -> vector;
	vectorsfirstbit = my_entry -> bitnr + 1;   // bit of decision not represented in vector
	currentbyte = ((PN->BitVectorSize-1) - vectorsfirstbit)/8 ;
	pfirst = PN->Places[currentplacenr]->startbit;
	plast = pfirst + PN->Places[currentplacenr]->nrbits - 1;
	cutplace = currentvector[currentbyte];
	cfirst = vectorsfirstbit + 8*currentbyte;
	clast = PN->BitVectorSize - 1;
	cutplace = cutplace >> (7-(clast -cfirst));	
	cutplace = cutplace << (7-(clast -cfirst));	


	while(1)
	{
		//vorn abschneiden
		if(cfirst < pfirst)
		{
			cutplace &= (1 << (cfirst + 8 - pfirst)) - 1;
		}
		cutplace = cutplace >> (7 - (clast - cfirst));
		// hinten abschneiden
		if(clast > plast)
		{
			cutplace = cutplace >> (clast - plast);	
		}


		// eintragen
		if(plast > clast)
		{
			v[currentplacenr] += cutplace << (plast - clast); 
		}
		else
		{
			v[currentplacenr] = cutplace;
		}
		
		
		if(cfirst == pfirst) {
			if(currentplacenr == 0) {
				return;
			}
			// new place, new byte
			
	
			currentplacenr--;

			pfirst = PN->Places[currentplacenr]->startbit;
			plast = pfirst + PN->Places[currentplacenr]->nrbits - 1;

			if(currentbyte == 0)
			{
				// new vector
				currentbindec = currentbindec -> prev;
				currentvector = currentbindec -> vector;
				vectorsfirstbit = currentbindec -> bitnr + 1;
				currentbyte = (cfirst - 1 - vectorsfirstbit) / 8;
				clast = cfirst - 1;
				cfirst = vectorsfirstbit + 8 * currentbyte;
				cutplace =	currentvector[currentbyte];
				cutplace = cutplace ^ (1 << ( 7 - (clast -cfirst)));
				cutplace = cutplace >> (7-(clast -cfirst));	
				cutplace = cutplace << (7-(clast -cfirst));	
			}
			else
			{
				currentbyte--;
				cfirst -= 8;
				clast = cfirst + 7;
				cutplace = currentvector[currentbyte];
			}
			continue;
		}
		
		if(cfirst < pfirst) {
			

			// new place
			currentplacenr--;
			pfirst = PN->Places[currentplacenr]->startbit;
			plast = pfirst + PN->Places[currentplacenr]->nrbits - 1;
			cutplace = currentvector[currentbyte];
			continue;
		}
		
		//(cfirst > pfirst --> new byte)
		if(currentbyte == 0)
		{
			// new vector
			currentbindec = currentbindec -> prev;
			currentvector = currentbindec -> vector;
			vectorsfirstbit = currentbindec -> bitnr + 1;
			currentbyte = (cfirst - 1 - vectorsfirstbit) / 8;
			clast = cfirst - 1;
			cfirst = vectorsfirstbit + 8 * currentbyte;
			cutplace =	currentvector[currentbyte];
			cutplace = cutplace ^ (1 << ( 7 - (clast -cfirst)));
			cutplace = cutplace >> (7-(clast -cfirst));	
			cutplace = cutplace << (7-(clast -cfirst));	
		}
		else
		{
			currentbyte--;
			cfirst -= 8;
			clast = cfirst + 7;
			cutplace = currentvector[currentbyte];
		}
	}

	trace(TRACE_5, "void State::decode(int * v, oWFN * PN):end\n");
}

