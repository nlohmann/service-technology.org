#include "pnapi.h"
#include "cdnf-cmdline.h"
#include "config.h"
#include "verbose.h"
#include "Output.h"

#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <string>
#include <libgen.h>

//*
using std::cerr;
using std::endl;
//*/
using std::vector;
using std::map;
using std::string;
using std::ifstream;
using std::ofstream;
using std::ostringstream;
using std::set;
using namespace pnapi;

/// the command line parameters
gengetopt_args_info args_info;

/// a variable holding the time of the call
clock_t start_clock = clock();

/// the net
PetriNet net;

/// name of the input file
string filename;

/// the invocation string
string invocation;

/// LoLA reachability graph
extern FILE * yyin;

/// set of reachable final markings
extern set<Marking*> finalMarkings;

// the parser
extern int yyparse();
extern int yylex_destroy();

struct FileObject {
    string filename;

    PetriNet *net;

    FileObject(string f) : filename(f), net(NULL) { }
};


/// evaluate the command line parameters
void evaluateParameters(int argc, char** argv) {
    // overwrite invocation for consistent error messages
    argv[0] = basename(argv[0]);

    // store invocation in a std::string for meta information in file output
    for (int i = 0; i < argc; ++i) {
        invocation += std::string(argv[i]) + " ";
    }
      
    // initialize the parameters structure
    struct cmdline_parser_params *params = cmdline_parser_params_create();

    // call the cmdline parser
    if (cmdline_parser(argc, argv, &args_info) != 0) {
        abort(1, "invalid command-line parameter(s)");
    }

    // check whether at most one file is given
    if (args_info.inputs_num > 1) {
        abort(4, "at most one input file must be given");
    }
    
    free(params);
}


/// a function collecting calls to organize termination (close files, ...)
void terminationHandler() {
    // print statistics
    if (args_info.stats_flag) {
        message("runtime: %.2f sec", (static_cast<double>(clock()) - static_cast<double>(start_clock)) / CLOCKS_PER_SEC);
        fprintf(stderr, "%s%s%s: memory consumption: ", _cm_, PACKAGE, _c_);
        int doNotCare = system((std::string("ps -o rss -o comm | ") + TOOL_GREP + " " + PACKAGE + " | " + TOOL_AWK + " '{ if ($1 > max) max = $1 } END { print max \" KB\" }' 1>&2").c_str());
        doNotCare = doNotCare; // get rid of compiler warning
    }
}


int main(int argc, char** argv) {
  
    /*----------------.
    | 0. initializing |
    `----------------*/
  
    // set the function to call on normal termination
    atexit(terminationHandler);
  
    evaluateParameters(argc, argv);
    Output::setTempfileTemplate(args_info.tmpfile_arg);
    Output::setKeepTempfiles(args_info.noClean_flag);


    vector<FileObject> objects;

    /*----------------------.
    | 1. parse the open net |
    `----------------------*/
    try {
        // parse either from standard input or from a given file
        if (args_info.inputs_num == 0) {
            std::cin >> meta(io::INVOCATION, invocation) >> pnapi::io::owfn >> net;
            filename = "net";
        } else {
            // strip suffix from input filename
            filename = std::string(args_info.inputs[0]).substr(0, std::string(args_info.inputs[0]).find_last_of("."));

            std::ifstream inputStream(args_info.inputs[0]);
            if (!inputStream) {
                abort(1, "could not open file '%s'", args_info.inputs[0]);
            }
            inputStream >> meta(pnapi::io::INPUTFILE, args_info.inputs[0])
                        >> meta(io::INVOCATION, invocation)
                        >> pnapi::io::owfn >> net;
        }
        if (args_info.verbose_flag) {
            std::ostringstream s;
            s << pnapi::io::stat << net;
            status("read net: %s", s.str().c_str());
        }
    } catch(pnapi::exception::InputError error) {
        std::ostringstream s;
        s << error;
        abort(2, "\b%s", s.str().c_str());
    }
 
    if (net.getTransitions().empty()) {
        abort(0, "net has no transitions -- only reachable marking is the initial marking");
    }
    
    /*--------------------------------------------.
    | 2. write inner of the open net to LoLA file |
    `--------------------------------------------*/
    Output * temp = new Output();
    temp->stream() << pnapi::io::lola << net;
    
    /*------------------------------------------.
    | 3. call LoLA and parse reachability graph |
    `------------------------------------------*/
    // select LoLA binary and build LoLA command
#if defined(__MINGW32__)
    // MinGW does not understand pathnames with "/", so we use the basename
    std::string command_line = std::string(basename(args_info.lola_arg)) + " " + temp->name() + " -M" + (args_info.verbose_flag ? "" : " 2> nul");
#else
    std::string command_line = std::string(args_info.lola_arg) + " " + temp->name() + " -M" + (args_info.verbose_flag ? "" : " 2> /dev/null");
#endif

    time_t start_time, end_time;
    
    // call LoLA
    status("calling %s: '%s'", _ctool_("LoLA"), command_line.c_str());
    time(&start_time);
    yyin = popen(command_line.c_str(), "r");
    yyparse();
    pclose(yyin);
    yylex_destroy();
    time(&end_time);
    status("%s is done [%.0f sec]", _ctool_("LoLA"), difftime(end_time, start_time));
    delete temp;
    
    /*-------------------------------.
    | 4. recalculate final condition |
    `-------------------------------*/
    net.getFinalCondition() = false;
    for(set<Marking*>::iterator m = finalMarkings.begin();
         m != finalMarkings.end(); ++m)
    {
      net.getFinalCondition().addMarking(**m);
      delete (*m);
    }
    
    /*------------------.
    | 5. output options |
    `------------------*/
    
    Output output(filename + ".cdnf.owfn", "open net");
    output.stream() << pnapi::io::owfn << net;

    /*-----------.
    | 6. cleanup |
    `-----------*/

    cmdline_parser_free(&args_info);

    return EXIT_SUCCESS;
}
