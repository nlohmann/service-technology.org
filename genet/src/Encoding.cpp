#include "Encoding.h"

Encoding::Encoding(Tts *t, int encoding_method, Cudd *manager) {
    mgr = manager;
    if (encoding_method == LOG_ENCODING) encode(t);
    else encode_on_events(t);
}

Encoding::Encoding(const Encoding &e) {
    mgr = e.mgr;
    mencoding = e.mencoding;
    nvars = e.nvars;
    ps_vars = e.ps_vars;
    ns_vars = e.ns_vars;
}


/*! Function to perform the encoding of the TS */
void
Encoding::encode(Tts *t) {
    cout << "# Logarithmic state space encoding\n";

    map<string,bool> states;
    int nstates = 0;

    // counting the number of states
    list<Ttrans>::iterator it;
    for(it = t->ltrans.begin(); it != t->ltrans.end(); ++it) {
        string s = (*it).source;
        if (states.find(s) == states.end()) {
            states[s] = true;
            ++nstates;
        }

        string t = (*it).target;
        if (states.find(t) == states.end()) {
            states[t] = true;
            ++nstates;
        }
    }
    // finding the number of BDD variables and creating the vector
    int logstates = int(log2(double(nstates))) + 1;
    ps_vars = mgr->bddOne();
    vector<BDD> vars(logstates);
    for(int i = 0; i < logstates; ++i) {
        vars[i] =  mgr->bddVar();
        ps_vars *= vars[i];
    }

    nvars = logstates;
    nred_vars = 0;

    // encoding the states
    map<string,bool>::iterator itm;
    int code = 0;
    for(itm = states.begin(); itm != states.end(); ++itm) {
        string s = itm->first;
        mencoding[s] = create_minterm(code,0,vars);
        ++code;
        /* 	cout << "encoding for " << s << " :";
         	mencoding[s].print(logstates,2);*/
    }



    // creating the variables for building the tr's
    ns_vars = mgr->bddOne();
    int nvi = 0;
    for (int i = 0; i < nvars; ++i) {
        ns_vars *=  mgr->bddVar();
        psvar_to_nsvar[i] = nvars + nvi;
        nsvar_to_psvar[nvars+ nvi] = i;
        nvi++;
    }
    nsvars = nvi;
// cout << "NS vars: " << nsvars << endl;
}


struct hashBDD
{
    size_t operator()(const BDD a) const {

        size_t h  = size_t(a.getNode()) % 2000;
//	 cout << "           bdd key: " << h << endl;
        return h;
    }
};

class KeysBDD
{
public :

    bool operator() (const BDD a, const BDD b) const
    {
        return a == b;
    }

};


/*! Function to perform the encoding of the TS */
void
Encoding::encode_on_events(Tts *t) {

    cout << "# Event-based state space encoding\n";
    map<string,int> events;
    map<string,list<string> > states;

    hash_map<const BDD, map<string,bool>, hashBDD, KeysBDD> hash_codis;

    int nevents = 0;

    // counting the number of events
    list<Ttrans>::iterator it;
    for(it = t->ltrans.begin(); it != t->ltrans.end(); ++it) {
        string s = (*it).event;
        if (events.find(s) == events.end()) {
            events[s] = nevents++;
        }
        states[(*it).source].push_back((*it).event);
        if (states.find((*it).target) == states.end()) states[(*it).target].clear();
    }

//cout << "events: " << nevents << endl;

    // finding the number of BDD variables and creating the vector
//  int logevents = int(log2(double(nevents))) + 1;
    vector<BDD> vars(nevents);
    vars.reserve(2*nevents);

    for(int i = 0; i < nevents; ++i) {
        vars[i] = mgr->bddVar();
    }

    // encoding the states
    map<string,list<string> >::iterator itm;
    // int code = 0;
    for(itm = states.begin(); itm != states.end(); ++itm) {
        string s = itm->first;
        hash_codis[create_minterm_enabling(vars,itm->second,events)][s] = true;
    }

    hash_map<const BDD, map<string,bool>, hashBDD, KeysBDD>::const_iterator ith;
    int max_colisions = 1;
    for(ith = hash_codis.begin(); ith != hash_codis.end(); ++ith) {
        if (max_colisions < (int) ith->second.size()) max_colisions = ith->second.size();
    }

    int extra_vars = 0;
    if (max_colisions > 1) extra_vars = int(log2(double(max_colisions))) + 1;
//  cout << "extra vars: " << extra_vars << endl;
    vars.resize(nevents+extra_vars);
    for(int i = nevents; i < nevents + extra_vars; ++i) {
        vars[i] = mgr->bddVar();
    }

    nvars = nevents + extra_vars;

    // encoding the states
    BDD state_space = mgr->bddZero();
    for(ith = hash_codis.begin(); ith != hash_codis.end(); ++ith) {
        int code = 0;
        map<string,bool>::const_iterator itm;
        for(itm = ith->second.begin(); itm != ith->second.end(); ++itm) {
            mencoding[itm->first] = ith->first * create_minterm(code,nevents,vars);
            ++code;
            state_space += mencoding[itm->first];
            /*  	cout << "encoding for " <<  itm->first << " :";
              	mencoding[itm->first].print(nvars,2);*/
        }
    }

    // now trying to reduce the number of variables
    nred_vars = 0;
    for(int i = 0; i < nvars; ++i) {
        BDD sp0 = state_space.Cofactor(!vars[i]);
        BDD sp1 = state_space.Cofactor(vars[i]);
        if (sp0.Intersect(sp1) == mgr->bddZero()) {
            state_space = state_space.ExistAbstract(vars[i]);
            reds_variables.insert(i);
            ++nred_vars;
        }
    }
//  cout << "Variable redundants " << nred_vars<< endl;

    map<string, BDD>::iterator itme;
    for(itme=mencoding.begin(); itme!=mencoding.end(); ++itme) {
        set<int>::const_iterator itred;
//  	cout << "Before: "; itme->second.print(2,2);
        for(itred = reds_variables.begin(); itred != reds_variables.end(); ++itred) {
            itme->second = itme->second.ExistAbstract(vars[*itred]);
        }
//  	cout << "after: "; itme->second.print(2,2);
    }

    // creating the variables for building the tr's
    ns_vars = mgr->bddOne();
    ps_vars = mgr->bddOne();
    int nvi = 0;
    for (int i = 0; i < nvars; ++i) {
        ps_vars *= vars[i];
        if (not variable_redundant(i)) {
            ns_vars *= mgr->bddVar();
            psvar_to_nsvar[i] = nvars + nvi;
            nsvar_to_psvar[nvars+ nvi] = i;
            nvi++;
        }
    }
    nsvars = nvi;
// cout << "nombre de ns vars: " << nsvars << endl;


    /*  for(itme=mencoding.begin();itme!=mencoding.end();++itme) {
       	cout << "encoding for " <<  itme->first << " :";
      	itme->second.print(nvars,2);
      }*/

}



BDD
Encoding::create_minterm(int x, int offset, const vector<BDD> &v) {

    BDD result = mgr->bddOne();
    int i = 0;
    while (x != 0 or (offset + i) < (int) v.size()) {
        if (x % 2 == 0) result *= !v[offset+i];
        else result *= v[offset+i];
        x /= 2;
        ++i;
    }
    return result;
}

BDD
Encoding::create_minterm_enabling(const vector<BDD> &v,list<string> &enabled_events, map<string,int> &events) {

    BDD result = mgr->bddOne();
    list<string>::const_iterator itl;
    set<int> svars;
    for(itl = enabled_events.begin(); itl != enabled_events.end(); ++itl) {
        result *=v[events[*itl]];
        svars.insert(events[*itl]);
    }
    for(unsigned int i = 0; i < v.size(); ++i) {
        if (svars.find(i) == svars.end()) result *= !v[i];
    }

    return result;
}


BDD
Encoding::state(string s) {
    BDD res = mencoding[s];
    return res;
}


BDD
Encoding::change_vars(const BDD &g) {
    BDD res = mgr->bddOne();
    /*
      for (int i = nvars; i < 2*nvars; ++i) {
      	if (not variable_redundant(i)) {
      		if (g.Cofactor(mgr->bddVar(i - nvars)) != mgr->bddZero()) res *= mgr->bddVar(i);
      		else res *= !mgr->bddVar(i);
      	}
      }
      */

    for (int i = nvars; i < 2*nvars - nred_vars; ++i) {
        if (not variable_redundant(nsvar_to_psvar[i])) {
            if (g.Cofactor(mgr->bddVar(nsvar_to_psvar[i])) != mgr->bddZero()) res *= mgr->bddVar(i);
            else res *= !mgr->bddVar(i);
        }
    }

    return res;
}

BDD
Encoding::flip_tr(const BDD &g, int* , int* ) { /* int *PStoNS, int *NStoPS */
    /*	BDD gg = g;
    	BDD rev_tr = mgr->bddZero();
    	BDDvector kk(nvars*2 - nred_vars);
    	for(int i=0; i< nvars*2 - nred_vars;++i) {
    		kk[i] = mgr->bddVar(i);
    	}
    	while (gg != mgr->bddZero()) {
    		BDD s = gg.PickOneMinterm(kk);
    		gg -= s;
    		BDD s_x = s.ExistAbstract(ns_vars);
    		s_x = s_x.Permute(PStoNS);
    		BDD s_y = s.ExistAbstract(ps_vars);
    		s_y = s_y.Permute(NStoPS);
    		rev_tr += s_y * s_x;
    	}*/

    BDDvector xvars(nvars - nred_vars), yvars(nvars - nred_vars);
    for(int i=0,j=0; i< nvars; ++i) {
        if (not variable_redundant(i)) {
            xvars[j] = mgr->bddVar(i);
            ++j;
        }
    }
    for(int i=nvars; i < 2*nvars - nred_vars; ++i) {
        yvars[i - nvars] = mgr->bddVar(i);
    }
    BDD rev_tr = g.SwapVariables(xvars,yvars);
    return rev_tr;
}
