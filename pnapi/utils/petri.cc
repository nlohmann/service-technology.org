#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <string>

#include "pnapi.h"
#include "cmdline.h"
#include "config.h"

using std::cin;
using std::cout;
using std::cerr;
using std::endl;
using std::vector;
using std::map;
using std::string;
using std::ifstream;
using std::ofstream;
using std::ostringstream;
using namespace pnapi;

/// the command line parameters
gengetopt_args_info args_info;

/// a suffix for the output filename
string suffix = "";


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
                case(input_arg_lola): {
                    cin >> meta(io::INPUTFILE, "stdin")
                        >> meta(io::CREATOR, PACKAGE_STRING)
                        >> meta(io::INVOCATION, invocation) >> io::lola >> net;
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
                    case(input_arg_lola): {
                        infile >> meta(io::INPUTFILE, args_info.inputs[i])
                            >> meta(io::CREATOR, PACKAGE_STRING)
                            >> meta(io::INVOCATION, invocation) >> io::lola >> net;
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


    /*********
    * WIRING *
    **********/
    if (args_info.wire_given) {

        // collect parsed nets and store them in a mapping
        map<string, PetriNet *> netsByName;
        for (unsigned int i = 0; i < nets.size(); ++i) {
            // strip filename extension
            string name = names[i].substr(0, names[i].find_last_of("."));
            netsByName[name] = &nets[i];
        }

        PetriNet net; // to store composition
        string compositionName; // to store the name of the composition

        if (args_info.wire_arg) {
            compositionName = args_info.wire_arg;

            // try to open wiring file
            ifstream infile(args_info.wire_arg, ifstream::in);
            if (!infile.is_open()) {
                cerr << "petri: could not read from wiring file '" << args_info.wire_arg << "'" << endl;
                exit(EXIT_FAILURE);
            }

            if (args_info.verbose_given) {
                cerr << "petri: composing " << nets.size() << " nets according to '" << args_info.wire_arg << "'" << endl;
            }

            // create a new net consisting of the composed nets
            try {
                infile >> meta(io::INPUTFILE, args_info.wire_arg)
                    >> meta(io::CREATOR, PACKAGE_STRING)
                    >> meta(io::INVOCATION, invocation)
                    >> io::onwd >> pnapi::io::nets(netsByName) >> net;
            } catch (io::InputError error) {
                cerr << "petri:" << error << endl;
                exit(EXIT_FAILURE);
            }
        } else {
            // no wiring file is given
            compositionName = "composition"; //FIXME: choose a nicer name

            if (args_info.verbose_given) {
                cerr << "petri: composing " << nets.size() << " nets using implicit wiring" << endl;
            }

            // calling implicit composition
            net = PetriNet::compose(netsByName);
        }

        if (args_info.verbose_given) {
            cerr << "petri:" << compositionName << ".owfn: " << io::stat << net << endl;
        }

        // remove the parsed nets
        names.clear();
        nets.clear();

        nets.push_back(net);
        names.push_back(compositionName);
    }


    /****************
    * MODIFICATIONS *
    *****************/
    if (args_info.normalize_given) {
        suffix += ".normalized";
        for (unsigned int i = 0; i < nets.size(); ++i) {

            if (args_info.verbose_given) {
                cerr << "petri: normalizing reducing Petri net '" << names[i] << "'..." << endl;
            }

            nets[i].normalize();
        }
    }

    if (args_info.negate_given) {
        suffix += ".negated";
        for (unsigned int i = 0; i < nets.size(); ++i) {

            if (args_info.verbose_given) {
                cerr << "petri: negating the final condition of net '" << names[i] << "'..." << endl;
            }

            nets[i].finalCondition().negate();
        }
    }

    if (args_info.mirror_given) {
        suffix += ".mirrored";
        for (unsigned int i = 0; i < nets.size(); ++i) {

            if (args_info.verbose_given) {
                cerr << "petri: mirroring the net '" << names[i] << "'..." << endl;
            }

            nets[i].mirror();
        }
    }


    /***********************
    * STRUCTURAL REDUCTION *
    ***********************/
    if (args_info.reduce_given) {
        suffix += ".reduced";
        for (unsigned int i = 0; i < nets.size(); ++i) {

            PetriNet::ReductionLevel level = PetriNet::NONE;

            // collect the chosen reduction rules
            for (unsigned int j = 0; j < args_info.reduce_given; ++j) {
                PetriNet::ReductionLevel newLevel = PetriNet::NONE;

                switch(args_info.reduce_arg[j]) {
                    case(reduce_arg_1): newLevel = PetriNet::LEVEL_1; break;
                    case(reduce_arg_2): newLevel = PetriNet::LEVEL_2; break;
                    case(reduce_arg_3): newLevel = PetriNet::LEVEL_3; break;
                    case(reduce_arg_4): newLevel = PetriNet::LEVEL_4; break;
                    case(reduce_arg_5): newLevel = PetriNet::LEVEL_5; break;
                    case(reduce_arg_6): newLevel = PetriNet::LEVEL_6; break;
                    case(reduce_arg_starke): newLevel = PetriNet::SET_STARKE; break;
                    case(reduce_arg_pillat): newLevel = PetriNet::SET_PILLAT; break;
                    case(reduce_arg_dead_nodes): newLevel = PetriNet::DEAD_NODES; break;
                    case(reduce_arg_identical_places): newLevel = PetriNet::IDENTICAL_PLACES; break;
                    case(reduce_arg_identical_transitions): newLevel = PetriNet::IDENTICAL_TRANSITIONS; break;
                    case(reduce_arg_series_places): newLevel = PetriNet::SERIES_PLACES; break;
                    case(reduce_arg_series_transitions): newLevel = PetriNet::SERIES_TRANSITIONS; break;
                    case(reduce_arg_self_loop_places): newLevel = PetriNet::SELF_LOOP_PLACES; break;
                    case(reduce_arg_self_loop_transitions): newLevel = PetriNet::SELF_LOOP_TRANSITIONS; break;
                    case(reduce_arg_equal_places): newLevel = PetriNet::EQUAL_PLACES; break;
                    case(reduce_arg_starke3p): newLevel = PetriNet::STARKE_RULE_3_PLACES; break;
                    case(reduce_arg_starke3t): newLevel = PetriNet::STARKE_RULE_3_TRANSITIONS; break;
                    case(reduce_arg_starke4): newLevel = PetriNet::STARKE_RULE_4; break;
                    case(reduce_arg_starke5): newLevel = PetriNet::STARKE_RULE_5; break;
                    case(reduce_arg_starke6): newLevel = PetriNet::STARKE_RULE_6; break;
                    case(reduce_arg_starke7): newLevel = PetriNet::STARKE_RULE_7; break;
                    case(reduce_arg_starke8): newLevel = PetriNet::STARKE_RULE_8; break;
                    case(reduce_arg_starke9): newLevel = PetriNet::STARKE_RULE_9; break;
                    case(reduce_arg_keep_normal): newLevel = PetriNet::KEEP_NORMAL; break;
                    case(reduce_arg_once): newLevel = PetriNet::ONCE; break;
                    case(reduce_arg_k_boundedness): newLevel = PetriNet::K_BOUNDEDNESS; break;
                    case(reduce_arg_boundedness): newLevel = PetriNet::BOUNDEDNESS; break;
                    case(reduce_arg_liveness): newLevel = PetriNet::LIVENESS; break;
                    default: /* do nothing */; break;
                }

                level = (PetriNet::ReductionLevel)(level | newLevel);
            }

            if (args_info.verbose_given) {
                cerr << "petri: structurally reducing Petri net '" << names[i] << "'..." << endl;
            }

            nets[i].reduce(level);
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
                string outname = names[i] + suffix + "." + args_info.output_orig[j];
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

                    // create LoLA output
                    case (output_arg_lola): {
                        outfile << io::lola << nets[i];
                        break;
                    }

                    // create automaton output
                    case (output_arg_sa): {
                        Automaton sauto(nets[i]);
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
                    case (output_arg_eps):
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
