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
#include <ctime>

// include Marlene's own headers
#include "config.h"
#include "cmdline.h"
#include "helper.h"
#include "macros.h"
#include "adapter.h"
#include "Output.h"
#include "verbose.h"

// include PN API headers
#include "pnapi/pnapi.h"

// include json library
#include "json/json.h"

/// a variable holding the time of the call
clock_t start_clock = clock();

/// a function collecting calls to organize termination (close files, ...)
void terminationHandler() {
    // release memory (used to detect memory leaks)
    // if (args_info.finalize_flag)
    // print statistics
    if (args_info.stats_flag) {
        message("runtime: %.2f sec", (static_cast<double> (clock())
                - static_cast<double> (start_clock)) / CLOCKS_PER_SEC);
        fprintf(stderr, "%s: memory consumption: ", PACKAGE);
        int result = system(
                (std::string("ps -o rss -o comm | ") + TOOL_GREP + " "
                        + PACKAGE + " | " + TOOL_AWK
                        + " '{ if ($1 > max) max = $1 } END { print max \" KB\" }' 1>&2").c_str());
        if (result != 0) {
            status("error while figuring out memory consumption\n");
        }
    }
}

int main(int argc, char* argv[]) {
    try {
        argv[0] = basename(argv[0]);

        // set the function to call on normal termination
        atexit(terminationHandler);

        std::vector<PetriNet_ptr> nets;

        /*************************************************\
    * Parsing the command line and evaluation options *
         \*************************************************/
        // evaluate command line options
        evaluate_command_line(argc, argv);
        Output::setTempfileTemplate(args_info.tmpfile_arg);
        Output::setKeepTempfiles(args_info.noClean_flag);

        // diagnosis is only possible, if libconfig was found
#if not defined(HAVE_LIBCONFIG__)
        if (args_info.diagnosis_flag)
        {
            message("Diagnosis of adapters is only possible, if %s was compiled with libconfig++ support",PACKAGE);
            exit(1);
        }
#endif

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
        if (args_info.inputs_num > 0) {
            for (unsigned i = 0; i < args_info.inputs_num; ++i) {
                PetriNet_ptr net(new pnapi::PetriNet());

                std::string filename = args_info.inputs[i];
                status("reading open net from file \"%s\"",
                        filename.c_str());
                try {
                    std::ifstream inputfile(filename.c_str(),
                            std::ios_base::in);
                    if (inputfile.good()) {
                        inputfile >> meta(pnapi::io::INPUTFILE, filename)
                                >> meta(pnapi::io::CREATOR, PACKAGE_STRING)
                                >> meta(pnapi::io::INVOCATION, invocation)
                                >> pnapi::io::owfn >> *net;

                        // adding a prefix to the net
                        if (args_info.withprefix_flag) {
                            net->prefixNames(toString(i) + ".");
                        }

                        nets.push_back(net);
                    } else {
                        abort(
                                2,
                                "Open net file %s could not be opened for reading",
                                filename.c_str());
                    }
                } catch (const pnapi::exception::InputError& error) {
                    std::cerr << PACKAGE << ":" << error << std::endl;
                    displayFileError(error.filename.c_str(), error.line,
                            error.token.c_str());
                    abort(2, " ");
                }

            }
        } else {
            //! parsing open net for stdin aka std::cin
            PetriNet_ptr net(new pnapi::PetriNet());
            nets.push_back(net);
            try {
                status("reading open net from STDIN");
                std::cin >> pnapi::io::owfn >> *net;
                // adding a prefix to the net
                if (args_info.withprefix_flag) {
                    net->prefixNames("0.");
                }
            } catch (const pnapi::exception::InputError& error) {
                std::cerr << PACKAGE << ":" << error << std::endl;
                abort(2, " ");
            }
        }
        time(&end_time);
        status("reading all input nets done [%.0f sec]", difftime(end_time,
                start_time));

        /*************************\
    * All necessary variables *
         \*************************/

        //! #rs contains a RuleSet that is provided for the adapter
        RuleSet rs;
        //! #adapter
        Adapter adapter(nets, rs, contType, messageBound, useCompPlaces);

        if (args_info.rulefile_given) {
            time(&start_time);
            status("reading transformation rules from file \"%s\"",
                    args_info.rulefile_arg);
#ifdef USE_SHARED_PTR
            std::tr1::shared_ptr<FILE> rulefile(fopen(
                    args_info.rulefile_arg, "r"), fclose);
            if (rulefile.get() != NULL) {
#else
                FILE * rulefile = fopen(args_info.rulefile_arg, "r");
                if (rulefile != NULL) {
#endif
                rs.addRules(rulefile);
            } else {
                abort(2, "Rule file %s could not be opened for reading",
                        args_info.rulefile_arg);
            }
            time(&end_time);
            status("reading all rules file done [%.0f sec]", difftime(
                    end_time, start_time));
#ifndef USE_SHARED_PTR
            fclose(rulefile);
#endif
        }

        /*********************\
    * Building the engine *
         \*********************/

        time(&start_time);
        pnapi::PetriNet engine(*adapter.buildEngine());
        time(&end_time);
        status("engine built [%.0f sec]", difftime(end_time, start_time));

        if (args_info.engineonly_flag) {
            std::string filename = (args_info.output_given ? std::string(
                    args_info.output_arg) : std::string("-"));
            Output outfile(filename, std::string("engine"));
            outfile.stream() << pnapi::io::owfn << meta(pnapi::io::CREATOR,
                    PACKAGE_STRING) << meta(pnapi::io::INVOCATION,
                    invocation) << engine;

            //quit
            {
                time_t start_time, end_time;
                time(&start_time);
                cmdline_parser_free(&args_info);
                time(&end_time);
                status("released memory [%.0f sec]", difftime(end_time,
                        start_time));
            }

#ifndef USE_SHARED_PTR
            // delete(controller);
            for (unsigned int i = 0; i < nets.size(); ++i) {
                delete(nets[i]);
            }
#endif
            nets.clear();
            return 0;
        }

        /*********************\
    * Composing the engine *
         \*********************/

        time(&start_time);
        pnapi::PetriNet composition(*adapter.composeEngine());
        time(&end_time);
        status("engine composed [%.0f sec]", difftime(end_time, start_time));

        if (args_info.composedonly_flag) {
            std::string filename = (args_info.output_given ? std::string(
                    args_info.output_arg) : std::string("-"));
            Output outfile(filename, std::string("composed"));
            outfile.stream() << pnapi::io::owfn << meta(pnapi::io::CREATOR,
                    PACKAGE_STRING) << meta(pnapi::io::INVOCATION,
                    invocation) << composition;

            //quit
            {
                time_t start_time, end_time;
                time(&start_time);
                cmdline_parser_free(&args_info);
                time(&end_time);
                status("released memory [%.0f sec]", difftime(end_time,
                        start_time));
            }

#ifndef USE_SHARED_PTR
            // delete(controller);
            for (unsigned int i = 0; i < nets.size(); ++i) {
                delete(nets[i]);
            }
#endif
            nets.clear();
            return 0;
        }

        /*************************\
    * Building the controller *
         \*************************/

        time(&start_time);
        PetriNet_const_ptr controller(adapter.buildController());
        time(&end_time);
        if (not args_info.diagnosis_flag) {
            status("controller built [%.0f sec]", difftime(end_time,
                    start_time));
        }

        if (args_info.controlleronly_flag) {
            std::string filename = (args_info.output_given ? std::string(
                    args_info.output_arg) : std::string("-"));
            Output outfile(filename, std::string("controller"));
            outfile.stream() << pnapi::io::owfn << meta(pnapi::io::CREATOR,
                    PACKAGE_STRING) << meta(pnapi::io::INVOCATION,
                    invocation) << *controller;

            //quit
            {
                time_t start_time, end_time;
                time(&start_time);
                cmdline_parser_free(&args_info);
                time(&end_time);
                status("released memory [%.0f sec]", difftime(end_time,
                        start_time));
            }

#ifndef USE_SHARED_PTR
            // delete(controller);
            for (unsigned int i = 0; i < nets.size(); ++i) {
                delete(nets[i]);
            }
#endif
            nets.clear();
            return 0;
        }

        /*******************************************************************\
    * Composing engine and controller in order to get the final adapter *
         \*******************************************************************/

        if (not args_info.diagnosis_flag) {
            engine.compose(*controller, "engine.", "controller.");

            engine.reduce(pnapi::PetriNet::LEVEL_5
                    | pnapi::PetriNet::SET_PILLAT); //
            {
                std::string filename =
                        (args_info.output_given ? std::string(
                                args_info.output_arg) : std::string("-"));
                Output outfile(filename, std::string("final adapter"));
                std::string ps(PACKAGE_STRING);
                outfile.stream() << pnapi::io::owfn << meta(
                        pnapi::io::CREATOR, ps) << meta(
                        pnapi::io::INVOCATION, invocation) << engine;
            }
        }

        /*******************\
    * Deleting all nets *
         \*******************/

#ifndef USE_SHARED_PTR
        // delete(controller);
        for (unsigned int i = 0; i < nets.size(); ++i) {
            delete(nets[i]);
        }
#endif
        nets.clear();

    } catch (pnapi::exception::UserCausedError& ex) {
        message("Unexpected error, please report this bug to %s.",
                PACKAGE_BUGREPORT);
        std::cerr << ex << std::endl;
        exit(1);
    }
    // cmdline_parser_free(&args_info);
    {
        time_t start_time, end_time;
        time(&start_time);
        cmdline_parser_free(&args_info);
        time(&end_time);
        status("released memory [%.0f sec]", difftime(end_time, start_time));
    }

    return 0;
}

