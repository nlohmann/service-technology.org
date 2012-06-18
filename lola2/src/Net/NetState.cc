/*!
 \file NetState.cc
 \author Christian K and Gregor B
 \status unknown
 \brief

 */

#include "Core/Dimensions.h"
#include "Net/NetState.h"
#include "Net/Net.h"
#include "Net/Place.h"
#include "Net/Transition.h"
#include "Net/Marking.h"
#include <cstdlib>

NetState* NetState::createNetStateFromInitial()
{
    NetState* ns = (NetState*) malloc(sizeof(NetState));
    ns->need_to_delete_members_on_delete = true;
    // copy the current marking at its hash
    ns->Current = (capacity_t*) malloc(Net::Card[PL] * SIZEOF_CAPACITY_T);
    for (int i = 0; i < Net::Card[PL]; i++)
    {
        ns->Current[i] = Marking::Initial[i];
    }
    ns->HashCurrent = Marking::HashInitial;

    // copy the currently enabled transitions
    ns->Enabled = (bool*) malloc(Net::Card[TR] * SIZEOF_BOOL);
    for (int i = 0; i < Net::Card[TR]; i++)
    {
        ns->Enabled[i] = Transition::Enabled[i];
    }
    ns->CardEnabled = Transition::CardEnabled;

    // copy current scapegoats
    ns->PositionScapegoat = (index_t*) malloc(Net::Card[TR] * SIZEOF_INDEX_T);
    for (int i = 0; i < Net::Card[TR]; i++)
    {
        ns->PositionScapegoat[i] = Transition::PositionScapegoat[i];
    }

    // copy the parts for the current net may be modified
    for (int type = PL; type <= TR; type++)
    {
        for (int direction = PRE; direction <= POST; direction++)
        {
            ns->Arc[type][direction] = (index_t**) malloc(
                                           Net::Card[type] * SIZEOF_VOIDP);
            ns->Mult[type][direction] = (mult_t**) malloc(
                                            Net::Card[type] * SIZEOF_VOIDP);
        }
    }

    // for places
    for (int i = 0; i < Net::Card[PL]; i++)
    {
        // allocate memory for place's arcs (is copied later with transitions)
        // pre-Zone
        ns->Arc[PL][PRE][i] = (index_t*) malloc(
                                  Net::CardArcs[PL][PRE][i] * SIZEOF_INDEX_T);
        for (int j = 0; j < Net::CardArcs[PL][PRE][i]; j++)
        {
            ns->Arc[PL][PRE][i][j] = Net::Arc[PL][PRE][i][j];
        }

        ns->Mult[PL][PRE][i] = (mult_t*) malloc(
                                   Net::CardArcs[PL][PRE][i] * SIZEOF_MULT_T);
        for (int j = 0; j < Net::CardArcs[PL][PRE][i]; j++)
        {
            ns->Mult[PL][PRE][i][j] = Net::Mult[PL][PRE][i][j];
        }


        // post-Zone
        ns->Arc[PL][POST][i] = (index_t*) malloc(
                                   Net::CardArcs[PL][POST][i] * SIZEOF_INDEX_T);
        for (int j = 0; j < Net::CardArcs[PL][POST][i]; j++)
        {
            ns->Arc[PL][POST][i][j] = Net::Arc[PL][POST][i][j];
        }

        ns->Mult[PL][POST][i] = (mult_t*) malloc(
                                    Net::CardArcs[PL][POST][i] * SIZEOF_MULT_T);
        for (int j = 0; j < Net::CardArcs[PL][POST][i]; j++)
        {
            ns->Mult[PL][POST][i][j] = Net::Mult[PL][POST][i][j];
        }
    }

    // for transitions
    for (int i = 0; i < Net::Card[TR]; i++)
    {
        // allocate memory for place's arcs (is copied later with transitions)
        // pre-Zone
        ns->Arc[TR][PRE][i] = (index_t*) malloc(
                                  Net::CardArcs[TR][PRE][i] * SIZEOF_INDEX_T);
        for (int j = 0; j < Net::CardArcs[TR][PRE][i]; j++)
        {
            ns->Arc[TR][PRE][i][j] = Net::Arc[TR][PRE][i][j];
        }

        ns->Mult[TR][PRE][i] = (mult_t*) malloc(
                                   Net::CardArcs[TR][PRE][i] * SIZEOF_MULT_T);
        for (int j = 0; j < Net::CardArcs[TR][PRE][i]; j++)
        {
            ns->Mult[TR][PRE][i][j] = Net::Mult[TR][PRE][i][j];
        }


        // post-Zone
        ns->Arc[TR][POST][i] = (index_t*) malloc(
                                   Net::CardArcs[TR][POST][i] * SIZEOF_INDEX_T);
        for (int j = 0; j < Net::CardArcs[TR][POST][i]; j++)
        {
            ns->Arc[TR][POST][i][j] = Net::Arc[TR][POST][i][j];
        }

        ns->Mult[TR][POST][i] = (mult_t*) malloc(
                                    Net::CardArcs[TR][POST][i] * SIZEOF_MULT_T);
        for (int j = 0; j < Net::CardArcs[TR][POST][i]; j++)
        {
            ns->Mult[TR][POST][i][j] = Net::Mult[TR][POST][i][j];
        }
    }


    // copy currently disabled
    ns->CardDisabled = (index_t*) calloc(Net::Card[PL] , SIZEOF_INDEX_T);
    ns->Disabled = (index_t**) malloc(Net::Card[PL] * SIZEOF_VOIDP);
    for (int i = 0; i < Net::Card[PL]; i++)
    {
        ns->CardDisabled[i] = Place::CardDisabled[i];
        ns->Disabled[i] = (index_t*) malloc(Net::CardArcs[PL][POST][i] * SIZEOF_INDEX_T);
        for (int j = 0; j < Net::CardArcs[PL][POST][i]; j++)
        {
            ns->Disabled[i][j] = Place::Disabled[i][j];
        }
    }

    return ns;
}









NetState* NetState::createNetStateFromCurrent(NetState &ons)
{
    NetState* ns = (NetState*) malloc(sizeof(NetState));
    ns->need_to_delete_members_on_delete = true;
    // copy the current marking at its hash
    ns->Current = (capacity_t*) malloc(Net::Card[PL] * SIZEOF_CAPACITY_T);
    for (int i = 0; i < Net::Card[PL]; i++)
    {
        ns->Current[i] = ons.Current[i];
    }
    ns->HashCurrent = ons.HashCurrent;

    // copy the currently enabled transitions
    ns->Enabled = (bool*) malloc(Net::Card[TR] * SIZEOF_BOOL);
    for (int i = 0; i < Net::Card[TR]; i++)
    {
        ns->Enabled[i] = ons.Enabled[i];
    }
    ns->CardEnabled = ons.CardEnabled;

    // copy current scapegoats
    ns->PositionScapegoat = (index_t*) malloc(Net::Card[TR] * SIZEOF_INDEX_T);
    for (int i = 0; i < Net::Card[TR]; i++)
    {
        ns->PositionScapegoat[i] = ons.PositionScapegoat[i];
    }

    // copy the parts for the current net may be modified
    for (int type = PL; type <= TR; type++)
    {
        for (int direction = PRE; direction <= POST; direction++)
        {
            ns->Arc[type][direction] = (index_t**) malloc(
                                           Net::Card[type] * SIZEOF_VOIDP);
            ns->Mult[type][direction] = (mult_t**) malloc(
                                            Net::Card[type] * SIZEOF_VOIDP);
        }
    }

    // for places
    for (int i = 0; i < Net::Card[PL]; i++)
    {
        // allocate memory for place's arcs (is copied later with transitions)
        // pre-Zone
        ns->Arc[PL][PRE][i] = (index_t*) malloc(
                                  Net::CardArcs[PL][PRE][i] * SIZEOF_INDEX_T);
        for (int j = 0; j < Net::CardArcs[PL][PRE][i]; j++)
        {
            ns->Arc[PL][PRE][i][j] = ons.Arc[PL][PRE][i][j];
        }

        ns->Mult[PL][PRE][i] = (mult_t*) malloc(
                                   Net::CardArcs[PL][PRE][i] * SIZEOF_MULT_T);
        for (int j = 0; j < Net::CardArcs[PL][PRE][i]; j++)
        {
            ns->Mult[PL][PRE][i][j] = ons.Mult[PL][PRE][i][j];
        }


        // post-Zone
        ns->Arc[PL][POST][i] = (index_t*) malloc(
                                   Net::CardArcs[PL][POST][i] * SIZEOF_INDEX_T);
        for (int j = 0; j < Net::CardArcs[PL][POST][i]; j++)
        {
            ns->Arc[PL][POST][i][j] = ons.Arc[PL][POST][i][j];
        }

        ns->Mult[PL][POST][i] = (mult_t*) malloc(
                                    Net::CardArcs[PL][POST][i] * SIZEOF_MULT_T);
        for (int j = 0; j < Net::CardArcs[PL][POST][i]; j++)
        {
            ns->Mult[PL][POST][i][j] = ons.Mult[PL][POST][i][j];
        }
    }

    // for transitions
    for (int i = 0; i < Net::Card[TR]; i++)
    {
        // allocate memory for place's arcs (is copied later with transitions)
        // pre-Zone
        ns->Arc[TR][PRE][i] = (index_t*) malloc(
                                  Net::CardArcs[TR][PRE][i] * SIZEOF_INDEX_T);
        for (int j = 0; j < Net::CardArcs[TR][PRE][i]; j++)
        {
            ns->Arc[TR][PRE][i][j] = ons.Arc[TR][PRE][i][j];
        }

        ns->Mult[TR][PRE][i] = (mult_t*) malloc(
                                   Net::CardArcs[TR][PRE][i] * SIZEOF_MULT_T);
        for (int j = 0; j < Net::CardArcs[TR][PRE][i]; j++)
        {
            ns->Mult[TR][PRE][i][j] = ons.Mult[TR][PRE][i][j];
        }


        // post-Zone
        ns->Arc[TR][POST][i] = (index_t*) malloc(
                                   Net::CardArcs[TR][POST][i] * SIZEOF_INDEX_T);
        for (int j = 0; j < Net::CardArcs[TR][POST][i]; j++)
        {
            ns->Arc[TR][POST][i][j] = ons.Arc[TR][POST][i][j];
        }

        ns->Mult[TR][POST][i] = (mult_t*) malloc(
                                    Net::CardArcs[TR][POST][i] * SIZEOF_MULT_T);
        for (int j = 0; j < Net::CardArcs[TR][POST][i]; j++)
        {
            ns->Mult[TR][POST][i][j] = ons.Mult[TR][POST][i][j];
        }
    }


    // copy currently disabled
    ns->CardDisabled = (index_t*) calloc(Net::Card[PL] , SIZEOF_INDEX_T);
    ns->Disabled = (index_t**) malloc(Net::Card[PL] * SIZEOF_VOIDP);
    for (int i = 0; i < Net::Card[PL]; i++)
    {
        ns->CardDisabled[i] = ons.CardDisabled[i];
        ns->Disabled[i] = (index_t*) malloc(Net::CardArcs[PL][POST][i] * SIZEOF_INDEX_T);
        for (int j = 0; j < Net::CardArcs[PL][POST][i]; j++)
        {
            ns->Disabled[i][j] = ons.Disabled[i][j];
        }
    }

    return ns;
}

void NetState::copyNetState(NetState &ns)
{
    deleteAllMembers();
    Current = ns.Current;
    HashCurrent = ns.HashCurrent;
    Enabled = ns.Enabled;
    CardEnabled = ns.CardEnabled;
    PositionScapegoat = ns.PositionScapegoat;
    Arc[0][0] = ns.Arc[0][0];
    Arc[0][1] = ns.Arc[0][1];
    Arc[1][0] = ns.Arc[1][0];
    Arc[1][1] = ns.Arc[1][1];
    Mult[0][0] = ns.Mult[0][0];
    Mult[0][1] = ns.Mult[0][1];
    Mult[1][0] = ns.Mult[1][0];
    Mult[1][1] = ns.Mult[1][1];
    CardDisabled = ns.CardDisabled;
    Disabled = ns.Disabled;
    // now the old netstate does not need to delete its members on deletions as they are now the members of this netstate;
    ns.need_to_delete_members_on_delete = false;
}


void NetState::deleteAllMembers()
{
    if (!need_to_delete_members_on_delete)
    {
        return;
    }
    free(Current);
    free(Enabled);
    free(PositionScapegoat);

    // for places
    for (int i = 0; i < Net::Card[PL]; i++)
    {
        free(Arc[PL][PRE][i]);
        free(Mult[PL][PRE][i]);
        free(Arc[PL][POST][i]);
        free(Mult[PL][POST][i]);
    }

    // for transitions
    for (int i = 0; i < Net::Card[TR]; i++)
    {
        free(Arc[TR][PRE][i]);
        free(Mult[TR][PRE][i]);
        free(Arc[TR][POST][i]);
        free(Mult[TR][POST][i]);
    }

    for (int type = PL; type <= TR; type++)
    {
        for (int direction = PRE; direction <= POST; direction++)
        {
            free(Arc[type][direction]);
            free(Mult[type][direction]);
        }
    }
    free(CardDisabled);
    for (int i = 0; i < Net::Card[PL]; i++)
    {
        free(Disabled[i]);
    }
    free(Disabled);
}

NetState::~NetState()
{
    deleteAllMembers();
}
