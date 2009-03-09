#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

#include "pnapi.h"
#include "cmdline.h"
#include "config.h"

using std::cin;
using std::cout;
using std::cerr;
using std::endl;
using std::vector;
using std::string;
using std::ifstream;
using std::ofstream;
using std::ostringstream;
using namespace pnapi;

/// the command line parameters
gengetopt_args_info args_info;


/// evaluate the command line parameters
void evaluateParameters(int argc, char** argv) {
    // set default values
    cmdline_parser_init(&args_info);

    // initialize the parameters structure
    struct cmdline_parser_params *params = cmdline_parser_params_create();

    // call the cmdline parser
    if (cmdline_parser (argc, argv, &args_info) != 0) {
        fprintf(stderr, "       see 'petri --help' for more information\n");
        exit(EXIT_FAILURE);
    }

    free(params);
}


int main(int argc, char** argv) {
    evaluateParameters(argc, argv);

    vector<PetriNet> nets;
    vector<string> names;

    // store invocation in a string for meta information in file output
    string invocation;
    for (int i = 0; i < argc; ++i) {
        invocation += string(argv[i]) + " ";
    }


    /********
    * INPUT *
    ********/
    if (!args_info.inputs_num) {
        // read from stdin
        PetriNet net;

        // try to parse net
        try {
            switch (args_info.input_arg) {
                case(input_arg_owfn): {
                    cin >> meta(io::INPUTFILE, "stdin")
                        >> meta(io::CREATOR, PACKAGE_STRING)
                        >> meta(io::INVOCATION, invocation) >> io::owfn >> net;
                    break;
                }
                case(input_arg_onwd): {
                    cin >> meta(io::INPUTFILE, "stdin")
                        >> meta(io::CREATOR, PACKAGE_STRING)
                        >> meta(io::INVOCATION, invocation) >> io::onwd >> net;
                    break;
                }
            }
        } catch (io::InputError error) {
            cerr << "petri:" << error << endl;
            exit(EXIT_FAILURE);
        }

        if (args_info.verbose_given) {
            cerr << "petri:<stdin>: " << io::stat << net << endl;
        }

        // store net
        nets.push_back(net);
        names.push_back("stdin");
    } else {
        // read from files
        for (unsigned int i = 0; i < args_info.inputs_num; ++i) {
            PetriNet net;

            // try to open file
            ifstream infile(args_info.inputs[i], ifstream::in);
            if (!infile.is_open()) {
                cerr << "petri: could not read from file '" << args_info.inputs[i] << "'" << endl;
                exit(EXIT_FAILURE);
            }

            // try to parse net
            try {
                switch (args_info.input_arg) {
                    case(input_arg_owfn): {
                        infile >> meta(io::INPUTFILE, args_info.inputs[i])
                            >> meta(io::CREATOR, PACKAGE_STRING)
                            >> meta(io::INVOCATION, invocation) >> io::owfn >> net;
                        break;
                    }
                    case(input_arg_onwd): {
                        infile >> meta(io::INPUTFILE, args_info.inputs[i])
                            >> meta(io::CREATOR, PACKAGE_STRING)
                            >> meta(io::INVOCATION, invocation) >> io::onwd >> net;
                        break;
                    }
                }
            } catch (io::InputError error) {
                cerr << "petri:" << error << endl;
                infile.close();
                exit(EXIT_FAILURE);
            }

            infile.close();

            if (args_info.verbose_given) {
                cerr << "petri:" << args_info.inputs[i] << ": " << io::stat << net << endl;
            }

            // store net
            nets.push_back(net);
            names.push_back(args_info.inputs[i]);
        }
    }


    /****************
    * MODIFICATIONS *
    *****************/
    if (args_info.normalize_given) {
        for (unsigned int i = 0; i < nets.size(); ++i) {

            if (args_info.verbose_given) {
                cerr << "petri: normalizing reducing Petri net '" << names[i] << "'..." << endl;
            }

            nets[i].normalize();
        }
    }


    /***********************
    * STRUCTURAL REDUCTION *
    ***********************/
    if (args_info.reduce_given && args_info.reduce_arg > 0) {
        for (unsigned int i = 0; i < nets.size(); ++i) {

            if (args_info.verbose_given) {
                cerr << "petri: structurally reducing Petri net '" << names[i] << "'..." << endl;
            }

            nets[i].reduce(args_info.reduce_arg);
        }
    }


    /************************
    * STRUCTURAL PROPERTIES *
    ************************/
    if (args_info.check_given || args_info.isFreeChoice_given || args_info.isNormal_given || args_info.isWorkflow_given) {
        for (unsigned int i = 0; i < nets.size(); ++i) {
            cerr << "petri:" << names[i] << ": ";

            // check for free choice
            if (args_info.check_arg == check_arg_freechoice || args_info.isFreeChoice_given) {
                cerr << nets[i].isFreeChoice() << endl;
            }

            // check for normality
            if (args_info.check_arg == check_arg_normal || args_info.isNormal_given) {
                cerr << nets[i].isNormal() << endl;
            }

            // check for workflow structure
            if (args_info.check_arg == check_arg_workflow || args_info.isWorkflow_given) {
                cerr << nets[i].isWorkflow() << endl;
            }
        }
    }


    /*********
    * OUTPUT *
    *********/
    if (args_info.output_given) {
        for (unsigned int i = 0; i < nets.size(); ++i) {
            for (unsigned int j = 0; j < args_info.output_given; ++j) {
                // try to open file to write
                string outname = names[i] + "." + args_info.output_orig[j];
                ofstream outfile(outname.c_str(), ofstream::trunc);
                if (!outfile.is_open()) {
                    cerr << "petri: could not write to file '" << outname << "'" << endl;
                    exit(EXIT_FAILURE);
                }

                if (args_info.verbose_given) {
                    cerr << "petri: creating file '" << outname << "'..." << endl;
                }

                outfile << meta(io::OUTPUTFILE, outname);

                switch(args_info.output_arg[j]) {
                    // create oWFN output
                    case (output_arg_owfn): {
                        outfile << io::owfn << nets[i];
                        break;
                    }

                    // create automaton output
                    case (output_arg_sa): {
                        ServiceAutomaton sauto(nets[i]);
                        outfile << io::sa << sauto;
                        break;
                    }

                    // create dot output
                    case (output_arg_dot): {
                        outfile << io::dot << nets[i];
                        break;
                    }

                    // create output using Graphviz dot
                    case (output_arg_png):
                    case (output_arg_pdf):
                    case (output_arg_svg): {
                        if (CONFIG_DOT == "not found") {
                            cerr << "petri: Graphviz dot was not found by configure script; see README" << endl;
                            cerr << "       necessary for option '--output=owfn'" << endl;
                            exit(EXIT_FAILURE);
                        }
#if !defined(HAVE_POPEN) || !defined(HAVE_PCLOSE)
                        cerr << "petri: cannot open UNIX pipe to Graphviz dot" << endl;
                        cerr << "       create dot file with '--output=dot' and call Graphviz dot manually" << endl;
                        exit(EXIT_FAILURE);
#endif
                        ostringstream d;
                        d << io::dot << nets[i];
                        string call = string(CONFIG_DOT) + " -T" + args_info.output_orig[j] + " -o " + outname + " &> /dev/null";
                        FILE *s = popen(call.c_str(), "w");
                        assert(s);
                        fprintf(s, "%s\n", d.str().c_str());
                        assert(!ferror(s));
                        pclose(s);
                        break;
                    }
                }

                outfile.close();
            }
        }
    }


    return EXIT_SUCCESS;
}
