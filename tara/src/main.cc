/*****************************************************************************\
 Hello -- <<-- Hello World -->>

 Copyright (c) <<-- 20XX Author1, Author2, ... -->>

 Hello is free software: you can redistribute it and/or modify it under the
 terms of the GNU Affero General Public License as published by the Free
 Software Foundation, either version 3 of the License, or (at your option)
 any later version.

 Hello is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for
 more details.

 You should have received a copy of the GNU Affero General Public License
 along with Hello.  If not, see <http://www.gnu.org/licenses/>.
\*****************************************************************************/


/* <<-- CHANGE START (main program) -->> */
// include header files
#include <config.h>
#include <ctime>
#include <libgen.h>
#include <fstream>
#include <sstream>
#include <string>

#include <pnapi/pnapi.h>
#include "config-log.h"
#include "cmdline.h"
#include "Output.h"
#include "verbose.h"
#include "Tara.h"
#include "Usecase.h"
#include "MaxCost.h"
#include "ServiceTools.h"
#include "Modification.h"
#include "iModification.h"
#include "CCSearch.h"
#include "Risk.h"
#include "Reset.h"

using std::cerr;
using std::cout;
using std::endl;
using std::map;
using std::ofstream;

/// a variable holding the time of the call
clock_t start_clock = clock();

/// a function collecting calls to organize termination (close files, ...)
void terminationHandler() {
    /* [USER] Add code here */

    // print statistics
    if(Tara::args_info.stats_flag) {
        message("runtime: %.2f sec", (static_cast<double>(clock()) - static_cast<double>(start_clock)) / CLOCKS_PER_SEC);
        fprintf(stderr, "%s: memory consumption: ", PACKAGE);
        int i=system((std::string("ps -o rss -o comm | ") + TOOL_GREP + " " + PACKAGE + " | " + TOOL_AWK + " '{ if ($1 > max) max = $1 } END { print max \" KB\" }' 1>&2").c_str());
    }
}


/// main-function
int main(int argc, char** argv) {
    
    /*--------------------------------------.
    | 0. parse the command line parameters  |
    `--------------------------------------*/
    Tara::evaluateParameters(argc, argv);

    //TODO: reorganize temp file stuff
    Output::setTempfileTemplate(Tara::args_info.tmpfile_arg);
    Output::setKeepTempfiles(Tara::args_info.noClean_flag);

    // set the function to call on normal termination
    atexit(terminationHandler);
    /*----------------------------.
    | 1. parse the open Tara::net |
    `----------------------------*/

    // Parsing the open Tara::net, using the PNAPI
	status("Processing %s", Tara::args_info.net_arg);
	Tara::net = new pnapi::PetriNet;

	try {

		// parse either from standard input or from a given file
		std::ifstream inputStream;
		inputStream.open(Tara::args_info.net_arg);
		inputStream >> pnapi::io::owfn >> *(Tara::net);
		inputStream.close();

		/*maybe only with verbose-flag?*/
		std::stringstream pnstats;
		pnstats << pnapi::io::stat << *(Tara::net);

		status("read Tara::net %s", pnstats.str().c_str());
	} catch (pnapi::exception::InputError error) {
		std::stringstream inputerror;
		inputerror << error;
		abort(3, "pnapi error %s", inputerror.str().c_str());
	}

    /*-------------------------------------.
    | 2. Parse Costfunction to partial map |
    \-------------------------------------*/

    message("Step 2: Parse the cost function from '%s' and apply it to the built statespace", Tara::args_info.costfunction_arg);

    status("parsing costfunction");
    if (strcmp(Tara::args_info.costfunction_arg, "-r") != 0) {
	    Parser::costfunction.parse(Tara::args_info.costfunction_arg);
    }
    else {  // if costfunction should be random
       status("generating random costfunction");
       const std::set<pnapi::Transition*> transitions=Tara::net->getTransitions();
   
       //get argument
       unsigned int min=(Tara::args_info.minrandomcost_given)?Tara::args_info.minrandomcost_arg:0;
       unsigned int mod=(Tara::args_info.maxrandomcost_given)?Tara::args_info.maxrandomcost_arg-min+1:101-min;
       unsigned int cur=0;

       //seed rand
       bool seedGiven=Tara::args_info.randomseed_given;
       unsigned int seed;
       if(seedGiven)
	       seed=(unsigned int) Tara::args_info.randomseed_arg;
       else
	       seed=time(NULL);

       srand(seed);

       for(std::set<pnapi::Transition*>::iterator it=transitions.begin();it!=transitions.end();++it) {
            //get next rand and pass it to partial cost function
            cur=min+(rand() % mod);
            Tara::partialCostFunction[*it]= cur; 
            if(cur>Tara::highestTransitionCosts) Tara::highestTransitionCosts=cur;
            status("random costfunction:  %s -> %d", (*it)->getName().c_str(), cur);
       }
    }
    if (Tara::args_info.riskcosts_given) {
        status("risk costs given with base %d", Tara::args_info.riskcosts_given);
        transformRiskCosts(& Tara::partialCostFunction, Tara::args_info.riskcosts_arg);
    }
    if (Tara::resetMap.size() > 0) {
        status("transforming %d Reset- Transitions", Tara::resetMap.size());
        Reset::transformNet();
    }

    /*----------------------------------.
    | 2. get most permissive Partner MP |
    `----------------------------------*/

    computeMP(*Tara::net, Tara::tempFile.name());
     
    // first create automaton partner
    pnapi::Automaton partner;
    std::ifstream partnerStream;

    //stream automaton
    partnerStream.open(Tara::tempFile.name().c_str(), std::ifstream::in);
    if(!partnerStream) {
        message("net is not controllable. Exit.");
        exit(EXIT_FAILURE);
    }

    partnerStream >> pnapi::io::sa >> partner;
    
    // convert to petri net
    pnapi::PetriNet composition(partner);

    /*---------------------.
    | 3. set modification  |
    `---------------------*/

    // set to default
    Tara::modification = new iModification(Tara::net);

    if(Tara::args_info.usecase_given) {

        // first parse usecase
        pnapi::PetriNet* usecase = new pnapi::PetriNet();

        try {
            // parse from a given file
            std::ifstream usecaseStream;
            usecaseStream.open(Tara::args_info.usecase_arg);
            usecaseStream >> pnapi::io::owfn >> *(usecase);
            usecaseStream.close();

            /*maybe only with verbose-flag?*/
            //std::stringstream pnstats;
            //pnstats << pnapi::io::stat << *(usecase);

        } catch (pnapi::exception::InputError error) {
		    std::stringstream inputerror;
    		inputerror << error;
	    	abort(3, "pnapi error %s", inputerror.str().c_str());
	    }
        // overwrite the modification
        // // TODO: Usecase modification cannot yet handle this
        delete Tara::modification;
        Tara::modification = new Usecase(Tara::net, usecase, &Tara::partialCostFunction, 0);
        //
        // TEMP: create dummy that modification works
        // Modification* dummy = new Usecase(Tara::net, usecase, &Tara::partialCostFunction, 0);
    }

    /*----------------------.
    | 5. Compute cost bound |
    `----------------------*/
    
    // compose
    try {
        composition.compose(*Tara::net, "mpp-", "");
	} catch (pnapi::exception::Error error) {
		std::stringstream inputerror;
		inputerror << error;
		abort(3, "pnapi error %s", inputerror.str().c_str());
	} 

    /*--------------------------.
    | 5.1. call lola with n+mp  |
    `--------------------------*/
    message("Step 2: Build the state space of '%s' and its most-permissive partner", Tara::args_info.net_arg);    

    // run lola-statespace from the service tools
    getLolaStatespace(composition,Tara::tempFile.name());

    /*--------------------------.
    | 5.2 Parse the inner Graph |
    \--------------------------*/
    status("parsing inner graph");
    Parser::lola.parse(Tara::tempFile.name().c_str()); 

    /*--------------------------------------------.
    | 5.3. Compute MaxCosts from the parsed graph | 
    \--------------------------------------------*/
    message("Step 4: Find an upper bound for the minimal budget w.r.t. Tara::net '%s' and cost function '%s'", Tara::args_info.net_arg, Tara::args_info.costfunction_arg);    

    // max Costs are the costs of the most expensive path through
    // the inner state graph
    
    unsigned int maxCostOfComposition=maxCost(Tara::net);
    status("max cost of composition bound: %d", maxCostOfComposition);

    /*------------------------------------------.
    | 7. Find a corresponding partner           | 
    \------------------------------------------*/

    // Build the modified Tara::net for maxCostOfComposition
    //Modification* modification = new iModification(Tara::net, maxCostOfComposition);
    Tara::modification->init(maxCostOfComposition);

    // Check whether N is controllable under budget maxCostOfComposition. If not, return the most permissive partner.
   bool bounded = Tara::args_info.usecase_given or isControllable(*Tara::net, true); 
   if(not bounded) {
       message("costs are unboundend for any partner");
   }

    // If N is not controllable under budget maxCostofComposition, return the mpp. 
    if (!bounded) {
        // Every partner is trivially cost-minimal. Thus, return the mpp
        if (Tara::args_info.sa_given) {
		    message("Any partner is cost-minimal, returning the most-permissive partner.");
            if (std::string(Tara::args_info.sa_arg).compare("-") != 0) {
                std::ofstream outputFile;
                outputFile.open(Tara::args_info.sa_arg);
                outputFile << partnerStream.rdbuf();
                outputFile.close();
            } else {
    		    cout << partnerStream.rdbuf();
            }
    	}
        if (Tara::args_info.og_given) {
            std::string s = "writing operating guidelines to ";
            if (std::string(Tara::args_info.og_arg).compare("-") == 0) {
                s += "standard out";             
            } else {
                s += "file '" + std::string(Tara::args_info.og_arg) + "'";
            }
		    message("Any partner is cost-minimal, %s.", s.c_str());
            computeOG(*Tara::net, Tara::args_info.og_arg);
        }

    } else { // there exists a partner with a bounded budget 
        
        unsigned int minBudget = maxCostOfComposition; // Initially set the minimal budget to the maxCostOfComposition
        
        if (maxCostOfComposition > 0) { // Binary search is only necessary if the upper bound is greater than 0.
            

            bool USE_CONCURRENCY = Tara::args_info.concurrency_given;

            if(USE_CONCURRENCY) {
                status("Step 5: Find minimal budget with concurrent search");
                // run the experimental conccurrent search for comparison with correct result
                CCSearch::setBounds(0,maxCostOfComposition);
                minBudget=CCSearch::search();
            }
            else {

                status("Step 5: Find the minimal budget with a binary search");

                int bsUpper = maxCostOfComposition-1; // for maxCostofComposition, it is controllable anyway. 
                int bsLower = Tara::minCosts; // gna task #7709
            
                while (bsLower <= bsUpper) {
                   
                    // Set the new budget to the middle of the interval
                    Tara::modification->setToValue((bsLower + bsUpper) / 2);
                        
                    status("Checking budget %d (lower bound: %d, upper bound: %d)", Tara::modification->getI(), bsLower, bsUpper);
                    bool bsControllable = isControllable(*Tara::net, true);
                    if (bsControllable) {
                        minBudget = Tara::modification->getI();
                        bsUpper = Tara::modification->getI() - 1;
                    } else {
                        bsLower = Tara::modification->getI() + 1;
                    }
                }
            }
        } 

        if(Tara::args_info.riskcosts_given) {
            double maxProb = backtransformRiskCost(minBudget);
            double percentBudget = (100.0 * maxProb);
            double budgetBase = (1.0 * Tara::args_info.riskcosts_arg) * maxProb;
            message("Maximal Propability found: %g/%d = %g%% ", budgetBase, Tara::args_info.riskcosts_arg, percentBudget);
        }

        else {
            message("Minimal budget found: %d", minBudget);
        }

        // Binary search done. The minimal budget is found. Return the partner for the minimal budget.    

        if(Tara::args_info.sa_given) {
            message("Synthesized a cost-minimal partner. (Costs = %d)", minBudget);
            std::string s = "writing partner to ";
            if (std::string(Tara::args_info.sa_arg).compare("-") == 0) {
                s += "standard out";             
            } else {
                s += "file '" + std::string(Tara::args_info.sa_arg) + "'";
            }
            message("Computing cost-minimal partner, %s.", s.c_str());
            Tara::modification->setToValue(minBudget);
            computeMP(*Tara::net, Tara::args_info.sa_arg);
    	}
        if (Tara::args_info.og_given) {
            std::string s = "writing operating guidelines to ";
            if (std::string(Tara::args_info.og_arg).compare("-") == 0) {
                s += "standard out";             
            } else {
                s += "file '" + std::string(Tara::args_info.og_arg) + "'";
            }
            message("Computing representation of all cost-minimal partners, %s.", s.c_str());
            Tara::modification->setToValue(minBudget);
            computeOG(*Tara::net, Tara::args_info.og_arg);
        }
    } 

    // A partner was written to standard out. Let's do the clean up and exit with success.    
    
    // TODO: CLEANUP

    return EXIT_SUCCESS;
}
/* <<-- CHANGE END -->> */
