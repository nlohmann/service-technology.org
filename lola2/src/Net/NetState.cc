/*!
 \file NetState.cc
 \author Christian K and Gregor B
 \status unknown
 */

#include <Core/Dimensions.h>
#include <Net/NetState.h>
#include <Net/Net.h>
#include <Net/Place.h>
#include <Net/Transition.h>
#include <Net/Marking.h>
#include <cstdlib>
#include <algorithm>

NetState *NetState::createNetStateFromInitial()
{
    NetState *ns = new NetState();
    ns->membersInitialized = true;
    // copy the current marking at its hash
    ns->Current = new capacity_t[Net::Card[PL]];
    for (int i = 0; i < Net::Card[PL]; i++)
    {
        ns->Current[i] = Marking::Initial[i];
    }
    ns->HashCurrent = Marking::HashInitial;

    // copy the currently enabled transitions
    ns->Enabled = new bool[Net::Card[TR]];
    for (int i = 0; i < Net::Card[TR]; i++)
    {
        ns->Enabled[i] = Transition::Enabled[i];
    }
    ns->CardEnabled = Transition::CardEnabled;

    // copy current scapegoats
    ns->PositionScapegoat = new index_t[Net::Card[TR]];
    for (int i = 0; i < Net::Card[TR]; i++)
    {
        ns->PositionScapegoat[i] = Transition::PositionScapegoat[i];
    }

    // copy the parts for the current net may be modified
    for (int type = PL; type <= TR; type++)
    {
        for (int direction = PRE; direction <= POST; direction++)
        {
            ns->Arc[type][direction] = new index_t *[Net::Card[type]];
            ns->Mult[type][direction] = new mult_t *[Net::Card[type]];
        }
    }

    // for places
    for (int i = 0; i < Net::Card[PL]; i++)
    {
        // allocate memory for place's arcs (is copied later with transitions)
        // pre-Zone
        ns->Arc[PL][PRE][i] = new index_t[Net::CardArcs[PL][PRE][i]];
        for (int j = 0; j < Net::CardArcs[PL][PRE][i]; j++)
        {
            ns->Arc[PL][PRE][i][j] = Net::Arc[PL][PRE][i][j];
        }

        ns->Mult[PL][PRE][i] = new mult_t[Net::CardArcs[PL][PRE][i]];
        for (int j = 0; j < Net::CardArcs[PL][PRE][i]; j++)
        {
            ns->Mult[PL][PRE][i][j] = Net::Mult[PL][PRE][i][j];
        }


        // post-Zone
        ns->Arc[PL][POST][i] = new index_t[Net::CardArcs[PL][POST][i]];
        for (int j = 0; j < Net::CardArcs[PL][POST][i]; j++)
        {
            ns->Arc[PL][POST][i][j] = Net::Arc[PL][POST][i][j];
        }

        ns->Mult[PL][POST][i] = new mult_t[Net::CardArcs[PL][POST][i]];
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
        ns->Arc[TR][PRE][i] = new index_t[Net::CardArcs[TR][PRE][i]];
        for (int j = 0; j < Net::CardArcs[TR][PRE][i]; j++)
        {
            ns->Arc[TR][PRE][i][j] = Net::Arc[TR][PRE][i][j];
        }

        ns->Mult[TR][PRE][i] = new mult_t[Net::CardArcs[TR][PRE][i]];
        for (int j = 0; j < Net::CardArcs[TR][PRE][i]; j++)
        {
            ns->Mult[TR][PRE][i][j] = Net::Mult[TR][PRE][i][j];
        }


        // post-Zone
        ns->Arc[TR][POST][i] = new index_t[Net::CardArcs[TR][POST][i]];
        for (int j = 0; j < Net::CardArcs[TR][POST][i]; j++)
        {
            ns->Arc[TR][POST][i][j] = Net::Arc[TR][POST][i][j];
        }

        ns->Mult[TR][POST][i] = new mult_t[Net::CardArcs[TR][POST][i]];
        for (int j = 0; j < Net::CardArcs[TR][POST][i]; j++)
        {
            ns->Mult[TR][POST][i][j] = Net::Mult[TR][POST][i][j];
        }
    }


    // copy currently disabled
    ns->CardDisabled = new index_t[Net::Card[PL] ]();
    ns->Disabled = new index_t *[Net::Card[PL]];
    for (int i = 0; i < Net::Card[PL]; i++)
    {
        ns->CardDisabled[i] = Place::CardDisabled[i];
        ns->Disabled[i] = new index_t[Net::CardArcs[PL][POST][i]];
        for (int j = 0; j < Net::CardArcs[PL][POST][i]; j++)
        {
            ns->Disabled[i][j] = Place::Disabled[i][j];
        }
    }

    return ns;
}









NetState::NetState(const NetState &ons)
{
    membersInitialized = true;
    // copy the current marking at its hash
    Current = new capacity_t[Net::Card[PL]];
    for (int i = 0; i < Net::Card[PL]; i++)
    {
        Current[i] = ons.Current[i];
    }
    HashCurrent = ons.HashCurrent;

    // copy the currently enabled transitions
    Enabled = new bool[Net::Card[TR]];
    for (int i = 0; i < Net::Card[TR]; i++)
    {
        Enabled[i] = ons.Enabled[i];
    }
    CardEnabled = ons.CardEnabled;

    // copy current scapegoats
    PositionScapegoat = new index_t[Net::Card[TR]];
    for (int i = 0; i < Net::Card[TR]; i++)
    {
        PositionScapegoat[i] = ons.PositionScapegoat[i];
    }

    // copy the parts for the current net may be modified
    for (int type = PL; type <= TR; type++)
    {
        for (int direction = PRE; direction <= POST; direction++)
        {
            Arc[type][direction] = new index_t *[Net::Card[type]];
            Mult[type][direction] = new mult_t *[Net::Card[type]];
        }
    }

    // for places
    for (int i = 0; i < Net::Card[PL]; i++)
    {
        // allocate memory for place's arcs (is copied later with transitions)
        // pre-Zone
        Arc[PL][PRE][i] = new index_t[Net::CardArcs[PL][PRE][i]];
        for (int j = 0; j < Net::CardArcs[PL][PRE][i]; j++)
        {
            Arc[PL][PRE][i][j] = ons.Arc[PL][PRE][i][j];
        }

        Mult[PL][PRE][i] = new mult_t[Net::CardArcs[PL][PRE][i]];
        for (int j = 0; j < Net::CardArcs[PL][PRE][i]; j++)
        {
            Mult[PL][PRE][i][j] = ons.Mult[PL][PRE][i][j];
        }


        // post-Zone
        Arc[PL][POST][i] = new index_t[Net::CardArcs[PL][POST][i]];
        for (int j = 0; j < Net::CardArcs[PL][POST][i]; j++)
        {
            Arc[PL][POST][i][j] = ons.Arc[PL][POST][i][j];
        }

        Mult[PL][POST][i] = new mult_t[Net::CardArcs[PL][POST][i]];
        for (int j = 0; j < Net::CardArcs[PL][POST][i]; j++)
        {
            Mult[PL][POST][i][j] = ons.Mult[PL][POST][i][j];
        }
    }

    // for transitions
    for (int i = 0; i < Net::Card[TR]; i++)
    {
        // allocate memory for place's arcs (is copied later with transitions)
        // pre-Zone
        Arc[TR][PRE][i] = new index_t[Net::CardArcs[TR][PRE][i]];
        for (int j = 0; j < Net::CardArcs[TR][PRE][i]; j++)
        {
            Arc[TR][PRE][i][j] = ons.Arc[TR][PRE][i][j];
        }

        Mult[TR][PRE][i] = new mult_t[Net::CardArcs[TR][PRE][i]];
        for (int j = 0; j < Net::CardArcs[TR][PRE][i]; j++)
        {
            Mult[TR][PRE][i][j] = ons.Mult[TR][PRE][i][j];
        }


        // post-Zone
        Arc[TR][POST][i] = new index_t[Net::CardArcs[TR][POST][i]];
        for (int j = 0; j < Net::CardArcs[TR][POST][i]; j++)
        {
            Arc[TR][POST][i][j] = ons.Arc[TR][POST][i][j];
        }

        Mult[TR][POST][i] = new mult_t[Net::CardArcs[TR][POST][i]];
        for (int j = 0; j < Net::CardArcs[TR][POST][i]; j++)
        {
            Mult[TR][POST][i][j] = ons.Mult[TR][POST][i][j];
        }
    }


    // copy currently disabled
    CardDisabled = new index_t[Net::Card[PL] ]();
    Disabled = new index_t *[Net::Card[PL]];
    for (int i = 0; i < Net::Card[PL]; i++)
    {
        CardDisabled[i] = ons.CardDisabled[i];
        Disabled[i] = new index_t[Net::CardArcs[PL][POST][i]];
        for (int j = 0; j < Net::CardArcs[PL][POST][i]; j++)
        {
            Disabled[i][j] = ons.Disabled[i][j];
        }
    }
}

NetState &NetState::operator=(const NetState &ns)
{
    NetState tmp(ns); // copy and swap
    swap(tmp);
    return *this;
}

void NetState::swap(NetState &ns)
{
    std::swap(Current, ns.Current);
    std::swap(HashCurrent, ns.HashCurrent);
    std::swap(Enabled, ns.Enabled);
    std::swap(CardEnabled, ns.CardEnabled);
    std::swap(PositionScapegoat, ns.PositionScapegoat);

    std::swap(Arc[PL][PRE], ns.Arc[PL][PRE]);
    std::swap(Arc[PL][POST], ns.Arc[PL][POST]);
    std::swap(Arc[TR][PRE], ns.Arc[TR][PRE]);
    std::swap(Arc[TR][POST], ns.Arc[TR][POST]);
    std::swap(Mult[PL][PRE], ns.Mult[PL][PRE]);
    std::swap(Mult[PL][POST], ns.Mult[PL][POST]);
    std::swap(Mult[TR][PRE], ns.Mult[TR][PRE]);
    std::swap(Mult[TR][POST], ns.Mult[TR][POST]);

    std::swap(CardDisabled, ns.CardDisabled);
    std::swap(Disabled, ns.Disabled);
    std::swap(membersInitialized, ns.membersInitialized);
}


/*!
\todo Find out why membersInitialized is used here.
*/
NetState::~NetState()
{
    if (!membersInitialized)
    {
        return;
    }

    delete[] Current;
    delete[] Enabled;
    delete[] PositionScapegoat;

    // for places
    for (int i = 0; i < Net::Card[PL]; i++)
    {
        delete[] Arc[PL][PRE][i];
        delete[] Mult[PL][PRE][i];
        delete[] Arc[PL][POST][i];
        delete[] Mult[PL][POST][i];
    }

    // for transitions
    for (int i = 0; i < Net::Card[TR]; i++)
    {
        delete[] Arc[TR][PRE][i];
        delete[] Mult[TR][PRE][i];
        delete[] Arc[TR][POST][i];
        delete[] Mult[TR][POST][i];
    }

    for (int type = PL; type <= TR; type++)
    {
        for (int direction = PRE; direction <= POST; direction++)
        {
            delete[] Arc[type][direction];
            delete[] Mult[type][direction];
        }
    }
    delete[] CardDisabled;
    for (int i = 0; i < Net::Card[PL]; i++)
    {
        delete[] Disabled[i];
    }
    delete[] Disabled;
}
