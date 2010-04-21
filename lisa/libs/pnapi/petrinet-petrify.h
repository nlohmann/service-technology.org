#ifndef PETRINETPETRIFY_H
#define PETRINETPETRIFY_H

#include <map>
#include <set>
#include <string>

#include "parser-petrify.h"

using std::map;
using std::set;
using std::string;

/// input file
extern FILE *pnapi_petrify_yyin;
/// actual parsing function
extern int pnapi_petrify_yyparse();

set<string> pnapi_petrify_transitions;
set<string> pnapi_petrify_places;
set<string> pnapi_petrify_initialMarked;
set<string> pnapi_petrify_interface;
map<string, set<string> > pnapi_petrify_arcs;


#endif /* PETRINETPETRIFY_H */
