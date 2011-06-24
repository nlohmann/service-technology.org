#include "SS.h"

SS::SS()
{
    if (mgr == 0) mgr = new Cudd(0,0);
    states = mgr->bddZero();
    minimized = false;
}

SS::SS(int i)
{
    if (mgr == 0) mgr = new Cudd(0,0);
    if (i == 0) states = mgr->bddZero();
    else states = mgr->bddOne();
    minimized = false;
}


SS::SS(const BDD &g)
{
    if (mgr == 0) mgr = new Cudd(0,0);
    states = g;
    minimized = false;
}


SS::SS(const SS &from)
{
    if (mgr == 0) mgr = new Cudd(0,0);
    states = from.states;
    minimized = from.minimized;
}

int
SS::operator==(const SS& other) const
{
    if (not minimized)
        return states==other.states;
    else
        return (states*reach) == (other.states*reach);
}


int
SS::operator!=(const SS& other) const
{
    if (not minimized)
        return states!=other.states;
    else
        return (states*reach) != (other.states*reach);
}


SS
SS::operator=(const SS& right)
{
    states = right.states;
    minimized = right.minimized;
    return *this;

}


int
SS::operator<=(const SS& other) const
{
    if (not minimized)
        return states <= other.states;
    else
        return states*reach <= other.states*reach;
}


int
SS::operator>=(const SS& other) const
{
    if (not minimized)
        return states >= other.states;
    else
        return states*reach  >= other.states*reach ;
}


int
SS::operator<(const SS& other) const
{
    if (not minimized)
        return states < other.states;
    else
        return states*reach  < other.states*reach ;
}


int
SS::operator>(const SS& other) const
{
    if (not minimized)
        return states > other.states;
    else
        return states*reach  > other.states*reach ;
}


SS
SS::operator!() const
{
    if (not minimized)
        return SS(!states);
    else
        return SS(Minimize(!(states*reach)));
}


SS
SS::operator~() const
{
    if (not minimized)
        return SS(!states);
    else
        return SS(Minimize(!(states*reach)));
}


SS
SS::operator*(const SS& other) const
{
    if (not minimized)
        return SS(states*other.states);
    else
        return SS(Minimize(states*other.states));

}


SS
SS::operator*=(const SS& other)
{
    states *= other.states;
    if (minimized) Minimize();
    return *this;
}


SS
SS::operator&(const SS& other) const
{
    if (not minimized)
        return SS(states&other.states);
    else
        return SS(Minimize(states&other.states));

}


SS
SS::operator&=(const SS& other)
{
    states &= other.states;
    if (minimized) Minimize();
    return *this;

}


SS
SS::operator+(const SS& other) const
{
    if (not minimized)
        return SS(states + other.states);
    else
        return SS(Minimize(states + other.states));

}


SS
SS::operator+=(const SS& other)
{
    states += other.states;
    if (minimized) Minimize();
    return *this;
}


SS
SS::operator|(const SS& other) const
{
    if (not minimized)
        return SS(states|other.states);
    else
        return SS(Minimize(states|other.states));
}


SS
SS::operator|=(const SS& other)
{
    states |=other.states;
    if (minimized) Minimize();
    return *this;
}


SS
SS::operator^(const SS& other) const
{
    if (not minimized)
        return SS(states^other.states);
    else
        return SS(Minimize(states^other.states));
}


SS
SS::operator^=(const SS& other)
{
    states ^=other.states;
    if (minimized) Minimize();
    return *this;
}


SS
SS::operator-(const SS& other) const
{
    if (not minimized)
        return SS(states - other.states);
    else
        return SS(Minimize(states - other.states));
}


SS
SS::operator-=(const SS& other)
{
    states -= other.states;
    if (minimized) Minimize();
    return *this;
}


SS
SS::AndAbstract(const SS& g, const SS& cube) const
{
    if (not minimized)
        return SS(states.AndAbstract(g.states,cube.states));
    else
        return SS(Minimize(states.AndAbstract(g.states,cube.states)));

}


SS
SS::AndAbstractLimit(const SS& g, const SS& cube, unsigned int limit)
const
{
    if (not minimized)
        return SS(states.AndAbstractLimit(g.states,cube.states,limit));
    else
        return SS(Minimize(states.AndAbstractLimit(g.states,cube.states,limit)));
}


SS
SS::ExistAbstract(const SS& cube) const
{
    if (not minimized)
        return SS(states.ExistAbstract(cube.states));
    else
        return SS(Minimize(states.ExistAbstract(cube.states)));
}


SS
SS::Intersect(const SS& g) const
{
    if (not minimized)
        return SS(states.Intersect(g.states));
    else
        return SS(Minimize(states.Intersect(g.states)));
}


SS
SS::And(const SS& g) const
{
    if (not minimized)
        return SS(states.And(g.states));
    else
        return SS(Minimize(states.And(g.states)));
}


SS
SS::Or(const SS& g) const
{
    if (not minimized)
        return SS(states.Or(g.states));
    else
        return SS(Minimize(states.Or(g.states)));
}


SS
SS::Nand(const SS& g) const
{
    if (not minimized)
        return SS(states.Nand(g.states));
    else
        return SS(Minimize(states.Nand(g.states)));
}


SS
SS::Nor(const SS& g) const
{
    if (not minimized)
        return SS(states.Nor(g.states));
    else
        return SS(Minimize(states.Nor(g.states)));
}


SS
SS::Xor(const SS& g) const
{
    if (not minimized)
        return SS(states.Xor(g.states));
    else
        return SS(Minimize(states.Xor(g.states)));
}


SS
SS::Xnor(const SS& g) const
{
    if (not minimized)
        return SS(states.Xnor(g.states));
    else
        return SS(Minimize(states.Xnor(g.states)));
}


int
SS::Leq(const SS& g) const
{
    return states.Leq(g.states);
}


SS
SS::Cofactor(const SS& g) const
{
    return SS(states.Cofactor(g.states));
}


SS
SS::Permute(int * permut) const
{
    /*	cout << "Input to permute: ";
    	print();
    	cout << "Output: ";
    	SS(states.Permute(permut)).print();*/

    return SS(states.Permute(permut));
}


SS
SS::Constrain(const SS& c) const
{
    return SS(states.Constrain(c.states));
}


SS
SS::Restrict(const SS& c) const
{
    return SS(states.Restrict(c.states));
}


SS
SS::Minimize(const SS& c) const
{
    return SS(states.Restrict(c.states));
}

void
SS::Minimize()
{
    if (not minimized) states = states.Restrict(reach);
    minimized = true;
}


void
SS::print()  const
{
    states.print(2,2);
}

double
SS::CountMinterm() const
{
    return states.CountMinterm(mgr->ReadSize());
}

int
SS::nodeCount() const
{
    return states.nodeCount();
}

SS
SS::PickOneMinterm(int nvars) const
{
    BDDvector vb(nvars);
    int i;
    for (i = 0; i < nvars; i++)
    {
        vb[i] = mgr->bddVar(i);
    }
    return SS(states.PickOneMinterm(vb));
}

