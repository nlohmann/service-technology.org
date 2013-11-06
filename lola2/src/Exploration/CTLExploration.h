

#pragma once

#include <vector>
#include <Formula/CTL/CTLFormula.h>
#include <Stores/Store.h>
#include <Exploration/Firelist.h>
#include <Net/NetState.h>

class CTLExploration
{
public:
    std::vector<int> witness;

    bool checkProperty(CTLFormula *formula, Store<void *> &store,
                       Firelist &firelist, NetState &ns);
};
