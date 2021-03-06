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
#include "graph.h"
#include "formula.h"
#include <climits>

#ifdef STUBBORN

#define MINIMUM(X,Y) ((X) < (Y) ? (X) : (Y))

void insert_up(State*, formula*);
void insert_down(State*, formula*);
void insert_global_down(formula*);

extern unsigned int* checkstart;
extern Transition* LastAttractor;  ///< Last transition in list of

unsigned int StubbStamp = 0;

void NewStubbStamp() {
    if (StubbStamp < UINT_MAX) {
        ++StubbStamp;
    } else {
        for (unsigned int i = 0; i < Globals::Transitions[0]->cnt; ++i) {
            Globals::Transitions[i]->stamp = 0;
        }
        StubbStamp = 1;
    }
}

void stubbornclosure() {
    Transition::NrStubborn = 0;

    // iterate stubborn list
    for (Transition* current = Transition::StartOfStubbornList; current; current = current->NextStubborn) {
#ifdef EXTENDEDCTL
        if (!(current->pathrestriction[TemporalIndex])) {
            continue;
        }
#endif
        if (current->enabled) {
            Transition::NrStubborn++;
#ifdef RELAXED
            // condition SPP1 from Kristensen/Valmari (Petri nets 2000)
            if (current->down) {
                insert_up(NULL, F);
            }
#endif
        }

        // add all mustbeincluded to StubbornList if not already included
        for (int i = 0; current->mustbeincluded[i]; i++) {
            if (not current->mustbeincluded[i]->instubborn) {
                current->mustbeincluded[i]->instubborn = true;
                current->mustbeincluded[i]->NextStubborn  = NULL;
                Transition::EndOfStubbornList->NextStubborn = current->mustbeincluded[i];
                Transition::EndOfStubbornList = current->mustbeincluded[i];
            }
        }
    }
}

/*!
 Sort places with only invisible pre-transitions to the beginning of the list
 used for enabledness-check, s.t. these places become scapegoat more likely.
*/
void sortscapegoats() {
    // count number of invisible pre-transitions
    for (unsigned int i = 0; i < Globals::Places[0]->cnt; i++) {
        for (unsigned int j = 0; Globals::Places[i]->PreTransitions[j]; j++) {
            if (Globals::Places[i]->PreTransitions[j]->visible) {
                Globals::Places[i]->visible ++;
            }
        }
    }

    // sort PrePlaces and Pre such that places without
    // visible pretransitions move to the beginning
    for (unsigned int i = 0; i < Globals::Transitions[i]->cnt; i++) {
        // sort lists of transition i
        unsigned int firstnz = 0;
        unsigned int firstunknown = 0;
        while (Globals::Transitions[i]->PrePlaces[firstunknown] < Globals::Places[0]->cnt) {
            if (not Globals::Places[Globals::Transitions[i]->PrePlaces[firstunknown]]->visible) {
                // swap firstnz <--> firstunknown
                unsigned int tmp = Globals::Transitions[i]->Pre[firstunknown];
                unsigned int tmpp = Globals::Transitions[i]->PrePlaces[firstunknown];
                Globals::Transitions[i]->Pre[firstunknown] = Globals::Transitions[i]->Pre[firstnz];
                Globals::Transitions[i]->PrePlaces[firstunknown] = Globals::Transitions[i]->PrePlaces[firstnz];
                Globals::Transitions[i]->Pre[firstnz] = tmp;
                Globals::Transitions[i]->PrePlaces[firstnz] = tmpp;
                firstnz++;
            }
            firstunknown++;
        }
    }
}

/*!
\brief adds transition to the StubbornList

The StubbornList is a single linked list that is organized by the pointers
  * Transition::StartOfStubbornList (static - pointer to the start of the list)
  * Transition::EndOfStubbornList (static - pointer to the end of the list)
  * Transition.NextStubborn (member - pointer to next transition)
*/
void stubborninsert(Transition* t) {
    // is transition already in the list?
    if (t->instubborn) {
        return;
    }

    // prepare this transition
    t->instubborn = true;
    t->NextStubborn = NULL;

    if (Transition::StartOfStubbornList) {
        // add transition to end of list
        Transition::EndOfStubbornList->NextStubborn = t;
        Transition::EndOfStubbornList = t;
    } else {
        // list is empty - the transition *is* the list
        Transition::StartOfStubbornList = Transition::EndOfStubbornList = t;
    }
}

void insert_up(State* s, formula* f) {
    switch (f->type) {
        case neq: /* add union of preset and postset */
            for (unsigned int i = 0; i < static_cast<atomicformula*>(f)->p->NrOfArriving; i++) {
                stubborninsert(static_cast<atomicformula*>(f)->p->ArrivingArcs[i]->tr);
            }
            for (unsigned int i = 0; i < static_cast<atomicformula*>(f)->p->NrOfLeaving; i++) {
                stubborninsert(static_cast<atomicformula*>(f)->p->LeavingArcs[i]->tr);
            }
            break;

        case eq: /* add preset if m(p)<k and postset if m(p)>k */
            if (Globals::CurrentMarking[static_cast<atomicformula*>(f)->p->index] < static_cast<atomicformula*>(f)->k) {
                for (unsigned int i = 0; i < static_cast<atomicformula*>(f)->p->NrOfArriving; i++) {
                    stubborninsert(static_cast<atomicformula*>(f)->p->ArrivingArcs[i]->tr);
                }
            } else {
                for (unsigned int i = 0; i < static_cast<atomicformula*>(f)->p->NrOfLeaving; i++) {
                    stubborninsert(static_cast<atomicformula*>(f)->p->LeavingArcs[i]->tr);
                }
            }
            break;

        case leq: /* add postset */
        case lt:
            for (unsigned int i = 0; i < static_cast<atomicformula*>(f)->p->NrOfLeaving; i++) {
                stubborninsert(static_cast<atomicformula*>(f)->p->LeavingArcs[i]->tr);
            }
            break;

        case geq: /* add preset */
        case gt:
            for (unsigned int i = 0; i < static_cast<atomicformula*>(f)->p->NrOfArriving; i++) {
                stubborninsert(static_cast<atomicformula*>(f)->p->ArrivingArcs[i]->tr);
            }
            break;

#ifdef MODELCHECKING
        case conj:
            unsigned int i;
            for (i = checkstart[f->index];;) {
                if (!(s->value[static_cast<booleanformula*>(f)->sub[i]->index])) {
                    break;
                }
                ++i;
                i %= static_cast<booleanformula*>(f)->cardsub;
                if (i == checkstart[f->index]) {
                    break;
                }
            }
#else
        case conj: /* add upset according to satfisfied formula: choose first unsatisfied formulae */
            unsigned int i;
            for (i = 0; i < static_cast<booleanformula*>(f)->cardsub; i++) {
                if (!(static_cast<booleanformula*>(f)->sub[i]->value)) {
                    break;
                }
            }
#endif
            insert_up(s, static_cast<booleanformula*>(f)->sub[i]);
            break;

        case disj: /* add upsets of all subformulae */
            for (unsigned int i = 0; i < static_cast<booleanformula*>(f)->cardsub; i++) {
                insert_up(s, static_cast<booleanformula*>(f)->sub[i]);
            }
            break;

        case neg:
            insert_down(s, static_cast<unarybooleanformula*>(f)->sub);
            break;

#ifdef MODELCHECKING
        case ef:
            break;

        case ag: {
            State* ss;
            for (ss = s; (ss->value[static_cast<unarytemporalformula*>(f)->element->index]); ss = ss->witness[f->tempindex]) {
                for (i = 0; ss->witnesstransition[f->tempindex]->conflicting[i]; i++) {
                    stubborninsert(ss->witnesstransition[f->tempindex]->conflicting[i]);
                }
                stubborninsert(ss->witnesstransition[f->tempindex]);
            }
            insert_up(ss, static_cast<unarytemporalformula*>(f)->element);
            break;
        }

        case eg:
            if (s->value[static_cast<unarytemporalformula*>(f)->element->index]) {
                insert_down(s, static_cast<unarytemporalformula*>(f)->element);
            } else {
                insert_up(s, static_cast<unarytemporalformula*>(f)->element);
            }
            break;

        case eu:
            if (s->value[static_cast<untilformula*>(f)->hold->index]) {
                insert_down(s, static_cast<untilformula*>(f)->hold);
            } else {
                insert_up(s, static_cast<untilformula*>(f)->hold);
                insert_up(s, static_cast<untilformula*>(f)->goal);
            }
            break;

        case af:
            for (State* ss = s; s->checkmin[f->tempindex] < UINT_MAX; ss = ss->witness[f->tempindex]) {
                ss->checkmin[f->tempindex] = UINT_MAX;
                for (i = 0; ss->witnesstransition[f->tempindex]->conflicting[i]; i++) {
                    stubborninsert(ss->witnesstransition[f->tempindex]->conflicting[i]);
                }
                stubborninsert(ss->witnesstransition[f->tempindex]);
                insert_up(ss, static_cast<unarytemporalformula*>(f)->element);
            }
            for (State* ss = s; ss->checkmin[f->tempindex] == UINT_MAX; ss = ss->witness[f->tempindex]) {
                ss->checkmin[f->tempindex] = 0;
            }
            break;

        case au: {
            State* ss;
            for (ss = s; (s->checkmin[f->tempindex] < UINT_MAX) && (s->value[static_cast<untilformula*>(f)->hold->index]); ss = ss->
                    witness[f->tempindex]) {
                ss->checkmin[f->tempindex] = UINT_MAX;
                for (i = 0; ss->witnesstransition[f->tempindex]->conflicting[i]; i++) {
                    stubborninsert(ss->witnesstransition[f->tempindex]->conflicting[i]);
                }
                stubborninsert(ss->witnesstransition[f->tempindex]);
                insert_up(ss, static_cast<untilformula*>(f)->goal);
            }
            if (!ss->value[static_cast<untilformula*>(f)->hold->index]) {
                insert_up(ss, static_cast<untilformula*>(f)->hold);
            }
            for (ss = s; ss->checkmin[f->tempindex] == UINT_MAX; ss =
                        ss->witness[f->tempindex]) {
                ss->checkmin[f->tempindex] = 0;
            }
            break;
        }

        case ax:
        case ex:
            for (unsigned int i = 0; i < Globals::Transitions[0]->cnt; i++) {
                stubborninsert(Globals::Transitions[i]);
            }
            break;
#endif
        default:
            cout << "feature not implemented (yet)\n";
    }
}

void insert_global_up(formula* f) {
    unsigned int i;

    switch (f->type) {
        case neq:
            for (i = 0; i < static_cast<atomicformula*>(f)->p->NrOfArriving; i++) {
                stubborninsert(static_cast<atomicformula*>(f)->p->ArrivingArcs[i]->tr);
            }
            for (i = 0; i < static_cast<atomicformula*>(f)->p->NrOfLeaving; i++) {
                stubborninsert(static_cast<atomicformula*>(f)->p->LeavingArcs[i]->tr);
            }
            break;
        case eq:
            for (i = 0; i < static_cast<atomicformula*>(f)->p->NrOfArriving; i++) {
                stubborninsert(static_cast<atomicformula*>(f)->p->ArrivingArcs[i]->tr);
            }
            for (i = 0; i < static_cast<atomicformula*>(f)->p->NrOfLeaving; i++) {
                stubborninsert(static_cast<atomicformula*>(f)->p->LeavingArcs[i]->tr);
            }
            break;
        case leq:
        case lt:
            for (i = 0; i < static_cast<atomicformula*>(f)->p->NrOfLeaving; i++) {
                stubborninsert(static_cast<atomicformula*>(f)->p->LeavingArcs[i]->tr);
            }
            break;
        case geq:
        case gt:
            for (i = 0; i < static_cast<atomicformula*>(f)->p->NrOfArriving; i++) {
                stubborninsert(static_cast<atomicformula*>(f)->p->ArrivingArcs[i]->tr);
            }
            break;
        case conj:
        case disj:
            for (i = 0; i < static_cast<booleanformula*>(f)->cardsub; i++) {
                insert_global_up(static_cast<booleanformula*>(f)->sub[i]);
            }
            break;
        case neg:
            insert_global_down(static_cast<unarybooleanformula*>(f)->sub);
            break;
        default:
            cout << "feature not implemented (yet)\n";
    }
}

void insert_down(State* s, formula* f) {
    unsigned int i;

    switch (f->type) {
        case neq:
            if (Globals::CurrentMarking[static_cast<atomicformula*>(f)->p->index] < static_cast<atomicformula*>(f)->k) {
                for (i = 0; i < static_cast<atomicformula*>(f)->p->NrOfArriving; i++) {
                    stubborninsert(static_cast<atomicformula*>(f)->p->ArrivingArcs[i]->tr);
                }
            } else {
                for (i = 0; i < static_cast<atomicformula*>(f)->p->NrOfLeaving; i++) {
                    stubborninsert(static_cast<atomicformula*>(f)->p->LeavingArcs[i]->tr);
                }
            }
            break;

        case eq:
            for (i = 0; i < static_cast<atomicformula*>(f)->p->NrOfArriving; i++) {
                stubborninsert(static_cast<atomicformula*>(f)->p->ArrivingArcs[i]->tr);
            }
            for (i = 0; i < static_cast<atomicformula*>(f)->p->NrOfLeaving; i++) {
                stubborninsert(static_cast<atomicformula*>(f)->p->LeavingArcs[i]->tr);
            }
            break;

        case geq:
        case gt:
            for (i = 0; i < static_cast<atomicformula*>(f)->p->NrOfLeaving; i++) {
                stubborninsert(static_cast<atomicformula*>(f)->p->LeavingArcs[i]->tr);
            }
            break;

        case leq:
        case lt:
            for (i = 0; i < static_cast<atomicformula*>(f)->p->NrOfArriving; i++) {
                stubborninsert(static_cast<atomicformula*>(f)->p->ArrivingArcs[i]->tr);
            }
            break;

        case disj:
            for (i = checkstart[f->index];;) {
#ifdef MODELCHECKING
                if (s->value[static_cast<booleanformula*>(f)->sub[i]->index]) {
                    break;
                }
#else
                if (static_cast<booleanformula*>(f)->sub[i]->value) {
                    break;
                }
#endif
                ++i;
                i %= static_cast<booleanformula*>(f)->cardsub;
                if (i == checkstart[f->index]) {
                    break;
                }
            }
            insert_down(s, static_cast<booleanformula*>(f)->sub[i]);
            break;

        case conj:
            for (i = 0; i < static_cast<booleanformula*>(f)->cardsub; i++) {
                insert_down(s, static_cast<booleanformula*>(f)->sub[i]);
            }
            break;

        case neg:
            insert_up(s, static_cast<unarybooleanformula*>(f)->sub);
            break;

#ifdef MODELCHECKING
        case ef: {
            State* ss;
            for (ss = s; !(ss->value[static_cast<unarytemporalformula*>(f)->element->index]); ss = ss->witness[f->tempindex]) {
                for (i = 0; ss->witnesstransition[f->tempindex]->conflicting[i]; i++) {
                    stubborninsert(ss->witnesstransition[f->tempindex]->conflicting[i]);
                }
                stubborninsert(ss->witnesstransition[f->tempindex]);
            }
            insert_down(ss, static_cast<unarytemporalformula*>(f)->element);
            break;
        }

        case ag:
            break;
        case eg: {
            State* ss;
            for (ss = s; (ss->checkmin[f->tempindex] < UINT_MAX); ss = ss->witness[f->tempindex]) {
                ss->checkmin[f->tempindex] = UINT_MAX;
                for (i = 0; ss->witnesstransition[f->tempindex]->conflicting[i]; i++) {
                    stubborninsert(ss->witnesstransition[f->tempindex]->conflicting[i]);
                }
                stubborninsert(ss->witnesstransition[f->tempindex]);
                insert_down(ss, static_cast<unarytemporalformula*>(f)->element);
            }
            for (ss = s; (ss->checkmin[f->tempindex] < UINT_MAX); ss = ss->witness[f->tempindex]) {
                ss->checkmin[f->tempindex] = 0;
            }
            break;
        }

        case eu: {
            State* ss;
            for (ss = s; !(ss->value[static_cast<untilformula*>(f)->goal->index]); ss = ss->witness[f->tempindex]) {
                for (i = 0; ss->witnesstransition[f->tempindex]->conflicting[i]; i++) {
                    stubborninsert(ss->witnesstransition[f->tempindex]->conflicting[i]);
                }
                stubborninsert(ss->witnesstransition[f->tempindex]);
                insert_down(ss, static_cast<untilformula*>(f)->hold);
            }
            insert_down(ss, static_cast<untilformula*>(f)->goal);
            break;
        }

        case af:
            if (s->value[static_cast<unarytemporalformula*>(f)->element->index]) {
                insert_down(s, static_cast<unarytemporalformula*>(f)->element);
            } else {
                insert_up(s, static_cast<unarytemporalformula*>(f)->element);
            }
            break;

        case au:
            if (s->value[static_cast<untilformula*>(f)->goal->index]) {
                insert_down(s, static_cast<untilformula*>(f)->goal);
            } else {
                insert_up(s, static_cast<untilformula*>(f)->goal);
            }
            break;

        case ax:
        case ex:
            for (i = 0; i < Globals::Transitions[0]->cnt; i++) {
                stubborninsert(Globals::Transitions[i]);
            }
            break;
#endif
        default:
            cout << "feature not implemented (yet)\n";
    }
}

void insert_global_down(formula* f) {
    unsigned int i;

    switch (f->type) {
        case neq:
            for (i = 0; i < static_cast<atomicformula*>(f)->p->NrOfArriving; i++) {
                stubborninsert(static_cast<atomicformula*>(f)->p->ArrivingArcs[i]->tr);
            }
            for (i = 0; i < static_cast<atomicformula*>(f)->p->NrOfLeaving; i++) {
                stubborninsert(static_cast<atomicformula*>(f)->p->LeavingArcs[i]->tr);
            }
            break;
        case eq:
            for (i = 0; i < static_cast<atomicformula*>(f)->p->NrOfArriving; i++) {
                stubborninsert(static_cast<atomicformula*>(f)->p->ArrivingArcs[i]->tr);
            }
            for (i = 0; i < static_cast<atomicformula*>(f)->p->NrOfLeaving; i++) {
                stubborninsert(static_cast<atomicformula*>(f)->p->LeavingArcs[i]->tr);
            }
            break;
        case geq:
        case gt:
            for (i = 0; i < static_cast<atomicformula*>(f)->p->NrOfLeaving; i++) {
                stubborninsert(static_cast<atomicformula*>(f)->p->LeavingArcs[i]->tr);
            }
            break;
        case leq:
        case lt:
            for (i = 0; i < static_cast<atomicformula*>(f)->p->NrOfArriving; i++) {
                stubborninsert(static_cast<atomicformula*>(f)->p->ArrivingArcs[i]->tr);
            }
            break;
        case disj:
        case conj:
            for (i = 0; i < static_cast<booleanformula*>(f)->cardsub; i++) {
                insert_global_down(static_cast<booleanformula*>(f)->sub[i]);
            }
            break;
        case neg:
            insert_global_up(static_cast<unarybooleanformula*>(f)->sub);
            break;
        default:
            cout << "feature not implemented (yet)\n";
    }
}

Transition** stubbornfirelist(State* s, formula* f) {
    Transition::StartOfStubbornList = NULL;
    insert_up(s, f);
    stubbornclosure();
    Transition** result = new Transition * [Transition::NrStubborn + 1];
    int i = 0;
    for (Transition* t = Transition::StartOfStubbornList; t; t = t->NextStubborn) {
        t->instubborn = false;
        if (t->enabled
#ifdef EXTENDEDCTL
                && (t->pathrestriction[TemporalIndex])
#endif
           ) {
            result[i++] = t;
        }
    }
    result[i] = NULL;
    CardFireList = i;
    return result;
}

Transition** stubbornfirelistpredicate() {
    return (F->value) ? NULL : stubbornfirelist(NULL, F);
}

Transition** stubbornfirelistneg(State* s, formula* f) {
    Transition::StartOfStubbornList = NULL;
    insert_down(s, f);
    stubbornclosure();
    Transition** result = new Transition * [Transition::NrStubborn + 1];
    int i = 0;
    for (Transition* t = Transition::StartOfStubbornList; t; t = t->NextStubborn) {
        t->instubborn = false;
        if (t->enabled
#ifdef EXTENDEDCTL
                && (t->pathrestriction[TemporalIndex])
#endif
           ) {
            result[i++] = t;
        }
    }
    result[i] = NULL;
    return result;
}

Transition** stubbornfirelistctl() {
    Transition** result;

    // stubborn does not work with X-operators
    if (xoperators) {
        return firelist();
    }

    // try for all enabled invisible transitions if there is
    // a stubborn superset without other enabled transitions
    for (Transition* start = Globals::Transitions[0]->StartOfEnabledList; start; start = start->NextEnabled) {
        if (start->visible) {
            continue;
        }
#ifdef EXTENDEDCTL
        if (!(start->pathrestriction[TemporalIndex])) {
            continue;
        }
#endif
        Transition::StartOfStubbornList = Transition::EndOfStubbornList = start;
        start->instubborn = true;
        start->NextStubborn = NULL;
        Transition* current;
        for (current = Transition::StartOfStubbornList; current; current = current->NextStubborn) {
            for (int i = 0; current->mustbeincluded[i]; i++) {
#ifdef EXTENDEDCTL
                if (!(current->mustbeincluded[i]->pathrestriction[TemporalIndex])) {
                    continue;
                }
#endif
                if (current->mustbeincluded[i]->enabled) {
                    if (current->mustbeincluded[i] != start) {
                        Transition* t;
                        for (t = Transition::StartOfStubbornList; t; t = t->NextStubborn) {
                            t->instubborn = false;
                        }
                        goto nextstart;
                    }
                }
                if (!current->mustbeincluded[i]->instubborn) {
                    current->mustbeincluded[i]->instubborn = true;
                    current->mustbeincluded[i]->NextStubborn  = NULL;
                    Transition::EndOfStubbornList->NextStubborn = current->mustbeincluded[i];
                    Transition::EndOfStubbornList = current->mustbeincluded[i];
                }
            }
        }
        result = new Transition*[2];
        result[0] = start;
        result[1] = NULL;
        for (current = Transition::StartOfStubbornList; current; current = current->NextStubborn) {
            current->instubborn = false;
        }
        return result;
nextstart:
        ;
    }

    return firelist();
}

/*!
 Computes a stubborn superset of a static attractor set. The attractor set
 is given as initial segment of the stubborn set list, terminated by
 LastAttractor.
*/
Transition** stubbornfireliststatic() {
    if (LastAttractor) {
        LastAttractor->NextStubborn = NULL;
        Transition::EndOfStubbornList = LastAttractor;
        stubbornclosure();
    }
    Transition** result = new Transition * [Transition::NrStubborn + 1];
    unsigned int i = 0;
    if (LastAttractor) {
        Transition* t;
        for (t = Transition::StartOfStubbornList;; t = t->NextStubborn) {
            if (t->enabled) {
                result[i++] = t;
            }
            if (t == LastAttractor) {
                break;
            }
        }
        for (t = t->NextStubborn; t; t = t->NextStubborn) {
            t->instubborn = false;
            if (t->enabled) {
                result[i++] = t;
            }
        }
    }
    result[i] = NULL;
    CardFireList = Transition::NrStubborn;
    return result;
}

/*!
 Computes a stubborn superset of a single enabled transition. We use the
 first element of enabling list.
*/
Transition** stubbornfirelistnogoal() {
    Transition::NrStubborn = 0;
    if (Transition::StartOfEnabledList) {
        Transition::StartOfEnabledList->NextStubborn = NULL;
        Transition::StartOfStubbornList = Transition::EndOfStubbornList = Transition::StartOfEnabledList;
        Transition::StartOfStubbornList->instubborn = true;
        stubbornclosure();
    } else {
        Transition::StartOfStubbornList = NULL;
    }
    Transition** result = new Transition * [Transition::NrStubborn + 1];
    unsigned int i = 0;
    for (Transition* t = Transition::StartOfStubbornList; t; t = t->NextStubborn) {
        t->instubborn = false;
        if (t->enabled) {
            result[i++] = t;
        }
    }
    result[i] = NULL;
    CardFireList = Transition::NrStubborn;
    return result;
}

/*!
 Computes a stubborn superset of an attractor set for the reachability of
 target_marking. Attractor set generation is controlled by the comparison
 between target and current marking.
*/
Transition** stubbornfirelistreach() {
    unsigned int i;
    for (i = 0; i < Globals::Places[0]->cnt; i++) {
        if (Globals::CurrentMarking[i] != Globals::Places[i]->target_marking) {
            break;
        }
    }
    if (i >= Globals::Places[0]->cnt) { // target_marking found!
        return NULL;
    }

    Transition** Attr = (Globals::CurrentMarking[i] > Globals::Places[i]->target_marking)
                        ? Globals::Places[i]->PostTransitions
                        : Globals::Places[i]->PreTransitions;

    Transition::StartOfStubbornList = NULL;
    for (i = 0; Attr[i]; i++) {
        Attr[i]->NextStubborn = Transition::StartOfStubbornList;
        Transition::StartOfStubbornList = Attr[i];
        Attr[i]->instubborn = true;
    }
    Transition::EndOfStubbornList = Attr[0];
    stubbornclosure();
    Transition** result = new Transition * [Transition::NrStubborn + 1];
    i = 0;
    for (Transition* t = Transition::StartOfStubbornList; t; t = t->NextStubborn) {
        if (t->enabled) {
            result[i++] = t;
        }
        t->instubborn = false;
    }
    result[i] = NULL;
    CardFireList = Transition::NrStubborn;
    return result;
}

/*!
 Computes stubborn set without goal orientation. The TSCC based optimisation
 is included.
*/
Transition** tsccstubbornlist() {
    unsigned int maxdfs;
    Transition* current, * next;

    // 1. start with enabled transition
    if ((Globals::Transitions[0]->TarjanStack = Globals::Transitions[0]->StartOfEnabledList)) {
        maxdfs = 0;
        NewStubbStamp();
        Globals::Transitions[0]->TarjanStack->nextontarjanstack = Globals::Transitions[0]->TarjanStack;
        Globals::Transitions[0]->TarjanStack->stamp  = StubbStamp;
        Globals::Transitions[0]->TarjanStack->dfs = Globals::Transitions[0]->TarjanStack->min = maxdfs++;
        Globals::Transitions[0]->TarjanStack->mbiindex = 0;
        current = Globals::Transitions[0]->TarjanStack;
        Globals::Transitions[0]->CallStack = current;
        current->nextoncallstack = NULL;
    } else {
        Transition** result = new Transition * [1];
        result[0] = NULL;
        CardFireList = 0;
        return result;
    }
    while (current) {
        if ((next = current->mustbeincluded[current->mbiindex])) {
            // Successor exists
            if (next->stamp == StubbStamp) {
                // already visited
                if (next->nextontarjanstack) {
                    // next still on stack
                    current->min = MINIMUM(current->min, next->dfs);
                }
                current->mbiindex++;
            } else {
                // not yet visited
                next->nextontarjanstack = Globals::Transitions[0]->TarjanStack;
                Globals::Transitions[0]->TarjanStack = next;
                next->min = next->dfs = maxdfs++;
                next->stamp = StubbStamp;
                next->mbiindex = 0;
                next->nextoncallstack = current;
                Globals::Transitions[0]->CallStack = next;
                current = next;
            }
        } else {
            // no more successors->scc detection and backtracking
            if (current->dfs == current->min) {
                // remove all states behind current from Tarjanstack;
                // if enabled->final sequence
                while (true) {
                    if (Globals::Transitions[0]->TarjanStack->enabled) {
                        // final sequence
                        unsigned int cardstubborn;
                        Transition* t;

                        cardstubborn = 0;
                        for (t = Globals::Transitions[0]->TarjanStack;; t = t->nextontarjanstack) {
                            if (t->enabled) {
                                cardstubborn ++;
                            }
                            if (t == current) {
                                break;
                            }
                        }
                        Transition** result = new Transition * [cardstubborn + 2];
                        cardstubborn = 0;
                        for (t = Globals::Transitions[0]->TarjanStack;; t = t->nextontarjanstack) {
                            if (t->enabled) {
                                result[cardstubborn++] = t;
                            }
                            if (t == current) {
                                result[cardstubborn] = NULL;
                                CardFireList = cardstubborn;
                                return result;
                            }
                        }
                    } else {
                        if (Globals::Transitions[0]->TarjanStack == current) {
                            break;
                        }
                        Globals::Transitions[0]->TarjanStack = Globals::Transitions[0]->TarjanStack->nextontarjanstack;
                    }
                }
            }
            // backtracking to previous state
            next = current->nextoncallstack;
            next->min = MINIMUM(current->min, next->min);
            next->mbiindex++;
            current = next;
        }
    }
}

Transition** relaxedstubbornset() {
#if defined(WITHFORMULA) && ! defined(MODELCHECKING)

#if !defined(LIVEPROP) || (defined(RELAXED) && defined(TWOPHASE))
    if (F->value) {
        return NULL;
    }
#endif
    Transition** firstattempt = tsccstubbornlist();
#if defined(LIVEPROP) && (!defined(RELAXED) || !defined(TWOPHASE))
    if (F->value) {
        return firstattempt;
    }
#endif
    unsigned int i;
    for (i = 0; firstattempt[i]; i++) {
        if (firstattempt[i]->down) {
            break;
        }
    }
    if (firstattempt[i]) {
        delete [] firstattempt;
        return stubbornfirelistpredicate();
    }
    return firstattempt;

#endif
}

Transition** structreachstubbornset() {
    // used: relaxed reachability, cycle detection by t-inv
#if defined(WITHFORMULA) && ! defined(MODELCHECKING) && defined(STRUCT)
    if (F->value) {
        return NULL;
    }
    Transition** firstattempt = tsccstubbornlist();
    unsigned int i;
    for (i = 0; firstattempt[i]; i++) {
        if (firstattempt[i]->cyclic) {
            break;
        }
    }
    if (firstattempt[i]) {
        // cycle detected, return stubborn closure of global UP set

        //delete [] firstattempt;
        Transition::StartOfStubbornList = NULL;
        insert_global_up(F);
        stubbornclosure();
        Transition** result = new Transition * [Transition::NrStubborn + 5];
        i = 0;
        for (Transition* t = Transition::StartOfStubbornList; t; t = t->NextStubborn) {
            t->instubborn = false;
            if (t->enabled) {
                result[i++] = t;
            }
        }
        result[i] = NULL;
        return result;
    }
    return firstattempt;
#endif
}

#ifdef STRUCT
Transition** structstubbornset() {
    Transition** reduced = tsccstubbornlist();
    bool Cycle = false;
    for (int i = 0; reduced[i]; i++) {
        if (reduced[i]->cyclic) {
            Cycle = true;
            break;
        }
    }
    if (Cycle) {
        delete [] reduced;
        return firelist();
    }
    return reduced;
}
#endif


#endif
