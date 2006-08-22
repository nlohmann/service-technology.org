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

extern char * statefile;
extern bool Sflg, sflg;
extern oWFN * PN;
extern unsigned int numberOfDecodes;
extern unsigned int NonEmptyHash;
extern unsigned int numberDeletedVertices;

// extern int yyparse();
extern void readnet();

typedef enum {TRANS, DEADLOCK, FINALSTATE, NN} stateType;   //!< TRANS == Transient; NN == NO IDEA ;-) --> should not be the case ever

#endif /*MAIN_H*/
