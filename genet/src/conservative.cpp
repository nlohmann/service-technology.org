#include <iostream>
#include <sstream>
#include <set>
#include <queue> // priority_queue
#include <ext/hash_map>
#include "TRel.h"
#include "Region.h"
#include "HashRegions.h"

extern bool essential_arc(const list<Region *> &preregs, map<Region *,bool> &arcs_red,Region *r,EvTRel *evtr);
extern list<Region *> compute_preregions(const list<Region *> &minregs,TRel &tr, const SS &er);

void print_cc(const list<Region *> &minregs,TRel &tr, const SS &initial, int KMAX,const set<string> &localEvs);
list<Region *> find_conservative_component(EvTRel *tr_ev,TRel &tr, int kmax, const SS &state_space,set<string> &localEvs, list<Region *> &minregs,
        set<string> &problem_events);
list<Region *> find_sm(EvTRel *tr_ev,TRel &tr, int kmax, const SS &state_space,set<string> &localEvs, list<Region *> &minregs,
                       set<string> &problem_events);
Region *find_sm_region(TRel &tr,const SS &er, const SS &partition, const SS &state_space,int kmax, list<Region *> &locregs, list<Region *> &minregs);
Region *find_cc_region(string event,TRel &tr,const SS &er, Region &partition, const SS &state_space,int kmax,
                       list<Region *> &locregs, list<Region *> &minregs,map<string,int> &flow_map, bool accept_existing=true);
Region *find_cc_newregion(string event,TRel &tr,const SS &er, Region &partition, const SS &state_space,int kmax,
                          list<Region *> &locregs, list<Region *> &minregs,map<string,int> &flow_map);

// the following two methods should be changed to incorporate the topset information, instead of the unitary inc/decrement
void update_covering_region(Region *r, map<string,EvTRel *> &tr_map,map<string,int> &flow_map,set<string> localEvs) {
    set<string>::const_iterator it;
    SS rsup = r->sup();
    for(it=localEvs.begin(); it!=localEvs.end(); ++it) {
        if (tr_map[*it]->get_er() <= rsup) {
            flow_map[*it]--;
        }
        if (tr_map[*it]->get_sr() <= rsup) {
            flow_map[*it]++;
        }
    }
}

void update_covering_event(string event, map<string,EvTRel *> &tr_map,map<string,int> &flow_map,list<Region *> &locregs) {
    list<Region *>::const_iterator it;
    SS er = tr_map[event]->get_er();
    SS sr = tr_map[event]->get_sr();
    for(it=locregs.begin(); it!=locregs.end(); ++it) {
        if (er <= (*it)->sup()) {
            flow_map[event]--;
        }
        if (sr <= (*it)->sup()) {
            flow_map[event]++;
        }
    }
}

void remove_covering_region(string /*event*/, map<string,EvTRel *> &tr_map, const SS &sr, Region &partition, const SS &/*state_space*/,
                            list<Region *> &localRegs,set<string> localEvs,map<string,int> &flow_map) {
    list<Region *>::iterator it;
    for(it=localRegs.begin(); it!=localRegs.end(); ++it) {
        if (sr <= (*it)->sup()) {
            break;
        }
    }
    if (it != localRegs.end()) {
        SS rsup = (*it)->sup();
        partition.additive_substraction(**it);
        localRegs.erase(it);
        set<string>::const_iterator ite;
        for(ite=localEvs.begin(); ite!=localEvs.end(); ++ite) {
            if (tr_map[*ite]->get_er() <= rsup) {
                flow_map[*ite]++;
            }
            if (tr_map[*ite]->get_sr() <= rsup) {
                flow_map[*ite]--;
            }
        }
    }
}

bool generate_conservative_cover(TRel &tr, int kmax, const SS &state_space, const SS &initial_state, list<Region *> &minregs) {

    set<string> Evs, problem_events;
    map<string,EvTRel *> tr_map = tr.get_map_trs();
    map<string,EvTRel *>::const_iterator itm;
    for(itm=tr_map.begin(); itm != tr_map.end(); ++itm) {
        Evs.insert(itm->first);
    }

    vector<list<Region *> > CCs(10);
    int i = 0;
    do {
        string cover_event = *(Evs.begin());
        set<string> localEvs;
        localEvs.insert(cover_event);
//		cout << "event a cobrir: " << cover_event << endl;
        CCs[i] = find_conservative_component(tr_map[cover_event],tr,kmax ,state_space,localEvs,minregs,problem_events);
//		cout << "Found a conservative component with " << CCs[i].size() << " places" << endl;
        if (not CCs[i].empty())	{
            cout << endl << endl << "# Conservative component " << i << " --------------------"<< endl;
            print_cc(CCs[i],tr,initial_state,kmax,localEvs);
            ++i;
        }
        set<string> tmpEvs;
        set_union(localEvs.begin(),localEvs.end(),problem_events.begin(),problem_events.end(),inserter(localEvs, localEvs.begin()));
        set_difference(Evs.begin(),Evs.end(),localEvs.begin(),localEvs.end(),inserter(tmpEvs, tmpEvs.begin()));
        Evs = tmpEvs;

        if (i>=10) CCs.resize(CCs.size() + 10);

    } while (not Evs.empty());
    if (not problem_events.empty()) {
        list<Region *> cc_problem;
        print_cc(cc_problem,tr,initial_state,kmax,problem_events);
    }
    return true;
}


list<Region *>
find_conservative_component(EvTRel *tr_ev,TRel &tr, int kmax, const SS &state_space,set<string> &localEvs, list<Region *> &minregs,
                            set<string> &problem_events) {
    map<string,bool> event_covered_cc;
    map<string,EvTRel *> tr_map = tr.get_map_trs();
    map<string,int> flow_map;
    map<string,EvTRel *>::const_iterator itm;
    for(itm=tr_map.begin(); itm != tr_map.end(); ++itm) {
        event_covered_cc[itm->first] = false;
        flow_map[itm->first] = 0;
    }

    list<Region *> localRegs;
    list<Region *> Pending;
    Region partition(SS(0));
    // covering the pre-set
    Region *r = find_cc_region(*(localEvs.begin()),tr,tr_ev->get_er(),partition,state_space,kmax,localRegs,minregs,flow_map);
    if (r == NULL) {
        /*		cout << "PROBLEM: couldnt find a region covering event " << *(localEvs.begin()) << endl;
        		exit(1);*/
        problem_events.insert(*(localEvs.begin()));
        return localRegs;
    }
    event_covered_cc[*(localEvs.begin())]=true;
    Pending.push_back(r);
    localRegs.push_back(r);

    partition.additive_union(*r,kmax);

    // covering the post-set
    r = find_cc_region(*(localEvs.begin()),tr,tr_ev->get_sr(),partition,state_space,kmax,localRegs,minregs,flow_map);
    if (r != NULL) {
        Pending.push_back(r);
        localRegs.push_back(r);
        partition.additive_union(*r,kmax);
    }
    update_covering_event(*(localEvs.begin()),tr_map,flow_map,localRegs);

    while (not Pending.empty()) {
        r = Pending.front();
        Pending.pop_front();
        SS rsup = r->sup();
        for(itm=tr_map.begin(); itm != tr_map.end(); ++itm) {
            bool event_involved = false;
            if (itm->second->get_er() <= rsup or itm->second->get_sr() <= rsup /*(not rsup.Intersect(itm->second->get_sr()).is_empty())*/) {
                if (not event_covered_cc[itm->first]) {
//					cout << "    event involved:" << itm->first <<endl;
                    event_involved = true;
                    localEvs.insert(itm->first);
                    update_covering_event(itm->first, tr_map,flow_map,localRegs);
                    r = find_cc_region(itm->first,tr,itm->second->get_er(),partition,state_space,kmax,localRegs,minregs,flow_map);
                    if (r != NULL) {
                        event_covered_cc[itm->first]=true;
                        Pending.push_back(r);
                        localRegs.push_back(r);
                        partition.additive_union(*r,kmax);
                        update_covering_region(r,tr_map,flow_map,localEvs);
                        //cout << "  new pre-region found\n";
                    }

                    r = find_cc_region(itm->first,tr,itm->second->get_sr(),partition,state_space,kmax,localRegs,minregs,flow_map);
                    if (r != NULL) {
                        event_covered_cc[itm->first]=true;
                        Pending.push_back(r);
                        localRegs.push_back(r);
                        partition.additive_union(*r,kmax);
                        update_covering_region(r,tr_map,flow_map,localEvs);
                        //cout << "  new post-region found\n";
                    }

                    // now forces the conservative condition to hold on the event
                    bool progress = true;
                    while (flow_map[itm->first]!= 0 and progress) {
                        cout << " Trying to add regions to balance " << itm->first << endl;
                        progress = false;
                        if (flow_map[itm->first] > 0) {
                            r = find_cc_newregion(itm->first,tr,itm->second->get_er(),partition,state_space,kmax,localRegs,minregs,flow_map);
                            if (r != NULL) {
//								cout << "   adding a preregion\n";
                                Pending.push_back(r);
                                localRegs.push_back(r);
                                partition.additive_union(*r,kmax);
                                update_covering_region(r,tr_map,flow_map,localEvs);
                                progress = true;
                            }
                        }
                        else {
                            r = find_cc_newregion(itm->first,tr,itm->second->get_sr(),partition,state_space,kmax,localRegs,minregs,flow_map);
                            if (r != NULL) {
//								cout << "   adding a postregion\n";
                                Pending.push_back(r);
                                localRegs.push_back(r);
                                partition.additive_union(*r,kmax);
                                update_covering_region(r,tr_map,flow_map,localEvs);
                                progress = true;
                            }
                        }
                    }

                    // if no addition can be done, then iteratively remove regions to force conservativeness
                    while (flow_map[itm->first]!= 0 ) {
                        if (flow_map[itm->first] > 0) {
//							cout << "Removing a post covering for " << itm->first << endl;
                            remove_covering_region(itm->first,tr_map,itm->second->get_sr(),partition,state_space,localRegs,localEvs,flow_map);
                        }
                        else {
//							cout << "Removing a pre covering for " << itm->first << endl;
                            remove_covering_region(itm->first,tr_map,itm->second->get_er(),partition,state_space,localRegs,localEvs,flow_map);
                        }
                    }
//					if (partition >= state_space) break;
                }
            }
//			if (partition >= state_space) break;
        }
    }

    for(itm=tr_map.begin(); itm != tr_map.end(); ++itm) {

//		if (flow_map[itm->first] != 0) cout << "Event " << itm->first << " unbalanced: " << flow_map[itm->first]<< "\n";

        // now forces the conservative condition to hold on the event
        bool progress = true;
        while (flow_map[itm->first]!= 0 and progress) {
            cout << " Trying to add regions to balance " << itm->first << endl;
            progress = false;
            if (flow_map[itm->first] > 0) {
                r = find_cc_newregion(itm->first,tr,itm->second->get_er(),partition,state_space,kmax,localRegs,minregs,flow_map);
                if (r != NULL) {
//					cout << "   adding a preregion\n";
                    Pending.push_back(r);
                    localRegs.push_back(r);
                    partition.additive_union(*r,kmax);
                    update_covering_region(r,tr_map,flow_map,localEvs);
                    progress = true;
                }
            }
            else {
                r = find_cc_newregion(itm->first,tr,itm->second->get_sr(),partition,state_space,kmax,localRegs,minregs,flow_map);
                if (r != NULL) {
//					cout << "   adding a postregion\n";
                    Pending.push_back(r);
                    localRegs.push_back(r);
                    partition.additive_union(*r,kmax);
                    update_covering_region(r,tr_map,flow_map,localEvs);
                    progress = true;
                }
            }
        }

        // if no addition can be done, then iteratively remove regions to force conservativeness
        while (flow_map[itm->first]!= 0 ) {
            if (flow_map[itm->first] > 0) {
//				cout << "Removing a post covering for " << itm->first << endl;
                remove_covering_region(itm->first,tr_map,itm->second->get_sr(),partition,state_space,localRegs,localEvs,flow_map);
            }
            else {
//				cout << "Removing a pre covering for " << itm->first << endl;
                remove_covering_region(itm->first,tr_map,itm->second->get_er(),partition,state_space,localRegs,localEvs,flow_map);
            }
        }
    }


    //cout << "Max_multiplicity in partition: " << partition.power() << endl;
//	if (partition.get_i(kmax) < state_space) cout << "Problem: couldnt cover the whole state space with " << kmax << " bounded regions\n";
//	cout << "nombre de regions trobades: " << localRegs.size()<< "\n";
    return localRegs;
}

Region *
find_cc_region(string /*event*/,TRel &tr,const SS &cover, Region &partition, const SS &state_space,int kmax,
               list<Region *> &locregs, list<Region *> &minregs,map<string,int> &/*flow_map*/, bool accept_existing) {
//	if (not er.Intersect(partition).is_empty()) return NULL;

    if (not partition.test_additive_union(Region(cover),kmax)) return NULL;

    //check whether a precomputed region exists in the component
    if (accept_existing) {
        list<Region *>::const_iterator it;
        for(it=locregs.begin(); it !=locregs.end(); ++it) {
            Region *r = *it;
            if (r->sup() >= cover and partition.test_additive_union(*r,kmax)) {
                return NULL;
            }
        }
    }

    //check whether a precomputed region can be used
    list<Region *>::const_iterator it;
    for(it=minregs.begin(); it !=minregs.end(); ++it) {
        Region *r = *it;
        if (r->sup() >= cover and partition.test_additive_union(*r,kmax)) {
            return r;
        }
    }

    queue<Region *> P;
    P.push(new Region(cover));
    Region *result = NULL;
    hash_map<Region*, bool, hashRegion, KeysReg> cache_generate_local;
    while(not P.empty()) {
        Region *r = P.front();
        P.pop();

        string violating_event = r->choose_event_non_constant_gradient(tr);
        if (violating_event  != "") {

            int gmin, gmax;
            r->corner_gradients(violating_event,tr,gmin,gmax);
            int g = int(floor(double(gmin + gmax) / 2.0));

            Region *r1 = new Region(*r);
            //cout << "--  Extending arcs leaving for " << violating_event << " with g= " << g << endl;
            bool b = r1->extend_arcs_leaving(violating_event,g,kmax,tr);
            if (not b) delete r1;
            else {
                if (b and  (cache_generate_local.find(r1) == cache_generate_local.end()) and partition.test_additive_union(*r1,kmax)
                        and not(r1->proper(state_space))) {
                    cache_generate_local[r1] = true;
                    if (r1->is_region(tr,false)) {
                        minregs.push_back(r1);
                        result = r1;//cout << "   1.regio\n";r1->print();
                        break;
                    }
                    else {
                        P.push(r1);
                    }
                }
                else delete r1;
            }

            Region *r2 = new Region(*r);
            //cout << "--  Extending arcs entering for " << violating_event << " with g= " << g +1 << endl;
            b = r2->extend_arcs_entering(violating_event,g+1,kmax,tr);
            if (not b) delete r2;
            else {
                if (b and (cache_generate_local.find(r2) == cache_generate_local.end()) and partition.test_additive_union(*r2,kmax)
                        and not(r2->proper(state_space)) ) {
                    cache_generate_local[r2] = true;
                    if (r2->is_region(tr,false)) {
                        minregs.push_back(r2);
                        result = r2; //cout << "   2.regio\n";r2->print();
                        break;
                    }
                    else {
                        P.push(r2);
                    }
                }
                else delete r2;
            }
        }
        else {
            minregs.push_back(r);
            result = r;
            break;
        }
    }

    while (not P.empty()) {
        Region *r = P.front();
        P.pop();
        delete r;
    }

    return result;
}

Region *
find_cc_newregion(string /*event*/,TRel &tr,const SS &cover, Region &partition, const SS &state_space,int kmax,
                  list<Region *> &locregs, list<Region *> &minregs,map<string,int> &/*flow_map*/) {
//	assert(er.Intersect(partition).is_empty());
//	if (not sr.Intersect(partition).is_empty()) return NULL;
    if (not partition.test_additive_union(Region(cover),kmax)) return NULL;


    //check whether a precomputed region can be used
    list<Region *>::const_iterator it;
    for(it=minregs.begin(); it !=minregs.end(); ++it) {
        Region *r = *it;
        if (r->sup() >= cover and partition.test_additive_union(*r,kmax)) {
            list<Region *>::const_iterator it;
            bool exist = false;
            for(it=locregs.begin(); it!=locregs.end() and not exist; ++it) {
                if ((**it) == (*r)) exist=true;
            }
            if (not exist) return r;
        }
    }


    queue<Region *> P;
    P.push(new Region(cover));
    Region *result = NULL;
    hash_map<Region*, bool, hashRegion, KeysReg> cache_generate_local;
    while(not P.empty()) {
        Region *r = P.front();
        P.pop();

        string violating_event = r->choose_event_non_constant_gradient(tr);
        if (violating_event  != "") {

            int gmin, gmax;
            r->corner_gradients(violating_event,tr,gmin,gmax);
            int g = int(floor(double(gmin + gmax) / 2.0));

            Region *r1 = new Region(*r);
            //cout << "--  Extending arcs leaving for " << violating_event << " with g= " << g << endl;
            bool b = r1->extend_arcs_leaving(violating_event,g,kmax,tr);
            if (not b) delete r1;
            else {
                if (b and  (cache_generate_local.find(r1) == cache_generate_local.end()) and partition.test_additive_union(*r1,kmax)
                        and not(r1->proper(state_space))) {
                    cache_generate_local[r1] = true;
                    if (r1->is_region(tr,false)) {
                        bool exist = false;
                        list<Region *>::const_iterator it;
                        for(it=locregs.begin(); it!=locregs.end() and not exist; ++it) {
                            if ((**it) == (*r1)) exist=true;
                        }
                        if (not exist) {
                            minregs.push_back(r1);
                            result = r1;//cout << "   1.regio\n";r1->print();
                            break;
                        }
                    }
                    else {
                        P.push(r1);
                    }
                }
                else delete r1;
            }

            Region *r2 = new Region(*r);
            //cout << "--  Extending arcs entering for " << violating_event << " with g= " << g +1 << endl;
            b = r2->extend_arcs_entering(violating_event,g+1,kmax,tr);
            if (not b) delete r2;
            else {
                if (b and (cache_generate_local.find(r2) == cache_generate_local.end()) and partition.test_additive_union(*r2,kmax)
                        and not(r2->proper(state_space))) {
                    cache_generate_local[r2] = true;
                    if (r2->is_region(tr,false)) {
                        bool exist = false;
                        list<Region *>::const_iterator it;
                        for(it=locregs.begin(); it!=locregs.end() and not exist; ++it) {
                            if ((**it) == (*r2)) exist=true;
                        }
                        if (not exist) {
                            minregs.push_back(r2);
                            result = r2; //cout << "   2.regio\n";r2->print();
                            break;
                        }
                    }
                    else {
                        P.push(r2);
                    }
                }
                else delete r2;
            }
        }
        else {
//			cout << "3.regio\n";r->print();
            minregs.push_back(r);
            result = r;
            break;
        }
    }

    while (not P.empty()) {
        Region *r = P.front();
        P.pop();
        delete r;
    }

    return result;
}


bool generate_conservative_sm_cover(TRel &tr, int /*kmax*/, const SS &state_space, const SS &initial_state, list<Region *> &minregs) {

    set<string> Evs, problem_events;
    map<string,EvTRel *> tr_map = tr.get_map_trs();
    map<string,EvTRel *>::const_iterator itm;
    for(itm=tr_map.begin(); itm != tr_map.end(); ++itm) {
        Evs.insert(itm->first);
    }

    vector<list<Region *> > CCs(10);
    int i = 0;
    do {
        string cover_event = *(Evs.begin());
        set<string> localEvs;
        localEvs.insert(cover_event);
        //cout << "event a cobrir: " << cover_event << endl;
        CCs[i] = find_sm(tr_map[cover_event],tr,/* kmax */1,state_space,localEvs,minregs,problem_events);
//		cout << "Found a conservative component with " << CCs[i-1].size() << " places" << endl;
        if (not CCs[i].empty())	{
            cout << endl << endl << "# State machine " << i << " --------------------"<< endl;
            print_cc(CCs[i],tr,initial_state,1,localEvs);
            ++i;
        }
        set<string> tmpEvs;
        set_union(localEvs.begin(),localEvs.end(),problem_events.begin(),problem_events.end(),inserter(localEvs, localEvs.begin()));
        set_difference(Evs.begin(),Evs.end(),localEvs.begin(),localEvs.end(),inserter(tmpEvs, tmpEvs.begin()));
        Evs = tmpEvs;

        if (i>=10) CCs.resize(CCs.size() + 10);

    } while (not Evs.empty());
    if (not problem_events.empty()) {
        list<Region *> cc_problem;
        print_cc(cc_problem,tr,initial_state,1,problem_events);
    }
    return true;
}



list<Region *>
find_sm(EvTRel *tr_ev,TRel &tr, int /*kmax*/, const SS &state_space,set<string> &localEvs, list<Region *> &minregs,
        set<string> &problem_events) {
    map<string,bool> event_covered_cc;
    map<string,EvTRel *> tr_map = tr.get_map_trs();
    map<string,EvTRel *>::const_iterator itm;
    for(itm=tr_map.begin(); itm != tr_map.end(); ++itm) {
        event_covered_cc[itm->first] = false;
    }

    list<Region *> localRegs;
    list<Region *> Pending;
    SS partition = SS(0);

    // covering the pre-set
    Region *r = find_sm_region(tr,tr_ev->get_er(),partition,state_space,/*kmax*/1,localRegs,minregs);
    if (r == NULL) {
        /*		cout << "PROBLEM: couldnt find a region covering event " << *(localEvs.begin()) << endl;
        		exit(1);*/
        problem_events.insert(*(localEvs.begin()));
        return localRegs;
    }
    event_covered_cc[*(localEvs.begin())]=true;
    Pending.push_back(r);
    localRegs.push_back(r);
    partition += r->sup();

    // covering the post-set
    r = find_sm_region(tr,tr_ev->get_sr(),partition,state_space,/*kmax*/1,localRegs,minregs);
    if (r != NULL) {
        Pending.push_back(r);
        localRegs.push_back(r);
        partition += r->sup();
    }

    while (not Pending.empty()) {
        r = Pending.front();
        Pending.pop_front();
        SS rsup = r->sup();
        for(itm=tr_map.begin(); itm != tr_map.end(); ++itm) {
            bool event_involved = false;
            if (itm->second->get_er() <= rsup or itm->second->get_sr() <= rsup /*(not rsup.Intersect(itm->second->get_sr()).is_empty())*/) {
                if (not event_covered_cc[itm->first]) {
                    event_involved = true;
                    localEvs.insert(itm->first);
                    r = find_sm_region(tr,itm->second->get_er(),partition,state_space,/*kmax*/1,localRegs,minregs);
                    if (r != NULL) {
                        event_covered_cc[itm->first]=true;
                        Pending.push_back(r);
                        localRegs.push_back(r);
                        partition += r->sup();
                    }

                    r = find_sm_region(tr,itm->second->get_sr(),partition,state_space,/*kmax*/1,localRegs,minregs);
                    if (r != NULL) {
                        event_covered_cc[itm->first]=true;
                        Pending.push_back(r);
                        localRegs.push_back(r);
                        partition += r->sup();
                    }
                    /*					if (r == NULL) {
                    							problem_events.insert(itm->first);
                    							return localRegs;
                    					}*/
//					if (partition >= state_space) break;
                }
            }
//			if (partition >= state_space) break;
        }
    }
//	cout << "nombre de regions trobades: " << localRegs.size()<< "\n";
    return localRegs;
}

Region *
find_sm_region(TRel &tr,const SS &sr, const SS &partition, const SS &state_space,int kmax, list<Region *> &locregs, list<Region *> &minregs) {
//	assert(er.Intersect(partition).is_empty());


    if (not sr.Intersect(partition).is_empty()) return NULL;

    //check whether a precomputed region exists in the component
    list<Region *>::const_iterator it;
    for(it=locregs.begin(); it !=locregs.end(); ++it) {
        Region *r = *it;
        if (r->sup() >= sr and partition.Intersect(r->sup()).is_empty()) return NULL;
    }

    //check whether a precomputed region can be used
    for(it=minregs.begin(); it !=minregs.end(); ++it) {
        Region *r = *it;
        if (r->sup() >= sr and partition.Intersect(r->sup()).is_empty()) return r;
    }

    queue<Region *> P;
    P.push(new Region(sr));
    Region *result = NULL;
    hash_map<Region*, bool, hashRegion, KeysReg> cache_generate_local;
    while(not P.empty()) {
        Region *r = P.front();
        P.pop();

        string violating_event = r->choose_event_non_constant_gradient(tr);
        if (violating_event  != "") {

            int gmin, gmax;
            r->corner_gradients(violating_event,tr,gmin,gmax);
            int g = int(floor(double(gmin + gmax) / 2.0));

            Region *r1 = new Region(*r);
            //cout << "--  Extending arcs leaving for " << violating_event << " with g= " << g << endl;
            bool b = r1->extend_arcs_leaving(violating_event,g,kmax,tr);
            if (not b) delete r1;
            else {
                if (b and  (cache_generate_local.find(r1) == cache_generate_local.end()) and partition.Intersect(r1->sup()).is_empty()
                        and not(r1->proper(state_space))) {
                    cache_generate_local[r1] = true;
                    if (r1->is_region(tr,false)) {
                        minregs.push_back(r1);
                        result = r1;//cout << "   1.regio\n";r1->print();
                        break;

                        /*					 minregs.push_back(r1);
                        					 update_topset_information(tr,tr_map,r1,tmp,"");
                        					 cache_regions[r1] = r1;
                        */
                        //updating the information on the ec events;
                        /*					 map<string,bool>::iterator ite;
                        	  			 for(ite = events_ec.begin(); ite != events_ec.end(); ++ite) {
                        	  			   SS erev = tr_map[ite->first]->get_er();
                        	  				 if ((not ite->second) and rsup >= erev)  {
                        	  					 ite->second = is_excitation_closed_event(minregs, ite->first,erev,tr, states_out_er[ite->first]);
                        	  				 }
                        	  			 }
                        */
                    }
                    else {
                        P.push(r1);
                    }
                }
                else delete r1;
            }

            Region *r2 = new Region(*r);
            //cout << "--  Extending arcs entering for " << violating_event << " with g= " << g +1 << endl;
            b = r2->extend_arcs_entering(violating_event,g+1,kmax,tr);
            if (not b) delete r2;
            else {
                if (b and (cache_generate_local.find(r2) == cache_generate_local.end()) and partition.Intersect(r2->sup()).is_empty()
                        and not(r2->proper(state_space))) {
                    cache_generate_local[r2] = true;
                    if (r2->is_region(tr,false)) {
                        minregs.push_back(r2);
                        result = r2; //cout << "   2.regio\n";r2->print();
                        break;

                        /*						minregs.push_back(r2);
                        						update_topset_information(tr,tr_map,r2,tmp,"");
                        						cache_regions[r2] = r2;
                        */
                        //updating the information on the ec events;
                        /*					  map<string,bool>::iterator ite;
                        	  				for(ite = events_ec.begin(); ite != events_ec.end(); ++ite) {
                        	  					SS erev = tr_map[ite->first]->get_er();
                        	  					if ((not ite->second) and rsup >= erev)  {
                        	  						ite->second = is_excitation_closed_event(minregs, ite->first,erev,tr, states_out_er[ite->first]);
                        	  					}
                        	  				}
                        */
                    }
                    else {
                        P.push(r2);
                    }
                }
                else delete r2;
            }
        }
        else {
            //cout << "			3.regio\n";r->print();
            minregs.push_back(r);
            result = r;
            break;

            /*				minregs.push_back(r);
            				update_topset_information(tr,tr_map,r,tmp,"");
            				cache_regions[r] = r;*/

            //updating the information on the ec events;
            /*			  map<string,bool>::iterator ite;
            	  		for(ite = events_ec.begin(); ite != events_ec.end(); ++ite) {
            	  			SS erev = tr_map[ite->first]->get_er();
            	  			if ((not ite->second) and rsup >= erev)  {
            	  				ite->second = is_excitation_closed_event(minregs, ite->first,erev,tr, states_out_er[ite->first]);
            	  			}
            	  		}
            	  						*/
        }
    }

    while (not P.empty()) {
        Region *r = P.front();
        P.pop();
        delete r;
    }

    return result;
}

void print_cc(const list<Region *> &locregs,TRel &tr, const SS &initial, int KMAX,const set<string> &localEvs) {
    int pnumber = 0;
    map<Region *, string> places;
    map<string,int> marking;
    map<string,EvTRel *> events_tr = tr.get_map_trs();
    cout << "# number of necessary regions: " << locregs.size() << endl;
    cout << ".inputs";
    map<string,EvTRel *>::iterator ittr;
    for(ittr = events_tr.begin(); ittr != events_tr.end(); ++ittr) if (localEvs.find(ittr->first) != localEvs.end())cout << " " << ittr->first;
    cout << endl << ".graph" << endl;

//	bool notsafe = false;
    list<Region *>::const_iterator itmr;
    for(itmr = locregs.begin(); itmr != locregs.end(); ++itmr) {
//(*itmr)->print();
        std::ostringstream o;
        if (!(o << pnumber++)) cout << "Problems when converting an integer to a string\n";
        places[*itmr] = "p" + o.str();
        if ((*itmr)->cardinality(initial)) {
            marking[places[*itmr]] = (*itmr)->cardinality(initial);
//			if (marking[places[*itmr]] > 1) notsafe = true;
        }
    }

    // detecting redundant arcs
    map <EvTRel *, map<Region *,bool> > selfloop_red;
    // arcs from places to transitions
    map<string,map<string,bool> > eventtoplaces;
    for(ittr = events_tr.begin(); ittr != events_tr.end(); ++ittr) {

        if (localEvs.find(ittr->first) == localEvs.end()) continue;

        string event = ittr->first;
        SS er_event = tr.get_event_tr(event)->get_er();
        list<Region *> pre_regions = compute_preregions(locregs,tr,er_event);
        bool pre_arc_inserted = false;
        for(itmr = pre_regions.begin(); itmr != pre_regions.end(); ++itmr) {
            if (not essential_arc(pre_regions,selfloop_red[tr.get_event_tr(event)],*itmr,tr.get_event_tr(event))) {
                //				cout << "event " << event << " is not connected to " << places[*itmr] <<  " " << pre_regions.size() <<endl;
            }
            else {
                pre_arc_inserted = true;
                Region *r = *itmr;
                Region max_topset = r->compute_max_enabling_topset(er_event);
                int k = max_topset.topset_weight();
// 			cout << "arc " << places[r] << " -- " << k << " -->" << event << endl;
                cout << places[r] << " " << event;
                if (k>1) cout << "(" << k << ") ";
                cout << endl;
                int k1,k2;
                r->corner_gradients(event,tr, k1, k2);
                if (k1 != k2) r->print();
                assert(k1==k2);
                if (k1 > -k) {
//	cout << "# arc(II) " << event <<  " k: " << k << " k1: " << k1  << " -->" << places[r]  << endl;
//  r->print();
                    eventtoplaces[event][places[r]] = true;
                    cout << event << " " << places[r];
                    if (k+k1>1) cout << "(" << k+k1 << ") ";
                    cout << endl;
                }
            }
        }
        // events without pre-regions: this is an option for pn mining
        if (not pre_arc_inserted and (not er_event.is_empty())) cout << event << endl;
    }

    // arcs from transitions to places
    for(ittr = events_tr.begin(); ittr != events_tr.end(); ++ittr) {

        if (localEvs.find(ittr->first) == localEvs.end()) continue;

        string event = ittr->first;
        SS er_event = tr.get_event_tr(event)->get_er();
        SS sr_event = tr.get_event_tr(event)->get_sr();
        for(itmr = locregs.begin(); itmr != locregs.end(); ++itmr) {
            Region *r = *itmr;
            if (sr_event <= r->sup() and not eventtoplaces[event][places[r]]/*and er_event * r->sup() == mgr.bddZero()*/) {
                int k1,k2;
//				r->recheck_gradients(tr,false);
                r->corner_gradients(event,tr, k1, k2);
                assert(k1 == k2);
                if (k1>0) {
                    //cout << "arc " << event <<  " -- " << k1 << " -->" << places[r]  << endl;
                    cout << event << " " << places[r];
                    if (k1>1) cout << "(" << k1 << ") ";
                    cout << endl;
                }
            }
        }
    }

    if (KMAX > 1) {

        cout << ".capacity ";
        list<Region *>::const_iterator itmr;
        for(itmr = locregs.begin(); itmr != locregs.end(); ++itmr) {
            cout << " " << places[*itmr] << "=" << KMAX;
        }
        cout <<  endl;
    }

    cout << ".marking {";
    map<string,int>::const_iterator itmark;
    for(itmark = marking.begin(); itmark != marking.end(); ++itmark) {
        if (itmark->second > 1) cout << " " << itmark->first << "=" << itmark->second;
        else cout << " " << itmark->first;
    }
    cout << " }\n" << ".end" << endl;

}



