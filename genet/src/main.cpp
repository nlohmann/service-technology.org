#include <iostream>
#include <fstream>
#include <cmath>
#include <sstream>
#include <set>
#include <queue> // priority_queue
#include <ext/hash_map>

#include "config.h"
#include "config-log.h"
#include "cmdline.h"
#include <libgen.h>
#include "Output.h"
#include "verbose.h"

#include "cuddObj.hh"
#include "SS.h"
#include "parser/tokens.h"
#include "parser/DLGLexer.h"
#include "parser/TSParser.h"
#include "parser/global.h"
#include "ATokenBuffer.h"
#include "PBlackBox.h"
#include "Encoding.h"
#include "TRel.h"
#include "Region.h"
#include "HashRegions.h"

typedef ANTLRCommonToken ANTLRToken;

using namespace std;
using namespace __gnu_cxx; // extension to include the hash_map


// extern functions
bool generate_conservative_cover(TRel &tr, int kmax, const SS &state_space, const SS &initial_state, list<Region *> &minregs);
bool generate_conservative_sm_cover(TRel &tr, int kmax, const SS &state_space, const SS &initial_state, list<Region *> &minregs);

void divide_conquer_cc(TRel &tr, int kmax, const SS &state_space, const SS &initial_state, list<Region *> &minregs);
void divide_conquer_mining(TRel &tr, int kmax, const SS &state_space, const SS &initial_state, list<Region *> &minregs);
void divide_conquer_synthesis(TRel &tr, int kmax, const SS &state_space, const SS &initial_state, list<Region *> &minregs);

void genet_usage();
void print_regions(const list<Region *> &minregs);
bool is_excitation_closed(const list<Region *> &minregs, map<Region *,bool> &essential,TRel &tr,string& violation_event);
bool is_excitation_closed_event(const list<Region *> &minregs, string event,const SS &er, TRel &tr, SS &states_out_er);
void pn_synthesis(const list<Region *> &minregs,TRel &tr, const SS &initial, int KMAX);
void pn_synthesis(const string outfile,const list<Region *> &minregs,TRel &tr, const SS &initial, int KMAX);
bool label_split(const list<Region *> &minregs,TRel &tr, string violation_event);
bool split_event(string violation_event, TRel &tr, Encoding &enc);
bool er_split(const list<Region *> &minregs,SS &g, TRel &tr, string v_event,list<string> &new_names);
bool split_trigger_events(const list<string> &ltrigg, const list<Region *> &minregs,TRel &tr, string violation_event);
void compute_essential(Region *r, const list<Region *> &minregs, map<Region *,bool> &essential,
                       TRel &tr, bool mining_flag = false);

void compute_smallest_enabling_topsets(list<pair<Region*,Region *> > &star_event,
                                       const list<Region *> &minregs,TRel &tr, string event);

void compute_events_covered(const list<Region *> &minregs,TRel &tr);

bool generate_minimal_regions_on_demand(TRel &tr, int k, int explore_regions, const SS &state_space,
                                        list<Region *> &minregs, list<string> &violation_events);
bool generate_minimal_regions_all(TRel &tr, int k, const SS &state_space,
                                  list<Region *> &minregs,bool mgcond, bool force_ec_sigs, map<string,bool> &ec_sigs,list<string> &violation_events);
bool remove_nonminimal_region(Region *r, hash_map<Region *,bool, hashRegion, KeysReg> &act_set, TRel &tr);

void update_topset_information(TRel &tr, map<string,EvTRel *> &tr_map,Region *r, SS &states_out_er, const string& event);
void update_cover_information(EvTRel * evt,const list<Region *> &minregs);
bool covers_some_uncovered_event(Region *r, map<string,EvTRel *> &tr_map);

// to store the intermidiate multisets computed one extending an ER
//hash_map<const SS, set<Region *,KeysReg>, hashSS, KeysSS> cache_generate;
hash_map<const SS, hash_map<Region *,bool,hashRegion,KeysReg>, hashSS, KeysSS> cache_generate;

// memoization on the set of regions generated
hash_map<Region*, Region *, hashRegion, KeysReg> cache_regions;

// memoization of the topsets
map<string,list<pair<Region*, Region* > > > cache_topsets;
map<string,bool> topset_computed;

// memoization of the set of regions that guarantee EC for each event
map<string, map<Region*,bool> > witness_ec_events;
hash_map<Region*, map<string,bool>,hashRegion, KeysReg> events_covered;

int ncovered;
map<string,bool> covering_flag;
int MAX_RCOV;

// the static definition of the cudd mgr
Cudd *SS::mgr;
int extra_vars;

BDD reach;
//int Region::nregs = 0;

/// the command line parameters
gengetopt_args_info args_info;

/// the invocation string
std::string invocation;

/// replace one occurrence of a substring in a string
std::string replaceOnce(std::string result, const std::string& replaceWhat,
                        const std::string& replaceWithWhat) {
    const int pos = result.find(replaceWhat);
    if (pos == -1) {
        return result;
    }
    result.replace(pos, replaceWhat.size(), replaceWithWhat);
    return result;
}


/// check if a file exists and can be opened for reading
inline bool fileExists(const std::string& filename) {
    std::ifstream tmp(filename.c_str(), std::ios_base::in);
    return tmp.good();
}


/// evaluate the command line parameters
void evaluateParameters(int argc, char** argv) {
    // overwrite invocation for consistent error messages
    // TODO: basename seems to allocated memory which is not freed afterwards,
    // i.e., valgrind 3.6.0.SVN reports a memory leak
    // a fix like "free(argv[0]);" after the call of the cmdline parser works an Mac OS 10.6.4
    // but unfortunately seems to fail on linux
    argv[0] = basename(argv[0]);

    // store invocation in a std::string for meta information in file output
    for (int i = 0; i < argc; ++i) {
        invocation += (i == 0 ? "" : " ") + std::string(argv[i]);
    }

    // initialize the parameters structure
    struct cmdline_parser_params* params = cmdline_parser_params_create();

    // call the cmdline parser
    if (cmdline_parser(argc, argv, &args_info) != 0) {
        abort(7, "invalid command-line parameter(s)");
    }

    // debug option
    if (args_info.bug_flag) {
        {
            Output debug_output("bug.log", "configuration information");
            debug_output.stream() << CONFIG_LOG;
        }
        message("Please send file '%s' to %s!", _cfilename_("bug.log"), _coutput_(PACKAGE_BUGREPORT));
        exit(EXIT_SUCCESS);
    }

    // read a configuration file if necessary
    if (args_info.config_given) {
        // initialize the config file parser
        params->initialize = 0;
        params->override = 0;

        // call the config file parser
        if (cmdline_parser_config_file(args_info.config_arg, &args_info, params) != 0) {
            abort(14, "error reading configuration file '%s'", args_info.config_arg);
        } else {
            status("using configuration file '%s'", _cfilename_(args_info.config_arg));
        }
    } else {
        // check for configuration files
        std::string conf_generic_filename = std::string(PACKAGE) + ".conf";
        std::string conf_filename = fileExists(conf_generic_filename) ? conf_generic_filename :
                                    (fileExists(std::string(SYSCONFDIR) + "/" + conf_generic_filename) ?
                                     (std::string(SYSCONFDIR) + "/" + conf_generic_filename) : "");

        if (conf_filename != "") {
            // initialize the config file parser
            params->initialize = 0;
            params->override = 0;
            if (cmdline_parser_config_file(const_cast<char*>(conf_filename.c_str()), &args_info, params) != 0) {
                abort(14, "error reading configuration file '%s'", conf_filename.c_str());
            } else {
                status("using configuration file '%s'", _cfilename_(conf_filename));
            }
        } else {
            status("not using a configuration file");
        }
    }

    // check whether at most one file is given
    if (args_info.inputs_num > 1) {
        abort(4, "at most one input file must be given");
    }

    free(params);
}


int genet_init(int argc,char *argv[])
{

    int KMAX = 1;
    int optind = 1;
    int explore_regions = 1;
    int minimal_k = 1;
    bool on_demand = true;
    bool mgcond = false;
    int enc_method = 1;
    string outfile = "";
    bool hiding = false;
    bool project = false;
    bool compute_ccs=false;
    bool compute_recursive = false;
    bool force_ec_sigs = false;
    char *fname = (char *) malloc(sizeof(char)*200);
    string hide_sigs, project_string, ec_string;
    map<string,bool> prj_sigs, ec_sigs;
    MAX_RCOV = 10000;
    ncovered = 0;

    /*--------------------------------------.
    | 0. parse the command line parameters  |
    `--------------------------------------*/
    evaluateParameters(argc, argv);
    
    cout << "# Usage:\n" << "#  " << invocation << std::endl;

    if (args_info.cardinality_given) {
        KMAX = args_info.cardinality_arg;
    }
    if (args_info.min_cardinality_given) {
        minimal_k = args_info.min_cardinality_arg;
    }
    if (args_info.explore_given) {
        explore_regions = args_info.explore_arg;
    }
    if (args_info.pm_given) {
        on_demand = false;
    }
    if (args_info.mg_given) {
        mgcond = true;
    }
    if (args_info.cov_given) {
        MAX_RCOV = args_info.cov_arg;
    }
    if (args_info.encoding_given) {
        enc_method = args_info.encoding_arg;
    }
    if (args_info.hide_given) {
        hide_sigs = args_info.hide_arg;
        hiding = true;
    }
    if (args_info.projection_given) {
        project_string = args_info.projection_arg;
        project = true;
    }
    if (args_info.ec_given) {
        ec_string = args_info.ec_arg;
        force_ec_sigs = true;
    }
    if (args_info.cc_given) {
        compute_ccs = true;
    }
    if (args_info.rec_given) {
        compute_recursive = true;
    }
    if (args_info.output_given) {
        outfile = args_info.output_arg;
    }

    if (args_info.inputs_num != 1) {
        abort(1, "need exactly one file as input");
    } else {
        if (not fileExists(std::string(args_info.inputs[0]))) {
            abort(1, "input file does not exist.");
        }
        strcpy(fname,args_info.inputs[0]);
    }

    cmdline_parser_free(&args_info);

    if (hiding)
    {
        string run_pfy = "petrify -dead -o genet_tmp.g -hide " + hide_sigs + " " + string(fname);
        cout << "# Running: " << run_pfy << endl;
        if (std::system(run_pfy.c_str()) != 0) {
            cout << "# There was an error while running petrify.";
        }
        string run_write_sg = "write_sg genet_tmp.g -o genet_sg.g";
        cout << "# Running: " << run_write_sg <<endl;
        if (std::system(run_write_sg.c_str()) != 0) {
            cout << "# There was an error while running petrify.";
        }
        cout << "# Done\n";
        strcpy(fname,"genet_sg.g");
    }

    ParserBlackBox<DLGLexer, TSParser, ANTLRToken> p(fname);
//cout << "Reading the ts ... "; cout.flush();
    Tts *t = new Tts();
    t = p.parser()->ats(t); //starting the parser
    //cout << " Done" << endl;
    // first var to create the Cudd manager! dont remove.
    SS state_space;
    Cudd mgr = *(SS::mgr);
//cout << "Encoding the ts ...";cout.flush();
    Encoding e(t, enc_method, &mgr);
    extra_vars = e.next_vars() + e.red_vars();
//cout << " Done" << endl;

//cout << "Building the transition relation ...";cout.flush();
    TRel tr(&e, t);
    if (project) prj_sigs = tr.tokenize(project_string,",");
    tr.build_tr(prj_sigs);
//cout << " Done" << endl;


//cout << "Computing the state space in a SS ..." ;cout.flush();
    state_space = tr.traversal(e.state(t->initial));
    reach = state_space.states;
//	state_space.print();

//	state_space.setCareSet(&state_space.states);
//	SS::care_set = &state_space.states;

//cout << "Care set: ";
//SS::care_set.print(2,2);
//cout << " Done" << endl;
    /*
    	map<string,EvTRel *>::const_iterator itm;
    	map<string,EvTRel *> tr_map = tr.get_map_trs();
    	int ersrtr_size = 0;
    	for(itm=tr_map.begin(); itm != tr_map.end(); ++itm) {
    		topset_computed [itm->first] = false;
    		itm->second->Minimize();
    		ersrtr_size += itm->second->getNodeCount();
    	}
      cout << "# Size for the ERs + SRs + TRs = " << ersrtr_size << endl;
    */
    // the maximal capacity allowed
    int k = minimal_k;
    bool ec = false;
    list<Region *> minregs;

    if (force_ec_sigs) ec_sigs = tr.tokenize(ec_string,",");

    if (compute_recursive and compute_ccs)
    {
        divide_conquer_cc(tr, KMAX, state_space,e.state(t->initial),minregs);
    }

    else if (compute_ccs)
    {
        if (KMAX==1)	generate_conservative_sm_cover(tr, 1, state_space,e.state(t->initial),minregs);
        else generate_conservative_cover(tr, KMAX, state_space,e.state(t->initial),minregs);
    }

    else if (compute_recursive and not on_demand)
    {
        divide_conquer_mining(tr, KMAX, state_space,e.state(t->initial),minregs);
    }

    else if (compute_recursive and on_demand)
    {
        divide_conquer_synthesis(tr, KMAX, state_space,e.state(t->initial),minregs);
    }

    else if (not on_demand)
    {
        list<string> violation_events;
        ec = generate_minimal_regions_all(tr, KMAX, state_space, minregs,mgcond,force_ec_sigs,ec_sigs,violation_events);
        //computing the witness/event covered information
        compute_events_covered(minregs,tr);
        // now computing the essential regions with the EC relaxed
        /*		map<Region *,bool> essential;
          	list<Region *>::const_iterator itmr;
        		for(itmr = minregs.begin(); itmr != minregs.end(); ++itmr) essential[*itmr] = true;
        		for(itmr = minregs.begin(); itmr != minregs.end(); ++itmr) {
        			compute_essential(*itmr,minregs,essential,tr,true);
        		}
        //			cout << "--------------- Removing non-essential (actual size: " << minregs.size() << ")\n";
        		hash_map<Region *,bool, hashRegion, KeysReg> tmp_set;
        	  list<Region *> todelete;
        		// deleting non-essential regions
        		for (itmr = minregs.begin(); itmr != minregs.end(); ++itmr) {
        			Region *rr(*itmr);
        			if (essential[rr]) tmp_set[rr] = true;
        			else {
        				todelete.push_back(rr);
        					//delete rr;
        //					cout << "   deleting a redundant region\n";
        			}
        		}
        		if (not todelete.empty()) {
        		  for (itmr = todelete.begin(); itmr != todelete.end(); ++itmr) delete *itmr;
        		}

        		minregs.clear();
        		hash_map<Region *,bool, hashRegion, KeysReg>::const_iterator iths;
        		for(iths = tmp_set.begin(); iths != tmp_set.end(); ++iths) minregs.push_front(iths->first);
        			//minregs = tmp_set;*/
//		print_regions(minregs);

// EXPERIMENTAL! trying to force the ec of some events in the mining option
        while (not ec)
        {
            explore_regions = 0;
            // code for label-splitting.
            cout << "# Excitation closure violation for some events in the mining mode. Splitting labels. Nb regs: " << minregs.size() <<  endl;
            list<string>::const_iterator itv;
            string min_viol = *(violation_events.begin());
            EvTRel *evtr = tr.get_event_tr(min_viol);
            SS er = evtr->get_er();
            Region rmin(er);
            int nmin = rmin.number_violations(tr,true);
//  			cout << "Event: " << min_viol << " nb viols: " << rmin.number_violations(tr,true) << endl;
            for(itv = ++violation_events.begin(); itv != violation_events.end(); ++itv)
            {
                string violation = *itv;
                EvTRel *evtr = tr.get_event_tr(violation);
                SS er = evtr->get_er();
                Region r(er);
                if (r.number_violations(tr,true) < nmin)
                {
                    min_viol = violation;
                    nmin = r.number_violations(tr,true);
                }
                //cout << "Event: " << violation << " nb viols: " << r.number_violations(tr,true) << endl;
            }
            //cout << "# Event with minim viols: " << min_viol << endl;
            violation_events.push_front(min_viol);
            bool splitted = false;
            list<string> new_names;

            for(itv = violation_events.begin(); itv != violation_events.end() and not splitted; ++itv)
            {
                string violation = *itv;
                string non_constant_event;
                EvTRel *evtr = tr.get_event_tr(violation);
                SS er = evtr->get_er();
                Region rr(er);
                non_constant_event = rr.choose_event_non_constant_gradient(tr);
                rr.recheck_gradients(tr,true);
                map<string ,map<int,SS> > gradients = rr.get_map_grads();
                tr.split_event_gradients(non_constant_event, gradients[non_constant_event],new_names);
                splitted = (not new_names.empty());
            }

            hash_map<const SS, hash_map<Region *,bool,hashRegion,KeysReg>, hashSS, KeysSS>::iterator itbdd = cache_generate.begin();
            while (itbdd != cache_generate.end())
            {
                itbdd->second.clear();
                ++itbdd;
            }
            // setting the flag denoting the computation of the ec for each event to false
            map<string,EvTRel *>::const_iterator itm;
            map<string,EvTRel *> tr_map = tr.get_map_trs();
            for(itm=tr_map.begin(); itm != tr_map.end(); ++itm)
            {
                if (ec_sigs[itm->first] and not itm->second->is_excitation_closed())
                {
                    tr.change_computed_ec(itm->first, false);
//			cache_topsets[itm->first].clear();
                    topset_computed[itm->first] = false;
//				cout << "topsets still needed for: " << itm->first << endl;
                }
            }

            // computing again regions
            violation_events.clear();
            ec = generate_minimal_regions_all(tr, KMAX, state_space, minregs,mgcond,force_ec_sigs,ec_sigs,violation_events);
        }



        if (outfile != "")  pn_synthesis(outfile,minregs,tr,e.state(t->initial),KMAX);
        else pn_synthesis(minregs,tr,e.state(t->initial),KMAX);
    }
    else
    {
        do
        {
            list<string> violation_events;
            ec = generate_minimal_regions_on_demand(tr,k,explore_regions,state_space,minregs,violation_events);
            map<Region *,bool> essential;
            string v_event;
            ec = is_excitation_closed(minregs,essential,tr,v_event);

            if (ec)
            {
                //computing the witness/event covered information
                compute_events_covered(minregs,tr);
//cout << "Excitation closure holds. Number of regions: " << minregs.size() << " .Trying to eliminate redundant regions\n";
                list<Region *>::const_iterator itmr;
                for(itmr = minregs.begin(); itmr != minregs.end(); ++itmr) essential[*itmr] = true;

                // now computing the essential regions to guarantee the EC.
                for(itmr = minregs.begin(); itmr != minregs.end(); ++itmr)
                {
                    compute_essential(*itmr,minregs,essential,tr);
                }
//	cout << "Finish computation essentials\n";
//print_regions(minregs);
//			cout << "--------------- Removing non-essential (actual size: " << minregs.size() << ")\n";
                hash_map<Region *,bool, hashRegion, KeysReg> tmp_set;
                list<Region *> todelete;
                // deleting non-essential regions
                for (itmr = minregs.begin(); itmr != minregs.end(); ++itmr)
                {
                    Region *rr(*itmr);
                    if (essential[rr]) tmp_set[rr] = true;
                    else
                    {
                        todelete.push_back(rr);
                        //delete rr;
//					cout << "   deleting a redundant region\n";
                    }
                }
                if (not todelete.empty())
                {
                    for (itmr = todelete.begin(); itmr != todelete.end(); ++itmr)
                    {
                        remove_nonminimal_region(*itmr,tmp_set, tr);
                    }
                }

                minregs.clear();
                hash_map<Region *,bool, hashRegion, KeysReg>::const_iterator iths;
                for(iths = tmp_set.begin(); iths != tmp_set.end(); ++iths) minregs.push_front(iths->first);
                //minregs = tmp_set;

//			cout << "--------------- Removing non-minimal (actual size: " << minregs.size() << ")\n";
                /* removing non-minimal regions */
                bool changes;
                do
                {
                    changes = false;
                    hash_map<Region *,bool, hashRegion, KeysReg> tmp_set;
                    list<Region *> todelete;
                    list<Region *>::const_iterator itmr;
                    for (itmr = minregs.begin(); itmr != minregs.end(); ++itmr)
                    {
                        Region *r1(*itmr);
                        bool rminimal = true;
                        list<Region *>::const_iterator itmr2;
                        SS x;
                        Region r_inside(x);
                        for (itmr2 = minregs.begin(); itmr2 != minregs.end(); ++itmr2)
                        {
                            if (itmr == itmr2) continue;
                            Region *r2(*itmr2);
                            if (*r1 > *r2 or *r1 == *r2)
                            {
                                rminimal = false;
                                r_inside+=(*r2);
                            }
                        }
                        if (rminimal or (r_inside == (*r1))) tmp_set[r1] = true;
                        else
                        {
                            bool found_mid_er = false;
                            map<string,bool>::const_iterator itec;
                            for(itec = events_covered[r1].begin(); itec != events_covered[r1].end() and not found_mid_er; ++itec)
                            {
                                string ev_covered = itec->first;
                                SS er_event = tr.get_event_tr(ev_covered)->get_er();
                                SS inter(1);
                                map<Region*,bool>::const_iterator itw;
                                for(itw = witness_ec_events[ev_covered].begin(); itw != witness_ec_events[ev_covered].end(); ++itw)
                                {
                                    if (not ((*(*itw).first) == *r1))
                                    {
                                        SS sup = (itw->first)->sup();
                                        inter *= sup;
                                    }
                                }
                                if (not (er_event > inter))
                                {
                                    found_mid_er = true;
                                    break;
                                }
                            }
                            if (found_mid_er) tmp_set[r1] = true;
                            else todelete.push_back(r1);
                        }
                    }

                    if (not todelete.empty())
                        for (itmr = todelete.begin(); itmr != todelete.end(); ++itmr) changes = changes or remove_nonminimal_region(*itmr,tmp_set,tr);

                    minregs.clear();
                    hash_map<Region *,bool, hashRegion, KeysReg>::const_iterator iths;
                    for(iths = tmp_set.begin(); iths != tmp_set.end(); ++iths) minregs.push_front(iths->first);
                }
                while (changes);

//	print_regions(minregs);
//			cout << "--------------- Size after removing redundant regions : " << minregs.size() << "\n";
                // synthesis of a PN.
//		  cout << "|minregs|: " << minregs.size() << endl;
                if (outfile != "")  pn_synthesis(outfile,minregs,tr,e.state(t->initial),KMAX);
                else pn_synthesis(minregs,tr,e.state(t->initial),KMAX);

//			pn_synthesis(outfile,minregs,tr,e.state(t->initial),KMAX);
                //cleanning memory
                /*	  map<string,EvTRel *>::const_iterator itm;
                	  map<string,EvTRel *> tr_map = tr.get_map_trs();
                		for(itm=tr_map.begin(); itm != tr_map.end(); ++itm) {
                			cache_topsets[itm->first].clear();
                		}*/
            }
            else
            {
//			cout << "|regs|: " << minregs.size() << endl;
//			cout << "|events without EC|:" << violation_events.size() <<endl;
                if (k == KMAX)
                {
                    explore_regions = 0;
                    // code for label-splitting.
                    cout << "# Excitation closure violation max capacity " << k << ". Splitting labels. Nb regs: " << minregs.size() <<  endl;
                    list<string>::const_iterator itv;
                    string min_viol = *(violation_events.begin());
                    EvTRel *evtr = tr.get_event_tr(min_viol);
                    SS er = evtr->get_er();
                    Region rmin(er);
                    int nmin = rmin.number_violations(tr,true);
//  			cout << "Event: " << min_viol << " nb viols: " << rmin.number_violations(tr,true) << endl;
                    list<string> new_names;
                    for(itv = ++violation_events.begin(); itv != violation_events.end(); ++itv)
                    {
                        string violation = *itv;
                        EvTRel *evtr = tr.get_event_tr(violation);
                        SS er = evtr->get_er();
                        Region r(er);
                        if (r.number_violations(tr,true) < nmin)
                        {
                            min_viol = violation;
                            nmin = r.number_violations(tr,true);
                        }
//					cout << "Event: " << violation << " nb viols: " << r.number_violations(tr,true) << endl;
                    }
//   		cout << "Minim viols: " << min_viol << endl;
                    violation_events.push_front(min_viol);
                    bool splitted = false;
                    for(itv = violation_events.begin(); itv != violation_events.end() and not splitted; ++itv)
                    {
                        string violation = *itv;
                        //				splitted = label_split(minregs,tr,violation,mgr);
                        EvTRel *evtr = tr.get_event_tr(violation);
                        SS er = evtr->get_er();
                        splitted = er_split(minregs,er,tr,violation,new_names);
                    }
                    if (not splitted)
                    {
                        cout << "# PRROBLEM ON SPLITTING\n";
                        // take one of the violation events and manually split it to make some progress
                        string violation = violation_events.front();
                        if (not split_event(violation,tr,e))
                        {
                            cout << "Fatal error: no splitting can be done\n";
                            exit(1);
                        }
                    }
                }
                else
                {
                    cout << "# Excitation closure violation for capacity " << k << ". Nb regs: " << minregs.size() << " . Trying " << k + 1 << endl;
                    ++k;
                }

                hash_map<const SS, hash_map<Region *,bool,hashRegion,KeysReg>, hashSS, KeysSS>::iterator itbdd = cache_generate.begin();
                while (itbdd != cache_generate.end())
                {
                    itbdd->second.clear();
                    ++itbdd;
                }
                // setting the flag denoting the computation of the ec for each event to false
                map<string,EvTRel *>::const_iterator itm;
                map<string,EvTRel *> tr_map = tr.get_map_trs();
                for(itm=tr_map.begin(); itm != tr_map.end(); ++itm)
                {
                    if (not itm->second->is_excitation_closed())
                    {
                        tr.change_computed_ec(itm->first, false);
//				cache_topsets[itm->first].clear();
                        topset_computed[itm->first] = false;
//					cout << "topsets still needed for: " << itm->first << endl;
                    }
                }
            }
        }
        while (not ec);

        int sumaNodes = 0;
        list<Region *>::const_iterator itmr;
        for (itmr = minregs.begin(); itmr != minregs.end(); ++itmr)
        {
            sumaNodes += (*itmr)->Size();
        }
        cout << "# Total size in nodes: " << sumaNodes << endl;

        sumaNodes = 0;
        for (itmr = minregs.begin(); itmr != minregs.end(); ++itmr)
        {
            (*itmr)->minimize(state_space);
            sumaNodes += (*itmr)->Size();
        }
        cout << "# Total size in nodes: " << sumaNodes << endl;
    }
    cache_generate.clear();
    cache_regions.clear();
    cache_topsets.clear();
    topset_computed.clear();
    witness_ec_events.clear();
    events_covered.clear();
    covering_flag.clear();
    extra_vars = 0;
    reach = *(new BDD());
    delete(SS::mgr);
    SS::mgr = NULL;

    return 0;
}


/////////////////////////////////   functions ///////////////////////////////////////////////////////////

bool remove_nonminimal_region(Region *r,hash_map<Region *,bool,hashRegion, KeysReg> &tmp_set, TRel &tr)
{
    /*
    cout << "-----------------------------------------------" << endl;	*/
//cout << "Removing region \n";r->print();

    bool added_new_regions = false;
    SS rsup = r->sup();
    map<string,bool>::const_iterator itec;
//	cout << "Events covered for:\n ";
//	r->print(); 		cout << endl;

    SS x;
    Region r_inside(x);
    hash_map<Region *,bool,hashRegion, KeysReg>::const_iterator itmr;
    for (itmr = tmp_set.begin(); itmr != tmp_set.end(); ++itmr)
    {
        if (*(itmr->first) < *r)
        {
            /*			cout << "used region \n";
            			itmr->first->print();*/
            r_inside += *(itmr->first);
        }
    }
//	cout << "Region accum:\n "; r_inside.print();
    bool found = false;
//	cout << "size events covered for the region: " << events_covered[r].size() << endl;
    for(itec = events_covered[r].begin(); itec != events_covered[r].end() and not found; ++itec)
    {
        string ev_covered = itec->first;
//  	cout << " " << ev_covered << " ";
        SS er_event = tr.get_event_tr(ev_covered)->get_er();
        SS inter(1);
        map<Region*,bool>::const_iterator itw;
        for(itw = witness_ec_events[ev_covered].begin(); itw != witness_ec_events[ev_covered].end(); ++itw)
        {
            if (not ((*(*itw).first) == *r))
            {
                SS sup = (itw->first)->sup();
                inter *= sup;
            }
        }

        if ((er_event > inter) and not(r_inside == *r))
        {
            found = true;
            break;
        }
    }
// 	cout << endl;
    if (found)
    {
        // there are some states not covered when eliminating the non minimal region r, which must be covered by other region in
        // order to preserve the ec
        Region *rr = new Region(*r);
//		cout << "subtracting :\n";r_inside.print();
        *rr -= r_inside;
//		cout << "region to add: "<< tmp_set[rr] << endl; rr->print();
        if ((not rr->sup().is_empty()) or tmp_set[rr]) delete rr;
        else
        {
// 			cout << " Added the new region:\n ";rr->print();
            rr->recheck_gradients(tr,false);
            added_new_regions = true;
            tmp_set[rr] = true;
            SS rrsup = rr->sup();
            for(itec = events_covered[r].begin(); itec != events_covered[r].end(); ++itec)
            {
                if (tr.get_event_tr(itec->first)->get_er() <= rrsup)
                {
                    events_covered[rr][itec->first]= true;
                    witness_ec_events[itec->first][rr] = true;
                }
            }
        }
    }

    for(itec = events_covered[r].begin(); itec != events_covered[r].end(); ++itec)
    {
        witness_ec_events[itec->first].erase(r);
    }
    events_covered.erase(r);
    tmp_set.erase(r);

    delete r;

// 	cout << "-----------------------------------------------" << endl;
    return added_new_regions;
}


bool er_split_rec(hash_map<Region*, int, hashRegion, KeysReg> &cache_split, SS &g, TRel &tr, string /*v_event*/)
{
    bool split = false;
    Region *r= new Region(g);

    if (cache_split.find(r) != cache_split.end())
    {
        delete r;
        return true;
    }

    cache_split[r] = r->number_violations(tr,true);

    /*	int grad;
    	if (cache_split[r] != tr.number_events()) {
    		// try to reduce the er using the event

    		if (g1 < g and g1 != mgr.bddZero()) {
    			split = er_split_rec(cache_split, g1, tr, v_event,mgr);
    			g -= g1;
    			if (g != mgr.bddZero())		split = er_split_rec(cache_split, g, tr, v_event, mgr) or split;
    		}
    	}*/

    return split;
}

bool er_split(const list<Region *> &/*minregs*/,SS &g, TRel &tr, string /*v_event*/, list<string> &new_names)
{

//	cout << "init Splitting " << v_event << endl;

    //only when there are more than two states makes sense the splitting!

//cout << g.CountMinterm()/pow(2.0,extra_vars) <<endl;
    if (g.CountMinterm()/pow(2.0,extra_vars) <= 1) return false;

    // memoization on the set of regions generated
    hash_map<Region*, int, hashRegion, KeysReg> cache_split;

//	er_split_rec(cache_split, g, tr, v_event, mgr);


    Region *r= new Region(g);

    if (cache_split.find(r) != cache_split.end())
    {
        delete r;
        return true;
    }

//	cout << "violations for the er: " << r->number_violations(tr) <<endl;
    r->recheck_gradients(tr,true);
    cache_split[r] = r->number_violations(tr,true);

    //now only remains to consider the intermediate regions for v_event
//	set<Region *,KeysReg>::iterator iti;
//cout << "Size generate: " << cache_generate[g].size() << endl;
    hash_map<Region *,bool,hashRegion,KeysReg>::iterator iti;
    for (iti = cache_generate[g].begin(); iti != cache_generate[g].end(); ++iti)
    {
        (iti->first)->recheck_gradients(tr,true);
        int k = (iti->first)->number_violations(tr,true);
        cache_split[iti->first] = k;
//		cout << " ..> " << k << endl;
    }


    if (cache_split.empty()) return false;

//cout << "Cache_split size: " << cache_split.size() << endl;
    Region *best = 	cache_split.begin()->first;
    int best_cost = (cache_split.begin())->second;
    double size_best = (cache_split.begin())->first->sup().CountMinterm();
    hash_map<Region*, int, hashRegion, KeysReg>::const_iterator ith;
    for(ith = (++cache_split.begin()); ith != cache_split.end(); ++ith)
    {
//		cout << "number viols " << ith->second << endl;
        if (((best_cost <= 0) and (ith->second > 0)) or
                ((ith->second > 0) and ((best_cost > ith->second) or ((best_cost == ith->second) and
                                        size_best > ith->first->sup().CountMinterm()))))
        {
            best = ith->first;
            best_cost = ith->second;
            size_best = ith->first->sup().CountMinterm();
//cout << "Now best is " << best_cost << " "; best->print();

        }
    }

    //splitting the event on the best partition found
    string event_selected;
//cout << "REgio per splitting: \n"; best->print();
    map<int,SS> gradients = best->compute_best_splitting(event_selected,tr);
//cout << "Splitting " << event_selected << endl;
//	assert(gradients.size()>1);

    tr.split_event_gradients(event_selected, gradients,new_names);


    // for some events the cover must be recomputed
    /*	map<string,EvTRel *> tr_map = tr.get_map_trs();
    	map<string,EvTRel *>::const_iterator itm;
    	for(itm=tr_map.begin(); itm != tr_map.end(); ++itm) {
    		if (itm->second->need_recompute_cover()) {
    			update_cover_information(itm->second,minregs,mgr);
    		}
    	}*/
    return true;
}

void split_disconnected_ers(const vector<SS> &/*lcomps*/, const list<Region *> &minregs,TRel &tr,
                            string violation_event)
{
//cout << "Trying to split er for " << violation_event << endl;
    list<pair<Region*, Region *> > star_event;
    compute_smallest_enabling_topsets(star_event,minregs,tr,violation_event);
    list<pair<Region*, Region *> >::const_iterator itse;
    EvTRel *evtr = tr.get_event_tr(violation_event);
    SS er = evtr->get_er();
    SS inter(1);
    for(itse = star_event.begin(); itse != star_event.end(); ++itse)
    {
        Region rsup = *((*itse).second);
        SS sup = rsup.sup();
        inter *= sup;
    }

    SS out_er = er - inter;
    if (not out_er.is_empty())
    {
        SS new_er_event;
        for(itse = star_event.begin(); itse != star_event.end(); ++itse)
        {
            Region rsup = *((*itse).second);
            SS sup = rsup.sup();
            if (not sup.Intersect(out_er).is_empty())
            {
                new_er_event += sup;
            }
        }
        tr.split_event_er(violation_event,new_er_event);
//		cout << "   Splitting event " << violation_event << endl;
    }
}


// this function is called if label splitting must be done. First it tries to split disconnected ers
// in order to solve the problem. If after disconnected er splitting there is still an EC violation
// the heuristic methods to split triggers are called.
bool label_split(const list<Region *> &minregs,TRel &tr, string v_event)
{

//cout << "Called label_split with " << v_event << endl;
    map<string,EvTRel *> tr_map = tr.get_map_trs();
    map<string,EvTRel *>::const_iterator itm;
    bool split = false;
    for(itm=tr_map.begin(); itm != tr_map.end(); ++itm)
    {
        vector<SS> lcomps;
        string event = itm->first;
        // EvTRel *evt = tr.get_event_tr(event);
        SS er_ev = itm->second->get_er();
//TOBEDONE!!!!!!	  tr.connected_components(er_ev,lcomps);
        //cout << "Number of ERs for " << event << ": " << lcomps.size() << endl;
        if (lcomps.size() > 1)
        {
            split_disconnected_ers(lcomps,minregs,tr, event);
            split = true;
        }
    }
    map<Region *,bool> essential;
    string violation_event; // this is one found by the following function. We dont want to use this for splitting.
    if (not is_excitation_closed(minregs,essential,tr,violation_event))
    {
        list<string> ltrigg;
        tr.trigger_set(v_event,ltrigg);
        if (not ltrigg.empty())
        {
            split = split_trigger_events(ltrigg,minregs,tr,v_event);
        }
        else
        {
//			cout << "event " << v_event << " has no triggers!\n";
        }
    }
    return split;
}


bool split_event(string violation_event, TRel &tr, Encoding &enc)
{
//cout << "Called split event "<< violation_event << endl;
    EvTRel *evt = tr.get_event_tr(violation_event);
    SS er = evt->get_er();
    Region r(er);
    int gmin, gmax;
    r.corner_gradients(violation_event,tr,gmin,gmax);
    int g = int(floor(double(gmin + gmax) / 2.0));
    SS new_er = r.collect_arcs_leaving(violation_event,g,tr);
    if (not new_er.is_empty() and new_er != er)
    {
        tr.split_event_er(violation_event,new_er);
// 		cout << "   splitting leaving " << violation_event << endl;
// 		cout << "   new ER: "; new_er.print(2,2);
        return true;
    }

    SS new_sr = r.collect_arcs_entering(violation_event,g+1,tr);
    if (not new_sr.is_empty() and new_sr < evt->get_sr())
    {
        tr.split_event_sr(violation_event,new_sr);
//		cout << "   splitting entering " << violation_event << endl;
// 		cout << "   new SR: "; new_sr.print(2,2);
        return true;
    }

    /*	SSvector vb(enc.enc_vars());
    	int i;
      for (i = 0; i < enc.enc_vars(); i++) {
            vb[i] = mgr.bddVar(i);
      }
    */
    SS s = er.PickOneMinterm(enc.enc_vars());
    if (not s.is_empty())
    {
        tr.split_event_er(violation_event, s);
//		cout << "    blind splitting " << violation_event << endl;
        return true;
    }

    return false;
}

/*  should implement the less-than sort STL predicate */
inline bool compR (Region *r1, Region *r2)
{
//  return r1->supSize() < r2->supSize();
//	return r1->number_gradients() > r2->number_gradients();
    return r1->Size() > r2->Size();
}


// Generating the regions under demand: computing the necessary regions to guarantee EC for every event
bool generate_minimal_regions_on_demand(TRel &tr, int kmax, int /*explore_regions*/,const SS &state_space, list<Region *> &minregs,
                                        list<string> &violation_events)
{
//cout << "---Generating minimal regions\n";
    list<Region> P;
    hash_map<const SS, bool, hashSS, KeysSS>	cache_init;
    map<string,EvTRel *> tr_map = tr.get_map_trs();
    map<string,EvTRel *>::const_iterator itm;
    for(itm=tr_map.begin(); itm != tr_map.end(); ++itm)
    {
//		if (not itm->second->is_excitation_closed()) {
        SS er = itm->second->get_er();
        if (cache_init.find(er) == cache_init.end())
        {
            P.push_back(Region(er));
            cache_init[er] = true;
        }
        SS sr = itm->second->get_sr();
        if (cache_init.find(sr) == cache_init.end())
        {
            P.push_back(Region(sr));
            cache_init[sr] = true;
        }
//		}
    }

    bool ec = true;
    for(itm=tr_map.begin(); itm != tr_map.end(); ++itm)
    {
        string event = itm->first;
        // look whether excitation closure hold for the event.
        SS er = itm->second->get_er();
        SS states_out_er;
        //cout << "looking EC for " << itm->first << endl;//er.print(); cout << endl;
        if (not is_excitation_closed_event(minregs, event, er,tr, states_out_er))
        {
            queue<Region *> Fset;
            list<Region>::const_iterator itp;
            for(itp = P.begin(); itp != P.end(); ++itp)
            {
                Region r = *itp;
                SS sup_r = r.sup();
                if (sup_r >= er and not(sup_r >= states_out_er))
                {
                    bool set_included = false;
                    list<Region>::const_iterator itf;
                    for (itf = P.begin(); itf != P.end() and not set_included; ++itf)
                    {
                        Region rp =  *itf;
                        SS otherP = rp.sup();
                        if (sup_r > otherP and otherP >= er) set_included = true;
                    }
                    if (not set_included)
                    {

                        Region *rr = new Region(r);
                        /*if (rr->is_region(tr,false)) Fset.push_front(rr);
                        else Fset.push_back(rr);*/
                        Fset.push(rr);
                    }
                }
            }

            if (Fset.empty()) cout << "Error(demand): event effectiveness problem for event "<< event << endl;

            bool ec_event = false;
            int nsets_explored = 0;
            //cout << " ----------------- " <<  event <<  " is not EC. "  << Fset.size()<<endl;
            cache_generate[er] = hash_map<Region *,bool,hashRegion,KeysReg>();
            hash_map<const SS, hash_map<Region *,bool,hashRegion,KeysReg>, hashSS, KeysSS>::iterator it_cache_er = cache_generate.find(er);
            do
            {
                Region *r = Fset.front();


                Fset.pop();
//cout << "|F|:" << Fset.size() << endl;
                ++nsets_explored;

                //memoization
                if (cache_regions.find(r) != cache_regions.end())
                {
                    /*cout << " region found in the cache\n";*/continue;
                }

                if (r->sup() >= states_out_er) continue;

                string violating_event;
//				do {
                violating_event = r->choose_event_non_constant_gradient(tr);
                if (violating_event  != "")
                {

                    Fset.push(r);
                    int gmin, gmax;
                    r->corner_gradients(violating_event,tr,gmin,gmax);
                    int g = int(floor(double(gmin + gmax) / 2.0));

                    Region *r1 = new Region(*r);
                    //			cout << "--  Extending arcs leaving for " << violating_event << " with g= " << g << endl;
                    bool b = r1->extend_arcs_leaving(violating_event,g,kmax,tr);//r1->print();
                    if (not b) delete r1;
                    else
                    {
                        SS r1supp = r1->sup();
                        if ((((*it_cache_er).second)).find(r1) == (((*it_cache_er).second)).end() and
                                not(r1supp >= states_out_er) and (not r1->proper(state_space)) /*and (r1->number_violations(tr,false) < violations_r)*/)
                        {
                            ((*it_cache_er).second).insert(
                                hash_map<Region *,bool,hashRegion,KeysReg>::value_type(r1,true));
                            if (r1->is_region(tr,false) and (cache_regions.find(r1) == cache_regions.end()))
                            {
                                minregs.push_back(r1);
                                update_topset_information(tr,tr_map,r1,states_out_er,event);

                                cache_regions[r1] = r1;
                                if (states_out_er.is_empty())
                                {
                                    ec_event = true;
                                    break;
                                }
                            }
                            else
                            {
                                Fset.push(r1);
                            }
                        }
                        else
                        {
                            delete r1;
                        }
                    }

                    Region *r2 = new Region(*r);
//			cout << "--  Extending arcs entering for " << violating_event << " with g= " << g +1 << endl;
                    b = r2->extend_arcs_entering(violating_event,g+1,kmax,tr);
                    if (not b) delete r2;
                    else
                    {
                        SS r2supp = r2->sup();
                        if (((*it_cache_er).second.find(r2) == (*it_cache_er).second.end()) and
                                not(r2supp >= states_out_er) and (not r2->proper(state_space)) /*and (r2->number_violations(tr,false) < violations_r)*/)
                        {
                            ((*it_cache_er).second).insert(
                                hash_map<Region *,bool,hashRegion,KeysReg>::value_type(r2,true));
                            if (r2->is_region(tr,false) and (cache_regions.find(r2) == cache_regions.end()))
                            {
                                minregs.push_back(r2);
                                update_topset_information(tr,tr_map,r2,states_out_er,event);
                                cache_regions[r2] = r2;
                                if (states_out_er.is_empty())
                                {
                                    ec_event = true;
                                    break;
                                }
                            }
                            else
                            {
                                Fset.push(r2);
                            }
                        }
                        else
                        {
                            delete r2;
                        }
                    }
                }
                else
                {
                    if (r->number_violations(tr,false) != 0) continue;
                    minregs.push_back(r);
                    update_topset_information(tr,tr_map,r,states_out_er,event);
                    if (states_out_er.is_empty())
                        ec_event = true;
                    cache_regions[r] = r;
                }
//				} while (violating_event != "" and not ec_event);
            }
            while (not Fset.empty() and not ec_event);

//cout << "---------------------------- end"  << endl;
            /*			while (ec_event and (not FsetFront.empty()) and (not is_excitation_closed_event(minregs, event, er,tr, states_out_er, mgr))) {
            				minregs.push_back(FsetFront.front());
            				FsetFront.pop_front();
            		}						 */
            if (not ec_event)
            {
                //violation_event = event;
                violation_events.push_back(event);
                ec = false;
            }
            else
            {
                //	cout << " got EC for event " << event << endl;
                tr.change_computed_ec(event, true);
                itm->second->set_excitation_closed(true);
                for (unsigned int i=0; i < Fset.size(); ++i)
                {
                    Region *rr = Fset.front();
                    Fset.pop();
                    delete rr;
                }
            }
//			cout << " multisets found: " <<nsets_explored + Fset.size() << " explored: " <<  nsets_explored << endl;
        }
    }
    return ec;
}


bool generate_minimal_regions_all(TRel &tr, int kmax, const SS &state_space, list<Region *> &minregs,bool mgcond,
                                  bool force_ec_list, map<string,bool> &ec_sigs, list<string> &violation_events)
{

    queue<Region *> P;
    SS tmp = SS(1);
    hash_map<Region*, bool, hashRegion, KeysReg> cache_generate_local;
    map<string,bool> events_ec;
    map<string,SS> states_out_er;
    hash_map<const SS, bool, hashSS, KeysSS>	cache_init;
    map<string,EvTRel *> tr_map = tr.get_map_trs();
    map<string,EvTRel *>::const_iterator itm;
    for(itm=tr_map.begin(); itm != tr_map.end(); ++itm)
    {
        SS er = itm->second->get_er();
        if (cache_init.find(er) == cache_init.end())
        {
            P.push/*_back*/(new Region(er));
            cache_init[er] = true;
        }
        SS sr = itm->second->get_sr();
        if (cache_init.find(sr) == cache_init.end())
        {
            P.push/*_back*/(new Region(sr));
            cache_init[sr] = true;
        }
        events_ec[itm->first] = false;
        states_out_er[itm->first] = SS(1);
    }

    int nevs = tr_map.size();

    while(not P.empty() and ncovered < nevs)
    {
        Region *r = P.front();
        P.pop();
        //memoization
        if (cache_regions.find(r) != cache_regions.end())
        {
            continue;
        }

        // covering cut
        if (not covers_some_uncovered_event(r,tr_map))
        {
            continue;
        }

        // ec cut
        map<string,bool>::iterator ite;
        SS rsup = r->sup();
        bool covers_event_ec = false;
        for(ite = events_ec.begin(); ite != events_ec.end(); ++ite)
        {
            if ((not ite->second) and rsup >= tr_map[ite->first]->get_er() and not(rsup >= states_out_er[ite->first]))
            {
                covers_event_ec = true;
                break;
            }
        }
        if (not covers_event_ec)
        {
            continue;
        }

        string violating_event = r->choose_event_non_constant_gradient(tr);
        if (violating_event  != "")
        {
            int gmin, gmax;
            r->corner_gradients(violating_event,tr,gmin,gmax);
            int g = int(floor(double(gmin + gmax) / 2.0));

            Region *r1 = new Region(*r);
//				cout << "--  Extending arcs leaving for " << event << " with g= " << g << endl;
            bool b = r1->extend_arcs_leaving(violating_event,g,kmax,tr);
            if (not b) delete r1;
            else
            {
                if (b and  (cache_generate_local.find(r1) == cache_generate_local.end()) and not(r1->proper(state_space)))
                {
                    cache_generate_local[r1] = true;
                    if (r1->is_region(tr,false) and (cache_regions.find(r1) == cache_regions.end()))
                    {
                        minregs.push_back(r1);
                        update_topset_information(tr,tr_map,r1,tmp,"");
                        cache_regions[r1] = r1;

                        //updating the information on the ec events;
                        map<string,bool>::iterator ite;
                        for(ite = events_ec.begin(); ite != events_ec.end(); ++ite)
                        {
                            SS erev = tr_map[ite->first]->get_er();
                            if ((not ite->second) and rsup >= erev)
                            {
                                ite->second = is_excitation_closed_event(minregs, ite->first,erev,tr, states_out_er[ite->first]);
                            }
                        }
                    }
                    else
                    {
                        P.push(r1);
                    }
                }
                else delete r1;
            }

            Region *r2 = new Region(*r);
            //				cout << "--  Extending arcs entering for " << event << " with g= " << g +1 << endl;
            b = r2->extend_arcs_entering(violating_event,g+1,kmax,tr);
            if (not b) delete r2;
            else
            {
                if (b and (cache_generate_local.find(r2) == cache_generate_local.end()) and not(r2->proper(state_space)))
                {
                    cache_generate_local[r2] = true;
                    if (r2->is_region(tr,false) and (cache_regions.find(r2) == cache_regions.end()))
                    {
                        minregs.push_back(r2);
                        update_topset_information(tr,tr_map,r2,tmp,"");
                        cache_regions[r2] = r2;

                        //updating the information on the ec events;
                        map<string,bool>::iterator ite;
                        for(ite = events_ec.begin(); ite != events_ec.end(); ++ite)
                        {
                            SS erev = tr_map[ite->first]->get_er();
                            if ((not ite->second) and rsup >= erev)
                            {
                                ite->second = is_excitation_closed_event(minregs, ite->first,erev,tr, states_out_er[ite->first]);
                            }
                        }
                    }
                    else
                    {
                        P.push(r2);
                    }
                }
                else delete r2;
            }
        }
        else
        {
            if ((not mgcond) or (r->is_MG_region(tr)))
            {
                minregs.push_back(r);
                update_topset_information(tr,tr_map,r,tmp,"");
                cache_regions[r] = r;

                //updating the information on the ec events;
                map<string,bool>::iterator ite;
                for(ite = events_ec.begin(); ite != events_ec.end(); ++ite)
                {
                    SS erev = tr_map[ite->first]->get_er();
                    if ((not ite->second) and rsup >= erev)
                    {
                        ite->second = is_excitation_closed_event(minregs, ite->first,erev,tr, states_out_er[ite->first]);
                    }
                }
            }
            else
            {
                // if the set does not satisfies the mg condition, it must be expanded for some event with positive or negative gradient
                bool b = r->extend_on_MG_violation(kmax,tr);
                if (b and not(r->proper(state_space)))
                {
                    P.push(r);
                }
            }
        }
    }


//	cout << "--------------- Removing non-regions (actual size: " << minregs.size() << ")\n";

    /* removing multisets that are not regions */
    /*	list<Region *> tmp_set;
    	list<Region *>::const_iterator itmr;
    	for (itmr = minregs.begin(); itmr != minregs.end(); ++itmr) {
    		Region *rr(*itmr);
    		if (rr->is_region(tr,false)) tmp_set.push_front(rr);
    		else {
    			delete rr;
    		}
    	}*/
//	cout << "minregs: (bef removing min)"<< minregs.size() << endl;

    /*	minregs = tmp_set;
    	tmp_set.clear();	*/

    bool changes;
    do
    {
        changes = false;
        hash_map<Region *,bool, hashRegion, KeysReg> tmp_set;
        list<Region *> todelete;
        list<Region *>::const_iterator itmr;
        for (itmr = minregs.begin(); itmr != minregs.end(); ++itmr)
        {
            Region *r1(*itmr);
            bool rminimal = true;
            list<Region *>::const_iterator itmr2;
            Region r_inside(SS);
            for (itmr2 = minregs.begin(); itmr2 != minregs.end(); ++itmr2)
            {
                if (itmr == itmr2) continue;
                Region *r2(*itmr2);
                if (*r1 > *r2 or *r1 == *r2)
                {
                    rminimal = false;
                }
            }
            if (rminimal) tmp_set[r1] = true;
            else changes = true;
        }

        if (not todelete.empty())
            for (itmr = todelete.begin(); itmr != todelete.end(); ++itmr)  delete *itmr;

        minregs.clear();
        hash_map<Region *,bool, hashRegion, KeysReg>::const_iterator iths;
        for(iths = tmp_set.begin(); iths != tmp_set.end(); ++iths) minregs.push_front(iths->first);
    }
    while (changes);
//	cout << "minregs: (aft removing min)"<< minregs.size() << endl;

    bool general_ec = true;
    if (force_ec_list)
    {
        map<string,bool>::iterator ite;
        for(ite = events_ec.begin(); ite != events_ec.end(); ++ite)
        {
            ite->second = is_excitation_closed_event(minregs, ite->first,tr_map[ite->first]->get_er(),tr, states_out_er[ite->first]);
            if ((not ite->second) and ec_sigs[ite->first])
            {
                violation_events.push_back(ite->first);
                general_ec = false;
            }
        }
    }


    return general_ec;
}

void update_topset_information(TRel &tr, map<string,EvTRel *> &tr_map,Region *r, SS &states_out_er, const string &event)
{
    SS sup_r = r->sup();
    map<string,EvTRel *>::const_iterator itm2;
    for(itm2=tr_map.begin(); itm2 != tr_map.end(); ++itm2)
    {
        SS er_actual = itm2->second->get_er();
        if (sup_r >= er_actual)
        {
            Region *max_topset = new Region(r->compute_max_enabling_topset(er_actual));
            if (not max_topset->sup().is_empty())
            {
                pair<Region*,Region *> p(r,max_topset);
                cache_topsets[itm2->first].push_back(p);
                if (event == itm2->first)
                {
                    if (states_out_er == SS(1))
                    {
                        states_out_er = max_topset->sup() - er_actual;
                    }
                    else
                    {
                        states_out_er *= max_topset->sup();
                    }
                }


// 				cout << "adding a topset to " << itm2->first << endl;
//			itm2->second->add_covering_region(sup_r);
                if (not itm2->second->is_excitation_closed())	tr.change_computed_ec(itm2->first,false);

                // when a maximum cover set is only required
                if (covering_flag.find(itm2->first) == covering_flag.end() and (int)cache_topsets[itm2->first].size() >= MAX_RCOV)
                {
                    ++ncovered;
                    covering_flag[itm2->first] = true;
                    //				cout << "event " << itm2->first << " is covered. Covered: " << ncovered << " total: " << tr_map.size() << endl;
                }
            }
            else delete max_topset;
        }
    }
}

bool covers_some_uncovered_event(Region *r, map<string,EvTRel *> &tr_map)
{

    if (MAX_RCOV == 10000) return true;

    SS sup = r->sup();
    map<string,EvTRel *>::const_iterator ittr;
    for(ittr= tr_map.begin(); ittr != tr_map.end(); ++ittr)
    {
        if (covering_flag.find(ittr->first) == covering_flag.end() and (int)cache_topsets[ittr->first].size() < MAX_RCOV and
                sup >= ittr->second->get_er()) return true;
    }

    return false;
}
/*
void update_cover_information(EvTRel * evt,const list<Region *> &minregs,Cudd &mgr) {
  list<Region*>::const_iterator itmr;
  SS er_actual = evt->get_er();
	for (itmr = minregs.begin(); itmr != minregs.end(); ++itmr) {
		Region *r = *itmr;
		SS rsup = r->sup();
		Region *max_topset = new Region(r->compute_max_enabling_topset(er_actual));
 		if ( rsup >= er_actual and max_topset->sup() != mgr.bddZero()) {
 			evt->add_covering_region(rsup);
 		}
	}
	evt->set_computed_cover();
}
*/
void
compute_smallest_enabling_topsets(list<pair<Region*, Region *> > &result, const list<Region *> &minregs,TRel &tr, string event)
{
    list<Region*>::const_iterator itmr;
    EvTRel *evt = tr.get_event_tr(event);
    if (topset_computed[event])
    {
// 	cout << event << " topsets computed, size: " << cache_topsets[event].size()<< "\n";
        result = cache_topsets[event];
    }
    else
    {
        SS er_event = evt->get_er();
        for (itmr = minregs.begin(); itmr != minregs.end(); ++itmr)
        {
            Region *r = *itmr;
            Region *max_topset = new Region(r->compute_max_enabling_topset(er_event));
            if (r->sup() >= er_event and (not max_topset->sup().is_empty()))
            {
                pair<Region*,Region *> p(r,max_topset);
                result.push_back(p);
            }
            else delete max_topset;
        }
        topset_computed[event] = true;
        cache_topsets[event] = result;
//		cout << event << " (2) topsets computed, size: " << cache_topsets[event].size()<< "\n";

    }
}

bool is_excitation_closed_event(const list<Region *> &minregs, string event,const SS &er,TRel &tr, SS &states_out_er)
{

    states_out_er = SS(1);

    EvTRel *evt = tr.get_event_tr(event);
//	states_out_er = evt->get_intersect_covering() - er;

    if (minregs.empty()) return false;

    if (tr.computed_ec(event))
    {
        //cout << "Called is_ec_event for " << event << " and found in the cache: " << evt->is_excitation_closed() << endl;
        return  evt->is_excitation_closed();
    }

    tr.change_computed_ec(event, true);

    /*
    	if (states_out_er != mgr.bddZero()) {
    		evt->set_excitation_closed(false);
    		return false;
    	}
    cout << "event " << event << " is EC\n";
    	evt->set_excitation_closed(true);
    	return true;
    */

// dead code below, for the time being ...

    list<pair<Region*, Region *> > star_event;
    compute_smallest_enabling_topsets(star_event,minregs,tr,event);
//	cout << event << " :|Topsets| = " << star_event.size()<< endl;
    if (star_event.empty())
    {
        // Violation of the "Event effectiveness" property
//		cout << "(1) Event effectiveness violation: event " << event << endl;
//	evt->set_excitation_closed(false);
        return false;
    }

    tr.change_computed_ec(event, true);
//cout << "computing ec event " << event << endl;

    list<pair<Region*, Region *> >::const_iterator itse;
    SS seintersect(1);
    for(itse = star_event.begin(); itse != star_event.end(); ++itse)
    {
        Region rsup = *((*itse).second);
        SS sup = rsup.sup();
        seintersect *= sup;
    }

    states_out_er = seintersect - er;

    if (er != seintersect)
    {
        // Violation of the "Excitation closure" property
//		cout << "Excitation closure violation: event " << event << endl;
//		seintersect.print(2,2);
        evt->set_excitation_closed(false);
        return false;
    }

    // Remembering the set of regions that guarantee the EC for the event
    /*	witness_ec_events[event].clear();
    	for(itse = star_event.begin(); itse != star_event.end(); ++itse) {
    		witness_ec_events[event][(*itse).first] = true;
    		if (events_covered[(*itse).first].find(event) == events_covered[(*itse).first].end())
    			events_covered[(*itse).first][event] = true;
    	}
    	*/
//cout << "event " << event << " is EC\n";
    evt->set_excitation_closed(true);
    return true;
}


bool is_excitation_closed(const list<Region *> &minregs, map<Region *,bool> &/*essential*/,
                          TRel &tr, string& violation_event)
{
    map<string,EvTRel *> tr_map = tr.get_map_trs();

    map<string,EvTRel *>::const_iterator itm;
    bool exclosure = true;
    list<Region*>::const_iterator itmregs;
    for(itm=tr_map.begin(); itm != tr_map.end(); ++itm)
    {
        string event = itm->first;

        if (tr.computed_ec(event) and itm->second->is_excitation_closed()) continue;

        list<pair<Region*, Region *> > star_event;
        compute_smallest_enabling_topsets(star_event,minregs,tr,event);
        if (star_event.empty())
        {
            // Violation of the "Event effectiveness" property
//			cout << "Event effectiveness violation: event " << event << endl;
            exclosure = false;
            violation_event = event;
//			itm->second->set_excitation_closed(false);
            break;
        }

//cout << "(2) computing ec event " << event << endl;
        tr.change_computed_ec(event, true);

        list<pair<Region*, Region *> >::const_iterator itse;
        SS seintersect(1);
//		cout << "Ktopsets:\n";
        for(itse = star_event.begin(); itse != star_event.end(); ++itse)
        {
            Region rsup = *((*itse).second);
//			rsup.print();
            SS sup = rsup.sup();
//			cout << "Support: ";
//			sup.print(2,2);
            seintersect *= sup;
        }
        if (itm->second->get_er() != seintersect)
        {
            // Violation of the "Excitation closure" property
//			cout << "Excitation closure violation: event " << event << endl;
//			seintersect.print(2,2);
            exclosure = false;
            violation_event = event;
            itm->second->set_excitation_closed(false);
            break;
        }
        itm->second->set_excitation_closed(true);
    }

    return exclosure;
}

void compute_essential(Region *r, const list<Region *> &minregs, map<Region *,bool> &essential,
                       TRel &tr, bool mining_flag)
{
    list<pair<Region*, Region> >::const_iterator itse;
    map<string,bool>::const_iterator itec;
    for(itec = events_covered[r].begin(); itec != events_covered[r].end(); ++itec)
    {
        string event = itec->first;
        list<pair<Region*, Region *> > star_event;
        compute_smallest_enabling_topsets(star_event, minregs,tr,event);
        SS seintersect(1);
        list<pair<Region*, Region *> >::const_iterator itse;
        for(itse = star_event.begin(); itse != star_event.end(); ++itse)
        {
            if ((itse->first == r) or (not essential[itse->first])) continue;
            Region rsup = *(itse->second);
            SS sup = rsup.sup();
            seintersect *= sup;
        }

        if (mining_flag)
        {
            if ((events_covered[r].size() == 1) or (not (seintersect >= r->sup())))
            {
                break;
            }
        }
        else
        {
            if (tr.get_event_tr(event)->get_er() != seintersect)
            {
                break;
            }
        }
    }

    essential[r] = (itec != events_covered[r].end()) or (mining_flag and events_covered[r].begin() == events_covered[r].end());
    //cout << "Region: "; r->print(); cout << "is essential? " << essential[r] << endl;
}

list<Region *> compute_preregions(const list<Region *> &minregs,TRel &/*tr*/, const SS &er)
{
    list<Region *> result;
    list<Region *>::const_iterator itmr;
    for(itmr = minregs.begin(); itmr != minregs.end(); ++itmr)
    {
        Region *r = *itmr;
        if (er <= r->sup()) result.push_back(r);
    }

    return result;
}

void compute_events_covered(const list<Region *> &minregs,TRel &tr)
{
    map<string,EvTRel *>::const_iterator itm;
    map<string,EvTRel *> tr_map = tr.get_map_trs();
    for(itm=tr_map.begin(); itm != tr_map.end(); ++itm)
    {
        string event = itm->first;
        SS er_actual = itm->second->get_er();
        list<Region *>::const_iterator itmr;
        for (itmr = minregs.begin(); itmr != minregs.end(); ++itmr)
        {
            Region *r = *itmr;
            Region *max_topset = new Region(r->compute_max_enabling_topset(er_actual));
            if ( r->sup() >= er_actual and (not max_topset->sup().is_empty()))
            {
                witness_ec_events[event][r] = true;
                events_covered[r][event] = true;
            }
            else delete max_topset;
        }
    }
}

bool essential_arc(const list<Region *> &preregs, map<Region *,bool> &arcs_red,
                   Region *r,EvTRel *evtr)
{

    if (not (r->sup() >= evtr->get_sr())) return true;
    SS er = evtr->get_er();
    SS ec(1);
    list<Region *>::const_iterator itmr;
    for(itmr = preregs.begin(); itmr != preregs.end(); ++itmr)
    {
        Region *rr = *itmr;
        if (r != rr and arcs_red.find(rr) == arcs_red.end()) ec *= rr->sup();
    }
    if (ec == er)
    {
        arcs_red[r] = true;
        return false;
    }
    return true;
}

// this function generates the pn corresponding to the TS tr,
// provided that the excitation closure holds for the set of
// minimal regions obtained.
void pn_synthesis(string outfile,const list<Region *> &minregs,TRel &tr, const SS &initial, int KMAX)
{


    if (outfile != "")
    {
        ofstream out(outfile.c_str());
        int pnumber = 0;
        map<Region *, string> places;
        map<string,int> marking;
        map<string,EvTRel *> events_tr = tr.get_map_trs();
        out << "# number of necessary regions: " << minregs.size() << endl;
        out << ".inputs";
        map<string,EvTRel *>::iterator ittr;
        for(ittr = events_tr.begin(); ittr != events_tr.end(); ++ittr) out << " " << ittr->first;
        out << endl << ".graph" << endl;

//	bool notsafe = false;
        list<Region *>::const_iterator itmr;
        for(itmr = minregs.begin(); itmr != minregs.end(); ++itmr)
        {

            std::ostringstream o;
            if (!(o << pnumber++)) out << "Problems when converting an integer to a string\n";
            places[*itmr] = "p" + o.str();
            if ((*itmr)->cardinality(initial))
            {
                marking[places[*itmr]] = (*itmr)->cardinality(initial);
//				if (marking[places[*itmr]] > 1) notsafe = true;
            }
        }

        // detecting redundant arcs
        map <EvTRel *, map<Region *,bool> > selfloop_red;
        // arcs from places to transitions
        map<string,map<string,bool> > eventtoplaces;
        for(ittr = events_tr.begin(); ittr != events_tr.end(); ++ittr)
        {
            string event = ittr->first;
            SS er_event = tr.get_event_tr(event)->get_er();
            list<Region *> pre_regions = compute_preregions(minregs,tr,er_event);
            bool pre_arc_inserted = false;
            for(itmr = pre_regions.begin(); itmr != pre_regions.end(); ++itmr)
            {
                if (not essential_arc(pre_regions,selfloop_red[tr.get_event_tr(event)],*itmr,tr.get_event_tr(event)))
                {
                    //				out << "event " << event << " is not connected to " << places[*itmr] <<  " " << pre_regions.size() <<endl;
                }
                else
                {
                    pre_arc_inserted = true;
                    Region *r = *itmr;
                    Region max_topset = r->compute_max_enabling_topset(er_event);
                    int k = max_topset.topset_weight();
// 			out << "arc " << places[r] << " -- " << k << " -->" << event << endl;
                    out << places[r] << " " << event;
                    if (k>1) out << "(" << k << ") ";
                    out << endl;
                    int k1,k2;
                    r->corner_gradients(event,tr, k1, k2);
                    if (k1 != k2) r->print();
                    assert(k1==k2);
                    if (k1 > -k)
                    {
//	out << "# arc(II) " << event <<  " k: " << k << " k1: " << k1  << " -->" << places[r]  << endl;
//  r->print();
                        eventtoplaces[event][places[r]] = true;
                        out << event << " " << places[r];
                        if (k+k1>1) out << "(" << k+k1 << ") ";
                        out << endl;
                    }
                }
            }
            // events without pre-regions: this is an option for pn mining
            if (not pre_arc_inserted and (not er_event.is_empty())) out << event << endl;
        }

        // arcs from transitions to places
        for(ittr = events_tr.begin(); ittr != events_tr.end(); ++ittr)
        {
            string event = ittr->first;
            SS er_event = tr.get_event_tr(event)->get_er();
            SS sr_event = tr.get_event_tr(event)->get_sr();
            for(itmr = minregs.begin(); itmr != minregs.end(); ++itmr)
            {
                Region *r = *itmr;
                if (sr_event <= r->sup() and not eventtoplaces[event][places[r]]/*and er_event * r->sup() == mgr.bddZero()*/)
                {
                    int k1,k2;
                    r->corner_gradients(event,tr, k1, k2);
                    assert(k1 == k2);
                    if (k1>0)
                    {
                        //out << "arc " << event <<  " -- " << k1 << " -->" << places[r]  << endl;
                        out << event << " " << places[r];
                        if (k1>1) out << "(" << k1 << ") ";
                        out << endl;
                    }
                }
            }
        }

        if (KMAX > 1)
        {

            out << ".capacity ";
            list<Region *>::const_iterator itmr;
            for(itmr = minregs.begin(); itmr != minregs.end(); ++itmr)
            {
                out << " " << places[*itmr] << "=" << KMAX;
            }
            out <<  endl;
        }

        out << ".marking {";
        map<string,int>::const_iterator itmark;
        for(itmark = marking.begin(); itmark != marking.end(); ++itmark)
        {
            if (itmark->second > 1) out << " " << itmark->first << "=" << itmark->second;
            else out << " " << itmark->first;
        }
        out << " }\n" << ".end" << endl;
        out.close();
    }

}


// this function generates the pn corresponding to the TS tr,
// provided that the excitation closure holds for the set of
// minimal regions obtained.
void pn_synthesis(const list<Region *> &minregs,TRel &tr, const SS &initial, int KMAX)
{


    int pnumber = 0;
    map<Region *, string> places;
    map<string,int> marking;
    map<string,EvTRel *> events_tr = tr.get_map_trs();
    cout << "# number of necessary regions: " << minregs.size() << endl;
    cout << ".outputs";
    map<string,EvTRel *>::iterator ittr;
    for(ittr = events_tr.begin(); ittr != events_tr.end(); ++ittr) cout << " " << ittr->first;
    cout << endl << ".graph" << endl;

//	bool notsafe = false;
    list<Region *>::const_iterator itmr;
    for(itmr = minregs.begin(); itmr != minregs.end(); ++itmr)
    {

        std::ostringstream o;
        if (!(o << pnumber++)) cout << "Problems when converting an integer to a string\n";
        places[*itmr] = "p" + o.str();
        if ((*itmr)->cardinality(initial))
        {
            marking[places[*itmr]] = (*itmr)->cardinality(initial);
//			if (marking[places[*itmr]] > 1) notsafe = true;
        }
    }

    // detecting redundant arcs
    map <EvTRel *, map<Region *,bool> > selfloop_red;
    // arcs from places to transitions
    map<string,map<string,bool> > eventtoplaces;
    for(ittr = events_tr.begin(); ittr != events_tr.end(); ++ittr)
    {
        string event = ittr->first;
        SS er_event = tr.get_event_tr(event)->get_er();
        list<Region *> pre_regions = compute_preregions(minregs,tr,er_event);
        bool pre_arc_inserted = false;
        for(itmr = pre_regions.begin(); itmr != pre_regions.end(); ++itmr)
        {
            if (not essential_arc(pre_regions,selfloop_red[tr.get_event_tr(event)],*itmr,tr.get_event_tr(event)))
            {
                //				cout << "event " << event << " is not connected to " << places[*itmr] <<  " " << pre_regions.size() <<endl;
            }
            else
            {
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
                if (k1 > -k)
                {
//	cout << "# arc(II) " << event <<  " k: " << k << " k1: " << k1  << " -->" << places[r]  << endl;
//  r->print();
                    eventtoplaces[event][places[r]] = true;
                    cout << event << " " << places[r];
                    if (k+k1>1) cout << "(" << k+k1 << ") ";
                    cout << endl;
                }
            }
        }
        // events withcout pre-regions: this is an option for pn mining
        if (not pre_arc_inserted and (not er_event.is_empty())) cout << event << endl;
    }

    // arcs from transitions to places
    for(ittr = events_tr.begin(); ittr != events_tr.end(); ++ittr)
    {
        string event = ittr->first;
        SS er_event = tr.get_event_tr(event)->get_er();
        SS sr_event = tr.get_event_tr(event)->get_sr();
        for(itmr = minregs.begin(); itmr != minregs.end(); ++itmr)
        {
            Region *r = *itmr;
            if (sr_event <= r->sup() and not eventtoplaces[event][places[r]]/*and er_event * r->sup() == mgr.bddZero()*/)
            {
                int k1,k2;
                r->corner_gradients(event,tr, k1, k2);
                assert(k1 == k2);
                if (k1>0)
                {
                    //cout << "arc " << event <<  " -- " << k1 << " -->" << places[r]  << endl;
                    cout << event << " " << places[r];
                    if (k1>1) cout << "(" << k1 << ") ";
                    cout << endl;
                }
            }
        }
    }

    if (KMAX > 1)
    {

        cout << ".capacity ";
        list<Region *>::const_iterator itmr;
        for(itmr = minregs.begin(); itmr != minregs.end(); ++itmr)
        {
            cout << " " << places[*itmr] << "=" << KMAX;
        }
        cout <<  endl;
    }

    cout << ".marking {";
    map<string,int>::const_iterator itmark;
    for(itmark = marking.begin(); itmark != marking.end(); ++itmark)
    {
        if (itmark->second > 1) cout << " " << itmark->first << "=" << itmark->second;
        else cout << " " << itmark->first;
    }
    cout << " }\n" << ".end" << endl;

}


void print_regions(const list<Region *> &minregs)
{
    cout << "______MINIMAL_REGIONS_____________________" << endl;
    list<Region*>::const_iterator itmr;
    for (itmr = minregs.begin(); itmr != minregs.end(); ++itmr)
    {
        cout << "REGION:\n";
        (*itmr)->print();
        cout << endl;
    }
}

/*
void genet_usage()
{


    cout << "   genet [options]  <name_of_transition_system> > <resulting_pn>\n" << endl;

    cout << "   GENERAL OPTIONS (SYNTHESIS & MINING)\n\n";

    cout << "    -k n : to look for region of cardinality at most n\n\n";

    cout << "    -min s: start the search for regions with bound n >= k => s, where n has\n";
    cout << "            been assigned by the -k option . This may speed up the search.\n\n";

    cout << "    -enc [0|1]: encoding of the states. 0: logarithmic encoding,\n";
    cout << "               1:event-based encoding.\n\n";

    cout << "    -prj event_list: projects the initial transition system into the events\n";
    cout << "                    in the list. event_list is a list of events separated\n";
    cout << "                    by commas.\n\n";


    cout << "   PETRI NET MINING\n\n";

    cout << "   -pm: to apply petri net mining (approximation of the excitation closure)\n\n";

    cout << "   -cov n: number of minimal regions to cover an event. This might be\n";
    cout << "           useful when the set of minimal regions is large and therefore one can\n";
    cout << "           weaken the condition to cover an event with few regions.\n\n";

    cout << "   -mg: mining of a marked graph. This is experimental, and not very \n";
    cout << "        debugged option (self-loops still not treated properly).\n\n";

    cout << "   -cc: mining of a set of state machines (1-bounded Petri nets exhibiting no\n";
    cout << "        concurrency) covering the set of events.\n\n";

    cout << "   -rec: hierarchical approach to find a set of views of the log in form of Petri\n";
    cout << "         nets. By using  some causal dependencies between the events, it collects\n";
    cout << "         groups ofevents that are tightly related and projects the log on each group.\n";
    cout << "         This option must be used in combination with the synthesis, mining or \n";
    cout << "         conservative components derivation.\n\n";

    cout << "   -ec event_list: by splitting necessary labels, forces excitation closure to hold\n";
    cout << "                   for the events in the list.\n\n";

    cout << "   Example: genet -k 3 shared532_sg.g > shared532_n3.g\n\n";

    cout << "   or with drawing\n\n";

    cout << "   genet -k 3  shared532_sg.g | draw_astg -nonames | gv -\n\n";

    cout << "   For the time being, the input to genet can only be a transition system\n";
    cout << "   (not another Petri net). To use genet on an already specified Petri net\n";
    cout << "   (e.g. to get a 3 bounded net for a given 1-bounded)\n";
    cout << "   preprocess it with write_sg:\n";

    cout << "       a. write_sg pn1.g > sg1.g\n";
    cout << "       b. genet -k 3 sg1.g > pn2.g\n\n";


}
*/

/////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////// REDUNDANT CODE ///////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////


bool split_trigger_events(const list<string> &ltrigg, const list<Region *> &minregs,TRel &tr,
                          string violation_event)
{
//cout << "   Trying to split labels using triggers" << endl;
    SS er_event = tr.get_event_tr(violation_event)->get_er();
    list<string>::const_iterator itlt;
    bool splitted = false;

    cout << "  1. Trying to split " << violation_event << endl;
    // First trying to split the event by finding a post-region o a trigger
    cout <<"  TRIGS: ";
    for(itlt=ltrigg.begin(); itlt!=ltrigg.end(); ++itlt)
        cout << *itlt << " ";
    cout << endl;

    for(itlt=ltrigg.begin(); itlt!=ltrigg.end() and not splitted; ++itlt)
    {
        string trig_event = *itlt;

        list<Region*>::const_iterator itmr;
        SS sr_trigg_event = tr.get_event_tr(trig_event)->get_sr();
//		cout << "sr for " << trig_event ; sr_trigg_event.print(2,2);
        for (itmr = minregs.begin(); itmr != minregs.end(); ++itmr)
        {
            Region *r = *itmr;
// 			cout << "regio ";r->sup().print(2,2);
            SS new_er_event =  er_event * r->sup();
            if (r->sup() >= sr_trigg_event and (not new_er_event.is_empty()) and
                    new_er_event != er_event)
            {
                cout << "Region used: \n";
                r->print();
                tr.split_event_er(violation_event,new_er_event);
                cout << "    using the event " << trig_event << " to split " << violation_event <<endl;
                cout << "    new er for " << violation_event << ": ";
                new_er_event.print();
                splitted = true;
                break;
            }
        }
    }

    // Second trying to split a trigger by finding a pre-region of the event
    if (not splitted)
    {
        for(itlt=ltrigg.begin(); itlt!=ltrigg.end(); ++itlt)
        {
            string trig_event = *itlt;
            cout << "  2. Trying to split " << trig_event << endl;
            SS sr_trigg_event = tr.get_event_tr(trig_event)->get_sr();
            list<Region*>::const_iterator itmr;
            for (itmr = minregs.begin(); itmr != minregs.end(); ++itmr)
            {
                Region *r = *itmr;
// 				cout << "regio ";r->sup().print(2,2);
                SS er_trig =  tr.get_event_tr(trig_event)->get_er();
                SS new_er = r->project_er_positive_gradient(trig_event,er_event,tr);
//				cout << "new er found for " << trig_event << ": ";new_er.print(2,2);
                if (er_event <= r->sup() and (not r->sup().Intersect(sr_trigg_event).is_empty()) and
                        new_er < er_trig and (not new_er.is_empty()))
                {
                    //				cout << "new ER for " << trig_event << ": ";new_er.print(2,2);
                    tr.split_event_er(trig_event,new_er);
                    cout << "   splitting " << trig_event << endl;
                    splitted = true;
                    break;
                }
            }
        }
    }

    return splitted;
}





