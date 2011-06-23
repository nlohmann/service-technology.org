#include "EvTRel.h"

EvTRel::EvTRel(const EvTRel &e) {
    name = e.name;
//	mgr = e.mgr;
    ER = e.ER;
    SR = e.SR;
    TR = e.TR;
    ec = e.ec;
    sizetr = e.sizetr;
    /*	states_intersect = e.states_intersect;
    	recompute_cover = e.recompute_cover;*/
}

EvTRel&
EvTRel::operator=(const EvTRel &other) {
    name = other.name;
//	mgr = other.mgr;
    ER = other.ER;
    SR = other.SR;
    TR = other.TR;
    ec = other.ec;
    sizetr = other.sizetr;
    /*	states_intersect = other.states_intersect;
    	recompute_cover = other.recompute_cover;*/
    return *this;
}


EvTRel::EvTRel(string n/*, Cudd m*/) {
    name = n;
//	mgr = m;
//	ER = SR = TR = mgr.bddZero();
    ER.emptySet();
    SR.emptySet();
    TR.emptySet();

    ec = false;
    sizetr = 0;
    /*	states_intersect = mgr.bddOne();
    	recompute_cover = true;*/
}

SS
EvTRel::add_er(const SS &g) {
    ER = ER + g;
    return ER;
}

SS
EvTRel::add_sr(const SS &g) {
    SR = SR + g;
    return SR;
}

SS
EvTRel::add_tr(const SS &g) {
    TR = TR + g;
    ++sizetr;
    return TR;
}

SS
EvTRel::add_rev_tr(const SS &g) {
    rTR = rTR + g;
    return rTR;
}


SS
EvTRel::set_er(const SS &g) {
    ER = g;
    return ER;
}

SS
EvTRel::set_sr(const SS &g) {
    SR = g;
    return SR;
}


SS
EvTRel::set_tr(const SS &g) {
    TR = g;
    return TR;
}

SS
EvTRel::set_rev_tr(const SS &g) {
    rTR = g;
    return rTR;
}
