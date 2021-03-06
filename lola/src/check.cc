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

#include "graph.h"
#include "dimensions.h"
#include "formula.h"
#include "net.h"
#include "stubborn.h"
#include "check.h"
#include "Globals.h"
#include "verbose.h"

#include <fstream>
#include <iostream>

using std::ofstream;

unsigned int NrStates = 0;
State* initial;

unsigned int* checkstart;
unsigned int formulaindex;


inline void report_statistics() {
    message("st: %10d edg: %10d", NrStates, Edges);
}


/*!
 \brief print a state

 This function prints a state (i.e., a marking of the Petri net) and is called
 to display a witness state (e.g., when checking for deadlocks whether the
 net is unbounded).

 \param c   name of the state (sometimes empty)
 \param st  a vector of tokens, indexed by places
*/
void printstate(char const* c, unsigned int* st) {
    // write state to result file
    if (Globals::resultfile) {
        fprintf(Globals::resultfile, "  state = ( ");
        for (unsigned int i = 0, j = 0; i < Globals::Places[0]->cnt; ++i) {
            if (st[i]) {
                if (st[i] == VERYLARGE) {
                    fprintf(Globals::resultfile, "%s(\"%s\", -1)", (j++ ? ", " : ""), Globals::Places[i]->name);
                } else {
                    fprintf(Globals::resultfile, "%s(\"%s\", %d)", (j++ ? ", " : ""), Globals::Places[i]->name, st[i]);
                }
            }
        }
        fprintf(Globals::resultfile, " );\n");
        return;
    }

    if (Globals::Sflg) {
        cout << "STATE";
        cout << c;
        for (unsigned int i = 0, j = 0; i < Globals::Places[0]->cnt; i++) {
            if (st[i]) {
                if (st[i] == VERYLARGE) {
                    cout << (j++ ? ",\n" : "\n") << Globals::Places[i]->name << " : oo" ;
                } else {
                    cout << (j++ ? ",\n" : "\n") << Globals::Places[i]->name << " : " << st[i];
                }
            }
        }
        cout << "\n" << endl;
        return;
    }

    if (Globals::sflg) {
        ofstream statestream(Globals::statefile);
        if (!statestream) {
            abort(4, "cannot open state output file '%s' - no output written", _cfilename_(Globals::statefile));
        }

        statestream << "STATE";
        for (unsigned int i = 0, j = 0; i < Globals::Places[0]->cnt; i++) {
            if (st[i]) {
                if (st[i] == VERYLARGE) {
                    statestream << (j++ ? ",\n" : "\n") << Globals::Places[i]->name << " : oo" ;
                } else {
                    statestream << (j++ ? ",\n" : "\n") << Globals::Places[i]->name << " : " << st[i];
                }
            }
        }
        statestream << "\n" << endl;
        return;
    }
}


#ifdef MODELCHECKING
extern unsigned int currentdfsnum;

#define MIN(X,Y) ( (X) < (Y) ? (X) : (Y))
#define MAX(X,Y) ( (X) > (Y) ? (X) : (Y))

void print_witness_eu(State*, untilformula*);
void print_witness_ef(State*, unarytemporalformula*);
void searchAF(State*, unarytemporalformula*);
void searchEG(State*, unarytemporalformula*);
void searchAU(State*, untilformula*);
void searchEU(State*, untilformula*);
void searchAG(State*, unarytemporalformula*);
void searchEF(State*, unarytemporalformula*);
void searchEX(State*, unarytemporalformula*);
void searchAX(State*, unarytemporalformula*);
void check(State* , formula*);

void print_witness_ex(State* s, unarytemporalformula* f, ofstream& out) {
    out << "PATH\n";
    if (s->witnesstransition[f->tempindex]) {
        out << s->witnesstransition[f->tempindex]->name << "\n";
    } else {
        out << " SELF LOOP AT TERMINAL STATE\n";
    }
}

void print_witness_ef(State* s, unarytemporalformula* f, ofstream& out) {
    out << "PATH\n";
    while (!s->value[f->element->index]) {
        out << s->witnesstransition[f->tempindex]->name << "\n";
        s = s->witness[f->tempindex];
    }
}

void print_witness_eg(State* s, unarytemporalformula* f, ofstream& out) {
    out  << "PATH\n";
    while (s->value[f->element->index]) {
        out << s->witnesstransition[f->tempindex]->name << "\n";
        s->value[f->element->index] = false;
        s = s->witness[f->tempindex];
    }
}

void print_counterexample_af(State* s, unarytemporalformula* f, ofstream& out) {
    out  << "PATH\n";
    while (!s->value[f->element->index]) {
        if (!(s->witness[f->tempindex])) {
            break;
        }
        out << s->witnesstransition[f->tempindex]->name << "\n";
        s->value[f->element->index] = true;
        s = s->witness[f->tempindex];
    }
}

void print_counterexample_au(State* s, untilformula* f, ofstream& out) {
    out  << "PATH\n";
    while (s->value[f->hold->index]) {
        out << s->witnesstransition[f->tempindex]->name << "\n";
        s->value[f->hold->index] = true;
        s = s->witness[f->tempindex];
    }
}

void print_witness_eu(State* s, untilformula* f, ofstream& out) {
    out  << "PATH\n";
    while (!s->value[f->goal->index]) {
        out << s->witnesstransition[f->tempindex]->name << "\n";
        s = s->witness[f->tempindex];
    }
}

void print_counterexample_ax(State* s, unarytemporalformula* f, ofstream& out) {
    out  << "PATH\n";
    if (s->witnesstransition[f->tempindex]) {
        out << s->witnesstransition[f->tempindex]->name << "\n";
    } else {
        out << "SELF LOOP AT TERMINAL STATE\n";
    }
}

void print_counterexample_ag(State* s, unarytemporalformula* f, ofstream& out) {
    out  << "PATH\n";
    while (s->value[f->element->index]) {
        out << s->witnesstransition[f->tempindex]->name << "\n";
        s = s->witness[f->tempindex];
    }
}

void print_witness_ex(State* s, unarytemporalformula* f) {
    cout << "PATH\n";
    if (s->witnesstransition[f->tempindex]) {
        cout << s->witnesstransition[f->tempindex]->name << "\n";
    } else {
        cout << " SELF LOOP AT TERMINAL STATE\n";
    }
}

void print_witness_ef(State* s, unarytemporalformula* f) {
    cout << "PATH\n";
    while (!s->value[f->element->index]) {
        cout << s->witnesstransition[f->tempindex]->name << "\n";
        s = s->witness[f->tempindex];
    }
}

void print_witness_eg(State* s, unarytemporalformula* f) {
    cout << "PATH\n";
    while (s->value[f->element->index]) {
        if (!(s->witnesstransition[f->tempindex])) {
            break;
        }
        cout << s->witnesstransition[f->tempindex]->name << "\n";
        s->value[f->element->index] = false;
        s = s->witness[f->tempindex];
    }
}

void print_counterexample_af(State* s, unarytemporalformula* f) {
    cout << "PATH\n";
    while (!s->value[f->element->index]) {
        if (!(s->witnesstransition[f->tempindex])) {
            break;
        }
        cout << s->witnesstransition[f->tempindex]->name << "\n";
        s->value[f->element->index] = true;
        s = s->witness[f->tempindex];
    }
}

void print_counterexample_au(State* s, untilformula* f) {
    cout << "PATH\n";
    while (s->value[f->hold->index]) {
        if (!(s->witnesstransition[f->tempindex])) {
            break;
        }
        cout << s->witnesstransition[f->tempindex]->name << "\n";
        s->value[f->hold->index] = true;
        s = s->witness[f->tempindex];
    }
}

void print_witness_eu(State* s, untilformula* f) {
    cout << "PATH\n";
    while (!s->value[f->goal->index]) {
        cout << s->witnesstransition[f->tempindex]->name << "\n";
        s = s->witness[f->tempindex];
    }
}

void print_counterexample_ax(State* s, unarytemporalformula* f) {
    cout << "PATH\n";
    if (s->witnesstransition[f->tempindex]) {
        cout << s->witnesstransition[f->tempindex]->name << "\n";
    } else {
        cout << "SELF LOOP AT TERMINAL STATE\n";
    }
}

void print_counterexample_ag(State* s, unarytemporalformula* f) {
    cout << "PATH\n";
    while (s->value[f->element->index]) {
        cout << s->witnesstransition[f->tempindex]->name << "\n";
        s = s->witness[f->tempindex];
    }
}

void print_check_path(State* ini, formula* form) {
    if ((form->type == ef) && ini->value[form->index]) {
        print_witness_ef(ini, static_cast<unarytemporalformula*>(form));
        return;
    }
    if ((form->type == ag) && !ini->value[form->index]) {
        print_counterexample_ag(ini, static_cast<unarytemporalformula*>(form));
        return;
    }
    if ((form->type == eg) && ini->value[form->index]) {
        print_witness_eg(ini, static_cast<unarytemporalformula*>(form));
        return;
    }
    if ((form->type == eu) && ini->value[form->index]) {
        print_witness_eu(ini, static_cast<untilformula*>(form));
        return;
    }
    if ((form->type == ex) && ini->value[form->index]) {
        print_witness_ex(ini, static_cast<unarytemporalformula*>(form));
        return;
    }
    if ((form->type == ax) && !ini->value[form->index]) {
        print_counterexample_ax(ini, static_cast<unarytemporalformula*>(form));
        return;
    }
    if ((form->type == af) && !ini->value[form->index]) {
        print_counterexample_af(ini, static_cast<unarytemporalformula*>(form));
        return;
    }
    if ((form->type == au) && !ini->value[form->index]) {
        print_counterexample_au(ini, static_cast<untilformula*>(form));
        return;
    }
    if (form->type != neg && form->type != conj && form->type != disj) {
        cout << "NO PATH\n";
        return;
    }
    if (form->type == neg) {
        print_check_path(ini, static_cast<unarybooleanformula*>(form)->sub);
        return;
    }
    if (form->type == conj) {
        unsigned int i;
        for (i = 0; i < static_cast<booleanformula*>(form)->cardsub; i++) {
            cout << "CONJ. " << i << ":\n";
            print_check_path(ini, static_cast<booleanformula*>(form)->sub[i]);
            if (!(ini->value[static_cast<booleanformula*>(form)->sub[i]->index])) {
                break;
            }
        }
        return;
    }
    if (form->type == disj) {
        unsigned int i;
        for (i = 0; i < static_cast<booleanformula*>(form)->cardsub; i++) {
            cout << "DISJ. " << i << ":\n";
            print_check_path(ini, static_cast<booleanformula*>(form)->sub[i]);
            if (ini->value[static_cast<booleanformula*>(form)->sub[i]->index]) {
                break;
            }
        }
        return;
    }
}

void print_check_path(State* ini, formula* form, ofstream& out) {
    if ((form->type == ef) && ini->value[form->index]) {
        print_witness_ef(ini, static_cast<unarytemporalformula*>(form), out);
        return;
    }
    if ((form->type == ag) && !ini->value[form->index]) {
        print_counterexample_ag(ini, static_cast<unarytemporalformula*>(form), out);
        return;
    }
    if ((form->type == eg) && ini->value[form->index]) {
        print_witness_eg(ini, static_cast<unarytemporalformula*>(form), out);
        return;
    }
    if ((form->type == eu) && ini->value[form->index]) {
        print_witness_eu(ini, static_cast<untilformula*>(form), out);
        return;
    }
    if ((form->type == ex) && ini->value[form->index]) {
        print_witness_ex(ini, static_cast<unarytemporalformula*>(form), out);
        return;
    }
    if ((form->type == ax) && !ini->value[form->index]) {
        print_counterexample_ax(ini, static_cast<unarytemporalformula*>(form), out);
        return;
    }
    if ((form->type == af) && !ini->value[form->index]) {
        print_counterexample_af(ini, static_cast<unarytemporalformula*>(form), out);
        return;
    }
    if ((form->type == au) && !ini->value[form->index]) {
        print_counterexample_au(ini, static_cast<untilformula*>(form), out);
        return;
    }
    if (form->type != neg && form->type != conj && form->type != disj) {
        out << "NO PATH\n";
        return;
    }
    if (form->type == neg) {
        print_check_path(ini, static_cast<unarybooleanformula*>(form)->sub, out);
        return;
    }
    if (form->type == conj) {
        unsigned int i;
        for (i = 0; i < static_cast<booleanformula*>(form)->cardsub; i++) {
            out << "CONJ. " << i << ":\n";
            print_check_path(ini, static_cast<booleanformula*>(form)->sub[i], out);
            if (!(ini->value[static_cast<booleanformula*>(form)->sub[i]->index])) {
                break;
            }
        }
        return;
    }
    if (form->type == disj) {
        unsigned int i;
        for (i = 0; i < static_cast<booleanformula*>(form)->cardsub; i++) {
            out << "DISJ. " << i << ":\n";
            print_check_path(ini, static_cast<booleanformula*>(form)->sub[i], out);
            if (ini->value[static_cast<booleanformula*>(form)->sub[i]->index]) {
                break;
            }
        }
        return;
    }
}

int modelcheck() {
    currentdfsnum = 1;
    for (unsigned int i = 0; i < Globals::Transitions[0]->cnt; i++) {
#ifdef EXTENDED
        for (unsigned int j = 0; j < F->tempcard; j++) {
            Globals::Transitions[i]->lstdisabled[j] = Globals::Transitions[i]->lstfired[j] = 0;
        }
#endif
        Globals::Transitions[i]->pathrestriction = new bool [F->tempcard];
        for (unsigned int j = 0; j < F->tempcard; j++) {
            Globals::Transitions[i]->pathrestriction[j] = false;
        }
    }
    DeadStatePathRestriction = new bool [F->tempcard];
    for (unsigned int j = 0; j < F->tempcard; j++) {
        DeadStatePathRestriction[j] = true;
    }


#ifdef WITHFORMULA
    for (unsigned int i = 0; i < Globals::Places[0]->cnt; i++) {
        Globals::Places[i]->propositions = new formula * [Globals::Places[i]->cardprop];
        Globals::Places[i]->cardprop = 0;
    }
#endif
    int res;
    F = F->reduce(& res);
    if (res < 2) {
        return 1 - res;
    }
    // remove "not"s in static subformulae
    F = F->posate();
    F->tempcard = 0;
    F->setstatic();
    cout << "\nFormula with \n" << F->card << " subformulas\nand " << F->tempcard << " temporal operators.\n";
    initial = binInsert();
    F->evaluateatomic(initial);
    checkstart = new unsigned int [ F->card ];
    for (unsigned int i = 0; i < F->card; i++) {
        checkstart[i] = 0;
    }
    check(initial, F);
    cout << "\nresult: " << (initial->value[F->index] ? "true" : "false") << "\n";
    statistics(NrStates + 1, Edges, NonEmptyHash);
    if (Globals::pflg) {
        ofstream pathstream(Globals::pathfile);
        if (!pathstream) {
            abort(4, "cannot open path output file '%s' - no output written", _cfilename_(Globals::pathfile));
        } else {
            print_check_path(initial, F, pathstream);
        }
    }
    if (Globals::Pflg) {
        print_check_path(initial, F);
    }
    return initial->value[F->index] ? 0 : 1;
}

bool check_analyse_fairness(State* pool, unsigned int level, unsigned int findex) {
    // 1. cut the (not necessarily connected) graph in pool into sccs.
    //    All states in search space have checktarlevel[findex] = level && ! checkexpired[findex].
    //    Before return "false", all states in pool must be "checkexpired[findex]".

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
        pool = pool->checknexttar[findex];
        if (pool == C) {
            pool = NULL;
        }
        T = C;
        // unlink from pool and init new dfs
        C->checknexttar[findex]->checkprevtar[findex] = C->checkprevtar[findex];
        C->checkprevtar[findex]->checknexttar[findex] = C->checknexttar[findex];
        C->checknexttar[findex] = C->checkprevtar[findex] = C;
        C->checkcurrent[findex] = 0;
        while (C->checkfirelist[findex][C->checkcurrent[findex]] && ! C->checksucc[findex][C->checkcurrent[findex]]) {
            C->checkcurrent[findex]++;
        }
        C->checktarlevel[findex] = level + 1;
        C->checkparent[findex] = NULL;
        C->checkddfs[findex] = C->checkmmin[findex] = 1;
        while (C) {
            if (C->checkfirelist[findex][C->checkcurrent[findex]]) {
                N = C->checksucc[findex][C->checkcurrent[findex]];
                if (N->checktarlevel[findex] == level && ! N->checkexpired[findex]) {
                    // new state
                    N->checkmmin[findex] = N->checkddfs[findex] = ++card;
                    N->checkcurrent[findex] = 0 ;
                    while (N->checkfirelist[findex][N->checkcurrent[findex]] && ! N->checksucc[findex][N->checkcurrent[findex]]) {
                        N->checkcurrent[findex]++;
                    }
                    N->checkparent[findex] = C;
                    N->checktarlevel[findex] = level + 1;
                    // put state on local tarjan stack,
                    // and remove it from pool
                    if (N == pool) {
                        pool = pool->checknexttar[findex];
                        if (pool == N) {
                            pool = NULL;
                        }
                    }
                    N->checknexttar[findex]->checkprevtar[findex] = N->checkprevtar[findex];
                    N->checkprevtar[findex]->checknexttar[findex] = N->checknexttar[findex];
                    if (T) {
                        N->checknexttar[findex] = T->checknexttar[findex];
                        T->checknexttar[findex] = N;
                        N->checkprevtar[findex] = T;
                        N->checknexttar[findex]->checkprevtar[findex] = N;
                    } else {
                        N->checknexttar[findex] = N->checkprevtar[findex] = N;
                    }
                    T = N;
                    C = N;
                } else {
                    if (N->checktarlevel[findex] == level + 1 && ! N->checkexpired[findex]) {
                        // existing state in same scc
                        C->checkmmin[findex] = MIN(N->checkddfs[findex], C->checkmmin[findex]);
                        C->checkcurrent[findex]++;
                        while (C->checkfirelist[findex][C->checkcurrent[findex]] && ! C->checksucc[findex][C->checkcurrent[findex]]) {
                            C->checkcurrent[findex]++;
                        }
                    } else {
                        // existing state in checkexpired[findex] scc or outside
                        // current search space
                        C->checkcurrent[findex]++;
                        while (C->checkfirelist[findex][C->checkcurrent[findex]] && ! C->checksucc[findex][C->checkcurrent[findex]]) {
                            C->checkcurrent[findex]++;
                        }
                    }
                }
            } else {
                // close state
                // A) check scc
                if (C->checkmmin[findex] == C->checkddfs[findex]) {
                    // data of C must be preserved for proper backtracking
                    // after having finished this scc
                    O = C->checkparent[findex];
                    oldc = C->checkcurrent[findex];
                    oldd = C->checkddfs[findex];
                    oldm = C->checkmmin[findex];
                    // B) process scc
                    // B0) unlink new component from local tarjan stack
                    if (C->checkddfs[findex] > 1) { // proper cut
                        C->checkprevtar[findex]->checknexttar[findex] = T->checknexttar[findex];
                        T->checknexttar[findex]->checkprevtar[findex] = C->checkprevtar[findex];
                        T->checknexttar[findex] = C;
                        S = C->checkprevtar[findex];
                        C->checkprevtar[findex] = T;
                        T = S;
                    }
                    S = C;

                    // B1) check fairness of new scc
                    State* ss;
#ifdef STABLEPROP
                    unsigned int cardphi = 0;
#endif
                    unsigned int cardS;
                    unsigned int i;
                    for (i = 0; i < Globals::Transitions[0]->cnt; i++) {
                        Globals::Transitions[i]->faired = Globals::Transitions[i]->fairabled = 0;
                    }
                    for (cardS = 1, ss = S->checknexttar[findex];; cardS++, ss = ss->checknexttar[findex]) {
                        for (i = 0; ss->checkfirelist[findex][i]; i++) {
                            ss->checkfirelist[findex][i]->fairabled ++;
                            if ((ss->checksucc[findex][i]->checktarlevel[findex] == level + 1) && !(ss->checksucc[findex][i]->checkexpired[findex])) {
                                ss->checkfirelist[findex][i]->faired ++;
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
                                    for (j = 0; S->checkfirelist[findex][j]; j++) {
                                        if (S->checkfirelist[findex][j] == Globals::Transitions[i]) {
                                            E = S;
                                            break;
                                        }
                                    }
                                    S = S->checknexttar[findex];
                                    if (E) {
                                        E->checkexpired[findex] = true;
                                        E->checknexttar[findex]->checkprevtar[findex] = E->checkprevtar[findex];
                                        E->checkprevtar[findex]->checknexttar[findex] = E->checknexttar[findex];
                                        Globals::Transitions[i]->fairabled--;
                                    }
                                }
                                if (check_analyse_fairness(S, level + 1, findex)) {
                                    return true;
                                }
                                goto aftercheck;
                            }
                        }
                        Globals::Transitions[i]->faired = Globals::Transitions[i]->fairabled = 0;
                    }
                    return true; // arrived here only if all transitions have paased fairness test.

aftercheck:
                    for (; !(S->checkexpired[findex]); S = S->checknexttar[findex]) {
                        S->checkexpired[findex] = true;
                    }
                    C->checkparent[findex] = O;
                    C->checkddfs[findex] = oldd;
                    C->checkmmin[findex] = oldm;
                    C->checkcurrent[findex] = oldc;

                }
                // end process scc
                // C) return to previous state
                N = C;
                C = C->checkparent[findex];
                if (C) {
                    C->checksucc[findex][C->checkcurrent[findex]] = N;
                    C->checkmmin[findex] = MIN(C->checkmmin[findex], N->checkmmin[findex]);
                    C->checkcurrent[findex]++;
                    while (C->checkfirelist[findex][C->checkcurrent[findex]] && ! C->checksucc[findex][C->checkcurrent[findex]]) {
                        C->checkcurrent[findex]++;
                    }
                }
            } // end for all new scc
        } // end for all states
    } // end, for all nodes in pool
    return false;
} // end analyse_fairness

void check(State* s, formula* f) {
    unsigned int i;
    formulaindex = f->index;
    if (s->known[f->index]) {
        return;
    }
    switch (f->type) {
        case neg:
            check(s, static_cast<unarybooleanformula*>(f)->sub);
            if (s->value[static_cast<unarybooleanformula*>(f)->sub->index]) {
                s->value[f->index] = false;
            } else {
                s->value[f->index] = true;
            }
            s->known[f->index] = true;
            return;

        case conj:
            checkstart[f->index]++;
            checkstart[f->index] %= static_cast<booleanformula*>(f)->cardsub;
            for (i = checkstart[f->index];;) {
                check(s, static_cast<booleanformula*>(f)->sub[i]);
                if (!(s->value[static_cast<booleanformula*>(f)->sub[i]->index])) {
                    s->value[f->index] = false;
                    s->known[f->index] = true;
                    return;
                }
                i++;
                i %= static_cast<booleanformula*>(f)->cardsub ;
                if (i == checkstart[f->index]) {
                    break;
                }
            }
            s->value[f->index] = true;
            s->known[f->index] = true;
            return;

        case disj:
            checkstart[f->index]++;
            checkstart[f->index] %= static_cast<booleanformula*>(f)->cardsub;
            for (i = checkstart[f->index];;) {

                check(s, static_cast<booleanformula*>(f)->sub[i]);
                if (s->value[static_cast<booleanformula*>(f)->sub[i]->index]) {
                    s->value[f->index] = true;
                    s->known[f->index] = true;
                    return;
                }
                i++;
                i %= static_cast<booleanformula*>(f)->cardsub ;
                if (i == checkstart[f->index]) {
                    break;
                }
            }
            s->value[f->index] = false;
            s->known[f->index] = true;
            return;

        case ef:
            check(s, static_cast<unarytemporalformula*>(f)->element);
            if (s->value[static_cast<unarytemporalformula*>(f)->element->index]) {
                s->value[f->index] = s->known[f->index] = true;
                s->witness[f->tempindex] = NULL;
                s->witnesstransition[f->tempindex] = NULL;
                return;
            }
            searchEF(s, static_cast<unarytemporalformula*>(f));
            return;

        case ag:
            check(s, static_cast<unarytemporalformula*>(f)->element);
            if (!s->value[static_cast<unarytemporalformula*>(f)->element->index]) {
                s->value[f->index] = false;
                s->known[f->index] = true;
                s->witness[f->tempindex] = NULL;
                s->witnesstransition[f->tempindex] = NULL;
                return;
            }
            searchAG(s, static_cast<unarytemporalformula*>(f));
            return;

        case eu:
            check(s, static_cast<untilformula*>(f)->goal);
            if (s->value[static_cast<untilformula*>(f)->goal->index]) {
                s->value[f->index] = true;
                s->known[f->index] = true;
                s->witness[f->tempindex] = NULL;
                s->witnesstransition[f->tempindex] = NULL;
                return;
            }
            check(s, static_cast<untilformula*>(f)->hold);
            if (!s->value[static_cast<untilformula*>(f)->hold->index]) {
                s->value[f->index] = false;
                s->known[f->index] = true;
                return;
            }
            searchEU(s, static_cast<untilformula*>(f));
            return;

        case eg:
            check(s, static_cast<unarytemporalformula*>(f)->element);
            if (!s->value[static_cast<unarytemporalformula*>(f)->element->index]) {
                s->value[f->index] = false;
                s->known[f->index] = true;
                return;
            }
            searchEG(s, static_cast<unarytemporalformula*>(f));
            return;

        case af:
            check(s, static_cast<unarytemporalformula*>(f)->element);
            if (s->value[static_cast<unarytemporalformula*>(f)->element->index]) {
                s->value[f->index] = true;
                s->known[f->index] = true;
                return;
            }
            searchAF(s, static_cast<unarytemporalformula*>(f));
            return;

        case au:
            check(s, static_cast<untilformula*>(f)->goal);
            if (s->value[static_cast<untilformula*>(f)->goal->index]) {
                s->value[f->index] = true;
                s->known[f->index] = true;
                return;
            }
            check(s, static_cast<untilformula*>(f)->hold);
            if (!s->value[static_cast<untilformula*>(f)->hold->index]) {
                s->value[f->index] = false;
                s->known[f->index] = true;
                s->witness[f->tempindex] = NULL;
                s->witnesstransition[f->tempindex] = NULL;
                return;
            }
            searchAU(s, static_cast<untilformula*>(f));
            return;

        case ex:
            searchEX(s, static_cast<unarytemporalformula*>(f));
            return;

        case ax:
            searchAX(s, static_cast<unarytemporalformula*>(f));
            return;

        default:
            cout << "temporal operator not supported\n";
    }
}

void searchEF(State* s, unarytemporalformula* f) {
    State* tarjanroot;
    bool found;
    unsigned int cardstates;
    State* CurrentState, * NewState;

#ifdef EXTENDEDCTL
    TemporalIndex = f->tempindex;
#endif
    found = false;
    cardstates = 1;
    CurrentState = s;
    tarjanroot = s;
    // End of Stack = Self reference! Not on stack->next = nil
    CurrentState->checknexttar[f->tempindex] = CurrentState;
    CurrentState->checkcurrent[f->tempindex] = 0;
    CurrentState->checkfirelist[f->tempindex] =
#ifdef STUBBORN
        stubbornfirelist(s, f->element);
#else
        firelist();
#endif
    CurrentState->checkparent[f->tempindex] = NULL;
    CurrentState->checkdfs[f->tempindex] = CurrentState->checkmin[f->tempindex] = 0;

    // process marking until returning from initial state

    while (CurrentState) {
        CurrentState->value[f->index] = false;
        CurrentState->known[f->index] = true;
        if (CurrentState->checkfirelist[f->tempindex][CurrentState->checkcurrent[f->tempindex]]) {
            // there is a next state that needs to be explored
            CurrentState->checkfirelist[f->tempindex][CurrentState->checkcurrent[f->tempindex]]->fire();
            if ((NewState = binSearch())) {
                Edges ++;
                if (!(Edges % REPORTFREQUENCY)) {
                    report_statistics();
                }
                if (NewState->known[f->index]) {
                    if (NewState->value[f->index]) {
                        // Value of new state known, true
                        //Witness for EF phi found!
                        found = true;
                        CurrentState->value[f->index] = CurrentState->known[f->index] = true;
                        CurrentState->witness[f->tempindex] = NewState;
                        CurrentState->witnesstransition[f->tempindex] = CurrentState->checkfirelist[f->tempindex][CurrentState->checkcurrent[f->tempindex]];
                        CurrentState->checkfirelist[f->tempindex][CurrentState->checkcurrent[f->tempindex]]->backfire();
                        break;
                    } else {
                        // Value of new state known, false
                        // NewState either on Stack or does not model EF phi
                        if (NewState->checknexttar[f->tempindex]) {
                            // on Stack: set min, update witness
                            if (CurrentState->checkmin[f->tempindex] > NewState->checkdfs[f->tempindex]) {
                                CurrentState->checkmin[f->tempindex] = NewState->checkdfs[f->tempindex];
                                CurrentState->witness[f->tempindex] = NewState;
                                CurrentState->witnesstransition[f->tempindex] = CurrentState->checkfirelist[f->tempindex][CurrentState->checkcurrent[f->tempindex]];
                            }
                        }
                        CurrentState->checkfirelist[f->tempindex][CurrentState->checkcurrent[f->tempindex]]->backfire();
                        CurrentState->checkcurrent[f->tempindex]++;
                    }
                } else {
                    // value of new state unknown
                    check(NewState, f->element);
                    if (NewState->value[f->element->index]) {
                        NewState->value[f->index] = true;
                        NewState->known[f->index] = true;
                        CurrentState->value[f->index] = true;
                        CurrentState->known[f->index] = true;
                        found = true;
                        CurrentState->checkfirelist[f->tempindex][CurrentState->checkcurrent[f->tempindex]]->backfire();
                        CurrentState->witness[f->tempindex] = NewState;
                        CurrentState->witnesstransition[f->tempindex] = CurrentState->checkfirelist[f->tempindex][CurrentState->checkcurrent[f->tempindex]];
                        break;
                    } else {
                        CurrentState->value[f->index] = false;
                        CurrentState->known[f->index] = true;
                        NewState->checkparent[f->tempindex] = CurrentState;
#ifdef EXTENDEDCTL
                        TemporalIndex = f->tempindex;
#endif
                        NewState->checkfirelist[f->tempindex] =
#ifdef STUBBORN
                            stubbornfirelist(NewState, f->element);
#else
                            firelist();
#endif
                        NewState->checkdfs[f->tempindex] = NewState->checkmin[f->tempindex] = cardstates++;
                        NewState->checkcurrent[f->tempindex] = 0;
                        NewState->checknexttar[f->tempindex] = tarjanroot;
                        tarjanroot = NewState;
                        CurrentState = NewState;
                    }
                }
            } else {
                NewState = binInsert();
                NewState->checknexttar[f->tempindex] = tarjanroot;
                tarjanroot = NewState;
                NrStates ++;

#ifdef MAXIMALSTATES
                checkMaximalStates(NrStates); ///// LINE ADDED BY NIELS
#endif

                Edges ++;
                if (!(Edges % REPORTFREQUENCY)) {
                    report_statistics();
                }
                NewState->checkcurrent[f->tempindex] = 0;
                NewState->checkparent[f->tempindex] = CurrentState;
                NewState->checkdfs[f->tempindex] = NewState->checkmin[f->tempindex] = cardstates++;
                check(NewState, f->element);
                if (NewState->value[f->element->index]) {
                    NewState->value[f->index] = true;
                    NewState->known[f->index] = true;
                    CurrentState->value[f->index] = true;
                    CurrentState->known[f->index] = true;
                    found = true;
                    CurrentState->witness[f->tempindex] = NewState;
                    CurrentState->witnesstransition[f->tempindex] = CurrentState->checkfirelist[f->tempindex][CurrentState->checkcurrent[f->tempindex]];
                    CurrentState->checkfirelist[f->tempindex][CurrentState->checkcurrent[f->tempindex]]->backfire();
                    break;
                } else {
#ifdef EXTENDEDCTL
                    TemporalIndex = f->tempindex;
#endif
                    NewState->checkfirelist[f->tempindex] =
#ifdef STUBBORN
                        stubbornfirelist(NewState, f->element);
#else
                        firelist();
#endif
                    NewState->checkcurrent[f->tempindex] = 0;
                    CurrentState->value[f->index] = false;
                    CurrentState->known[f->index] = true;
                    CurrentState = NewState;
                }
            }
        } else {
            // test scc, update checknexttar, return to previous state
            if (CurrentState->checkmin[f->tempindex] == CurrentState->checkdfs[f->tempindex]) {
                // scc closed
                CurrentState->value[f->index] = false;
                CurrentState->known[f->index] = true;
                while ((tarjanroot->checkdfs[f->tempindex] >= CurrentState->checkdfs[f->tempindex])) {
                    State* tmp = tarjanroot;
                    tarjanroot->witness[f->tempindex] = NULL;
                    if (tarjanroot == tarjanroot->checknexttar[f->tempindex]) {
                        tarjanroot->checknexttar[f->tempindex] = NULL;
                        break;
                    }
                    tarjanroot = tarjanroot->checknexttar[f->tempindex];
                    tmp->checknexttar[f->tempindex] = NULL;
                }
            }
            // return to previous state
            if (CurrentState->checkparent[f->tempindex]) {
                if (CurrentState->checkparent[f->tempindex]->checkmin[f->tempindex] > CurrentState->checkmin[f->tempindex]) {
                    CurrentState->checkparent[f->tempindex]->checkmin[f->tempindex] = CurrentState->checkmin[f->tempindex];
                    CurrentState->checkparent[f->tempindex]->witness[f->tempindex] = CurrentState;
                    CurrentState->checkparent[f->tempindex]->witnesstransition[f->tempindex] = CurrentState->checkparent[f->tempindex]->checkfirelist[f->tempindex][ CurrentState->checkparent[f->tempindex]->checkcurrent[f->tempindex]];
                }
                CurrentState = CurrentState->checkparent[f->tempindex];
                CurrentState->checkfirelist[f->tempindex][CurrentState->checkcurrent[f->tempindex]]->backfire();
                CurrentState->checkcurrent[f->tempindex] ++;
            } else {
                CurrentState = NULL;
            }
        }
    }
    if (found) {
        while (CurrentState->checkparent[f->tempindex]) {
            CurrentState->checkparent[f->tempindex]->witness[f->tempindex] = CurrentState;
            CurrentState->checkparent[f->tempindex]->witnesstransition[f->tempindex] = CurrentState->checkparent[f->tempindex]->checkfirelist[f->tempindex][CurrentState->checkparent[f->tempindex]->checkcurrent[f->tempindex]];

            CurrentState = CurrentState->checkparent[f->tempindex];
            CurrentState->value[f->index] = true;
            CurrentState->checkfirelist[f->tempindex][CurrentState->checkcurrent[f->tempindex]]->backfire();
        }
        while (true) {
            State* tmp = tarjanroot;
            tarjanroot->value[f->index] = true;
            if (tarjanroot == tarjanroot->checknexttar[f->tempindex]) {
                tarjanroot->checknexttar[f->tempindex] = NULL;
                break;
            }
            tarjanroot = tarjanroot->checknexttar[f->tempindex];
            tmp->checknexttar[f->tempindex] = NULL;
        }
    }
}

void searchAX(State* s, unarytemporalformula* f) {
    State* NewState;

#ifdef EXTENDEDCTL
    TemporalIndex = f->tempindex;
#endif
    s->checkfirelist[f->tempindex] = firelist();
    s->value[f->index] = true;
    s->known[f->index] = true;

    if (!(s->checkfirelist[f->tempindex][0])) {
        if (Globals::Transitions[0]->NrEnabled) {
            // no tau-successors, but state not dead
            return;
        }
        // dead state: this state counts as successor state
        if (DeadStatePathRestriction[f->tempindex]) {
            check(s, f->element);
            if (!(s->value[f->element->index])) {
                s->value[f->index] = false;
                s->witness[f->tempindex] = NULL;
                s->witnesstransition[f->tempindex] = NULL;
            }
        }
        return;
    }
    s->checkcurrent[f->tempindex] = 0;
    do {
        // there is a next state that needs to be explored
        s->checkfirelist[f->tempindex][s->checkcurrent[f->tempindex]]->fire();
        if (!(NewState = binSearch())) {
            NewState = binInsert();
            NrStates ++;
#ifdef MAXIMALSTATES
            checkMaximalStates(NrStates); ///// LINE ADDED BY NIELS
#endif
        }
        Edges ++;
        if (!(Edges % REPORTFREQUENCY)) {
            report_statistics();
        }
        check(NewState, f->element);
        if (!(NewState->value[f->element->index])) {
            s->value[f->index] = false;
            s->witness[f->tempindex] = NewState;
            s->witnesstransition[f->tempindex] = s->checkfirelist[f->tempindex][s->checkcurrent[f->tempindex]];
            s->checkfirelist[f->tempindex][s->checkcurrent[f->tempindex]]->backfire();
            return;
        }
        // return to previous state
        s->checkfirelist[f->tempindex][s->checkcurrent[f->tempindex]]->backfire();
        s->checkcurrent[f->tempindex] ++;
    } while (s->checkfirelist[f->tempindex][s->checkcurrent[f->tempindex]]);
}

void searchEX(State* s, unarytemporalformula* f) {
    State* NewState;

#ifdef EXTENDEDCTL
    TemporalIndex = f->tempindex;
#endif
    s->checkfirelist[f->tempindex] = firelist();
    s->value[f->index] = false;
    s->known[f->index] = true;

    if (!(s->checkfirelist[f->tempindex][0])) {
        if (Globals::Transitions[0]->NrEnabled) {
            // no tau-successors, but state not dead
            return;
        }
        // dead state: this state counts as successor state
        if (DeadStatePathRestriction[f->tempindex]) {
            check(s, f->element);
            if (s->value[f->element->index]) {
                s->value[f->index] = true;
                s->witness[f->tempindex] = NULL;
                s->witnesstransition[f->tempindex] = NULL;
            }
        }
        return;
    }
    s->checkcurrent[f->tempindex] = 0;
    do {
        // there is a next state that needs to be explored
        s->checkfirelist[f->tempindex][s->checkcurrent[f->tempindex]]->fire();
        if (!(NewState = binSearch())) {
            NewState = binInsert();
            NrStates ++;
#ifdef MAXIMALSTATES
            checkMaximalStates(NrStates); ///// LINE ADDED BY NIELS
#endif
        }
        Edges ++;
        if (!(Edges % REPORTFREQUENCY)) {
            report_statistics();
        }
        check(NewState, f->element);
        if (NewState->value[f->element->index]) {
            s->value[f->index] = true;
            s->witness[f->tempindex] = NewState;
            s->witnesstransition[f->tempindex] = s->checkfirelist[f->tempindex][s->checkcurrent[f->tempindex]];
            s->checkfirelist[f->tempindex][s->checkcurrent[f->tempindex]]->backfire();
            return;
        }
        // return to previous state
        s->checkfirelist[f->tempindex][s->checkcurrent[f->tempindex]]->backfire();
        s->checkcurrent[f->tempindex] ++;
    } while (s->checkfirelist[f->tempindex][s->checkcurrent[f->tempindex]]);
}

void searchAG(State* s, unarytemporalformula* f) {
    State* tarjanroot;
    State* NewState;

    bool found = false;
    unsigned int cardstates = 1;
    State* CurrentState = s;
    tarjanroot = s;
    // End of Stack = Self reference! Not on stack->next = nil
    CurrentState->checknexttar[f->tempindex] = CurrentState;
    CurrentState->checkcurrent[f->tempindex] = 0;
#ifdef EXTENDEDCTL
    TemporalIndex = f->tempindex;
#endif
    CurrentState->checkfirelist[f->tempindex] =
#ifdef STUBBORN
        stubbornfirelistneg(s, f->element);
#else
        firelist();
#endif
    CurrentState->checkparent[f->tempindex] = NULL;
    CurrentState->checkdfs[f->tempindex] = CurrentState->checkmin[f->tempindex] = 0;

    // process marking until returning from initial state

    while (CurrentState) {
        CurrentState->value[f->index] = true;
        CurrentState->known[f->index] = true;
        if (CurrentState->checkfirelist[f->tempindex][CurrentState->checkcurrent[f->tempindex]]) {
            // there is a next state that needs to be explored
            CurrentState->checkfirelist[f->tempindex][CurrentState->checkcurrent[f->tempindex]]->fire();
            if ((NewState = binSearch())) {
                Edges ++;
                if (!(Edges % REPORTFREQUENCY)) {
                    report_statistics();
                }
                if (NewState->known[f->index]) {
                    if (!(NewState->value[f->index])) {
                        // Value of new state known, true
                        //Counterexample for AG phi found!
                        found = true;
                        CurrentState->value[f->index] = false;
                        CurrentState->witness[f->tempindex] = NewState;
                        CurrentState->witnesstransition[f->tempindex] = CurrentState->checkfirelist[f->tempindex][CurrentState->checkcurrent[f->tempindex]];
                        CurrentState->checkfirelist[f->tempindex][CurrentState->checkcurrent[f->tempindex]]->backfire();
                        break;
                    } else {
                        // Value of new state known, false
                        // NewState either on Stack or does not model EF phi
                        if (NewState->checknexttar[f->tempindex]) {
                            // on Stack: set min, update witness
                            if (CurrentState->checkmin[f->tempindex] > NewState->checkdfs[f->tempindex]) {
                                CurrentState->checkmin[f->tempindex] = NewState->checkdfs[f->tempindex];
                                CurrentState->witness[f->tempindex] = NewState;
                                CurrentState->witnesstransition[f->tempindex] = CurrentState->checkfirelist[f->tempindex][CurrentState->checkcurrent[f->tempindex]];
                            }
                        }
                        CurrentState->checkfirelist[f->tempindex][CurrentState->checkcurrent[f->tempindex]]->backfire();
                        CurrentState->checkcurrent[f->tempindex]++;
                    }
                } else {
                    // value of new state unknown
                    check(NewState, f->element);
                    if (!(NewState->value[f->element->index])) {
                        NewState->value[f->index] = false;
                        NewState->known[f->index] = true;
                        CurrentState->value[f->index] = false;
                        CurrentState->known[f->index] = true;
                        found = true;
                        CurrentState->checkfirelist[f->tempindex][CurrentState->checkcurrent[f->tempindex]]->backfire();
                        CurrentState->witness[f->tempindex] = NewState;
                        CurrentState->witnesstransition[f->tempindex] = CurrentState->checkfirelist[f->tempindex][CurrentState->checkcurrent[f->tempindex]];
                        break;
                    } else {
                        CurrentState->value[f->index] = true;
                        CurrentState->known[f->index] = true;
                        NewState->checkparent[f->tempindex] = CurrentState;
#ifdef EXTENDEDCTL
                        TemporalIndex = f->tempindex;
#endif
                        NewState->checkfirelist[f->tempindex] =
#ifdef STUBBORN
                            stubbornfirelistneg(NewState, f->element);
#else
                            firelist();
#endif
                        NewState->checkdfs[f->tempindex] = NewState->checkmin[f->tempindex] = cardstates++;
                        NewState->checkcurrent[f->tempindex] = 0;
                        NewState->checknexttar[f->tempindex] = tarjanroot;
                        tarjanroot = NewState;
                        CurrentState = NewState;
                    }
                }
            } else {
                NewState = binInsert();
                NewState->checknexttar[f->tempindex] = tarjanroot;
                tarjanroot = NewState;
                NrStates ++;
#ifdef MAXIMALSTATES
                checkMaximalStates(NrStates); ///// LINE ADDED BY NIELS
#endif
                Edges ++;
                if (!(Edges % REPORTFREQUENCY)) {
                    report_statistics();
                }
                NewState->checkcurrent[f->tempindex] = 0;
                NewState->checkparent[f->tempindex] = CurrentState;
                NewState->checkdfs[f->tempindex] = NewState->checkmin[f->tempindex] = cardstates++;
                check(NewState, f->element);
                if (!(NewState->value[f->element->index])) {
                    NewState->value[f->index] = false;
                    NewState->known[f->index] = true;
                    CurrentState->value[f->index] = false;
                    CurrentState->known[f->index] = true;
                    found = true;
                    CurrentState->witness[f->tempindex] = NewState;
                    CurrentState->witnesstransition[f->tempindex] = CurrentState->checkfirelist[f->tempindex][CurrentState->checkcurrent[f->tempindex]];
                    CurrentState->checkfirelist[f->tempindex][CurrentState->checkcurrent[f->tempindex]]->backfire();
                    break;
                } else {
#ifdef EXTENDEDCTL
                    TemporalIndex = f->tempindex;
#endif
                    NewState->checkfirelist[f->tempindex] =
#ifdef STUBBORN
                        stubbornfirelistneg(NewState, f->element);
#else
                        firelist();
#endif
                    NewState->checkcurrent[f->tempindex] = 0;
                    CurrentState->value[f->index] = true;
                    CurrentState->known[f->index] = true;
                    CurrentState = NewState;
                }
            }
        } else {
            // test scc, update checknexttar, return to previous state
            if (CurrentState->checkmin[f->tempindex] == CurrentState->checkdfs[f->tempindex]) {
                // scc closed
                CurrentState->value[f->index] = true;
                CurrentState->known[f->index] = true;
                while ((tarjanroot->checkdfs[f->tempindex] >= CurrentState->checkdfs[f->tempindex])) {
                    State* tmp;
                    tmp = tarjanroot;
                    tarjanroot->witness[f->tempindex] = NULL;
                    if (tarjanroot == tarjanroot->checknexttar[f->tempindex]) {
                        tarjanroot->checknexttar[f->tempindex] = NULL;
                        break;
                    }
                    tarjanroot = tarjanroot->checknexttar[f->tempindex];
                    tmp->checknexttar[f->tempindex] = NULL;
                }
            }
            // return to previous state
            if (CurrentState->checkparent[f->tempindex]) {
                if (CurrentState->checkparent[f->tempindex]->checkmin[f->tempindex] > CurrentState->checkmin[f->tempindex]) {
                    CurrentState->checkparent[f->tempindex]->checkmin[f->tempindex] = CurrentState->checkmin[f->tempindex];
                    CurrentState->checkparent[f->tempindex]->witness[f->tempindex] = CurrentState;
                    CurrentState->checkparent[f->tempindex]->witnesstransition[f->tempindex] = CurrentState->checkparent[f->tempindex]->checkfirelist[f->tempindex][ CurrentState->checkparent[f->tempindex]->checkcurrent[f->tempindex]];
                }
                CurrentState = CurrentState->checkparent[f->tempindex];
                CurrentState->checkfirelist[f->tempindex][CurrentState->checkcurrent[f->tempindex]]->backfire();
                CurrentState->checkcurrent[f->tempindex] ++;
            } else {
                CurrentState = NULL;
            }
        }
    }
    if (found) {
        while (CurrentState->checkparent[f->tempindex]) {
            CurrentState->checkparent[f->tempindex]->witness[f->tempindex] = CurrentState;
            CurrentState->checkparent[f->tempindex]->witnesstransition[f->tempindex] = CurrentState->checkparent[f->tempindex]->checkfirelist[f->tempindex][CurrentState->checkparent[f->tempindex]->checkcurrent[f->tempindex]];

            CurrentState = CurrentState->checkparent[f->tempindex];
            CurrentState->value[f->index] = false;
            CurrentState->checkfirelist[f->tempindex][CurrentState->checkcurrent[f->tempindex]]->backfire();
        }
        while (true) {
            State* tmp = tarjanroot;
            tarjanroot->value[f->index] = false;
            if (tarjanroot == tarjanroot->checknexttar[f->tempindex]) {
                tarjanroot->checknexttar[f->tempindex] = NULL;
                break;
            }
            tarjanroot = tarjanroot->checknexttar[f->tempindex];
            tmp->checknexttar[f->tempindex] = NULL;
        }
    }
}

void searchEU(State* s, untilformula* f) {
    State* tarjanroot;
    State* NewState;

    bool found = false;
    State* CurrentState = s;
    s->checknexttar[f->tempindex] = s;
    tarjanroot = s; // End of Stack is recognised as self-loop
    // Thus, states not on stack can be detected through
    // nil-pointer in checknexttar
#ifdef EXTENDEDCTL
    TemporalIndex = f->tempindex;
#endif
    s->checkfirelist[f->tempindex] =
#ifdef STUBBORN
        stubbornfirelistctl();
#else
        firelist();
#endif
    s->checkparent[f->tempindex] = NULL;
    s->checkdfs[f->tempindex] = s->checkmin[f->tempindex] = currentdfsnum;
    currentdfsnum++;
    s->checkcurrent[f->tempindex] = 0;

    while (CurrentState) {
        CurrentState->value[f->index] = false;
        CurrentState->known[f->index] = true;

        unsigned int MinBookmark = 0;
        if (CurrentState->checkfirelist[f->tempindex][CurrentState->checkcurrent[f->tempindex]]) {
            // explore next state
            CurrentState->checkfirelist[f->tempindex][CurrentState->checkcurrent[f->tempindex]]->fire();
            Edges ++;
            if (!(Edges % REPORTFREQUENCY)) {
                report_statistics();
            }
            if ((NewState = binSearch())) {
                // state exists
                if (NewState->known[f->index]) {
                    // state exists, value known
                    if (NewState->value[f->index]) {
                        // state exists, value true -->witness found!
                        found = true;
                        CurrentState->witness[f->tempindex] = NewState;
                        CurrentState->witnesstransition[f->tempindex] = CurrentState->checkfirelist[f->tempindex][CurrentState->checkcurrent[f->tempindex]];
                        CurrentState->checkfirelist[f->tempindex][CurrentState->checkcurrent[f->tempindex]]->backfire();
                        break;
                    } else {
                        // state exists, value false->on stack or
                        // does not model phi
                        if (NewState->checknexttar[f->tempindex]) {
                            // state exists, on stack->set min,
                            // update witness
                            if (CurrentState->checkmin[f->tempindex] > NewState->checkdfs[f->tempindex]) {
                                CurrentState->checkmin[f->tempindex] = NewState->checkdfs[f->tempindex];
                                CurrentState->witness[f->tempindex] = NewState;
                                CurrentState->witnesstransition[f->tempindex] = CurrentState->checkfirelist[f->tempindex][CurrentState->checkcurrent[f->tempindex]];
                            }
                        } else {
                            // new state belongs to other scc->
                            // current state does not belong to tscc
                            MinBookmark = CurrentState->checkdfs[f->tempindex];
                        }
                        CurrentState->checkfirelist[f->tempindex][CurrentState->checkcurrent[f->tempindex]]->backfire();
                        CurrentState->checkcurrent[f->tempindex]++;
                    }
                } else {
                    // state exists, value unknown
                    check(NewState, f->goal);
                    if (NewState->value[f->goal->index]) {
                        // right subformula true->witness found!
                        found = true;
                        CurrentState->witness[f->tempindex] = NewState;
                        CurrentState->witnesstransition[f->tempindex] = CurrentState->checkfirelist[f->tempindex][CurrentState->checkcurrent[f->tempindex]];
                        CurrentState->checkfirelist[f->tempindex][CurrentState->checkcurrent[f->tempindex]]->backfire();
                        break;
                    } else {
                        // state exists, right subformula false
                        check(NewState, f->hold);
                        if (NewState->value[f->hold->index]) {
                            // state exists, left true, right false
                            //->continue search
                            NewState->checkparent[f->tempindex] = CurrentState;
#ifdef EXTENDEDCTL
                            TemporalIndex = f->tempindex;
#endif
                            NewState->checkfirelist[f->tempindex] =
#ifdef STUBBORN
                                stubbornfirelistctl();
#else
                                firelist();
#endif
                            NewState->checkdfs[f->tempindex] = NewState->checkmin[f->tempindex] = currentdfsnum;
                            currentdfsnum++;
                            NewState->checkcurrent[f->tempindex] = 0;
                            NewState->checknexttar[f->tempindex] = tarjanroot;
                            tarjanroot = NewState;
                            CurrentState = NewState;
                        } else {
                            // state exists, left false, right false
                            //->backtracking
                            CurrentState->checkfirelist[f->tempindex][CurrentState->checkcurrent[f->tempindex]]->backfire();
                            CurrentState->checkcurrent[f->tempindex]++;
                        }
                    }
                }
            } else {
                // state is new
                NewState = binInsert();
                NrStates ++;
#ifdef MAXIMALSTATES
                checkMaximalStates(NrStates); ///// LINE ADDED BY NIELS
#endif
                NewState->checkcurrent[f->tempindex] = 0;
                NewState->checkdfs[f->tempindex] = NewState->checkmin[f->tempindex] = currentdfsnum;
                currentdfsnum ++;
                check(NewState, f->goal);
                if (NewState->value[f->goal->index]) {
                    // right subformula true->witness found!
                    found = true;
                    CurrentState->witness[f->tempindex] = NewState;
                    CurrentState->witnesstransition[f->tempindex] =  CurrentState->checkfirelist[f->tempindex][CurrentState->checkcurrent[f->tempindex]];
                    CurrentState->checkfirelist[f->tempindex][CurrentState->checkcurrent[f->tempindex]]->backfire();
                    break;
                } else {
                    // right subformula false
                    check(NewState, f->hold);
                    if (NewState->value[f->hold->index]) {
                        // left true, right false
                        //->continue search
                        NewState->checkparent[f->tempindex] = CurrentState;
#ifdef EXTENDEDCTL
                        TemporalIndex = f->tempindex;
#endif
                        NewState->checkfirelist[f->tempindex] =
#ifdef STUBBORN
                            stubbornfirelistctl();
#else
                            firelist();
#endif
                        NewState->checkcurrent[f->tempindex] = 0;
                        NewState->checknexttar[f->tempindex] = tarjanroot;
                        tarjanroot = NewState;
                        CurrentState = NewState;
                    } else {
                        // left false, right false
                        //->backtracking
                        CurrentState->checkfirelist[f->tempindex][CurrentState->checkcurrent[f->tempindex]]->backfire();
                        CurrentState->checkcurrent[f->tempindex]++;
                    }
                }
            }
        } else {
            // firelist finished: test scc, update checknexttar,
            // check for ignored transitions,
            // return to previous state
            if (CurrentState->checkdfs[f->tempindex] ==
                    CurrentState->checkmin[f->tempindex]) {

                // check if scc is tscc
                if (CurrentState->checkdfs[f->tempindex] > MinBookmark) {
                    // is tscc->check for ignored transitions
                    unsigned int CardIgnored;
                    Transition* ignored;

                    CardIgnored = 0;
                    for (ignored = ignored->StartOfEnabledList; ignored; ignored = ignored->NextEnabled) {
                        if ((ignored->lstdisabled[f->tempindex] >= CurrentState->checkdfs[f->tempindex]) &&
                                (ignored->lstfired[f->tempindex] < CurrentState->checkdfs[f->tempindex])) {
                            CardIgnored ++;
                            break;
                        }
                    }
                    if (CardIgnored) {
                        // there are ignored transitions
                        //->fire all transitions
                        int i;
#ifdef EXTENDEDCTL
                        TemporalIndex = f->tempindex;
#endif
                        Transition** newFL = firelist();
                        // in ctl modelchecking, ignoring implies that
                        // current firelist was reduced, i.e. singleton.
                        // thus, we sort the unique already
                        //fired transition to the beginning of the firelist.

                        for (i = 0; newFL[i] != CurrentState->checkfirelist[f->tempindex][0]; i++) {
                            ;
                        }
                        newFL[i] = newFL[0];
                        newFL[0] = CurrentState->checkfirelist[f->tempindex][0];
                        delete [] CurrentState->checkfirelist[f->tempindex];
                        CurrentState->checkfirelist[f->tempindex] = newFL;
                        continue;
                    }
                }

                // scc closed: remove members from checknexttar, value = F

                CurrentState->value[f->index] = false;
                CurrentState->known[f->index] = true;
                while (tarjanroot->checkdfs[f->tempindex] >= CurrentState->checkdfs[f->tempindex]) {
                    State* tmp = tarjanroot;
                    tarjanroot->witness[f->tempindex] = NULL;
                    if (tarjanroot == tarjanroot->checknexttar[f->tempindex]) {
                        tarjanroot->checknexttar[f->tempindex] = NULL;
                        break;
                    }
                    tarjanroot = tarjanroot->checknexttar[f->tempindex];
                    tmp->checknexttar[f->tempindex] = NULL;
                }
            }
            // return to previous state
            if ((NewState = CurrentState->checkparent[f->tempindex])) {
                if (NewState->checkmin[f->tempindex] > CurrentState->checkmin[f->tempindex]) {
                    NewState->checkmin[f->tempindex] = CurrentState->checkmin[f->tempindex];
                    NewState->witness[f->tempindex] = CurrentState;
                    NewState->witnesstransition[f->tempindex] = NewState->checkfirelist[f->tempindex][NewState->checkcurrent[f->tempindex]];
                }
                NewState->checkfirelist[f->tempindex][NewState->checkcurrent[f->tempindex]]->backfire();
                NewState->checkcurrent[f->tempindex]++;
            }
            CurrentState = NewState;
        }
    }
    if (found) {
        // set witness for current path
        while ((NewState = CurrentState->checkparent[f->tempindex])) {
            NewState->witness[f->tempindex] = CurrentState;
            NewState->witnesstransition[f->tempindex] = NewState->checkfirelist[f->tempindex][NewState->checkcurrent[f->tempindex]];
            NewState->value[f->index] = true;
            NewState->checkfirelist[f->tempindex][NewState->checkcurrent[f->tempindex]]->backfire();
            CurrentState = NewState;
        }
        while (true) {
            State* tmp;
            tmp = tarjanroot;
            tarjanroot->value[f->index] = true;
            if (tarjanroot == tarjanroot->checknexttar[f->tempindex]) {
                tarjanroot->checknexttar[f->tempindex] = NULL;
                break;
            }
            tarjanroot = tarjanroot->checknexttar[f->tempindex];
            tmp->checknexttar[f->tempindex] = NULL;
        }
    }
}

void futuresearchAU(State* s, untilformula* f) {
    State* tarjanroot;
    unsigned int i;
    State* NewState;

    int found = 0;
    State* CurrentState = s;
    s->checkprevtar[ f->tempindex] = s->checknexttar[f->tempindex] = s;
    tarjanroot = s; // End of Stack is recognised as loop back to root
    // Thus, states not on stack can be detected through
    // nil-pointer in checknexttar
#ifdef EXTENDEDCTL
    TemporalIndex = f->tempindex;
#endif
    s->checkfirelist[f->tempindex] =
#ifdef STUBBORN
        stubbornfirelistctl();
#else
        firelist();
#endif
    s->checksucc[f->tempindex] = new State * [f->tempcard];
    for (i = 0; i < f->tempcard; i++) {
        s->checksucc[f->tempindex][i] = NULL;
    }
    s->checkparent[f->tempindex] = NULL;
    s->checkdfs[f->tempindex] = s->checkmin[f->tempindex] = currentdfsnum++;
    s->checkcurrent[f->tempindex] = 0;
    if (!(s->checkfirelist[f->tempindex]) || !(s->checkfirelist[f->tempindex][0])) {
        found = true;
        s->witness[f->tempindex] = NULL;
        s->witnesstransition[f->tempindex] = NULL;
    } else {
        while (CurrentState) {
            CurrentState->value[f->index] = true;
            CurrentState->known[f->index] = true;

            unsigned int MinBookmark = 0;
            if (CurrentState->checkfirelist[f->tempindex][CurrentState->checkcurrent[f->tempindex]]) {
                // explore next state
                CurrentState->checkfirelist[f->tempindex][CurrentState->checkcurrent[f->tempindex]]->fire();
                Edges ++;
                if (!(Edges % REPORTFREQUENCY)) {
                    report_statistics();
                }
                if ((NewState = binSearch())) {
                    // state exists
                    if (NewState->known[f->index]) {
                        // state exists, value known
                        if (NewState->value[f->index]) {
                            // state exists, value true->on tarjan stack or
                            // does not model phi
                            if (NewState->checknexttar[f->tempindex]) {
                                // state exists, on tarjan stack->set min,
                                // update counterexample path
                                if (CurrentState->checkmin[f->tempindex] > NewState->checkdfs[f->tempindex]) {
                                    CurrentState->checkmin[f->tempindex] = NewState->checkdfs[f->tempindex];
                                    CurrentState->witness[f->tempindex] = NewState;
                                    CurrentState->witnesstransition[f->tempindex] = CurrentState->checkfirelist[f->tempindex][CurrentState->checkcurrent[f->tempindex]];
                                }
                            } else {
                                // new state belongs to other scc->
                                // current state does not belong to tscc
                                MinBookmark = CurrentState->checkdfs[f->tempindex];
                            }
                            CurrentState->checkfirelist[f->tempindex][CurrentState->checkcurrent[f->tempindex]]->backfire();
                            CurrentState->checkcurrent[f->tempindex]++;
                        } else {
                            // state exists, value false -->counterexample found!
                            found = 1;
                            CurrentState->witness[f->tempindex] = NewState;
                            CurrentState->witnesstransition[f->tempindex] = CurrentState->checkfirelist[f->tempindex][CurrentState->checkcurrent[f->tempindex]];
                            CurrentState->checkfirelist[f->tempindex][CurrentState->checkcurrent[f->tempindex]]->backfire();
                            break;
                        }
                    } else {
                        // state exists, value unknown
                        check(NewState, f->goal);
                        if (NewState->value[f->goal->index]) {
                            // state exists, right true
                            //->backtracking
                            CurrentState->checkfirelist[f->tempindex][CurrentState->checkcurrent[f->tempindex]]->backfire();
                            CurrentState->checkcurrent[f->tempindex]++;
                        } else {
                            // state exists, right subformula false
                            check(NewState, f->hold);
                            if (NewState->value[f->hold->index]) {
                                // state exists, left true, right false
                                //->continue search
                                NewState->checkparent[f->tempindex] = CurrentState;
#ifdef EXTENDEDCTL
                                TemporalIndex = f->tempindex;
#endif
                                NewState->checkfirelist[f->tempindex] =
#ifdef STUBBORN
                                    stubbornfirelistctl();
#else
                                    firelist();
#endif
                                if (!(NewState->checkfirelist[f->tempindex]) || !(NewState->checkfirelist[f->tempindex][0])) {
                                    // deadlock->counterexample found!
                                    found = 1;
                                    NewState->witness[f->tempindex] = NULL;
                                    CurrentState->witnesstransition[f->tempindex] =  NULL;
                                    CurrentState->witness[f->tempindex] = NewState;
                                    CurrentState->witnesstransition[f->tempindex] = CurrentState->checkfirelist[f->tempindex][CurrentState->checkcurrent[f->tempindex]];
                                    CurrentState->checkfirelist[f->tempindex][CurrentState->checkcurrent[f->tempindex]]->backfire();
                                    break;
                                }
                                NewState->checksucc[f->tempindex] = new State * [f->tempcard];
                                for (i = 0; i < f->tempcard; i++) {
                                    NewState->checksucc[f->tempindex][i] = NULL;
                                }
                                NewState->checkdfs[f->tempindex] = NewState->checkmin[f->tempindex] = currentdfsnum;
                                currentdfsnum++;
                                NewState->checkcurrent[f->tempindex] = 0;
                                //NewState->checknexttar[f->tempindex] =
                                //tarjanroot;
                                //tarjanroot = NewState;
                                NewState->checkprevtar[f->tempindex] = tarjanroot;
                                NewState->checknexttar[f->tempindex] = tarjanroot->checknexttar[f->tempindex];
                                tarjanroot = tarjanroot->checknexttar[f->tempindex] = NewState;
                                NewState->checknexttar[f->tempindex]->checkprevtar[f->tempindex] = NewState;

                                CurrentState->checksucc[f->tempindex][CurrentState->checkcurrent[f->tempindex]] = NewState;
                                CurrentState = NewState;
                            } else {
                                // left+right false->counterexample found!
                                found = 1;
                                CurrentState->witness[f->tempindex] = NewState;
                                CurrentState->witnesstransition[f->tempindex] = CurrentState->checkfirelist[f->tempindex][CurrentState->checkcurrent[f->tempindex]];
                                CurrentState->checkfirelist[f->tempindex][CurrentState->checkcurrent[f->tempindex]]->backfire();
                                break;
                            }
                        }
                    }
                } else {
                    // state is new
                    NewState = binInsert();
                    NrStates ++;
#ifdef MAXIMALSTATES
                    checkMaximalStates(NrStates); ///// LINE ADDED BY NIELS
#endif
                    NewState->checkcurrent[f->tempindex] = 0;
                    NewState->checkdfs[f->tempindex] = NewState->checkmin[f->tempindex] = currentdfsnum;
                    currentdfsnum ++;
                    check(NewState, f->goal);
                    if (NewState->value[f->goal->index]) {
                        // right true
                        //->backtracking
                        CurrentState->checkfirelist[f->tempindex][CurrentState->checkcurrent[f->tempindex]]->backfire();
                        CurrentState->checkcurrent[f->tempindex]++;
                    } else {
                        // right subformula false
                        check(NewState, f->hold);
                        if (NewState->value[f->hold->index]) {
                            // left true, right false
                            //->continue search
                            NewState->checkparent[f->tempindex] = CurrentState;
#ifdef EXTENDEDCTL
                            TemporalIndex = f->tempindex;
#endif
                            NewState->checkfirelist[f->tempindex] =
#ifdef STUBBORN
                                stubbornfirelistctl();
#else
                                firelist();
#endif
                            if (!(NewState->checkfirelist[f->tempindex]) || !(NewState->checkfirelist[f->tempindex][0])) {
                                // deadlock->counterexample found!
                                found = 1;
                                NewState->witness[f->tempindex] = NULL;
                                CurrentState->witnesstransition[f->tempindex] =  NULL;
                                CurrentState->witness[f->tempindex] = NewState;
                                CurrentState->witnesstransition[f->tempindex] = CurrentState->checkfirelist[f->tempindex][CurrentState->checkcurrent[f->tempindex]];
                                CurrentState->checkfirelist[f->tempindex][CurrentState->checkcurrent[f->tempindex]]->backfire();
                                break;
                            }
                            NewState->checksucc[f->tempindex] = new State * [f->tempcard];
                            for (i = 0; i < f->tempcard; i++) {
                                NewState->checksucc[f->tempindex][i] = NULL;
                            }
                            NewState->checkcurrent[f->tempindex] = 0;
                            //NewState->checknexttar[f->tempindex] =
                            //tarjanroot;
                            //tarjanroot = NewState;
                            NewState->checkprevtar[f->tempindex] = tarjanroot;
                            NewState->checknexttar[f->tempindex] = tarjanroot->checknexttar[f->tempindex];
                            tarjanroot = tarjanroot->checknexttar[f->tempindex] = NewState;
                            NewState->checknexttar[f->tempindex]->checkprevtar[f->tempindex] = NewState;
                            CurrentState->checksucc[f->tempindex][CurrentState->checkcurrent[f->tempindex]] = NewState;
                            CurrentState = NewState;
                        } else {
                            // left+right false->counterexample found!
                            found = 1;
                            CurrentState->witness[f->tempindex] = NewState;
                            CurrentState->witnesstransition[f->tempindex] = CurrentState->checkfirelist[f->tempindex][CurrentState->checkcurrent[f->tempindex]];
                            CurrentState->checkfirelist[f->tempindex][CurrentState->checkcurrent[f->tempindex]]->backfire();
                            break;
                        }
                    }
                }
            } else {
                // firelist finished: test scc, update checknexttar,
                // check for ignored transitions,
                // return to previous state
                if (CurrentState->checkdfs[f->tempindex] == CurrentState->checkmin[f->tempindex]) {

                    // scc closed
                    // check if scc is tscc
                    if (CurrentState->checkdfs[f->tempindex] > MinBookmark) {
                        // is tscc->check for ignored transitions
                        unsigned int CardIgnored;
                        Transition* ignored;

                        CardIgnored = 0;
                        for (ignored = ignored->StartOfEnabledList; ignored;
                                ignored = ignored->NextEnabled) {
                            if ((ignored->lstdisabled[f->tempindex] >= CurrentState->checkdfs[f->tempindex]) &&
                                    (ignored->lstfired[f->tempindex] < CurrentState->checkdfs[f->tempindex])) {
                                CardIgnored ++;
                                break;
                            }
                        }
                        if (CardIgnored) {
                            // there are ignored transitions
                            //->fire all transitions
                            int i;
#ifdef EXTENDEDCTL
                            TemporalIndex = f->tempindex;
#endif
                            Transition** newFL = firelist();
                            // in ctl modelchecking, ignoring implies that
                            // current firelist was reduced, i.e. singleton.
                            // thus, we sort the unique already
                            //fired transition to the beginning of the firelist.

                            for (i = 0; newFL[i] != CurrentState->checkfirelist[f->tempindex][0]; i++) {
                                ;
                            }
                            newFL[i] = newFL[0];
                            newFL[0] = CurrentState->checkfirelist[f->tempindex][0];
                            delete [] CurrentState->checkfirelist[f->tempindex];
                            CurrentState->checkfirelist[f->tempindex] = newFL;
                            continue;
                        }
                    }

                    // scc closed: remove members from tarjanstack, search fair sc set

                    //while(tarjanroot->checkdfs[f->tempindex] >=
                    //    CurrentState->checkdfs[f->tempindex])
                    //{
                    //  State * tmp;
                    //  tmp = tarjanroot;
                    //  tarjanroot->witness[f->tempindex] = NULL;
                    //  if(tarjanroot == tarjanroot->checknexttar[f->tempindex])
                    //  {
                    //    tarjanroot->checknexttar[f->tempindex] = NULL;
                    //    break;
                    //  }
                    //  tarjanroot = tarjanroot->checknexttar[f->tempindex];
                    //  tmp->checknexttar[f->tempindex] = NULL;
                    //}
                    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
                    // unlink scc and check it for counterexample sc sets
                    //                        if(CurrentState != tarjanroot->checknexttar[f->tempindex]) // current != bottom(stack)
                    //                        {
                    //                                State * newroot;
                    //                                newroot = CurrentState->checkprevtar[f->tempindex];
                    //                                newroot->checknexttar[f->tempindex] = tarjanroot->checknexttar[f->tempindex];
                    //                                tarjanroot->checknexttar[f->tempindex]->checkprevtar[f->tempindex] = newroot;
                    //                                tarjanroot->checknexttar[f->tempindex] = CurrentState;
                    //                                CurrentState->checkprevtar[f->tempindex] = tarjanroot;
                    //                                tarjanroot = newroot;
                    //                        }
                    //      State * start;
                    //                               for(s = CurrentState->checknexttar[f->tempindex]; s != CurrentState; s = s->checknexttar[f->tempindex])
                    //                                {
                    //     s->checktarlevel[f->tempindex] = 1;
                    //                 }
                    //start->checktarlevel[f->tempindex] = 1;
                    //                                // analyze this
                    //                                State * oldpar;
                    //                                unsigned int oldc;
                    //                                oldpar = CurrentState->checkparent[f->tempindex];
                    //                                oldc = CurrentState->checkcurrent[f->tempindex];
                    //                                if(check_analyse_fairness(start,1,f->tempindex))
                    //                                {
                    //          // value false; infinite counterexample
                    //          found = 2;
                    //          break;
                    //        }
                    //                                CurrentState->checkparent[f->tempindex] = oldpar;
                    //                                CurrentState->checkcurrent[f->tempindex] = oldc;
                    //
                    ////////////////////////////////////////////////////////////////////////////////////////////////////////////
                }
                // return to previous state
                if ((NewState = CurrentState->checkparent[f->tempindex])) {
                    if (NewState->checkmin[f->tempindex] > CurrentState->checkmin[f->tempindex]) {
                        NewState->checkmin[f->tempindex] = CurrentState->checkmin[f->tempindex];
                        NewState->witness[f->tempindex] = CurrentState;
                        NewState->witnesstransition[f->tempindex] = NewState->checkfirelist[f->tempindex][NewState->checkcurrent[f->tempindex]];
                    }
                    NewState->checkfirelist[f->tempindex][NewState->checkcurrent[f->tempindex]]->backfire();
                    NewState->checkcurrent[f->tempindex]++;
                }
                CurrentState = NewState;
            }
        }
    }
    if (found) {
        // set witness for current path
        while ((NewState = CurrentState->checkparent[f->tempindex])) {
            NewState->witness[f->tempindex] = CurrentState;
            NewState->witnesstransition[f->tempindex] = NewState->checkfirelist[f->tempindex][NewState->checkcurrent[f->tempindex]];
            NewState->value[f->index] = true;
            NewState->checkfirelist[f->tempindex][NewState->checkcurrent[f->tempindex]]->backfire();
            CurrentState = NewState;
        }
        while (true) {
            State* tmp = tarjanroot;
            tarjanroot->value[f->index] = true;
            if (tarjanroot == tarjanroot->checknexttar[f->tempindex]) {
                tarjanroot->checkprevtar[f->tempindex] = NULL;
                break;
            }
            tarjanroot = tarjanroot->checkprevtar[f->tempindex];
            tmp->checkprevtar[f->tempindex] = tmp->checknexttar[f->tempindex] = NULL;
        }
    }
}

void searchAU(State* s, untilformula* f) {
    State* NewState;
    bool found = false;
    State* CurrentState = s;
#ifdef EXTENDEDCTL
    TemporalIndex = f->tempindex;
#endif
    s->checkfirelist[f->tempindex] =
#ifdef STUBBORN
        stubbornfirelistctl();
#else
        firelist();
#endif
    s->checkcurrent[f->tempindex] = 0;
    s->checkparent[f->tempindex] = NULL;

    while (CurrentState) {
        CurrentState->value[f->index] = false;
        CurrentState->known[f->index] = true;

        if (CurrentState->checkfirelist[f->tempindex][CurrentState->checkcurrent[f->tempindex]]) {
            // explore next state
            CurrentState->checkfirelist[f->tempindex][CurrentState->checkcurrent[f->tempindex]]->fire();
            Edges ++;
            if (!(Edges % REPORTFREQUENCY)) {
                report_statistics();
            }
            if ((NewState = binSearch())) {
                // state exists
                if (NewState->known[f->index]) {
                    // state exists, value known
                    if (!NewState->value[f->index]) {
                        // state exists, value f -->counterex found!
                        // state either on stack or does not model A phi U psi
                        found = true;
                        CurrentState->witness[f->tempindex] = NewState;
                        CurrentState->witnesstransition[f->tempindex] = CurrentState->checkfirelist[f->tempindex][CurrentState->checkcurrent[f->tempindex]];
                        CurrentState->checkfirelist[f->tempindex][CurrentState->checkcurrent[f->tempindex]]->backfire();
                        break;
                    } else {
                        // state exists, value true->continue search
                        CurrentState->checkfirelist[f->tempindex][CurrentState->checkcurrent[f->tempindex]]->backfire();
                        CurrentState->checkcurrent[f->tempindex]++;
                    }
                } else {
                    // state exists, value unknown
                    check(NewState, f->goal);
                    if (NewState->value[f->goal->index]) {
                        // right subformula true->backtracking
                        CurrentState->checkfirelist[f->tempindex][CurrentState->checkcurrent[f->tempindex]]->backfire();
                        CurrentState->checkcurrent[f->tempindex]++;
                    } else {
                        // state exists, right subformula true
                        check(NewState, f->hold);
                        if (NewState->value[f->hold->index]) {
                            // state exists, left true, right false
                            //->continue search
                            NewState->checkparent[f->tempindex] = CurrentState;
#ifdef EXTENDEDCTL
                            TemporalIndex = f->tempindex;
#endif
                            NewState->checkfirelist[f->tempindex] =
#ifdef STUBBORN
                                stubbornfirelistctl();
#else
                                firelist();
#endif
                            NewState->checkcurrent[f->tempindex] = 0;
                            CurrentState = NewState;
                        } else {
                            // state exists, left false, right false
                            //->counterex found
                            found = true;
                            CurrentState->witness[f->tempindex] = NewState;
                            CurrentState->witnesstransition[f->tempindex] = CurrentState->checkfirelist[f->tempindex][CurrentState->checkcurrent[f->tempindex]];
                            CurrentState->checkfirelist[f->tempindex][CurrentState->checkcurrent[f->tempindex]]->backfire();
                            break;
                        }
                    }
                }
            } else {
                // state is new
                NewState = binInsert();
                NrStates ++;
#ifdef MAXIMALSTATES
                checkMaximalStates(NrStates); ///// LINE ADDED BY NIELS
#endif
                NewState->checkcurrent[f->tempindex] = 0;
                check(NewState, f->goal);
                if (NewState->value[f->goal->index]) {
                    // right subformula true->backtracking!
                    CurrentState->checkfirelist[f->tempindex][CurrentState->checkcurrent[f->tempindex]]->backfire();
                    CurrentState->checkcurrent[f->tempindex]++;
                } else {
                    // right subformula false
                    check(NewState, f->hold);
                    if (NewState->value[f->hold->index]) {
                        // left true, right false
                        //->continue search
                        NewState->checkparent[f->tempindex] = CurrentState;
#ifdef EXTENDEDCTL
                        TemporalIndex = f->tempindex;
#endif
                        NewState->checkfirelist[f->tempindex] =
#ifdef STUBBORN
                            stubbornfirelistctl();
#else
                            firelist();
#endif
                        NewState->checkcurrent[f->tempindex] = 0;
                        CurrentState = NewState;
                    } else {
                        // left false, right false
                        //->backtracking
                        found = true;
                        CurrentState->witness[f->tempindex] = NewState;
                        CurrentState->witnesstransition[f->tempindex] = CurrentState->checkfirelist[f->tempindex][CurrentState->checkcurrent[f->tempindex]];
                        CurrentState->checkfirelist[f->tempindex][CurrentState->checkcurrent[f->tempindex]]->backfire();
                        break;
                    }
                }
            }
        } else {
            // return to previous state
            CurrentState->value[f->index] = true;
            if ((NewState = CurrentState->checkparent[f->tempindex])) {
                NewState->checkfirelist[f->tempindex][NewState->checkcurrent[f->tempindex]]->backfire();
                NewState->checkcurrent[f->tempindex]++;
            }
            CurrentState = NewState;
        }
    }
    if (found) {
        // set witness for current path
        while ((NewState = CurrentState->checkparent[f->tempindex])) {
            NewState->witness[f->tempindex] = CurrentState;
            NewState->witnesstransition[f->tempindex] = NewState->checkfirelist[f->tempindex][NewState->checkcurrent[f->tempindex]];
            NewState->checkfirelist[f->tempindex][NewState->checkcurrent[f->tempindex]]->backfire();
            CurrentState = NewState;
        }
    }
}

void searchAF(State* s, unarytemporalformula* f) {
    State* NewState;
    bool found = false;
    State* CurrentState = s;
#ifdef EXTENDEDCTL
    TemporalIndex = f->tempindex;
#endif
    s->checkfirelist[f->tempindex] =
#ifdef STUBBORN
        stubbornfirelistctl();
#else
        firelist();
#endif
    s->checkcurrent[f->tempindex] = 0;
    s->checkparent[f->tempindex] = NULL;

    while (CurrentState) {
        CurrentState->value[f->index] = false;
        CurrentState->known[f->index] = true;

        if (CurrentState->checkfirelist[f->tempindex][CurrentState->checkcurrent[f->tempindex]]) {
            // explore next state
            CurrentState->checkfirelist[f->tempindex][CurrentState->checkcurrent[f->tempindex]]->fire();
            Edges ++;
            if (!(Edges % REPORTFREQUENCY)) {
                report_statistics();
            }
            if ((NewState = binSearch())) {
                if (NewState->known[f->index]) {
                    // state exists, value known
                    if (!NewState->value[f->index]) {
                        // state exists, value f -->counterex found!
                        // state either on stack or does not model A phi U psi
                        found = true;
                        CurrentState->witness[f->tempindex] = NewState;
                        CurrentState->witnesstransition[f->tempindex] = CurrentState->checkfirelist[f->tempindex][CurrentState->checkcurrent[f->tempindex]];
                        CurrentState->checkfirelist[f->tempindex][CurrentState->checkcurrent[f->tempindex]]->backfire();
                        break;
                    } else {
                        // state exists, value true->continue search
                        CurrentState->checkfirelist[f->tempindex][CurrentState->checkcurrent[f->tempindex]]->backfire();
                        CurrentState->checkcurrent[f->tempindex]++;
                    }
                }   else {
                    // state exists, value unknown
                    check(NewState, f->element);
                    if (NewState->value[f->element->index]) {
                        // subformula true->backtracking
                        CurrentState->checkfirelist[f->tempindex][CurrentState->checkcurrent[f->tempindex]]->backfire();
                        CurrentState->checkcurrent[f->tempindex]++;
                    } else {
                        // subformula false->continue search
                        NewState->checkparent[f->tempindex] = CurrentState;
#ifdef EXTENDEDCTL
                        TemporalIndex = f->tempindex;
#endif
                        NewState->checkfirelist[f->tempindex] =
#ifdef STUBBORN
                            stubbornfirelistctl();
#else
                            firelist();
#endif
                        NewState->checkcurrent[f->tempindex] = 0;
                        CurrentState = NewState;
                    }
                }
            } else {
                // state is new
                NewState = binInsert();
                NrStates ++;
#ifdef MAXIMALSTATES
                checkMaximalStates(NrStates); ///// LINE ADDED BY NIELS
#endif
                NewState->checkcurrent[f->tempindex] = 0;
                check(NewState, f->element);
                if (NewState->value[f->element->index]) {
                    // subformula true -> backtracking!
                    CurrentState->checkfirelist[f->tempindex][CurrentState->checkcurrent[f->tempindex]]->backfire();
                    CurrentState->checkcurrent[f->tempindex]++;
                } else {
                    // subformula false -> continue search
                    NewState->checkparent[f->tempindex] = CurrentState;
#ifdef EXTENDEDCTL
                    TemporalIndex = f->tempindex;
#endif
                    NewState->checkfirelist[f->tempindex] =
#ifdef STUBBORN
                        stubbornfirelistctl();
#else
                        firelist();
#endif
                    NewState->checkcurrent[f->tempindex] = 0;
                    CurrentState = NewState;
                }
            }
        } else {
            // return to previous state
            CurrentState->value[f->index] = true;
            CurrentState->known[f->index] = true;
            if (!(CurrentState->checkfirelist[f->tempindex][0])) {
                if (Globals::Transitions[0]->NrEnabled) {
                    // no tau-successors, but state not dead
                }
                // dead state: this state counts as successor state
                if (DeadStatePathRestriction[f->tempindex]) {
                    check(CurrentState, f->element);
                    if (!(CurrentState->value[f->element->index])) {
                        CurrentState->value[f->index] = false;
                        CurrentState->witness[f->tempindex] = NULL;
                        CurrentState->witnesstransition[f->tempindex] = NULL;
                        found = true;
                        break;
                    }
                }
            }
            if ((NewState = CurrentState->checkparent[f->tempindex])) {
                NewState->checkfirelist[f->tempindex][NewState->checkcurrent[f->tempindex]]->backfire();
                NewState->checkcurrent[f->tempindex]++;
            }
            CurrentState = NewState;
        }
    }
    if (found) {
        // set witness for current path
        while ((NewState = CurrentState->checkparent[f->tempindex])) {
            NewState->witness[f->tempindex] = CurrentState;
            NewState->witnesstransition[f->tempindex] = NewState->checkfirelist[f->tempindex][NewState->checkcurrent[f->tempindex]];
            NewState->checkfirelist[f->tempindex][NewState->checkcurrent[f->tempindex]]->backfire();
            CurrentState = NewState;
        }
    }
}

void searchEG(State* s, unarytemporalformula* f) {
    State* NewState;
    bool found = false;
    State* CurrentState = s;
#ifdef EXTENDEDCTL
    TemporalIndex = f->tempindex;
#endif
    s->checkfirelist[f->tempindex] =
#ifdef STUBBORN
        stubbornfirelistctl();
#else
        firelist();
#endif
    s->checkcurrent[f->tempindex] = 0;
    s->checkparent[f->tempindex] = NULL;
    s->checkdfs[f->tempindex] = s->checkmin[f->tempindex] = 0;

    while (CurrentState) {
        CurrentState->value[f->index] = true;
        CurrentState->known[f->index] = true;

        if (CurrentState->checkfirelist[f->tempindex][CurrentState->checkcurrent[f->tempindex]]) {
            // explore next state
            CurrentState->checkfirelist[f->tempindex][CurrentState->checkcurrent[f->tempindex]]->fire();
            Edges ++;
            if (!(Edges % REPORTFREQUENCY)) {
                report_statistics();
            }
            if ((NewState = binSearch())) {
                // state exists
                if (NewState->known[f->index]) {
                    // state exists, value known
                    if (NewState->value[f->index]) {
                        // state exists, value t -->witness found!
                        // state either on stack or models EG phi
                        found = true;
                        CurrentState->witness[f->tempindex] = NewState;
                        CurrentState->witnesstransition[f->tempindex] = CurrentState->checkfirelist[f->tempindex][CurrentState->checkcurrent[f->tempindex]];
                        CurrentState->checkfirelist[f->tempindex][CurrentState->checkcurrent[f->tempindex]]->backfire();
                        break;
                    } else {
                        // state exists, value false->continue search
                        CurrentState->checkfirelist[f->tempindex][CurrentState->checkcurrent[f->tempindex]]->backfire();
                        CurrentState->checkcurrent[f->tempindex]++;
                    }
                } else {
                    // state exists, value unknown
                    check(NewState, f->element);
                    if (!NewState->value[ f->element->index]) {
                        // subformula false->backtracking
                        CurrentState->checkfirelist[f->tempindex][CurrentState->checkcurrent[f->tempindex]]->backfire();
                        CurrentState->checkcurrent[f->tempindex]++;
                    } else {
                        // subformula true->continue search
                        NewState->checkparent[f->tempindex] = CurrentState;
#ifdef EXTENDEDCTL
                        TemporalIndex = f->tempindex;
#endif
                        NewState->checkfirelist[f->tempindex] =
#ifdef STUBBORN
                            stubbornfirelistctl();
#else
                            firelist();
#endif
                        NewState->checkcurrent[f->tempindex] = 0;
                        CurrentState = NewState;
                    }
                }
            } else {
                // state is new
                NewState = binInsert();
                NrStates ++;
#ifdef MAXIMALSTATES
                checkMaximalStates(NrStates); ///// LINE ADDED BY NIELS
#endif
                NewState->checkcurrent[f->tempindex] = 0;
                check(NewState, f->element);
                if (!NewState->value[f->element->index]) {
                    // subformula false->backtracking!
                    CurrentState->checkfirelist[f->tempindex][CurrentState->checkcurrent[f->tempindex]]->backfire();
                    CurrentState->checkcurrent[f->tempindex]++;
                } else {
                    // subformula true->continue search
                    NewState->checkparent[f->tempindex] = CurrentState;
#ifdef EXTENDEDCTL
                    TemporalIndex = f->tempindex;
#endif
                    NewState->checkfirelist[f->tempindex] =
#ifdef STUBBORN
                        stubbornfirelistctl();
#else
                        firelist();
#endif
                    NewState->checkcurrent[f->tempindex] = 0;
                    CurrentState = NewState;
                }
            }
        } else {
            // return to previous state
            CurrentState->value[f->index] = false;
            CurrentState->known[f->index] = false;
            if (!(CurrentState->checkfirelist[f->tempindex][0])) {
                if (Globals::Transitions[0]->NrEnabled) {
                    // no tau-successors, but state not dead
                }
                // dead state: this state counts as successor state
                if (DeadStatePathRestriction[f->tempindex]) {
                    check(CurrentState, f->element);
                    if ((CurrentState->value[f->element->index])) {
                        CurrentState->value[f->index] = true;
                        CurrentState->witness[f->tempindex] = NULL;
                        CurrentState->witnesstransition[f->tempindex] = NULL;
                        found = true;
                        break;
                    }
                }
            }
            if ((NewState = CurrentState->checkparent[f->tempindex])) {
                NewState->checkfirelist[f->tempindex][NewState->checkcurrent[f->tempindex]]->backfire();
                NewState->checkcurrent[f->tempindex]++;
            }
            CurrentState = NewState;
        }
    }
    if (found) {
        // set witness for current path
        while ((NewState = CurrentState->checkparent[f->tempindex])) {
            NewState->witness[f->tempindex] = CurrentState;
            NewState->witnesstransition[f->tempindex] = NewState->checkfirelist[f->tempindex][NewState->checkcurrent[f->tempindex]];
            NewState->checkfirelist[f->tempindex][NewState->checkcurrent[f->tempindex]]->backfire();
            CurrentState = NewState;
        }
    }
}

void futuresearchAF(State* s, unarytemporalformula* f) {
    State* NewState;
    bool found = false;
    State* CurrentState = s;
#ifdef EXTENDEDCTL
    TemporalIndex = f->tempindex;
#endif
    s->checkfirelist[f->tempindex] =
#ifdef STUBBORN
        stubbornfirelistctl();
#else
        firelist();
#endif
    s->checkcurrent[f->tempindex] = 0;
    s->checkparent[f->tempindex] = NULL;
    s->value[f->index] = false;
    s->known[f->index] = true;

    if (!(s->checkfirelist[f->tempindex]) || !(s->checkfirelist[f->tempindex][0])) {
        found = true;
        s->witness[f->tempindex] = NULL;
        s->witnesstransition[f->tempindex] = NULL;
    } else {
        while (CurrentState) {
            CurrentState->value[f->index] = false;
            CurrentState->known[f->index] = true;

            if (CurrentState->checkfirelist[f->tempindex][CurrentState->checkcurrent[f->tempindex]]) {
                // explore next state
                CurrentState->checkfirelist[f->tempindex][CurrentState->checkcurrent[f->tempindex]]->fire();
                Edges ++;
                if (!(Edges % REPORTFREQUENCY)) {
                    report_statistics();
                }
                if ((NewState = binSearch())) {
                    // state exists
                    if (NewState->known[f->index]) {
                        // state exists, value known
                        if (!NewState->value[f->index]) {
                            // state exists, value f -->counterex found!
                            // state either on stack or does not model AF phi
                            found = true;
                            CurrentState->witness[f->tempindex] = NewState;
                            CurrentState->witnesstransition[f->tempindex] = CurrentState->checkfirelist[f->tempindex][CurrentState->checkcurrent[f->tempindex]];
                            CurrentState->checkfirelist[f->tempindex][CurrentState->checkcurrent[f->tempindex]]->backfire();
                            break;
                        } else {
                            // state exists, value true->continue search
                            CurrentState->checkfirelist[f->tempindex][CurrentState->checkcurrent[f->tempindex]]->backfire();
                            CurrentState->checkcurrent[f->tempindex]++;
                        }
                    } else {
                        // state exists, value unknown
                        check(NewState, f->element);
                        if (NewState->value[f->element->index]) {
                            // subformula true->backtracking
                            CurrentState->checkfirelist[f->tempindex][CurrentState->checkcurrent[f->tempindex]]->backfire();
                            CurrentState->checkcurrent[f->tempindex]++;
                        } else {
                            // subformula false->continue search
                            NewState->checkparent[f->tempindex] = CurrentState;
#ifdef EXTENDEDCTL
                            TemporalIndex = f->tempindex;
#endif
                            NewState->checkfirelist[f->tempindex] =
#ifdef STUBBORN
                                stubbornfirelistctl();
#else
                                firelist();
#endif
                            if (!(NewState->checkfirelist[f->tempindex]) || !(NewState->checkfirelist[f->tempindex][0])) {
                                // deadlock -->counterex found!
                                found = true;
                                CurrentState->witness[f->tempindex] = NewState;
                                NewState->witness[f->tempindex] = NULL;
                                NewState->witnesstransition[f->tempindex] = NULL;
                                CurrentState->witnesstransition[f->tempindex] = CurrentState->checkfirelist[f->tempindex][CurrentState->checkcurrent[f->tempindex]];
                                CurrentState->checkfirelist[f->tempindex][CurrentState->checkcurrent[f->tempindex]]->backfire();
                                break;
                            } else {
                                NewState->checkcurrent[f->tempindex] = 0;
                                CurrentState = NewState;
                            }
                        }
                    }
                } else {
                    // state is new
                    NewState = binInsert();
                    NrStates ++;
#ifdef MAXIMALSTATES
                    checkMaximalStates(NrStates); ///// LINE ADDED BY NIELS
#endif
                    NewState->checkcurrent[f->tempindex] = 0;
                    check(NewState, f->element);
                    if (NewState->value[f->element->index]) {
                        // subformula true->backtracking!
                        CurrentState->checkfirelist[f->tempindex][CurrentState->checkcurrent[f->tempindex]]->backfire();
                        CurrentState->checkcurrent[f->tempindex]++;
                    } else {
                        // subformula false->continue search
                        NewState->checkparent[f->tempindex] = CurrentState;
#ifdef EXTENDEDCTL
                        TemporalIndex = f->tempindex;
#endif
                        NewState->checkfirelist[f->tempindex] =
#ifdef STUBBORN
                            stubbornfirelistctl();
#else
                            firelist();
#endif
                        if (!(NewState->checkfirelist[f->tempindex]) || !(NewState->checkfirelist[f->tempindex][0])) {
                            // deadlock -->counterex found!
                            found = true;
                            CurrentState->witness[f->tempindex] = NewState;
                            NewState->witness[f->tempindex] = NULL;
                            NewState->witnesstransition[f->tempindex] = NULL;
                            CurrentState->witnesstransition[f->tempindex] = CurrentState->checkfirelist[f->tempindex][CurrentState->checkcurrent[f->tempindex]];
                            CurrentState->checkfirelist[f->tempindex][CurrentState->checkcurrent[f->tempindex]]->backfire();
                            break;
                        } else {
                            NewState->checkcurrent[f->tempindex] = 0;
                            CurrentState = NewState;
                        }
                    }
                }
            } else {
                // return to previous state
                CurrentState->value[f->index] = true;
                if ((NewState = CurrentState->checkparent[f->tempindex])) {
                    NewState->checkfirelist[f->tempindex][NewState->checkcurrent[f->tempindex]]->backfire();
                    NewState->checkcurrent[f->tempindex]++;
                }
                CurrentState = NewState;
            }
        }
    }
    if (found) {
        // set witness for current path
        while ((NewState = CurrentState->checkparent[f->tempindex])) {
            NewState->witness[f->tempindex] = CurrentState;
            NewState->witnesstransition[f->tempindex] = NewState->checkfirelist[f->tempindex][NewState->checkcurrent[f->tempindex]];
            NewState->checkfirelist[f->tempindex][NewState->checkcurrent[f->tempindex]]->backfire();
            CurrentState = NewState;
        }
    }
}

void futuresearchEG(State* s, unarytemporalformula* f) {
    State* NewState;
    bool found = false;
    State* CurrentState = s;
#ifdef EXTENDEDCTL
    TemporalIndex = f->tempindex;
#endif
    s->checkfirelist[f->tempindex] =
#ifdef STUBBORN
        stubbornfirelistctl();
#else
        firelist();
#endif
    s->checkcurrent[f->tempindex] = 0;
    s->checkparent[f->tempindex] = NULL;
    s->checkdfs[f->tempindex] = s->checkmin[f->tempindex] = 0;
    s->value[f->index] = true;
    s->known[f->index] = true;

    if (!(s->checkfirelist[f->tempindex]) || !(s->checkfirelist[f->tempindex][0])) {
        found = true;
        s->witness[f->tempindex] = NULL;
        s->witnesstransition[f->tempindex] = NULL;
    } else {
        while (CurrentState) {
            CurrentState->value[f->index] = true;
            CurrentState->known[f->index] = true;

            if (CurrentState->checkfirelist[f->tempindex][CurrentState->
                                                          checkcurrent[f->tempindex]]) {
                // explore next state
                CurrentState->checkfirelist[f->tempindex][CurrentState->
                                                          checkcurrent[f->tempindex]]->fire();
                Edges ++;
                if (!(Edges % REPORTFREQUENCY)) {
                    report_statistics();
                }
                if ((NewState = binSearch())) {
                    // state exists
                    if (NewState->known[f->index]) {
                        // state exists, value known
                        if (NewState->value[f->index]) {
                            // state exists, value t -->witness found!
                            // state either on stack or models EG phi
                            found = true;
                            CurrentState->witness[f->tempindex] = NewState;
                            CurrentState->witnesstransition[f->tempindex] = CurrentState->checkfirelist[f->tempindex][CurrentState->checkcurrent[f->tempindex]];
                            CurrentState->checkfirelist[f->tempindex][CurrentState->checkcurrent[f->tempindex]]->backfire();
                            break;
                        } else {
                            // state exists, value false->continue search
                            CurrentState->checkfirelist[f->tempindex][CurrentState->checkcurrent[f->tempindex]]->backfire();
                            CurrentState->checkcurrent[f->tempindex]++;
                        }
                    } else {
                        // state exists, value unknown
                        check(NewState, f->element);
                        if (!NewState->value[ f->element->index]) {
                            // subformula false->backtracking
                            CurrentState->checkfirelist[f->tempindex][CurrentState->checkcurrent[f->tempindex]]->backfire();
                            CurrentState->checkcurrent[f->tempindex]++;
                        } else {
                            // subformula true->continue search
                            NewState->checkparent[f->tempindex] = CurrentState;
#ifdef EXTENDEDCTL
                            TemporalIndex = f->tempindex;
#endif
                            NewState->checkfirelist[f->tempindex] =
#ifdef STUBBORN
                                stubbornfirelistctl();
#else
                                firelist();
#endif
                            if (!(NewState->checkfirelist[f->tempindex]) || !(NewState->checkfirelist[f->tempindex][0])) {
                                // deadlock -->witness found!
                                found = true;
                                CurrentState->witness[f->tempindex] = NewState;
                                CurrentState->witnesstransition[f->tempindex] = CurrentState->checkfirelist[f->tempindex][CurrentState->checkcurrent[f->tempindex]];
                                CurrentState->checkfirelist[f->tempindex][CurrentState->checkcurrent[f->tempindex]]->backfire();
                                break;
                            } else {
                                NewState->checkcurrent[f->tempindex] = 0;
                                CurrentState = NewState;
                            }
                        }
                    }
                } else {
                    // state is new
                    NewState = binInsert();
                    NrStates ++;
#ifdef MAXIMALSTATES
                    checkMaximalStates(NrStates); ///// LINE ADDED BY NIELS
#endif
                    NewState->checkcurrent[f->tempindex] = 0;
                    check(NewState, f->element);
                    if (!NewState->value[f->element->index]) {
                        // subformula false->backtracking!
                        CurrentState->checkfirelist[f->tempindex][CurrentState->checkcurrent[f->tempindex]]->backfire();
                        CurrentState->checkcurrent[f->tempindex]++;
                    } else {
                        // subformula true->continue search
                        NewState->checkparent[f->tempindex] = CurrentState;
#ifdef EXTENDEDCTL
                        TemporalIndex = f->tempindex;
#endif
                        NewState->checkfirelist[f->tempindex] =
#ifdef STUBBORN
                            stubbornfirelistctl();
#else
                            firelist();
#endif
                        if (!(NewState->checkfirelist[f->tempindex]) || !(NewState->checkfirelist[f->tempindex][0])) {
                            // deadlock -->witness found!
                            found = true;
                            CurrentState->witness[f->tempindex] = NewState;
                            CurrentState->witnesstransition[f->tempindex] = CurrentState->checkfirelist[f->tempindex][CurrentState->checkcurrent[f->tempindex]];
                            CurrentState->checkfirelist[f->tempindex][CurrentState->checkcurrent[f->tempindex]]->backfire();
                            break;
                        } else {
                            NewState->checkcurrent[f->tempindex] = 0;
                            CurrentState = NewState;
                        }
                    }
                }
            } else {
                // return to previous state
                CurrentState->value[f->index] = false;
                if ((NewState = CurrentState->checkparent[f->tempindex])) {
                    NewState->checkfirelist[f->tempindex][NewState->checkcurrent[f->tempindex]]->backfire();
                    NewState->checkcurrent[f->tempindex]++;
                }
                CurrentState = NewState;
            }
        }
    }
    if (found) {
        // set witness for current path
        while ((NewState = CurrentState->checkparent[f->tempindex])) {
            NewState->witness[f->tempindex] = CurrentState;
            NewState->witnesstransition[f->tempindex] = NewState->checkfirelist[f->tempindex][NewState->checkcurrent[f->tempindex]];
            NewState->checkfirelist[f->tempindex][NewState->checkcurrent[f->tempindex]]->backfire();
            CurrentState = NewState;
        }
    }
}

#endif
