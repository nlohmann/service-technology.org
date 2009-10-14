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

typedef enum { TYPE_OPENNET, TYPE_LOLANET } objectType;

struct FileObject {
    objectType type;
    string filename;

    PetriNet *net;

    FileObject(string f) : filename(f), net(NULL) { }
};


/// evaluate the command line parameters
void evaluateParameters(int argc, char** argv) {
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

    vector<FileObject> objects;

    // store invocation in a string for meta information in file output
    string invocation;
    for (int i = 0; i < argc; ++i) {
        invocation += string(argv[i]) + " ";
    }

    if (args_info.converter_given)
    {
      switch(args_info.converter_arg)
      {
      case (converter_arg_petrify) : 
      {
        pnapi::PetriNet::setAutomatonConverter(pnapi::PetriNet::PETRIFY);
        break;
      }
      case (converter_arg_genet) :
      {
        pnapi::PetriNet::setAutomatonConverter(pnapi::PetriNet::GENET);
        break;
      }
      case (converter_arg_statemachine) :
      {
        pnapi::PetriNet::setAutomatonConverter(pnapi::PetriNet::STATEMACHINE);
        break;
      }
      default: assert(false);
      }
    }

    /********
    * INPUT *
    ********/
    if (!args_info.inputs_num) {
        // read from stdin
        FileObject current("stdin");

        // try to parse net
        try {
            switch (args_info.input_arg) {
                case(input_arg_owfn): {
                    current.net = new PetriNet();
                    std::cin >> meta(io::INPUTFILE, current.filename)
                        >> meta(io::CREATOR, PACKAGE_STRING)
                        >> meta(io::INVOCATION, invocation) >> io::owfn >> *(current.net);

                    current.type = TYPE_OPENNET;
                    break;
                }
                case(input_arg_lola): {
                    current.net = new PetriNet();
                    std::cin >> meta(io::INPUTFILE, current.filename)
                        >> meta(io::CREATOR, PACKAGE_STRING)
                        >> meta(io::INVOCATION, invocation) >> io::lola >> *(current.net);

                    current.type = TYPE_LOLANET;
                    break;
                }
                case(input_arg_sa): {
                    Automaton sa;
                    std::cin >> meta(io::INPUTFILE, current.filename)
                        >> meta(io::CREATOR, PACKAGE_STRING)
                        >> meta(io::INVOCATION, invocation) >> io::sa >> sa;
                    current.net = new PetriNet(sa);
                    
                    current.type = TYPE_OPENNET;
                    break;
                }
            }
        } catch (io::InputError error) {
            cerr << "petri:" << error << endl;
            exit(EXIT_FAILURE);
        }

        if (args_info.verbose_given) {
          cerr << "petri:<stdin>: " << io::stat << *(current.net) << endl;
        }

        // store object
        objects.push_back(current);
    } else {
        // read from files
        for (unsigned int i = 0; i < args_info.inputs_num; ++i) {
            // prepare a new object
            FileObject current(args_info.inputs[i]);

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
                        current.net = new PetriNet();
                        infile >> meta(io::INPUTFILE, current.filename)
                            >> meta(io::CREATOR, PACKAGE_STRING)
                            >> meta(io::INVOCATION, invocation) >> io::owfn >> *(current.net);

                        current.type = TYPE_OPENNET;
                        break;
                    }
                    case(input_arg_lola): {
                        current.net = new PetriNet();
                        infile >> meta(io::INPUTFILE, current.filename)
                            >> meta(io::CREATOR, PACKAGE_STRING)
                            >> meta(io::INVOCATION, invocation) >> io::lola >> *(current.net);

                        current.type = TYPE_LOLANET;
                        break;
                    }
                    case(input_arg_sa): {
                        Automaton sa;
                        infile >> meta(io::INPUTFILE, current.filename)
                            >> meta(io::CREATOR, PACKAGE_STRING)
                            >> meta(io::INVOCATION, invocation) >> io::sa >> sa;
                        current.net = new PetriNet(sa);

                        current.type = TYPE_OPENNET;
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
              cerr << "petri:" << args_info.inputs[i] << ": " << io::stat << *(current.net) << endl;
            }

            // store object
            objects.push_back(current);
        }
    }


    /*********
    * WIRING *
    **********/
    if (args_info.wire_given) {
        // collect parsed nets and store them in a mapping
        map<string, PetriNet *> netsByName;
        for (unsigned int i = 0; i < objects.size(); ++i) {
            // strip filename extension
            string name = objects[i].filename.substr(0, objects[i].filename.find_last_of("."));
            netsByName[name] = objects[i].net;
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
                cerr << "petri: composing " << objects.size() << " nets according to '" << args_info.wire_arg << "'" << endl;
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
                cerr << "petri: composing " << objects.size() << " nets using implicit wiring" << endl;
            }

            // calling implicit composition
            net = PetriNet::composeByWiring(netsByName);
        }

        if (args_info.verbose_given) {
            cerr << "petri:" << compositionName << ".owfn: " << io::stat << net << endl;
        }

        // remove the parsed objects
        objects.clear();

        FileObject current(compositionName);
        current.type = TYPE_OPENNET;
        current.net = new PetriNet(net);        
        objects.push_back(current);
    }


    /***************
     * COMPOSITION *
     ***************/
    if (args_info.compose_given) {
        // try to open file
        ifstream infile(args_info.compose_arg, ifstream::in);
        if (!infile.is_open()) {
            cerr << "petri: could not read from file '" << args_info.compose_arg << "'" << endl;
            exit(EXIT_FAILURE);
        }

        PetriNet secondNet; // to store composition "partner"
        string secondNetName = args_info.compose_arg;

        // read net
        infile >> meta(io::INPUTFILE, secondNetName)
            >> meta(io::CREATOR, PACKAGE_STRING)
            >> meta(io::INVOCATION, invocation) >> io::owfn >> secondNet;

        // compose nets
        for (int i=0; i < objects.size(); ++i) {
            objects[i].net->compose(secondNet, objects[i].filename, secondNetName);
            objects[i].filename += ".composed"; 
        }
    }


    /***********
     * PRODUCT *
     ***********/
    if (args_info.produce_given) {
        if (args_info.inputs_num > 1) {
            cerr << "petri: at most one net can be used with '--produce' parameter" << endl;
            exit(EXIT_FAILURE);
        }

        // try to open file
        ifstream infile(args_info.produce_arg, ifstream::in);
        if (!infile.is_open()) {
            cerr << "petri: could not read from file '" << args_info.produce_arg << "'" << endl;
            exit(EXIT_FAILURE);
        }

        PetriNet constraintNet; // to store constraint

        // read net
        infile >> meta(io::INPUTFILE, args_info.produce_arg)
            >> meta(io::CREATOR, PACKAGE_STRING)
            >> meta(io::INVOCATION, invocation) >> io::owfn >> constraintNet;

        // produce nets
        objects[0].net->produce(constraintNet);
    }

    /****************
    * MODIFICATIONS *
    *****************/
    if (args_info.normalize_given) {
        suffix += ".normalized";
        for (unsigned int i = 0; i < objects.size(); ++i) {

            if (args_info.verbose_given) {
                cerr << "petri: normalizing reducing Petri net '" << objects[i].filename << "'..." << endl;
            }

            objects[i].net->normalize();
        }
    }

    if (args_info.negate_given) {
        suffix += ".negated";
        for (unsigned int i = 0; i < objects.size(); ++i) {

            if (args_info.verbose_given) {
                cerr << "petri: negating the final condition of net '" << objects[i].filename << "'..." << endl;
            }

            objects[i].net->finalCondition().negate();
        }
    }

    if (args_info.mirror_given) {
        suffix += ".mirrored";
        for (unsigned int i = 0; i < objects.size(); ++i) {

            if (args_info.verbose_given) {
                cerr << "petri: mirroring the net '" << objects[i].filename << "'..." << endl;
            }

            objects[i].net->mirror();
        }
    }


    /***********************
    * STRUCTURAL REDUCTION *
    ***********************/
    if (args_info.reduce_given) {
        suffix += ".reduced";
        for (unsigned int i = 0; i < objects.size(); ++i) {

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
                cerr << "petri: structurally reducing Petri net '" << objects[i].filename << "'..." << endl;
            }

            objects[i].net->reduce(level);
        }
    }


    /************************
    * STRUCTURAL PROPERTIES *
    ************************/
    if (args_info.check_given or
        args_info.isFreeChoice_given or
        args_info.isNormal_given or
        args_info.isWorkflow_given) {
        for (unsigned int i = 0; i < objects.size(); ++i) {
            cerr << "petri:" << objects[i].filename << ": ";

            // check for free choice
            if (args_info.check_arg == check_arg_freechoice or args_info.isFreeChoice_given) {
                cerr << objects[i].net->isFreeChoice() << endl;
            }

            // check for normality
            if (args_info.check_arg == check_arg_normal or args_info.isNormal_given) {
                cerr << objects[i].net->isNormal() << endl;
            }

            // check for workflow structure
            if (args_info.check_arg == check_arg_workflow or args_info.isWorkflow_given) {
                cerr << objects[i].net->isWorkflow() << endl;
            }
        }
    }


    /*********
    * OUTPUT *
    *********/
    if (args_info.output_given) {
        for (unsigned int i = 0; i < objects.size(); ++i) {
            for (unsigned int j = 0; j < args_info.output_given; ++j) {
                // try to open file to write
                string outname = objects[i].filename + suffix + "." + args_info.output_orig[j];
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
                        outfile << io::owfn << *(objects[i].net);
                        break;
                    }

                    // create LoLA output
                    case (output_arg_lola): {
                        if (args_info.formula_flag)
                          outfile << io::lola << io::formula << *(objects[i].net);
                        else
                          outfile << io::lola << *(objects[i].net);
                        break;
                    }

                    // create automaton output
                    case (output_arg_sa): {
                        Automaton sauto(*(objects[i].net));
                        outfile << io::sa << sauto;
                        break;
                    }

                    // create dot output
                    case (output_arg_dot): {
                        outfile << io::dot << *(objects[i].net);
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
#if !defined(HAVE_POPEN) or !defined(HAVE_PCLOSE)
                        cerr << "petri: cannot open UNIX pipe to Graphviz dot" << endl;
                        cerr << "       create dot file with '--output=dot' and call Graphviz dot manually" << endl;
                        exit(EXIT_FAILURE);
#endif
                        ostringstream d;
                        d << io::dot << *(objects[i].net);
                        string call = string(CONFIG_DOT) + " -T" + args_info.output_orig[j] + " -q -o " + outname;;
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
    
    /**********
    * CLEANUP *
    **********/
    for (unsigned int i = 0; i < objects.size(); ++i) {
      delete (objects[i].net);
    }


    return EXIT_SUCCESS;
}
