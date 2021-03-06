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


#include "net.h"
#include "formula.h"
#include "graph.h"
#include "stubborn.h"
#include "dimensions.h"
#include "reports.h"
#include "Globals.h"
#include "verbose.h"

#ifdef DISTRIBUTE
#include "distribute.h"
#endif

#include <fstream>
#include <cstring>
#include <cstdio>

Decision** HashTable;
unsigned int*   BitHashTable;
unsigned int LastChoice;
Decision* LastDecision;
unsigned int Scapegoat;
Statevector* LastVector;
unsigned int currentdfsnum = 1;
SearchTrace* Trace;
unsigned int CardFireList;
unsigned int pos;
unsigned int isbounded;
unsigned int State::card = 0;
binDecision** binHashTable;
unsigned int largest_sat = 0; ///< largest dfs + 1 of a state satisfying given predicate
unsigned int bin_p; ///< (=place); index in MARKINGVECTOR
unsigned int bin_pb; ///< next bit of place to be processed;
unsigned char bin_byte; ///< byte to be matched against tree vector; constructed from MARKINGVECTOR
unsigned int bin_t; ///< index in tree vector
unsigned char* bin_v;  ///< current tree vector
unsigned int bin_s; ///< nr of bits pending in byte from previous iteration
unsigned int bin_d; ///< difference position
unsigned int bin_b; ///< bit nr at start of byte
unsigned int bin_dir; ///< bit nr at start of byte
binDecision* fromdec, * todec, *vectordec;

#ifdef FULLTARJAN
State* TarStack;
#endif

int Persistents = 0;

#ifdef COVER
unsigned int* Ancestor;
#endif

char rapportstring[] = "search";

unsigned int NrOfStates;
unsigned int Edges;


inline void report_statistics() {
    message("st: %10d edg: %10d", NrOfStates, Edges);
}



/*!
 \brief print statistics on the generated state space

 \param s  the number of states
 \param e  the number of edges
 \param h  the number of hash table entries

 \note The preprocessor macro could be removed here, when this function is
       just not called in any function of STATESPACE.
*/
void statistics(unsigned int s, unsigned int e, unsigned int h) {
#ifndef STATESPACE
//    cerr << "\n\n";
    message(">>>>> %d States, %d Edges, %d Hash table entries", s, e, h);
//    cerr << "\n";
//    cout << "\n\n>>>>> " << s << " States, " << e << " Edges, " << h << " Hash table entries\n\n";

    static bool first = true;
    if (Globals::resultfile && first) {
        fprintf(Globals::resultfile, "statistics: {\n  states = %d;\n  edges = %d;\n  hash_table_entries = %d;\n};\n", s, e, h);
        first = false;
    }
#endif
}



/*!
  \brief return list of enabled transitions
 */
Transition** firelist() {
    Transition** tl;
    Transition* t;
    int i;
    tl = new Transition * [Globals::Transitions[0]->NrEnabled + 1];
    for (i = 0, t = Globals::Transitions[0]->StartOfEnabledList; t; t = t->NextEnabled) {

        // when using EXTENDEDCTL, only return enabled transitions satisfying
        // the transition formula
#ifdef EXTENDEDCTL
        if (t->pathrestriction[TemporalIndex]) {
#endif
            tl[i++] = t;

#ifdef EXTENDEDCTL
        }
#endif
    }
    tl[i] = NULL;
    CardFireList = i;
    return tl;
}




void printstate(char const*, unsigned int*);
void printmarking() {
    unsigned int i;

    for (i = 0; i < Globals::Places[0]->cnt; i++) {
        cout << Globals::Places[i]->name << " : " << Globals::CurrentMarking[i] << endl;
    }
    cout << "-------" << endl;
}

StatevectorList* TSCCRepresentitives;

#ifndef MODELCHECKING

#ifdef STUBBORN
#include"stubborn.h"
#endif
State* SEARCHPROC();
unsigned int MinBookmark; // MIN number of the first closed marking
// in the currently or last recently processed TSCC





/*!
 \brief print a path

 Print a path from the initial state to the current state. Therefore, the
 "parent" relation of the states is traversed until a state without parents
 (i.e., the initial state) is found. Then, the path can be printed in the
 correct order.

 \param s                    current state
 \param pathstream           the stream to write to (will be initialized)

 \note This function is very similar to the print_reg_path() function.
*/
void print_path(State* s, ostream* pathstream = NULL) {
    // check if this is the first call (i.e., pathstream == NULL)
    if (pathstream == NULL) {
        if (!Globals::pflg && ! Globals::Pflg && !Globals::resultfile) {
            return;
        }

        // prepare pathstream: either open a file or point to stdout
        if (Globals::pflg) {
            pathstream = new ofstream(Globals::pathfile);
            if (pathstream->fail()) {
                abort(4, "cannot open path output file '%s' - no output written", _cfilename_(Globals::pathfile));
            }
        } else {
            pathstream = &cout;
        }

        if (Globals::resultfile) {
            fprintf(Globals::resultfile, "  path = (");
        } else {
            (*pathstream) << "PATH \n";
        }

        // recursive call
        print_path(s, pathstream);

        if (Globals::resultfile) {
            fprintf(Globals::resultfile, ");\n");
        }

        // close pathstream to force file output
        if (pathstream != &cout) {
            delete pathstream;
        }

        return;
    }


    // print a path from initial state to s
    if (s->parent) {
        // recursive call
        print_path(s->parent, pathstream);

        if (Globals::resultfile) {
            static bool comma = false;
            if (comma) {
                fprintf(Globals::resultfile, ", ");
            } else {
                comma = true;
            }
            fprintf(Globals::resultfile, "\"%s\"", s->parent->firelist[s->parent->current]->name);
            return;
        }

        (*pathstream) << s->parent->firelist[s->parent->current]->name << "\n";
    }
}





/*!
  \brief print parts of the built reachability graph

  This function is called if the -M/-m/-g/-G parameter is given and the
  search is successfully aborted. In this setting, it is possible that not
  every successor state for each state is calculated. Then, only the activated
  transitions with "?" as target are printed.

  \param s            the current state
  \param graphstream  the stream to write to
  \param level        1 -->top level, no firelist, mark '!'
                      0 -->other level, firelist,  mark '*'

  \note In case the search is not prematurely aborted, the reachability
        graph is printed in the respective search function.
*/
void printincompletestates(State* s, ostream* graphstream, int level = 1) {
#ifndef CYCLE
#ifdef TARJAN
    int i, j;

    if (!s) {
        return;
    }

    // output to result file
    if (Globals::resultfile) {
        static bool comma = false;
        if (comma) {
            fprintf(Globals::resultfile, ",");
        } else {
            comma = true;
        }
        fprintf(Globals::resultfile, "\n    { id = %d; ", s->dfs);
        if (level) {
            fprintf(Globals::resultfile, "\n      witness = true;");
        }
        fprintf(Globals::resultfile, "\n      state = (");

        for (i = 0, j = 0; i < Globals::Places[0]->cnt; ++i) {
            if (Globals::CurrentMarking[i]) {
                if (Globals::CurrentMarking[i] == VERYLARGE) {
                    fprintf(Globals::resultfile, "%s(\"%s\", -1)", (j++ ? ", " : ""), Globals::Places[i]->name);
                } else {
                    fprintf(Globals::resultfile, "%s(\"%s\", %d)", (j++ ? ", " : ""), Globals::Places[i]->name, Globals::CurrentMarking[i]);
                }
            }
        }

        fprintf(Globals::resultfile, ");\n");
        fprintf(Globals::resultfile, "      successors = (");

        if (!level) {
            bool comma = false;
            for (i = 0; i < s->current; ++i) {
                if (comma) {
                    fprintf(Globals::resultfile, ", ");
                } else {
                    comma = true;
                }
                fprintf(Globals::resultfile, "(\"%s\", %d)", s->firelist[i]->name, s->succ[i]->dfs);
            }
            if (comma) {
                fprintf(Globals::resultfile, ", ");
            } else {
                comma = true;
            }
            fprintf(Globals::resultfile, "(\"%s\", %d)", s->firelist[i]->name, s->succ[i]->dfs);
            for (i = s->current + 1; s->firelist[i]; ++i) {
                fprintf(Globals::resultfile, ", (\"%s\", -1)", s->firelist[i]->name);
            }
        }

        fprintf(Globals::resultfile, "); }");
    }


    // output to stream (file or stdout)
    if (Globals::gmflg || Globals::GMflg) {
        (*graphstream) << "STATE " << (level ? "! " : "* ") << s->dfs ;
        j = 0;
        if (Globals::graphformat == 'm') {
            for (i = 0; i < Globals::Places[0]->cnt; ++i) {
                if (Globals::CurrentMarking[i]) {
                    if (Globals::CurrentMarking[i] == VERYLARGE) {
                        (*graphstream) << (j++ ? ",\n" : "\n") << Globals::Places[i]->name << " : " << "oo" ;
                    } else {
                        (*graphstream) << (j++ ? ",\n" : "\n") << Globals::Places[i]->name << " : " << Globals::CurrentMarking[i] ;
                    }
                }
            }

        }
        (*graphstream) << "\n\n";
        if (!level) {
            for (i = 0; i < s->current; ++i) {
                (*graphstream) << s->firelist[i]->name << " -> " << s->succ[i]->dfs << "\n";
            }
            (*graphstream) << s->firelist[s->current]->name << " => " << s->succ[s->current]->dfs << "\n";
            for (i = s->current + 1; s->firelist[i]; ++i) {
                (*graphstream) << s->firelist[i]->name << "->?\n";
            }
        }
        (*graphstream) << "\n";
    }

    if (!s->parent) {
        return;
    }

#ifdef COVER
    if (s->NewOmega) {
        // Replace new omegas by their old values
        for (i = 0; s->NewOmega[i]; ++i) {
            s->NewOmega[i]->set_cmarking(s->NewOmega[i]->lastfinite);
            s->NewOmega[i]->bounded = true;
        }
        delete[] s->NewOmega;
    }
#endif

    s->parent->firelist[s->parent->current]->backfire();
    printincompletestates(s->parent, graphstream, 0);
#endif
#endif

    // we return from initial call: clean up graph stream
    if (level == 1) {
        if (graphstream != &cout) {
            delete graphstream;
        }
    }
}

#ifdef COVER
/*!
 \brief print a regular path expression

 This function is called if the -M/-m/-g/-G parameter is given and an
 w-marking was found. As witness of the unboundedness, a lasso path is printed.
 It consists of a (possibly empty) acyclic path to a cycle on which the
 marking for the unbounded can grow arbitrarily.

 \param s                    current state
 \param startofrepeatingseq  state from which the cycle begins
 \param pathstream           the stream to write to (will be initialized)

 \note This function is very similar to the print_path() function.
*/
void print_reg_path(State* s, State* startofrepeatingseq, ostream* pathstream = NULL) {
    // check if this is the first call (i.e., pathstream == NULL)
    if (pathstream == NULL) {
        if (!Globals::pflg && ! Globals::Pflg && !Globals::resultfile) {
            return;
        }

        // prepare pathstream: either open a file or point to stdout
        if (Globals::pflg) {
            pathstream = new ofstream(Globals::pathfile);
            if (pathstream->fail()) {
                abort(4, "cannot open path output file '%s' - no output written", _cfilename_(Globals::pathfile));
            }
        } else {
            pathstream = &cout;
        }

        if (Globals::resultfile) {
            fprintf(Globals::resultfile, "  path = (");
        } else {
            (*pathstream) << "PATH EXPRESSION \n";
        }

        // recursive call
        print_reg_path(s, startofrepeatingseq, pathstream);

        if (Globals::resultfile) {
            fprintf(Globals::resultfile, ");\n");
        }

        // close pathstream to force file output
        if (pathstream != &cout) {
            delete pathstream;
        }

        return;
    }

    static bool comma = false;

    // print a regular expression for path  from initial state to
    // generalised state s in coverability graph
    if (startofrepeatingseq) {
        if (s == startofrepeatingseq) {
            if (s->parent) {
                // unused: int i;
                print_reg_path(s->parent, s->smaller, pathstream);
                if (s->smaller) {
                    if (Globals::Pflg || Globals::pflg) {
                        (*pathstream) << " ";
                    }
                } else {
                    if (Globals::Pflg || Globals::pflg) {
                        (*pathstream) << "\n";
                    }
                }
                if (Globals::Pflg || Globals::pflg) {
                    (*pathstream) << s->parent->firelist[s->parent->current]->name;
                } else {
                    if (comma) {
                        fprintf(Globals::resultfile, ", ");
                    }
                    fprintf(Globals::resultfile, "\"%s\"", s->parent->firelist[s->parent->current]->name);
                    comma = false;
                }
            }
            if (s->smaller) {
                if (Globals::Pflg || Globals::pflg) {
                    (*pathstream) << " )";
                }
            }
            if (Globals::Pflg || Globals::pflg) {
                (*pathstream) << "\n(";
            } else {
                if (comma) {
                    fprintf(Globals::resultfile, ", ");
                }
                fprintf(Globals::resultfile, ");\n  cycle = (");
                comma = false;
            }
        } else {
            if (s->parent) {
                // unused: int i;
                print_reg_path(s->parent, startofrepeatingseq, pathstream);
                if (Globals::Pflg || Globals::pflg) {
                    (*pathstream)  << " " << s->parent->firelist[s->parent->current]->name;
                } else {
                    if (comma) {
                        fprintf(Globals::resultfile, ", ");
                    } else {
                        comma = true;
                    }
                    fprintf(Globals::resultfile, "\"%s\"", s->parent->firelist[s->parent->current]->name);
                }
            }
            if (s->smaller) {
                if (Globals::Pflg || Globals::pflg) {
                    (*pathstream) << " )";
                }
            }
        }
    } else {
        if (s->parent) {
            // unused: int i;
            print_reg_path(s->parent, s->smaller, pathstream);
            if (s->smaller) {
                if (Globals::Pflg || Globals::pflg) {
                    (*pathstream) << " ";
                }
            } else {
                if (Globals::Pflg || Globals::pflg) {
                    (*pathstream) << "\n";
                }
            }
            if (Globals::Pflg || Globals::pflg) {
                (*pathstream) << s->parent->firelist[s->parent->current]->name;
            } else {
                if (comma) {
                    fprintf(Globals::resultfile, ", ");
                } else {
                    comma = true;
                }
                fprintf(Globals::resultfile, "\"%s\"", s->parent->firelist[s->parent->current]->name);
            }

        }
        if (s->smaller) {
            if (Globals::Pflg || Globals::pflg) {
                (*pathstream) << " )";
            }
        }
    }
}
#endif





#define MIN(X,Y) ( (X) < (Y) ? (X) : (Y))
#define MAX(X,Y) ( (X) > (Y) ? (X) : (Y))

State* CurrentState;




#if defined(FAIRPROP) || defined(EVENTUALLYPROP) || defined(STABLEPROP)
bool analyse_fairness(State* pool, unsigned int level) {
    // 1. cut the (not necessarily connected) graph in pool into sccs.
    //    All states in search space have tarlevel = level && ! expired.
    //    Before return "false", all states in pool must be "expired".

    State* C, * N; // current, new state in dfs
    State* T;  // local tarjan stack;
    State* S;  // local scc
    unsigned int card = 1;
    State* O;  // parent of root of local scc (must be set after analysis of scc)
    unsigned int oldd, oldm, oldc;

    while (pool) {
        // proceed as long as there are states in pool
        // choose element from pool
        C = pool;
        pool = pool->nexttar;
        if (pool == C) {
            pool = NULL;
        }
        T = C;
        // unlink from pool and init new dfs
        C->nexttar->prevtar = C->prevtar;
        C->prevtar->nexttar = C->nexttar;
        C->nexttar  = C->prevtar = C;
        C->current = 0;
        C->tarlevel = level + 1;
        C->parent = NULL;
        C->ddfs = C->mmin = 1;
        while (C) {
            if (C->firelist[C->current]) {
                N = C->succ[C->current];
                if (N->tarlevel == level && ! N->expired) {
                    // new state
                    N->mmin = N->ddfs = ++card;
                    N->current = 0 ;
                    N->parent = C;
                    N->tarlevel = level + 1;
                    // put state on local tarjan stack,
                    // and remove it from pool
                    if (N == pool) {
                        pool = pool->nexttar;
                        if (pool == N) {
                            pool = NULL;
                        }
                    }
                    N->nexttar->prevtar = N->prevtar;
                    N->prevtar->nexttar = N->nexttar;
                    if (T) {
                        N->nexttar = T->nexttar;
                        T->nexttar = N;
                        N->prevtar = T;
                        N->nexttar->prevtar = N;
                    } else {
                        N->nexttar = N->prevtar = N;
                    }
                    T = N;
                    C = N;
                } else {
                    if (N->tarlevel == level + 1 && ! N->expired) {
                        // existing state in same scc
                        C->mmin = MIN(N->ddfs, C->mmin);
                        C->current++;
                    } else {
                        // existing state in expired scc or outside
                        // current search space
                        C->current++;
                    }
                }
            } else {
                // close state
                // A) check scc
                if (C->mmin == C->ddfs) {
                    // data of C must be preserved for proper backtracking
                    // after having finished this scc
                    O = C->parent;
                    oldc = C->current;
                    oldd = C->ddfs;
                    oldm = C->mmin;
                    // B) process scc
                    // B0) unlink new component from local tarjan stack
                    if (C->ddfs > 1) { // proper cut
                        C->prevtar->nexttar = T->nexttar;
                        T->nexttar->prevtar = C->prevtar;
                        T->nexttar = C;
                        S = C->prevtar;
                        C->prevtar = T;
                        T = S;
                    }
                    S = C;

                    // B1) check fairness of new scc
                    State* ss;
#ifdef STABLEPROP
                    unsigned int cardphi;
                    cardphi = 0;
#endif
                    unsigned int cardS;
                    unsigned int i;
                    for (i = 0; i < Globals::Transitions[0]->cnt; i++) {
                        Globals::Transitions[i]->faired = Globals::Transitions[i]->fairabled = 0;
                    }
                    for (cardS = 1, ss = S->nexttar;; cardS++, ss = ss->nexttar) {
                        for (i = 0; ss->firelist[i]; i++) {
                            ss->firelist[i]->fairabled ++;
                            if ((ss->succ[i]->tarlevel == level + 1) && !(ss->succ[i]->expired)) {
                                ss->firelist[i]->faired ++;
                            }
                        }
#ifdef STABLEPROP
                        if (ss->phi) {
                            cardphi ++;
                        }
#endif
                        if (ss == S) {
                            break;
                        }
                    }

#ifdef STABLEPROP
                    if (cardphi ==  cardS) {
                        goto aftercheck;
                    }
#endif
                    for (i = 0; i < Globals::Transitions[0]->cnt; i++) {
                        if (Globals::Transitions[i]->fairness > 0) {
                            if ((!Globals::Transitions[i]->faired) &&
                                    Globals::Transitions[i]->fairabled == cardS) {
                                goto aftercheck;
                                // no subset can be fair
                            }
                        }
                    }
                    for (i = 0; i < Globals::Transitions[0]->cnt; i++) {
                        if (Globals::Transitions[i]->fairness == 2) {
                            if (Globals::Transitions[i]->fairabled && ! Globals::Transitions[i]->faired) {
                                // 1. remove all transitions
                                // from S that enable t[i]
                                // At this point, there must
                                // be some state remaining in S,
                                // otherwise the weak fairness test
                                // would have failed.
                                while (Globals::Transitions[i]->fairabled) {
                                    State* E;
                                    unsigned int j;
                                    E = NULL;
                                    for (j = 0; S->firelist[j]; j++) {
                                        if (S->firelist[j] == Globals::Transitions[i]) {
                                            E = S;
                                            break;
                                        }
                                    }
                                    S = S->nexttar;
                                    if (E) {
                                        E->expired = true;
                                        E->nexttar->prevtar = E->prevtar;
                                        E->prevtar->nexttar = E->nexttar;
                                        Globals::Transitions[i]->fairabled--;
                                    }
                                }
                                if (analyse_fairness(S, level + 1)) {
                                    return true;
                                }
                                goto aftercheck;
                            }
                        }
                        Globals::Transitions[i]->faired = Globals::Transitions[i]->fairabled = 0;
                    }
                    return true; // arrived here only if all transitions have paased fairness test.

aftercheck:
                    for (; !(S->expired); S = S->nexttar) {
                        S->expired = true;
                    }
                    C->parent = O;
                    C->ddfs = oldd;
                    C->mmin = oldm;
                    C->current = oldc;

                }
                // end process scc
                // C) return to previous state
                N = C;
                C = C->parent;
                if (C) {
                    C->mmin = MIN(C->mmin, N->mmin);
                    C->current++;
                }
            } // end for all new scc
        } // end for all states
    } // end, for all nodes in pool
    return false;
} // end analyse_fairness
#endif





/*!
 \brief the depth first search

 \return 1 if we found what we were looking for; that is, the search is aborted
         Examples: deadlock found, state found, unbounded net found, etc.

 \return 0 if we did not found what we were looking for; that is, complete search
         Examples: no deadlock found, no dead transition found, etc.
*/
unsigned int depth_first() {
#ifdef DEPTH_FIRST
    ostream* graphstream = NULL;
    unsigned int i;
    State* NewState;
#ifdef CYCLE
    bool IsCyclic;
    unsigned int silentpath; // nr of consecutive non-stored states
    Transition** fl;
#endif
    // init initial marking and hash table
    isbounded = 1;
#ifndef CYCLE
    // organize output file for -m/-M/-g/-G parameters
    if (Globals::gmflg) {
        graphstream = new ofstream(Globals::graphfile);
        if (graphstream->fail()) {
            abort(4, "cannot open graph output file '%s' - no output written", _cfilename_(Globals::graphfile));
        }
    }
    if (Globals::GMflg) {
        graphstream = &std::cout;
    }
#else
    silentpath = 0;
#endif
#ifndef DISTRIBUTE
#if defined(SYMMETRY) && SYMMINTEGRATION==1
    Trace = new SearchTrace [Globals::Places[0]->cnt];
#endif
    // initialize hash table
    for (i = 0; i < HASHSIZE; ++i) {
#ifdef BITHASH
        BitHashTable[i] = 0;
#else
        binHashTable[i] = NULL;
#endif
    }
#endif
#ifdef WITHFORMULA
#ifndef TWOPHASE
    int res;

    F = F->reduce(&res);
    if (res < 2) {
        return res;
    }
    F = F->posate();
    F->tempcard = 0;
    F->setstatic();
    if (F->tempcard) {
        abort(3, "temporal operators are not allowed in state predicates (%s)", _cparameter_("WITHFORMULA"));
    }
    //cout << "\n Formula with\n" << F->card << " subformula(s).\n";
    message("Formula with %d subformula(s) (%s)", F->card, _cparameter_("WITHFORMULA"));
    F->parent = NULL;
#endif
#endif
#ifdef DISTRIBUTE
    Reason WhyTerminated;
    NrOfStates = 0;
    Edges = 0;

    while (get_new_vector(Globals::CurrentMarking, WhyTerminated)) {
        // don't worry, this loop ends only after the end of the actual dfs loop

        CurrentState = new State;
        for (i = 0; i < Globals::Transitions[0]->cnt; ++i) {
            Globals::Transitions[i]->check_enabled();
        }
        CurrentState->firelist = FIRELIST();
        ++NrOfStates;
#ifdef MAXIMALSTATES
        checkMaximalStates(NrOfStates); ///// LINE ADDED BY NIELS
#endif
#else
    if (SEARCHPROC()) {
        cerr << "Sollte eigentlich nicht vorkommen";
    }

    NrOfStates = 1;
    Edges = 0;
#ifdef CYCLE
    // check if state enables cycling transition
    fl = FIRELIST();
#ifdef NONBRANCHINGONLY
    if (fl && fl[0] && (fl[1] || fl[0]->cyclic)) {
        IsCyclic = true;
    } else {
        IsCyclic = false;
    }
#else
    if (fl) {
        for (i = 0, IsCyclic = false; fl[i]; ++i) {
            if (fl[i]->cyclic) {
                IsCyclic = true;
                break;
            }
        }
    } else {
        IsCyclic = false;
    }
#endif
    if (IsCyclic) {
        CurrentState = INSERTPROC();
        CurrentState->firelist = fl;
    } else {
        CurrentState = new State();
        CurrentState->firelist = fl;
        NrOfStates = 0;
    }
#else

    CurrentState = INSERTPROC();
    CurrentState->firelist = FIRELIST();
#endif
#endif
#ifdef COVER
        Ancestor = new unsigned int[Globals::Places[0]->cnt + 1];
#endif
        CurrentState->current = 0;
        CurrentState->parent = NULL;
#if defined(DEADLOCK) && !defined(DISTRIBUTE)
        if (!(CurrentState->firelist) || !(CurrentState->firelist[0])) {
            // early abortion
            //cout << "\ndead state found!\n";
            message(_cgood_("dead state found!"));

            if (Globals::resultfile) {
                fprintf(Globals::resultfile, "deadlock: {\n  result = true;\n  ");
            }

            printstate("", Globals::CurrentMarking);
            print_path(CurrentState);

            if (Globals::resultfile) {
                fprintf(Globals::resultfile, "};\n");
                fprintf(Globals::resultfile, "statespace: {\n  complete = false;\n  states = ( ");
            }

            printincompletestates(CurrentState, graphstream);

            if (Globals::resultfile) {
                fprintf(Globals::resultfile, "\n  );\n};\n");
            }

            statistics(NrOfStates, Edges, NonEmptyHash);

            return 1;
        }
#endif
#ifdef TSCC
        TSCCRepresentitives = NULL;
#endif
#ifdef DEADTRANSITION
#ifndef DISTRIBUTE
        if (Globals::CheckTransition->enabled) {
            // early abortion
            //cout << "\ntransition " << Globals::CheckTransition->name << " is not dead!\n";
            message("%s %s %s", _cgood_("transition"), _cgood_(Globals::CheckTransition->name), _cgood_("is not dead!"));
            printstate("", Globals::CurrentMarking);
            print_path(CurrentState);
            printincompletestates(CurrentState, graphstream);
            statistics(NrOfStates, Edges, NonEmptyHash);
            return 1;
        }
#endif
#endif
#if defined ( STUBBORN ) && defined ( REACHABILITY ) && ! defined(DISTRIBUTE)
        if (!CurrentState->firelist) {
            // early abortion
            //cout << "\nstate found!\n";
            message(_cgood_("state found!"));
            printstate("", Globals::CurrentMarking);
            print_path(CurrentState);
            printincompletestates(CurrentState, graphstream);
            statistics(NrOfStates, Edges, NonEmptyHash);
            return 1;
        }
#endif
#if defined (REACHABILITY ) && ! defined ( STUBBORN ) && ! defined(DISTRIBUTE)
        for (i = 0; i < Globals::Places[0]->cnt; ++i) {
            if (Globals::CurrentMarking[i] != Globals::Places[i]->target_marking) {
                break;
            }
        }
        if (i >= Globals::Places[0]->cnt) { // target_marking found!
            // early abortion
            //cout << "\nstate found!\n";
            message(_cgood_("state found!"));
            printstate("", Globals::CurrentMarking);
            print_path(CurrentState);
            printincompletestates(CurrentState, graphstream);
            statistics(NrOfStates, Edges, NonEmptyHash);
            return 1;
        }
#endif
#ifdef COVER
        CurrentState->NewOmega = NULL;
#endif
#if defined(FAIRPROP) || defined(STABLEPROP)
        F->initatomic();
#endif

#ifdef EVENTUALLYPROP
        if (F->initatomic()) {
            //cout << "\neventually phi holds.\n";
            message("eventually phi holds (%s)", _cparameter_("EVENTUALLYPROP"));
            statistics(NrOfStates, Edges, NonEmptyHash);
        }
#endif
#ifdef STATEPREDICATE
        if (F->initatomic()) {
            if (Globals::resultfile) {
                fprintf(Globals::resultfile, "statepredicate: {\n  result = true;\n  ");
            }
#if defined(LIVEPROP) && ! defined(TWOPHASE)
            largest_sat = 1;
#else
            //cout << "\nstate found!\n";
            message(_cgood_("state found!"));

            printstate("", Globals::CurrentMarking);
            print_path(CurrentState);
            printincompletestates(CurrentState, graphstream);

            if (Globals::resultfile) {
                fprintf(Globals::resultfile, "};\n");
            }

            statistics(NrOfStates, Edges, NonEmptyHash);
#ifdef DISTRIBUTE
            heureka(resultfixedR, Globals::CurrentMarking);
            end_communication();
#endif
            return 1;
#endif
        }
#endif
#ifdef TARJAN
        CurrentState->succ = new State*[CardFireList + 1];
        CurrentState->dfs = CurrentState->min = 0;
#ifdef FULLTARJAN
#ifndef STATESPACE
        CurrentState->phi = F->value;
#endif
        CurrentState->nexttar = CurrentState->prevtar = CurrentState;
        TarStack = CurrentState;
#endif
#endif

        // process marking until returning from initial state
        while (CurrentState) {
#ifdef EXTENDED
            // ddfsnum must be passed to net.h for tracing lastdisabed and lastfired
            currentdfsnum = CurrentState->dfs + 1; // 0 reserved for "never disabled"
            // and "never fired"
#endif
            if (CurrentState->firelist[CurrentState->current]) {
                // there is a next state that needs to be explored
                ++Edges;
                if (!(Edges % REPORTFREQUENCY)) {
#ifdef DISTRIBUTE
                    rapport(rapportstring);
#else
                    report_statistics();
#endif
                }
                CurrentState->firelist[CurrentState->current]->fire();
#ifdef DISTRIBUTE
                // In the distributed context, we we to check for target states right now,
                // since a call to SEARCHPROC could mean that the state is shipped somewhere
                // else, waiting there a long time before being touched. Thus, search would
                // proceed much longer than necessary. Outside the distributed context, it is
                // however wise to postpone the check for target states, because
                // 1. The check can be skipped when the state is not new
                // 2. Information gathered during fire list generation can be used for
                // checking the property efficiently.
#ifdef DEADLOCK
                if (!Transitions[0]->NrEnabled) {
                    cout << "heureka" << endl;  //ToDo?! ;-)
                    heureka(resultfixedR, Globals::CurrentMarking);
                    end_communication();
                    return 1;
                }
#endif
#ifdef DEADTRANSITION
                if (Globals::CheckTransition->enabled) {
                    heureka(resultfixedR, Globals::CurrentMarking);
                    end_communication();
                    return 1;
                }
#endif
#ifdef REACHABILITY
                for (i = 0; i < Globals::Places[0]->cnt; ++i) {
                    if (Globals::CurrentMarking[i] != Globals::Places[i]->target_marking) {
                        break;
                    }
                }
                if (i < Globals::Places[0]->cnt) {
                    heureka(resultfixedR, Globals::CurrentMarking);
                    end_communication();
                    return 1;
                }
#endif
#ifdef WITHFORMULA
#ifndef TWOPHASE
                update_formula(CurrentState->firelist[CurrentState->current]);
#endif
#endif
#ifdef STATEPREDICATE
                if (F->value) {
                    heureka(resultfixedR, Globals::CurrentMarking);
                    end_communication();
                    return 1;
                }
#endif
#endif
#ifdef COVER
                //In coverability graphs, we need to check for new w
                // 1. Search backwards until last w-Intro for smaller state
                unsigned int NrCovered;
                State* smallerstate;
                Place** NewOmegas;

                NewOmegas = NULL;
                // for all ancestor states do ...
                for (i = 0; i < Globals::Places[0]->cnt; ++i) {
                    Ancestor[i] = Globals::CurrentMarking[i];
                }
                for (smallerstate = CurrentState; smallerstate; smallerstate = smallerstate->parent) {
                    smallerstate->firelist[smallerstate->current]->traceback();
                    NrCovered = 0;
                    for (i = 0; i < Globals::Places[0]->cnt; ++i) {
                        // case 1: smaller state[i] > current state [i]
                        // --->continue with previous state
                        if (Ancestor[i] > Globals::CurrentMarking[i]) {
                            goto nextstate;
                        }

                        // case 2: smaller state < current state
                        // count w-Intro
                        if (Ancestor[i] < Globals::CurrentMarking[i]) {
                            NrCovered++;
                        }
                        // case 3: smaller state = current state -->do nothing
                    }

                    // if arrived here, it holds smaller <= current
                    // covering is proper iff NrCovered > 0
                    // If covering is not proper, (smaller state = current state)
                    // current marking is not new, ancestors of smaller marking cannot
                    // be smaller than current marking, since they would be smaller than
                    // this smaller marking -->leave w-Intro procedure
                    if (!NrCovered) {
                        smallerstate = NULL;
                        goto endomegaproc;
                    }

                    // Here, smallerstate IS less than current state.
                    isbounded = 0;
                    NewOmegas = new Place*[NrCovered + 1];

                    // for all fragements of state vector do ...
                    NrCovered = 0;
                    for (i = 0; i < Globals::Places[0]->cnt; ++i) {
                        if (Ancestor[i] < Globals::CurrentMarking[i]) {
                            // Here we have a place that deserves a new Omega
                            // 1. set old value in place record
                            Globals::Places[i]->lastfinite = Globals::CurrentMarking[i];
                            Globals::Places[i]->set_cmarking(VERYLARGE);
                            Globals::Places[i]->bounded = false;
                            NewOmegas[NrCovered++] = Globals::Places[i];
                        }
                    }
                    NewOmegas[NrCovered] = NULL;
                    goto endomegaproc;
                    if (smallerstate->smaller) { // smallerstate is a omega-introducing state
                        break;
                    }

nextstate:
                    ;
                }

endomegaproc:
                if (!NewOmegas) {
                    smallerstate = NULL;
                }
#endif
#ifdef DISTRIBUTE
#if defined(SYMMETRY) && (SYMMINTEGRATION == 3)
                canonize();
                if (!search_and_insert(kanrep))
#else
                if (!search_and_insert(Globals::CurrentMarking))
#endif
#else
                if ((NewState = SEARCHPROC()))
#endif
                {
                    // State exists! (or, at least, I am not responsible for it (in the moment))
#ifdef COVER
                    if (NewOmegas) {
                        // Replace new omegas by their old values
                        for (i = 0; NewOmegas[i]; ++i) {
                            NewOmegas[i]->set_cmarking(NewOmegas[i]->lastfinite);
                            NewOmegas[i]->bounded = true;
                        }
                        delete[] NewOmegas;
                    }
#endif
                    CurrentState->firelist[CurrentState->current]->backfire();
#ifdef WITHFORMULA
#ifndef TWOPHASE
                    update_formula(CurrentState->firelist[CurrentState->current]);
#endif
#endif
#ifdef CYCRED
// closing in {0,1} -->on stack ( 0 = has successor on stack and has not been extended by spp2)
// closing = 2 -->no longer on stack
                    if (NewState->closing <= 1) {
                        CurrentState->closing = 0;
                    }
#endif
#if defined(TARJAN) && !defined(DISTRIBUTE)
                    CurrentState->succ[CurrentState->current] = NewState;
#ifdef FULLTARJAN
                    if (!(NewState->tarlevel))
#endif
                        CurrentState->min = MIN(CurrentState->min, NewState->min);
#endif
                    ++(CurrentState->current);
                } else {
#if defined(WITHFORMULA) && ! defined(DISTRIBUTE)
#ifndef TWOPHASE
                    update_formula(CurrentState->firelist[CurrentState->current]);
#endif
#endif
#ifdef CYCLE
                    fl = FIRELIST();
#ifdef NONBRANCHINGONLY
                    if (fl && fl[0] && (fl[1] || fl[0]->cyclic)) {
                        IsCyclic = true;
                    } else {
                        IsCyclic = false;
                    }
#else
                    IsCyclic = false;
                    if (fl) {
                        for (i = 0; fl[i]; ++i) {
                            if (fl[i]->cyclic) {
                                IsCyclic = true;
                                silentpath = 0;
                                break;
                            }
                        }
                    } else {
                        IsCyclic = false;
                    }

                    if (silentpath >= MAXUNSAVED) {
                        silentpath = 0;
                        IsCyclic = true;
                    }
#endif
                    if (IsCyclic) {
                        NewState = INSERTPROC();
                        NewState->firelist = fl;
#ifdef TARJAN
                        NewState->dfs = NewState->min = NrOfStates++;
#ifdef MAXIMALSTATES
                        checkMaximalStates(NrOfStates); ///// LINE ADDED BY NIELS
#endif
#else
                        ++NrOfStates;
#ifdef MAXIMALSTATES
                        checkMaximalStates(NrOfStates); ///// LINE ADDED BY NIELS
#endif
#endif
                    } else {
                        NewState = new State();
                        silentpath ++;
                        NewState->firelist = fl;
                    }
#else
#ifdef DISTRIBUTE
                    NewState = new State();
#else
                    NewState = INSERTPROC();
#endif
#ifdef CYCRED
                    NewState->closing = 1;
#endif
                    NewState->firelist = FIRELIST();
#ifdef TARJAN
                    NewState->dfs = NewState->min = NrOfStates++;
#ifdef MAXIMALSTATES
                    checkMaximalStates(NrOfStates); ///// LINE ADDED BY NIELS
#endif
#ifdef FULLTARJAN
#ifndef STATESPACE
                    NewState->phi = F->value;
#endif
#ifdef EVENTUALLYPROP
                    if (!F->value) {
#endif
                        NewState->prevtar = TarStack;
                        NewState->nexttar = TarStack->nexttar;
                        TarStack = TarStack->nexttar = NewState;
                        NewState->nexttar->prevtar = NewState;
#ifdef EVENTUALLYPROP
                    }
#endif
#endif
#else
                    ++NrOfStates;
#ifdef MAXIMALSTATES
                    checkMaximalStates(NrOfStates); ///// LINE ADDED BY NIELS
#endif
#endif
#endif
                    NewState->current = 0;
                    NewState->parent = CurrentState;
#ifdef TARJAN
                    NewState->succ =  new State * [CardFireList + 1];
                    CurrentState->succ[CurrentState->current] = NewState;
#endif
#ifdef EVENTUALLYPROP
                    // need to backtrack if phi is satisfied
                    if (F->value) {
                        CurrentState->firelist[CurrentState->current]->backfire();
                        update_formula(CurrentState->firelist[CurrentState->current]);
                        ++(CurrentState->current);
                        continue;
                    }
#endif
#ifndef DISTRIBUTE
#ifdef STATEPREDICATE
                    if (F->value) {
#if defined(LIVEPROP) && !defined(TWOPHASE)
                        if (largest_sat < NewState->dfs + 1) {
                            largest_sat = NewState->dfs + 1;
                        }
#else
                        if (Globals::resultfile) {
                            fprintf(Globals::resultfile, "statepredicate: {\n  result = true;\n  ");
                        }

                        // early abortion
                        //cout << "\nstate found!\n";
                        message(_cgood_("state found!"));
                        printstate("", Globals::CurrentMarking);
                        print_path(NewState);
                        printincompletestates(NewState, graphstream);

                        if (Globals::resultfile) {
                            fprintf(Globals::resultfile, "};\n");
                        }


                        statistics(NrOfStates, Edges, NonEmptyHash);
                        return 1;
#endif
                    }
#endif
#ifdef COVER
                    NewState->smaller = smallerstate;
                    NewState->NewOmega = NewOmegas;
#endif
#ifdef DEADLOCK
                    if (!(NewState->firelist) || !(NewState->firelist[0])) {
                        // early abortion
                        //cout << "\ndead state found!\n";
                        message(_cgood_("dead state found!"));
                        if (Globals::resultfile) {
                            fprintf(Globals::resultfile, "deadlock: {\n  result = true;\n");
                        }

                        printstate("", Globals::CurrentMarking);
                        print_path(NewState);

                        if (Globals::resultfile) {
                            fprintf(Globals::resultfile, "};\n");
                            fprintf(Globals::resultfile, "statespace: {\n  complete = false;\n  states = ( ");
                        }

                        printincompletestates(NewState, graphstream);

                        if (Globals::resultfile) {
                            fprintf(Globals::resultfile, "\n  );\n};\n");
                        }

                        statistics(NrOfStates, Edges, NonEmptyHash);

                        return 1;
                    }
#endif
#ifdef DEADTRANSITION
                    if (Globals::CheckTransition->enabled) {
                        // early abortion
                        //cout << "\ntransition " <<  Globals::CheckTransition->name << " is not dead!\n";
                        message("%s %s %s", _cgood_("transition"), _cgood_(Globals::CheckTransition->name), _cgood_("is not dead!"));
                        printstate("", Globals::CurrentMarking);
                        print_path(NewState);
                        printincompletestates(NewState, graphstream);
                        statistics(NrOfStates, Edges, NonEmptyHash);
                        return 1;
                    }
#endif
#if ( defined ( STUBBORN ) && defined ( REACHABILITY ) )
                    if (!NewState->firelist) {
                        // early abortion
                        //cout << "\nstate found!\n";
                        message(_cgood_("state found!"));
                        printstate("", Globals::CurrentMarking);
                        print_path(NewState);
                        printincompletestates(NewState, graphstream);
                        statistics(NrOfStates, Edges, NonEmptyHash);
                        return 1;
                    }
#endif
#if ( defined (REACHABILITY ) && ! defined ( STUBBORN ) )
                    for (i = 0; i < Globals::Places[0]->cnt; ++i) {
                        if (Globals::CurrentMarking[i] != Globals::Places[i]->target_marking) {
                            break;
                        }
                    }
                    // target_marking found!
                    if (i >= Globals::Places[0]->cnt) {
                        // early abortion
                        //cout << "\nstate found!\n";
                        message(_cgood_("state found!"));
                        printstate("", Globals::CurrentMarking);
                        print_path(NewState);
                        printincompletestates(NewState, graphstream);
                        statistics(NrOfStates, Edges, NonEmptyHash);
                        return 1;
                    }
#endif
#endif
                    CurrentState = NewState;
#ifdef BOUNDEDNET
                    if (!isbounded) {
                        //cout << "net is unbounded!\n";
                        message(_cgood_("net is unbounded!"));

                        if (Globals::resultfile) {
                            fprintf(Globals::resultfile, "unbounded: {\n  result = true;\n");
                        }

                        printstate("", Globals::CurrentMarking);
                        print_reg_path(CurrentState, CurrentState->smaller);
                        cout << "\n";

                        if (Globals::resultfile) {
                            fprintf(Globals::resultfile, "};\n");
                            fprintf(Globals::resultfile, "statespace: {\n  complete = false;\n  states = ( ");
                        }

                        printincompletestates(CurrentState, graphstream);

                        if (Globals::resultfile) {
                            fprintf(Globals::resultfile, " );\n};\n");
                        }

                        statistics(NrOfStates, Edges, NonEmptyHash);
                        return 1;
                    }
#endif
#ifdef BOUNDEDPLACE
                    if (!Globals::CheckPlace->bounded) {
                        //cout << "place " << Globals::CheckPlace->name << " is unbounded!\n";
                        message("%s %s %s", _cgood_("place"), _cgood_(Globals::CheckPlace->name), _cgood_("is unbounded!"));
                        printstate("", Globals::CurrentMarking);
                        print_reg_path(CurrentState, CurrentState->smaller);
                        cout << "\n";
                        printincompletestates(CurrentState, graphstream);
                        statistics(NrOfStates, Edges, NonEmptyHash);
                        return 1;
                    }
#endif
                }
            } else {
                // close state and return to previous state
#if defined(FAIRPROP) || defined(EVENTUALLYPROP) || defined(STABLEPROP)
                if (CurrentState->dfs == CurrentState->min) {
                    // unlink scc and check it for counterexample sc sets
                    if (CurrentState != TarStack->nexttar) { // current != bottom(stack)
                        State* newroot;
                        newroot = CurrentState->prevtar;
                        newroot->nexttar = TarStack->nexttar;
                        TarStack->nexttar->prevtar = newroot;
                        TarStack->nexttar = CurrentState;
                        CurrentState->prevtar = TarStack;
                        TarStack = newroot;
                    }
                    // remove all phi-states
                    State* s, *start;
#ifdef FAIRPROP
                    for (s = CurrentState; s->phi && (s->nexttar != s); s = s->nexttar) {
                        s->nexttar->prevtar = s->prevtar;
                        s->prevtar->nexttar = s->nexttar;
                    }
                    if (!(s->phi)) { // only nonempty sets need to be checked
                        start = s;
#else
                    start = CurrentState;
#endif
                        for (s = start->nexttar; s != start; s = s->nexttar) {
#ifdef FAIRPROP
                            if (s->phi) {
                                s->prevtar->nexttar = s->nexttar;
                                s->nexttar->prevtar = s->prevtar;
                            } else {
#endif
                                s->tarlevel = 1;
#ifdef FAIRPROP
                            }
#endif
                        }
                        start->tarlevel = 1;
                        // analyze this
                        State* oldpar;
                        unsigned int oldc;
                        oldpar = CurrentState->parent;
                        oldc = CurrentState->current;
                        if (analyse_fairness(start, 1)) {
#ifdef EVENTUALLYPROP
                            //cout << "eventually phi does not hold" << endl;
                            message(_cgood_("eventually phi does not hold"));
#endif
#ifdef FAIRPROP
                            //cout << "GF phi does not hold" << endl;
                            message(_cgood_("GF phi does not hold"));
#endif
#ifdef STABLEPROP
                            //cout << "FG phi does not hold" << endl;
                            message(_cgood_("FG phi does not hold"));
#endif
                            return 1;
                        }
                        CurrentState->parent = oldpar;
                        CurrentState->current = oldc;

#ifdef FAIRPROP
                    }
#endif
                }

#endif
#ifdef CYCRED
                if (CurrentState->closing == 0) {
                    // cycle closed. Check for ignored transitions or incomplete up-sets
#if defined(STATEPREDICATE) && defined(RELAXED) && !defined(TWOPHASE)
                    // implement SPP2 of Kristensen/Valmari (2000)
                    Transition** forgotten;

                    forgotten = F->spp2(CurrentState);
                    if (forgotten) {
                        // fire list must be extended
                        unsigned int nf;
                        for (nf = 0; forgotten[nf]; nf++) {
                            ;
                        }
                        Transition** newFL = new Transition * [nf + CurrentState->current + 1];
                        State** newSucc = new State * [nf + CurrentState->current];
                        for (i = 0; i < CurrentState->current; ++i) {
                            newFL[i] = CurrentState->firelist[i];
                            newSucc[i] = CurrentState->succ[i];
                        }
                        for (i = 0; i < nf; ++i) {
                            newFL[CurrentState->current + i] = forgotten[i];
                        }
                        newFL[CurrentState->current + i] = NULL;
                        // delete [] CurrentState->firelist;
                        // delete [] CurrentState->succ;
                        CurrentState->firelist = newFL;
                        CurrentState->succ = newSucc;
                        CurrentState->closing = 1;
                        continue;
                    }
#endif
                }
                CurrentState->closing = 2;
#endif
#ifndef CYCLE
#ifdef TARJAN
                if (Globals::gmflg || Globals::GMflg) {
                    (*graphstream) << "STATE " << CurrentState->dfs;
                    (*graphstream) << " Prog: " << CurrentState->progress_value;

                    if (CurrentState->persistent) {
                        (*graphstream) << " persistent ";
                    }
                    if (Globals::graphformat == 'm') {
                        int j = 0;
                        for (i = 0; i < Globals::Places[0]->cnt; ++i) {
                            if (Globals::CurrentMarking[i]) {
                                if (Globals::CurrentMarking[i] == VERYLARGE) {
                                    (*graphstream) << (j++ ? ",\n" : "\n") << Globals::Places[i]->name << " : " << "oo" ;
                                } else {
                                    (*graphstream) << (j++ ? ",\n" : "\n") << Globals::Places[i]->name << " : " << Globals::CurrentMarking[i];
                                }
                            }
                        }
                    }
                    (*graphstream) << "\n\n";
                    for (i = 0; CurrentState->firelist[i]; ++i) {
                        (*graphstream) << CurrentState->firelist[i]->name << " -> " << CurrentState->succ[i]->dfs << "\n";
                    }
                    (*graphstream) << endl;
                }

#ifndef BOUNDEDNET // there is still a bug...
                /*
                      if (Globals::resultfile) {
                        static bool first = true;
                        if (first) {
                          first = false;
                          fprintf(Globals::resultfile, "statespace: {\n  complete = false; // we cannot be sure\n  states = (\n");
                        }

                        static bool comma = false;
                        if (comma) {
                          fprintf(Globals::resultfile, ",\n");
                        } else {
                          comma = true;
                        }
                        fprintf(Globals::resultfile, "    { id = %d;\n", CurrentState->dfs);
                        fprintf(Globals::resultfile, "      progress = %d;\n", CurrentState->progress_value);
                        if(CurrentState->persistent) {
                          fprintf(Globals::resultfile, "      persistent = true;\n");
                        }
                        fprintf(Globals::resultfile, "      state = (");

                        int j = 0;
                        for(i=0;i<Globals::Places[0]->cnt; ++i) {
                          if(Globals::CurrentMarking[i]) {
                            if(Globals::CurrentMarking[i] == VERYLARGE) {
                              fprintf(Globals::resultfile, "%s(\"%s\", -1)", (j++ ? ", " : ""), Globals::Places[i]->name);
                            }
                            else {
                              fprintf(Globals::resultfile, "%s(\"%s\", %d)", (j++ ? ", " : ""), Globals::Places[i]->name, Globals::CurrentMarking[i]);
                            }
                          }
                        }
                        fprintf(Globals::resultfile, ");\n");
                        fprintf(Globals::resultfile, "      successors = (");

                        j = 0;
                        for(i=0; CurrentState->firelist[i]; ++i) {
                          fprintf(Globals::resultfile, "%s(\"%s\", %d)", (j++ ? ", " : ""), CurrentState->firelist[i]->name, CurrentState->succ[i]->dfs);
                        }
                        fprintf(Globals::resultfile, "); }");
                      }
                */
#endif

#endif
#endif
#if defined(EXTENDED) && ! defined(MODELCHECKING) && !defined(CYCRED)
                // if last state in TSCC, retrieve ignored transitions
                if ((CurrentState->dfs == CurrentState->min)
                        && (CurrentState->dfs >= MinBookmark)) {
                    // TSCC closed. Check for ignored transitions or incomplete up-sets
#if defined(STATEPREDICATE) && defined(RELAXED) && ! defined(CYCRED) && !defined(TWOPHASE)
                    // implement SPP2 of Kristensen/Valmari (2000)
                    Transition** forgotten;

                    forgotten = F->spp2(CurrentState);
#if defined(LIVEPROP)
                    if (!forgotten) {
                        // check for forgotten down transitions (SPP3 of
                        // Kristensen/Valmari (2000)
                        for (i = 0; i < Globals::Transitions[0]->cnt; ++i) {
                            if (Globals::Transitions[i]->down) {
                                if (Globals::Transitions[i]->lastfired <= CurrentState->dfs) {
                                    // no occurrence of down transition
                                    // inside tscc->need to extend
                                    stubborninsert(Globals::Transitions[i]);
                                    stubbornclosure();
                                    // check for unfired transitions in
                                    // stubborn set

                                    unsigned int m, n;
                                    Transition* st;

                                    for (st = Globals::Transitions[0]->StartOfStubbornList; st; st = st->NextStubborn) {
                                        if (st->enabled) {
                                            for (m = 0; CurrentState->firelist[m]; ++m) {
                                                if (st == CurrentState->firelist[m]) {
                                                    break;
                                                }
                                            }
                                            if (CurrentState->firelist[m]) {
                                                // found unfired transition in stubborn set
                                                // around down transition->can stop
                                                // searching and extend firing list by
                                                // unfired transitions in this stubborn set

                                                forgotten = new Transition * [Globals::Transitions[0]->NrStubborn + 1];
                                                for (n = 0; st; st = st->NextStubborn) {
                                                    st->instubborn = false;
                                                    if (st->enabled) {
                                                        Globals::Transitions[0]->NrStubborn--;
                                                        for (m = 0; CurrentState->firelist[m]; ++m) {
                                                            if (CurrentState->firelist[m] == st) {
                                                                break;
                                                            }
                                                        }
                                                        if (CurrentState->firelist[m]) {
                                                            forgotten[n++] = st;
                                                        }
                                                    }
                                                }
                                                Globals::Transitions[0]->StartOfStubbornList = NULL;
                                                goto afterdownsearch;
                                            }
                                        }
                                        Globals::Transitions[0]->StartOfStubbornList = st->NextStubborn;
                                        st->instubborn = false;
                                        if (Globals::Transitions[i]->enabled) {
                                            Globals::Transitions[0]->NrStubborn--;
                                        }
                                    }
                                }
                            }
                        }
                        forgotten = NULL;
                    }
afterdownsearch:

#endif
                    if (forgotten) {
                        // fire list must be extended
                        unsigned int nf;
                        for (nf = 0; forgotten[nf]; ++nf) {
                            ;
                        }
                        Transition** newFL = new Transition * [nf + CurrentState->current + 1];
                        State** newSucc = new State * [nf + CurrentState->current];
                        for (i = 0; i < CurrentState->current; ++i) {
                            newFL[i] = CurrentState->firelist[i];
                            newSucc[i] = CurrentState->succ[i];
                        }
                        for (i = 0; i < nf; ++i) {
                            newFL[CurrentState->current + i] = forgotten[i];
                        }
                        newFL[CurrentState->current + i] = NULL;
                        delete [] CurrentState->firelist;
                        delete [] CurrentState->succ;
                        CurrentState->firelist = newFL;
                        CurrentState->succ = newSucc;
                        continue;
                    } else {
                        //TSCC really closed
                        MinBookmark = NrOfStates;
#if defined(LIVEPROP) && ! defined(TWOPHASE)
                        // check if tscc reached property
                        if (largest_sat <= CurrentState->dfs) {
                            // tscc did not reach prop->prop not live
                            //cout << "\npredicate not live: not satisfiable beyond reported state\n\n";
                            message(_cgood_("predicate not live: not satisfiable beyond reported state"));
                            printstate("", Globals::CurrentMarking);
                            statistics(NrOfStates, Edges, NonEmptyHash);
                            return 1;
                        }
#endif
                    }

#else
                    Transition* ignored;
                    unsigned int CardIgnored;

                    CardIgnored = 0;
                    Globals::Transitions[0]->StartOfIgnoredList = NULL;
                    for (ignored = Globals::Transitions[0]->StartOfEnabledList; ignored;
                            ignored = ignored->NextEnabled) {

                        if ((ignored->lastdisabled <= CurrentState->dfs)
                                && (ignored->lastfired <= CurrentState->dfs)) {

                            // transition IS ignored
                            ++CardIgnored;
                            ignored->NextIgnored = Globals::Transitions[0]->StartOfIgnoredList;
                            Globals::Transitions[0]->StartOfIgnoredList = ignored;
                        }
                    }
                    if (Globals::Transitions[0]->StartOfIgnoredList) {
                        // there are ignored transitions
                        Transition* tt;
                        Transition** newFL = new Transition *[CurrentState->current + CardIgnored + 1];
                        State** newSucc = new State *[CurrentState->current + CardIgnored];
                        int u;
                        for (u = 0; u < CurrentState->current; ++u) {
                            newFL[u] = CurrentState->firelist[u];
                            newSucc[u] = CurrentState->succ[u];
                        }
                        for (tt = Globals::Transitions[0]->StartOfIgnoredList; tt; tt = tt->NextIgnored) {
                            newFL[u++] = tt;
                        }
                        newFL[u] = NULL;
                        delete [] CurrentState->firelist;
                        delete [] CurrentState->succ;
                        CurrentState->firelist = newFL;
                        CurrentState->succ = newSucc;
                        continue;
                    }
#ifdef TSCC
                    else {
                        //TSCC really closed: deposit state
                        StatevectorList* s;

                        MinBookmark = NrOfStates;
                        s = new StatevectorList;
                        s->sv = new Statevector(Globals::Places[0]->cnt);
                        s->next = TSCCRepresentitives;
                        TSCCRepresentitives = s;
                        for (i = 0; i < Globals::Places[0]->cnt; ++i) {
                            s->sv->set(i, Globals::CurrentMarking[i]);
                        }
                    }
#endif
#endif
                }
#endif
#ifdef COVER
                if (CurrentState->NewOmega) {
                    // Replace new omegas by their old values
                    for (i = 0; CurrentState->NewOmega[i]; ++i) {
                        CurrentState->NewOmega[i]->set_cmarking(CurrentState->NewOmega[i]->lastfinite);
                        CurrentState->NewOmega[i]->bounded = true;
                    }
                    delete [] CurrentState->NewOmega;
                }
#endif
#ifdef CYCLE
                silentpath = 0;
#else
#ifdef TARJAN
                if (CurrentState->parent) {
                    CurrentState->parent->min = MIN(CurrentState->min, CurrentState->parent->min);
                }
#endif
#endif

#ifdef CYCRED
                CurrentState->closing = 2;
#endif
                CurrentState = CurrentState->parent;
//**     delete TmpState;
                if (CurrentState) {
                    CurrentState->firelist[CurrentState->current]->backfire();
#ifdef WITHFORMULA
#ifndef TWOPHASE
                    update_formula(CurrentState->firelist[CurrentState->current]);
#endif
#endif
                    ++(CurrentState->current);
                }
            }
        }
#ifdef DISTRIBUTE
    } //finally, we close that while loop around the dfs search

#else
#ifdef BITHASH
        //cout << "\nno conclusive result!\n";
        message(_cbad_("no conclusive result!"));
#else
#ifdef REACHABILITY
        //cout << "\nstate is not reachable!\n";
        message(_cbad_("state is not reachable!"));
#endif
#ifdef DEADLOCK
        //cout << "\nnet does not have deadlocks!\n";
        message(_cbad_("net does not have deadlocks!"));
        if (Globals::resultfile) {
            fprintf(Globals::resultfile, "\n  );\n};\ndeadlock: {\n  result = false;\n};\n");
        }
#endif
#if defined(STATEPREDICATE) && ! defined(LIVEPROP)
        //cout << "\n predicate is not satisfiable!\n";
        message(_cbad_("predicate is not satisfiable!"));
        if (Globals::resultfile) {
            fprintf(Globals::resultfile, "statepredicate: {\n  result = false;\n};\n");
        }

#endif
#ifdef DEADTRANSITION
        //cout << "\ntransition " << Globals::CheckTransition->name << " is dead!\n";
        message("%s %s %s", _cbad_("transition"), _cbad_(Globals::CheckTransition->name), _cbad_("is dead!"));
#endif
#ifdef BOUNDEDPLACE
        //cout << "\nplace " << Globals::CheckPlace->name << " is bounded!\n";
        message("%s %s %s", _cbad_("place"), _cbad_(Globals::CheckPlace->name), _cbad_("is bounded!"));
#endif
#if defined(LIVEPROP) && ! defined(TWOPHASE)
        //cout << "\npredicate is live!\n";
        message(_cbad_("predicate is live!"));
#endif
#ifdef BOUNDEDNET
        if (isbounded) {
            //cout << "\nnet is bounded!\n";
            message(_cbad_("net is bounded!"));
            if (Globals::resultfile) {
                fprintf(Globals::resultfile, "unbounded: {\n  result = false;\n};\n");
            }
        } else {
            /// can this ever happen here?
            /// mmmh, das läßt sich herausfinden...
            assert(isbounded == true);
            cout << "\nnet is unbounded!\n";
        }
#endif
#endif
#endif
#ifdef STABLEPROP
    //cout << "\n FG phi holds" << endl;
    message(_cbad_("FG phi holds!"));
#endif
#ifdef FAIRPROP
    //cout << "\n GF phi holds" << endl;
    message(_cbad_("GF phi holds!"));
#endif
#ifdef EVENTUALLYPROP
    //cout << "\n eventually phi holds\n";
    message(_cbad_("eventually phi holds!"));
#endif
#ifdef FULL
    if (Globals::resultfile) {
        fprintf(Globals::resultfile, "\n  );\n};\n");
    }
#endif
    statistics(NrOfStates, Edges, NonEmptyHash);

    // tidy of the graph output stream, because we did not call
    // printincompletestates()
    if (graphstream != &cout) {
        delete graphstream;
    }

    // return 0: we did not find what we were looking for
    return 0;
#endif
}

// specify property specific initialization routines

int initialize_none() {
    return -1;
}
int initialize_place() {
    return -1;
}
int initialize_transition() {
    return -1;
}

// select property specific initialization procedure

#ifdef STATEPREDICATE
#define INITIALIZE_PROPERTY initialize_statepredicate
#define CHECK_EARLY_ABORTION (F->value)
#define EARLY_ABORT_MESSAGE "state found!"
#define LATE_ABORT_MESSAGE "state not found!"
#define RESULT_NAME "statepredicate"
#endif

#ifdef DEADLOCK
#define INITIALIZE_PROPERTY initialize_none
#define CHECK_EARLY_ABORTION (!CurrentState->firelist || !(CurrentState->firelist[0]))
#define EARLY_ABORT_MESSAGE "dead state found!"
#define LATE_ABORT_MESSAGE "net does not have deadlocks!"
#define RESULT_NAME "deadlock"
#endif

#ifdef BOUNDEDPLACE
#define INITIALIZE_PROPERTY initialize_place
#define CHECK_EARLY_ABORTION (!Globals::CheckPlace->bounded)
#define EARLY_ABORT_MESSAGE std::string("place " + std::string(Globals::CheckPlace->name) + " is unbounded!").c_str()
#define LATE_ABORT_MESSAGE std::string("place " + std::string(Globals::CheckPlace->name) + " is bounded!").c_str()
#define RESULT_NAME "unbounded place"
#endif

#ifdef DEADTRANSITION
#define INITIALIZE_PROPERTY initialize_transition
#define CHECK_EARLY_ABORTION (Globals::CheckTransition->enabled)
#define EARLY_ABORT_MESSAGE std::string("transition " + std::string(Globals::CheckTransition->name) + " is not dead").c_str()
#define LATE_ABORT_MESSAGE std::string("transition " + std::string(Globals::CheckTransition->name) + " is dead").c_str()
#define RESULT_NAME "not dead"
#endif

/*!
    \brief compares current and target marking, used in search procedures
    \return 0 markings different
    \return 1 markings different
*/
int compare_markings() {
    for (unsigned int i = 0; i < Globals::Places[0]->cnt; ++i) {
        if (Globals::CurrentMarking[i] != Globals::Places[i]->target_marking) {
            return 0;
        }
    }
    return 1;
}

#ifdef REACHABILITY
#define INITIALIZE_PROPERTY initialize_none
#ifdef STUBBORN
#define CHECK_EARLY_ABORTION (!CurrentState->firelist)
#else
#define CHECK_EARLY_ABORTION (compare_markings())
#endif
#define EARLY_ABORT_MESSAGE "state found!"
#define LATE_ABORT_MESSAGE "state is unreachable!"
#define RESULT_NAME "reachable"
#endif

#ifdef BOUNDEDNET
#define INITIALIZE_PROPERTY initialize_none
#define CHECK_EARLY_ABORTION (NewOmegas)
#define EARLY_ABORT_MESSAGE "net is unbounded!"
#define LATE_ABORT_MESSAGE "net is bounded!"
#define RESULT_NAME "unbounded"
#endif

#ifndef INITIALIZE_PROPERTY
#define INITIALIZE_PROPERTY initialize_none
#endif
#ifndef CHECK_EARLY_ABORTION
#define CHECK_EARLY_ABORTION (1)
#endif
#ifndef EARLY_ABORT_MESSAGE
#define EARLY_ABORT_MESSAGE ""
#endif
#ifndef LATE_ABORT_MESSAGE
#define LATE_ABORT_MESSAGE ""
#endif
#ifndef RESULT_NAME
#define RESULT_NAME ""
#endif

/*!
 \brief plain depth first search, no scc, no tscc, no fairness

 \return 1 if we found what we were looking for; that is, the search is aborted
         Examples: deadlock found, state found, unbounded net found, etc.

 \return 0 if we did not found what we were looking for; that is, complete search
         Examples: no deadlock found, no dead transition found, etc.
*/
unsigned int simple_depth_first() {

    State* NewState;  ///< state most recently generated during search


    // initialize hash table
    for (int i = 0; i < HASHSIZE; ++i) {
#ifdef BITHASH
        BitHashTable[i] = 0;
#else
        binHashTable[i] = NULL;
#endif
    }

#if defined(SYMMETRY) && SYMMINTEGRATION==1
    // this data structure is used for skipping symmetries in the
    //    "for all symmetries sigma lookup sigma(m)" loop
    Trace = new SearchTrace [Globals::Places[0]->cnt];
#endif

// initialize property
// return value > 0 signals that result for this property is
// determined structurally
// instances of this macros have shape int initialize_*();

    int result = INITIALIZE_PROPERTY();
    (void)result;

// Insert initial state (already in Globals::CurrentMarking)
// into data stucture. The preceding SEARCHPROC
// sets static variables needed by INSERTPROC.
    if (SEARCHPROC()) {
        cerr << "Sollte eigentlich nicht vorkommen";
    }
    CurrentState = INSERTPROC();
    CurrentState->parent = NULL; // parent in search tree
    CurrentState->firelist = FIRELIST(); // the transitions to be fired
    CurrentState->current = 0; // index of current transition to be fired in firelist

    // report initial state to graph output
    TheGraphReport->tell();

// Initialize statistical information
    NrOfStates = 1;
    Edges = 0;

#ifdef COVER
    // a marking that is used to search backwards for covered states
    Ancestor = new unsigned int[Globals::Places[0]->cnt + 1];
    CurrentState->NewOmega = NULL; // a vector that records where new
    // omegas have been inserted
    // while producung this state, used for
    // backtracking
    Place** NewOmegas = NULL;      // holds CurrentState->NewOmega when
    // processing it (saves derefencings)

#endif

#ifndef FULL
    if (CHECK_EARLY_ABORTION) { // this is property dependent
        //cout << "\n";
        //cout << _ctool_("lola: ") << _cgood_(EARLY_ABORT_MESSAGE) << "\n";
        message(_cgood_(EARLY_ABORT_MESSAGE));
        if (Globals::resultfile) {
            fprintf(Globals::resultfile, "%s : {\n  result = true;\n  ", RESULT_NAME);
        }

        printstate("", Globals::CurrentMarking); // print witness state
        print_path(CurrentState);     // print witness path

        if (Globals::resultfile) {
            fprintf(Globals::resultfile, "};\n");
            fprintf(Globals::resultfile, "statespace: {\n  complete = false;\n  states = ( ");
        }


        if (Globals::resultfile) {
            fprintf(Globals::resultfile, "\n  );\n};\n");
        }

        statistics(NrOfStates, Edges, NonEmptyHash);

        return 1;
    }
#endif

    // process marking until returning from initial state
    while (CurrentState) { // NULL after having backtracked from initial state
        if (CurrentState->firelist[CurrentState->current]) {
            // there is a next state that needs to be explored
            ++Edges;
            if (!(Edges % REPORTFREQUENCY)) {
                report_statistics();
            }
            CurrentState->firelist[CurrentState->current]->fire();

#ifdef COVER
            //In coverability graphs, we need to check for new w
            // 1. Search backwards until last w-Intro for smaller state
            unsigned int NrCovered;
            State* smallerstate;

            NewOmegas = NULL;
            // for all ancestor states do ...
            for (int i = 0; i < Globals::Places[0]->cnt; ++i) {
                Ancestor[i] = Globals::CurrentMarking[i];
            }
            for (smallerstate = CurrentState; smallerstate; smallerstate = smallerstate->parent) {
                smallerstate->firelist[smallerstate->current]->traceback();
                NrCovered = 0;
                for (int i = 0; i < Globals::Places[0]->cnt; ++i) {
                    // case 1: smaller state[i] > current state [i]
                    // --->continue with previous state
                    if (Ancestor[i] > Globals::CurrentMarking[i]) {
                        goto nextstate;
                    }

                    // case 2: smaller state < current state
                    // count w-Intro
                    if (Ancestor[i] < Globals::CurrentMarking[i]) {
                        NrCovered++;
                    }
                    // case 3: smaller state = current state -->do nothing
                }

                // if arrived here, it holds smaller <= current
                // covering is proper iff NrCovered > 0
                // If covering is not proper, (smaller state = current state)
                // current marking is not new, ancestors of smaller marking cannot
                // be smaller than current marking, since they would be smaller than
                // this smaller marking -->leave w-Intro procedure
                if (!NrCovered) {
                    smallerstate = NULL;
                    goto endomegaproc;
                }

                // Here, smallerstate IS less than current state.
                NewOmegas = new Place*[NrCovered + 1];

                // for all fragements of state vector do ...
                NrCovered = 0;
                for (int i = 0; i < Globals::Places[0]->cnt; ++i) {
                    if (Ancestor[i] < Globals::CurrentMarking[i]) {
                        // Here we have a place that deserves a new Omega
                        // 1. set old value in place record
                        Globals::Places[i]->lastfinite = Globals::CurrentMarking[i];
                        Globals::Places[i]->set_cmarking(VERYLARGE);
                        Globals::Places[i]->bounded = false;
                        NewOmegas[NrCovered++] = Globals::Places[i];
                    }
                }
                NewOmegas[NrCovered] = NULL;
                goto endomegaproc;
                if (smallerstate->smaller) { // smallerstate is a omega-introducing state
                    break;
                }

nextstate:
                ;
            }

endomegaproc:
            if (!NewOmegas) {
                smallerstate = NULL;
            }
#endif

            /// I really mean = and not ==
            if ((NewState = SEARCHPROC())) {
                // State exists! -->backtracking to previous state

#ifdef COVER
                // revert omega introduction
                if (NewOmegas) {
                    // Replace new omegas by their old values
                    for (int i = 0; NewOmegas[i]; ++i) {
                        NewOmegas[i]->set_cmarking(NewOmegas[i]->lastfinite);
                        NewOmegas[i]->bounded = true;
                    }
                    delete[] NewOmegas;
                }
#endif
                CurrentState->firelist[CurrentState->current]->backfire();
                ++(CurrentState->current);
            } else {
                // State does not exist! -->check property, insert, go ahead

                NewState = INSERTPROC();
#ifdef WITHFORMULA
                update_formula(CurrentState->firelist[CurrentState->current]);
#endif
                NewState->firelist = FIRELIST();
                ++NrOfStates;
                TheGraphReport->tell();
#ifdef MAXIMALSTATES
                checkMaximalStates(NrOfStates); ///// LINE ADDED BY NIELS
#endif
                NewState->current = 0;
                NewState->parent = CurrentState;
#ifdef COVER
                NewState->smaller = smallerstate;
                NewState->NewOmega = NewOmegas;
#endif
                CurrentState = NewState;
#ifndef FULL
                if (CHECK_EARLY_ABORTION) {
                    //cout << "\n";
                    //cout << _ctool_("lola: ") << _cgood_(EARLY_ABORT_MESSAGE) << "\n";
                    message(_cgood_(EARLY_ABORT_MESSAGE));
                    if (Globals::resultfile) {
                        fprintf(Globals::resultfile, "%s: {\n  result = true;\n  ", RESULT_NAME);
                    }

                    printstate("", Globals::CurrentMarking);
                    print_path(CurrentState);

                    if (Globals::resultfile) {
                        fprintf(Globals::resultfile, "};\n");
                        fprintf(Globals::resultfile, "statespace: {\n  complete = false;\n  states = ( ");
                    }


                    if (Globals::resultfile) {
                        fprintf(Globals::resultfile, "\n  );\n};\n");
                    }

                    statistics(NrOfStates, Edges, NonEmptyHash);

                    return 1;
                }
#endif
            } // end else branch for "if state exists"
        } else {
            // firing list completed -->close state and return to previous state
#ifdef COVER
            if (CurrentState->NewOmega) {
                // Replace new omegas by their old values
                for (int i = 0; CurrentState->NewOmega[i]; ++i) {
                    CurrentState->NewOmega[i]->set_cmarking(CurrentState->NewOmega[i]->lastfinite);
                    CurrentState->NewOmega[i]->bounded = true;
                }
                delete [] CurrentState->NewOmega;
            }
#endif

            CurrentState = CurrentState->parent;
            if (CurrentState) {
                CurrentState->firelist[CurrentState->current]->backfire();
#ifdef WITHFORMULA
                update_formula(CurrentState->firelist[CurrentState->current]);
#endif
                ++(CurrentState->current);
            }
        }
    }
// search finished
#ifdef BITHASH
    //cout << "\nno conclusive result!\n";
    message(_cbad_("no conclusive result!"));
#else
#ifdef FULL
    if (Globals::resultfile) {
        fprintf(Globals::resultfile, "\n  );\n};\n");
    }
#else
    //cout << "\n";
    //cout << _ctool_("lola: ") << _cbad_(LATE_ABORT_MESSAGE) << "\n";
    message(_cbad_(LATE_ABORT_MESSAGE));
    if (Globals::resultfile) {
        fprintf(Globals::resultfile, "%s: {\n  result = false;\n};\n", RESULT_NAME);
    }
#endif
#endif
    statistics(NrOfStates, Edges, NonEmptyHash);


    // return 0: we did not find what we were looking for
    return 0;
}






unsigned int DistributeNow = 0;

unsigned int breadth_first() {
    // min true = Dieser Knoten hat noch fruchtbare Zweige
    // succ[i] 0 = dieser Zweig ist nicht mehr fruchtbar
#ifdef BREADTH_FIRST

    ostream* graphstream = NULL;
    unsigned int limit;
    unsigned int d;
    unsigned int i;

    Edges = 0;
    if (Globals::gmflg) {
        graphstream = new ofstream(Globals::graphfile);
        if (graphstream->fail()) {
            abort(4, "cannot open graph output file '%s' - no output written", _cfilename_(Globals::graphfile));
        }
    }
    if (Globals::GMflg) {
        graphstream = &std::cout;
    }


    State* CurrentState, * NewState, * initial;
    // init initial marking and hash table
    isbounded = 1;
#ifdef SYMMETRY
    Trace = new SearchTrace [CardStore];
#endif
#ifndef DISTRIBUTE
    for (i = 0; i < HASHSIZE; i++) {
#ifdef BITHASH
        BitHashTable[i] = 0;
#else
        binHashTable[i] = NULL;
#endif
    }
#endif
    NrOfStates = d = limit = 1;
#ifdef DISTRIBUTE
#if defined(SYMMETRY) && SYMMINTEGRATION == 3
    canonize();
    search_and_insert(kanrep);
#else
    search_and_insert(Globals::CurrentMarking);
#endif
    initial = CurrentState = new State();
#else
    CurrentState = SEARCHPROC(); // inits search data structure
    initial = CurrentState = INSERTPROC();
#endif
    CurrentState->dfs = 0;
    CurrentState->current = 0;
    CurrentState->min = true;
    for (i = 0; i < Globals::Transitions[0]->cnt; i++) {
        Globals::Transitions[i]->check_enabled();
    }
    CurrentState->firelist = FIRELIST();

#ifdef COVER
    CurrentState->NewOmega = NULL;
    Ancestor = new unsigned int [Globals::Places[0]->cnt + 1];
#endif
    int j;
    if (Globals::gmflg || Globals::GMflg) {
        (*graphstream) << "STATE " << CurrentState->dfs << "; DEPTH 0";
        j = 0;
        if (Globals::graphformat == 'm') {
            for (i = 0; i < Globals::Places[0]->cnt; ++i) {
                if (Globals::CurrentMarking[i]) {
                    if (Globals::CurrentMarking[i] == VERYLARGE) {
                        (*graphstream) << (j++ ? ",\n" : "\n") << Globals::Places[i]->name << " : " << "oo" ;
                    } else {
                        (*graphstream) << (j++ ? ",\n" : "\n") << Globals::Places[i]->name << " : " << Globals::CurrentMarking[i];
                    }
                }
            }
        }
        (*graphstream) << "\n\n";
        for (i = 0; CurrentState->firelist[i]; ++i) {
            (*graphstream) << CurrentState->firelist[i]->name << "\n";
        }
        (*graphstream) << "\n";
    }
#ifdef STUBBORN
    if (!CurrentState->firelist) {
        // early abortion
#ifdef REACHABILITY
        //cout << "\nstate found!\n";
        message(_cgood_("state found!"));
#endif
#ifdef DEADTRANSITION
        //cout << "\ntransition " << Globals::CheckTransition->name << " is not dead!\n";
        message("%s %s %s", _cgood_("transition"), _cgood_(Globals::CheckTransition->name), _cgood_("is not dead!"));
#endif
        printstate("", Globals::CurrentMarking);
        print_path(CurrentState);
        statistics(NrOfStates, Edges, NonEmptyHash);
        return 1;
    }
#endif
#if defined(DEADTRANSITION) && !defined(STUBBORN)
    if (Globals::CheckTransition->enabled) {
        //cout << "\ntransition " << Globals::CheckTransition->name << " is not dead!\n";
        message("%s %s %s", _cgood_("transition"), _cgood_(Globals::CheckTransition->name), _cgood_("is not dead!"));
        printstate("", Globals::CurrentMarking);
        print_path(CurrentState);
        statistics(NrOfStates, Edges, NonEmptyHash);
        return 1;
    }
#endif
#ifdef DEADLOCK
    if (!CurrentState->firelist || !(CurrentState->firelist[0])) {
        // early abortion
        //cout << "\ndead state found!\n";
        message(_cgood_("dead state found!"));
        if (Globals::resultfile) {
            fprintf(Globals::resultfile, "deadlock: {\n  result = true;\n");
        }
        printstate("", Globals::CurrentMarking);
        print_path(CurrentState);

        if (Globals::resultfile) {
            fprintf(Globals::resultfile, "};\n");
        }

        statistics(NrOfStates, Edges, NonEmptyHash);
        return 1;
    }
#endif
#ifdef STATEPREDICATE
    int res;
    F = F->reduce(&res);
    if (res < 2) {
        return res;
    }
    F = F->posate();
    F->tempcard = 0;
    F->setstatic();
    if (F->tempcard) {
        abort(3, "temporal operators are not allowed in state predicates - not allowed for task %s", _cparameter_("STATEPREDICATE"));
    }
    //cout << "\n Formula with\n" << F->card << " subformula.\n";
    message("Formula with %d subformula(s) (%s)", F->card, _cparameter_("STATEPREDICATE"));
    F->parent = NULL;
    if (F->initatomic()) {
        //cout << "\nstate found!\n";
        message(_cgood_("state found!"));
        printstate("", Globals::CurrentMarking);
        print_path(CurrentState);
        printincompletestates(CurrentState, graphstream);
        if (graphstream != &cout) {
            delete graphstream;
        }
        statistics(NrOfStates, Edges, NonEmptyHash);
        return 1;
    }
#endif

#if ( defined (REACHABILITY ) && ! defined ( STUBBORN ) )
    for (i = 0; i < Globals::Places[0]->cnt; ++i) {
        if (Globals::CurrentMarking[i] != Globals::Places[i]->target_marking) {
            break;
        }
    }
    if (i >= Globals::Places[0]->cnt) { // target_marking found!
        // early abortion
        //cout << "\nstate found!\n";
        message(_cgood_("state found!"));
        printstate("", Globals::CurrentMarking);
        print_path(CurrentState);
        statistics(NrOfStates, Edges, NonEmptyHash);
        return 1;
    }
#endif
    CurrentState->parent = NULL;
    CurrentState->succ = new State * [CardFireList];

    // process marking until returning from initial state
    while (CurrentState) {
        if (CurrentState->firelist[CurrentState->current]) {
            if (d == limit) {
                // working layer
                ++Edges;
                if (!(Edges % REPORTFREQUENCY)) {
#ifdef DISTRIBUTE
                    rapport(rapportstring);
#else
                    report_statistics();
#endif
                }
                CurrentState->firelist[CurrentState->current]->fire();
#ifdef COVER
                //In coverability graphs, we need to check for new w

                // 1. Search backwards until last w-Intro for smaller state
                unsigned int NrCovered;
                State* smallerstate;
                Place** NewOmegas;

                for (i = 0; i < Globals::Places[0]->cnt; ++i) {
                    Ancestor[i] = Globals::CurrentMarking[i];
                }
                NewOmegas = NULL;
                // for all ancestor states do ...
                for (smallerstate = CurrentState; smallerstate; smallerstate = smallerstate->parent) {
                    smallerstate->firelist[smallerstate->current]->traceback();
                    NrCovered = 0;
                    for (i = 0; i < Globals::Places[0]->cnt; ++i) {
                        // case 1: smaller state[i] > current state [i]
                        // --->continue with previous state
                        if (Ancestor[i] > Globals::CurrentMarking[i]) {
                            goto nextstate;
                        }
                        // case 2: smaller state < current state
                        // count w-Intro
                        if (Ancestor[i] < Globals::CurrentMarking[i]) {
                            ++NrCovered;
                        }
                        // case 3: smaller state = current state -->do nothing
                    }
                    // if arrived here, it holds smaller <= current
                    // covering is proper iff NrCovered > 0
                    // If covering is not proper, (smaller state = current state)
                    // current marking is not new, ancestors of smaller marking cannot
                    // be smaller than current marking, since they would be smaller than
                    // this smaller marking -->leave w-Intro procedure
                    if (!NrCovered) {
                        smallerstate = NULL;
                        goto endomegaproc;
                    }
                    // Here, smallerstate IS less than current state.
                    isbounded = 0;
                    NewOmegas = new Place * [NrCovered + 1];
                    // for all fragements of state vector do ...
                    NrCovered = 0;
                    for (i = 0; i < Globals::Places[0]->cnt; ++i) {
                        if (Ancestor[i] < Globals::CurrentMarking[i]) {
                            // Here we have a place that deserves a new Omega
                            // 1. set old value in place record
                            Globals::Places[i]->lastfinite = Globals::CurrentMarking[i];
                            Globals::Places[i]->set_cmarking(VERYLARGE);
                            Globals::Places[i]->bounded = false;
                            NewOmegas[NrCovered++] = Globals::Places[i];
                        }
                    }
                    NewOmegas[NrCovered] = NULL;
                    goto endomegaproc;

nextstate:
                    if (smallerstate->smaller) { // smallerstate is a omega-introducing state
                        break;
                    }
                }
endomegaproc:
                if (!NewOmegas) {
                    smallerstate = NULL;
                }
#endif
#ifdef WITHFORMULA
#ifndef TWOPHASE
                update_formula(CurrentState->firelist[CurrentState->current]);
#endif
#endif
#ifdef DISTRIBUTE
#if defined(SYMMETRY) && SYMMINTEGRATION == 3
                canonize();
                if (!search_and_insert(kanrep, DistributeNow))
#else
                if (!search_and_insert(Globals::CurrentMarking, DistributeNow))
#endif
#else
                if (NewState = SEARCHPROC())
#endif
                {
#ifdef COVER
                    if (NewOmegas) {
                        // Replace new omegas by their old values
                        for (i = 0; NewOmegas[i]; i++) {
                            NewOmegas[i]->set_cmarking(NewOmegas[i]->lastfinite);
                            NewOmegas[i]->bounded = true;
                        }
                        delete [] NewOmegas;
                    }
#endif
                    CurrentState->firelist[CurrentState->current]->backfire();
#ifdef WITHFORMULA
#ifndef TWOPHASE
                    update_formula(CurrentState->firelist[CurrentState->current]);
#endif
#endif
                    CurrentState->succ[(CurrentState->current)++] = NULL;
                } else {
#ifdef DISTRIBUTE
                    NewState = new State();
#else
                    NewState = INSERTPROC();
#endif
                    CurrentState->min = true;
                    NewState->dfs =  NrOfStates ++;
#ifdef MAXIMALSTATES
                    checkMaximalStates(NrOfStates); ///// LINE ADDED BY NIELS
#endif
#ifdef DISTRIBUTE
                    if (NrOfStates >= 500) {
                        DistributeNow = 1;
                    }
#endif
                    NewState->current = 0;
                    NewState->firelist = FIRELIST();
                    NewState->parent = CurrentState;
                    NewState->succ =  new State * [CardFireList];
                    if (Globals::gmflg || Globals::GMflg) {
                        (*graphstream) << "STATE " << NewState->dfs << " FROM " <<
                                       CurrentState->dfs << " BY " <<
                                       CurrentState->firelist[CurrentState->current]->name
                                       << "; DEPTH " << limit;
                        j = 0;
                        if (Globals::graphformat == 'm') {
                            for (i = 0; i < Globals::Places[0]->cnt; i++) {
                                if (Globals::CurrentMarking[i]) {
                                    if (Globals::CurrentMarking[i] == VERYLARGE) {
                                        (*graphstream) << (j++ ? ",\n" : "\n") << Globals::Places[i]->name << " : " << "oo" ;
                                    } else {
                                        (*graphstream) << (j++ ? ",\n" : "\n") << Globals::Places[i]->name << " : " << Globals::CurrentMarking[i] ;
                                    }
                                }
                            }

                        }
                        (*graphstream) << "\n\n";
                        if (NewState->firelist) {
                            for (i = 0; NewState->firelist[i]; i++) {
                                (*graphstream) << NewState->firelist[i]->name << "\n";
                            }
                        }
                        (*graphstream) << "\n";
                    }
                    CurrentState->succ[CurrentState->current] = NewState;
#ifdef STUBBORN
                    if (!NewState->firelist) {
                        // early abortion
#ifdef REACHABILITY
                        //cout << "\nstate found!\n";
                        message(_cgood_("state found!"));
#endif
#ifdef STATEPREDICATE
                        //cout << "state found!\n";
                        message(_cgood_("state found!"));
#endif
                        printstate("", Globals::CurrentMarking);
                        print_path(NewState);
                        statistics(NrOfStates, Edges, NonEmptyHash);
                        return 1;
                    }
#endif
#ifdef BOUNDEDPLACE
                    if (!Globals::CheckPlace->bounded) {
                        //cout << "place " << Globals::CheckPlace->name << " is unbounded!\n";
                        message("%s %s %s", _cgood_("place"), _cgood_(Globals::CheckPlace->name), _cgood_("is unbounded!"));
                        printstate("", Globals::CurrentMarking);
                        NewState->smaller = smallerstate;
                        print_reg_path(NewState, smallerstate);
                        cout << "\n";
                        statistics(NrOfStates, Edges, NonEmptyHash);
                        return 1;
                    }
#endif
#ifdef BOUNDEDNET
                    if (!isbounded) {
                        //cout << "net is unbounded!\n";
                        message(_cgood_("net is unbounded!"));

                        if (Globals::resultfile) {
                            fprintf(Globals::resultfile, "unbounded: {\n  result = true;\n");
                        }

                        printstate("", Globals::CurrentMarking);
                        NewState->smaller = smallerstate;
                        print_reg_path(NewState, smallerstate);
                        cout << "\n";

                        if (Globals::resultfile) {
                            fprintf(Globals::resultfile, "};\n");
                        }

                        statistics(NrOfStates, Edges, NonEmptyHash);
                        return 1;
                    }
#endif
#ifdef DEADLOCK
                    if (!NewState->firelist || !(NewState->firelist[0])) {
                        // early abortion
                        //cout << "\ndead state found!\n";
                        message(_cgood_("dead state found!"));

                        if (Globals::resultfile) {
                            fprintf(Globals::resultfile, "deadlock: {\n  result = true;\n");
                        }

                        printstate("", Globals::CurrentMarking);
                        print_path(NewState);

                        if (Globals::resultfile) {
                            fprintf(Globals::resultfile, "};\n");
                        }

                        statistics(NrOfStates, Edges, NonEmptyHash);
                        return 1;
                    }
#endif
#if defined(DEADTRANSITION)
                    if (Globals::CheckTransition->enabled) {
                        //cout << "\ntransition " << Globals::CheckTransition->name << " is not dead!\n";
                        message("%s %s %s", _cgood_("transition"), _cgood_(Globals::CheckTransition->name), _cgood_("is not dead!"));
                        printstate("", Globals::CurrentMarking);
                        print_path(CurrentState);
                        statistics(NrOfStates, Edges, NonEmptyHash);
                        return 1;
                    }
#endif
#ifdef STATEPREDICATE
                    if (! NewState->firelist || F->value) {
                        // early abortion
                        //cout << "\nstate found!\n";
                        message(_cgood_("state found!"));
                        printstate("", Globals::CurrentMarking);
                        print_path(NewState);
                        statistics(NrOfStates, Edges, NonEmptyHash);
                        return 1;
                    }
#endif
#if ( defined (REACHABILITY ) && ! defined ( STUBBORN ) )
                    for (i = 0; i < Globals::Places[0]->cnt; i++) {
                        if (Globals::CurrentMarking[i] != Globals::Places[i]->target_marking) {
                            break;
                        }
                    }
                    if (i >= Globals::Places[0]->cnt) { // target_marking found!
                        // early abortion
                        //cout << "\nstate found!\n";
                        message(_cgood_("state found!"));
                        printstate("", Globals::CurrentMarking);
                        print_path(NewState);
                        statistics(NrOfStates, Edges, NonEmptyHash);
                        return 1;
                    }
#endif
#ifdef COVER
                    NewState->smaller = smallerstate;
                    NewState->NewOmega = NewOmegas;
                    if (NewOmegas) {
                        // Replace new omegas by their old values
                        for (i = 0; NewOmegas[i]; i++) {
                            NewOmegas[i]->set_cmarking(NewOmegas[i]->lastfinite);
                            NewOmegas[i]->bounded = true;
                        }
                    }
#endif
                    CurrentState->firelist[CurrentState->current]->backfire();

#ifdef WITHFORMULA
#ifndef TWOPHASE
                    update_formula(CurrentState->firelist[CurrentState->current]);
#endif
#endif
                    CurrentState->current++;
                }
            } else {
                // proceed forward to working layer
                if (CurrentState->succ[CurrentState->current]) {
                    CurrentState->firelist[CurrentState->current]->fire();
#ifdef WITHFORMULA
#ifndef TWOPHASE
                    update_formula(CurrentState->firelist[CurrentState->current]);
#endif
#endif
                    CurrentState = CurrentState->succ[CurrentState->current];
#ifdef COVER
                    // set omegas according to stored list
                    if (CurrentState->NewOmega) {
                        for (i = 0; CurrentState->NewOmega[i]; i++) {
                            CurrentState->NewOmega[i]->lastfinite =
                                Globals::CurrentMarking[CurrentState->NewOmega[i]->index];
                            CurrentState->NewOmega[i]->bounded = false;
                            CurrentState->NewOmega[i]->set_cmarking(VERYLARGE);
                        }
                    }
#endif
                    CurrentState->min = false;
                    CurrentState->current = 0;
                    d++;
                } else {
                    (CurrentState->current)++;
                }
            }
        } else {

            // close state and return to previous state
            if (CurrentState->min) {
                CurrentState = CurrentState->parent;
                if (CurrentState) {
                    CurrentState->min = true;
                }
            } else {
                CurrentState = CurrentState->parent;
                if (CurrentState) {
                    CurrentState->succ[CurrentState->current] = NULL;
                }
            }

            d--;
            if (CurrentState) {
#ifdef COVER
                if (CurrentState->NewOmega) {
                    // Replace new omegas by their old values
                    for (i = 0; CurrentState->NewOmega[i]; i++) {
                        CurrentState->NewOmega[i]->set_cmarking(CurrentState->NewOmega[i]->lastfinite);
                        CurrentState->NewOmega[i]->bounded = true;
                    }
                }
#endif
                CurrentState->firelist[CurrentState->current]->backfire();
#ifdef WITHFORMULA
#ifndef TWOPHASE
                update_formula(CurrentState->firelist[CurrentState->current]);
#endif
#endif
                CurrentState->current ++;
            } else {
                if (initial->min) {
                    limit++;
                    d = 1;
                    CurrentState = initial;
                    CurrentState->current = 0;
                    initial->min = false;
                }
            }
        }

    }

#ifdef REACHABILITY
    //cout << "\n state is not reachable!\n";
    message(_cbad_("state is not reachable!"));
#endif
#ifdef DEADLOCK
    //cout << "\nnet does not have deadlocks!\n";
    message(_cbad_("net does not have deadlocks!"));
    if (Globals::resultfile) {
        fprintf(Globals::resultfile, "deadlock: {\n  result = false;\n}\n");
    }
#endif
#if defined(STATEPREDICATE) && ! defined(LIVEPROP)
    //cout << "\n predicate is not satisfiable!\n";
    message(_cbad_("predicate is not satisfiable!"));
#endif
#ifdef DEADTRANSITION
    //cout << "\ntransition " << Globals::CheckTransition->name << " is dead!\n";
    message("%s %s %s", _cbad_("transition"), _cbad_(Globals::CheckTransition->name), _cbad_("is dead!"));
#endif
#ifdef BOUNDEDPLACE
    //cout << "\nplace " << Globals::CheckPlace->name << " is bounded!\n";
    message("%s %s %s", _cbad_("place"), _cbad_(Globals::CheckPlace->name), _cbad_("is unbounded!"));
#endif
#ifdef BOUNDEDNET
    if (isbounded) {
        //cout << "\nnet is bounded!\n";
        message(_cbad_("net is bounded!"));
        if (Globals::resultfile) {
            fprintf(Globals::resultfile, "unbounded: {\n  result = false;\n}\n\n");
        }
    } else {
        //sollte nicht stattfinden, oder?!
        assert(isbounded == true);
        //cout << "\nnet is unbounded!\n";
        message(_cgood_("net is unbounded!"));
    }
#endif
#endif
    statistics(NrOfStates, Edges, NonEmptyHash);

    // return 0: we did not find what we were looking for
    return 0;
}





void RemoveGraph() {
#ifndef BITHASH
    for (int i = 0; i < HASHSIZE; ++i) {
        if (binHashTable[i]) {
            delete binHashTable[i];
        }
        binHashTable[i] = NULL;
    }
#endif
}





#ifdef STUBBORN
bool mutual_reach() {
#ifdef TARJAN
    // unused: unsigned int i;
    State* NewState;
    // init initial marking and hash table
#ifdef SYMMETRY
    Trace = new SearchTrace [CardStore];
#endif
    RemoveGraph();
    NrOfStates = 1;
    NonEmptyHash = 0;
    if (binSearch()) {
        cerr << " Wat soll dat denn?";
    }
    CurrentState = binInsert();
    CurrentState->current = 0;
    Edges = 0;
    CurrentState->firelist = stubbornfirelistreach();

    if (!CurrentState->firelist) {
        // early abortion
        statistics(NrOfStates, Edges, NonEmptyHash);
        return true;
    }
    CurrentState->parent = NULL;
    CurrentState->succ = new State * [CardFireList];

    // process marking until returning from initial state

    while (CurrentState) {
        if (CurrentState->firelist[CurrentState->current]) {
            // there is a next state that needs to be explored
            Edges ++;
            if (!(Edges % REPORTFREQUENCY)) {
                report_statistics();
            }
            CurrentState->firelist[CurrentState->current]->fire();
            if ((NewState = SEARCHPROC())) {
                // State exists!
                CurrentState->firelist[CurrentState->current]->backfire();
                CurrentState->succ[CurrentState->current] = NewState;
                (CurrentState->current) ++;
            } else {
                NewState = INSERTPROC();
                NewState->current = 0;
                NewState->firelist = stubbornfirelistreach();
                NrOfStates ++ ;
#ifdef MAXIMALSTATES
                checkMaximalStates(NrOfStates); ///// LINE ADDED BY NIELS
#endif
                NewState->parent = CurrentState;
                NewState->succ =  new State * [CardFireList];
                CurrentState->succ[CurrentState->current] = NewState;
                if (!NewState->firelist) {
                    // early abortion
                    statistics(NrOfStates, Edges, NonEmptyHash);
                    return true;
                }
                CurrentState = NewState;
            }
        } else {
            // close state and return to previous state
            CurrentState = CurrentState->parent;
            if (CurrentState) {
                CurrentState->firelist[CurrentState->current]->backfire();
                CurrentState->current ++;
            }
        }
    }
#endif
    statistics(NrOfStates, Edges, NonEmptyHash);
    return false;

}
#endif





#ifdef STUBBORN
#ifdef WITHFORMULA
bool target_reach() {
    // unused: unsigned int i;
    unsigned int NrOfStates;
    State* NewState;
    // init initial marking and hash table
#ifdef SYMMETRY
    Trace = new SearchTrace [CardStore];
#endif
    RemoveGraph();
    NrOfStates = 1;
    NonEmptyHash = 0;
    if (binSearch()) {
        cerr << " Wat soll dat denn?";
    }
    CurrentState = binInsert();
    CurrentState->current = 0;
#if defined(RELAXED) && ! defined(STRUCT)
    CurrentState->dfs = CurrentState->min = 0;
    MinBookmark = 1;
#endif
    Edges = 0;
    if (F->initatomic()) {
        return true;
    }
#ifdef RELAXED
    CurrentState->firelist = relaxedstubbornset();
#else
    CurrentState->firelist = stubbornfirelistpredicate();
#endif

    if (!CurrentState->firelist) {
        // early abortion
        statistics(NrOfStates, Edges, NonEmptyHash);
        return true;
    }
    CurrentState->parent = NULL;
#ifdef TARJAN
    CurrentState->succ = new State * [CardFireList];
#endif

    // process marking until returning from initial state

    while (CurrentState) {
        if (CurrentState->firelist[CurrentState->current]) {
            // there is a next state that needs to be explored
            Edges ++;
            if (!(Edges % REPORTFREQUENCY)) {
                report_statistics();
            }
            CurrentState->firelist[CurrentState->current]->fire();
            if ((NewState = SEARCHPROC())) {
                // State exists!
                CurrentState->firelist[CurrentState->current]->backfire();
#ifdef TARJAN
                CurrentState->succ[CurrentState->current] = NewState;
                CurrentState->min = MIN(CurrentState->min, NewState->min);
#endif
                (CurrentState->current) ++;
            } else {
                NewState = INSERTPROC();
                update_formula(CurrentState->firelist[CurrentState->current]);
                NewState->current = 0;
#ifdef RELAXED
                NewState->firelist = relaxedstubbornset();
#else
                NewState->firelist = stubbornfirelistpredicate();
#endif
                NewState->parent = CurrentState;
#ifdef TARJAN
                NewState->succ =  new State * [CardFireList];
                CurrentState->succ[CurrentState->current] = NewState;
                NewState->dfs = NewState->min = NrOfStates;
#endif
                NrOfStates ++;
#ifdef MAXIMALSTATES
                checkMaximalStates(NrOfStates); ///// LINE ADDED BY NIELS
#endif
                if (!NewState->firelist) {
                    // early abortion
                    statistics(NrOfStates, Edges, NonEmptyHash);
                    return true;
                }
                CurrentState = NewState;
            }
        } else {
            // close state and return to previous state
#if defined(RELAXED) && !defined(STRUCT)
            Transition** forgotten;

            forgotten = F->spp2(CurrentState);
            if (forgotten && forgotten[0]) {
                // fire list must be extended
                unsigned int nf;
                for (nf = 0; forgotten[nf]; nf++) {
                    ;
                }
                Transition** newFL = new Transition * [nf + CurrentState->current + 1];
                State** newSucc = new State * [nf + CurrentState->current];
                unsigned int i;
                for (i = 0; i < CurrentState->current; i++) {
                    newFL[i] = CurrentState->firelist[i];
                    newSucc[i] = CurrentState->succ[i];
                }
                for (i = 0; i < nf; i++) {
                    newFL[CurrentState->current + i] = forgotten[i];
                }
                newFL[CurrentState->current + i] = NULL;
                delete [] CurrentState->firelist;
                delete [] CurrentState->succ;
                CurrentState->firelist = newFL;
                CurrentState->succ = newSucc;
                continue;
            } else {
                //TSCC really closed
                MinBookmark = NrOfStates;
                CurrentState = CurrentState->parent;
            }
#else
            CurrentState = CurrentState->parent;
#endif
            if (CurrentState) {
                CurrentState->firelist[CurrentState->current]->backfire();
                update_formula(CurrentState->firelist[CurrentState->current]);
                CurrentState->current ++;
            }
        }
    }
    statistics(NrOfStates, Edges, NonEmptyHash);
    return false;
}
#endif
#endif





#ifdef REVERSIBILITY
int reversibility() {
    unsigned int i;
    for (i = 0; i < Globals::Places[0]->cnt; i++) {
        Globals::Places[i]->initial_marking = Globals::CurrentMarking[i];
    }
    // Compute representitives of all TSCC
    depth_first();
    //Check whether initial marking is reachable
    for (; TSCCRepresentitives; TSCCRepresentitives = TSCCRepresentitives->next) {
        // 1. initialize start and target markings
        for (i = 0; i < Globals::Places[0]->cnt; i++) {
            Globals::Places[i]->target_marking = Globals::Places[i]->initial_marking;
            Globals::Places[i]->set_cmarking((*TSCCRepresentitives->sv)[i]);
        }
        for (i = 0; i < Globals::Transitions[0]->cnt; i++) {
            Globals::Transitions[i]->check_enabled();
        }
        if (!mutual_reach()) {
            //cout << "\nnot reversible: no return to m0 from reported state\n\n";
            message(_cbad_("not reversible: no return to m0 from reported state"));

            if (Globals::resultfile) {
                fprintf(Globals::resultfile, "reversibility: {\n  result = false;\n");
            }

            printstate("", Globals::CurrentMarking);

            if (Globals::resultfile) {
                fprintf(Globals::resultfile, "};\n");
            }

            return 1;
        }
    }
    //cout << "\n net is reversible!\n";
    message(_cgood_("net is reversible!"));

    if (Globals::resultfile) {
        fprintf(Globals::resultfile, "reversibility: {\n  result = true;\n};\n");
    }
    return(0);
}
#endif





#if defined(LIVEPROP) && defined(TWOPHASE)
int liveproperty() {
    unsigned int i;

    int res;
    F = F->reduce(&res);
    if (res < 2) {
        return res;
    }
    F = F->posate();
    F->tempcard = 0;
    F->setstatic();
    if (F->tempcard) {
        abort(3, "temporal operators are not allowed in state predicates - not allowed for task %s", _cparameter_("LIVEPROP"));
    }
    //cout << "\n Formula with\n" << F->card << " subformula.\n";
    message("Formula with %d subformula(s) (%s)", F->card, _cparameter_("LIVEPROP"));
    F->parent = NULL;

    for (i = 0; i < Globals::Places[0]->cnt; i++) {
        Globals::Places[i]->initial_marking = Globals::CurrentMarking[i];
    }
    // Compute representitives of all TSCC
    depth_first();
    //Check whether target state is reachable
    for (; TSCCRepresentitives; TSCCRepresentitives = TSCCRepresentitives->next) {
        // 1. initialize start and target markings
        for (i = 0; i < Globals::Places[0]->cnt; i++) {
            Globals::Places[i]->target_marking = Globals::Places[i]->initial_marking;
            Globals::Places[i]->set_cmarking((*(TSCCRepresentitives->sv))[i]);
        }
        for (i = 0; i < Globals::Transitions[0]->cnt; i++) {
            Globals::Transitions[i]->check_enabled();
        }
        if (!target_reach()) {
            //cout << "\npredicate not live: not satisfiable beyond reported state\n\n";
            message(_cbad_("predicate not live: not satisfiable beyond reported state"));
            printstate("", Globals::CurrentMarking);
            return 1;
        }
    }
    //cout << "\n predicate is live!\n";
    message(_cgood_("predicate is live!"));
    return(0);
}
#endif





#if defined(HOME) && defined(TWOPHASE)
int home() {
    unsigned int i;
    StatevectorList* New, * Old, * Candidate;


    // Compute representitives of all TSCC
    depth_first();
    Candidate = TSCCRepresentitives;
    if (!Candidate) {
        abort(2, "serious internal error, maybe memory overflow?");
    }
    New = Candidate->next;
    Old = NULL;

    // First loop creates candidate for home property
    while (New) {
        // 1. initialize start and target markings
        for (i = 0; i < Globals::Places[0]->cnt; i++) {
            Globals::Places[i]->target_marking = Candidate->sv->vector[i];
            Globals::Places[i]->set_cmarking(New->sv->vector[i]);
        }
        for (i = 0; i < Globals::Transitions[0]->cnt; i++) {
            Globals::Transitions[i]->check_enabled();
        }
        if (!mutual_reach()) {
            Candidate->next = Old;
            Old = Candidate;
            Candidate = New;
        }
        New = New->next;
    }
    while (Old) {
        // 1. initialize start and target markings
        for (i = 0; i < Globals::Places[0]->cnt; i++) {
            Globals::Places[i]->target_marking = Candidate->sv->vector[i];
            Globals::Places[i]->set_cmarking(Old->sv->vector[i]);
        }
        for (i = 0; i < Globals::Transitions[0]->cnt; i++) {
            Globals::Transitions[i]->check_enabled();
        }
        if (!mutual_reach()) {
            //cout << "\nnet does not have home markings!\n\n";
            message(_cbad_("net does not have home markings!"));
            if (Globals::resultfile) {
                fprintf(Globals::resultfile, "homemarking: {\n  result = false;\n};\n");
            }
            return 1;
        }
        Old = Old->next;
    }

    //cout << "\n\n home marking found (reported state)\n\n";
    message(_cgood_("home marking found (reported state)"));
    if (Globals::resultfile) {
        fprintf(Globals::resultfile, "homemarking: {\n  result = true;\n");
    }

    for (i = 0; i < Globals::Places[0]->cnt; i++) {
        Globals::Places[i]->set_cmarking(Candidate->sv->vector[i]);
    }
    printstate("", Globals::CurrentMarking);

    if (Globals::resultfile) {
        fprintf(Globals::resultfile, "};\n");
    }

    return(0);
}
#else
int home() {
    return 0;
}
#endif





void print_binDec(binDecision* d, int indent);
void print_binDec(int h) {
    for (unsigned int i = 0; i < Globals::Places[0]->NrSignificant; i++) {
        cout << Globals::Places[i]->name << ": " << Globals::Places[i]->nrbits;
    }
    cout << endl;
    print_binDec(binHashTable[h], 0);
}




/*!
 Print bin decision table at hash entry h.
*/
void print_binDec(binDecision* d, int indent) {
    for (int i = 0; i < indent; i++) {
        cout << ".";
    }

    if (!d) {
        cout << " nil " << endl;
        return;
    }

    cout << "b " << d->bitnr << " v ";
    for (unsigned int i = 0; i < (Globals::BitVectorSize - (d->bitnr + 1)) ; i += 8) {
        cout << (unsigned int)(d->vector)[i / 8] << " " ;
    }

    for (unsigned int i = 0; i < Globals::BitVectorSize - (d->bitnr + 1); i++) {
        cout << (((d->vector)[i / 8] >> (7 - i % 8)) % 2);
    }
    cout << endl;
    print_binDec(d->nextold, indent + 1);
    print_binDec(d->nextnew, indent + 1);
}





#ifdef STATESPACE
unsigned int compute_scc() {
    ostream* graphstream = NULL;
    unsigned int i;
    State* NewState;

    // init initial marking and hash table
    if (Globals::gmflg || Globals::GMflg) {
        if (Globals::gmflg) {
            graphstream = new ofstream(Globals::graphfile);
            if (graphstream->fail()) {
                abort(4, "cannot open graph output file '%s' - no output written", _cfilename_(Globals::graphfile));
            }
        }
        if (Globals::GMflg) {
            graphstream = &std::cout;
        }
    }

    for (i = 0; i < HASHSIZE; ++i) {
        binHashTable[i] = NULL;
    }

    if (SEARCHPROC()) {
        cerr << "Sollte eigentlich nicht vorkommen";
    }
    NrOfStates = 1;
    Edges = 0;

    CurrentState = INSERTPROC();
    CurrentState->firelist = FIRELIST();
    CurrentState->current = 0;
    CurrentState->parent = NULL;

    CurrentState->succ = new State * [CardFireList + 1];
    CurrentState->dfs = CurrentState->min = 0;
    CurrentState->nexttar = CurrentState->prevtar = CurrentState;
    TarStack = CurrentState;

#ifdef LIMITCAPACITY
    bool boundbroken = false;
#endif

    // process marking until returning from initial state
    while (CurrentState) {
        if (CurrentState->firelist[CurrentState->current]) {
            // there is a next state that needs to be explored
            ++Edges;
            if (!(Edges % REPORTFREQUENCY)) {
                report_statistics();
            }

#ifdef LIMITCAPACITY
            boundbroken =
#endif
            CurrentState->firelist[CurrentState->current]->fire();
            if ((NewState = SEARCHPROC())) {
                // State exists! (or, at least, I am not responsible for it (in the moment))
                CurrentState->firelist[CurrentState->current]->backfire();
                CurrentState->succ[CurrentState->current] = NewState;
                if (!(NewState->tarlevel)) {
                    CurrentState->min = MIN(CurrentState->min, NewState->dfs);
                }
                (CurrentState->current) ++;
            } else {
                NewState = INSERTPROC();
#ifndef LIMITCAPACITY
                NewState->firelist = FIRELIST();
#endif
#ifdef LIMITCAPACITY
                if (!boundbroken) {
                	NewState->firelist = FIRELIST();
                } else {
                	// if the capacity was broken then mark the state as bad and add an empty firelist
                	Transition** tl = new Transition * [1];
                	tl[0] = NULL;
                	NewState->firelist = tl;
                	NewState->boundbroken = true;
                }
#endif
                NewState->dfs = NewState->min = NrOfStates++;
                NewState->prevtar = TarStack;
                NewState->nexttar = TarStack->nexttar;
                TarStack = TarStack->nexttar = NewState;
                NewState->nexttar->prevtar = NewState;
                NewState->current = NewState->tarlevel = 0;
                NewState->parent = CurrentState;
                NewState->succ =  new State * [CardFireList + 1];
                CurrentState->succ[CurrentState->current] = NewState;
                CurrentState = NewState;
            }
        } else {
            // close state and return to previous state

            if (Globals::gmflg || Globals::GMflg) {
                (*graphstream) << "STATE " << CurrentState->dfs;
#ifdef LIMITCAPACITY
                (*graphstream) << " BAD: ";
                if (CurrentState->boundbroken)
                	(*graphstream) << "1";
                else
                	(*graphstream) << "0";
#endif
                (*graphstream) << " Lowlink: " << CurrentState->min;
            }

            if (CurrentState->dfs == CurrentState->min) {
                // unlink scc
                if (CurrentState != TarStack->nexttar) { // current != bottom(stack)
                    State* newroot;
                    newroot = CurrentState->prevtar;
                    newroot->nexttar = TarStack->nexttar;
                    TarStack->nexttar->prevtar = newroot;
                    TarStack->nexttar = CurrentState;
                    CurrentState->prevtar = TarStack;
                    TarStack = newroot;
                }
                State* s;

                bool nonTrivialSCC = false;

                // print out SCC
                for (s = CurrentState ; s->nexttar != CurrentState; s = s->nexttar) {
                    if (! nonTrivialSCC) {
                        if (Globals::gmflg || Globals::GMflg) {
                            (*graphstream) << " SCC:";
                        }
                    }
                    nonTrivialSCC = true;

                    if (Globals::gmflg || Globals::GMflg) {
                        (*graphstream) << " " << s->nexttar->dfs;
                    }

                    s->tarlevel = 1;
                }
                s->tarlevel = 1;
            }

            if (Globals::gmflg || Globals::GMflg) {
                // in the next line "&& gmflg" was added by Niels, because it was like this in the code I collected
                if (CurrentState->persistent && Globals::gmflg) {
                    (*graphstream) << " persistent ";
                }
                if (Globals::graphformat == 'm') {
                    int j = 0;
                    for (i = 0; i < Globals::Places[0]->cnt; ++i) {
                        if (Globals::CurrentMarking[i]) {
                            if (Globals::CurrentMarking[i] == VERYLARGE) {
                                (*graphstream) << (j++ ? ",\n" : "\n") << Globals::Places[i]->name << " : " << "oo" ;
                            } else {
                                (*graphstream) << (j++ ? ",\n" : "\n") << Globals::Places[i]->name << " : " << Globals::CurrentMarking[i];
                            }
                        }
                    }
                }
                (*graphstream) << "\n\n";
                for (i = 0; CurrentState->firelist[i]; ++i) {
                    (*graphstream) << CurrentState->firelist[i]->name << " -> " << CurrentState->succ[i]->dfs << "\n";
                }
                (*graphstream) << endl;
            }
            if (CurrentState->parent) {
                CurrentState->parent->min = MIN(CurrentState->min, CurrentState->parent->min);
            }

            CurrentState = CurrentState->parent;
//**     delete TmpState;
            if (CurrentState) {
                CurrentState->firelist[CurrentState->current]->backfire();
                ++(CurrentState->current);
            }
        }
    }

    if (graphstream != &cout) {
        delete graphstream;
    }

    statistics(NrOfStates, Edges, NonEmptyHash);
    return 0;
}
#endif


#endif
