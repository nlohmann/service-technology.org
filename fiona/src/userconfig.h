// User configuration file for Fiona

// specify whether each new operation shall be logged
// #define LOG_NEW

// Specify whether for debugging purposes various assertions should be checked.
// Helps to prevent doing unintended memory operations such as writing outside
// of array bounds or losing memory by overwriting pointers that should not be
// overwritten.
#undef NDEBUG     // assertions are tested
//#define NDEBUG 1     // assertions are ignored

// Select your operating system
#define WINDOWS
//#define LINUX
//#define SUNOS

// CAPACITY is the largest expected number of tokens on any place.
// Set CAPACITY to 1 for safe nets. Do not define CAPACITY if
// you do not know upper limits for the number of tokens on places.

//#define CAPACITY 4

// REPORTFREQUENCY controls the output of progress messages to the
// terminal (to be precise: to the standard error stream).
// Progress messages are written during graph generation 
// after every REPORTFREQUENCY transition occurrences and during
// symmetry calculation after passing a search depth that is
// a multiple of REOPRTFREQUENCY. If  REPORTFREQUENCY is not
// defined, a standard value applies.

#define REPORTFREQUENCY 1000

// MAXPATH is only used for the FINDPATH algorithm. There, LoLA aborts
// a search attempt after having reached a depth of MAXPATH and
// starts another attempt. When MAXPATH is not defined, a standard
// value applies

#define MAXPATH 3000000

// HASHSIZE is the number of entries in the table that hashes the 
// reachable states.  Larger values should speed up search but
// consume more memory. This variable must be set to a power of 2

#define HASHSIZE 65536

// Defining COVER and/or SYMMETRY and/or STUBBORN enables the application of the
// corresponding reduction technique (if more than one value is defined,
// the corresponding techniques are applied jointly). 
// If a reduction technique does not fit to the analysis task, the reduction
// technique will be switched off automatically.

//#define COVER
#define STUBBORN
//#define SYMMETRY

// In connection with STATEPREDICATE (below), there exist two versions of
// stubborn set. RELAXED selects the Kristensen/Valmari method (Petri net
// conference 2000). Without RELAXED, the Schmidt method (Petri net conference
// 1999) is selected. Kristensen/Valmari behaves significantly better for 
// non-satisfiable predicates while Schmidt tends to approach target states more
// directly if there exist any. Without STUBBORN, or without STATEPREDICATE,
// setting or removing RELAXED does not influence anything.

//#define RELAXED

// When SYMMETRY is defined, the value of SYMMINTEGRATION specifies the
// technique for using the symmetries in graph generation
// 1 refers to the iteration of symmetries (leads to maximal reduction,
//   fast for small symmetry groups, slow for large symmetry groups)
// 2 refers to the iteration of states (leads to maximal reduction.
//	 fast for large symmetry groups, slow for small symmetry griups)
// 3 refers to the canonical representitive technique (does not lead
//   to maximal reduction, fast for all kind of symmetry groups)
// 4 refers to the canonical representitive technique on the fly (i.e.
//   without pre-computed symmetries) (does not lead to maximal reduction)
// 5 refers to the canonical representitive technique on the fly (i.e.
//   without pre-computed symmetries) and maxiaml reduction (slower than 4)

//#define SYMMINTEGRATION 3
#define MAXATTEMPT 10

// GRAPH specifies the strategy of graph exploration. It must have
// either value depth_first or value breadth_first.


#define DEPTH_FIRST
//#define BREADTH_FIRST

// The following list contains all available analysis tasks. Please,
// define exactly one of the following values.
// REACHABILITY searches for a given state
// REVERSIBILITY verifies reversibility of the net
// HOME checks for home states
// BOUNDEDNET decides boundedness of the net
// BOUNDEDPLACE decides boundedness of a given place
// DEADTRANSITION verifies whether a given transition is dead
// MODELCHECKING verifies a given temporal logic formula
// FINDPATH performs a memoryless search for a state that satisfies a
// given state predicate
// FULL just computes a graph without verifying any property
// DEADLOCK verifies whether a dead state is reachable
// STATEPREDICATE searches for a state satisfying a given predicate
// NONE does not verify anything. Use this if you only want to get the
// generating set of the symmetries or the LL net.

//#define REACHABILITY
#define MODELCHECKING
//#define BOUNDEDPLACE
//#define BOUNDEDNET
//#define DEADTRANSITION
//#define REVERSIBILITY
//#define HOME
//#define FINDPATH
//#define FULL
//#define DEADLOCK
//#define NONE
//#define STATEPREDICATE

// EXTENDEDCTL enables the use of transition formulae for restricting the scope of
// path quantifiers in CTL formulae. If disabled, transition formulae may appear in
// formulae but are ignored. Only relevant in combination with MODELCHECKING

//#define EXTENDEDCTL
