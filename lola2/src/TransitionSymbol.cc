/*!
\author Karsten
\file TransitionSymbol.cc
\status new

\brief class implementation for a symbol with payload for transition

Transition symbols carry name as key, and
- list of incoming arcs
- list of outgoing arcs
- a fairness assumption
as payload. 
All information is available in the context of transition definition, thus
all information is provided in constructor
*/

#pragma once

#include "Symbol.h"
#include "FairnessAssumptions.h"
#include "ArcList.h"

/// Generate and initialize a symbol
TransitionSymbol(string k,tFairnessAssumption f,ArcList* pr, ArcList* po): base (k)
{
	fairness = f;
	Pre = pr;
	Post = po;
	// count incomning arcs
	for(ArcList * a = Pre,cardPre = 0; a -> getNext();a = a -> getNext())
	{
		++cardPre;
	}
	// count outgoing arcs
	for(ArcList * a = Post,cardPost = 0; a -> getNext();a = a -> getNext())
	{
		++cardPost;
	}
}

/// Getter for number of incoming arcs
unsigned int getCardPre()
{
	return cardPre;
}

/// Getter for number of outgoing arcs
unsigned int getCardPost()
{
	return cardPost;
}

/// Getter for incoming arcs
unsigned int getPre()
{
	return Pre;
}

/// Getter for number of post-places
unsigned int getPost()
{
	return Post;
}
