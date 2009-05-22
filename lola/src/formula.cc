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


#include "formula.H"
#include "graph.H"
#include "unfold.H"
#include <cassert>

formula * F;
unsigned int formula::card = 0;
unsigned int formula::tempcard = 0;

unsigned int TemporalIndex;
bool * DeadStatePathRestriction;
unsigned int xoperators = 0;

void yyerror(char const *);

hlatomicformula::hlatomicformula(FType t, PlSymbol * pp, UExpression * te)
{
  type = t;
  p = (Place *) 0;
  ps = pp;
  color = te;
}

unsigned int atomicformula::counttype(FType t)
{
	return 1;
}

unsigned int staticformula::counttype(FType t)
{
	return 1;
}

unsigned int transitionformula::counttype(FType t)
{
	return 1;
}

unsigned int unarybooleanformula::counttype(FType t)
{
	return 1;
}

unsigned int binarybooleanformula::counttype(FType t)
{
	if(type == t)
	{
		return left -> counttype(t) + right -> counttype(t);
	}
	return 1;
}

unsigned int booleanformula::counttype(FType t)
{
	if(type == t)
	{
		unsigned int i,c;
		for(i=0,c=0;i < cardsub;i++)
		{
			c += sub[i] -> counttype(t);
		}
		return c;
	}
	return 1;
}

unsigned int unarytemporalformula::counttype(FType t)
{
	return 1;
}

unsigned int untilformula::counttype(FType t)
{
	return 1;
}

unsigned int transitionformula::collectsubs(FType ty, formula ** subs, unsigned int pos)
{
	subs[pos++] = this;
	return pos;
}

unsigned int staticformula::collectsubs(FType ty, formula ** subs, unsigned int pos)
{
	subs[pos++] = this;
	return pos;
}

unsigned int atomicformula::collectsubs(FType ty, formula ** subs, unsigned int pos)
{
	subs[pos++] = this;
	return pos;
}

unsigned int unarybooleanformula::collectsubs(FType ty, formula ** subs, unsigned int pos)
{
	subs[pos++] = this;
	return pos;
}

unsigned int binarybooleanformula::collectsubs(FType ty, formula ** subs, unsigned int pos)
{
	if(type == ty)
	{
		pos = left -> collectsubs(ty,subs,pos);
		return right -> collectsubs(ty,subs,pos);
	}
	subs[pos++] = this;
	return pos;
}

unsigned int booleanformula::collectsubs(FType ty, formula ** subs, unsigned int pos)
{
	if(type == ty)
	{
		unsigned int i;
		for(i=0;i<cardsub;i++)
		{
			pos = sub[i]->collectsubs(ty,subs,pos);
		}
		return pos;
	}
	subs[pos++] = this;
	return pos;
}

unsigned int unarytemporalformula::collectsubs(FType ty, formula ** subs, unsigned int pos)
{
	tempindex = tempcard++;
	subs[pos++] = this;
	return pos;
}

unsigned int untilformula::collectsubs(FType ty, formula ** subs, unsigned int pos)
{
	tempindex = tempcard++;
	subs[pos++] = this;
	return pos;
}


atomicformula::atomicformula(FType t, Place * pp, unsigned int kk)
{
  type = t;
  p = pp;
  k = kk;
  // mark environment of p as visible
#ifdef STUBBORN
  for(unsigned int i=0; p -> PreTransitions[i];i++)
  {
	p -> PreTransitions[i] -> visible = true;
  }
  for(unsigned int i=0; p -> PostTransitions[i];i++)
  {
	p -> PostTransitions[i] -> visible = true;
  }
#endif
}

transitionformula::transitionformula(Transition * t)
{
	transition = t;
	hltransition = (TrSymbol *) 0;
	firingmode = (fmode *) 0;
}

transitionformula::transitionformula(TrSymbol * t, fmode * mode)
{
	type = trans;
	transition = (Transition *) 0;
	hltransition = t;
	firingmode = mode;
}

quantifiedformula::quantifiedformula(FType t, UVar * v, formula * l)
{
  var = v;
  type = t;
  sub = l;
}

unarytemporalformula::unarytemporalformula(FType t, formula * l, formula * tf)
{
  type = t;
  element = l;
  tformula = tf;
}

unarybooleanformula::unarybooleanformula(FType t, formula * l)
{
  type = t;
  sub = l;
}

binarybooleanformula::binarybooleanformula(FType t, formula * l, formula * r)
{
  type = t;
  left = l;
  right = r;
}

untilformula::untilformula(FType t, formula * l, formula * r, formula * tf)
{
  type = t;
  hold = l;
  goal = r;
  tformula = tf;
}







void atomicformula::evaluateatomic(State * s)
{
#ifdef MODELCHECKING
	switch(type)
	{
	case eq: if(CurrentMarking[p->index] == k)
		 {
			s -> value[index] = true;

                 }
		 else
		 {
			s -> value[index] = false;
                 }
                 s -> known[index] = true;
                 break;
	case neq: if(CurrentMarking[p->index] != k)
		 {
			s -> value[index] = true;

                 }
		 else
		 {
			s -> value[index] = false;
                 }
                 s -> known[index] = true;
                 break;
	case leq: if(CurrentMarking[p->index] <= k)
		 {
			s -> value[index] = true;

                 }
		 else
		 {
			s -> value[index] = false;
                 }
                 s -> known[index] = true;
                 break;
	case geq: if(CurrentMarking[p->index] >= k)
		 {
			s -> value[index] = true;

                 }
		 else
		 {
			s -> value[index] = false;
                 }
                 s -> known[index] = true;
                 break;
	case lt: if(CurrentMarking[p->index] < k)
		 {
			s -> value[index] = true;

                 }
		 else
		 {
			s -> value[index] = false;
                 }
                 s -> known[index] = true;
                 break;
	case gt: if(CurrentMarking[p->index] > k)
		 {
			s -> value[index] = true;

                 }
		 else
		 {
			s -> value[index] = false;
                 }
                 s -> known[index] = true;
                 break;
	}
#endif
}


void unarybooleanformula::evaluateatomic(State * s)
{
	sub -> evaluateatomic(s);
}


void binarybooleanformula::evaluateatomic(State * s)
{
	left -> evaluateatomic(s);
	right -> evaluateatomic(s);
}


void booleanformula::evaluateatomic(State * s)
{
	unsigned int i;
	for(i=0;i<cardsub;i++)
	{
		sub[i] -> evaluateatomic(s);
	}
}


void unarytemporalformula::evaluateatomic(State * s)
{
	element -> evaluateatomic(s);
}


void untilformula::evaluateatomic(State * s)
{
	hold -> evaluateatomic(s);
	goal -> evaluateatomic(s);
}

bool atomicformula::initatomic()
{
	switch(type)
	{
		case  eq: if(CurrentMarking[p->index]==k) return(value=true); return(value=false);
		case neq: if(CurrentMarking[p->index]!=k) return(value=true); return(value=false);
		case leq: if(CurrentMarking[p->index]<=k) return(value=true); return(value=false);
		case geq: if(CurrentMarking[p->index]>=k) return(value=true); return(value=false);
		case  lt: if(CurrentMarking[p->index] <k) return(value=true); return(value=false);
		case  gt: if(CurrentMarking[p->index] >k) return(value=true); return(value=false);
		
		// Karsten muss das checken!
        default: /* this should not happen */ assert(false); return false;
	}
}

bool unarybooleanformula::initatomic()
{
	if(sub -> initatomic()) return (value = false);
	return(value = true);
}

bool binarybooleanformula::initatomic()
{
	value = left -> initatomic();
	switch(type)
	{
		case conj: if(right -> initatomic()) return value;
				   return value = false;
		case disj: if(right -> initatomic()) return value = true;
				   return value;
				   
	    // Karsten fragen
    	default: /* this should not happen */ assert(false); return false;
	}
}

bool booleanformula::initatomic()
{
	firstvalid = cardsub;
	unsigned int m;

	m=0;
	while(m < firstvalid)
	{
		if(sub[m] -> initatomic())
		{
			formula * tmp;
			firstvalid --;
			tmp = sub[firstvalid];
			sub[firstvalid] = sub[m];
			sub[m] = tmp;
			sub[m] -> parentindex = m;
			sub[firstvalid] -> parentindex = firstvalid;
		}
		else
		{
			m++;
		}
	}
	switch(type)
	{
	case conj:
		if(firstvalid)
		{
			return value = false;
		}
		return value = true;
	case disj:
		if(firstvalid < cardsub)
		{
			return value = true;
		}
		return value = false;
		
	// Karsten fragen
	default: /* this should not happen */ assert(false); return false;
	}	
}

bool unarytemporalformula::initatomic()
{
	element -> initatomic();
	
	// Karsten fragen: Rückgabe?
}

bool untilformula::initatomic()
{
	hold -> initatomic();
	goal -> initatomic();
	
	// Karsten fragen: Rückgabe?
}

formula * hlatomicformula::copy()
{
	hlatomicformula * f;
	f = new hlatomicformula(type,ps,color);
	f -> k = k;
	return f;
}

formula * transitionformula::copy()
{
    transitionformula * f;
    if(transition)
    {
        f = new transitionformula(transition);
    }
    else
    {
        f = new transitionformula(hltransition,firingmode);
    }
    f -> type = type;
    return f;
}

formula * atomicformula::copy()
{
    atomicformula * f;
    f = new atomicformula(type,p,k);
    return f;
}

formula * unarybooleanformula::copy()
{
    unarybooleanformula * f;
    f = new unarybooleanformula(type,sub -> copy());
    return f;
}

formula * binarybooleanformula::copy()
{
    binarybooleanformula * f;
    f = new binarybooleanformula(type,left->copy(),right -> copy());
    return f;
}

formula * booleanformula::copy()
{
    unsigned int i;
    booleanformula * f;
	formula **newsub;
    newsub = new formula * [cardsub];
    for(i=0;i<cardsub;i++)
    {
        newsub[i] = sub[i]->copy();
    }
    f = new booleanformula();
    f -> type = type;
    f -> sub = newsub;
    return f;
}

formula * unarytemporalformula::copy()
{
    unarytemporalformula * f;
	if(tformula)
	{
    f = new unarytemporalformula(type,element -> copy(),tformula -> copy());
	}
	else
	{
    f = new unarytemporalformula(type,element -> copy(),(transitionformula *) 0);
	}
    return f;
}

formula * untilformula::copy()
{
	untilformula * f;
	if(tformula)
	{
	f = new untilformula(type,hold -> copy(),goal -> copy(),tformula -> copy());
	}
	else
	{
	f = new untilformula(type,hold -> copy(),goal -> copy(),(transitionformula *) 0);
	}
	return f;
}

formula * quantifiedformula::copy()
{
	quantifiedformula * f;
	f = new quantifiedformula(type,var, sub->copy());
	return f;
}

formula * transitionformula::replacequantifiers()
{
	if(transition) return this;

		TrSymbol * TS;
           unsigned int i, j;
           fmode * fm;
           VaSymbol * v;
           UValue * vl;
           char ** cc;
           char ** inst;
           unsigned int len;
           cc = new char * [hltransition -> vars ->  card];
           inst= new char * [hltransition -> vars -> card];
           len = strlen(hltransition ->name) + 4;
           j=0;
           for(i=0;i<hltransition -> vars ->size;i++)
           {
            for(v = (VaSymbol *) (hltransition -> vars  -> table[i]);v; v = (VaSymbol *) (v -> next))
            {
				UValue * pl;
                for(fm=firingmode;fm;fm=fm ->next)
                {
                    if(fm -> v == v) break;
                }
                if(!fm) yyerror("firing mode incomplete");
                vl = fm -> t -> evaluate();
				pl = v -> var -> type -> make();
				pl -> assign(vl);
                cc[j] = pl -> text();
                inst[j] = new char [strlen(v -> name)+strlen(cc[j]) + 2];
                strcpy(inst[j],v -> name);
                strcpy(inst[j]+strlen(inst[j]),"=");
                strcpy(inst[j]+strlen(inst[j]),cc[j]);
                len += strlen(inst[j]) + 1;
                j++;
            }
           }
           char * llt;
           llt = new char[len];
           strcpy(llt,hltransition -> name);
           strcpy(llt+strlen(llt),".[");
           for(j=0;j<hltransition->vars->card;j++)
           {
            strcpy(llt + strlen(llt),inst[j]);
            strcpy(llt + strlen(llt),"|");
           }
           strcpy(llt + (strlen(llt) - 1),"]");
           TS = (TrSymbol *) TransitionTable -> lookup(llt);
           if(!TS) yyerror("transition instance does not exist");
           if(TS -> vars && TS -> vars -> card)
           {
            yyerror("HL and LL transition names mixed up");
           }
           transition = TS -> transition;
		   hltransition = (TrSymbol *) 0;
		   firingmode = (fmode *) 0;
		   return this;
}

formula * atomicformula::replacequantifiers()
{
	return this;
}

formula * unarybooleanformula::replacequantifiers()
{
	sub = sub -> replacequantifiers();
	return this;
}

formula * binarybooleanformula::replacequantifiers()
{
	left = left -> replacequantifiers();
	right = right -> replacequantifiers();
	return this;
}

formula * booleanformula::replacequantifiers()
{
	unsigned int i;
	for(i=0;i< cardsub;i++)
	{
		sub[i] = sub[i]->replacequantifiers();
	}
	return this;
}

formula * unarytemporalformula::replacequantifiers()
{
	element = element -> replacequantifiers();
	if(tformula) tformula = tformula -> replacequantifiers();
	return this;
}

formula * untilformula::replacequantifiers()
{
	hold = hold -> replacequantifiers();
	goal = goal -> replacequantifiers();
	if(tformula) tformula = tformula -> replacequantifiers();
	return this;
}

formula * hlatomicformula::replacequantifiers()
{
	atomicformula * f;
	int kk;
	UValue * vl;
	vl = k -> evaluate();
	if(k < 0) yyerror("atomic formula compares with negative value");
	kk = ((UNumValue *) vl) -> v;
	if(!color)
	{
		return new atomicformula(type, ps -> place, kk);
	}

	UValue * pv;
	pv = ps -> sort -> make();
	vl = color -> evaluate();
	pv -> assign(vl);
	char * inst;
	inst = pv -> text();
	char * ll;
	ll = new char [strlen(ps -> name) + strlen(inst) + 2];
	strcpy(ll, ps-> name);
	strcpy(ll + strlen(ll),".");
	strcpy(ll + strlen(ll),inst);
	ps = (PlSymbol *) PlaceTable -> lookup(ll);
	if(!ps) yyerror("place instance does not exist");
	if(ps->sort) yyerror("mixed up HL and LL place names");
	f = new atomicformula(type,ps -> place,kk);
	return f;
}

formula * quantifiedformula::replacequantifiers()
{
	unsigned int i;
	booleanformula * f;
	f = new booleanformula();
	if(type == qa)
	{
		f -> type = conj;
	}
	else
	{
		f -> type = disj;
	}
	f -> cardsub = var -> type -> size;
	f -> sub = new formula * [var -> type -> size + 1];
	f -> sub[0] = sub;
	for(i=1;i<var -> type -> size;i++)
	{
		f -> sub[i] = sub -> copy();
	}
	i = 0;
	do
	{
		f -> sub[i] = f -> sub[i] -> replacequantifiers();
		(*(var -> get()))++;
		i++;
	}
	while (!(var -> get()->isfirst()));
	return f;
}

formula * staticformula::merge()
{
	return this;
}

formula * transitionformula::merge()
{
	return this;
}

formula * atomicformula::merge()
{
	return this;
}

formula * unarybooleanformula::merge()
{
	sub = sub -> merge();
	sub -> parent = this;
	return this;
}

formula * unarytemporalformula::merge()
{
	element = element -> merge();
	tempcard++;
	element -> parent = this;
	if(tformula) tformula = tformula -> merge();
	if(tformula) tformula -> parent = this;
	return this;
}

formula * untilformula::merge()
{
	hold = hold -> merge();
	hold -> parent = this;
	goal = goal -> merge();
	goal -> parent = this;
	tempcard++;
	if(tformula) tformula = tformula -> merge();
	if(tformula) tformula -> parent = this;
	return this;
}

formula * binarybooleanformula::merge()
{
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
		f -> sub[i] -> parentindex = i;
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
		f -> sub[i] -> parentindex = i;
	}
	f -> sub[f -> cardsub] = (formula *) 0;
	for(i=0;i<f -> cardsub;i++)
	{
		f -> sub[i] = f -> sub[i]->merge();
	}
	return f;
}

bool transitionformula::evaluatetransition(Transition * t)
{
	if(transition == t) return true;
	return false;
}

bool unarybooleanformula::evaluatetransition(Transition *  t)
{
	if(sub -> evaluatetransition(t)) return false;
	return true;
}

bool binarybooleanformula::evaluatetransition(Transition * t)
{
	if(left -> evaluatetransition(t))
	{
		if(type == disj) return true;
	}
	else
	{
		if(type == conj) return false;
	}
	return left -> evaluatetransition(t);
}

bool booleanformula::evaluatetransition(Transition * t)
{
	unsigned int i;

	if(type == conj)
	{
		for(i=0;i<cardsub;i++)
		{
			if(!(sub[i]->evaluatetransition(t)))
			{
				return false;
			}
		}
		return true;
	}
	else
	{
		for(i=0;i<cardsub;i++)
		{
			if(sub[i]->evaluatetransition(t))
			{
				return true;
			}
		}
		return false;
	}
}

void atomicformula::setstatic()
{
#ifdef WITHFORMULA
	if(!(p -> propositions))
	{
		p -> propositions = new formula * [p -> cardprop+10];
		p -> cardprop = 0;
	}
	p -> propositions[p -> cardprop ++] = this;
#endif
	index = card ++;
#if defined(STUBBORN) && (defined(STATEPREDICATE) || defined(LIVEPROP))
	// compute global down set for state predicate
	unsigned int i;
	switch(type)
	{
	case lt:
	case leq: for(i=0;i<p -> NrOfArriving;i++)
			  {
				p -> ArrivingArcs[i]->tr->down = true;
			  }
			  break;
	case gt:
	case geq: for(i=0;i<p -> NrOfLeaving;i++)
			  {
				p -> LeavingArcs[i]->tr->down = true;
			  }
			  break;
	case eq:
	case neq: for(i=0;i<p -> NrOfArriving;i++)
			  {
				p -> ArrivingArcs[i]->tr->down = true;
			  }
			  for(i=0;i<p -> NrOfLeaving;i++)
			  {
				p -> LeavingArcs[i]->tr->down = true;
			  }
			  break;
	}
#endif
}

void unarybooleanformula::setstatic()
{
	sub -> setstatic();
	sub -> parent = this;
	index = card ++;
}

void binarybooleanformula::setstatic()
{
	left -> setstatic();
	right -> setstatic();
left -> parent = right -> parent = this;
	index = card ++;
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
	index = card++;
}

void untilformula::setstatic()
{

#ifdef WITHFORMULA
	hold -> setstatic();
hold -> parent = this;
	goal -> setstatic();
goal -> parent = this;
	index = card++;
	tempindex = tempcard++;
	unsigned int i;
	if(tformula)
	{
		for(i=0;i<Transitions[0]->cnt;i++)
		{
			Transitions[i]->pathrestriction[tempindex] = tformula -> evaluatetransition(Transitions[i]);
		}
		DeadStatePathRestriction[tempindex] = tformula -> evaluatetransition((Transition *) 0);
	}
	else
	{
		for(i=0;i<Transitions[0]->cnt;i++)
		{
			Transitions[i]->pathrestriction[tempindex] = true;
		}
		DeadStatePathRestriction[tempindex] = true;
	}
#endif
}


void unarytemporalformula::setstatic()
{
#ifdef WITHFORMULA
	element -> setstatic();
	element -> parent = this;
	index = card++;
	tempindex = tempcard++;
	unsigned int i;
	if(tformula)
	{
		for(i=0;i<Transitions[0]->cnt;i++)
		{
			Transitions[i]->pathrestriction[tempindex] = tformula -> evaluatetransition(Transitions[i]);
		}
		DeadStatePathRestriction[tempindex] = tformula -> evaluatetransition((Transition *) 0);
	}
	else
	{
		for(i=0;i<Transitions[0]->cnt;i++)
		{
			Transitions[i]->pathrestriction[tempindex] = true;
		}
		DeadStatePathRestriction[tempindex] = true;
	}
	if(type == ex || type == ax)
	{
		xoperators++;
	}
#endif
}

unsigned int subindex;

void atomicformula::updateatomic()
{
	bool newvalue;
	newvalue = false;

	switch(type)
	{
		case eq: if(CurrentMarking[p -> index] == k) newvalue = true; break;
		case neq: if(CurrentMarking[p -> index] != k) newvalue = true; break;
		case leq: if(CurrentMarking[p -> index] <= k) newvalue = true; break;
		case geq: if(CurrentMarking[p -> index] >= k) newvalue = true; break;
		case gt: if(CurrentMarking[p -> index] > k) newvalue = true; break;
		case lt: if(CurrentMarking[p -> index] < k) newvalue = true; break;
		
        default: break;
	}
	if(newvalue != value)
	{
		value = newvalue;
		subindex = parentindex;
		if(parent) parent -> updateatomic();
	}
}

void unarybooleanformula::updateatomic()
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
		parent -> updateatomic();
	}
}

void booleanformula::updateatomic()
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
		if(parent) parent -> updateatomic();
	}
}

formula * atomicformula::reduce(int * res)
{
	atomic = true;
	if(p -> references)
	{
		* res = 2;
		return this;
	}
	switch(type)
	{
	case eq: if(CurrentMarking[p -> index] == k) * res = 1;else * res = 0; break;
	case neq: if(CurrentMarking[p -> index] != k) * res = 1;else * res = 0; break;
	case leq: if(CurrentMarking[p -> index] <= k) * res = 1;else * res = 0; break;
	case geq: if(CurrentMarking[p -> index] >= k) * res = 1;else * res = 0; break;
	case lt: if(CurrentMarking[p -> index] < k) * res = 1;else * res = 0; break;
	case gt: if(CurrentMarking[p -> index] > k) * res = 1;else * res = 0; break;
    default: break;
	}
	return (formula *) 0;
}

formula * unarybooleanformula::reduce(int * res)
{
    int subres;
    sub = sub -> reduce(& subres);
    if(sub) atomic = sub -> atomic;
    switch(subres)
    {
        case 0: * res = 1; return (formula *) 0;
        case 1: * res = 0; return (formula *) 0;
        case 2: * res = 2; return this;
        
        // Karsten fragen
        default: /* this should not happen */ assert(false); return false;
    }
}

formula * binarybooleanformula::reduce(int * res)
{
    int subres1, subres2;

    left = left -> reduce(& subres1);
    right = right -> reduce(& subres2);
    if(type == conj)
    {
        switch(subres1)
        {
            case 0: * res = 0; return (formula *) 0;
            case 1: * res = subres2; if(right) atomic = right -> atomic; return right;
            case 2: switch(subres2)
            {
                case 0: * res = 0; return (formula *) 0;
                case 1: * res = 2; if(left) atomic = left -> atomic; return left;
                case 2: * res = 2; if(left -> atomic && right -> atomic) atomic = true; else atomic = false; return this;
            }
        }
    }
    else
    {
        switch(subres1)
        {
            case 0: * res = subres2; if(right) atomic = right -> atomic; return right;
            case 1: * res = 1; return (formula *) 0;
            case 2: switch(subres2)
            {
                case 0: * res = 2; if(left) atomic = left -> atomic; return left;
                case 1: * res = 1; return (formula *) 0;
                case 2: * res = 2; if(left -> atomic && right -> atomic) atomic = true;else atomic = false; return this;
            }
        }
    }
    
    // Karsten fragen
    /* this should not happen */ assert(false); return false;
}

formula * booleanformula::reduce(int * res)
{
	int subres;
	unsigned int i;
	atomic = true;
	if(type == conj)
	{
		i = 0;
		while(i<cardsub)
		{
			sub[i] = sub[i] -> reduce(& subres);
			if(sub[i] && (sub[i] -> atomic == false)) atomic = false;
			switch(subres)
			{
			case 0: * res = 0; return (formula *) 0;
			case 1: sub[i] = sub[cardsub - 1]; cardsub --; break;
			case 2: i++;break;
			}
		}
		if(cardsub == 0)
		{
			* res = 1; return (formula *) 0;
		}
		else
		{
			if(cardsub == 1)
			{
				* res = 2;
				return sub[0];
			}
			else
			{
				* res = 2;
				return this;
			}
		}
	}
	else
	{
		i = 0;
		while(i<cardsub)
		{
			sub[i] = sub[i] -> reduce(& subres);
			if(sub[i] && (sub[i] -> atomic == false)) atomic = false;
			switch(subres)
			{
			case 1: * res = 1; return (formula *) 0;
			case 0: sub[i] = sub[cardsub - 1]; cardsub --; break;
			case 2: i++;break;
			}
		}
		if(cardsub == 0)
		{
			* res = 0; return (formula *) 0;
		}
		else
		{
			if(cardsub == 1)
			{
				* res = 2;
				return sub[0];
			}
			else
			{
				* res = 2;
				return this;
			}
		}
	}
}

formula * unarytemporalformula::reduce(int * res)
{
	int subres;
	atomic = false;
	if(type == ex || type == ax)
	{
		* res = 2;
		return this;
	}
	element = element -> reduce(& subres);
	* res = subres;
	return this;
}

formula * untilformula::reduce(int * res)
{
    int subres;
    atomic = false;
    goal = goal -> reduce(& subres);
    if(subres < 2)
    {
        * res = subres;
        return(formula *) 0;
    }
    hold = hold -> reduce(& subres);
    switch(subres)
    {
        case 0: * res = 2; return goal;
        case 1: * res = 2; switch(type)
        {
            case au: return new unarytemporalformula(af,goal,tformula);
            case eu: return new unarytemporalformula(ef,goal,tformula);

                               // Karsten fragen
            default: /* this should not happen */ assert(false); return false;
        }
        case 2: * res = 2; return this;
        
        // Karsten fragen:
        default: /* this should not happen */ assert(false); return false;
    }
}

formula * atomicformula::posate()
{
#ifdef WITHFORMULA
	p -> cardprop ++;
#endif
	return this;
}

formula * atomicformula::negate()
{
#ifdef WITHFORMULA
	p -> cardprop ++;
#endif
	switch(type)
	{
	case gt: type = leq; break;
	case geq: type = lt; break;
	case lt: type = geq; break;
	case leq: type = gt; break;
	case eq: type = neq; break;
	case neq: type = eq; break;
    default: break;
	}
	return this;
}

void transitionformula::print()
{
	cout << "[tf: ";
	if(transition) cout << transition -> name;
	if(hltransition) cout << hltransition;
	cout << "]";
}

void atomicformula::print()
{
	cout << "[at: ";
	cout << p -> name;
	switch(type)
	{
	case eq: cout << " = "; break;
	case neq: cout << " # "; break;
	case leq: cout << " <= "; break;
	case geq: cout << " >= "; break;
	case lt: cout << " < "; break;
	case gt: cout << " > "; break;
	default: cout << "???";
	}
	cout << k << "]";
}

void unarytemporalformula::print()
{
	cout << "[un: ";
	if(tformula) tformula -> print();
	switch(type)
	{
	case ax: cout << "AX "; break;
	case ex: cout << "EX "; break;
	case af: cout << "AF "; break;
	case ag: cout << "AG "; break;
	case ef: cout << "EF "; break;
	case eg: cout << "EG "; break;
	default: cout << "???";
	}
	element -> print();
	cout << "]";
}

void untilformula::print()
{
	cout << "[ut: ";
	if(tformula) tformula -> print();
	switch(type)
	{
	case au: cout << "AU "; break;
	case eu: cout << "EU "; break;
	default: cout << "???";
	}
	hold -> print();
	cout << ",";
	goal -> print();
	cout << "]";
}
	
void hlatomicformula::print()
{
	cout << "[hl: "<< p -> name << "]" ;
}


void unarybooleanformula::print()
{
	cout << "[ub: " << "NOT ";
	sub -> print();
	cout << "]";
}

void binarybooleanformula::print()
{	cout << "[bb: ";
	switch(type)
	{
	case conj: cout << "AND "; break;
	case disj: cout << "OR "; break;
	default: cout << "???";
	}
	left -> print();
	cout << ", ";
	right -> print();
	cout << "]";
}


void booleanformula::print()
{
	cout << "[bo: ";
	switch(type)
	{
	case conj: cout << "AND "; break;
	case disj: cout << "OR "; break;
	default: cout << "???";
	}
	for(int i = 0; i < cardsub; i++)
	{
		sub[i]->print();
		cout << ", ";
	}
	cout << "]";
}


void quantifiedformula::print()
{
	cout << "[qu: ";
	switch(type)
	{
	case qe: cout << "EXISTS "; break;
	case qa: cout << "ALL "; break;
	default: cout << "???";
	}
	cout << "]";
}

void staticformula::print()
{	
	cout << "[st: ]";
}
	

formula * unarybooleanformula::posate()
{
	if(!atomic)
	{
		sub = sub -> posate();
		return this;
	}
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

formula * unarytemporalformula::posate()
{
	element = element -> posate();
	return this;
}

formula * untilformula::posate()
{
	hold = hold -> posate();
	goal = goal -> posate();
	return this;
}

void update_formula(Transition * t)
{
    // update value of formula after having fired t

#ifdef WITHFORMULA
    for(unsigned int i=0;t->DecrPlaces[i] < Places[0]->cnt;i++)
    {
        for(unsigned int j=0; j < Places[t->DecrPlaces[i]] -> cardprop;j++)
        {
            Places[t->DecrPlaces[i]]->propositions[j] -> updateatomic();
        }
    }
    for(unsigned int i=0;t->IncrPlaces[i] < Places[0]->cnt;i++)
    {
        for(unsigned int j=0; j < Places[t->IncrPlaces[i]] -> cardprop;j++)
        {
            Places[t->IncrPlaces[i]]->propositions[j] -> updateatomic();
        }
    }
#endif
}

Transition ** booleanformula::spp2(State * s)
{
	unsigned int i;
	Transition ** fl;

	if(type == conj)
	{
		return sub[0]->spp2(s);
	}
	for(i=0;i<cardsub;i++)
	{
		if( (fl = sub[i] -> spp2(s)) )
		{
			return fl;
		}
	}
	return (Transition **) 0;
}

#ifdef STUBBORN
Transition ** stubbornfirelist(State *, formula *);
#endif

Transition ** atomicformula::spp2(State * s)
{
    // Karsten fragen: Rückgabewert immer definiert?
    
#if defined(RELAXED) && ! defined(STRUCT)
	unsigned int i;

	// 1. up(f) coverd by TSCC?
	//      yes -> return 0
	//      no -> goto 2.
	switch(type)
	{
	case leq:
	case lt:
		for(i=0;i<p -> NrOfLeaving;i++)
		{
			if(p -> LeavingArcs[i]->tr  -> lastfired <= s -> dfs)
			{
				// last occurrence of t happened before entering this scc
				// Note: lastfired is dfs of last occurrence plus 1!
				break;
			}
		}
		if(i >= p -> NrOfLeaving) return (Transition **) 0;
		break;
	case geq:
	case gt:
		for(i=0;i<p -> NrOfArriving;i++)
		{
			if(p -> ArrivingArcs[i] -> tr -> lastfired <= s -> dfs)
			{
				// last occurrence of t happened before entering this scc
				// Note: lastfired is dfs of last occurrence plus 1!
				break;
			}
		}
		if(i >= p -> NrOfArriving) return (Transition **) 0;
		break;
	case eq:
		if(CurrentMarking[p -> index] < k)
		{
			for(i=0;i<p -> NrOfArriving;i++)
			{
				if(p -> ArrivingArcs[i] -> tr -> lastfired <= s -> dfs)
				{
					// last occurrence of t happened before entering this scc
					// Note: lastfired is dfs of last occurrence plus 1!
					break;
				}
			}
			if(i >= p -> NrOfArriving) return (Transition **) 0;
			break;
		}
		else
		{
			for(i=0;i<p -> NrOfLeaving;i++)
			{
				if(p -> LeavingArcs[i] -> tr  -> lastfired <= s -> dfs)
				{
					// last occurrence of t happened before entering this scc
					// Note: lastfired is dfs of last occurrence plus 1!
					break;
				}
			}
			if(i >= p -> NrOfLeaving) return (Transition **) 0;
			break;
		}
	case neq:
		for(i=0;i<p -> NrOfArriving;i++)
		{
			if(p -> ArrivingArcs[i] -> tr -> lastfired <= s -> dfs)
			{
				// last occurrence of t happened before entering this scc
				// Note: lastfired is dfs of last occurrence plus 1!
				break;
			}
		}
		if(i >= p -> NrOfArriving)
		{
			for(i=0;i<p -> NrOfLeaving;i++)
			{
				if(p -> LeavingArcs[i] -> tr -> lastfired <= s -> dfs)
				{
					// last occurrence of t happened before entering this scc
					// Note: lastfired is dfs of last occurrence plus 1!
					break;
				}
			}
			if(i >= p -> NrOfLeaving) return (Transition **) 0;
		}
		break;
	}

	// 2. check whether stubborn set around up(f) contains unfired enabled transitions
	//    yes: return array of these transitions
	//    no: return 0

	Transition ** stublist;
	unsigned int j;
	unsigned int cardnew = 0;
#ifdef STUBBORN
	stublist = stubbornfirelist(s,this);
#endif
	for(i=0;stublist[i];i++)
	{
		for(j=0;s -> firelist[j];j++)
		{
			if(s -> firelist[j] == stublist[i])
			{
				stublist[i] = (Transition *) 0;
				break;
			}
		}
		if(stublist[i])
		{
			// forgotten transition
			cardnew ++;
		}
	}
	if(!cardnew) return (Transition **) 0;
	Transition ** result;
	result = new Transition * [cardnew+1];
	for(i=0,j=0;i<cardnew;i++)
	{
		while(!stublist[j]) j++;
		result[i] = stublist[j++];
	}
	result[i] = (Transition *) 0;
	return result;
#endif
}

formula * staticformula::replacequantifiers()
{
	value = ((UBooValue *) exp -> evaluate()) -> v;
	return this;
}

formula * staticformula::reduce(int * res)
{
	* res = value;
	return this;
}

formula * staticformula::copy()
{
	staticformula * f;
	f = new staticformula(exp);
	f -> value = value;
	f -> parent = parent;
	f -> atomic = true;
	return f;
}

staticformula::staticformula(UExpression * e)
{
	exp = e;
	type = stat;
	atomic =true;
}




