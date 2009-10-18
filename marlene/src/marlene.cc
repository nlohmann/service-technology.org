/*****************************************************************************\
 Marlene -- synthesizing behavioral adapters

 Copyright (C) 2009  Christian Gierds <gierds@informatik.hu-berlin.de>

 Marlene is free software: you can redistribute it and/or modify it under the
 terms of the GNU Affero General Public License as published by the Free
 Software Foundation, either version 3 of the License, or (at your option)
 any later version.

 Marlene is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for
 more details.

 You should have received a copy of the GNU Affero General Public License
 along with Marlene.  If not, see <http://www.gnu.org/licenses/>. 
\*****************************************************************************/

// include standard C++ headers
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cstdlib>
#include <libgen.h>

// include Marlene's own headers
#include "config.h"
#include "cmdline.h"
#include "helper.h"
#include "macros.h"
#include "adapter.h"
#include "Output.h"

// include PN Api headers
#include "pnapi/pnapi.h"

/// a variable holding the time of the call
clock_t start_clock = clock();

/// a function collecting calls to organize termination (close files, ...)
void terminationHandler() {
    // release memory (used to detect memory leaks)
/*    if (args_info.finalize_flag) {
        time_t start_time, end_time;
        time(&start_time);
        cmdline_parser_free(&args_info);
        InnerMarking::finalize();
        StoredKnowledge::finalize();
        time(&end_time);
        status("released memory [%.0f sec]", difftime(end_time, start_time));
    }
*/
    // print statistics
    if (args_info.stats_flag) {
        message("runtime: %.2f sec", (static_cast<double>(clock()) - static_cast<double>(start_clock)) / CLOCKS_PER_SEC);
        fprintf(stderr, "%s: memory consumption: ", PACKAGE);
        system((std::string("ps -o rss -o comm | ") + TOOL_GREP + " " + PACKAGE + " | " + TOOL_AWK + " '{ if ($1 > max) max = $1 } END { print max \" KB\" }' 1>&2").c_str());
    }
}

int main(int argc, char* argv[])
{
    argv[0]=basename(argv[0]);
    
    // set the function to call on normal termination
    atexit(terminationHandler);

    std::vector< pnapi::PetriNet * > nets;
    
    /*************************************************\
    * Parsing the command line and evaluation options *
    \*************************************************/
    // evaluate command line options
    evaluate_command_line(argc, argv);

    // store invocation in a string for meta information in file output
    std::string invocation;
    for (int i = 0; i < argc; ++i) {
        invocation += std::string(argv[i]) + " ";
    }

    time_t start_time, end_time;

    /******************\
    * Reading all nets *
    \******************/
    time(&start_time);
    // if nets given as arguments read from files
    if ( args_info.inputs_num > 0 )
    {
        for ( unsigned i = 0; i < args_info.inputs_num; ++i )
        {
            pnapi::PetriNet * net = new pnapi::PetriNet();
            
            std::string filename = args_info.inputs[i];
            status("reading open net from file \"%s\"", filename.c_str());
            try {
                std::ifstream inputfile(filename.c_str(), std::ios_base::in);
                if (inputfile.good())
                {
                    inputfile >> meta(pnapi::io::INPUTFILE, filename)
                    >> meta(pnapi::io::CREATOR, PACKAGE_STRING)
                    >> meta(pnapi::io::INVOCATION, invocation) >> pnapi::io::owfn >> *net;

                    // adding a prefix to the net
                    if (args_info.withprefix_flag)
                    {
                        net->prefixNodeNames(toString(i) + ".", false);
                    }
                    
                    nets.push_back(net);
                    //std::cout << pnapi::io::dot << *net;

                }
                else
                {
                    abort(2, "Open net file %s could not be opened for reading", filename.c_str());
                }
            } catch (pnapi::io::InputError error) {
                std::cerr << PACKAGE << ":" << error << std::endl;
                abort(2, " ");
            }

        }
    }
    else
    { //! parsing open net for stdin aka std::cin
        pnapi::PetriNet * net = new pnapi::PetriNet();
        nets.push_back(net);
        try {
            status("reading open net from STDIN");
            std::cin >> pnapi::io::owfn >> *net;
            // adding a prefix to the net
            if (args_info.withprefix_flag)
            {
                net->prefixNodeNames("0.", false);
            }
        } catch (pnapi::io::InputError error) {
            std::cerr << PACKAGE << ":" << error << std::endl;
            abort(2, " ");
        }
    }
    time(&end_time);
    status("reading all input nets done [%.0f sec]", difftime(end_time, start_time));
    
    /*************************\
    * All necessary variables *
    \*************************/
    
    //! #rs contains a RuleSet that is provided for the adapter
    RuleSet rs;
    //! #adapter
    Adapter adapter(nets, rs, contType, messageBound, useCompPlaces);

    if ( args_info.rulefile_given )
    {
        time(&start_time);
        status("reading transformation rules from file \"%s\"", args_info.rulefile_arg);
        FILE * rulefile (NULL);
        if((rulefile = fopen(args_info.rulefile_arg,"r")))
        {
            rs.addRules(rulefile);
            // not needed here, rulefile will be closed in addRules()
            // fclose(rulefile);
            rulefile = NULL;
        }
        else
        {
            abort(2, "Rule file %s could not be opened for reading", args_info.rulefile_arg);
        }
        time(&end_time);
        status("reading all rules file done [%.0f sec]", difftime(end_time, start_time));
    }
    
    /*********************\
    * Building the engine *
    \*********************/
    
    time(&start_time);
    pnapi::PetriNet engine (*adapter.buildEngine());
    time(&end_time);
    status("engine built [%.0f sec]", difftime(end_time, start_time));

    if (args_info.engineonly_flag)
    {
        std::string filename = (args_info.output_given ? std::string(args_info.output_arg) : std::string("-"));
        Output outfile(filename, std::string("engine"));
        outfile.stream() << pnapi::io::owfn << meta(pnapi::io::CREATOR, PACKAGE_STRING) 
            << meta(pnapi::io::INVOCATION, invocation) << engine;
        
        //quit
        return 0;
    }
    
    /*************************\
    * Building the controller *
    \*************************/
    
    time(&start_time);
    pnapi::PetriNet controller (*adapter.buildController());
    time(&end_time);
    status("controller built [%.0f sec]", difftime(end_time, start_time));

    if (args_info.controlleronly_flag)
    {
        std::string filename = (args_info.output_given ? std::string(args_info.output_arg) : std::string("-"));
        Output outfile(filename, std::string("controller"));
        outfile.stream() << pnapi::io::owfn << meta(pnapi::io::CREATOR, PACKAGE_STRING) 
            << meta(pnapi::io::INVOCATION, invocation) << controller;
        
        //quit
        return 0;
    }
    
    /*******************************************************************\
    * Composing engine and controller in order to get the final adapter *
    \*******************************************************************/
    
    engine.compose(controller, "engine.", "controller.");
    
    engine.reduce(pnapi::PetriNet::LEVEL_4);
    {
        std::string filename = (args_info.output_given ? std::string(args_info.output_arg) : std::string("-"));
        Output outfile(filename, std::string("final adapter"));
        outfile.stream() << pnapi::io::owfn << meta(pnapi::io::CREATOR, PACKAGE_STRING) 
            << meta(pnapi::io::INVOCATION, invocation) << engine;
    }

    /*******************\
    * Deleting all nets *
    \*******************/
/*
    for ( unsigned int index = 0; index < nets.size(); ++index)
    {
        delete nets[index];
    }
    nets.clear();
*/    
    return 0;
}

