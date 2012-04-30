/*!
\file DisjunctionStatePredicate.cc
\author Karsten
\status new

\brief class implementation for disjunction state predicates
*/

#include "Formula/DisjunctionStatePredicate.h"
#include <cstdlib>

DisjunctionStatePredicate::DisjunctionStatePredicate(unsigned int n)
{
	sub = (StatePredicate **) malloc(n * SIZEOF_VOIDP);
	cardSub = n;
}

void DisjunctionStatePredicate::addSub(unsigned int i, StatePredicate * f)
{
	sub[i] = f;
	sub[i] -> position = i;
}

index_t DisjunctionStatePredicate::getUpSet(index_t * stack, bool * onstack)
{
	// call only if this formula is false
	assert(cardSat == 0);

	index_t stackpointer = 0;
	for(index_t i = 0; i < cardSub; i++)
	{
		stackpointer += sub[i] -> getUpSet(stack+stackpointer, onstack);
	}
	return stackpointer;
} 
                                 
void DisjunctionStatePredicate::updateTF(index_t i)
{
	// assumption: satisfied left, unsatisfied right

	// --> sub[cardSat] is first unsatisfied
	if(cardSat-- == 1)
	{
		value = false;
		parent -> updateTF(position);
	}
	StatePredicate * tmp = sub[cardSat];
	sub[cardSat] = sub[i];
	sub[i] = tmp;
	sub[i] -> position = i;
	sub[cardSat]->position = cardSat;
}

void DisjunctionStatePredicate::updateFT(index_t i)
{
	// assumption: satisfied left, unsatisfied right

	// --> sub[cardSat] is first satisfied
	StatePredicate * tmp = sub[cardSat];
	sub[cardSat] = sub[i];
	sub[i] = tmp;
	sub[i] -> position = i;
	sub[cardSat]->position = cardSat;
	if(++cardSat == 1)
	{
		value = true;
		parent -> updateFT(position);
	}
}


	
void DisjunctionStatePredicate::evaluate()
{
	for(unsigned int i = 0; i < cardSub; i++)
	{
		sub[i] -> evaluate();
	}
	index_t left = 0;
	index_t right = cardSub;
	
	// sort satisfied to left, unsat to right of sub list
	// loop invariant: formulas left of left (not including left) are satisfied, 
	// formulas right of right (including right) are unsatisfied
	while(true)
	{
		while(left < cardSub && sub[left]->value)
		{
			++left;
		}
		while(right > 0 && !sub[right-1]->value)
		{
			--right;
		}
		if(left >= right) // array sorted
		{
			break;
		}
		StatePredicate * tmp = sub[left];
		sub[left++] = sub[--right];
		sub[right] = tmp;
	}
	cardSat = left;
	if(cardSat > 0)
	{
		value = true;
	}
	else
	{
		value = false;
	}
	
	// update position in sub formulas
	for(index_t i = 0; i < cardSub;i++)
	{
		sub[i]->position = i;
	}
}
