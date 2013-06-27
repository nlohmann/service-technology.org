#include "CCSearch.h"

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <iostream>
#include <fstream>
#include <sstream>

#include "tinythread.h"
#include "verbose.h"

#include <stdio.h>
#include <vector>
#include <time.h>

/// little help struct
struct bruch {
    int z;
    int n;
};

volatile unsigned int CCSearch::upper=0;
volatile unsigned int CCSearch::lower=0;

void CCSearch::setBounds(unsigned int newLower,unsigned int newUpper) {
    upper=newUpper; lower=newLower;
}

void CCSearch::threadFunction(void* args) {

    // thread may be called only this way,
    // otherwise seg-fault
    bruch* b=(bruch*) args;
    int zaehler=b->z;
    int nenner=b->n;
    while(abs(upper-lower)>0) {
        unsigned int t=(lower)+zaehler*((upper)-(lower))/nenner;
        bool lessEq=isLessEq(t);
        // here it is important, that upper and lower are volatile
        // otherwise caching would cause pain in the ass
        if(lessEq && upper>t) upper=t;
        if(!lessEq && lower<=t) lower=t+1;

        status("CCSearch: new bounds: %d and %d \n", lower,upper);
        // maybe nothing happens, if another thread found a better bound faster
    }
}

unsigned int CCSearch::search() {

    // determine the optimal number of processes
    unsigned int prozesse;
    if(Tara::args_info.concurrency_given && Tara::args_info.concurrency_arg > 0)
        prozesse=Tara::args_info.concurrency_arg;
    else
        prozesse=tthread::thread::hardware_concurrency();

    // if the number may not be defined, use 1..
    if(prozesse==0) prozesse=1;

    status("CCSearch: #threads: %d \n", prozesse);

    // TODO: set this value by a command line parameter

    //create the processes and assign them their fract
    std::vector<tthread::thread*> pv(prozesse);
    for(unsigned int i=1;i<=prozesse;++i) {
        bruch* b=new bruch();
        b->z=i;
        b->n=prozesse+1;
        pv[i-1]= new tthread::thread(&CCSearch::threadFunction,b);
    }
    // wait for all processes to be ready...
    for(std::vector<tthread::thread*>::iterator it=pv.begin();it!=pv.end();++it) {
        (*it)->join();
    }

    // this should never ever happen
    if(upper!=lower)
        printf("concurrent search failed, no exact result... oh crap ");
    
    return lower;
}

// BEGIN: most of this should be in service tools file

/// check if a file exists and can be opened for reading
inline bool fileExists(const std::string& filename) {
    std::ifstream tmp(filename.c_str(), std::ios_base::in);
    return tmp.good();
}
bool CCSearch::isLessEq(unsigned int x) {
    static tthread::mutex m;

    //only one may change the net, so lock the mutex
    m.lock();
    Tara::modification->setToValue(x);

    std::stringstream ss("");
    ss << pnapi::io::owfn << (*Tara::net) << std::flush;

    // next one may mod the net...
    m.unlock();
   
    std::string tempFile=Output().name();
    std::string wendyCommand("wendy --correctness=livelock ");
    wendyCommand+= " --waitstatesOnly --receivingBeforeSending --seqReceivingEvents   --succeedingSendingEvent  --quitAsSoonAsPossible ";
    wendyCommand+=" --sa="+tempFile;

    // call wendy and open a pipe
    FILE* fp = popen(wendyCommand.c_str(), "w");

    // send the net to wendy
    fprintf(fp, "%s", ss.str().c_str());

    // close the pipe
    pclose(fp);

    if(!fileExists(tempFile)) {
       return false;
    }
    return true;
}

// END OF: this should be in serviceTools.cc
