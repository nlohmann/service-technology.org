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


using std::set;
using std::vector;

#define MIN_CALL popen("cat tmp.obj tmp.stub tmp.events tmp.known tmp.decl | lp_solve -S1 ","r");
#define MAX_CALL popen("cat tmp.obj tmp.stub tmp.events tmp.known tmp.decl | lp_solve -S1 -max","r");

const std::string intToStr(const int);

extern int NR_OF_EVENTS;
extern std::string* EVENT_STRINGS;
extern pnapi::Place** EVENT_PLACES;

extern int GET_EVENT_ID(std::string* s);
#endif /* HELPERS_H */
