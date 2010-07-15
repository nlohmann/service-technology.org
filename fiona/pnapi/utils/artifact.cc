#include <config.h>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include "pnapi.h"

using namespace pnapi;

int main() {
    std::vector<std::string> nets;
    nets.push_back("cup");
    nets.push_back("coin");
    nets.push_back("state");

    PetriNet all;

    PNAPI_FOREACH(netname, nets){
        PetriNet n;
        std::ifstream infile(std::string(*netname + ".owfn").c_str(), std::ifstream::in);
        infile >> io::owfn >> n;
        n.prefixNames(*netname + ".");
        all.compose(n, "", "");
    }

    PetriNet policy;
    std::ifstream policy_file("policy.owfn", std::ifstream::in);
    policy_file >> io::owfn >> policy;
    all.produce(policy, "net", "policy");

    std::cout << io::owfn << all;

    return EXIT_SUCCESS;
}
