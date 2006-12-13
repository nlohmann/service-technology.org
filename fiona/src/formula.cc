/*****************************************************************************
 * Copyright 2005, 2006 Peter Massuthe, Daniela Weinberg, Karsten Wolf       *
 *                      Jan Bretschneider, Christian Gierds                  *
 *                                                                           *
 * This file is part of Fiona.                                               *
 *                                                                           *
 * Fiona is free software; you can redistribute it and/or modify it          *
 * under the terms of the GNU General Public License as published by the     *
 * Free Software Foundation; either version 2 of the License, or (at your    *
 * option) any later version.                                                *
 *                                                                           *
 * Fiona is distributed in the hope that it will be useful, but WITHOUT      *
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or     *
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for  *
 * more details.                                                             *
 *                                                                           *
 * You should have received a copy of the GNU General Public License along   *
 * with Fiona; if not, write to the Free Software Foundation, Inc., 51       *
 * Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.                      *
 *****************************************************************************/

/*!
 * \file    formula.cc
 *
 * \brief   functions for expressing and evaluating the formula that represents
 *          the Petri net's final condition
 *
 * \author  responsible: Daniela Weinberg <weinberg@informatik.hu-berlin.de>
 *
 * \note    This file is part of the tool Fiona and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 */

#include "mynew.h"
#include "formula.h"

unsigned int atomicformula::counttype(FType t) {
	return 1;
}

unsigned int unarybooleanformula::counttype(FType t) {
	return 1;
}

unsigned int binarybooleanformula::counttype(FType t) {
	if(type == t) {
		return left -> counttype(t) + right -> counttype(t);
	}
	return 1;
}

unsigned int booleanformula::counttype(FType t) {
	if(type == t) {
		unsigned int i,c;
		for(i=0,c=0;i < cardsub;i++) {
			c += sub[i] -> counttype(t);
		}
		return c;
	}
	return 1;
}

unsigned int atomicformula::collectsubs(FType ty, formula ** subs, unsigned int pos) {
	subs[pos++] = this;
	return pos;
}
	
unsigned int unarybooleanformula::collectsubs(FType ty, formula ** subs, unsigned int pos) {
	subs[pos++] = this;
	return pos;
}
	
unsigned int binarybooleanformula::collectsubs(FType ty, formula ** subs, unsigned int pos) {
	if(type == ty) {
		pos = left -> collectsubs(ty,subs,pos);
		return right -> collectsubs(ty,subs,pos);
	}
	subs[pos++] = this;
	return pos;
}
	
unsigned int booleanformula::collectsubs(FType ty, formula ** subs, unsigned int pos) {
	if(type == ty) {
		unsigned int i;
		for(i=0;i<cardsub;i++) {
			pos = sub[i]->collectsubs(ty,subs,pos);
		}
		return pos;
	}
	subs[pos++] = this;
	return pos;
}
	
void atomicformula::collectplaces(std::set<owfnPlace*>& places)
{
	places.insert(p);
}

void unarybooleanformula::collectplaces(std::set<owfnPlace*>& places)
{
	sub->collectplaces(places);
}

void binarybooleanformula::collectplaces(std::set<owfnPlace*>& places)
{
	left->collectplaces(places);
	right->collectplaces(places);
}

void booleanformula::collectplaces(std::set<owfnPlace*>& places)
{
	for (size_t isub = 0; isub != cardsub; ++isub)
	{
		sub[isub]->collectplaces(places);
	}
}

atomicformula::atomicformula(FType t, owfnPlace * pp, unsigned int kk) {
//CG  unsigned int i;
  type = t;
  p = pp;
  k = kk;
}


unarybooleanformula::unarybooleanformula(FType t, formula * l) {
  type = t;
  sub = l;
}

unarybooleanformula::~unarybooleanformula()
{
    delete sub;
}

binarybooleanformula::binarybooleanformula(FType t, formula * l, formula * r) {
  type = t;
  left = l;
  right = r;
}

binarybooleanformula::~binarybooleanformula()
{
    delete left;
    delete right;
}

booleanformula::~booleanformula()
{
    for (size_t isub = 0; isub != cardsub; ++isub)
    {
        delete sub[isub];
    }

    delete[] sub;
}

bool atomicformula::init(unsigned int * CurrentMarking) {
    switch(type) {
        case  eq: if(CurrentMarking[p->index]==k) return(value=true); return(value=false);
	case neq: if(CurrentMarking[p->index]!=k) return(value=true); return(value=false);
	case leq: if(CurrentMarking[p->index]<=k) return(value=true); return(value=false);
	case geq: if(CurrentMarking[p->index]>=k) return(value=true); return(value=false);
	case  lt: if(CurrentMarking[p->index] <k) return(value=true); return(value=false);
	case  gt: if(CurrentMarking[p->index] >k) return(value=true); return(value=false);
        default: assert(false); /* should not happen, since this is an atomic formula */
    }

    return false;
}

bool unarybooleanformula::init(unsigned int * m) {
	if(sub -> init(m)) return (value = false);
	return(value = true);
}

bool binarybooleanformula::init(unsigned int * m) {
    value = left -> init(m);
    switch(type) {
        case conj: if(right -> init(m))
	               return value;
		   return value = false;
	case disj: if(right -> init(m))
                       return value = true;
		   return value;
        default: assert(false); /* should not happen, since this is a binary boolean formula */
    }
    return false;
}

bool booleanformula::init(unsigned int * m) {
    firstvalid = cardsub;
    unsigned int n;

    n=0;
    while(n < firstvalid) {
        if(sub[n] -> init(m)) {
	    formula * tmp;
	    firstvalid --;
	    tmp = sub[firstvalid];
	    sub[firstvalid] = sub[n];
	    sub[n] = tmp;
	    sub[n] -> parentindex = n;
	    sub[firstvalid] -> parentindex = firstvalid;
	} else {
	    n++;
	}
    }
    switch(type) {
	case conj:
		if(firstvalid) {
			return value = false;
		}
		return value = true;
	case disj:
		if(firstvalid < cardsub) {
			return value = true;
		}
		return value = false;
        default: assert(false); /* should not happen, since this is an boolean formula */
    }
    return false;
}


atomicformula * atomicformula::deep_copy() {
    return flat_copy();
}

atomicformula * atomicformula::flat_copy() {
    atomicformula * f;
    f              = new atomicformula(type,p,k);
    f->value       = value;
    f->parent      = parent;
    f->parentindex = parentindex;
    return f;
}

unarybooleanformula * unarybooleanformula::deep_copy() {
    unarybooleanformula * f;
    f = new unarybooleanformula(type,sub->deep_copy());
    f->value       = value;
    f->parent      = parent;
    f->parentindex = parentindex;
    f->sub->parent = f;
    return f;
}

unarybooleanformula * unarybooleanformula::flat_copy() {
    unarybooleanformula * f;
    f              = new unarybooleanformula(type,sub);
    f->value       = value;
    f->parent      = parent;
    f->parentindex = parentindex;
    return f;
}

binarybooleanformula * binarybooleanformula::deep_copy() {
    binarybooleanformula * f;
    f = new binarybooleanformula(type,left->deep_copy(),right->deep_copy());
    f->value = value;
    f->parent = parent;
    f->parentindex = parentindex;
    f->left->parent = f->right->parent = f;
    return f;
}

binarybooleanformula * binarybooleanformula::flat_copy() {
    binarybooleanformula * f;
    f = new binarybooleanformula(type,left,right);
    f->value = value;
    f->parent = parent;
    f->parentindex = parentindex;
    return f;
}

booleanformula * booleanformula::deep_copy() {
    unsigned int i;
    booleanformula * f;

    f              = new booleanformula();

    formula **newsub;
    newsub = new formula * [cardsub];
    for(i=0;i<cardsub;i++) {
        newsub[i] = sub[i]->deep_copy();
        newsub[i]->parent = f;
        newsub[i]->parentindex = i;
    }
    f->type        = type;
    f->sub         = newsub;
    f->cardsub     = cardsub;
    f->firstvalid  = firstvalid;
    f->value       = value;
    f->parent      = parent;
    f->parentindex = parentindex;
    return f;
}

booleanformula * booleanformula::flat_copy() {
    unsigned int i;
    booleanformula * f;
    f              = new booleanformula();
    formula **newsub;
    newsub = new formula * [cardsub];
    for(i=0;i<cardsub;i++) {
        newsub[i] = sub[i];
    }
    f->type        = type;
    f->sub         = newsub;
    f->cardsub     = cardsub;
    f->firstvalid  = firstvalid;
    f->value       = value;
    f->parent      = parent;
    f->parentindex = parentindex;
    return f;
}

formula * atomicformula::merge() {
    return deep_copy();
}

formula * unarybooleanformula::merge() {
    unarybooleanformula* f = flat_copy();
    f->sub = f->sub->merge();
    f->sub->parent = f;
    return f;
}


formula * binarybooleanformula::merge() {
	booleanformula * f;
	f = new booleanformula();
	f -> type = type;
	f -> cardsub = counttype(type);
	f -> sub = new formula * [f -> cardsub + 1];
	unsigned int i;
	i = left -> collectsubs(type,f -> sub,0);
	i = right -> collectsubs(type,f->sub,i);
	f -> sub[f -> cardsub] = (formula *) 0;
	for(i=0;i<f -> cardsub;i++)
	{
		f -> sub[i] = f -> sub[i]->merge();
		f -> sub[i] -> parent = f;
		f -> sub[i] -> parentindex = i;
	}

	// order sub formulas 
	f->firstvalid = f->cardsub;
	unsigned int n;

	n=0;
	while(n < f->firstvalid) {
		if(f->sub[n] -> value) {
			formula * tmp;
			f->firstvalid --;
			tmp = f->sub[f->firstvalid];
			f->sub[f->firstvalid] = f->sub[n];
			f->sub[n] = tmp;
			f->sub[n] -> parentindex = n;
			f->sub[f->firstvalid] -> parentindex = f->firstvalid;
		} else {
			n++;
		}
	}

	// set value of new merged formula f
	switch(f->type) {
	case conj:
		if(f->firstvalid) {
			f->value = false;
		} else {
			f->value = true;
		}
		break;
	case disj:
		if(f->firstvalid < f->cardsub) {
			f->value = true;
		} else {
			f->value = false;
		}
		break;
        default: assert(false); /* should not happen, since this is a binary boolean formula */
	}

	return f;
}
		

formula * booleanformula::merge()
{
	booleanformula * f;
	f = new booleanformula();
	f -> cardsub = counttype(type);
	f -> type = type;
	f -> sub = new formula * [f -> cardsub + 1];
	unsigned int i,j;
	for(j=0,i=0;j<cardsub;j++)
	{
		i = sub[j] -> collectsubs(type,f -> sub,i);
	}
	f -> sub[f -> cardsub] = (formula *) 0;
	for(i=0;i<f -> cardsub;i++)
	{
		f -> sub[i] = f -> sub[i]->merge();
		f -> sub[i] -> parent = f;
		f -> sub[i] -> parentindex = i;
	}

	// order sub formulas 
	f->firstvalid = f->cardsub;
	unsigned int n;

	n=0;
	while(n < f->firstvalid) {
		if(f->sub[n] -> value) {
			formula * tmp;
			f->firstvalid --;
			tmp = f->sub[f->firstvalid];
			f->sub[f->firstvalid] = f->sub[n];
			f->sub[n] = tmp;
			f->sub[n] -> parentindex = n;
			f->sub[f->firstvalid] -> parentindex = f->firstvalid;
		} else {
			n++;
		}
	}

	// set value of new merged formula f
	switch(f->type) {
	case conj:
		if(f->firstvalid) {
			f->value = false;
		} else {
			f->value = true;
		}
		break;
	case disj:
		if(f->firstvalid < f->cardsub) {
			f->value = true;
		} else {
			f->value = false;
		}
		break;
        default: assert(false); /* should not happen, since this is a boolean formula */
	}

	return f;
}
		
void atomicformula::setstatic()
{
	if(!(p -> proposition))
	{
		p -> proposition = new formula * [p -> cardprop];
		p -> cardprop = 0;
	}
	p -> proposition[p -> cardprop ++] = this;
}

void unarybooleanformula::setstatic()
{
	sub -> setstatic();
	sub -> parent = this;
}

void binarybooleanformula::setstatic()
{
	left -> setstatic();
	right -> setstatic();
left -> parent = right -> parent = this;
}

void booleanformula::setstatic()
{
	unsigned int i;
	for(i=0;i<cardsub;i++)
	{
		sub[i]-> setstatic();
		sub[i]-> parent = this;
		sub[i]->parentindex = i;
	}
}


unsigned int subindex;

void atomicformula::update(unsigned int m) // m is new marking of place involved in this formula
{
	bool newvalue;
	newvalue = false;
	
	switch(type)
	{
		case eq: if(m == k) newvalue = true; break;
		case neq: if(m != k) newvalue = true; break;
		case leq: if(m <= k) newvalue = true; break;
		case geq: if(m >= k) newvalue = true; break;
		case gt: if(m > k) newvalue = true; break;
		case lt: if(m < k) newvalue = true; break;
                default: assert(false); /* should not happen, since this is an atomic formula */
	}
	if(newvalue != value)
	{
		value = newvalue;
		subindex = parentindex;
		if(parent) parent -> update(m);
	}
}

void unarybooleanformula::update(unsigned int m)  // m not used, just for type compliance with atomic formula

{
	if(value)
	{
		value = false;
	}
	else
	{
		value = true;
	}
	if(parent)
	{
		subindex = parentindex;
		parent -> update(m);
	}
}

void booleanformula::update(unsigned int m) // m not used
{
	formula * tmp;
	bool newvalue;
	if(sub[subindex]->value)
	{
		firstvalid--;
		tmp=sub[firstvalid];
		sub[firstvalid] = sub[subindex];
		sub[subindex] = tmp;
		sub[firstvalid]->parentindex = firstvalid;
		sub[subindex] -> parentindex = subindex;
	}
	else
	{
		tmp = sub[firstvalid];
		sub[firstvalid] = sub[subindex];
		sub[subindex] = tmp;
		sub[firstvalid]->parentindex = firstvalid;
		sub[subindex] -> parentindex = subindex;
		firstvalid++;
	}
	if(type == conj)
	{
		if(firstvalid)
		{
			newvalue = false;
		}
		else
		{
			newvalue = true;
		}
	}
	else
	{
		if(firstvalid <cardsub)
		{
			newvalue = true;
		}
		else
		{
			newvalue = false;
		}
	}
	if(newvalue != value)
	{
		value = newvalue;
		subindex = parentindex;
		if(parent) parent -> update(m);
	}
}


formula * atomicformula::posate()
{
	p -> cardprop ++;
	return this;
}

formula * atomicformula::negate()
{
	switch(type)
	{
	case gt: type = leq; break;
	case geq: type = lt; break;
	case lt: type = geq; break;
	case leq: type = gt; break;
	case eq: type = neq; break;
	case neq: type = eq; break;
        default: assert(false); /* should not happen, since this is an atomic formula */
	}
	p->cardprop++;
	return this;
}

formula * unarybooleanformula::posate()
{
	return sub -> negate();
}

formula * unarybooleanformula::negate()
{
	return sub -> posate();
}

formula * booleanformula::posate()
{
	unsigned int i;
	for(i=0;i<cardsub;i++)
	{
		sub[i] = sub[i]->posate();
	}
	return this;
}

formula * booleanformula::negate()
{
	unsigned int i;
	for(i=0;i<cardsub;i++)
	{
		sub[i] = sub[i] -> negate();
	}
	if(type == conj)
	{
		type = disj;
	}
	else
	{
		type = conj;
	}
	return this;
}

