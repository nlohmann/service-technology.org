/*!
\file ConjunctionStatePredicate.cc
\author Karsten
\status new

\brief class implementation for conjunction state predicates
*/

#include "Formula/ConjunctionStatePredicate.h"

ConjunctionStatePredicate::ConjunctionStatePredicate(unsigned int n)
{
	sub = (StatePredicate **) malloc(n * SIZEOF_VOIDP);
	cardSub = n;
}

void ConjunctionStatePredicate::addSub(unsigned int i, StatePredicate * f)
{
	sub[i] = f;
	sub[i] -> position = i;
}

index_t ConjunctionStatePredicate::getUpSet(index_t * stack, bool * onstack)
{
	// call only if this formula is false
	assert(cardSat < cardSub);
	return sub[cardSat] -> getUpSet(stack,onstack);
} 
                                 
void ConjunctionStatePredicate::updateTF(index_t i)
{
	// assumption: satisfied left, unsatisfied right

	// --> sub[cardSat] is first unsatisfied
	if(cardSat-- == cardSub)
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

void ConjunctionStatePredicate::updateFT(index_t i)
{
	// assumption: satisfied left, unsatisfied right

	// --> sub[cardSat] is first unsatisfied
	StatePredicate * tmp = sub[cardSat];
	sub[cardSat] = sub[i];
	sub[i] = tmp;
	sub[i] -> position = i;
	sub[cardSat]->position = cardSat;
	if(++cardSat == cardSub)
	{
		value = true;
		parent -> updateFT(position);
	}
}


	
void ConjunctionStatePredicate::evaluate()
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
	if(cardSat == cardSub)
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
