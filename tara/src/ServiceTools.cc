/*****************************************************************************\
 Hello -- <<-- Hello World -->>

 Copyright (c) <<-- 20XX Author1, Author2, ... -->>

 Hello is free software: you can redistribute it and/or modify it under the
 terms of the GNU Affero General Public License as published by the Free
 Software Foundation, either version 3 of the License, or (at your option)
 any later version.

 Hello is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for
 more details.

 You should have received a copy of the GNU Affero General Public License
 along with Hello.  If not, see <http://www.gnu.org/licenses/>.
\*****************************************************************************/


/* <<-- CHANGE START (main program) -->> */
// include header files
#include "ServiceTools.h"
#include <ctime>
#include <libgen.h>
#include <fstream>
#include <sstream>
#include <string>

#include <pnapi/pnapi.h>
#include "Output.h"
#include "verbose.h"


/// check if a file exists and can be opened for reading
inline bool fileExists(const std::string& filename) {
    std::ifstream tmp(filename.c_str(), std::ios_base::in);
    return tmp.good();
}


bool isControllable(pnapi::PetriNet &net, std::string &outputFile, bool useWendyOptimization) {
    
    std::string wendyCommand("wendy --correctness=livelock ");
    if (useWendyOptimization) {
        wendyCommand+= " --waitstatesOnly --receivingBeforeSending --seqReceivingEvents   --succeedingSendingEvent  --quitAsSoonAsPossible ";
    }
    wendyCommand+=" --sa="+outputFile;
    
//    message("creating a pipe to wendy by calling '%s'", wendyCommand.c_str());

    // create stringstream to store the open net
    std::stringstream ss;

    // ss << pnapi::io::lola << *(net) << std::flush;
    ss << pnapi::io::owfn << net << std::flush;
      
    // call wendy and open a pipe
    FILE* fp = popen(wendyCommand.c_str(), "w");

    // send the net to wendy
    fprintf(fp, "%s", ss.str().c_str());

    // close the pipe
    pclose(fp);

    if(!fileExists(outputFile)) {
        return false;
    }
    return true;
}


void computeOG(pnapi::PetriNet &net, std::string outputFile) {
    
    std::string wendyCommand("wendy --correctness=livelock ");
    wendyCommand+=" --og="+outputFile;
    
    // create stringstream to store the open net
    std::stringstream ss;

    // ss << pnapi::io::lola << *(net) << std::flush;
    ss << pnapi::io::owfn << net << std::flush;
      
    // call wendy and open a pipe
    FILE* fp = popen(wendyCommand.c_str(), "w");

    // send the net to wendy
    fprintf(fp, "%s", ss.str().c_str());

    // close the pipe
    pclose(fp);

}


/**
This function calls lola-statespace with the given net and returns a file pointer to the state automaton
of the inner graph.
The second argument  is used as temporary filename.
 */
void getLolaStatespace(pnapi::PetriNet &net, std::string &tempFileName) {

    std::string command="lola-statespace -m "; //TODO: as cmd-param

    command+=tempFileName;

    time_t start_time;
    time_t end_time;

    // call lola
    message("creating a pipe to lola by calling '%s'", command.c_str());
    {
        // set start time
        time(&start_time);
        // create stringstream to store the open net
        std::stringstream ss;

        // ss << pnapi::io::lola << *(net) << std::flush;
        ss << pnapi::io::lola << net << std::flush;
      
        // call lola and open a pipe
        FILE* fp = popen(command.c_str(), "w");
        // send the net to lola

	fprintf(fp, "%s", ss.str().c_str());

        // close the pipe
        pclose(fp);
        // set end time
        time(&end_time);
    }

    // status message
    status("lola is done [%.0f sec]", difftime(end_time, start_time)); 

    // DEBUG: write lola output to cout
    /*char c;
    FILE* f=fopen(lolaFN.c_str(),"r");
    while ((c=fgetc(f)) != EOF) {
      cout << c;
    }*/

}
