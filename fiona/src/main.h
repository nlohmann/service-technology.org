#ifndef MAIN_H
#define MAIN_H


//#define DEBUG			// shows all the information available ;-)
//#define DEBUG1		// shows the nodes we are working on 
//#define DEBUG2		// just for the calculation of the reachable states for now

//#define LOOP

//#define ONLY_BLUE_NODES // only blue nodes in the dot file
//#define NO_RED_NODES    // no red nodes in the dot file

//#define NO_EMPTY_NODE   // don't show the empty node in the dot file

//#define NO_STATES_PER_NODE

#include "mynew.h"

extern char * lownetfile;
extern char * pnmlfile;
extern char * analysefile;
extern char * graphfile;
extern char * pathfile;
extern char * statefile;
extern char * symmfile;
extern char * netbasename;

extern bool hflg, Nflg, nflg, Aflg, Sflg, Yflg, Pflg,GMflg, aflg,
sflg, yflg,pflg,gmflg, cflg;

extern char graphformat;

extern int garbagefound;
extern char * reserve;

extern oWFN * PN;

extern unsigned int numberOfDecodes;

extern unsigned int NonEmptyHash;

typedef enum {TRANS, DEADLOCK, FINALSTATE, NN} stateType;   //!< TRANS == Transient; NN == NO IDEA ;-) --> should not be the case ever


// extern int yyparse();
extern void readnet();


extern unsigned int numberDeletedVertices;

#endif /*MAIN_H*/
