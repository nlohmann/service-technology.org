/*!
\file ChooseTransition.h
\author Karsten
\status new

\brief Selects a transition from a fire list
*/

#pragma once

#include <Core/Dimensions.h>

class ChooseTransition
{
public:
	virtual index_t choose(index_t cardfirelist, index_t * firelist) = 0;
};
