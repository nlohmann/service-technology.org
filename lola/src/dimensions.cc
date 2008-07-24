#include<iostream>
#include"dimensions.H"

using std::cout;
using std::endl;

void reportconfiguration()
{
	cout << "LoLA - A low level Analyzer" << endl;
	cout << "---------------------------" << endl << endl;
	cout << "state space based verification of low level Petri nets." << endl;
	cout << "written by Karsten Schmidt" << endl;
	cout << "NO WARRANTY!" << endl;
	cout << "http://www.informatik.hu-berlin.de/~kschmidt/lola.html" << endl << endl;
	cout << "This executable is compiled in the following configuration:" << endl;
	cout << "* Verification problem: " << endl;
#ifdef REACHABILITY
	cout << "  REACHABILITY: check if a (completely specified) marking" << endl;
	cout << "                is reachable from the initial marking" << endl;
#endif
#ifdef BOUNDEDNET
	cout << "  BOUNDEDNET: check if the net is bounded" << endl;
#endif
#ifdef BOUNDEDPLACE
	cout << "  BOUNDEDPLACE: check if a particular place is bounded" << endl;
#endif
#ifdef DEADTRANSITION
	cout << "  DEADTRANSITION: check if a particular transition is dead" << endl;
#endif
#ifdef DEADLOCK
	cout << "  DEADLOCK: check if a dead marking is reachable" << endl;
#endif
#ifdef HOME
	cout << "  HOME: check if the net has home states" << endl;
#endif 
#ifdef REVERSIBILITY
	cout << "  REVERSIBILITY: check if the net is reversible" << endl;
#endif
#ifdef MODELCHECKING 
#ifdef EXTENDEDCTL
	cout << "  MODELCHECKING+EXTENDEDCTL: verify an ECTL-formula" << endl;
#else
	cout << "  MODELCHECKING: verify a CTL-formula" << endl;
#endif
#endif
#ifdef FINDPATH
	cout << "  FINDPATH: simulate the net repeatedly to find a marking" << endl;
	cout << "            that satisfies a given state predicate" << endl;
#endif
#ifdef FULL 
	cout << "  FULL: compute a graph without evaluating any property" << endl;
#endif
#ifdef NONE 
	cout << "  NONE: do not compute any graph" << endl;
#endif
#if defined(STATEPREDICATE) && !defined(LIVEPROP)
	cout << "  STATEPREDICATE: check if a state is reachable that" << endl;
	cout << "                  satisfies a given property." << endl;
#endif
#ifdef LIVEPROP
	cout << "  LIVEPROP: check if a given statepredicate is live" << endl;
#endif
	cout << endl << "* Graph exploration strategy: " << endl;
#ifdef SWEEP
	cout << "  SWEEP: search order controlled by the sweep line method" << endl;
#else
#ifdef DEPTH_FIRST
	cout << "  DEPTH_FIRST: depth first search order" << endl;
#else
	cout << "  BREADTH_FIRST: breadth first search order" << endl;
#endif
#endif
	cout << endl << "* Reduction techniques applied:" << endl;
#ifdef STUBBORN 
	cout << "  STUBBORN   : partial order reduction using stubborn sets" << endl;
#endif
#ifdef STATEPREDICATE
#ifdef RELAXED
	cout << "               according to Kristensen/Valmari, 2000" << endl;
#else
	cout << "               according to Schmidt,1999" << endl;
#endif
#endif
#ifdef SYMMETRY
	cout << "  SYMMETRY   : state space condensation based on net automorphisms" << endl;
#endif
#ifdef COVER
	cout << "  COVER      : infinite sequence abstraction using " << endl;
	cout << "               the coverability graph technique" << endl;
#endif
#ifdef CYCLE
	cout << "  CYCLE      : store only cycle closing states, based on  " << endl;
	cout << "               T-invariants analysis " << endl;
#endif
#ifdef PREDUCTION
	cout << "  PREDUCTION : compress states using P-invariants" << endl;
#endif
#ifdef SMALLSTATE
	cout << "  SMALLSTATE : use light weight data structure for states" << endl;
#endif
#ifdef BITHASH
	cout << "  BITHASH    : store only hash value of states" << endl;
#endif
#ifdef SYMMETRY
	cout << endl << "* Way of using symmetries in graph exploration:" << endl;
	switch(SYMMINTEGRATION)
	{
	case 1: cout << "  1 - (pre-compute generating set) iterate symmetries " << endl;
		cout << "      and look up images" << endl; break;
	case 2: cout << "  2 - (no generating set) iterate states and try to map" << endl;
		cout << "      them to current state" << endl; break;
	case 3: cout << "  3 - (pre-compute generating set) use generators to" << endl;
		cout << "      transform current marking into (lx.) small one" << endl; break;
	case 4: cout << "  4 - (no generating set) compute approximation of " << endl;
		cout << "      canonical representitive on the fly." << endl; break;
	case 5: cout << "  5 - (no generating set) compute canonical " << endl;
		cout << "      representitive on the fly." << endl; break;
	}
#endif
#ifdef LIVEPROP
	cout << endl << "* Method of checking for liveness: " << endl;
#ifdef TWOPHASE
	cout << "  TWOPHASE: use two phased algorithm with" << endl;
#ifdef RELAXED
	cout << "  relaxed stubborn sets in phase two" << endl;
#else
	cout << "  attractor set based stubborn sets in phase two" << endl;
#endif
#else
	cout << "  use one phased algorithm with home property preserving stubborn sets" << endl;
#endif
#endif
	cout << endl << "* Values of numerical parameters: " << endl;
	cout << "  HASHSIZE        : " << HASHSIZE << " entries in hash table." << endl;
	cout << "  REPORTFREQUENCY : status report every " << REPORTFREQUENCY << " transitions fired." << endl;
#ifdef CAPACITY
	cout << "  CAPACITY        : at most " << CAPACITY << " tokens on any place." << endl;
#endif
#ifdef FINDPATH
	cout << "  MAXPATH         : reset to initial state after " << MAXPATH << " transition occurrences." << endl;
#endif
#ifdef CYCLE
	cout << "  MAXUNSAVED      : store, in addition to cycle closing states," << endl;
	cout << "                    states that have more than " << MAXUNSAVED <<
" unsaved predecessors." << endl;
#ifdef NONBRANCHINGONLY
	cout << "  NONBRANCHINGONLY: store all states with more than one successor," << endl;
#endif
#endif
#if defined(SYMMETRY) && SYMMINTEGRATION == 4
	cout << "  MAXATTEMPT      : " << "perform at most " << MAXATTEMPT << " loops of approximizing the canonical representitive." << endl;
#endif
}


