#include "mynew.h"
#include"formula.h"

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
	
atomicformula::atomicformula(FType t, owfnPlace * pp, unsigned int kk) {
  unsigned int i;
  type = t;
  p = pp;
  k = kk;
}


unarybooleanformula::unarybooleanformula(FType t, formula * l) {
  type = t;
  sub = l;
}

binarybooleanformula::binarybooleanformula(FType t, formula * l, formula * r) {
  type = t;
  left = l;
  right = r;
}

bool atomicformula::init(unsigned int * CurrentMarking) {
	switch(type) {
		case  eq: if(CurrentMarking[p->index]==k) return(value=true); return(value=false);
		case neq: if(CurrentMarking[p->index]!=k) return(value=true); return(value=false);
		case leq: if(CurrentMarking[p->index]<=k) return(value=true); return(value=false);
		case geq: if(CurrentMarking[p->index]>=k) return(value=true); return(value=false);
		case  lt: if(CurrentMarking[p->index] <k) return(value=true); return(value=false);
		case  gt: if(CurrentMarking[p->index] >k) return(value=true); return(value=false);
	}
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
	}
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
	}
}


formula * atomicformula::copy() {
    atomicformula * f;
    f = new atomicformula(type,p,k);
    return f;
}

formula * unarybooleanformula::copy() {
    unarybooleanformula * f;
    f = new unarybooleanformula(type,sub -> copy());
    return f;
}

formula * binarybooleanformula::copy() {
    binarybooleanformula * f;
    f = new binarybooleanformula(type,left->copy(),right -> copy());
    return f;
}

formula * booleanformula::copy() {
    unsigned int i;
    booleanformula * f;
	formula **newsub;
    newsub = new formula * [cardsub];
    for(i=0;i<cardsub;i++) {
        newsub[i] = sub[i]->copy();
    }
    f = new booleanformula();
    f -> type = type;
    f -> sub = newsub;
    return f;
}

formula * atomicformula::merge() {
	return this;
}

formula * unarybooleanformula::merge() {
	sub = sub -> merge();
	sub -> parent = this;
	return this;
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
	for(i=0;i<f->cardsub;i++)
	{
		f -> sub[i] -> parent = f;
	}
	f -> sub[f -> cardsub] = (formula *) 0;
	for(i=0;i<f -> cardsub;i++)
	{
		f -> sub[i] = f -> sub[i]->merge();
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
	for(i=0;i<f->cardsub;i++)
	{
		f -> sub[i] -> parent = f;
	}
	f -> sub[f -> cardsub] = (formula *) 0;
	for(i=0;i<f -> cardsub;i++)
	{
		f -> sub[i] = f -> sub[i]->merge();
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

