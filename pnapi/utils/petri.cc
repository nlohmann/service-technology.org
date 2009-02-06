#include <iostream>
#include <fstream>
#include <cstdlib>
#include <vector>
#include <string>
#include <libgen.h>
#include "pnapi.h"
#include "cmdline.h"

using namespace pnapi;

using pnapi::io::owfn;
using pnapi::io::meta;
using pnapi::io::FILENAME;
using pnapi::io::InputError;

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
    vector<std::string> names;
    
    
    /********
    * INPUT *
    ********/
    if (args_info.inputs_num == 0) {
        // read from stdin
        PetriNet net;
        
        try {
            std::cin >> meta(FILENAME, "<stdin>") >> owfn >> net;
        } catch (InputError error) {
            std::cerr << error << std::endl;
            exit(EXIT_FAILURE);
        }

        if (args_info.verbose_given) {
            std::cerr << "<stdin>: " << net << std::endl;
        }
        
        nets.push_back(net);
        names.push_back("<stdin>");
    } else {
        // read from files
        for (unsigned int i = 0; i < args_info.inputs_num; i++) {
            PetriNet net;

            std::fstream infile(args_info.inputs[i], std::fstream::in);
            if (!infile.is_open()) {
                std::cerr << args_info.inputs[i] << ": could not open file to read" << std::endl;
                exit(EXIT_FAILURE);
            }
            
            try {
                infile >> meta(FILENAME, args_info.inputs[i]) >> owfn >> net;
            } catch (InputError error) {
                std::cerr << error << std::endl;
                infile.close();            
                exit(EXIT_FAILURE);
            }
            
            infile.close();
            
            if (args_info.verbose_given) {
                std::cerr << args_info.inputs[i] << ": " << net << std::endl;
            }
            
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
            std::cerr << names[i] << ": ";

            if (args_info.check_arg == check_arg_freechoice || args_info.isFreeChoice_given) {
                std::cerr << nets[i].isFreeChoice() << std::endl;                
            }
            
            if (args_info.check_arg == check_arg_normal || args_info.isNormal_given) {
                std::cerr << nets[i].isNormal() << std::endl;                
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
            std::string outname = names[0] + "." + args_info.output_orig[i];
            std::ofstream outfile(outname.c_str(), std::fstream::trunc);
            if (!outfile.is_open()) {
                std::cerr << outname << ": could not write to file" << std::endl;
                exit(EXIT_FAILURE);
            }

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
