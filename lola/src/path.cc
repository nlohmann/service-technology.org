/*****************************************************************************\
 LoLA -- a Low Level Petri Net Analyzer

 Copyright (C)  1998, 1999, 2000, 2001, 2002, 2003, 2004, 2005, 2006, 2007,
                2008, 2009  Karsten Wolf <lola@service-technology.org>

 LoLA is free software: you can redistribute it and/or modify it under the
 terms of the GNU Affero General Public License as published by the Free
 Software Foundation, either version 3 of the License, or (at your option)
 any later version.

 LoLA is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for
 more details.

 You should have received a copy of the GNU Affero General Public License
 along with LoLA.  If not, see <http://www.gnu.org/licenses/>.
\*****************************************************************************/


#include <cstdlib>
#include <config.h>
#include "dimensions.h"
#include "net.h"
#include "formula.h"
#include "graph.h"
#include "stubborn.h"
#include "check.h"
#include "Globals.h"
#include "verbose.h"

#ifdef FINDPATH

Transition** Reserve;  // Transiitons rejected in first selection scheme
unsigned int Rescounter; // next free entry in Reserve
unsigned int* hashcounter;
unsigned int* globalhashcounter;

#ifdef HAVE_DRAND48
#define myRand() drand48()
#else
#define myRand() rand()
#endif

#define stubbinsert(X)  {\
        if(!((X)->instubborn == StubbornStamp))\
        {\
            if((X)->enabled)\
            {\
                if((myRand() <= (1.0 / (1.0 + globalhashcounter[(Globals::Places[0]->hash_value+(X)->hash_change) % HASHSIZE] ))))\
                {\
                    return(X);\
                }\
                else\
                {\
                    Reserve[Rescounter++] = (X);\
                }\
            }\
            (X)->instubborn = StubbornStamp;\
            (X)->NextStubborn = NULL;\
            if((X) -> StartOfStubbornList)\
            {\
                (X) -> EndOfStubbornList -> NextStubborn = (X);\
                (X) -> EndOfStubbornList = (X);\
            }\
            else\
            {\
                (X) -> StartOfStubbornList = (X) -> EndOfStubbornList = (X);\
            }\
        }\
    }
void printstate(const char*, unsigned int*);
unsigned int StubbornStamp;


#ifdef STUBBORN
Transition* closure() {
    Transition* current;
    int i;

    for (current = current -> StartOfStubbornList; current; current = current -> NextStubborn) {
        for (i = 0; current -> mustbeincluded[i]; i++) {
            stubbinsert(current->mustbeincluded[i]);
        }
    }
    return NULL;
}

Transition* insert_down(formula*);

Transition* insert_up(formula* f) {

    unsigned int i;
    Transition* tt;

    switch (f -> type) {
        case neq:
            for (i = 0; i < static_cast<atomicformula*>(f) ->p->NrOfArriving; i++) {
                stubbinsert(static_cast<atomicformula*>(f) ->p->ArrivingArcs[i]->tr);
            }
            for (i = 0; i < static_cast<atomicformula*>(f) ->p->NrOfLeaving; i++) {
                stubbinsert(static_cast<atomicformula*>(f) ->p->LeavingArcs[i]->tr);
            }
            break;
        case eq:
            if (Globals::CurrentMarking[static_cast<atomicformula*>(f) ->p->index] < static_cast<atomicformula*>(f)  -> k) {
                for (i = 0; i < static_cast<atomicformula*>(f) ->p->NrOfArriving; i++) {
                    stubbinsert(static_cast<atomicformula*>(f) ->p->ArrivingArcs[i]->tr);
                }
            } else {
                for (i = 0; i < static_cast<atomicformula*>(f) ->p->NrOfLeaving; i++) {
                    stubbinsert(static_cast<atomicformula*>(f) ->p->LeavingArcs[i]->tr);
                }
            }
            break;
        case leq:
        case lt:
            for (i = 0; i < static_cast<atomicformula*>(f) ->p->NrOfLeaving; i++) {
                stubbinsert(static_cast<atomicformula*>(f) ->p->LeavingArcs[i]->tr);
            }
            break;
        case geq:
        case gt:
            for (i = 0; i < static_cast<atomicformula*>(f) ->p->NrOfArriving; i++) {
                stubbinsert(static_cast<atomicformula*>(f) ->p->ArrivingArcs[i]->tr);
            }
            break;
        case conj:
            return(insert_up(static_cast<booleanformula*>(f)->sub[0]));
        case disj:
            for (i = 0; i < static_cast<booleanformula*>(f)->cardsub; i++) {
                if (tt = insert_up(static_cast<booleanformula*>(f)->sub[i])) {
                    return tt;
                }
            }
            return NULL;
        case neg:
            return(insert_down(static_cast<unarybooleanformula*>(f) ->sub));
            break;
        default:
            cout << "feature not implemented\n";
    }
    return NULL;
}

Transition* insert_down(formula* f) {

    unsigned int i;
    Transition* tt;

    switch (f -> type) {
        case neq:
            if (Globals::CurrentMarking[static_cast<atomicformula*>(f) ->p->index] < static_cast<atomicformula*>(f) ->k) {
                for (i = 0; i < static_cast<atomicformula*>(f) ->p->NrOfArriving; i++) {
                    stubbinsert(static_cast<atomicformula*>(f) ->p->ArrivingArcs[i]->tr);
                }
            } else {
                for (i = 0; i < static_cast<atomicformula*>(f) ->p->NrOfLeaving; i++) {
                    stubbinsert(static_cast<atomicformula*>(f) ->p->LeavingArcs[i]->tr);
                }
            }
            break;
        case eq:
            for (i = 0; i < static_cast<atomicformula*>(f) ->p->NrOfArriving; i++) {
                stubbinsert(static_cast<atomicformula*>(f) ->p->ArrivingArcs[i]->tr);
            }
            for (i = 0; i < static_cast<atomicformula*>(f) ->p->NrOfLeaving; i++) {
                stubbinsert(static_cast<atomicformula*>(f) ->p->LeavingArcs[i]->tr);
            }
            break;
        case geq:
        case gt:
            for (i = 0; i < static_cast<atomicformula*>(f) ->p->NrOfLeaving; i++) {
                stubbinsert(static_cast<atomicformula*>(f) ->p->LeavingArcs[i]->tr);
            }
            break;
        case leq:
        case lt:
            for (i = 0; i < static_cast<atomicformula*>(f) ->p->NrOfArriving; i++) {
                stubbinsert(static_cast<atomicformula*>(f) ->p->ArrivingArcs[i]->tr);
            }
            break;
        case disj:
            return(insert_down(static_cast<booleanformula*>(f) ->sub[(rand() % (static_cast<booleanformula*>(f) ->cardsub - static_cast<booleanformula*>(f) ->firstvalid)) + static_cast<booleanformula*>(f) ->firstvalid]));
        case conj:
            for (i = static_cast<booleanformula*>(f)->firstvalid; i < static_cast<booleanformula*>(f)->cardsub; i++) {
                if (tt = insert_down(static_cast<booleanformula*>(f)->sub[i])) {
                    return tt;
                }
            }
            return NULL;
        case neg:
            return(insert_up(static_cast<unarybooleanformula*>(f) ->sub));
            break;
        default:
            cout << "feature not implemented\n";
    }
    return NULL;
}
#endif

#ifndef STUBBORN
Transition* GetFullTransition() {
    Transition** fl;
    Transition* t;

    fl = firelist();
    if (fl && CardFireList) {
        t = fl[rand() % CardFireList];
    } else {
        t = NULL;
    }
    delete [] fl;
    return(t);
}
#endif

#ifdef STUBBORN
Transition* GetStubbornTransition() {
    Transition* t;

    Globals::Transitions[0]->StartOfStubbornList = NULL;
    StubbornStamp++;
    Rescounter = 0;
    if (t = insert_up(F)) {
        return t;
    }
    if (t = closure()) {
        return t;
    }
    if (Rescounter) {
        return Reserve[rand() % Rescounter];
    } else {
        return NULL;
    }
}
#endif

unsigned int j;
unsigned int attempt;
unsigned int nonemptyhash;
unsigned int globalnonemptyhash;
unsigned int x;
extern  unsigned int NrOfStates;
unsigned int depth;
Transition** path;
unsigned int k; // current path element;


unsigned int* strichliste;

void find_path() {

//Transition ** otherpath;
//Transition ** tmppath;
//unsigned int sss;
//strichliste = new unsigned int [12];
//enabledstatistic = new unsigned int [45];

    int res;
    unsigned int OriginalNrEnabled;
    Reserve = new Transition * [Globals::Transitions[0]->cnt];
    if (!F) {
        abort(4, "specify predicate in analysis task file - mandatory for task %s", _cparameter_("FINDPATH"));
    }
    F = F -> reduce(& res);
    if (res == 0) {
        return;
    }
    if (res == 1) {
        _exit(0);
    }
    F = F -> posate();
    F -> tempcard = 0;
    F -> setstatic();
    if (F -> tempcard > 0) {
        abort(3, "do not use temporal operators in this mode - not allowed for task %s", _cparameter_("FINDPATH"));
    }
    cout << "\nFormula with\n" << F -> card << " subformulas.\n";
    attempt = 0;
    StubbornStamp = 0;
    // 1. initial state speichern
    for (x = 0; x < Globals::Places[0] -> cnt; x++) {
        Globals::Places[x]->initial_marking = Globals::CurrentMarking[x];
    }
    // store change of hash values by transition occurrences
    for (x = 0; x < Globals::Transitions[0]->cnt; x++) {
        Globals::Transitions[x] -> set_hashchange();
    }
    // 2. new hash table
    hashcounter = new unsigned int [HASHSIZE + 1];
    globalhashcounter = new unsigned int [HASHSIZE + 1];
    // 3. new path
// was like this, but did not compile:  path = new (Transition *)[MAXPATH+1];
    path = new Transition *[MAXPATH + 1];
//otherpath = new (Transition *) [MAXPATH+1];

    // 4. optimize formula
    // 4.1. create, for all places, list of propositions mentioning place
    // Pass 1 count;
    F->parent = NULL;
    for (x = 0; x < HASHSIZE; x++) {
        globalhashcounter[x] = 0;
    }
    globalnonemptyhash = 1;
    OriginalNrEnabled = Globals::Transitions[0]->NrEnabled;
    while (1) {
//for(sss=0;sss<11;sss++)
//{
//strichliste[sss] = 0;
//}
//for(sss=0;sss<45;sss++)
//{
//  enabledstatistic[sss] = 0;
//}
        // init new search attempt
        attempt++;
        message("Attempt #%d", attempt);
        for (x = 0; x < Globals::Places[0]->cnt; x++) {
            Globals::Places[x]->set_marking(Globals::Places[x]->initial_marking);
        }
        for (x = 0; x < Globals::Transitions[0]->cnt; x++) {
            Globals::Transitions[x]->enabled = true;
            if (x < Globals::Transitions[0]->cnt - 1) {
                Globals::Transitions[x]->NextEnabled = Globals::Transitions[x + 1];
            }
            if (x) {
                Globals::Transitions[x]->PrevEnabled = Globals::Transitions[x - 1];
            }
#ifdef STUBBORN
            Globals::Transitions[x]->mustbeincluded =
                Globals::Transitions[x]->conflicting;
#endif
        }
        Globals::Transitions[0]->StartOfEnabledList = Globals::Transitions[0];
        Globals::Transitions[Globals::Transitions[0]->cnt - 1]->NextEnabled = NULL;
        Globals::Transitions[0]->PrevEnabled = NULL;
        for (x = 0; x < Globals::Transitions[0]->cnt; x++) {
            Globals::Transitions[x]->check_enabled();
#ifdef STUBBORN
            Globals::Transitions[x]->instubborn = 0;
#endif
        }
        Globals::Transitions[0]->NrEnabled = OriginalNrEnabled;
        for (x = 0; x < HASHSIZE; x++) {
            hashcounter[x] = 0;
        }
        nonemptyhash = NrOfStates = 1;
        hashcounter[Globals::Places[0]->hash_value] ++;
        globalhashcounter[Globals::Places[0]->hash_value] ++;
        if (F -> initatomic()) {
            cout << "\n\nInitial state satisfies formula\n\n";
            if (Globals::pflg) {
                ofstream pathstream(Globals::pathfile);
                if (!pathstream) {
                    abort(4, "cannot open path output file '%s' - no output written", _cfilename_(Globals::pathfile));
                }
                pathstream << "PATH\n";
            }
            if (Globals::Pflg) {
                cout <<  "PATH\n";
            }
            return;
        }
        // SEARCH!!
        for (k = 0; k < MAXPATH; k++) {
            Transition* t;

            if (!((NrOfStates) % REPORTFREQUENCY)) {
                message("Depth %d", NrOfStates);
            }
            if (!(t =
#ifdef STUBBORN
                        GetStubbornTransition()
#else
                        GetFullTransition()
#endif
                 )) {
                cerr << "DEADLOCK at depth " << k << "\n";
                break; // Deadlock!
            }
            t->fire();
            path[k] = t;
            update_formula(t);
            if (F->value) {
                cout << "\n\n State found!\n\n";
                if (Globals::pflg) {
                    ofstream pathstream(Globals::pathfile);
                    if (!pathstream) {
                        abort(4, "cannot open path output file '%s' - no output written", _cfilename_(Globals::pathfile));
                    }
                    pathstream << "PATH\n";
                    for (j = 0; j <= k; j++) {
                        pathstream << path[j]->name << "\n";
                    }
                }
                if (Globals::Pflg) {
                    cout << "PATH\n";
                    for (j = 0; j <= k; j++) {
                        cout << path[j]->name << "\n";
                    }
                }
                printstate("", Globals::CurrentMarking);
                return;
            }
            NrOfStates++;
            if (!hashcounter[Globals::Places[0]->hash_value]) {
                nonemptyhash++;
            }
            if (!globalhashcounter[Globals::Places[0]->hash_value]) {
                globalnonemptyhash++;
            }
            hashcounter[Globals::Places[0]->hash_value]++;
            globalhashcounter[Globals::Places[0]->hash_value]++;
        }
        cerr << "hash entries (this attempt): " << nonemptyhash << " (all attempts): " << globalnonemptyhash << "\n";
    }
}

#endif
