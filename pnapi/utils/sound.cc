#include <config.h>
#include <fstream>
#include "pnapi.h"
#include "verbose.h"

using namespace pnapi;

int main(int argc, char** argv) {
    PetriNet net;

    {
        std::ifstream i;
        i.open(argv[1], std::ios_base::in);

        try {
            i >> pnapi::io::owfn >> net;
        } catch(pnapi::exception::InputError e) {
            i.seekg(0, std::ios::beg);
            i >> pnapi::io::lola >> net;
        }
    }

    std::cout << ".PHONY : clean\n\n";
    std::cout << "net = " << argv[1] << "\n";
    std::cout << "transitions = ";
    PNAPI_FOREACH(t, net.getTransitions()) {
         std::cout << (*t)->getName() << " ";
    }
    std::cout << "\nplaces = ";
    PNAPI_FOREACH(p, net.getPlaces()) {
         std::cout << (*p)->getName() << " ";
    }
    std::cout << "\n\n";

    // check 1: quasi-liveness
    std::cout << "quasiliveness: $(transitions:%=%.quasiliveness.result)\n\n";
    std::cout << "%.quasiliveness.result:\n";
    std::cout << "\t@echo \"ANALYSE TRANSITION $(@:%.quasiliveness.result=%)\" > $(@:%.result=%.task)\n\n";

    // check 2: boundedness
    std::cout << "boundedness: $(places:%=%.boundedness.result)\n\n";
    std::cout << "%.boundedness.result:\n";
    std::cout << "\t@echo \"ANALYSE PLACE $(@:%.boundedness.result=%)\" > $(@:%.result=%.task)\n\n";

    // clean up
    std::cout << "clean:\n";
    std::cout << "\t@rm -f $(transitions:%=%.quasiliveness.result)\n";
    std::cout << "\t@rm -f $(places:%=%.boundedness.result)\n";
    std::cout << "\t@rm -f $(transitions:%=%.quasiliveness.task)\n";
    std::cout << "\t@rm -f $(places:%=%.boundedness.task)\n";
    std::cout << "\n";

    return 0;
}
