#include <cstdio>                  /* for FILE io */
#include <cstdlib>
#include "cmdline.h"
#include "verbose.h"
#include <libgen.h>
#include <unistd.h>
#include <sstream>
#include <iostream>
#include <istream>
#include <fstream>
#include <csignal>
#include <string.h>

#include <pnapi/pnapi.h>
#include "imatrix.h"
#include "rules.h"
#include "Runtime.h"

using pnapi::PetriNet;

/// the command line parameters
//+ gengetopt_args_info args_info;
//+ extern const char *cmdline_parser_profile_values[];

/// the invocation string
//+ std::string invocation;

//+ FILE *outfile = NULL;

void* threadReduce(void*);
void translatePaths(JSON& item, JSON& dict);
void translate(JSON& item, JSON& plist, vector<string>& out);

/// transfer struct for the start of a parallel search thread
struct tpArguments {
    /// the number of the current thread
    int threadNumber;
    /// the incidence matrix
    IMatrix* im;
	/// the reduction rules
	Rules* rules;
};


int main(int argc, char* argv[]) {
    Runtime::init(argc, argv);

    if (Runtime::args_info.inputs_num > 1) {
        abort(5, "at most one input file can be given");
    }

//    status("using profile %s", _ctool_(cmdline_parser_profile_values[Runtime::args_info.profile_arg]));

    
    /*--------------------------.
    | 1. read a given JSON file |
    `--------------------------*/

	JSON json;
	if (Runtime::args_info.readjson_given)
	{
		std::ifstream inputStream(Runtime::args_info.readjson_arg);
        if (!inputStream)
            abort(1, "could not open file '%s'", Runtime::args_info.readjson_arg);

		inputStream >> json;

        // additional output if verbose-flag is set
        if (Runtime::args_info.verbose_flag)
            status("read JSON properties file: %s", Runtime::args_info.readjson_arg);
	}

    /*-----------------------------.
    | 2. Obtain and Translate path |
    `-----------------------------*/

	if (Runtime::args_info.readjson_given)
	{
		JSON paths;
        if (Runtime::args_info.inputs_num == 0)
			abort(11,"missing input file for path");
		else {
			const char* filename(Runtime::args_info.inputs[0]);
            std::ifstream inputStream(filename);
            if (!inputStream)
                abort(1, "could not open file '%s'", filename);
			inputStream >> paths;
	        if (Runtime::args_info.verbose_flag)
            	status("read JSON path file: %s", filename);
		}

		translatePaths(paths,json);

		// if we want the translation in a JSON file:
	    if (Runtime::args_info.writejson_given) {
			// open JSON output stream
	        std::ofstream outputStream(Runtime::args_info.writejson_arg);
	        // ... and abort, if an error occurs
	        if (!outputStream) {
	            abort(1, "could not open file '%s'", Runtime::args_info.writejson_arg);
	        }
			outputStream << paths.toString() << std::endl;
		    if (Runtime::args_info.verbose_flag) {
	    	    status("write JSON path file: %s", Runtime::args_info.writejson_arg);
	    	}		
		}

		return EXIT_SUCCESS;
	}

    /*-----------------------.
    | 3. parse the Petri net |
    `-----------------------*/

    PetriNet net;

    try {
        // parse either from standard input or from a given file
        if (Runtime::args_info.inputs_num == 0) {
            // parse the open net from standard input
            if (Runtime::args_info.pnml_given) 
				std::cin >> pnapi::io::pnml >> net;
            else if (Runtime::args_info.lola_given) 
				std::cin >> pnapi::io::lola >> net;
            else 
				std::cin >> pnapi::io::owfn >> net;
        } else {
            // open input file as an input file stream
			unsigned int len(0);
			const char* filename(Runtime::args_info.inputs[0]);
            std::ifstream inputStream(filename);
            // ... and abort, if an error occurs
            if (!inputStream)
                abort(1, "could not open file '%s'", filename);

            // parse the open net from the input file stream
			if (Runtime::args_info.pnml_given)
	            inputStream >> meta(pnapi::io::INPUTFILE, filename)
    	                    >> pnapi::io::pnml >> net;
			else if (Runtime::args_info.lola_given)
	            inputStream >> meta(pnapi::io::INPUTFILE, filename)
    	                    >> pnapi::io::lola >> net;
			else 
	            inputStream >> meta(pnapi::io::INPUTFILE, filename)
    	                    >> pnapi::io::owfn >> net;
        }
        // additional output if verbose-flag is set
        if (Runtime::args_info.verbose_flag) {
            std::ostringstream s;
            s << pnapi::io::stat << net;
            status("read net: %s", s.str().c_str());
        }
    } catch (const pnapi::exception::InputError& error) {
        std::ostringstream s;
        s << error;
        abort(2, "\b%s", s.str().c_str());
    }


    /*--------------------------------.
    | 4. Transform Net and Parameters |
    `--------------------------------*/

	JSON log;
	IMatrix im(net,log);

	if (Runtime::args_info.allplaces_given) im.makeVisible(false);
	if (Runtime::args_info.alltransitions_given) im.makeVisible(true);
	for(unsigned int i=0; i<Runtime::args_info.persistent_given; ++i)
		im.makePersistent(string(Runtime::args_info.persistent_arg[i]));
	for(unsigned int i=0; i<Runtime::args_info.seen_given; ++i)
		im.makeVisible(string(Runtime::args_info.seen_arg[i]));

	uint64_t cond(0);
	if (Runtime::args_info.ctl_given) cond |= Facts::CTL;
	if (Runtime::args_info.ctlx_given) cond |= Facts::CTLX;
	if (Runtime::args_info.ltl_given) cond |= Facts::LTL;
	if (Runtime::args_info.final_given || Runtime::args_info.ltlx_given) cond |= Facts::LTLX;
	if (Runtime::args_info.altl_given) cond |= Facts::ALTL;
	if (Runtime::args_info.altlx_given) cond |= Facts::ALTLX;
	if (Runtime::args_info.live_given || Runtime::args_info.LIVE_given) cond |= Facts::LIVENESS;
	if (Runtime::args_info.nonlive_given || Runtime::args_info.LIVE_given) cond |= Facts::NONLIVE;
	if (Runtime::args_info.bounded_given || Runtime::args_info.BOUNDED_given) cond |= Facts::BOUNDEDNESS;
	if (Runtime::args_info.nonbounded_given || Runtime::args_info.BOUNDED_given) cond |= Facts::UNBOUNDED;
	if (Runtime::args_info.safe_given || Runtime::args_info.SAFE_given) cond |= Facts::SAFETY;
	if (Runtime::args_info.unsafe_given || Runtime::args_info.SAFE_given) cond |= Facts::UNSAFE;
	if (Runtime::args_info.path_given) cond |= Facts::PATHS;
	if (Runtime::args_info.complexpath_given) cond |= Facts::COMPLEXPATHS;
	if (Runtime::args_info.bisimulation_given) cond |= Facts::BISIM;
	if (Runtime::args_info.reversibility_given) cond |= Facts::REVERSE;
	if (Runtime::args_info.marking_given) cond |= Facts::INVARIANT;
	Facts facts(im,cond);

    /*------------------------.
    | 5. Reduce the Petri net |
    `------------------------*/

	Rules rules(im,facts);

	if (!Runtime::args_info.threads_given || Runtime::args_info.threads_arg<2)
	{
		tpArguments args;
        args.threadNumber = 0;
        args.im = &im;
		args.rules = &rules;
		threadReduce(&args);

	} else {
		// allocate space for thread arguments
	    tpArguments* args = (tpArguments*) calloc(Runtime::args_info.threads_arg,
	                           sizeof(tpArguments));
	    for (int i = 0; i < Runtime::args_info.threads_arg; ++i) {
	        args[i].threadNumber = i;
	        args[i].im = &im;
			args[i].rules = &rules;
	    }

		// allocate space for threads
	    pthread_t* threads = (pthread_t*) calloc(Runtime::args_info.threads_arg, sizeof(pthread_t));

	    // start the threads
	    for (int i = 0; i < Runtime::args_info.threads_arg; i++)
	        if (UNLIKELY(pthread_create(threads + i, NULL, threadReduce, args + i))) {
	            // LCOV_EXCL_START
	            abort(3,"threads could not be created");
	            // LCOV_EXCL_STOP
	        }

	    //// THREADS ARE RUNNING AND SEARCHING

	    // wait for all threads to finish
	    for (int i = 0; i < Runtime::args_info.threads_arg; i++) {
	        void* return_value;
	        if (UNLIKELY(pthread_join(threads[i], &return_value))) {
	            // LCOV_EXCL_START
	            abort(4,"threads could not be joined");
	            // LCOV_EXCL_STOP
	        }
	    }
	}

    /*-----------------------.
    | 6. Write the Petri net |
    `-----------------------*/

	// get the new net
	PetriNet* net2(im.exportNet(facts));

    // write either to standard output or to given files
    if (!Runtime::args_info.outputnet_given) {
        // parse the net to standard output
        if (Runtime::args_info.pnml_given)      std::cout << pnapi::io::pnml << *net2;
		else if (Runtime::args_info.lola_given) std::cout << pnapi::io::lola << *net2;
		else                           std::cout << pnapi::io::owfn << *net2;
    } else {
        // open output file as an output file stream
        std::ofstream outputStream(Runtime::args_info.outputnet_arg);
        // ... and abort, if an error occurs
        if (!outputStream) {
            abort(1, "could not open file '%s'", Runtime::args_info.outputnet_arg);
        }
        // print the first component to the output file stream
		if (Runtime::args_info.pnml_given)
            outputStream << meta(pnapi::io::OUTPUTFILE, Runtime::args_info.outputnet_arg)
   	                    << pnapi::io::pnml << *net2;
		else if (Runtime::args_info.lola_given)
            outputStream << meta(pnapi::io::OUTPUTFILE, Runtime::args_info.outputnet_arg)
   	                    << pnapi::io::lola << *net2;
		else
            outputStream << meta(pnapi::io::OUTPUTFILE, Runtime::args_info.outputnet_arg)
   	                    << pnapi::io::owfn << *net2;
	    // additional output if verbose-flag is set
	    if (Runtime::args_info.verbose_flag) {
	        std::ostringstream s;
	        s << pnapi::io::stat << *net2;
	        status("write net: %s", s.str().c_str());
	    }
	}
	delete net2;

	if (!Runtime::args_info.writejson_given)
		facts.printFacts();
	else {
        std::ofstream outputStream2(Runtime::args_info.writejson_arg);
        // ... and abort, if an error occurs
        if (!outputStream2) {
            abort(1, "could not open file '%s'", Runtime::args_info.writejson_arg);
        }
		facts.printFacts(log);
		if (Runtime::args_info.inputs_num > 0)
			log["net"] = string(Runtime::args_info.inputs[0]);
		if (Runtime::args_info.outputnet_given)
			log["reduced net"] = string(Runtime::args_info.outputnet_arg);
		outputStream2 << log.toString() << std::endl;
	    if (Runtime::args_info.verbose_flag) {
    	    status("write JSON properties file: %s", Runtime::args_info.writejson_arg);
    	}
	}

    return EXIT_SUCCESS;
}

void* threadReduce(void* args) {
	unsigned int id(((tpArguments*)args)->threadNumber);
	IMatrix* im(((tpArguments*)args)->im);
	Rules* rules(((tpArguments*)args)->rules);
	unsigned int mode;
	while ((mode = im->findMode(*rules,id)) != Rules::MAXMODE)
		rules->apply(mode,id);
}

void translate(JSON& item, JSON& plist, vector<string>& out) {
	switch (item.type()) {
		case JSON::string:
			out.push_back(item);
			break;
		case JSON::object: {
			if (item[(string)"*if*"].type() != JSON::array)
				abort(9,"error in JSON translation file: missing array after 'if'");
			// collect pre/postset arcs of the inspected place(s)
			unsigned int size(item[(string)"*if*"].size());
			map<string,int> cnt;
			for(int i=0; i<size; ++i)
			{
				string tmp = item["*if*"][i];
				JSON::iterator jit;
				for(jit=plist[tmp].begin(); jit!=plist[tmp].end(); ++jit)
					cnt[(string)jit.key()] += (int)jit.value();
			}
			// calculate actual token number on the inspected place(s)
			int pcnt(cnt["*init*"]);
			for(int i=0; i<out.size(); ++i)
				pcnt += cnt[out[i]];
			// select the correct transition list to add to the path
			JSON& next(item[(string)(pcnt>0 ? "then" : "else")]);
			if (next.type() != JSON::array)
				abort(9,"error in JSON translation file: missing array after '%s'",(pcnt>0?"then":"else"));
			// add each single item recursively
			for(int i=0; i<next.size(); ++i)
				translate(next[i],plist,out);
			break; }
		default:
			abort(9,"error in JSON translation file: string or object expected");
	}
}

JSON translateOnePath(JSON& item, JSON& dict) {
		unsigned int len(item.size());
		vector<string> out;
		unsigned int plen(dict["path"]["*init*"].size());
		for(unsigned int j=0; j<plen; ++j)
			translate(dict["path"]["*init*"][j],dict["xchange"],out);
		for(unsigned int i=0; i<len; ++i)
		{
			string tmp = item[i];
			unsigned int plen(dict["path"][tmp].size());
			if (plen==0) out.push_back(tmp);
			else for(unsigned int j=0; j<plen; ++j)
					translate(dict["path"][tmp][j],dict["xchange"],out);
		}
		JSON result;
		for(unsigned int i=0; i<out.size(); ++i)
			result += out[i];
	    if (!Runtime::args_info.writejson_given) {
			std::cout << "Path: ";
			for(unsigned int i=0; i<out.size(); ++i)
				std::cout << out[i] << " ";
			std::cout << endl;
		}
		return result;
}

void translatePaths(JSON& item, JSON& dict) {
	switch(item.type()) {
		case JSON::object:
			for(JSON::iterator jit=item.begin(); jit!=item.end(); ++jit)
			{
				if (jit.key() == Runtime::args_info.keyword_arg && jit.value().type() == JSON::array)
					item[(string)Runtime::args_info.keyword_arg] = translateOnePath(jit.value(),dict);
				else
					translatePaths(jit.value(),dict);
			}
			break;
		case JSON::array:
			for(JSON::iterator jit=item.begin(); jit!=item.end(); ++jit)
				translatePaths(jit.value(),dict);
			break;
		default: break;
	}
}

