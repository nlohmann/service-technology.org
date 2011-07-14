#pragma once

#include "Arc.h"
#include "Node.h"
#include "Globals.h"
#include "verbose.h"
#include "dimensions.h"

class Transition;
class formula;

// TODO: shift to check.h ?
#ifdef MODELCHECKING
extern unsigned int formulaindex; ///< in modelchecking,
///< the different subformulas must be treated spearately
#endif

extern unsigned int currentdfsnum;


/// A Petri net place
class Place: public Node {
    public:
        static unsigned int cnt; ///< overall number of places;
        Place(char*, unsigned int, unsigned int);  ///< constructor
        ~Place(); ///< destructor
        static unsigned int hash_value; ///< hash value of Globals::CurrentMarking
        // TODO: conditional compilation?
        unsigned int target_marking; ///< marking that is checked for reachability
        unsigned int initial_marking; ///< the initial marking (as defined in input file)
        unsigned int hash_factor; ///< weight of place in hash value calculation (= weighted sum of markings)
        void operator += (unsigned int);  ///<increment marking of place
        void operator -= (unsigned int);  ///< decrement marking of place
        bool operator >= (unsigned int);  ///< test enabledness with respect to place
        void set_marking(unsigned int);   ///< set initial marking of place;
        void set_cmarking(unsigned int);   ///< set current marking of place;
        void set_hash(unsigned int);      ///< define a factor for hash value calculation
        ///< hash(m) = sum(p in P) p.hash_factor*Globals::CurrentMarking[p]
        unsigned int index; ///< index in place array, necessary for symmetries
        unsigned int references; ///< #objects referring to this place - if 0 => remove place as isolated
        static unsigned int NrSignificant; ///< nr of significant places (insignificant =
        ///< linear dependent on signifiucant places
        bool significant; ///< set when PREDUCTION is switched on ; only marking of sig. places is stored
#ifdef STUBBORN
        unsigned int visible; ///< number of visible pre-transitions; used in stubborn set algorithms
        Transition** PreTransitions;  ///< List of pre-transitions (used as "mustbeincluded" in stubborn set algorithms)
        Transition** PostTransitions;  ///< List of post-transitions (used as "mustbeincluded" in stubborn set algorithms)
        void initialize(); ///< should be called upon parsing a net
#endif
#ifdef COVER
        bool bounded; ///< true if marking in current state is not omega (coverability feature)
        unsigned int lastfinite; ///< number of tokens in the moment of omega-intro (coverability feature)
#endif
#ifdef WITHFORMULA
        unsigned int cardprop; ///< number of propositions mentioning this place when a formula is involved
        formula** propositions;  ///< arry of all propositions mentioning this place. Used whebn formula values are updated
#endif
        unsigned int capacity;   ///< maximum capacity used for determining nr of bits in Decision tree
        unsigned int nrbits;     ///< nr of bits required for storing its marking (= log capacity)
};

/// constructor
inline Place::Place(char* name, unsigned int _capacity, unsigned int _nrbits) : Node(name), capacity(_capacity), nrbits(_nrbits) {
    cnt += 1;
#ifdef PREDUCTION
    significant = false; // in this case, psolve() sets sign. places subsequently
#else
    significant = true; // without PREDUCTION, all places are significant
#endif
    if (!(cnt % REPORTFREQUENCY)) {
        message("%d places parsed", cnt);
    }
    references = initial_marking = target_marking = hash_factor = 0;
#ifdef COVER
    bounded = true;
#endif
#ifdef STUBBORN
    visible = false; // real setting in sortscapegoats()
#endif
#ifdef WITHFORMULA
    cardprop = 0;
#endif
}

///desctructor
// TODO: free arrays that are present depending on STUBBORN, MODELCHECKING etc.?
inline Place::~Place() {
    cnt -= 1;
}

/// add i to initial marking of place, adjust hash value
inline void Place::operator += (unsigned int i) {
    initial_marking += i;
#ifndef SWEEP
    hash_value += i * hash_factor;
    hash_value %= HASHSIZE;
#endif
}

/// sub i from initial marking of place, adjust hash value
inline void Place::operator -= (unsigned int i) {
    initial_marking -= i;
#ifndef SWEEP
    hash_value -= i * hash_factor;
    hash_value %= HASHSIZE;
#endif
}

// TODO: Do we ever use this?
/// compare initial marking with i
inline bool Place::operator >= (unsigned int i) {
    return((initial_marking >= i) ? 1 : 0);
}

/// define a weight for a place that is used for computing a hash value
inline void Place::set_hash(unsigned int i) {
    hash_value -= hash_factor * initial_marking;
    hash_factor = i;
    hash_value += hash_factor * initial_marking;
    hash_value %= HASHSIZE;
}

/// set the initial marking  of a place to i; adjust hash values
inline void Place::set_marking(unsigned int i) {
#ifndef SWEEP
    hash_value -= hash_factor * initial_marking;
#endif
    initial_marking = i;
#ifndef SWEEP
    hash_value += hash_factor * initial_marking;
    hash_value %= HASHSIZE;
#endif
}

/// set Globals::CurrentMarking of a place, adjust hash value
inline void Place::set_cmarking(unsigned int i) {
#ifndef SWEEP
    hash_value -= hash_factor * Globals::CurrentMarking[index];
#endif
    Globals::CurrentMarking[index] = i;
#ifndef SWEEP
    hash_value += hash_factor * Globals::CurrentMarking[index];
    hash_value %= HASHSIZE;
#endif
}


#ifdef STUBBORN

/// create those arrays for the place which are required for stubborn set calculations
inline void Place::initialize() {
    int i;

    // Create list of pre-transitions. If this place is the scapegoat for a disabled transition in
    // stubborn set then these transitions must be in the stubborn set, too.
    PreTransitions = new Transition * [NrOfArriving + 1];
    PostTransitions = new Transition * [NrOfLeaving + 1];
    for (i = 0; i < NrOfArriving; i++) {
        PreTransitions[i] = ArrivingArcs[i]-> tr;
    }
    PreTransitions[NrOfArriving] = NULL;
    for (i = 0; i < NrOfLeaving; i++) {
        PostTransitions[i] = LeavingArcs[i]-> tr;
    }
    PostTransitions[NrOfLeaving] = NULL;
}
#endif
