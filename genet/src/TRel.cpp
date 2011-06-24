#include "TRel.h"



TRel::TRel(Encoding *e,Tts *ts/*, const Cudd &manager*/)
{
    enc= e;
    mts = ts;
//	mgr = manager;

}

map<string,bool>
TRel::tokenize(const string prj_sigs, const string pattern)
{
    map<string,bool> result;
    size_t n = prj_sigs.length();

    int pos = 0;
    do
    {
        size_t pos_final = min(n,prj_sigs.find(pattern,pos));
        string s;
        s.assign(prj_sigs, pos, pos_final - pos);
        result[s] = true;
        pos = pos_final + 1;
    }
    while (pos <= ((int) n - 1));

    return result;

}

void
TRel::build_tr(map<string,bool> &visible_events)
{
    map<string, list<string> > state_classes;
    if (not visible_events.empty())
    {
        list<Ttrans>::iterator it;
        for(it = mts->ltrans.begin(); it != mts->ltrans.end(); ++it)
        {
            string s = (*it).event;
            string source = (*it).source;
            string target = (*it).target;
            string minlex = (source < target ? source : target);
            string maxlex = (source > target ? source : target);
            if (visible_events.find(s) == visible_events.end() or (not visible_events[s]))
            {
                int n = state_classes[minlex].size();
                if (n == 0)
                {
                    state_classes[minlex].push_back(minlex);
                }
                else if (n == 1)   // the real minimal is another state!
                {
                    minlex = state_classes[minlex].front();
                }
                int nn = state_classes[maxlex].size();
                if (nn == 0)
                {
                    state_classes[minlex].push_back(maxlex);
                    state_classes[maxlex].push_back(minlex);
                }
                else if (nn >= 1)
                {
                    if (nn == 1)
                    {
                        string cpmaxlex = maxlex;
                        string othermin = state_classes[maxlex].front();
                        maxlex = (minlex >  othermin ? minlex : othermin);
                        minlex = (minlex <  othermin ? minlex : othermin);
                        state_classes[cpmaxlex].clear();
                        state_classes[cpmaxlex].push_back(minlex);
                    }
                    if (maxlex != minlex)
                    {
                        list<string>::const_iterator itm;
                        for(itm = state_classes[maxlex].begin(); itm != state_classes[maxlex].end(); ++itm)
                        {
                            if (maxlex != minlex)	state_classes[minlex].push_back(*itm);
                            if (*itm != maxlex)
                            {
                                state_classes[*itm].clear();
                                state_classes[*itm].push_back(minlex);
                            }
                        }
                        state_classes[maxlex].clear();
                        state_classes[maxlex].push_back(minlex);
                    }
                }
            }
        }

        /*  	map<string,list<string> >::const_iterator itmap;
          	for(itmap = state_classes.begin(); itmap != state_classes.end(); ++itmap) {
          		cout << "class for " << (*itmap).first << " :";
          		list<string>::const_iterator itl;
          		for(itl = (*itmap).second.begin(); itl!=(*itmap).second.end(); ++itl) {
          			cout << " " << *itl;
          		}
          		cout << endl;
          	}*/
    }

//cout << "state classes computed \n";

    list<Ttrans>::iterator it;
    for(it = mts->ltrans.begin(); it != mts->ltrans.end(); ++it)
    {
        string s = (*it).event;
        string source = (*it).source;
        if (state_classes.find(source) != state_classes.end())
        {
            source = state_classes[source].front();
        }
        string target = (*it).target;
        if (state_classes.find(target) != state_classes.end())
        {
            target = state_classes[target].front();
        }
        if (visible_events.empty() or (visible_events.find(s) != visible_events.end() and visible_events[s]))
        {
// cout << source << " " << s << " " << target << endl;
            map<string,EvTRel *>::iterator itb = eventTRs.find(s);
            if (itb == eventTRs.end())
            {
                eventTRs[s] = new EvTRel(s/*,mgr*/);
                eventTRs[s]->add_er(SS(enc->state(source)));
                eventTRs[s]->add_sr(SS(enc->state(target)));
                SS x = SS(enc->state(source) * enc->change_vars(enc->state(target)));
                eventTRs[s]->add_tr(x);

                // experiment: adding the reverse tr
                SS y = SS(enc->state(target) * enc->change_vars(enc->state(source)));
//  		cout << "adding rev_tr:";y.print();
                eventTRs[s]->add_rev_tr(y);

                fresh_copy[s] = 1;
                computed_ecs[s] = false;
            }
            else
            {
                EvTRel *etr = (*itb).second;
                etr->add_er(SS(enc->state(source)));
                etr->add_sr(SS(enc->state(target)));
                SS x = SS(enc->state(source) * enc->change_vars(enc->state(target)));
                eventTRs[s]->add_tr(x);

                // experiment: adding the reverse tr
                SS y = SS(enc->state(target) * enc->change_vars(enc->state(source)));
                //  		cout << "adding rev_tr:";y.print();
                eventTRs[s]->add_rev_tr(y);
            }
        }
    }


    /*  map<string,EvTRel *>::const_iterator itm;
      int ersrtr_size = 0;
    	for(itm=eventTRs.begin(); itm != eventTRs.end(); ++itm) {
    		ersrtr_size += itm->second->getNodeCount();
    	}
    	cout << "# Size for the ERs + SRs + TRs = " << ersrtr_size << endl;
    */

    PStoNS = (int *) malloc(sizeof(int)*2*enc->enc_vars() - enc->red_vars());
    NStoPS = (int *) malloc(sizeof(int)*2*enc->enc_vars() - enc->red_vars());
    int nvi = 0;
    for(int i = 0; i < enc->enc_vars(); ++i)
    {
        if (not enc->variable_redundant(i))
        {
            /*  		PStoNS[i]=enc->enc_vars() + i;
              		NStoPS[i]=enc->enc_vars() + i;
            */
            PStoNS[i]=enc->ns_var_index(i);
            NStoPS[i]=enc->enc_vars() + nvi;
            ++nvi;
        }
        else
        {
            PStoNS[i]=i;
            NStoPS[i]=i;
        }
    }
    nvi = 0;
    for(int i = enc->enc_vars(); i < 2*enc->enc_vars() - enc->red_vars(); ++i)
    {
        NStoPS[i]=enc->ps_var_index(i);
        PStoNS[i]=enc->ps_var_index(i);
        ++nvi;
    }
    /*  cout << "TR::nombre de ns vars " << nvi << endl;

      for(int i = 0; i < 2*enc->enc_vars()- enc->red_vars(); ++i) {
      	cout << "Var NS(" << i << "):" << NStoPS[i] << endl;
      }
       for(int i = 0; i < 2*enc->enc_vars()- enc->red_vars(); ++i) {
      	cout << "Var PS(" << i << "):" << PStoNS[i] << endl;
      }*/

    /*
     map<string,EvTRel *>::iterator itb;
     for (itb = eventTRs.begin(); itb != eventTRs.end(); ++itb) {
     	EvTRel *etr = itb->second;
    	etr->print_name();
     	etr->print_tr();
     }
    */

}

SS
TRel::image(string name, const SS &g)
{

    EvTRel *etr = eventTRs[name];
//	cout << "TR: ";etr->get_tr().print();
    SS x = g.AndAbstract(etr->get_tr(), SS(enc->ps_vars));
//  cout << "x: ";x.print();
    SS result = x.Permute(NStoPS);
    return result;
}

SS
TRel::back_image(string name, const SS &g)
{

    EvTRel *etr = eventTRs[name];
    //classical approach for back image
    /*	SS gy = g.Permute(PStoNS);
     	SS result = gy.AndAbstract(etr->get_tr(), SS(enc->ns_vars));
     	*/
    SS x = g.AndAbstract(etr->get_rev_tr(), SS(enc->ps_vars));
    SS result = x.Permute(NStoPS);
    return result;
}


SS
TRel::forward(const SS &g)
{

//	SS result = mgr.bddZero();
    SS result;
    map<string,EvTRel *>::iterator itb;
    for (itb = eventTRs.begin(); itb != eventTRs.end(); ++itb)
    {
        EvTRel *etr = itb->second;
        SS x = g.AndAbstract(etr->get_tr(), SS(enc->ps_vars));
        result += x;
    }
    SS xp = result.Permute(NStoPS);
    return xp;
}

SS
TRel::forward_restrict(const SS &g, const SS &r)
{
//	SS result = mgr.bddZero();
    SS result;
    map<string,EvTRel *>::iterator itb;
    for (itb = eventTRs.begin(); itb != eventTRs.end(); ++itb)
    {
        EvTRel *etr = itb->second;
        SS x = g.AndAbstract(etr->get_tr(), SS(enc->ps_vars));
        result += x;
    }
    SS xp = result.Permute(NStoPS);
    xp *= r;

    return xp;
}



SS
TRel::backward(const SS &g)
{
//	SS result = mgr.bddZero();
    SS result;
    map<string,EvTRel *>::iterator itb;
    SS gy = g.Permute(PStoNS);
    for (itb = eventTRs.begin(); itb != eventTRs.end(); ++itb)
    {
        EvTRel *etr = itb->second;
        SS x = gy.AndAbstract(etr->get_tr(), SS(enc->ns_vars));
        result += x;
    }
    return result;
}

SS
TRel::backward_restrict(const SS &g, const SS &r)
{
//	SS result = mgr.bddZero();
    SS result;
    map<string,EvTRel *>::iterator itb;
    SS gy = g.Permute(PStoNS);
    for (itb = eventTRs.begin(); itb != eventTRs.end(); ++itb)
    {
        EvTRel *etr = itb->second;
        SS x = gy.AndAbstract(etr->get_tr(), SS(enc->ns_vars));
        x *= r;
        result += x;
    }

    return result;
}

SS
TRel::traversal(const SS &g)
{

    SS from = g;
    SS reached = from;
    SS newto;
    do
    {
        SS to = forward(from);
        newto = to - reached;
        from = newto;
        reached += newto;
    }
    while(not newto.is_empty());

    return reached;
}

SS
TRel::traversal_restrict(const SS &g, const SS &r)
{

    SS from = g;
    SS reached = from;
    SS newto;
    do
    {
        SS to = forward_restrict(from,r);
        newto = to - reached;
        from = newto;
        reached += newto;
    }
    while(not newto.is_empty());

    return reached;
}


SS
TRel::backward_traversal_restrict(const SS &g, const SS &r)
{

    SS from = g;
    SS reached = from;
    SS newto;
    do
    {
        SS to = backward_restrict(from,r);
        newto = to - reached;
        from = newto;
        reached += newto;
    }
    while(not newto.is_empty());

    return reached;
}
/*
void
TRel::connected_components(const SS &g, vector<SS> &comps) {
	SS x = g;
	SSvector vb(enc->enc_vars());
	int i;
  for (i = 0; i < enc->enc_vars(); i++) {
        vb[i] = mgr.bddVar(i);
  }

	while (x != mgr.bddZero()) {
		SS s = x.PickOneMinterm(vb);
		SS down = traversal_restrict(s,g);
		SS up = backward_traversal_restrict(down,g);
		comps.push_back(up);
		x -= up;
	}
}
*/
void
TRel::split_event_gradients(string event_selected, map<int,SS> &gradients, list<string> &new_names)
{
//cout << "#   Split event: " << event_selected << endl;
    map<int,SS>::const_iterator itg = ++gradients.begin();
    /*
    for(map<int,SS>::const_iterator itg2 = gradients.begin(); itg2 != gradients.end();++itg2) {
    	cout << "Gradient " << itg2->first << " states: \n";itg2->second.print(2,2);
    }*/

    EvTRel *etr = eventTRs[event_selected];
    SS acc_tr,acc_rev_tr;
    while (itg != gradients.end())
    {
        // setting the new event
        char cchar[10];
        sprintf (cchar, "_%d",fresh_copy[event_selected]++);
        string str_number= string(cchar);
        string new_name = event_selected + str_number;
        EvTRel *etr_new = new EvTRel(new_name/*,mgr*/);
        etr_new->set_excitation_closed(false);
        etr_new->add_tr(itg->second);
        etr_new->add_er(itg->second.ExistAbstract(SS(enc->ns_vars)));
        SS g = etr_new->get_er();
        SS x = g.AndAbstract(etr_new->get_tr(), SS(enc->ps_vars));

        SS xp = x.Permute(NStoPS);
        etr_new->add_sr(xp);

        etr_new->add_rev_tr(SS(enc->flip_tr(etr_new->get_tr().states,PStoNS,NStoPS)));
//		cout <<"Rev tr: ";etr_new->get_rev_tr().print();

        eventTRs[new_name] = etr_new;
        fresh_copy[new_name] = 1;
        computed_ecs[new_name] = false;
        computed_ecs[event_selected] = false;
//	  etr_new->set_intersect_covering(cover);

        // accumulating the ers
        acc_tr += etr_new->get_tr();
        acc_rev_tr += etr_new->get_rev_tr();
        /*
        cout << "ER for " << new_name << ": ";
        etr_new->get_er().print();
        cout << "TR for " << new_name << ": ";
        etr_new->get_tr().print(2,2);
        */
        new_names.push_back(new_name);
        ++itg;
    }

    // updating the old event
//cout << "antic rev tr: ";
//etr->get_rev_tr().print();
    etr->set_excitation_closed(false);
    etr->set_tr(etr->get_tr() - acc_tr);
    etr->set_er(etr->get_tr().AndAbstract(etr->get_er(), SS(enc->ns_vars)));
    etr->set_rev_tr(etr->get_rev_tr() - acc_rev_tr);

    SS x = etr->get_er().AndAbstract(etr->get_tr(), SS(enc->ps_vars));
//	SS xp = rename_vars_to_x(x, 2*enc->enc_vars() -1);
    SS xp = x.Permute(NStoPS);
    etr->set_sr(xp);

    /*cout << "ER for " << event_selected << ": ";
    etr->get_er().print(2,2);*/


}

void
TRel::split_event_er(string name, const SS &g)
{
    EvTRel *etr = eventTRs[name];

    // setting the new event
    char cchar[10];
    sprintf (cchar, "_%d",fresh_copy[name]++);
    string str_number= string(cchar);
    string new_name = name + str_number;
    EvTRel *etr_new = new EvTRel(new_name/*,mgr*/);
    etr_new->add_er(g);
    etr_new->add_tr(etr->get_tr() * g);
    SS x = g.AndAbstract(etr_new->get_tr(), SS(enc->ps_vars));
//  SS xp = rename_vars_to_x(x, 2*enc->enc_vars() -1);
    SS xp = x.Permute(NStoPS);
    etr_new->add_sr(xp);
    eventTRs[new_name] = etr_new;
    fresh_copy[new_name] = 1;

    // updating the old event
    etr->set_er(etr->get_er() - g);
    etr->set_tr(etr->get_tr() * etr->get_er());
    x = etr->get_er().AndAbstract(etr->get_tr(), SS(enc->ps_vars));
//	xp = rename_vars_to_x(x, 2*enc->enc_vars() -1);
    xp = x.Permute(NStoPS);
    etr->set_sr(xp);


    /*cout << "ER for " << name << ": ";
    etr->get_er().print(2,2);*/
    /*cout << "SR for " << name << ": ";
    etr->get_sr().print(2,2);
    cout << "TR for " << name << ": ";
    etr->get_tr().print(2,2);
    */
    /*cout << "ER for " << new_name << ": ";
    etr_new->get_er().print();*/
    /*cout << "SR for " << new_name << ": ";
    etr_new->get_sr().print(2,2);
    cout << "TR for " << new_name << ": ";
    etr_new->get_tr().print(2,2);
    */
}


void
TRel::split_event_sr(string name, const SS &g)
{
    EvTRel *etr = eventTRs[name];

    // setting the new event
    char cchar[10];
    sprintf (cchar, "%d",fresh_copy[name]++);
    string str_number= string(cchar);
    string new_name = name + str_number;
    EvTRel *etr_new = new EvTRel(new_name/*,mgr*/);
    etr_new->add_sr(g);
//	SS gy = rename_vars_to_y(g,2*enc->enc_vars()-1);
    SS gy = g.Permute(PStoNS);
    etr_new->add_tr(etr->get_tr() * gy);
    SS result = gy.AndAbstract(etr_new->get_tr(), SS(enc->ns_vars));
    etr_new->add_er(result);
    eventTRs[new_name] = etr_new;

    // updating the old event
    etr->set_sr(etr->get_sr() - g);
//	gy = rename_vars_to_y(etr->get_sr(),2*enc->enc_vars()-1);
    gy = g.Permute(PStoNS);
    etr->set_tr(etr->get_tr() * gy);
    etr->set_er(gy.AndAbstract(etr->get_tr(), SS(enc->ns_vars)));

    cout << "ER for " << name << " : ";
    etr->get_er().print();
    cout << "ER for " << new_name << ": ";
    etr_new->get_er().print();


}

void
TRel::trigger_set(string name, list<string> &triggset)
{
    map<string,EvTRel *>::iterator itb;
    EvTRel *ev_ref = eventTRs[name];
    for (itb = eventTRs.begin(); itb != eventTRs.end(); ++itb)
    {
        EvTRel *etr = itb->second;
        SS x = etr->get_sr() * ev_ref->get_er();
        SS back_x = backward(x) - ev_ref->get_er();
        if (not back_x.is_empty())
        {
            triggset.push_back(itb->first);
//  		cout << "trigger de " << name << " es " << itb->first <<endl;
        }
    }
}

SS
TRel::compute_local_state_space()
{
    SS result(0);
    map<string,EvTRel *>::iterator itb;
    for (itb = eventTRs.begin(); itb != eventTRs.end(); ++itb)
    {
        EvTRel *etr = itb->second;
        result += etr->get_er();
        result += etr->get_sr();
    }
    return result;
}

