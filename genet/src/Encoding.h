#ifndef ENCODING_H_
#define ENCODING_H_

#include <ext/hash_map>
#include <cmath>
#include <map>
#include <vector>
#include "cuddObj.hh"
#include "parser/global.h"
#include <set>
//#include "HashRegions.h"

/*! Class for the type of encoding done on the set of states of the TS.
 *  For the time being, one-hot encoding is used.
 *
 */

const int LOG_ENCODING = 0;
const int EVENT_ENCODING = 1;


using namespace __gnu_cxx; // extension to include the hash_map

class Encoding {
    map<string, BDD> mencoding;
    Cudd *mgr;
    int nvars;
    int nsvars;
    int nred_vars;
    set<int> reds_variables;
    map<int,int> psvar_to_nsvar;
    map<int,int> nsvar_to_psvar;

    BDD create_minterm(int x, int offset,const vector<BDD> &v);
    BDD create_minterm_enabling(const vector<BDD> &v,list<string> &enabled_events, map<string,int> &events);

public:
    BDD ps_vars, ns_vars;
    Encoding() {}
    Encoding(Tts *t, int encoding_method, Cudd *manager);
    Encoding(const Encoding &e);
    void encode(Tts *t);
    void encode_on_events(Tts *t);
    BDD state(string s);
    int enc_vars() const {
        return nvars;
    }
    int red_vars() const {
        return nred_vars;
    }
    int next_vars() const {
        return nsvars;
    }
    BDD change_vars(const BDD &g);
    bool variable_redundant(int i) {
        return reds_variables.find(i) != reds_variables.end();
    }
    int ns_var_index(int i)  {
        assert(i < nvars);
        return psvar_to_nsvar[i];
    }
    int ps_var_index(int i)  {
        assert(i >= nvars);
        return nsvar_to_psvar[i];
    }
    BDD flip_tr(const BDD &g, int *PStoNS, int *NStoPS);

};


#endif /*ENCODING_H_*/
