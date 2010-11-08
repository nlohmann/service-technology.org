
#ifndef SARALIB_H
#define SARALIB_H

#include <vector>
#include <map>
#include "pnapi/pnapi.h"

using std::vector;
using std::map;
using pnapi::PetriNet;
using pnapi::Marking;
using pnapi::Place;
using pnapi::Transition;

namespace sara {

vector<Transition*> ReachabilityTest(PetriNet& pn, Marking& m0, Marking& mf, map<Place*,int>& cover);
vector<Transition*> RealizabilityTest(PetriNet& pn, Marking& m0, map<Transition*,int>& parikh);
void setVerbose();

}

#endif
