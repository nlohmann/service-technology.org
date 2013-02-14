/*!
\file NegationStatePredicate.cc
\author Karsten
\status new

\brief class implementation for disjunction state predicates
*/

#include <cstdlib>
#include <cstdio>
#include <Formula/NegationStatePredicate.h>
#include <Net/Net.h>

NegationStatePredicate::NegationStatePredicate(StatePredicate* f)
{
    //printf("+ %p->NegationStatePredicate(n=%d)\n", this, n);

    parent = NULL;
    sub = f;
    sub -> position = 0;
    sub -> parent = this;
}

NegationStatePredicate::~NegationStatePredicate()
{
	delete(sub);
}

index_t NegationStatePredicate::getUpSet(index_t* stack, bool* onstack)
{
    // call only if this formula is false
    assert(!sat);
    index_t stackpointer = sub -> getUpSet(stack + stackpointer, onstack);
    assert(stackpointer <= Net::Card[TR]);
    return stackpointer;
}

void NegationStatePredicate::updateTF(index_t i)
{
	// sub is now false, thus i am true
	value = true;
	if (parent)
	{
		parent -> updateFT(position);
	}
}

void NegationStatePredicate::updateFT(index_t i)
{
	// sub is now true, thus i am false
	value = false;
	if (parent)
	{
		parent -> updateTF(position);
	}
}



void NegationStatePredicate::evaluate(NetState &ns)
{
	sub -> evaluate(ns);
    value = !sub->value;
}

// LCOV_EXCL_START
bool NegationStatePredicate::DEBUG__consistency(NetState &ns)
{

    if (sub->value)
        assert(!value);
    else
        assert(value);
    return true;
}

// LCOV_EXCL_STOP


StatePredicate* NegationStatePredicate::copy(StatePredicate* parent)
{
    NegationStatePredicate* nsp = new NegationStatePredicate(sub->copy(nsp));
    nsp->value = value;
    nsp->position = position;
    nsp->parent = parent;
    return nsp;
}


bool NegationStatePredicate::isOrNode() const
{
	return false;
}

index_t NegationStatePredicate::countAtomic() const
{
    return sub -> countAtomic();
}

index_t NegationStatePredicate::collectAtomic(AtomicStatePredicate** p)
{
    return sub->collectAtomic(p);
}

index_t NegationStatePredicate::getSubs(StatePredicate const *const **subs) const
{
	*subs = &sub;
	return 1;
}

