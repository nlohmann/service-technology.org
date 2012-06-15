/*!
\file ChooseTransitionHashDriven.h
\author Karsten
\status new

\brief Selects a transition from a fire list based on counting hash values and
a priority imposed on the fire list
*/

#pragma once

#include <Exploration/ChooseTransition.h>

class ChooseTransitionHashDriven : public ChooseTransition
{
    public:
        ChooseTransitionHashDriven();
        ~ChooseTransitionHashDriven();

        virtual index_t choose(NetState &ns, index_t cardfirelist, index_t* firelist);
    private:
        unsigned long int* table;
};
