#pragma once

#include <iostream>
#include "Globals.h"
#include "Arc.h"
#include "Node.h"
#include "Place.h"
using std::cerr;

//extern Place** Globals::Places;
extern unsigned int NrOfStates;
extern unsigned int Edges;
extern unsigned int NonEmptyHash;
extern void statistics(unsigned int s, unsigned int e, unsigned int h);

/// A Petri net transition
class Transition: public Node {
    public:
        static unsigned int cnt; ///< statistics; overall number of transitions
        static Transition* StartOfEnabledList;  ///< anchor to list of enabled transitions
        Transition(char*, unsigned int);  ///< constructor
        ~Transition(); ///< destructor
        unsigned int fairness; ///< fairness constraint defined for this transition: 0=no, 1=weak, 2=strong
        static unsigned int NrEnabled; ///< Number of enabled tr. at Globals::CurrentMarking
        bool enabled; ///< is this transition enabled in Globals::CurrentMarking?
        Transition* NextEnabled;  ///< enabled transitions are kept in double linked list --> insertion, deletion in O(1)
        Transition* PrevEnabled;  ///< enabled transitions are kept in double linked list --> insertion, deletion in O(1)
        // in the sequel, lists are represented as 0-terminated arrays
        unsigned int* PrePlaces;  ///< Globals::Places to be checked for enabledness after this fired
        unsigned int* Pre;  ///< Multiplicity to be checked for enabledness after this fired
        unsigned int* IncrPlaces;  ///< Globals::Places that are incremented by this transition firing
        unsigned int* Incr;  ///< Amount of increment when this transition fires
        unsigned int* DecrPlaces;  ///< Globals::Places that are decremented by this transition firing
        unsigned int* Decr;  ///< amount of decrement when this transition fires
        Transition** ImproveEnabling;  ///< list of transitions where enabledness
        ///< must be checked again after firing this transition
        Transition** ImproveDisabling;  ///< list of transitions where disabledness
        ///< must be checked again after firing this transition
        void initialize(); ///< Set arrays, list, enabled etc. for this transition
        void fire(); ///< replace current marking by successor marking, force
        ///< enabling test where necessary
        void backfire(); ///< fire transition backwards to replace original state,
        ///< force enabling tests where necessary, used in backtracking
        inline void check_enabled(); ///< test if tr is enabled. If necessary, rearrange Enabled list
        int hash_change; ///< Delta of hash value: must be added to current hash value when t fires;
        void set_hashchange(); ///< set the hash value Delta for this transition
#ifdef STUBBORN
        static Transition* StartOfStubbornList;  ///< Anchor to start of stubborn set
        static Transition* EndOfStubbornList;  ///< Anchor to end of stubborn set
        static unsigned int NrStubborn; ///< Nr. of enabled (!) transitions in stubborn set
        Transition* NextStubborn;  ///< stubborn set is organized as a single linked list
        bool instubborn; ///< This transition in stubborn set?
        Place* scapegoat;  ///< insufficiently marked place for disabled transition
        Transition** mustbeincluded;  ///< If this transition is in a stubborn set, these ones must be, too
        Transition** conflicting;  ///< Transitions that can disable this transition, used as mustbeincluded
        unsigned int dfs; ///< dfs number for depth first search through mustbeincluded graph
        unsigned int min; ///< lowlink used for finding SCC in mustbeincluded graph
        unsigned int stamp; ///< used for determining whether dfs values of this section stem from this search
        unsigned int mbiindex; ///< index in mustbeincluded graph: represents successor in depth first search through mustbeincluded graph;
        Transition* nextontarjanstack;  ///< represents Tarjan stack for scc detection in mustbeincluded graph
        Transition* nextoncallstack;  ///< represents call stack for scc detection in mustbeincluded graph
        static Transition* TarjanStack;  ///< Top of Tarjan stack in mustbeincluded graph search
        static Transition* CallStack;  ///< Top of call stack in mustbeincluded graph search
        bool visible; ///< can this transition alter any atomic proposition in used formula?
#endif
#ifdef MODELCHECKING
        unsigned int* lstdisabled;  ///<array containing, for each recursive search, largest dfsnum where this transition has been disabled
        ///< used for resolving fairness and ignorance issues
        unsigned int* lstfired;  ///< array containing, for each recursive search, largest dfsnum where this transition has fired.
        ///< used for resolving fairness and ignorance issues ///
#else
        bool down; ///< transition is in the global down set of a state predicate (see Kristensen/Valmari approach)
        unsigned int lastdisabled; ///< dfsnum of last state where
        ///< some fired transition disables this one
        ///< used for resolving fairness and ignorance issues
        unsigned int lastfired; ///< dfsnum of last state where this tr. was fired
        ///< used for resolving fairness and ignorance issues
        Transition** add_up;  ///< addditional transitions to be included when
        ///< this transition is in down set
        ///< used for resolving fairness and ignorance issues in the Kristensen/Valmari approach
#endif
        static Transition* StartOfIgnoredList;  ///< Anchor to Ignored list
        Transition* NextIgnored;                ///< list of potentially ignored transitions
#ifdef WITHFORMULA
        bool* pathrestriction;   ///< for which subformulas, this transition is in the cope of corresponding path quantifier?
        ///< used for extended CTL treatment
#endif
#if defined(CYCLE) || defined(STRUCT)
        bool cyclic ; ///< can firing potentially close cycle in reach-graph?
#endif
#ifdef COVER
        void traceback();///< fire transition backwards, but in Ancestor vector instead of Globals::CurrentMarking
        ///< used for walking backwards in coverability graph generation
#endif
#if defined(FAIRPROP) || defined(EVENTUALLYPROP) || defined(STABLEPROP) || defined(MODELCHECKING)
        unsigned int fairabled; ///< (#enabled [strongfair trans], #disabled [weak])
        unsigned int faired; ///< #succs inside scc [strongfair trans])
#endif
#ifdef SWEEP
        long int progress_value; ///< Delta of this transition to progress values of markings
#endif
};

/// constructor
inline Transition::Transition(char* name, unsigned int _fairness): Node(name), fairness(_fairness) {
    cnt += 1;
    if (!(cnt % REPORTFREQUENCY)) {
        message("%d transitions parsed", cnt);
    }
    enabled = false;  // real setting in first enabledness check
#ifdef STUBBORN
    stamp = 0; // not seen in any search
    NextStubborn = NULL;
    instubborn = false; // real set in init of stubbornm set
    visible = false; // real set upon parsing the formula
#endif
#ifdef EXTENDED
#ifndef MODELCHECKING
    lastfired = lastdisabled = 0;
    down = false;
    add_up = NULL;
#endif
#endif
#ifdef CYCLE
    cyclic = false ;
#endif
#if defined(FAIRPROP) || defined(EVENTUALLYPROP) || defined(STABLEPROP)
    faired = fairabled = 0;
#endif
}

/// calculate impact of this transition to hash value of a marking
inline void Transition::set_hashchange() {
    unsigned int i;

    hash_change = 0;
    for (i = 0; IncrPlaces[i] < Globals::Places[0]->cnt; i++) {
        hash_change += Incr[i] * Globals::Places[IncrPlaces[i]]->hash_factor;
    }
    for (i = 0; DecrPlaces[i] < Globals::Places[0]->cnt; i++) {
        hash_change -= Decr[i] * Globals::Places[DecrPlaces[i]]->hash_factor;
    }
    hash_change %= HASHSIZE;
}

/// destructor
inline Transition::~Transition() {
    cnt -= 1;
}

/// init transition specific arrays
inline void Transition::initialize() {
    unsigned int i, j, k;

    // Create list of Pre-Places for enabling test
    PrePlaces = new unsigned int  [NrOfArriving + 1];
    Pre = new unsigned int [NrOfArriving + 1];
    for (i = 0; i < NrOfArriving; i++) {
        PrePlaces[i] = ArrivingArcs[i]->pl->index;
        Pre[i] = ArrivingArcs[i]->Multiplicity;
    }
    PrePlaces[NrOfArriving] = UINT_MAX;
    // Create list of places where transition increments marking
    IncrPlaces = new unsigned int [NrOfLeaving + 1];
    Incr = new unsigned int [NrOfLeaving + 1];

    k = 0;
    for (i = 0; i < NrOfLeaving; i++) {
        //Is Place a loop place?
        for (j = 0; j < NrOfArriving; j++) {
            if ((LeavingArcs[i]->Destination) == (ArrivingArcs[j]->Source)) {
                break;
            }
        }
        if (j < NrOfArriving) {
            //yes, loop place
            if (LeavingArcs[i]->Multiplicity > ArrivingArcs[j]->Multiplicity) {
                // indeed, transition increments place
                IncrPlaces[k] = LeavingArcs[i]->pl->index;
                Incr[k] = LeavingArcs[i]->Multiplicity - ArrivingArcs[j]->Multiplicity;
                k++;
            }
        } else {
            // no loop place
            IncrPlaces[k] = LeavingArcs[i]->pl->index;
            Incr[k] = LeavingArcs[i]->Multiplicity;
            k++;
        }
    }
    IncrPlaces[k] = UINT_MAX;
    Incr[k] = 0;
    // Create list of places where transition decrements marking
    DecrPlaces = new unsigned int [NrOfArriving + 1];
    Decr = new unsigned int [NrOfArriving + 1];
    k = 0;
    for (i = 0; i < NrOfArriving; i++) {
        //Is Place a loop place?
        for (j = 0; j < NrOfLeaving; j++) {
            if ((ArrivingArcs[i]->Source) == (LeavingArcs[j]->Destination)) {
                break;
            }
        }
        if (j < NrOfLeaving) {
            //yes, loop place
            if (ArrivingArcs[i]->Multiplicity > LeavingArcs[j]->Multiplicity) {
                // indeed, transition decrements place
                DecrPlaces[k] = ArrivingArcs[i]->pl->index;
                Decr[k] = ArrivingArcs[i]->Multiplicity - LeavingArcs[j]->Multiplicity;
                k++;
            }
        } else {
            // no loop place
            DecrPlaces[k] = ArrivingArcs[i]->pl->index;
            Decr[k] = ArrivingArcs[i]->Multiplicity;
            k++;
        }
    }
    DecrPlaces[k] = UINT_MAX;
    Decr[k] = 0;

    // Create list of transitions where enabledness can change
    // if this transition fires. For collecting these transitions, we
    // abuse the Enabled linked list
    StartOfEnabledList = NULL;
    NrEnabled = 0;
    for (i = 0; IncrPlaces[i] < Globals::Places[0]->cnt; i++) {
        for (j = 0; j < Globals::Places[IncrPlaces[i]]->NrOfLeaving; j++) {
            if (!(Globals::Places[IncrPlaces[i]]->LeavingArcs)[j]->tr->enabled) {
                // not yet in list
                (Globals::Places[IncrPlaces[i]]->LeavingArcs)[j]->tr->NextEnabled = StartOfEnabledList;
                StartOfEnabledList = (Globals::Places[IncrPlaces[i]]->LeavingArcs)[j]->tr;
                NrEnabled++;
                (Globals::Places[IncrPlaces[i]]->LeavingArcs)[j]->tr->enabled = true;
            }
        }
    }
    ImproveEnabling = new Transition * [NrEnabled + 1];
    for (i = 0; StartOfEnabledList; StartOfEnabledList = StartOfEnabledList -> NextEnabled, i++) {
        StartOfEnabledList->enabled = false;
        ImproveEnabling[i] = StartOfEnabledList;
    }
    ImproveEnabling[i] = NULL;

    // Create list of transitions where enabledness can change
    // if this transition fires. For collecting these transitions, we
    // abuse the Enabled linked list
    StartOfEnabledList = NULL;
    NrEnabled = 0;
    for (i = 0; DecrPlaces[i] < Globals::Places[0]->cnt; i++) {
        for (j = 0; j < Globals::Places[DecrPlaces[i]]->NrOfLeaving; j++) {
            if (!(Globals::Places[DecrPlaces[i]]->LeavingArcs)[j]->tr->enabled) {
                // not yet in list
                (Globals::Places[DecrPlaces[i]]->LeavingArcs)[j]->tr->NextEnabled = StartOfEnabledList;
                StartOfEnabledList = (Globals::Places[DecrPlaces[i]]->LeavingArcs)[j]->tr;
                NrEnabled++;
                (Globals::Places[DecrPlaces[i]]->LeavingArcs)[j]->tr->enabled = true;
            }
        }
    }
    ImproveDisabling = new Transition * [NrEnabled + 1];
    for (i = 0; StartOfEnabledList; StartOfEnabledList = StartOfEnabledList -> NextEnabled, i++) {
        StartOfEnabledList->enabled = false;
        ImproveDisabling[i] = StartOfEnabledList;
    }
    ImproveDisabling[i] = NULL;

#ifdef STUBBORN
    // Create list of conflicting transitions. If this transition is enabled and member of a
    // stubborn set then these transitions must be in the stubborn set, too.
    StartOfEnabledList = NULL;
    NrEnabled = 0;
    for (i = 0; PrePlaces[i] < Globals::Places[0]->cnt; i++) {
        for (j = 0; j < Globals::Places[PrePlaces[i]]->NrOfLeaving; j++) {
            if (!((Globals::Places[PrePlaces[i]]->LeavingArcs)[j]->tr == this)) {
                if (!(Globals::Places[PrePlaces[i]]->LeavingArcs)[j]->tr->enabled) {
                    // not yet in list
                    (Globals::Places[PrePlaces[i]]->LeavingArcs)[j]->tr->NextEnabled = StartOfEnabledList;
                    StartOfEnabledList = (Globals::Places[PrePlaces[i]]->LeavingArcs)[j]->tr;
                    NrEnabled++;
                    (Globals::Places[PrePlaces[i]]->LeavingArcs)[j]->tr->enabled = true;
                }
            }
        }
    }

    // create "conflicting" array, copy data from EnabledList and reset EnabledList
    conflicting = new Transition * [NrEnabled + 1];
    for (i = 0; StartOfEnabledList; StartOfEnabledList = StartOfEnabledList -> NextEnabled, i++) {
        StartOfEnabledList->enabled = false;
        conflicting[i] = StartOfEnabledList;
    }
    conflicting[i] = NULL;
    mustbeincluded = conflicting;
#endif

    NrEnabled = 0;
    set_hashchange();
}

/// fire this transition on Globals::CurrentMarking, re-evaluate enabledness
inline void Transition::fire() {
    unsigned int* p;
    Transition** t;
    unsigned int* i;

#ifdef EXTENDED
#ifdef MODELCHECKING
    lstfired[formulaindex] = currentdfsnum;
#else
    lastfired = currentdfsnum;
#endif
#endif
    for (p = IncrPlaces, i = Incr; *p < UINT_MAX; p++, i++) {
#ifdef COVER
        if (Globals::Places[(*p)]-> bounded) {
#endif
            Globals::CurrentMarking[* p] += * i;
#ifdef CHECKCAPACITY
            if (Globals::CurrentMarking[*p] > Globals::Places[*p]->capacity) {
                statistics(NrOfStates, Edges, NonEmptyHash);
                message("capacity of place '%s' exceeded", _cimportant_(Globals::Places[*p]->name));
                message("CAPACITY=%d, but m(%s)=%d",
                        Globals::Places[*p]->capacity, _cimportant_(Globals::Places[*p]->name), Globals::CurrentMarking[*p]);

                if (Globals::resultfile) {
                    fprintf(Globals::resultfile, "capacity: {\n  exceeded = true;\n  capacity = %d;\n  place = \"%s\";\n  marking = %d;\n};\n", Globals::Places[*p]->capacity, Globals::Places[*p]->name, Globals::CurrentMarking[*p]);
                }


                exit(4);
            }
#endif
#ifdef COVER
#ifndef SWEEP
            Globals::Places[0]->hash_value += (*i) * Globals::Places[*p]->hash_factor;
            Globals::Places[0]->hash_value %= HASHSIZE;
#endif
        }
#endif
    }
    for (p = DecrPlaces, i = Decr; * p < UINT_MAX; p++, i++) {
#ifdef COVER
        if (Globals::Places[(*p)]-> bounded) {
#endif
            Globals::CurrentMarking[* p] -= * i;
#ifdef COVER
#ifndef SWEEP
            Globals::Places[0]->hash_value -= (*i) * Globals::Places[*p]->hash_factor;
            Globals::Places[0]->hash_value %= HASHSIZE;
#endif
        }
#endif
    }


#ifndef COVER
#ifndef SWEEP
    Globals::Places[0]->hash_value += hash_change;
    Globals::Places[0]->hash_value %= HASHSIZE;
#endif
#endif
    for (t = ImproveEnabling; *t; t++) {
        if (!((*t) -> enabled)) {
            (*t)->check_enabled();
        }
    }
    for (t = ImproveDisabling; *t; t++) {
        if ((*t)->enabled) {
            (*t)->check_enabled();
        }
    }
}

/// undo effect of firing transition (used for backtracking)
inline void Transition::backfire() {
    unsigned int* p;
    Transition** t;
    unsigned int* i;

    for (p = IncrPlaces, i = Incr; *p < UINT_MAX; p++, i++) {
#ifdef COVER
        if (Globals::Places[(*p)]->bounded) {
#endif
            Globals::CurrentMarking[* p] -= * i;
#ifdef COVER
#ifndef SWEEP
            Globals::Places[0]->hash_value -= (*i) * Globals::Places[*p]->hash_factor;
            Globals::Places[0]->hash_value %= HASHSIZE;
#endif
        }
#endif
    }
    for (p = DecrPlaces, i = Decr; * p < UINT_MAX; p++, i++) {
#ifdef COVER
        if (Globals::Places[(*p)]->bounded) {
#endif
            Globals::CurrentMarking[* p] += * i;
#ifdef COVER
#ifndef SWEEP
            Globals::Places[0]->hash_value += (*i) * Globals::Places[*p]->hash_factor;
            Globals::Places[0]->hash_value %= HASHSIZE;
#endif
        }
#endif
    }
#ifndef COVER
#ifndef SWEEP
    Globals::Places[0]->hash_value -= hash_change;
    Globals::Places[0]->hash_value %= HASHSIZE;
#endif
#endif
    for (t = ImproveEnabling; *t; t++) {
        if ((*t) -> enabled) {
            (*t)->check_enabled();
        }
    }
    for (t = ImproveDisabling; *t; t++) {
        if (!((*t)->enabled)) {
            (*t)->check_enabled();
        }
    }
}

#ifdef COVER
extern unsigned int* Ancestor;

/// proceed to predeccessor marking in Ancestor vector
inline void Transition::traceback() {
    unsigned int* p;
    unsigned int* i;

    for (p = IncrPlaces, i = Incr; *p < UINT_MAX; p++, i++) {
        if (Globals::Places[(*p)]->bounded) {
            Ancestor[* p] -= * i;
        }
    }
    for (p = DecrPlaces, i = Decr; * p < UINT_MAX; p++, i++) {
        if (Globals::Places[(*p)]->bounded) {
            Ancestor[* p] += * i;
        }
    }
}
#endif

/// check activation of this transition
inline void Transition::check_enabled() {
    unsigned int* p;
    unsigned int* i;

    for (p = PrePlaces , i = Pre ; *p < UINT_MAX; p++ , i++) {
        if (Globals::CurrentMarking[*p] < *i) {
            if (enabled) {
#ifdef EXTENDED
#ifdef MODELCHECKING
                lstdisabled[formulaindex] = currentdfsnum;
#else
                lastdisabled = currentdfsnum;
#endif
#endif
                // exclude transition from list of enabled transitions
                if (NextEnabled) {
                    NextEnabled -> PrevEnabled = PrevEnabled;
                }
                if (PrevEnabled) {
                    PrevEnabled -> NextEnabled = NextEnabled;
                } else {
                    StartOfEnabledList = NextEnabled;
                }
                enabled = false;
                NrEnabled--;
            }
#ifdef STUBBORN
            mustbeincluded = Globals::Places[(*p)]->PreTransitions;
            scapegoat = Globals::Places[*p];
#endif
            return;
        }
    }
    if (!enabled) {
        // include transition into list of enabled transitions
        NextEnabled = StartOfEnabledList;
        if (StartOfEnabledList) {
            NextEnabled -> PrevEnabled = this;
        }
        StartOfEnabledList = this;
        PrevEnabled = NULL;
        enabled = true;
        NrEnabled++;
#ifdef STUBBORN
        mustbeincluded = conflicting;
#endif
    }
}
