#include <iostream>
#include <fstream>
#include <cstdlib>
#include <vector>
#include <string>
#include <libgen.h>
#include "pnapi.h"
#include "parser.h"
#include "cmdline.h"

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
    vector<std::string> names;
    
    
    /********
    * INPUT *
    ********/
    if (args_info.inputs_num == 0) {
        // read from stdin
        PetriNet net;
        
        try {
            std::cin >> io::owfn >> net;
        } catch (pnapi::parser::InputError error) {
            std::cerr << "stdin:" << error.line << ": " << error.message << std::endl;
            std::cerr << "stdin:" << error.line << ": last token `" << error.token << "'" << std::endl;
            
            exit(EXIT_FAILURE);
        }
        
        nets.push_back(net);
        names.push_back("stdin");
    } else {
        // read from files
        for (unsigned int i = 0; i < args_info.inputs_num; i++) {
            PetriNet net;
            std::fstream infile;
            infile.open(args_info.inputs[i], std::fstream::in);
            
            try {
                infile >> io::owfn >> net;
            } catch (pnapi::parser::InputError error) {
                std::cerr << args_info.inputs[i] << ":" << error.line << ": " << error.message << std::endl;
                std::cerr << args_info.inputs[i] << ":" << error.line << ": last token `" << error.token << "'" << std::endl;
                
                infile.close();            
                exit(EXIT_FAILURE);
            }
            
            infile.close();            
            nets.push_back(net);
            names.push_back(args_info.inputs[i]);
        }
    }
    
    
    /************************
    * STRUCTURAL PROPERTIES *
    ************************/
    if (args_info.check_given) {
        for (unsigned int i = 0; i < nets.size(); ++i) {
            std::cerr << names[i] << ": ";

            switch(args_info.check_arg) {            
                case (check_arg_freechoice):
                std::cerr << nets[i].isFreeChoice() << std::endl;
                break;

                case (check_arg_normal):
                std::cerr << nets[i].isNormal() << std::endl;
                break;

                case (check_arg_workflow):
                nets[i].isWorkflow();
                break;
            }
        }
    }
    
    
    /*********
    * OUTPUT *
    *********/   
    if (args_info.output_given) {
        for (unsigned int i = 0; i < args_info.output_given; ++i) {
            std::ofstream outfile;
            std::string outname = names[0] + "." + args_info.output_orig[i];
            outfile.open(outname.c_str());

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
