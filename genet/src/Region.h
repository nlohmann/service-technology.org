#ifndef REGIONBDD_H_
#define REGIONBDD_H_

#include <string>
#include "SS.h"
#include "TRel.h"
#include "RegionGeneric.h"

extern int extra_vars;

#define MAX_MULTIPLICITY 10000
#define MAX_RESERVATION 2

class Region : public RegionGeneric {
    vector<SS> R;
    int max_multiplicity;
    map<string,map<int,SS> > grads;
    map<string,bool> computed_grads;
    map<string,bool> events_reported;
    bool all_grads_computed;
    int ngrads;


    SS er_i(string name, int i, TRel &tr);
    void sr_i(string name, int i, TRel &tr, map<int, SS> &result, int &min, int &max);
    bool compute_maximum_lower_bound_leaving(string name, int i, int k, int kmax, bool &viol,TRel &tr);
    bool compute_maximum_lower_bound_entering(string name, int i, int k, int kmax, bool &viol,TRel &tr);
    SS collect_maximum_lower_bound_leaving(string name, int i, int k, TRel &tr);
    SS collect_maximum_lower_bound_entering(string name, int i, int k, TRel &tr);
    Region get_k_topset(int k);

public:
    Region(const SS &g);
    Region(const Region &g);
//	static int nregs;

    string choose_event_non_constant_gradient(TRel &tr);
    int number_violations(TRel &tr, bool compute_trs);
    bool check_splitting(TRel &tr);
    int power();
    int topset_weight() {
        int i = 1;
        while(R[i].is_empty()) ++i;
        return  i;
    }
    int cardinality(const SS &g) {
        int i = 0;
        while(not R[i] >= g) ++i;
        return  i;
    }
    bool is_region(TRel &tr, bool compute_trs);
    void corner_gradients(string name, TRel &tr, int &min, int &max);
    bool constant_gradient(string name, TRel &tr, int &grad);
    bool extend_arcs_leaving(string name, int k, int kmax, TRel &tr);
    bool extend_arcs_entering(string name, int k, int kmax, TRel &tr);
    bool extend_on_MG_violation(int kmax, TRel &tr);
    SS collect_arcs_leaving(string name, int k, TRel &tr);
    SS collect_arcs_entering(string name, int k, TRel &tr);
    void gradient_vector(string name, TRel &tr, bool compute_trs);
    map<int,SS> &compute_best_splitting(string &event_selected, TRel &tr);
    void minimize(SS &state_space);

    bool proper(const SS &g);
    SS project_er_positive_gradient(string trig_event,SS &er_event, TRel &tr);
    SS sup();
    double supSize() {
        return sup().CountMinterm();
    }
    int Size() {
        int s = 0;
        for (int k = power(); k >= 0; --k) s += R[k].nodeCount();
        return s;
    }
    Region compute_max_enabling_topset(const SS &er);
    void print();
    void recheck_gradients(TRel &tr, bool compute_trs);
    void lazy_recheck_gradients() {
        all_grads_computed = false;
    }

    bool is_MG_region(TRel &tr);
    int number_gradients() {
        return ngrads;
    }

    Region &operator=(const Region &other);
    Region operator+=(const Region &other);
    Region operator-=(const Region &other);
    bool operator<(const Region &other) const;
    bool operator<=(const Region &other) const;
    bool operator>(const Region &other) const;
    bool operator==(const Region &other) const;
    void Minimize()  {
        for(int i = 0; i <= power(); ++i) R[i].Minimize();
    }

    void complement();
    void remove(const SS &g);

    SS get_i(int i) {
        return R[i];
    }
    size_t cache_key();

    map<string,map<int,SS> > &get_map_grads() {
        return grads;
    }

    //new: to support conservative components generation
    bool additive_union(const Region &other, int maxk);
    bool test_additive_union(const Region &other, int maxk);
    void additive_substraction(const Region &other);
    virtual ~Region() {}
};

#endif /*REGIONBDD_H_*/
