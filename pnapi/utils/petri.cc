#include <iostream>
#include <fstream>
#include <cstdlib>
#include <vector>
#include <string>
#include <libgen.h>
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
    if (cmdline_parser (argc, argv, &args_info) != 0)
        exit(EXIT_FAILURE);    
        
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
            cin >> meta(io::INPUTFILE, "stdin") 
                >> meta(io::CREATOR, PACKAGE_STRING)
                >> meta(io::INVOCATION, invocation) >> io::owfn >> net;
        } catch (io::InputError error) {
            cerr << error << endl;
            exit(EXIT_FAILURE);
        }

        if (args_info.verbose_given) {
            cerr << "<stdin>: " << io::stat << net << endl;
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
                cerr << args_info.inputs[i] << ": could not open file to read" << endl;
                exit(EXIT_FAILURE);
            }
            
            // try to parse net
            try {
                infile >> meta(io::INPUTFILE, args_info.inputs[i])
                    >> meta(io::CREATOR, PACKAGE_STRING)
                    >> meta(io::INVOCATION, invocation) >> io::owfn >> net;
            } catch (io::InputError error) {
                cerr << error << endl;
                infile.close();            
                exit(EXIT_FAILURE);
            }
            
            infile.close();
            
            if (args_info.verbose_given) {
                cerr << args_info.inputs[i] << ": " << io::stat << net << endl;
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
            nets[i].normalize();
        }        
    }
    
    
    /***********************
    * STRUCTURAL REDUCTION *
    ***********************/
    if (args_info.reduce_given && args_info.reduce_arg > 0) {
        for (unsigned int i = 0; i < nets.size(); ++i) {
            nets[i].reduce(args_info.reduce_arg);
        }
    }
    

    /************************
    * STRUCTURAL PROPERTIES *
    ************************/
    if (args_info.check_given || args_info.isFreeChoice_given ||
        args_info.isNormal_given || args_info.isWorkflow_given) {
        for (unsigned int i = 0; i < nets.size(); ++i) {
            cerr << names[i] << ": ";

            if (args_info.check_arg == check_arg_freechoice || args_info.isFreeChoice_given) {
                cerr << nets[i].isFreeChoice() << endl;                
            }
            
            if (args_info.check_arg == check_arg_normal || args_info.isNormal_given) {
                cerr << nets[i].isNormal() << endl;                
            }
            
            if (args_info.check_arg == check_arg_workflow || args_info.isWorkflow_given) {
                nets[i].isWorkflow();                
            }
        }
    }
    
    
    /*********
    * OUTPUT *
    *********/   
    if (args_info.output_given) {
        for (unsigned int i = 0; i < args_info.output_given; ++i) {
            string outname = names[0] + "." + args_info.output_orig[i];
            ofstream outfile(outname.c_str(), ofstream::trunc);
            if (!outfile.is_open()) {
                cerr << outname << ": could not write to file" << endl;
                exit(EXIT_FAILURE);
            }

            outfile << meta(io::OUTPUTFILE, outname);

            switch(args_info.output_arg[i]) {
                case (output_arg_owfn): {
                    outfile << io::owfn << nets[0];
                    break;
                }

                case (output_arg_sa): {
                    Automaton sa(nets[0]);
                    outfile << sa;
                    break;
                }
            }
            
            outfile.close();
        }
    }
    
    
    return EXIT_SUCCESS;
}
