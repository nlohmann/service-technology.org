#include "Tara.h"


unsigned int Tara::highestTransitionCosts = 0;
unsigned int Tara::sumOfLocalMaxCosts = 0;
unsigned int Tara::initialState = 0;
unsigned int Tara::minCosts = 0; // gna task #7709
lprec* Tara::lp = 0; 
int Tara::nrOfEdges = 0;
int Tara::nrOfFinals = 0;
std::map<pnapi::Transition*, unsigned int> Tara::partialCostFunction;
std::map<pnapi::Transition*, bool> Tara::resetMap;

pnapi::PetriNet* Tara::net = 0; 
Modification* Tara::modification = 0; 

gengetopt_args_info Tara::args_info;

std::deque<innerState *> Tara::graph;

unsigned int Tara::cost(pnapi::Transition* t) {
   std::map<pnapi::Transition*,unsigned int>::iterator cost = Tara::partialCostFunction.find(t);
   if(cost==partialCostFunction.end()) 
      return 0;

   return cost->second;
}

bool Tara::isReset(pnapi::Transition* t) {
   std::map<pnapi::Transition*, bool>::iterator r = Tara::resetMap.find(t);
   if(r == Tara::resetMap.end())  {
      return false;
   }
   return r->second;
}

/// The global temp file, used for lola & wendy calls
Output Tara::tempFile=Output();


/// check if a file exists and can be opened for reading
inline bool fileExists(const std::string& filename) {
    std::ifstream tmp(filename.c_str(), std::ios_base::in);
        return tmp.good();
	}

void Tara::evaluateParameters(int argc, char** argv) {

    //invocation string
    std::string invocation;

    // overwrite invocation for consistent error messages
    argv[0] = basename(argv[0]);

    // store invocation in a std::string for meta information in file output
    for (int i = 0; i < argc; ++i) {
        invocation += std::string(argv[i]) + " ";
    }

    // initialize the parameters structure
    struct cmdline_parser_params* params = cmdline_parser_params_create();

    // call the cmdline parser
    if (cmdline_parser(argc, argv, &Tara::args_info) != 0) {
        abort(7, "invalid command-line parameter(s)");
    }

    // debug option
    if (Tara::args_info.bug_flag) {
        {
            Output debug_output("bug.log", "configuration information");
            debug_output.stream() << CONFIG_LOG << std::flush;
        }
        message("please send file 'bug.log' to %s!", PACKAGE_BUGREPORT);
        exit(EXIT_SUCCESS);
    }

    // read a configuration file if necessary
    if (Tara::args_info.config_given) {
        // initialize the config file parser
        params->initialize = 0;
        params->override = 0;

        // call the config file parser
        if (cmdline_parser_config_file(Tara::args_info.config_arg, &Tara::args_info, params) != 0) {
            abort(14, "error reading configuration file '%s'", Tara::args_info.config_arg);
        } else {
            status("using configuration file '%s'", Tara::args_info.config_arg);
        }
    } else {
        // check for configuration files
        std::string conf_generic_filename = std::string(PACKAGE) + ".conf";
        std::string conf_filename = fileExists(conf_generic_filename) ? conf_generic_filename :
                                    (fileExists(std::string(SYSCONFDIR) + "/" + conf_generic_filename) ?
                                     (std::string(SYSCONFDIR) + "/" + conf_generic_filename) : "");

        if (conf_filename != "") {
            // initialize the config file parser
            params->initialize = 0;
            params->override = 0;
            if (cmdline_parser_config_file(const_cast<char*>(conf_filename.c_str()), &Tara::args_info, params) != 0) {
                abort(14, "error reading configuration file '%s'", conf_filename.c_str());
            } else {
                status("using configuration file '%s'", conf_filename.c_str());
            }
        } else {
            status("not using a configuration file");
        }
    }


    // check whether at most one file is given
   // if (Tara::args_info.inputs_num > 1) {
   //     abort(4, "at most one reachability graph must be given");
   // }

    free(params);
}

void Tara::constructLP() {

    // Number of rows: For each vertex, we include a row -- includes the virtual final vertex
    int NUMBER_OF_ROWS = graph.size() + 1;

    // Number of columns: For each edge, we include a column -- includes the virtual edges to the virtual final vertex
    int NUMBER_OF_COLUMNS = nrOfEdges + nrOfFinals; 

    lp = make_lp(NUMBER_OF_ROWS, NUMBER_OF_COLUMNS);
    
    set_verbose(lp, 3);    

    int currentEdge = 1;

    for (unsigned int vertexCounter = 0; vertexCounter < graph.size(); ++vertexCounter) {
    
        innerState* currentVertex = graph[vertexCounter];        

        for (unsigned int edgeCounter = 0; edgeCounter < currentVertex->transitions.size(); ++edgeCounter) {
            REAL sparsecolumn[3]; /* one element per non-zero value -- always exactly three: objective function, source vertex and target vertex. self loops are eliminated while parsing */
            int rowno[3];

            int targetVertex = (int) currentVertex->transitions[edgeCounter].successor + 1;

            rowno[0] = 0; // objective function
            rowno[1] = vertexCounter + 1; // source vertex
            rowno[2] = targetVertex; // the target vertex row
            
            sparsecolumn[0] = (REAL) currentVertex->transitions[edgeCounter].costs; // the costs for visiting the edge
            sparsecolumn[1] = -1.0; // a token is taken from the source vertex                
            sparsecolumn[2] = 1.0; // a token is put to the target vertex 

            set_binary(lp, currentEdge, TRUE); /* sets variable to binary */
            unsigned char res = set_columnex(lp, currentEdge, 3, sparsecolumn, rowno); /* changes the values of existing column 2 */
            if (res != TRUE) abort(7,"Could not set the values in the LP."); 
            ++currentEdge;
        }


        if (currentVertex->final) {
            REAL sparsecolumn[1]; /* one element per non-zero value -- always exactly two: current vertex and final vertex. */
            int rowno[1];
            rowno[0] = vertexCounter + 1;
            sparsecolumn[0] = -1.0;
            rowno[1] = NUMBER_OF_ROWS; // virtual final vertex
            sparsecolumn[1] = +1.0;
            set_binary(lp, currentEdge, TRUE); /* sets variable to binary */
            unsigned char res = set_columnex(lp, currentEdge, 2, sparsecolumn, rowno); /* changes the values of existing column 2 */ 
            if (res != TRUE) abort(7, "Could not set the values in the LP."); 
            ++currentEdge;
        }               

        set_constr_type(lp, vertexCounter+1, EQ);
        set_rh(lp, vertexCounter+1, 0);

    }
    set_rh(lp, initialState+1, -1);
    set_rh(lp, NUMBER_OF_ROWS, +1);
    set_constr_type(lp, NUMBER_OF_ROWS, EQ);
    
    set_maxim(lp);    
    
}

int Tara::solveLP() {

    solve(lp);
    int res = get_objective(lp);

    // gna task #7709
    set_minim(lp);
    solve(lp);
    int min = get_objective(lp);
    Tara::minCosts = min;
    status("Using LP lower bound: %d", min);

    return res;

}

void Tara::deleteLP() {
    delete_lp(lp);
    lp = 0;
}

void Tara::printLP() {
    print_lp(lp);

}
