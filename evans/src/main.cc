/*****************************************************************************\
 Evans -- Equivalent public View for ANalyzing service Substitution.

 Copyright (C) 2011  Jarungjit Parnjai

 Evans is free software: you can redistribute it and/or modify it under the
 terms of the GNU Affero General Public License as published by the Free
 Software Foundation, either version 3 of the License, or (at your option)
 any later version.

 Evans is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for
 more details.

 You should have received a copy of the GNU Affero General Public License
 along with Maxis.  If not, see <http://www.gnu.org/licenses/>.
\*****************************************************************************/

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <map>
#include <set>
#include <string>
#include <queue>

#include <config.h>
#include "cmdline.h"
#include "util.h"
#include "formula.h"
#include "verbose.h"

using std::cerr;
using std::cout;
using std::endl;
using std::ofstream;
using std::ostringstream;
using std::stringstream;
using std::map;
using std::multimap;
using std::pair;
using std::string;
using std::set;
using std::ostream;

// the command line parameters
gengetopt_args_info args_info;

// lexer and parser
extern int yyparse();
extern int yylex_destroy();
extern FILE* yyin;

/// a variable holding the time of the call
clock_t start_clock = clock();


/// output stream
std::ostream* myOut = &cout;

/****************************
 * Variables used by parser *
 ****************************/
/// non deterministic successor list of recent OG
map<unsigned int, multimap<string, unsigned int> > succ;
/// annotations of recent OG
map<unsigned int, Formula *> formulae;
/// input labels of recent OG
set<string> inputs;
/// output labels of recent OG
set<string> outputs;
/// synchronous labels of recent OG
set<string> synchronous;
/// present labels of recent OG
map<unsigned int, set<string> > presentLabels;
/// initial node ID of recent OG
unsigned int * initialID = NULL;

/***************************
 * Variables used by Megan *
 ***************************/
enum mode { pvp, pvs, ogp, ogs, eg };

std::string liberal_flag = "0";
std::list<mode> output_mode;
mode current_mode;

std::string wendy_str = "wendy";
std::string maxis_str = "maxis";
std::string pnapi_str = "petri";

/// the invocation string
string invocation;

/****************************************
 * evaluate the command line parameters *
 ****************************************/
void evaluate_parameters(int argc, char** argv)
{
	// initialize the parameters structure
	struct cmdline_parser_params *params = cmdline_parser_params_create();


	// call the cmdline parser
	if (cmdline_parser(argc, argv, &args_info) != 0) {
		abort(7, "invalid command-line parameter(s)");
	}

    // store invocation in a std::string for meta information in file output
    for (int i = 0; i < argc; ++i) {
        invocation += (i == 0 ? "" : " ") + std::string(argv[i]);
    }

	// if none of input OG files is given
	if(args_info.inputs_num == 0 ) {
		message("ERROR : an OG input file must be given");
		status("terminated with failure!");
		exit(EXIT_FAILURE);
	}

    if (args_info.ogp_given) { output_mode.push_back( ogp );  }
    if (args_info.pvp_given) { output_mode.push_back( pvp );  }
    if (args_info.ogs_given) { output_mode.push_back( ogs ); ;  }
    if (args_info.pvs_given) { output_mode.push_back( pvs );  }
    if (args_info.eg_given) {
    	output_mode.push_back( eg );
        message("sorry, --eg is not yet implemented!");
        exit(EXIT_SUCCESS);
    }

	std::string outstr;
    if (!output_mode.empty()) {
    	 for(std::list<mode>::iterator it = output_mode.begin();
    			 it!=output_mode.end();  ++it) {
        	mode m = *it;
        	switch (m) {
            	case (ogp) : outstr += "ogp "; break;
            	case (pvp) : outstr += "pvp ";break;
            	case (ogs) : outstr += "ogs ";break;
            	case (pvs) : outstr += "pvs ";break;
            	case (eg) :
            			outstr += "eg(not yet implemented) ";
            	        break;
            	default : outstr += " ";break;
        	}
    	 }
    } else {
    	outstr = std::string("ogp pvp ogs pvs");
    	output_mode.push_back( ogp );
    	output_mode.push_back( ogp );
    	output_mode.push_back( ogp );
    	output_mode.push_back( pvs );
//  	output_mode.push_back( eg );
    }


	if(args_info.liberal_given) {
		if (args_info.liberal_arg < 0 or args_info.liberal_arg > 2) {
	        abort(8, "liberal mode must be 0, 1, or 2");
		}
		switch (args_info.liberal_arg) {
		   case 0 :  liberal_flag = std::string("0"); break;
   		   case 1 :  liberal_flag = std::string("1");  break;
		   case 2 :  liberal_flag = std::string("2");   break;
		   default : liberal_flag = "0";  break;
		}
	}

    if (args_info.verbose_flag) {
       message("synthesis options: %s", outstr.c_str());
    }
    // message bound
    if (args_info.messagebound_arg <= 0) {
        abort(8, "message bound must be positive");
    }

    free(params);
}


/***********************
 * parse OG input file *
 ***********************/
void processing_input()
{
	std::string filename = std::string(PACKAGE) + "_output";
    // strip suffix from input filename
    filename = std::string(args_info.inputs[0]).substr(0, std::string(args_info.inputs[0]).find_last_of("."));

    std::ifstream inputStream(args_info.inputs[0]);
    if (!inputStream) {
        abort(1, "could not open file '%s'", args_info.inputs[0]);
    }

    //---------------------------
    // 1. always start with ogp
    //---------------------------
    // assert : output_mode is not empty
    if (output_mode.front() == ogp)  output_mode.pop_front();

    current_mode = ogp;
    if (args_info.verbose_flag) {
        message("current mode: generating partners's OG for '%s'",_ctool_(args_info.inputs[0]));
    }

    std::string input_file = std::string(args_info.inputs[0]);
	std::string output_file = filename + ".og";
    const std::string wendy_command_line =  wendy_str + " " + input_file + " --og " //+ "--messagebound=" + args_info.messagebound_arg
    		     		                              + (args_info.verbose_flag ? " -v" : " 2> /dev/null");
//    const std::string command_line =  wendy_str + " -v " + std::string(args_info.inputs[0]) + " --og=-" + (args_info.verbose_flag ? "" : " 2> /dev/null");

    time_t start_time, end_time;
	// call Wendy
    status("calling %s with: '%s'", _ctool_(wendy_str), wendy_command_line.c_str());
    time(&start_time);

    FILE *stream  = popen(wendy_command_line.c_str(), "r");
    // link input file pointer
 //   yyin = stream;
     /// actual parsing
 //   yyparse();
    pclose(stream);
    time(&end_time);

    status("%s is done [%.0f sec] and '%s' has been created.", _ctool_(wendy_str),  difftime(end_time, start_time), _ctool_(output_file) );

    if (output_mode.empty()) 	return;

    //---------------------------
    // 2. generating pvp
    //---------------------------
    // assert : output_mode is not empty
    if (output_mode.front() == pvp)  output_mode.pop_front();

    current_mode = pvp;
    if (args_info.verbose_flag) {
        message("current mode: generating a partner's PV for '%s'",_ctool_(args_info.inputs[0]));
    }

    input_file = output_file;
	output_file = filename + ".m.sa";
    //std::string maxis_str =  "maxis  bits.og  --liberal=0";
    const std::string maxis_command_line =  maxis_str + " " + input_file
    		                                          + " --output=" + output_file
    		                                          + " --liberal=" +  liberal_flag
    		                                          + (args_info.verbose_flag ? " -v" : " 2> /dev/null");

	// call Wendy
    status("calling %s with: '%s'", _ctool_("maxis"), maxis_command_line.c_str());
    time(&start_time);

    stream  = popen(maxis_command_line.c_str(), "r");

 //   char buffer[512];
//    string data;
//    while ( fgets(buffer, 512, stream) != NULL )
//      data.append(buffer);
    //    cout << data << endl;
    //    data.clear();
    pclose(stream);
    time(&end_time);

    status("%s is done [%.0f sec] and '%s' has been created.", _ctool_("maxis"),  difftime(end_time, start_time), _ctool_(output_file) );

    if (output_mode.empty()) 	return;

    //-------------------------------
    // 3.1. generating owfn from .sa
    //-------------------------------
    // assert : output_mode is not empty
    if (output_mode.front() == ogs)  output_mode.pop_front();

    input_file = output_file;
    output_file = filename + ".m.sa.owfn";
    const std::string pnapi_command_line =  pnapi_str + " " + input_file + " --input=sa --output=owfn --removePorts"
    		                                          + (args_info.verbose_flag ? " -v" : " 2> /dev/null");

	// call pnapi's petri
    status("calling %s with: '%s'", _ctool_(pnapi_str), pnapi_command_line.c_str());
    time(&start_time);

    stream  = popen(pnapi_command_line.c_str(), "r");
    pclose(stream);
    time(&end_time);

    status("%s is done [%.0f sec] and '%s' has been created.", _ctool_(pnapi_str),  difftime(end_time, start_time), _ctool_(output_file) );

    //---------------------------
    // 3.2 generating ogs
    //---------------------------
	current_mode = ogs;
    if (args_info.verbose_flag) {
     	message("current mode: generating substituion's OG for services of '%s'",_ctool_(args_info.inputs[0]));
    }

    input_file = filename + ".m.sa.owfn" ;
	output_file = filename + ".m.sa.og";
    const std::string wendy_command_line_2 =  wendy_str + " " + input_file + " --og "
    		                                            + (args_info.verbose_flag ? " -v" : " 2> /dev/null");

	// call Wendy
    status("calling %s with: '%s'", _ctool_(wendy_str), wendy_command_line_2.c_str());
    time(&start_time);

    stream  = popen(wendy_command_line_2.c_str(), "r");
    pclose(stream);
    time(&end_time);

    status("%s is done [%.0f sec] and '%s' has been created.", _ctool_(wendy_str),  difftime(end_time, start_time), _ctool_(output_file) );

    if (output_mode.empty()) 	return;

    //---------------------------
    // 4. generating pvs
    //---------------------------
    if (output_mode.front() == pvs)  output_mode.pop_front();

	current_mode = pvs;
    if (args_info.verbose_flag) {
     	message("current mode: generating a service's PV for %s'",_ctool_(args_info.inputs[0]));
    }

    input_file = output_file;
	output_file = filename + ".m.m.sa";
    //std::string maxis_str =  "maxis  bits.og  --liberal=0";
    const std::string maxis_command_line_2 =  maxis_str + " " + input_file
    		                                            + " --output=" + output_file
    		                                            + " --liberal=" + liberal_flag
    		                                            + (args_info.verbose_flag ? " -v" : " 2> /dev/null");

	// call Wendym
    status("calling %s with: '%s'", _ctool_("maxis"), maxis_command_line_2.c_str());
    time(&start_time);

    stream  = popen(maxis_command_line_2.c_str(), "r");
    pclose(stream);
    time(&end_time);

    status("%s is done [%.0f sec] and '%s' has been created.", _ctool_("maxis"),  difftime(end_time, start_time), _ctool_(output_file) );

    if (output_mode.empty())
    	return;

    return;
}


void terminationHandler() {
    if (args_info.stats_flag) {
        message("runtime: %s%.2f sec%s", _bold_, (static_cast<double>(clock()) - static_cast<double>(start_clock)) / CLOCKS_PER_SEC, _c_);
        std::string call = std::string("ps -o rss -o comm | ") + TOOL_GREP + " " + PACKAGE + " | " + TOOL_AWK + " '{ if ($1 > max) max = $1 } END { print max \" KB\" }'";
        FILE* ps = popen(call.c_str(), "r");
        unsigned int memory;
        int res = fscanf(ps, "%u", &memory);
        assert(res != EOF);
        pclose(ps);
        message("memory consumption: %s%u KB %s", _bold_, memory, _c_);

    }
}

void finalize()
{
    /// close input
    fclose(yyin);
  
    /// clean lexer memory
    yylex_destroy();
}


/*****************
 * main function *
 *****************/
int main(int argc, char** argv)
{
	// set the function to call on normal termination
    atexit(terminationHandler);

    // parse the command line parameters
    if (args_info.verbose_flag) {
    	message("evaluating command line parameters... ");
    }
    evaluate_parameters(argc, argv);

    // parse the command line parameters
    if (args_info.verbose_flag) {
        message("processing input... ");
    }
    processing_input();

    if (args_info.verbose_flag) {
    	message("successfully terminated.");
    }
    exit(EXIT_SUCCESS); // finished parsing
}

