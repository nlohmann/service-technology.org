#ifndef EVTREL_H_
#define EVTREL_H_


//#include "cuddObj.hh"
#include "SS.h"

class EvTRel
{
    string name;
//	Cudd mgr;
    SS ER;
    SS SR;
    SS TR;
    SS rTR;
    bool ec;
    /*	SS states_intersect;
    	bool recompute_cover;*/
    int sizetr;


public:
//	EvTRel() { };
    EvTRel(const EvTRel &e);
    EvTRel(string n/*, Cudd m*/);
    SS add_er(const SS &g);
    SS add_sr(const SS &g);
    SS add_tr(const SS &g);
    SS add_rev_tr(const SS &g);

    SS set_er(const SS &g);
    SS set_sr(const SS &g);
    SS set_tr(const SS &g);
    SS set_rev_tr(const SS &g);


    SS get_tr()
    {
        return TR;
    }
    SS get_rev_tr()
    {
        return rTR;
    }
    SS get_er()
    {
        return ER;
    }
    SS get_sr()
    {
        return SR;
    }
    int get_sizetr() const
    {
        return sizetr;
    }

    EvTRel& operator=(const EvTRel& other);

    void print_name() const
    {
        cout << name << " ";
    }

    void print_er()
    {
        ER.print();
    }
    void print_tr()
    {
        TR.print();
    }
    void Minimize()
    {
        ER.Minimize();
        SR.Minimize();
        TR.Minimize();
    }

    /*	void print_er() { ER.print(mgr.ReadSize(),2); }
    	void print_tr() { TR.print(mgr.ReadSize(),2); }*/
    bool is_excitation_closed() const
    {
        return ec;
    }
    void set_excitation_closed(bool b)
    {
        ec = b;
    }
    int getNodeCount()
    {
        return size_t(ER.nodeCount() + SR.nodeCount() + TR.nodeCount());
    }
    /*	void set_intersect_covering(SS &g) {states_intersect = g;}
    	void add_covering_region(SS &g) { states_intersect *= g;}
    	SS get_intersect_covering() {return states_intersect;}
    	bool need_recompute_cover() {return recompute_cover;}
    	void set_computed_cover() { recompute_cover = true;}
    */
};


#endif /*EVTREL_H_*/
