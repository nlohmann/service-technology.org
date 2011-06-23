#ifndef TREL_H_
#define TREL_H_

#include "parser/global.h"
#include "Encoding.h"
//#include "cuddObj.hh"
#include "SS.h"
#include "EvTRel.h"


#define MAX_VARS 200

using namespace std;

/*! Class to implement the disjunctive TR */
class TRel {

    friend class Region;

    Tts *mts;
    Cudd mgr;
    Encoding *enc;
    map<string,EvTRel *>  eventTRs;
    map<string,int> fresh_copy;
    map<string,bool> computed_ecs;
    int *PStoNS,*NStoPS;

    /*	BDD rename_vars_to_x(const BDD &g, int i);
    	BDD rename_vars_to_y(const BDD &g, int i);*/


public:
    TRel(Encoding *e,Tts *ts/*, const Cudd &manager*/);
    void build_tr(map<string,bool> &visible_events  /*const string prj_signals*/);
    SS forward(const SS &g);
    SS forward_restrict(const SS &g, const SS &r);
    SS backward(const SS &g);
    SS backward_restrict(const SS &g, const SS &r);
    SS traversal(const SS &g);
    SS traversal_restrict(const SS &g, const SS &r);
    SS backward_traversal_restrict(const SS &g, const SS &r);
    void connected_components(const SS &g, vector<SS> &comps);
    SS image(string name, const SS &g);
    SS back_image(string name, const SS &g);
    SS compute_local_state_space();

    Encoding *get_encoding() {
        return enc;
    }
    Tts *get_ts() {
        return mts;
    }
    map<string,bool> tokenize(const string prj_sigs, const string pattern);


    void trigger_set(string name, list<string>& triggset);

    void split_event_er(string name, const SS &g);
    void split_event_sr(string name, const SS &g);
//	void split_event_gradients(string event_selected, map<int,SS> &gradients);
    void split_event_gradients(string event_selected, map<int,SS> &gradients, list<string> &new_names);

    EvTRel *get_event_tr(string name) {
        return eventTRs[name];
    }
    map<string,EvTRel *> &get_map_trs() {
        return eventTRs;
    }
    int number_events() {
        return eventTRs.size();
    }
    bool computed_ec(string name) {
        return computed_ecs[name];
    }
    void change_computed_ec(string name,bool b) {
        computed_ecs[name] = b;
    }


};

#endif /*TREL_H_*/
