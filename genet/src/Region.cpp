#include "Region.h"


Region::Region(const SS &g) {
    R = vector<SS>(2);
    R.reserve(MAX_RESERVATION);  // to avoid reallocations ...
    max_multiplicity = 1;
    ngrads = 0;
    all_grads_computed = false;
    R[0] = !g;
    R[1] = g; 			// because the regions are initiallized with ER/SR, we know the multiplicity is 1
    //cout << " new region " << nregs++ << endl;

//   cout << "new region created. Size: " << R.size() << " capacity: " << R.capacity() << endl;
}

Region::Region(const Region &rg) {
    R.reserve(MAX_RESERVATION);
    R = rg.R;
//  grads = rg.grads;
//  computed_grads = rg.computed_grads;
    ngrads = rg.ngrads;
    all_grads_computed = rg.all_grads_computed;
    max_multiplicity = rg.max_multiplicity;
    /*  for(int i = 0; i <= max_multiplicity; ++i) R[i] = rg.R[i];
      for(int i = max_multiplicity +1; i < MAX_VECTOR_D; ++i) R[i] = mgr.bddZero();*/
    //cout << " new region " << nregs++ << endl;

}

Region &
Region::operator=(const Region &other) {
    R.reserve(MAX_RESERVATION);
    R = other.R;
//  grads = other.grads;
//  computed_grads = other.computed_grads;
    ngrads = other.ngrads;
    all_grads_computed = other.all_grads_computed;
    max_multiplicity = other.max_multiplicity;
    return *this;
}

bool
Region::operator<=(const Region &other) const {
    bool result = R[0] > other.R[0];
    bool some_greater = false;
    int i = 1;
//	SS accum = mgr->bddZero();
    SS accum;
    while (result and i <= MIN(max_multiplicity,other.max_multiplicity)) {
        accum -= accum * other.R[i];
        accum += R[i] - other.R[i];
        some_greater = some_greater or R[i] <= other.R[i];
        ++i;
    }
    return result and some_greater and i == max_multiplicity + 1 and accum.is_empty();
}

bool
Region::operator<(const Region &other) const {
    bool result = R[0] >= other.R[0];
    bool some_greater = false;
    int i = 1;
    //SS accum = mgr->bddZero();
    SS accum;
    while (result and i <= MIN(max_multiplicity,other.max_multiplicity)) {
        accum -= accum * other.R[i];
        accum += R[i] - other.R[i];
        some_greater = some_greater or R[i] < other.R[i];
        ++i;
    }
    return result and some_greater and i == max_multiplicity + 1 and accum.is_empty();
}

bool
Region::operator>(const Region &other) const {
    bool result = R[0] <= other.R[0];
    bool some_greater = false;
    int i = 1;
//	SS accum = mgr->bddZero();
    SS accum;
    while (result and i <= MIN(max_multiplicity,other.max_multiplicity)) {
        accum -= accum * R[i];
        accum += other.R[i] - R[i];
        some_greater = some_greater or R[i] > other.R[i];
        ++i;
    }
    return result and some_greater and i == other.max_multiplicity + 1 and accum.is_empty();
}

bool
Region::operator==(const Region &other) const {
//	bool kk = (R[0] == other.R[0]);
//cout << "Comparing with " << max_multiplicity <<  " " << other.max_multiplicity << "\n";
//print();
    bool result = max_multiplicity == other.max_multiplicity  and R[0] == other.R[0];
    int i = 1;
    while (result and i <= max_multiplicity) {
        result = R[i] == other.R[i];
//		cout << "--- " << i << endl;
        ++i;
    }
//cout << "\nResult: " << result << endl;
    return result;
}

Region
Region::operator+=(const Region &other) {
    for (int i = 1; i <= other.max_multiplicity; ++i) {
        SS x_i = other.R[i];
        for (int j = power(); j >= i; --j) {
            x_i -= R[j];
        }
        if (not x_i.is_empty()) {
            if (i > max_multiplicity) {
                max_multiplicity = i;
                R.resize(max_multiplicity + 1,SS());
                R[i].emptySet();
            }
            R[i] += x_i;
            for (int k = i-1; k>=0; k--) {
                R[k] -= x_i;
            }
        }
    }
    return *this;
}

Region
Region::operator-=(const Region &other) {
    for (int i = 1; i <= other.max_multiplicity; ++i) {
        SS x_i = other.R[i];
        for (int j = power(); j >= i; --j) {
            SS rmv_i = x_i * R[j];
            if (not rmv_i.is_empty()) {
                R[j] -= rmv_i;
                R[(j-i < 0 ? 0 : j-i)] += rmv_i;
                x_i -= rmv_i;
            }
        }
    }
    return *this;
}

void
Region::complement() {
    int n = power();
    for(int i = 0; i < n / 2; ++i) {
        SS tmp = R[i];
        R[i] = R[n-i];
        R[n-i] = tmp;
    }
}

void
Region::remove(const SS &g) {
    int n = power();
    for (int k = n; k > 0; --k) {
        R[k] -= g;
        if ((k == max_multiplicity) and (R[k].is_empty())) --max_multiplicity;
    }
    R[0] += g;
}


// this should be changed. There must be an easy and efficient way to do this in Cudd++
int
Region::power() {
    return max_multiplicity;
}

Region
Region::compute_max_enabling_topset(const SS &er) {
    if (not (er.Intersect(R[0]).is_empty())) {
        return Region(SS());
    }
    else {
        for (int k = power(); k > 0; --k) {
            Region rk = get_k_topset(k);
            if (rk.sup() >= er) {
//				cout << "topset found for " << k << endl;
                return rk;
            }
        }
//		cout << "Any topset contains er\n";
        return Region(SS());
    }
}


void
Region::print() {
    for (int i = 0; i <= max_multiplicity; ++i) {
        cout << "  R[" << i << "] ";
        R[i].print();
//		cout << " " << &R[i] << endl;
    }
}

size_t
Region::cache_key() {
    size_t h = 0;
    for (int i = 0; i <= max_multiplicity; ++i) {
//  	cout << "adreca element  "<< i << " " << &R[i] << endl;
        size_t k = R[i].getNodeSize();
//	 	size_t(R[i].getNode());
//	 	cout << "k :" << k << endl;
        h += 13 * k;
    }

    return h;
}

bool
Region::check_splitting(TRel &tr) {
    map<string,EvTRel *>::const_iterator itm;
    map<string,EvTRel *> tr_map = tr.get_map_trs();
    for(itm=tr_map.begin(); itm != tr_map.end(); ++itm) {
        if (not tr.computed_ec(itm->first) and grads[itm->first].find(0) == grads[itm->first].end()) {
//cout << "Grad Checking for " << 	itm->first << endl;
            gradient_vector(itm->first, tr,true);
            computed_grads[itm->first] = true;
            if (grads[itm->first].size() > 1) return false;
            tr.change_computed_ec(itm->first,true);
        }
    }
    // no violations where found so the region can be reused
    ngrads = tr.eventTRs.size();
    return true;
}

// this member function assumes the multiset is not a region
string
Region::choose_event_non_constant_gradient(TRel &tr) {
    if (not all_grads_computed) {
        string result = "";
        ngrads = 0;
        map<string,EvTRel *>::iterator itb;
        for (itb = tr.eventTRs.begin(); itb != tr.eventTRs.end(); ++itb) {
            /* EvTRel *etr = itb->second; */
            if (not computed_grads[itb->first]) {
                gradient_vector(itb->first, tr,false);
                computed_grads[itb->first] = true;
            }
            if ((not events_reported[itb->first]) and (grads[itb->first].size() > 1)) {
                events_reported[itb->first] = true;
                result = itb->first;
//  			return itb->first;		// i dont know yet why this is not improving the search, but the contrary
            }
            ngrads += grads[itb->first].size();
        }
//cout << " --> " << ngrads - tr.eventTRs.size() << endl;
        all_grads_computed = true;
        return result;
    }
    else {
        // the information for the actual multiset is available, so only a search for violating evens is done
        string result = "";
        map<string,EvTRel *>::iterator itb;
        for (itb = tr.eventTRs.begin(); itb != tr.eventTRs.end(); ++itb) {
            // EvTRel *etr = itb->second;
            if ((not events_reported[itb->first]) and (grads[itb->first].size() > 1)) {
                events_reported[itb->first] = true;
                return itb->first;
            }
        }
        return result;
    }
}

// this member function assumes the multiset is not a region
int
Region::number_violations(TRel &tr, bool compute_trs) {
    if (all_grads_computed) return (ngrads - tr.eventTRs.size());

    map<string,EvTRel *>::iterator itb;
    ngrads = 0;
    for (itb = tr.eventTRs.begin(); itb != tr.eventTRs.end(); ++itb) {
        if (not computed_grads[itb->first]) {
            gradient_vector(itb->first, tr,compute_trs);
            computed_grads[itb->first] = true;
        }
        if (grads[itb->first].size() > 1) return 1;
        ngrads += grads[itb->first].size();
// 	if (grads[itb->first].size() > 1) cout << "viol for " << itb->first << endl;
    }
    all_grads_computed =  true;
//cout << " --> " << ngrads - tr.eventTRs.size() << endl;
    return ngrads - tr.eventTRs.size();
}

bool
Region::is_region(TRel &tr, bool compute_trs) {
//  return choose_event_non_constant_gradient(tr) == "";
    return number_violations(tr,compute_trs) == 0;
}

void
Region::recheck_gradients(TRel &tr, bool compute_trs) {
    map<string,EvTRel *>::iterator itb;
    ngrads = 0;
    for (itb = tr.eventTRs.begin(); itb != tr.eventTRs.end(); ++itb) {
        gradient_vector(itb->first, tr,compute_trs);
        ngrads += grads[itb->first].size();
        computed_grads[itb->first] = true;
    }
    all_grads_computed =  true;
}

void
Region::corner_gradients(string name, TRel &tr, int &min, int &max) {

    if /*(not all_grads_computed)*/ (not computed_grads[name]) {
// 	choose_event_non_constant_gradient(tr);
        gradient_vector(name,tr,false);
    }

    if (grads[name].empty()) {
        gradient_vector(name,tr,false);
    }
    min = max = grads[name].begin()->first;
    for(map<int,SS>::const_iterator it = grads[name].begin(); it != grads[name].end(); ++it) {
        if (it->first < min) min = it->first;
        if (it->first > max) max = it->first;
    }
    /*
     	bool first_grad = true;
      int i = 0;

      do {
      	map<int, SS> srv_i;
        int max_local_grad,min_local_grad;

        sr_i(name, i, tr,srv_i,min_local_grad,max_local_grad);

        if (first_grad) {
          first_grad = false;
          min = min_local_grad;
          max = max_local_grad;
        }
        else {
        	min = MIN(min_local_grad, min);
        	max = MAX(max_local_grad, max);
        }
        ++i;
      } while (i != power() + 1);	*/
}

void
Region::gradient_vector(string name, TRel &tr, bool compute_trs) {
    int i = 0;
    EvTRel *evt = tr.get_event_tr(name);
    SS er_event = evt->get_er();
    SS ev_tr = evt->get_tr();

    if (computed_grads[name]) grads[name].clear();
    do {
        SS er_i = R[i] * er_event;
        SS img_er_i = tr.image(name,er_i);
        if (not img_er_i.is_empty()) {
            er_event -= er_i;
            for (int k = 0; k <= power() and (not img_er_i.is_empty()); ++k) {
                SS x =  img_er_i * R[k];
                if (not x.is_empty()) {
                    img_er_i -= x;
                    SS g;
                    if (compute_trs) {
                        SS yx = x.Permute(tr.PStoNS);
                        SS xy = tr.back_image(name,x) * er_i;
                        g =  ev_tr * xy;
                        g *= yx;
                    }
                    if (grads[name].find(k-i) == grads[name].end()) grads[name][k-i].emptySet();
                    if (compute_trs) grads[name][k-i] += g;
                }
            }
        }
        ++i;
    } while ((i != power() + 1) and (not er_event.is_empty()));
    /*  cout << "Grads for event: " << name << endl;
      for(map<int,SS>::const_iterator it=vgrad.begin(); it != vgrad.end(); ++it){cout << "Grad " << it->first << ": ";it->second.print(); cout <<endl;}*/
    //  cout << "Grad Size for " << name << " is: " << vgrad.size() <<endl;
}

map<int,SS> &
Region::compute_best_splitting(string &event_selected, TRel & /*tr*/) {
//cout << "Region selected:";
//print();
    int best_grads = grads.begin()->second.size();
    event_selected = grads.begin()->first;
    map<string,map<int,SS> >::const_iterator itg = ++grads.begin();
    while (itg != grads.end()) {
//cout << "grad for " << 		itg->first << " is " << itg->second.size() << endl;
        // EvTRel *evt = tr.get_event_tr(itg->first);
//		if (tr.computed_ec(itg->first) and evt->is_excitation_closed()) {++itg;continue;}
        if (best_grads == 1 or (best_grads > (int) itg->second.size() and itg->second.size() > 1)) {
            best_grads = itg->second.size();
            event_selected = itg->first;
        }
        ++itg;
    }
    return grads[event_selected];
}

/*! Fix-point algorithm for extending the region in order to have out-going arcs
 * with gradient less than k
 */
bool
Region::extend_arcs_leaving(string name, int k, int kmax, TRel &tr) {

    bool changes;
    do {
        changes = false;
        for(int i = 0; i <= power(); ++i) {
            bool viol = false;
            bool b = compute_maximum_lower_bound_leaving(name,i,k,kmax,viol,tr);
            changes = changes or b;
            if (viol) return false;
        }
    } while (changes);

    // recompute the number of gradients
    ngrads = 0;
    all_grads_computed = false;
    map<string,EvTRel *>::iterator itb;
    for (itb = tr.eventTRs.begin(); itb != tr.eventTRs.end(); ++itb) {
        computed_grads[itb->first] = false;
        grads[itb->first].clear();
    }

    return true;
//  cout << "Recomputed number of gradients: " << ngrads << endl;

}

bool
Region::extend_arcs_entering(string name, int k, int kmax, TRel &tr) {
    bool changes;
    do {
        changes = false;
        for(int i = 0; i <= power(); ++i) {
            bool viol = false;
            bool b = compute_maximum_lower_bound_entering(name,i,k,kmax,viol,tr);
            changes = changes or b;
            if (viol) return false;
        }
    } while (changes);

    // recompute the number of gradients
    ngrads = 0;
    all_grads_computed = false;
    map<string,EvTRel *>::iterator itb;
    for (itb = tr.eventTRs.begin(); itb != tr.eventTRs.end(); ++itb) {
        computed_grads[itb->first] = false;
        grads[itb->first].clear();
    }
// cout << "Recomputed number of gradients: " << ngrads << endl;
    return true;
}


bool
Region::proper(const SS &state_space) {
    return state_space.Intersect(R[0]).is_empty();
}

SS
Region::sup() {
    return not R[0];
}



SS
Region::project_er_positive_gradient(string trig_event, SS &er_event, TRel &tr) {

//	SS result = mgr->bddZero();
    SS result;
    for (int i = 1; i <= power(); ++i) {
        SS er_i = R[i] - er_event;
        if (er_i.is_empty()) continue;
        SS img_er_i = tr.image(trig_event,er_i) * er_event;
        if (not img_er_i.is_empty()) {
            SS x = tr.back_image(trig_event,img_er_i) * R[i];
            result += x;
        }
    }

    return result;
}


SS
Region::collect_arcs_leaving(string name, int k, TRel &tr) {
//	SS result = mgr->bddZero();
    SS result;
    for(int i = 0; i <= power(); ++i) {
        SS b = collect_maximum_lower_bound_leaving(name,i,k,tr);
        result += b;
    }
    return result;
}

SS
Region::collect_arcs_entering(string name, int k, TRel &tr) {
//	SS result = mgr->bddZero();
    SS result;
    for(int i = 0; i <= power(); ++i) {
        SS b = collect_maximum_lower_bound_entering(name,i,k,tr);
        result += b;
    }
    return result;
}


/*! ************** Private members *******************/

bool
Region::constant_gradient(string name, TRel &tr, int &grad) {

//cout << "computing constant gradient for " << name << endl;
    bool is_constant = true, first_grad = true;
    int i = 0;
    int glob_grad;

    do {
        map<int, SS> srv_i;
        int min_local_grad, max_local_grad;
        sr_i(name, i, tr,srv_i,min_local_grad,max_local_grad);

        is_constant = (min_local_grad == max_local_grad or min_local_grad == MAX_MULTIPLICITY)
                      and srv_i.size() <= 1;

        if (not is_constant) break;

//cout << " multiplicitat: "<<  local_grad << endl;
        if (first_grad and min_local_grad != MAX_MULTIPLICITY) {
            first_grad = false;
            glob_grad = max_local_grad;
        }
        else  is_constant = is_constant and
                                (max_local_grad == glob_grad or min_local_grad == MAX_MULTIPLICITY);
        ++i;
    } while (is_constant and i != power() + 1);

    grad = glob_grad;

    return is_constant;
}

SS
Region::er_i(string name, int i, TRel &tr) {

    EvTRel *etr = tr.get_event_tr(name);
    SS result = R[i] * etr->get_er();

    return result;
}

void
Region::sr_i(string name, int ii, TRel &tr, map<int, SS> &vresult, int &min, int &max) {

    max = 0;
    bool assigned_min = false;
    SS img_er_ii = tr.image(name,er_i(name,ii, tr));
    if (img_er_ii.is_empty()) {
        max = - MAX_MULTIPLICITY;
        min =	MAX_MULTIPLICITY;
    }
    else {
        for (int i = 0; i <= power(); ++i) {
            SS x =  img_er_ii * R[i];
            if (not x.is_empty()) {
                vresult[i] = x;
                max = i;
                if (not assigned_min) {
                    min = i;
                    assigned_min = true;
                }
            }
        }

        max -= ii;
        min -= ii;
    }
//cout << "sr_i for " << ii << " gives "<< min << " " << max << endl;
}

/* This member function traverses the region starting at multiplicity i,
 * and modifies those states s from R such that there is an arc to another state s'
 * such that R(s') - R(s) >  k. The region R will be changed to the new region R'
 * as follows: R'(s) = R(s') - k
 */
bool
Region::compute_maximum_lower_bound_leaving(string name,int multi, int k, int kmax, bool &viol, TRel &tr) {
    bool change = false;
    SS img_R = tr.image(name,R[multi]);
    for (int i = max_multiplicity; i >= MAX(0,multi + k) and i - multi > k; --i) {
        SS fwd_Ri =  img_R * R[i];
        if (fwd_Ri.is_empty()) continue;
        change = true;
        SS back_Ri = tr.back_image(name,fwd_Ri);
        R[multi] -= back_Ri;
        if (i - k > max_multiplicity) {
            if (i-k > kmax) {
                viol = true;
                return false;
            }
            R.resize(i-k+1,SS());
            R[i - k] = back_Ri;
            max_multiplicity = i - k;
        }
        else {
            R[i - k] += back_Ri;
        }
        // recomputing the affected gradients
//cout << "Recomputing grads leaving: deleting from " << i-multi << " and adding to " << k<<endl;
        /*		SS moving_tr = fwd_Ri;
        		if (grads[name].find(i - multi) != grads[name].end()) {
        			grads[name][i - multi] -= moving_tr;
        			if (grads[name][i - multi] == mgr.bddZero()) grads[name].erase(i - multi);
        		}
        		if (grads[name].find(k) == grads[name].end()) grads[name][k] = mgr.bddZero();
        		grads[name][k] += moving_tr;
        		*/
    }

    return change;
}

/* This member function traverses the region starting at multiplicity multi,
 * and modifies those states s from R such that there is an arc from another state s'
 * such that R(s') - R(s) >  k. The region R will be changed to the new region R'
 * as follows: R'(s) = R(s') + k
 */


bool
Region::compute_maximum_lower_bound_entering(string name,int multi, int k, int kmax,bool &viol, TRel &tr) {
    bool change = false;
    SS back_R = tr.back_image(name,R[multi]);
    for (int i = max_multiplicity; i >= MAX(0,multi - k) and multi - i < k; --i) {
        SS back_Ri =  back_R * R[i];
        if (back_Ri.is_empty()) continue;
        change = true;
        SS fwd_Ri = tr.image(name,back_Ri);
        R[multi] -= fwd_Ri;
        if (i + k > max_multiplicity) {
            if (i+k > kmax) {
                viol = true;
                return false;
            }
            R.resize(i+k+1,SS());
//			for(int l = max_multiplicity +1 ; l < i+k +1; ++l) R[l] = SS(0);
            R[i + k] = fwd_Ri;
            max_multiplicity = i + k;
        }
        else {
            R[i + k] += fwd_Ri;
        }
//cout << "Recomputing grads entering: deleting from " << multi -i << " and adding to " << k<<endl;
        // recomputing the affected gradients
        /*		SS moving_tr = back_Ri;
        		if (grads[name].find(multi - i) != grads[name].end()) {
        			//SS moving_tr = grads[name][multi - i] * back_Ri;
        			grads[name][multi - i] -= moving_tr;
        			if (grads[name][multi -i] == mgr.bddZero()) grads[name].erase(multi -i);
        		}
        		if (grads[name].find(k) == grads[name].end()) grads[name][k] = mgr.bddZero();
        		grads[name][k] += moving_tr;
        */
    }

    return change;
}



SS
Region::collect_maximum_lower_bound_entering(string name,int multi, int k, TRel &tr) {
//	SS result = mgr->bddZero();
    SS result;
    SS back_R = tr.back_image(name,R[multi]);
    for (int i = max_multiplicity; i >= MAX(0,multi - k) and multi - i < k; --i) {
        SS back_Ri =  back_R * R[i];
        if (back_Ri.is_empty()) continue;
        SS fwd_Ri = tr.image(name,back_Ri);
        result += fwd_Ri;
    }

    return result;
}

SS
Region::collect_maximum_lower_bound_leaving(string name,int multi, int k, TRel &tr) {
//	SS result = mgr->bddZero();
    SS result;
    SS img_R = tr.image(name,R[multi]);
    for (int i = max_multiplicity; i >= MAX(0,multi + k) and i - multi > k; --i) {
        SS fwd_Ri =  img_R * R[i];
        if (fwd_Ri.is_empty()) continue;
        SS back_Ri = tr.back_image(name,fwd_Ri);
        result += back_Ri;
    }

    return result;
}

// computing the multiset that represents the k-topset of the current region
Region
Region::get_k_topset(int k) {
    Region result(*this);
    for (int i = 1; i < k; ++i) {
        result.R[0] += result.R[i];
        result.R[i] = SS();
    }

    return result;
}

// the following member checks the mg condition on the actual multiset
bool
Region::is_MG_region(TRel &tr) {

    int n_enter = 0;
    int n_exit = 0;
    map<string,EvTRel *>::iterator itb;
    for (itb = tr.eventTRs.begin(); itb != tr.eventTRs.end(); ++itb) {
        map<int,SS>::const_iterator itg;
        for(itg=grads[itb->first].begin(); itg!=grads[itb->first].end(); ++itg) {
// 		 cout << " " << itg->first << endl;
            if (itg->first > 0) ++n_enter;
            if (itg->first < 0) ++n_exit;
            if (n_exit > 1 or n_enter > 1) return false;
        }
    }
    return true;
}

bool
Region::extend_on_MG_violation(int /*kmax*/, TRel &tr) {
    int n_enter = 0;
    int n_exit = 0;
    string event_violation = "";
    map<string,EvTRel *>::iterator itb;
    for (itb = tr.eventTRs.begin(); itb != tr.eventTRs.end(); ++itb) {
        map<int,SS>::const_iterator itg;
        for(itg=grads[itb->first].begin(); itg!=grads[itb->first].end(); ++itg) {
//  		 cout << " " << itg->first << endl;
            if (itg->first > 0) ++n_enter;
            if (itg->first < 0) ++n_exit;
            if (n_exit > 1 or n_enter > 1) {
                event_violation = itb->first;
                break;
            }
        }
//  	cout << "enter " << n_enter << " exit " << n_exit << endl;
        if (event_violation != "") break;
    }

//  cout << "gonna extend for " << event_violation << endl;
    if (n_exit > 1) {
        for (int i = max_multiplicity; i > 0; --i) {
            SS img_Ri = tr.image(event_violation,R[i]);
            for(int j = i - 1; j >= 0; --j) {
                SS move_Rj = img_Ri * R[j];
                if (not move_Rj.is_empty()) {
                    R[j] -= move_Rj;
                    R[i] += move_Rj;
                    return true;
                }
            }
        }
    }
    else {
        for (int i = 0; i < max_multiplicity; ++i) {
            SS img_Ri = tr.image(event_violation,R[i]);
            for(int j = i + 1; j <= max_multiplicity; ++j) {
                SS atRj = img_Ri * R[j];
                if (not atRj.is_empty()) {
                    SS move_Ri = tr.back_image(event_violation,atRj) * R[i];
                    R[j] += move_Ri;
                    R[i] -= move_Ri;
                    return true;
                }
            }
        }
    }
    return false;
}

void
Region::minimize(SS &state_space) {
    for (int k = power(); k >= 0; --k) {
        R[k] = R[k].Minimize(state_space);
    }
}

bool
Region::test_additive_union(const Region &other, int maxk) {
    bool result = (max_multiplicity <= maxk and other.max_multiplicity <= maxk);
    if (not result) return result;
    for (int j = other.max_multiplicity; j>0; --j) {
        // check whether the additive union does not go beyond bound maxk for elements in j
        for (int i = maxk -j + 1; i<=max_multiplicity; ++i) {
            if (not R[i].Intersect(other.R[j]).is_empty()) return false;
        }
    }
    return true;
}



bool
Region::additive_union(const Region &other, int maxk) {
    bool result = (max_multiplicity <= maxk and other.max_multiplicity <= maxk);
    if (not result) return result;
    for (int j = other.max_multiplicity; j>0; --j) {
        // check whether the additive union does not go beyond bound maxk for elements in j
        /*		for (int i = maxk -j + 1;i<=max_multiplicity;++i) {
        			if (not R[i].Intersect(other.R[j]).is_empty()) return false;
        		}*/
        // if the additive union is fine with the bound, then apply it for elements in j
        SS otherRj= other.R[j];
        for (int i = 1; i <=max_multiplicity and not otherRj.is_empty(); ++i) {
            if (not R[i].Intersect(otherRj).is_empty()) {
                SS sij = R[i] * otherRj;
                R[i] -= sij;
                otherRj -= sij;
                if (i + j > max_multiplicity) {
                    R.resize(i+j+1,SS());
                    R[i + j] = sij;
                    max_multiplicity = i + j;
                }
                else	R[i+j] = sij;
            }
        }
        if (not otherRj.is_empty()) {
            if (j > max_multiplicity) {
                R.resize(j+1,SS());
                max_multiplicity = j;
            }
            R[j] = otherRj;
            max_multiplicity = MAX(j,max_multiplicity);
        }
    }
    return true;
}

void
Region::additive_substraction(const Region &other) {

    for (int j = other.max_multiplicity; j>0; --j) {
        // if the additive union is fine with the bound, then apply it for elements in j
        SS otherRj= other.R[j];
        for (int i = 1; i <=max_multiplicity and not otherRj.is_empty(); ++i) {
            if (not R[i].Intersect(otherRj).is_empty()) {
                SS sij = R[i] * otherRj;
                R[i] -= sij;
                otherRj -= sij;
                if (i - j >= 0) {
                    R[i-j] = sij;
                }
                else	R[0] = sij;
            }
        }
    }
}

