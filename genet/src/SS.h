#ifndef SS_H_
#define SS_H_

#include "cuddObj.hh"

extern BDD reach;

// undefine SS, since it seems to be somehow definded in gcc 3.4.5 on Solaris-i386
#undef SS
class SS
{
    bool minimized;
//	BDD care_set;

public:
    BDD states;
    static Cudd *mgr;
// 	static BDD *care_set;
    SS();
    SS(int i);
    SS(const BDD &g);
    SS(const SS &from);
    bool is_empty() const
    {
        return states == mgr->bddZero();
    }
    int operator==(const SS& other) const;
    int operator!=(const SS& other) const;
    SS operator=(const SS& right);
    int operator<=(const SS& other) const;
    int operator>=(const SS& other) const;
    int operator<(const SS& other) const;
    int operator>(const SS& other) const;
    SS operator!() const;
    SS operator~() const;
    SS operator*(const SS& other) const;
    SS operator*=(const SS& other);
    SS operator&(const SS& other) const;
    SS operator&=(const SS& other);
    SS operator+(const SS& other) const;
    SS operator+=(const SS& other);
    SS operator|(const SS& other) const;
    SS operator|=(const SS& other);
    SS operator^(const SS& other) const;
    SS operator^=(const SS& other);
    SS operator-(const SS& other) const;
    SS operator-=(const SS& other);
    SS AndAbstract(const SS& g, const SS& cube) const;
    SS AndAbstractLimit(const SS& g, const SS& cube, unsigned int limit)
    const;
    SS ExistAbstract(const SS& cube) const;
    SS Intersect(const SS& g) const;
    SS And(const SS& g) const;
    SS Or(const SS& g) const;
    SS Nand(const SS& g) const;
    SS Nor(const SS& g) const;
    SS Xor(const SS& g) const;
    SS Xnor(const SS& g) const;
    int Leq(const SS& g) const;
    SS Cofactor(const SS& g) const;
    SS Permute(int * permut) const;
    SS Constrain(const SS& c) const;
    SS Restrict(const SS& c) const;
    SS Minimize(const SS& c) const;
    void Minimize();
    void emptySet()
    {
        states = mgr->bddZero();
    }
//    void setCareSet(BDD *g) {care_set = g;}

    SS PickOneMinterm(int nvars) const;

    double CountMinterm() const;
    int nodeCount() const;
    void print()  const;
    size_t getNodeSize()
    {
        return size_t(states.getNode());
    }
//    SS PickOneMinterm(SSvector vars) const;

}; // SS



#endif /*SS_H_*/
