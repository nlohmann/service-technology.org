#include <config.h>
#include <cstdlib>
#include <fstream>
#include <vector>
#include <string>
#include <set>
#include "pnapi.h"

using namespace pnapi;

int main(int argc, char** argv) {
    PetriNet net;
    std::vector<Transition*> path;
    std::map<Place*, unsigned int> marking;

    // read net
    std::ifstream i;
    i.open(argv[1], std::ios_base::in);
    i >> pnapi::io::lola >> net;
    i.close();

    // read path
    std::string line;
    bool first = true;
    i.open(argv[2], std::ios_base::in);
    while(std::getline(i, line)) {
        if (first) {
            first = false;
            continue;
        }

        Transition *t = net.findTransition(line);
        path.push_back(t);
    }
    i.close();
    std::cerr << "parsed path of length " << path.size() << "\n";

    // set initial marking
    PNAPI_FOREACH(p, net.getPlaces()) {
        marking[*p] = (*p)->getTokenCount();
    }

    // process path
    PNAPI_FOREACH(t, path) {
        std::cerr << (*t)->getName() << " ";
        
        std::set<Node*> cluster_nodes = net.getConflictCluster(**t);
        std::set<Node*> cluster_transitions;
        PNAPI_FOREACH(ct, cluster_nodes) {
            if (dynamic_cast<Transition *>(*ct) != NULL) {
                cluster_transitions.insert(*ct);
            }
        }

        std::cerr << "cluster size: " << cluster_transitions.size() << "\n";
    }

    return 0;
}
