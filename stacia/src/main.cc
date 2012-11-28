/*****************************************************************************\
 Stacia -- Siphon Trap Analysis -- Conquer in Action

 Copyright (c) 2012 Harro Wimmel

 Stacia is free software: you can redistribute it and/or modify it under the
 terms of the GNU Affero General Public License as published by the Free
 Software Foundation, either version 3 of the License, or (at your option)
 any later version.

 Stacia is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for
 more details.

 You should have received a copy of the GNU Affero General Public License
 along with Stacia.  If not, see <http://www.gnu.org/licenses/>.
\*****************************************************************************/

/* <<-- CHANGE START (main program) -->> */
// include header files
#include <config.h>
#include <ctime>
#include <libgen.h>
#include <fstream>
#include <sstream>
#include <string>
#include <climits>

#include <pnapi/pnapi.h>
#include "config-log.h"
#include "cmdline.h"
#include "Output.h"
#include "verbose.h"
#include "types.h"
#include "matchings.h"
#include "subnet.h"
#include "subnetinfo.h"
#include "imatrix.h"
#include "siphontrap.h"
#include "formula.h"
#include "sthread.h"
#include "netdata.h"

// data structures used for multithreading only
extern pthread_mutex_t main_mutex;
extern pthread_cond_t main_cond;
extern bool fsolved,multithreaded,forcequit,texit;
extern set<unsigned int> idleID;
extern unsigned int maxthreads;
extern vector<SThread*> threaddata;

using std::cout;
using std::endl;

/// the command line parameters
gengetopt_args_info args_info;

/// the invocation string
std::string invocation;

/// a file to store a mapping from marking ids to actual Petri net markings
Output* markingoutput = NULL;

/// a variable holding the time of the call
clock_t start_clock = clock();

/// a set containg the IDs of threads that have just completed their tasks
set<unsigned int> tindex;

/// how many (open) siphons a component must have such that divide-and-conquer is stopped
unsigned int siphonthreshold;

/// if we should check for siphons without marked traps or without traps at all
bool unmarkedtraps(true);

/// check if a file exists and can be opened for reading
inline bool fileExists(const std::string& filename) {
    std::ifstream tmp(filename.c_str(), std::ios_base::in);
    return tmp.good();
}


/// evaluate the command line parameters
void evaluateParameters(int argc, char** argv) {
    // overwrite invocation for consistent error messages
    argv[0] = basename(argv[0]);

    // store invocation in a std::string for meta information in file output
    for (int i = 0; i < argc; ++i) {
        invocation += std::string(argv[i]) + " ";
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
            debug_output.stream() << CONFIG_LOG << std::flush;
        }
        message("please send file 'bug.log' to %s!", PACKAGE_BUGREPORT);
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
            status("using configuration file '%s'", args_info.config_arg);
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
                status("using configuration file '%s'", conf_filename.c_str());
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


/// a function collecting calls to organize termination (close files, ...)
void terminationHandler() {
    /* [USER] Add code here */

    // print statistics
    if (args_info.stats_flag) {
        message("runtime: %.2f sec", (static_cast<double>(clock()) - static_cast<double>(start_clock)) / CLOCKS_PER_SEC);
        fprintf(stderr, "%s: memory consumption: ", PACKAGE);
        system((std::string("ps -o rss -o comm | ") + TOOL_GREP + " " + PACKAGE + " | " + TOOL_AWK + " '{ if ($1 > max) max = $1 } END { print max \" KB\" }' 1>&2").c_str());
    }
}

/// main-function
int main(int argc, char** argv) {
    time_t start_time, end_time;

    // set the function to call on normal termination
    atexit(terminationHandler);

    /*--------------------------------------.
    | 0. parse the command line parameters  |
    `--------------------------------------*/
    evaluateParameters(argc, argv);
    Output::setTempfileTemplate(args_info.tmpfile_arg);
    Output::setKeepTempfiles(args_info.noClean_flag);


    /*----------------------.
    | 1. parse the open net |
    `----------------------*/
    pnapi::PetriNet net;

    try {
        // parse either from standard input or from a given file
        if (args_info.inputs_num == 0) {
            // parse the open net from standard input
            if (args_info.pnml_given) std::cin >> pnapi::io::pnml >> net;
            else if (args_info.lola_given) std::cin >> pnapi::io::lola >> net;
            else std::cin >> pnapi::io::owfn >> net;
        } else {
            // strip suffix from input filename (if necessary: uncomment next line for future use)
            //inputFilename = std::string(args_info.inputs[0]).substr(0, std::string(args_info.inputs[0]).find_last_of("."));

            // open input file as an input file stream
            std::ifstream inputStream(args_info.inputs[0]);
            // ... and abort, if an error occurs
            if (!inputStream) {
                abort(1, "could not open file '%s'", args_info.inputs[0]);
            }

            // parse the open net from the input file stream
			if (args_info.pnml_given)
	            inputStream >> meta(pnapi::io::INPUTFILE, args_info.inputs[0])
    	                    >> pnapi::io::pnml >> net;
			else if (args_info.lola_given)
	            inputStream >> meta(pnapi::io::INPUTFILE, args_info.inputs[0])
    	                    >> pnapi::io::lola >> net;
			else 
	            inputStream >> meta(pnapi::io::INPUTFILE, args_info.inputs[0])
    	                    >> pnapi::io::owfn >> net;
        }
        // additional output if verbose-flag is set
        if (args_info.verbose_flag) {
            std::ostringstream s;
            s << pnapi::io::stat << net;
            status("read net: %s", s.str().c_str());
        }
    } catch (pnapi::exception::InputError error) {
        std::ostringstream s;
        s << error;
        abort(2, "\b%s", s.str().c_str());
    }

    /*-----------------------------------------------------------.
    | 2. add a loop transition for nets with start and end place |
    `-----------------------------------------------------------*/

	if (args_info.loopnet_flag) {
		status("creating loop transition");
		pnapi::Transition& t(net.createTransition("(loop)"));
		const set<pnapi::Place*>& pset(net.getPlaces()); 
		set<pnapi::Place*>::const_iterator pit;
		for(pit=pset.begin(); pit!=pset.end(); ++pit)
		{
			if ((*pit)->getPreset().empty()) net.createArc(t,**pit);
			if ((*pit)->getPostset().empty()) net.createArc(**pit,t);
		}
	}

    /*---------------------.
    | 3. interpret options |
    `---------------------*/

	// capture verbose & debug flags
	bool verbose(args_info.verbose_flag);
	bool debug(args_info.debug_flag);

	if (args_info.notraps_flag) unmarkedtraps=false;

	// check flag for full computation even in the case of a found siphon without marked trap
	if (args_info.noearlyquit_flag) forcequit=false;

	// get the minimal size of pre+postset for a place to be split
	unsigned int psplit(0);
	if (args_info.placesplit_given) psplit=args_info.placesplit_arg;

	unsigned int threshold(8);
	if (args_info.goaloriented_given) threshold=args_info.goaloriented_arg;

	unsigned int maxinterfacesize(UINT_MAX);
	if (args_info.maxinterfacesize_given) maxinterfacesize=args_info.maxinterfacesize_arg;

	siphonthreshold = UINT_MAX;
	if (args_info.siphonthreshold_given) siphonthreshold=args_info.siphonthreshold_arg;

	if (args_info.dac_flag) { psplit=3; threshold=7; }
	if (args_info.combined_flag) { psplit=3; threshold=7; siphonthreshold=25; }

	unsigned int algorithm(2);
	if (args_info.algorithm_given) algorithm = args_info.algorithm_arg;

    /*----------------------------------------.
    | 4. split places with large pre/postsets |
    `----------------------------------------*/

	IMatrix im1(net);
	vector<unsigned int> sp(im1.placesToSplit(psplit));
	if (!sp.empty()) status("splitting places with pre+postset larger than %d",(psplit>2?psplit-1:2));
	if (debug) {
		cout << "Places to split: ";
		for(unsigned int i=0; i<sp.size(); ++i) cout << im1.pName(sp[i]) << " ";
		cout << endl;
	}
	IMatrix im(im1,sp);
	SubNet sn(im);
	if (args_info.shownet_flag) {
		if (args_info.loopnet_flag || !sp.empty()) status("Modified Input Net:");
		else status("Input Net:");
		sn.printNet();
		cout << endl;
	}

    /*---------------------------.
    | 5. brute force SAT-solving |
    `---------------------------*/

	// The component size at which the divide-and-conquer algorithm should begin.
	// Below this size no information about siphons and traps is computed,
	// components are just unified. At this size, siphons and traps are computed
	// with a direct approach. Above this size, information about siphons and
	// traps are computed from the according information of the contained components.
	unsigned int csize(0);
	if (args_info.componentsize_given) {
		csize = args_info.componentsize_arg;
		if (csize==0) csize = im.numTransitions();
	}
	if (args_info.bruteforce_flag) csize = im.numTransitions();
	if (args_info.dac_flag || args_info.combined_flag) csize = 1;
	if (debug) cout << "Threshold component size: " << csize << endl;

	// use brute force if we have only one component
	if (csize==im.numTransitions()) 
	{
			status("using brute force SAT-solving");
			clock_t startc = clock();
			Formula fo3(sn,unmarkedtraps);
//cout << "Formula:" << endl; fo3.print(); cout << endl;
			// if we find such a siphon, we might quit the program
//if (!fo3.check(false)) cout << args_info.inputs[0] << endl;
			if (fo3.check(false)) { cout << "The net contains a siphon without " << (unmarkedtraps ? "marked" : "any") << " trap:" << endl;
									im.printSiphon(fo3.getSiphon());
			} else cout << "All siphons contain a" << (unmarkedtraps?"n initially marked":"") << " trap." << endl;
			clock_t endc = clock();
			cout << "Time: " << ((double)(endc-startc))/CLOCKS_PER_SEC << " secs." << endl;
			return EXIT_SUCCESS;
	}

    /*----------------------------------.
    | 6. divide the net into components |
    `----------------------------------*/

	clock_t startc = clock();

	// to memorize all components:
	vector<NetData*> netdata;
	// compute the components around each transition t
	status("computing net components");
	for(unsigned int t=0; t<im1.numTransitions(); ++t)
	{
		// get all transitions belonging to the same component
		vector<TransitionID> ct(im.componentTransitions(t));
		// make sure the new component is disjoint to every component found so far
		// non-disjoint components are identical
		unsigned int nd;
		for(nd=0; nd<netdata.size(); ++nd)
			if (netdata[nd]->getSubNet().isTransition(ct.at(0))) break;
		if (nd==netdata.size()) {
			if (debug) {
				cout << "Component " << nd << ": ";
				for(unsigned int i=0; i<ct.size(); ++i) cout << im.tName(ct[i]) << " ";
				cout << endl;
			}
			// for a new component, create a subnet and store it
			netdata.push_back(new NetData(im,ct));
			// create info on the size of interfaces of combined components
			netdata.back()->setIndex(netdata.size()-1);
			for(unsigned int j=0; j<netdata.size()-1; ++j)
			{
				netdata[j]->addPartner(netdata.back());
				netdata.back()->addPartner(netdata[j]);
			}
			if (debug) {
				netdata.back()->getSubNet().printNet();
				cout << endl;
			}
		}
	}
	status("found %d components",netdata.size());

    /*----------------------------------------.
    | 7. plan the recombination of components |
    `----------------------------------------*/

	bool bruteforce(false); // go back to brute force solving (instead of divide and conquer)
	unsigned int minis(0);
	int mindiff(0);
	NetData *minp,*comb;
	if (netdata.size()>1) do {
		minp = NULL;
		comb = NULL;
		if (debug) {
			cout << "Partners: ";
			for(unsigned int t=0; t<netdata.size(); ++t) 
				if (!netdata[t]->isUsed() && netdata[t]->getMinPartner()) 
					cout << "(" << t << "," << netdata[t]->getMinPartner()->getIndex() << "):" << netdata[t]->getMinInterfaceSize() << " ";
			cout << endl;
		}

		if (algorithm>3) {
			unsigned int k,kmax(0);
			vector<NetData*> todo;
			set<NetData*> done;
			for(k=0; k<netdata.size(); ++k)
				if (!netdata[k]->isUsed() && netdata[k]->getMinInterfaceSize()>=kmax) {
					minp = netdata[k];
					kmax = netdata[k]->getMinInterfaceSize();
				}
			todo.push_back(minp);
			if (debug) cout << "[chk] " << minp->getIndex() << endl; 
			kmax=0;
			while (kmax<todo.size())
			{
				NetData* next(todo[kmax]);
				if (done.find(next)!=done.end()) { ++kmax; continue; }
				done.insert(next);
				if (next->getMinInterfaceSize()<threshold) { minp=NULL; break; }
				next->traversePartners();
				do {
					unsigned int cisize,mindiff;
					minp = next->nextPartner(cisize);
					if (!minp || minp->isUsed()) continue;
					if (kmax==0 && done.find(minp)==done.end()) todo.push_back(minp);
					Interface in(next->getSubNet().combinedInterface(minp->getSubNet()));
					minp->traversePartners();
					comb = NULL;
					NetData* nx2;
					do {
						unsigned int cis2;
						nx2 = minp->nextPartner(cis2);
						if (!nx2 || nx2->isUsed() || nx2==next || nx2->getSubNet().combinedInterfaceSize(minp->getSubNet())>=threshold) continue;
						int diff(nx2->getSubNet().diffInterface(in));
						if (!comb || mindiff>diff) {
							comb = nx2;
							mindiff = diff;
						}
					} while (nx2);
					if (comb && mindiff<0) break;
				} while (minp);
				if (minp) break;
			}
			if (debug && minp) cout << "[" << todo[kmax]->getIndex() << "] sel (" << minp->getIndex() << "," << comb->getIndex() << "):" << mindiff << endl;
		}
		if (algorithm==2 || algorithm==4) {
			if (!minp) {
				unsigned int choice;
				comb = NULL;
				for(unsigned int t=0; t<netdata.size(); ++t)
				{
					if (netdata[t]->isUsed()) continue;
					netdata[t]->traversePartners();
					NetData* nx2;
					do {
						unsigned int cis2;
						nx2 = netdata[t]->nextPartner(cis2);
						if (!nx2 || nx2->isUsed() || nx2==netdata[t] || cis2>=threshold) continue;
						unsigned int iis(nx2->getSubNet().innerInterfaceSize(netdata[t]->getSubNet()));
						if (!comb || minis<iis) {
							comb = nx2;
							minis = iis;
							choice = t;
						}
					} while (nx2);
				}
				if (comb) minp = netdata[choice];
				if (debug && minp) cout << "[inn] sel (" << minp->getIndex() << "," << comb->getIndex() << "):" << minis << endl;
			}
		}
		if (algorithm>0) {
			if (!minp) {
				comb = NULL;
				for(unsigned int t=0; t+1<netdata.size(); ++t)
				{
					if (netdata[t]->isUsed()) continue;
					if (minp==NULL || minis>netdata[t]->getMinInterfaceSize()) {
						minis = netdata[t]->getMinInterfaceSize();
						minp = netdata[t];
					}
				}
				if (debug && minp) cout << "[def] sel (" << minp->getIndex() << "," << minp->getMinPartner()->getIndex() << "):" << minis << endl;
			}
		} else {
			unsigned int t;
			for(t=0; t+1<netdata.size(); ++t)
			{
				if (netdata[t]->isUsed() || netdata[t]->getMinInterfaceSize()<threshold
						|| netdata[t]->getMinPartner()->getMinInterfaceSize()>=threshold) continue;
				Interface in(netdata[t]->getSubNet().combinedInterface(netdata[t]->getMinPartner()->getSubNet()));
				minp = NULL;
				comb = NULL;
				for(unsigned int k=0; k<netdata.size(); ++k)
				{
					if (netdata[k]->isUsed() || k==t || k==netdata[t]->getMinPartner()->getIndex()) continue;
					if (netdata[k]->getSubNet().combinedInterfaceSize(netdata[t]->getMinPartner()->getSubNet())>=threshold) continue;
					int diff(netdata[k]->getSubNet().diffInterface(in));
					if (minp==NULL || mindiff>diff) {
						minp = netdata[k];
						mindiff = diff;
						comb = netdata[t]->getMinPartner();
					}
				}
				if (debug) cout << "[" << t << "] sel (" << minp->getIndex() << "," << comb->getIndex() << "):" << mindiff << endl;
				if (minp && mindiff<0 && minp->getSubNet().combinedInterfaceSize(comb->getSubNet())<threshold) break;
			}
			if (t+1==netdata.size()) {
				minp = NULL;
				comb = NULL;
				for(t=0; t+1<netdata.size(); ++t)
				{
					if (netdata[t]->isUsed()) continue;
					if (minp==NULL || minis>netdata[t]->getMinInterfaceSize()) {
						minis = netdata[t]->getMinInterfaceSize();
						minp = netdata[t];
					}
				}
				if (debug && minp) cout << "[def] sel (" << minp->getIndex() << "," << minp->getMinPartner()->getIndex() << "):" << minis << endl;
			}
		}
		if (minp) {
			if (comb) minis = minp->getSubNet().combinedInterfaceSize(comb->getSubNet());
			if (debug) cout << "use (" << minp->getIndex() << "," << (comb?comb:minp->getMinPartner())->getIndex() << "):" << minis << endl;
			netdata.push_back(new NetData(minp,(comb?comb:minp->getMinPartner()),false));
			netdata.back()->setIndex(netdata.size()-1);
			for(unsigned int j=0; j<netdata.size()-1; ++j)
			{
				netdata[j]->checkPartner();
				netdata[j]->addPartner(netdata.back());
				netdata.back()->addPartner(netdata[j]);
			}
			if (minis>=maxinterfacesize) { bruteforce=true; break; }
		}
	} while (minp);
	
    /*-------------------------------------------------.
    | 8. resort to brute force if interfaces are large |
    `-------------------------------------------------*/

	if (bruteforce) {
			status("selected algorithm: brute force SAT-solving");
			Formula fo(sn,unmarkedtraps);
			// if we find such a siphon, we might quit the program
			if (fo.check(false)) { cout << "The net contains a siphon without " << (unmarkedtraps ? "marked" : "any") << " trap." << endl;
									im.printSiphon(fo.getSiphon());
			} else cout << "All siphons contain a" << (unmarkedtraps?"n initially marked":"") << " trap." << endl;
			clock_t endc = clock();
			cout << "Time: " << ((double)(endc-startc))/CLOCKS_PER_SEC << " secs." << endl;
			for(unsigned int i=0; i<netdata.size(); ++i)
				delete netdata[i];
			return EXIT_SUCCESS;
	}

    /*-------------------------------------------------.
    | 9. the sequential conquer phase of the algorithm |
    `-------------------------------------------------*/

	/// if one of the formulae is satisfiable, this will contain the witness siphon
	set<PlaceID> witness;

	status("selected algorithm: divide and conquer");

	if (!args_info.threads_given) {
		status("computing component info");
		unsigned int pos(0),stpos(0);
		while (pos<netdata.size())
		{
			// check if this component has already been done
			if (netdata[pos]->isDone()) { 
				if (pos++==stpos) ++stpos; 
				if (pos==netdata.size()) pos=stpos;
				continue; 
			}
			if (netdata[pos]->isComputed()) {
				netdata[pos]->setDone();
				continue;
			}

			// compute siphons/traps for this component brute force, if there is no info yet
			if (netdata[pos]->getChild(true)==NULL) {
				// but first check if there is an internal siphon without marked trap first
				Formula fo(netdata[pos]->getSubNet(),unmarkedtraps);
				// if we find such a siphon, we might quit the program
				if (fo.check(false)) { 
					witness = fo.getSiphon();
					fsolved=true; 
					if (debug) {
						cout << "Formula true" << endl; 
						fo.printResult(); 
					}
					if (forcequit) break;
				} else if (debug) cout << "Formula false" << endl;

				// otherwise compute the siphons/traps explicitly
				SiphonTrap* st(new SiphonTrap(netdata[pos]->getSubNet()));
				netdata[pos]->setSiphonTrap(st);
				st->computeBruteForce(unmarkedtraps);
				// then obtain the necessary info about the subnet from its siphons and traps
				if (netdata[pos]->getSubNetInfo()->computeComponentInfo(*st)) break;
				if (debug) netdata[pos]->getSubNetInfo()->printInfo();
				netdata[pos]->setComputed();
				continue;
			}

			// this component should be derived from two subcomponents
			// first, compute the matchings of the subcomponents
			Matchings* m(new Matchings(*(netdata[pos]->getChild(false)->getSubNetInfo()),*(netdata[pos]->getChild(true)->getSubNetInfo()),unmarkedtraps));
			netdata[pos]->setMatchings(m);

			if (debug) {
				cout << "Siphon Matchings: ";
				netdata[pos]->printSiphonMatchings();
				cout << endl << "Interface-Trap Matchings: ";
				m->printMatchings(Matchings::ITRAP);
				cout << endl << "Token-Trap Matchings: ";
				m->printMatchings(Matchings::TTRAP);
				cout << endl;
			}

			// compute the siphon/trap formula from the matchings
			Formula f(*m,unmarkedtraps);
			// and check it. If true, we have a siphon without marked trap and may quit
			if (f.check(false)) { 
//f.print();
				witness = netdata[pos]->getSiphon(f.getMatching());
				fsolved=true; 
				if (debug) {
					cout << "SwomT found" << endl; 
					f.printResult(); 
				}
				if (forcequit) break;
			} else if (debug) cout << "No SwomT found" << endl;

			// otherwise we must compute the new info for the combined subnets
			// from the info of the selected subnets (conquer part!)
			if (netdata[pos]->getSubNetInfo()->computeComponentInfo(*m,unmarkedtraps)) break;
			if (debug) netdata[pos]->getSubNetInfo()->printInfo();
			netdata[pos]->setComputed();
		}


		// after a break due to too many siphons, we revert to brute force
		if (!fsolved && pos<netdata.size()) {
			status("reverting to brute force SAT-solving");
			Formula fo2(sn,unmarkedtraps);
			if (fo2.check(false)) { cout << "The net contains a siphon without " << (unmarkedtraps ? "marked" : "any") << " trap:" << endl;
									witness = fo2.getSiphon();
			} else cout << "All siphons contain a" << (unmarkedtraps?"n initially marked":"") << " trap." << endl;
			bruteforce = true;
		}

	}

    /*------------------------.
    | 9. start multithreading |
    `------------------------*/

	else {
		initThreadData(args_info.threads_given ? args_info.threads_arg : 0);
		if (maxthreads>0) status("starting %d threads",maxthreads);
		startThreads();

    /*---------------------------.
    | 10. threaded conquer phase |
    `---------------------------*/

		status("computing component info using threads");
		unsigned int pos(0),openpos(netdata.size());
		bool waitnow(true);
		while (pos<netdata.size() || openpos<netdata.size() || idleID.size()<maxthreads)
		{
			if (texit) break;
			// check if there are an idle thread and a processable subnet to continue, wait otherwise
			pthread_mutex_lock(&main_mutex);
			while ( !(fsolved && forcequit) && tindex.empty() && (idleID.empty() || (pos==netdata.size()&&waitnow)) )
	 				pthread_cond_wait(&main_cond,&main_mutex);
	
			// if a thread has completed a job, mark the related subnet as done
			if (!tindex.empty()) { // contains all thread IDs with just completed jobs
				waitnow = false;
				set<unsigned int>::iterator it;
				for(it=tindex.begin(); it!=tindex.end(); ++it)
				{
					netdata[threaddata[*it]->job]->setDone(); // mark the subnet as done
					// memorise the structures constructed by the thread, then let the thread forget
					if (threaddata[*it]->m) { netdata[threaddata[*it]->job]->setMatchings(threaddata[*it]->m); threaddata[*it]->m = NULL; }
					if (threaddata[*it]->st) { netdata[threaddata[*it]->job]->setSiphonTrap(threaddata[*it]->st); threaddata[*it]->st = NULL; }
					// construct a witness from the thread data if one exists
					if (threaddata[*it]->solved) {
						if (!threaddata[*it]->s.empty()) witness = threaddata[*it]->s;
						else witness = netdata[threaddata[*it]->job]->getSiphon(threaddata[*it]->wmat);
					}
					idleID.insert(*it);
					if (debug) cout << "Info complete for component " << threaddata[*it]->job << endl;
				}
				tindex.clear();
			}
	
			// check if some thread found a siphon without a marked trap;
			// if we are not forced to make a complete run through all subnets, we can quit now
			if (fsolved && forcequit) {
				pthread_mutex_unlock(&main_mutex);
				break;
			}
			pthread_mutex_unlock(&main_mutex);
	
			// if we were waiting for a processible subnet, check now at the earliest possible index for that subnet
			if (pos==netdata.size()) {
				waitnow = true;
				pos = openpos;
				openpos = netdata.size();
			}
	
			// find the next processable subnet
			while (pos<netdata.size() && netdata[pos]->isScheduled()) ++pos;
			if (pos==netdata.size()) continue;
	
			// check which subnets to combine (if any)
			NetData* child1(netdata[pos]->getChild(false));
			NetData* child2(netdata[pos]->getChild(true));
	
			// if this is a base job (no component info yet) we compute the siphons&traps directly
			if (!child1) {
				// grab a thread to do the job
				unsigned int thrnr(*(idleID.begin()));
				idleID.erase(thrnr);
				if (debug) cout << " calculating base info " << pos << " on thread " << thrnr << endl;
				// copy the necessary data to the thread and start it
				initThread(thrnr,pos,netdata[pos]->getSubNetPtr(),netdata[pos]->getSubNetInfo());
				netdata[pos]->setScheduled();
				++pos;
				continue;
			}
	
			// if the subnet cannot be processed yet (processing of a child is still in progress)
			if (!child1->isDone() || !child2->isDone()) {
				if (openpos==netdata.size()) openpos = pos;
				++pos;
				continue;
			}
	
			// grab a thread to do the job
			unsigned int thrnr(*(idleID.begin()));
			idleID.erase(thrnr);
			if (debug) cout << " combine " << child1->getIndex() << "+" << child2->getIndex() << ": init thread " << thrnr << endl;
			// and transfer the job to it
			initThread(thrnr,pos,child1->getSubNetInfo(),child2->getSubNetInfo(),netdata[pos]->getSubNetInfo());
			netdata[pos]->setScheduled();
			++pos;
		}

		// after a break due to too many siphons, we revert to brute force
		if (texit && !fsolved) {
			status("reverting to brute force SAT-solving");
			Formula fo(sn,unmarkedtraps);
			if (fo.check(false)) { cout << "The net contains a siphon without " << (unmarkedtraps ? "marked" : "any") << " trap:" << endl;
									im.printSiphon(fo.getSiphon());
			} else cout << "All siphons contain a" << (unmarkedtraps?"n initially marked":"") << " trap." << endl;
			bruteforce = true;
		}

    /*-----------------------.
    | 11. end multithreading |
    `-----------------------*/

		// clean up, end threads
		makeThreadsIdle();
		waitForAllIdle();
		stopThreads();
		destroyThreadData();
	}

    /*------------------------------.
    | 12. clean up and print result |
    `------------------------------*/

	if (!bruteforce) {
		if (fsolved) { cout << "The net contains a siphon without " << (unmarkedtraps ? "marked" : "any") << " trap:" << endl;
						im.printSiphon(witness);
		} else cout << "All siphons contain a" << (unmarkedtraps?"n initially marked":"") << " trap." << endl;
	}
	clock_t endc = clock();
	cout << "Time: " << ((double)(endc-startc))/CLOCKS_PER_SEC << " secs." << endl;
	status("clean up");

	for(unsigned int i=0; i<netdata.size(); ++i)
		delete netdata[i];

    return EXIT_SUCCESS;
}
/* <<-- CHANGE END -->> */
