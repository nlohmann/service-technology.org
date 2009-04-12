#include <cstdlib>
#include <ctime>
#include <iostream>
#include <fstream>

#include "StoredKnowledge.h"
#include "Label.h"
#include "cmdline.h"


/// the input file
extern FILE *graph_in;

/// the parser
extern int graph_parse();


/// the command line parameters
gengetopt_args_info args_info;


/// evaluate the command line parameters
void evaluateParameters(int argc, char** argv) {
    // set default values
    cmdline_parser_init(&args_info);

    // initialize the parameters structure
    struct cmdline_parser_params *params = cmdline_parser_params_create();

    // call the cmdline parser
    cmdline_parser(argc, argv, &args_info);


    // initialize the report frequency
    if (args_info.reportFrequency_arg < 1) {
        fprintf(stderr, "%s: report frequency must be positive -- aborting\n", PACKAGE);
        exit(EXIT_FAILURE);
    }
    StoredKnowledge::reportFrequency = args_info.reportFrequency_arg;


    // check whether at most one file is given
    if (args_info.inputs_num > 1) {
        fprintf(stderr, "%s: at most one input file must be given -- aborting\n", PACKAGE);
        exit(EXIT_FAILURE);
    }


    free(params);
}


int main(int argc, char** argv) {
    time_t start_time, end_time;
    
    /*--------------------------------------.
    | 0. parse the command line parameters  |
    `--------------------------------------*/
    evaluateParameters(argc, argv);


    /*----------------------.
    | 1. parse the open net |
    `----------------------*/
    try {
        // parse either from standard input or from a given file
        if (args_info.inputs_num == 0) {
            std::cin >> pnapi::io::owfn >> *(InnerMarking::net);
        } else {
            assert (args_info.inputs_num == 1);
            std::ifstream inputStream;
            inputStream.open(args_info.inputs[0]);
            inputStream >> pnapi::io::owfn >> *(InnerMarking::net);
            inputStream.close();
        }
        if (args_info.verbose_given) {
            std::cerr << PACKAGE << ": read net " << pnapi::io::stat << *(InnerMarking::net) << std::endl;
        }
    } catch (pnapi::io::InputError error) {
        std::cerr << PACKAGE << error << std::endl;
        exit(EXIT_FAILURE);
    }
    
    // only normal nets are supported so far
    if (!InnerMarking::net->isNormal()) {
        fprintf(stderr, "%s: the input open net must be normal -- aborting\n", PACKAGE);
        exit (EXIT_FAILURE);
    }


    /*--------------------------------------------.
    | 2. initialize labels and interface markings |
    `--------------------------------------------*/
    // initialize labels
    Label::initialize();
    // initialize the interface markings with the given message bound
    InterfaceMarking::initialize(args_info.messagebound_arg);


    /*--------------------------------------------.
    | 3. write inner of the open net to LoLA file |
    `--------------------------------------------*/
    std::ofstream lolaFile;
    lolaFile.open("tmp.lola", std::ofstream::out | std::ofstream::trunc);
    lolaFile << pnapi::io::lola << *(InnerMarking::net);
    lolaFile.close();


    /*------------------------------------------.
    | 4. call LoLA and parse reachability graph |
    `------------------------------------------*/
    time(&start_time);
#if defined(HAVE_POPEN) && defined(HAVE_PCLOSE)
    // use a pipe
    graph_in = popen("lola-full tmp.lola -M 2> /dev/null", "r");
    graph_parse();
    pclose(graph_in);
    system("rm -f tmp.lola");
#else
    // use a file
    system("lola-full tmp.lola -m &> /dev/null");
    graph_in = fopen("tmp.graph", "r");
    graph_parse();
    fclose(graph_in);
    system("rm -f tmp.graph tmp.lola");
#endif
    time(&end_time);


    /*-------------------------------.
    | 5. organize reachability graph |
    `-------------------------------*/
    InnerMarking::initialize();
    delete InnerMarking::net;

    if (args_info.verbose_given) {
        fprintf(stderr, " [%.0f sec]\n", difftime(end_time, start_time));
    }


    /*-------------------------------.
    | 6. calculate knowledge bubbles |
    `-------------------------------*/
    time(&start_time);
    Knowledge *K0 = new Knowledge(0);
    StoredKnowledge::root = new StoredKnowledge(K0);
    StoredKnowledge::root->store();

    StoredKnowledge::calcRecursive(K0, StoredKnowledge::root);
    delete K0;
    time(&end_time);

    if (args_info.verbose_given) {
        fprintf(stderr, "%s: stored %d knowledges [%.0f sec]\n",
            PACKAGE, StoredKnowledge::storedKnowledges, difftime(end_time, start_time));
        fprintf(stderr, "%s: used %d of %d hash buckets, maximal bucket size: %d\n",
            PACKAGE, StoredKnowledge::hashTree.size(), (1 << (8*sizeof(hash_t))), StoredKnowledge::maxBucketSize);
    }


    /*----------------------------.
    | 7. add predecessor relation |
    `----------------------------*/
    time(&start_time);
    unsigned int edges = StoredKnowledge::addPredecessors();
    time(&end_time);

    if (args_info.verbose_given) {
        fprintf(stderr, "%s: added predecessor relation (%d edges) [%.0f sec]\n",
            PACKAGE, edges, difftime(end_time, start_time));
    }


    /*----------------------------------.
    | 8. detect and delete insane nodes |
    `----------------------------------*/
    time(&start_time);
    unsigned int redNodes = StoredKnowledge::removeInsaneNodes();
    time(&end_time);
    
    if (args_info.verbose_given) {
        fprintf(stderr, "%s: removed %d red nodes in %d iterations [%.0f sec]\n",
            PACKAGE, redNodes, StoredKnowledge::iterations, difftime(end_time, start_time));
    }

    // analyze root node
    fprintf(stderr, "%s: net is controllable: %s\n",
        PACKAGE, (StoredKnowledge::root->is_sane)?"YES":"NO");


    /*------------------.
    | 9. output options |
    `------------------*/
    // dot output
    if (args_info.dot_given) {
        StoredKnowledge::dot(args_info.showEmptyNode_given, args_info.formula_arg);
    }
    
    // operating guidelines output
    if (args_info.og_given) {
        StoredKnowledge::OGoutput();
    }
    

    return EXIT_SUCCESS;
}
