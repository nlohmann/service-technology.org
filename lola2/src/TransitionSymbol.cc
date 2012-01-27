/*!
\author Karsten
\file TransitionSymbol.cc
\status approved 25.01.2012
\ingroup g_frontend g_symboltable

\brief class implementation for a symbol with payload for transition
*/

#include "TransitionSymbol.h"
#include "PlaceSymbol.h"
#include "FairnessAssumptions.h"

/// Generate and initialize a symbol
TransitionSymbol::TransitionSymbol(char* k, tFairnessAssumption f, ArcList* pr, ArcList* po)
    :
    Symbol(k),
    fairness(f),
    cardPost(0),
    cardPre(0),
    Post(po),
    Pre(pr)
{
    // count incomning arcs at transition and at places
    for (ArcList* a = Pre; a -> getNext(); a = a -> getNext())
    {
        ++cardPre;
	a -> getPlace() -> notifyPost();
    }

    // count outgoing arcs
    for (ArcList* a = Post; a -> getNext(); a = a -> getNext())
    {
        ++cardPost;
	a -> getPlace() -> notifyPre();
    }
}

/// Getter for number of incoming arcs
unsigned int TransitionSymbol::getCardPre() const
{
    return cardPre;
}

/// Getter for number of outgoing arcs
unsigned int TransitionSymbol::getCardPost() const
{
    return cardPost;
}

/// Getter for incoming arcs
ArcList* TransitionSymbol::getPre() const
{
    return Pre;
}

/// Getter for number of post-places
ArcList* TransitionSymbol::getPost() const
{
    return Post;
}

/// Getter for fairness assumption
tFairnessAssumption TransitionSymbol::getFairness() const
{
    return fairness;
}

/// Delete TransitionSymbol
TransitionSymbol::~TransitionSymbol()
{
    while (Pre)
    {
        ArcList* tmp = Pre;
        Pre = Pre -> getNext();
        delete tmp;
    }
    while (Post)
    {
        ArcList* tmp = Post;
        Post = Post -> getNext();
        delete tmp;
    }
}
