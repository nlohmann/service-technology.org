#ifndef REGIONGENERIC_H_
#define REGIONGENERIC_H_

#include "TRel.h"
#include "SS.h"

class RegionGeneric {

    virtual bool constant_gradient(string name, TRel &tr, int &grad) = 0;

public:
    virtual string choose_event_non_constant_gradient(TRel &tr) = 0;
    virtual int power() = 0;
    virtual bool is_region(TRel &tr, bool compute_trs) = 0;
    virtual void corner_gradients(string name, TRel &tr, int &min, int &max) = 0;
    virtual	bool extend_arcs_leaving(string name, int k, int kmax, TRel &tr) = 0;
    virtual	bool extend_arcs_entering(string name, int k, int kmax, TRel &tr) = 0;
    virtual bool proper(const SS &g) = 0;
};

#endif /*REGIONGENERIC_H_*/
