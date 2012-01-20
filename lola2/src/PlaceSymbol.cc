/*!
\author Karsten
\file PlaceSymbol.cc
\status new

\brief class implementation for a symbol with payload for place

Place symbols carry name as key, and
- capacity
- initial marking
as payload. While capacity is in the context of place declaration (thus,
part of constructor), initial marking is specified separately (thus, setter).
Additional information is number of pre-transitions and number of post-transitions. This information is updated while parsing transitions.
*/

/// Constructor. Capacity available in context, the other ingredients are not
PlaceSymbol::PlaceSymbol(string k,unsigned int cap):base(k)
{
	capacity = cap;
	cardPre = 0;
	cardPost = 0;
	initialMarking = 0;
}

/// Getter for capacity
unsigned int getCapacity()
{
	return capacity;
}

/// Adder for initial marking
unsigned int addInitialMarking(unsigned int newTokens)
{
	initialMarking += newTokens;
}


/// Getter for number of pre-transitions
unsigned int getCardPre()
{
	return cardPre;
}

/// Getter for number of post-transitions
unsigned int getCardPost()
{
	return cardPost;
}

/// Incrementor for number of post-transitions
void notifyPost()
{
	++cardPost;
}

/// Incrementor for number of pre-transitions
void notifyPre()
{
	++cardPre
}
