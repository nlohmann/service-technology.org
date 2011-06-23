#include <iostream>
#include <sstream>
#include <set>
#include <queue> // priority_queue
#include <ext/hash_map>
#include "TRel.h"
#include "Region.h"
#include "parser/global.h"


//interfacing the alglib library
#include "ap.h"
#include "blas.h"
#include "rotations.h"
#include "tdevd.h"
#include "sblas.h"
#include "reflections.h"
#include "tridiagonal.h"
#include "sevd.h"

extern bool generate_conservative_cover(TRel &tr, int kmax, const SS &state_space, const SS &initial_state, list<Region *> &minregs);
extern bool generate_conservative_sm_cover(TRel &tr, int kmax, const SS &state_space, const SS &initial_state, list<Region *> &minregs);
extern bool generate_minimal_regions_on_demand(TRel &tr, int k, int explore_regions, const SS &state_space,
        list<Region *> &minregs, list<string> &violation_events);
extern bool generate_minimal_regions_all(TRel &tr, int k, const SS &state_space,
        list<Region *> &minregs,bool mgcond, bool force_ec_sigs, map<string,bool> &ec_sigs,list<string> &violation_events);

extern void pn_synthesis(const list<Region *> &minregs,TRel &tr, const SS &initial, int KMAX);
extern void pn_synthesis(const string outfile,const list<Region *> &minregs,TRel &tr, const SS &initial, int KMAX);



/*************************************************************************
Unsets 2D array.
*************************************************************************/
void unset2d(ap::real_2d_array& a)
{

    a.setbounds(0, 0, 0, 0);
    a(0,0) = 2*ap::randomreal()-1;
}


/*************************************************************************
Unsets 1D array.
*************************************************************************/
void unset1d(ap::real_1d_array& a)
{

    a.setbounds(0, 0);
    a(0) = 2*ap::randomreal()-1;
}

/*************************************************************************
Spectral partition (cut) of the causal dependencies graph
*************************************************************************/
bool find_spectral_partition(TRel &tr,map<string,bool> &cut1,map<string,bool> &cut2)
{
    map<string,EvTRel *>::const_iterator itm1;
    map<string,EvTRel *> tr_map = tr.get_map_trs();
    if (tr_map.size() <= 7) return true;
    map<string,int> ev_to_ind;
    vector<string> ind_to_ev(tr_map.size());
    int n = 0;
    for(itm1=tr_map.begin(); itm1 != tr_map.end(); ++itm1) {
//cout << n << ": " << itm1->first << endl;
        ind_to_ev[n] = itm1->first;
        ev_to_ind[itm1->first] = n++;
    }

    // bool result;
    ap::real_2d_array al;
    ap::real_2d_array z;
    int i;
    int j;

    al.setbounds(0, n-1, 0, n-1);
    for(i = 0; i <= n-1; i++) {
        for(j = 0; j <= n-1; j++) {
            al(i,j) = al(i,j) = 0 ;
        }
    }
    // Loading the Laplacian matrix
    int nord = 0;
    int nconc = 0;
    for(itm1=tr_map.begin(); itm1 != tr_map.end(); ++itm1) {
        EvTRel *ev1 = itm1->second;
        map<string,EvTRel *>::const_iterator itm2;
        for(itm2=tr_map.begin(); itm2 != tr_map.end(); ++itm2) {
            if (itm1 == itm2)  continue;
            EvTRel *ev2 = itm2->second;
            if ((not ev1->get_sr().Intersect(ev2->get_er()).is_empty()) and ev2->get_sr().Intersect(ev1->get_er()).is_empty()) {
                al(ev_to_ind[itm1->first],ev_to_ind[itm2->first]) = al(ev_to_ind[itm2->first],ev_to_ind[itm1->first]) = -1;
                al(ev_to_ind[itm1->first],ev_to_ind[itm1->first])++;
                al(ev_to_ind[itm2->first],ev_to_ind[itm2->first])++;
                ++nord;
            }
            if ((not ev1->get_sr().Intersect(ev2->get_er()).is_empty()) and not ev2->get_sr().Intersect(ev1->get_er()).is_empty()) ++nconc;
        }
    }
    //cout << "|ord|: " << nord << " |concs|: " << nconc <<endl;
    // Heuristic to decide when the recursion might be finished
    //cout << nconc << " " << n*n/10 << endl;
    if (n*n/10 > nconc) {
        cout << "# Low concurrency degree exhibited in the considered log\n";
        return true;
    }

    //
    // Find eigenvalues and eigenvectors
    //
    ap::real_1d_array lambda;
    ap::real_1d_array lambdaref;
    bool wsucc;
    // double v;

    unset1d(lambdaref);
    unset2d(z);
    wsucc = smatrixevd(al, n, 1, false, lambdaref, z);
    if( !wsucc )
    {
        cout << "Failing at computing spectral bipartition\n";
        exit(0);
    }


    // Obtaining the partition
    int size_cut1 = 0, size_cut2 = 0;
    for(i=0; i <= n-1; ++i) {
        if (z(i,1) < 0) {
            cut1[ind_to_ev[i]] = true;
            ++size_cut1;
        }
        else {
            cut2[ind_to_ev[i]] = true;
            ++size_cut2;
        }
    }
    /*		cout <<"cut1 (inside spectral): ";
    		map<string,bool>::const_iterator it;
    		for(it=cut1.begin(); it!=cut1.end(); ++it) {
    			if (it->second)		cout << " " << it->first;
    		}
    		cout << endl;
    		cout << "cut2 (inside spectral): ";
    		for(it=cut2.begin(); it!=cut2.end(); ++it) {
    			if (it->second)		cout << " " << it->first;
    		}
    		cout << endl;
    */
    // adding the borders: only allow to add at most a proportion of the events in the set.
    map<string,bool>::iterator it;
    int try_first = 0, ntries = 0;
    if (size_cut1 > size_cut2) try_first = 1;
    bool success_cut = false;
    map<string,bool> tmp;
    do {
        int max_border;
        switch(try_first) {
        case 0:
            tmp.clear();
            max_border = size_cut1/2 + 1;
            for(it = cut1.begin(); it != cut1.end() and max_border > 0; ++it) {
                if (it->second) {
                    for(i=0; i <= n-1 and max_border > 0; ++i) {
                        if ((ev_to_ind[it->first] == i) or (al(ev_to_ind[it->first],i) == 0) or (not cut2[ind_to_ev[i]]) or tmp[ind_to_ev[i]]) continue;
                        tmp[ind_to_ev[i]] = true;
                        //						cout << "  Adding " << ind_to_ev[i] << " to cut1" << endl;
                        --max_border;
                    }
                }
            }
            // adding only the borders when the partition is not nearly trivial
            if ((int) (cut1.size() + tmp.size()) <= n - 1) {
                for(map<string,bool>::iterator it1 = tmp.begin(); it1 != tmp.end(); ++it1) {
                    cut1[it1->first] = true;
                }
                success_cut = true;
            }
            break;
        case 1:
            tmp.clear();
            max_border = size_cut2/2 + 1;
            for(it = cut2.begin(); it != cut2.end() and max_border > 0; ++it) {
                if (it->second) {
                    for(i=0; i <= n-1 and max_border > 0; ++i) {
                        if ((ev_to_ind[it->first] == i) or (al(ev_to_ind[it->first],i) == 0) or (not cut1[ind_to_ev[i]]) or tmp[ind_to_ev[i]]) continue;
                        tmp[ind_to_ev[i]] = true;
//  		   			cout << "  Adding " << ind_to_ev[i] << " to cut2" << endl;
                        --max_border;
                    }
                }
            }
            if ((int)(cut2.size() + tmp.size()) <= n - 1) {
                for(map<string,bool>::iterator it1 = tmp.begin(); it1 != tmp.end(); ++it1) {
                    cut2[it1->first] = true;
                }
                success_cut = true;
            }
            break;
        }
        try_first = (try_first + 1) %2;
        ++ntries;
    } while (ntries < 2 and not success_cut);

    /*	if (not succes_cut) {
      	cout << "Only half of the interface events were incorporated in one of the cuts\n";
      	int k = tmp.size();
      	cout << "Size before: " << k << endl;
      	k /= 2;
     		k = tmp.size();
     		cout << "Size after: " << k << endl;
     		for(map<string,bool>::iterator it1 = tmp.begin();it1 != tmp.end() and k >= 0;++it1,--k){
     			cut2[it1->first] = true;
     		}
     		assert(cut2.size() < n -1);
      }
      */
    assert(success_cut);
    return false;

}



void divide_conquer_cc(TRel &tr, int kmax, const SS &state_space, const SS &initial_state, list<Region *> &minregs) {


    map<string,bool> cut1,cut2;
    bool leaf = find_spectral_partition(tr,cut1,cut2);
    map<string,bool>::const_iterator it;
    if (not leaf and not cut1.empty() and not cut2.empty()) {
        cout <<"# cut1: ";
        for(it=cut1.begin(); it!=cut1.end(); ++it) {
            if (it->second)		cout << " " << it->first;
        }
        cout << endl;
        cout << "# cut2: ";
        for(it=cut2.begin(); it!=cut2.end(); ++it) {
            if (it->second)		cout << " " << it->first;
        }
        cout << endl;
        TRel tr1(tr.get_encoding(),tr.get_ts()), tr2(tr.get_encoding(),tr.get_ts());
//		tr.partition_tr(cut, tr1, tr2);
        tr1.build_tr(cut1);
        SS state_space1 = tr1.compute_local_state_space();
//		state_space1.print();
        list<Region *> minregs1;
        divide_conquer_cc(tr1,kmax,state_space1,initial_state,minregs1);
        tr2.build_tr(cut2);
        SS state_space2 = tr2.compute_local_state_space();
//		generate_conservative_cover(tr2, kmax, state_space2,initial_state,minregs);
        list<Region *> minregs2;
        divide_conquer_cc(tr2,kmax,state_space2,initial_state,minregs2);
    }
    else {
        if (kmax == 1) generate_conservative_sm_cover(tr, kmax, state_space,initial_state,minregs);
        else generate_conservative_cover(tr, kmax, state_space,initial_state,minregs);
    }
}

void divide_conquer_mining(TRel &tr, int kmax, const SS &state_space, const SS &initial_state, list<Region *> &minregs) {


    map<string,bool> cut1,cut2;
    bool leaf = find_spectral_partition(tr,cut1,cut2);
    map<string,bool>::const_iterator it;
    if (not leaf and not cut1.empty() and not cut2.empty()) {
        cout <<"cut1: ";
        for(it=cut1.begin(); it!=cut1.end(); ++it) {
            cout << " " << it->first;
        }
        cout << endl;
        cout << "cut2: ";
        for(it=cut2.begin(); it!=cut2.end(); ++it) {
            cout << " " << it->first;
        }
        cout << endl;
        TRel tr1(tr.get_encoding(),tr.get_ts()), tr2(tr.get_encoding(),tr.get_ts());
//		tr.partition_tr(cut, tr1, tr2);
        tr1.build_tr(cut1);
        SS state_space1 = tr1.compute_local_state_space();
//		state_space1.print();
        list<Region *> minregs1;
        divide_conquer_mining(tr1,kmax,state_space1,initial_state,minregs1);
        tr2.build_tr(cut2);
        SS state_space2 = tr2.compute_local_state_space();
//		generate_conservative_cover(tr2, kmax, state_space2,initial_state,minregs);
        list<Region *> minregs2;
        divide_conquer_mining(tr2,kmax,state_space2,initial_state,minregs2);
    }
    else {
        map<string,bool> ec_sigs;
        list<string> violation_events;
        generate_minimal_regions_all(tr, kmax, state_space,minregs,false, false,ec_sigs,violation_events);
        pn_synthesis(minregs,tr, initial_state, kmax);
    }
}

void divide_conquer_synthesis(TRel &tr, int kmax, const SS &state_space, const SS &initial_state, list<Region *> &minregs) {


    map<string,bool> cut1,cut2;
    bool leaf = find_spectral_partition(tr,cut1,cut2);
    map<string,bool>::const_iterator it;
    if (not leaf and not cut1.empty() and not cut2.empty()) {
        cout <<"cut1: ";
        for(it=cut1.begin(); it!=cut1.end(); ++it) {
            cout << " " << it->first;
        }
        cout << endl;
        cout << "cut2: ";
        for(it=cut2.begin(); it!=cut2.end(); ++it) {
            cout << " " << it->first;
        }
        cout << endl;
        TRel tr1(tr.get_encoding(),tr.get_ts()), tr2(tr.get_encoding(),tr.get_ts());
//		tr.partition_tr(cut, tr1, tr2);
        tr1.build_tr(cut1);
        SS state_space1 = tr1.compute_local_state_space();
//		state_space1.print();
        list<Region *> minregs1;
        divide_conquer_mining(tr1,kmax,state_space1,initial_state,minregs1);
        tr2.build_tr(cut2);
        SS state_space2 = tr2.compute_local_state_space();
//		generate_conservative_cover(tr2, kmax, state_space2,initial_state,minregs);
        list<Region *> minregs2;
        divide_conquer_mining(tr2,kmax,state_space2,initial_state,minregs2);
    }
    else {
        list<string> viol_events;
        generate_minimal_regions_on_demand(tr, kmax, 1, state_space,minregs,viol_events);
        pn_synthesis(minregs,tr, initial_state, kmax);
    }
}
