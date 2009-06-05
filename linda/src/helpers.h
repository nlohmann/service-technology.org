#ifndef LINDA_HELPERS_H
#define LINDA_HELPERS_H

#include <sstream>
#include <iostream>
#include <fstream>
#include <set>
#include <vector>

#include "config.h"
#include <pnapi/pnapi.h>

#include "lp_lib.h"

#include "bounds.h"
#include "messageprofile.h"

using std::set;
using std::vector;

#define MIN_CALL popen("cat tmp.obj tmp.stub tmp.events tmp.known tmp.decl | lp_solve -S1 ","r");
#define MAX_CALL popen("cat tmp.obj tmp.stub tmp.events tmp.known tmp.decl | lp_solve -S1 -max","r");

const std::string intToStr(const int);
std::string getText(FILE*);

typedef set<pnapi::Place* >  PPS;
typedef vector<pnapi::Place* >  PPV;
typedef Bounds<pnapi::Place* > EventBound;
typedef Bounds<PPS> EventSetBound;
typedef Bounds<PPV> EventVectorBound;
typedef vector<EventBound>  EBV;

void findMaxDependenciesR(EBV candidate, vector<EventSetBound>& results, set<PPS>& alreadyTested, lprec *lp, std::map<EVENT,unsigned int> EventID);


#endif /* HELPERS_H */
