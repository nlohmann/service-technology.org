/*****************************************************************************\
 Copyright (c) 2008  Robert Danitz, Christian Gierds, Niels Lohmann,
                     Peter Massuthe, Richard Mu"ller, Janine Ott,
                     Jan Su"rmeli, Daniela Weinberg, Karsten Wolf,
                     Martin Znamirowski

 Copyright (c) 2007  Jan Bretschneider, Robert Danitz, Christian Gierds,
                     Kathrin Kaschner, Leonard Kern, Niels Lohmann,
                     Peter Massuthe, Daniela Weinberg, Karsten Wolf

 Copyright (c) 2006  Kathrin Kaschner, Peter Massuthe, Daniela Weinberg,
                     Karsten Wolf

 This file is part of Fiona, see <http://service-technology.org/fiona>.

 Fiona is free software; you can redistribute it and/or modify it under the
 terms of the GNU General Public License as published by the Free Software
 Foundation; either version 3 of the License, or (at your option) any later
 version.
 
 Fiona is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License along with
 Fiona (see file COPYING). If not, see <http://www.gnu.org/licenses/>.
\*****************************************************************************/

/*!
 * \file    formula.cc
 *
 * \brief   functions for expressing and evaluating the formula that represents
 *          the Petri net's final condition
 *
 * \author  responsible: Daniela Weinberg <weinberg@informatik.hu-berlin.de>
 */

#include "mynew.h"
#include "owfn.h"
#include "formula.h"
#include "debug.h"
#include <cassert>

//! \brief returns the longest chain of formulas connected by a single given operator type
//! \param FType type of the operator
//! \return returns 1
unsigned int atomicformula::counttype(FType) {
    return 1;
}


//! \brief returns the longest chain of formulas connected by a single given operator type
//! \param FType type of the operator
//! \return returns 1
unsigned int unarybooleanformula::counttype(FType) {
    return 1;
}


//! \brief returns the longest chain of formulas connected by a single given operator type
//! \param FType type of the operator
//! \return returns 1, or the sum of the subformulas counttypes if the type of this binary
//!         formula is matching the given type
unsigned int binarybooleanformula::counttype(FType t) {
    if (type == t) {
        return left->counttype(t) + right->counttype(t);
    }
    return 1;
}


//! \brief returns the longest chain of formulas connected by a single given operator type
//! \param FType type of the operator
//! \return returns 1, or the sum of the subformulas counttypes if the type of this boolean
//!         formula is matching the given type
unsigned int booleanformula::counttype(FType t) {
    if (type == t) {
        unsigned int i, c;
        for (i=0, c=0; i < cardsub; i++) {
            c += sub[i]->counttype(t);
        }
        return c;
    }
    return 1;
}


//! \brief returns an array of subformulas consisting of longest chain of subformulas connected by 
//!        a specified operator
//! \param ty type of the operator
//! \param subs array of subformulas to fill
//! \param pos current position in the array
//! \return the new position after subformulas have been added
unsigned int atomicformula::collectsubs(FType, formula** subs, unsigned int pos) {
    subs[pos++] = this;
    return pos;
}


//! \brief returns an array of subformulas consisting of longest chain of subformulas connected by 
//!        a specified operator
//! \param ty type of the operator
//! \param subs array of subformulas to fill
//! \param pos current position in the array
//! \return the new position after subformulas have been added
unsigned int unarybooleanformula::collectsubs(FType,
                                              formula** subs,
                                              unsigned int pos) {
    subs[pos++] = this;
    return pos;
}


//! \brief returns an array of subformulas consisting of longest chain of subformulas connected by 
//!        a specified operator
//! \param ty type of the operator
//! \param subs array of subformulas to fill
//! \param pos current position in the array
//! \return the new position after subformulas have been added
unsigned int binarybooleanformula::collectsubs(FType ty,
                                               formula** subs,
                                               unsigned int pos) {
    if (type == ty) {
        pos = left->collectsubs(ty, subs, pos);
        return right->collectsubs(ty, subs, pos);
    }
    subs[pos++] = this;
    return pos;
}


//! \brief returns an array of subformulas consisting of longest chain of subformulas connected by 
//!        a specified operator
//! \param ty type of the operator
//! \param subs array of subformulas to fill
//! \param pos current position in the array
//! \return the new position after subformulas have been added
unsigned int booleanformula::collectsubs(FType ty,
                                         formula** subs,
                                         unsigned int pos) {
    if (type == ty) {
        unsigned int i;
        for (i=0; i<cardsub; i++) {
            pos = sub[i]->collectsubs(ty, subs, pos);
        }
        return pos;
    }
    subs[pos++] = this;
    return pos;
}

//! \brief collects all places that are used in the formula
//! \param places set of places to be filled
void atomicformula::collectplaces(std::set<owfnPlace*>& places) {
    places.insert(p);
}


//! \brief collects all places that are used in the formula
//! \param places set of places to be filled
void unarybooleanformula::collectplaces(std::set<owfnPlace*>& places) {
    sub->collectplaces(places);
}


//! \brief collects all places that are used in the formula
//! \param places set of places to be filled
void binarybooleanformula::collectplaces(std::set<owfnPlace*>& places) {
    left->collectplaces(places);
    right->collectplaces(places);
}


//! \brief collects all places that are used in the formula
//! \param places set of places to be filled
void booleanformula::collectplaces(std::set<owfnPlace*>& places) {
    for (size_t isub = 0; isub != cardsub; ++isub) {
        sub[isub]->collectplaces(places);
    }
}


//! \brief constructor (3 parameters)
//! \param t type of operator being used
//! \param kk compare value for the operator
//! \param pp place in comparision
atomicformula::atomicformula(FType t, owfnPlace* pp, unsigned int kk) {
    type = t;
    p = pp;
    k = kk;
}


//! \brief constructor (2 parameters)
//! \param t type of operator being used
//! \param l subformula the operator is applied to
unarybooleanformula::unarybooleanformula(FType t, formula* l) {
    type = t;
    sub = l;
}


//! \brief deconstructor
unarybooleanformula::~unarybooleanformula() {
    TRACE(TRACE_5, "unarybooleanformula::~unarybooleanformula() : start\n");
    delete sub;
    TRACE(TRACE_5, "unarybooleanformula::~unarybooleanformula() : end\n");
}


//! \brief constructor (3 parameters)
//! \param t type of operator being used
//! \param l left subformula the operator is applied to
//! \param r right subformula the operator is applied to
binarybooleanformula::binarybooleanformula(FType t, formula* l, formula* r) {
    type = t;
    left = l;
    right = r;
}


//! \brief deconstructor
binarybooleanformula::~binarybooleanformula() {
    delete left;
    delete right;
}

//! \brief deconstructor
booleanformula::~booleanformula() {
    TRACE(TRACE_5, "booleanformula::~booleanformula() : start\n");
    for (size_t isub = 0; isub != cardsub; ++isub) {
        delete sub[isub];
    }

    delete[] sub;
    TRACE(TRACE_5, "booleanformula::~booleanformula() : end\n");
}


//! \brief checks whether this atomic formula is true with a value from the current marking
//! \param CurrentMarking number of tokens for all used places
//! \return returns true if the given value satisfies the formula, false else
bool atomicformula::init(unsigned int* CurrentMarking) {
    switch (type) {
        case eq:
            if (CurrentMarking[p->getUnderlyingOWFN()->getPlaceIndex(p)]==k)
                return (value=true);
            return (value=false);
        case neq:
            if (CurrentMarking[p->getUnderlyingOWFN()->getPlaceIndex(p)]!=k)
                return (value=true);
            return (value=false);
        case leq:
            if (CurrentMarking[p->getUnderlyingOWFN()->getPlaceIndex(p)]<=k)
                return (value=true);
            return (value=false);
        case geq:
            if (CurrentMarking[p->getUnderlyingOWFN()->getPlaceIndex(p)]>=k)
                return (value=true);
            return (value=false);
        case lt:
            if (CurrentMarking[p->getUnderlyingOWFN()->getPlaceIndex(p)] <k)
                return (value=true);
            return (value=false);
        case gt:
            if (CurrentMarking[p->getUnderlyingOWFN()->getPlaceIndex(p)] >k)
                return (value=true);
            return (value=false);
        case conj: /* fall through */
        case disj: /* fall through */
        case neg:
            assert(false);
            break; // should never happen, since this is an atomic formula
    }

    return false;
}


//! \brief checks whether this unary boolean formula is true with a given current marking
//! \param CurrentMarking number of tokens for all used places
//! \return returns true if the given values satisfies the formula, false else
bool unarybooleanformula::init(unsigned int* m) {
    if (sub->init(m))
        return (value = false);
    return (value = true);
}


//! \brief checks whether this binary boolean formula is true with a given current marking
//! \param CurrentMarking number of tokens for all used places
//! \return returns true if the given values satisfies the formula, false else
bool binarybooleanformula::init(unsigned int* m) {
    value = left->init(m);
    switch (type) {
        case conj:
            if (right->init(m))
                return value;
            return value = false;
        case disj:
            if (right->init(m))
                return value = true;
            return value;
        case eq: /* fall through */
        case neq: /* fall through */
        case geq: /* fall through */
        case leq: /* fall through */
        case lt: /* fall through */
        case gt: /* fall through */
        case neg:
            assert(false);
            break; // should never happen, since this is a binary boolean formula
    }
    return false;
}


//! \brief checks whether this boolean formula is true with a given current marking
//! \param CurrentMarking number of tokens for all used places
//! \return returns true if the given values satisfies the formula, false else
bool booleanformula::init(unsigned int* m) {
    firstvalid = cardsub;
    unsigned int n;

    n=0;
    while (n < firstvalid) {
        if (sub[n]->init(m)) {
            formula* tmp;
            firstvalid --;
            tmp = sub[firstvalid];
            sub[firstvalid] = sub[n];
            sub[n] = tmp;
            sub[n]->parentindex = n;
            sub[firstvalid]->parentindex = firstvalid;
        } else {
            n++;
        }
    }
    switch (type) {
        case conj:
            if (firstvalid) {
                return value = false;
            }
            return value = true;
        case disj:
            if (firstvalid < cardsub) {
                return value = true;
            }
            return value = false;
        case eq: /* fall through */
        case neq: /* fall through */
        case geq: /* fall through */
        case leq: /* fall through */
        case lt: /* fall through */
        case gt: /* fall through */
        case neg:
            assert(false);
            break; // should never happen, since this is a boolean formula
    }
    return false;
}


//! \brief returns a deep copy of this formula (which is a flat copy for an atomic formula)
//! \return  deep copy (flat copy)
atomicformula* atomicformula::deep_copy() {
    return flat_copy();
}


//! \brief returns a flat copy of this formula
//! \return flat copy
atomicformula* atomicformula::flat_copy() {
    atomicformula* f;
    f = new atomicformula(type, p, k);
    f->value = value;
    f->parent = parent;
    f->parentindex = parentindex;
    return f;
}


//! \brief returns a deep copy of this formula
//! \return deep copy
unarybooleanformula* unarybooleanformula::deep_copy() {
    unarybooleanformula* f;
    f = new unarybooleanformula(type, sub->deep_copy());
    f->value = value;
    f->parent = parent;
    f->parentindex = parentindex;
    f->sub->parent = f;
    return f;
}


//! \brief returns a flat copy of this formula
//! \return flat copy
unarybooleanformula* unarybooleanformula::flat_copy() {
    unarybooleanformula* f;
    f = new unarybooleanformula(type, sub);
    f->value = value;
    f->parent = parent;
    f->parentindex = parentindex;
    return f;
}


//! \brief returns a deep copy of this formula
//! \return deep copy
binarybooleanformula* binarybooleanformula::deep_copy() {
    binarybooleanformula* f;
    f = new binarybooleanformula(type, left->deep_copy(), right->deep_copy());
    f->value = value;
    f->parent = parent;
    f->parentindex = parentindex;
    f->left->parent = f->right->parent = f;
    return f;
}


//! \brief returns a flat copy of this formula
//! \return flat copy
binarybooleanformula* binarybooleanformula::flat_copy() {
    binarybooleanformula* f;
    f = new binarybooleanformula(type, left, right);
    f->value = value;
    f->parent = parent;
    f->parentindex = parentindex;
    return f;
}


//! \brief returns a deep copy of this formula
//! \return deep copy
booleanformula* booleanformula::deep_copy() {
    unsigned int i;
    booleanformula* f;

    f = new booleanformula();

    formula **newsub;
    newsub = new formula* [cardsub];
    for (i=0; i<cardsub; i++) {
        newsub[i] = sub[i]->deep_copy();
        newsub[i]->parent = f;
        newsub[i]->parentindex = i;
    }
    f->type = type;
    f->sub = newsub;
    f->cardsub = cardsub;
    f->firstvalid = firstvalid;
    f->value = value;
    f->parent = parent;
    f->parentindex = parentindex;
    return f;
}


//! \brief returns a flat copy of this formula
//! \return flat copy
booleanformula* booleanformula::flat_copy() {
    unsigned int i;
    booleanformula* f;
    f = new booleanformula();
    formula **newsub;
    newsub = new formula* [cardsub];
    for (i=0; i<cardsub; i++) {
        newsub[i] = sub[i];
    }
    f->type = type;
    f->sub = newsub;
    f->cardsub = cardsub;
    f->firstvalid = firstvalid;
    f->value = value;
    f->parent = parent;
    f->parentindex = parentindex;
    return f;
}


//! \brief Compress chains of AND or OR to single n-ary AND/OR and return a new
//!        formula.
//! \return compressed formula
formula* atomicformula::merge() {
    return deep_copy();
}


//! \brief Compress chains of AND or OR to single n-ary AND/OR and return a new
//!        formula.
//! \return compressed formula
formula* unarybooleanformula::merge() {
    unarybooleanformula* f = flat_copy();
    f->sub = f->sub->merge();
    f->sub->parent = f;
    return f;
}


//! \brief Compress chains of AND or OR to single n-ary AND/OR and return a new
//!        formula.
//! \return compressed formula
formula* binarybooleanformula::merge() {
    booleanformula* f;
    f = new booleanformula();
    f->type = type;
    f->cardsub = counttype(type);
    f->sub = new formula* [f->cardsub + 1];
    unsigned int i;
    i = left->collectsubs(type, f->sub, 0);
    i = right->collectsubs(type, f->sub, i);
    f->sub[f->cardsub] = (formula*) 0;
    for (i=0; i<f->cardsub; i++) {
        f->sub[i] = f->sub[i]->merge();
        f->sub[i]->parent = f;
        f->sub[i]->parentindex = i;
    }

    // order sub formulas
    f->firstvalid = f->cardsub;
    unsigned int n;

    n=0;
    while (n < f->firstvalid) {
        if (f->sub[n]->value) {
            formula* tmp;
            f->firstvalid --;
            tmp = f->sub[f->firstvalid];
            f->sub[f->firstvalid] = f->sub[n];
            f->sub[n] = tmp;
            f->sub[n]->parentindex = n;
            f->sub[f->firstvalid]->parentindex = f->firstvalid;
        } else {
            n++;
        }
    }

    // set value of new merged formula f
    switch (f->type) {
        case conj:
            if (f->firstvalid) {
                f->value = false;
            } else {
                f->value = true;
            }
            break;
        case disj:
            if (f->firstvalid < f->cardsub) {
                f->value = true;
            } else {
                f->value = false;
            }
            break;
        case eq: /* fall through */
        case neq: /* fall through */
        case geq: /* fall through */
        case leq: /* fall through */
        case lt: /* fall through */
        case gt: /* fall through */
        case neg:
            assert(false);
            break; // should never happen, since this is a binary boolean formula
    }

    return f;
}


//! \brief Compress chains of AND or OR to single n-ary AND/OR and return a new
//!        formula.
//! \return compressed formula
formula* booleanformula::merge() {
    booleanformula* f;
    f = new booleanformula();
    f->cardsub = counttype(type);
    f->type = type;
    f->sub = new formula* [f->cardsub + 1];
    unsigned int i, j;
    for (j=0, i=0; j<cardsub; j++) {
        i = sub[j]->collectsubs(type, f->sub, i);
    }
    f->sub[f->cardsub] = (formula*) 0;
    for (i=0; i<f->cardsub; i++) {
        f->sub[i] = f->sub[i]->merge();
        f->sub[i]->parent = f;
        f->sub[i]->parentindex = i;
    }

    // order sub formulas
    f->firstvalid = f->cardsub;
    unsigned int n;

    n=0;
    while (n < f->firstvalid) {
        if (f->sub[n]->value) {
            formula* tmp;
            f->firstvalid --;
            tmp = f->sub[f->firstvalid];
            f->sub[f->firstvalid] = f->sub[n];
            f->sub[n] = tmp;
            f->sub[n]->parentindex = n;
            f->sub[f->firstvalid]->parentindex = f->firstvalid;
        } else {
            n++;
        }
    }

    // set value of new merged formula f
    switch (f->type) {
        case conj:
            if (f->firstvalid) {
                f->value = false;
            } else {
                f->value = true;
            }
            break;
        case disj:
            if (f->firstvalid < f->cardsub) {
                f->value = true;
            } else {
                f->value = false;
            }
            break;
        case eq: /* fall through */
        case neq: /* fall through */
        case geq: /* fall through */
        case leq: /* fall through */
        case lt: /* fall through */
        case gt: /* fall through */
        case neg:
            assert(false);
            break; // should never happen, since this is a boolean formula
    }

    return f;
}


//! \brief set links to parents and from/to mentioned places
void atomicformula::setstatic() {
    if (!(p->proposition)) {
        p->proposition = new formula* [p->cardprop];
        p->cardprop = 0;
    }
    p->proposition[p->cardprop ++] = this;
}


//! \brief set links to parents and from/to mentioned places
void unarybooleanformula::setstatic() {
    sub->setstatic();
    sub->parent = this;
}


//! \brief set links to parents and from/to mentioned places
void binarybooleanformula::setstatic() {
    left->setstatic();
    right->setstatic();
    left->parent = right->parent = this;
}


//! \brief set links to parents and from/to mentioned places
void booleanformula::setstatic() {
    unsigned int i;
    for (i=0; i < cardsub; i++) {
        sub[i]->setstatic();
        sub[i]->parent = this;
        sub[i]->parentindex = i;
    }
}


unsigned int subindex;

//! \brief incremental re-calculation of partial formula
//!        m is new marking of place involved in this formula
void atomicformula::update(unsigned int m) {
    bool newvalue;
    newvalue = false;

    switch (type) {
        case eq:
            if (m == k)
                newvalue = true;
            break;
        case neq:
            if (m != k)
                newvalue = true;
            break;
        case leq:
            if (m <= k)
                newvalue = true;
            break;
        case geq:
            if (m >= k)
                newvalue = true;
            break;
        case gt:
            if (m > k)
                newvalue = true;
            break;
        case lt:
            if (m < k)
                newvalue = true;
            break;
        case conj: /* fall through */
        case disj: /* fall through */
        case neg:
            assert(false);
            break; // should never happen, since this is an atomic formula
    }

    if (newvalue != value) {
        value = newvalue;
        subindex = parentindex;
        if (parent)
            parent->update(m);
    }
}


//! \brief incremental re-calculation of partial formula
//!        m not used, just for type compliance with atomic formula
void unarybooleanformula::update(unsigned int m) {
    if (value) {
        value = false;
    } else {
        value = true;
    }
    if (parent) {
        subindex = parentindex;
        parent->update(m);
    }
}


//! \brief incremental re-calculation of partial formula
//!        m not used
void booleanformula::update(unsigned int m) {
    formula* tmp;
    bool newvalue;
    if (sub[subindex]->value) {
        firstvalid--;
        tmp=sub[firstvalid];
        sub[firstvalid] = sub[subindex];
        sub[subindex] = tmp;
        sub[firstvalid]->parentindex = firstvalid;
        sub[subindex]->parentindex = subindex;
    } else {
        tmp = sub[firstvalid];
        sub[firstvalid] = sub[subindex];
        sub[subindex] = tmp;
        sub[firstvalid]->parentindex = firstvalid;
        sub[subindex]->parentindex = subindex;
        firstvalid++;
    }
    if (type == conj) {
        if (firstvalid) {
            newvalue = false;
        } else {
            newvalue = true;
        }
    } else {
        if (firstvalid <cardsub) {
            newvalue = true;
        } else {
            newvalue = false;
        }
    }
    if (newvalue != value) {
        value = newvalue;
        subindex = parentindex;
        if (parent)
            parent->update(m);
    }
}


//! \brief remove negation in formulae without temporal
//! \return returns the modified formula
formula* atomicformula::posate() {
    p->cardprop ++;
    return this;
}


formula* atomicformula::negate() {
    switch (type) {
        case gt:
            type = leq;
            break;
        case geq:
            type = lt;
            break;
        case lt:
            type = geq;
            break;
        case leq:
            type = gt;
            break;
        case eq:
            type = neq;
            break;
        case neq:
            type = eq;
            break;
        case conj: /* fall through */
        case disj: /* fall through */
        case neg:
            assert(false);
            break; // should never happen, since this is an aomic formula
    }
    p->cardprop++;
    return this;
}


//! \brief remove negation in formulae without temporal
//! \return returns the modified formula
formula* unarybooleanformula::posate() {
    return sub->negate();
}


//! \brief negates the formula
//! \return returns the negated formula
formula* unarybooleanformula::negate() {
    return sub->posate();
}


//! \brief remove negation in formulae without temporal
//! \return returns the modified formula
formula* booleanformula::posate() {
    unsigned int i;
    for (i=0; i<cardsub; i++) {
        sub[i] = sub[i]->posate();
    }
    return this;
}


//! \brief negates the formula
//! \return returns the negated formula
formula* booleanformula::negate() {
    unsigned int i;
    for (i=0; i<cardsub; i++) {
        sub[i] = sub[i]->negate();
    }
    if (type == conj) {
        type = disj;
    } else {
        type = conj;
    }
    return this;
}
