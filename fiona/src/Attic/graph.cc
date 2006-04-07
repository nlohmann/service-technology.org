#include"graph.h"
#include"dimensions.h"
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


