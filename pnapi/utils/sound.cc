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

//        try {
//            i >> pnapi::io::owfn >> net;
//        } catch(pnapi::exception::InputError e) {
//            i.seekg(0, std::ios::beg);
            i >> pnapi::io::lola >> net;
//        }
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

    // display help
    std::cout << "all:\n\t@echo \"Valid targets are quasiliveness, boundedness, and clean.\"\n\n";

    // check 1: quasi-liveness
    std::cout << "quasiliveness: $(transitions:%=%.quasiliveness.result)\n\n";
    std::cout << "%.quasiliveness.result:\n";
    std::cout << "\t@echo \"ANALYSE TRANSITION $(@:%.quasiliveness.result=%)\" > $(@:%.result=%.task)\n";
    std::cout << "\t-@lola-deadtransition $(net) -a$(@:%.result=%.task) &> $@ ; echo \"RESULT: $$?\" >> $@\n\n";

    // check 2: boundedness (transitions)
    std::cout << "boundedness: $(places:%=%.boundedness.result)\n\n";
    std::cout << "%.boundedness.result:\n";
    std::cout << "\t@echo \"ANALYSE PLACE $(@:%.boundedness.result=%)\" > $(@:%.result=%.task)\n\n";

    // check 2: boundedness (whole net)
    std::cout << "boundednessnet: boundednessnet.result\n\n";
    std::cout << "boundednessnet.result:\n";
    std::cout << "\t-@lola-boundednet $(net) &> $@ ; echo \"RESULT: $$?\" >> $@\n\n";

    // check for quasiliveness by findpath
    std::cout << "quasilivenessfindpath: $(transitions:%=%.findpath.result)\n";
    std::cout << "\t-@grep -L \"RESULT: 0\" *.findpath.result | sed -e 's/.findpath.result//' | xargs\n\n";

    PNAPI_FOREACH(t, net.getTransitions()) {
         std::cout << (*t)->getName() << ".findpath.result:\n\t@echo \"FORMULA (";
         bool first = true;
         PNAPI_FOREACH(n, (*t)->getPreset()) {
             if (not first) {
                 std::cout << " AND";
             } else {
                 first = false;
             }
             std::cout << " " << (*n)->getName() << " > 0";
         }
         std::cout << " )\" > $(@:%.result=%.task)\n";
         std::cout << "\t-@lola-findpath $(net) -a$(@:%.result=%.task) &> $@ ; echo \"RESULT: $$?\" >> $@\n";
    }

    // clean up
    std::cout << "\nclean:\n";
    std::cout << "\t@rm -f boundednessnet.result\n";
    std::cout << "\t@rm -f $(transitions:%=%.quasiliveness.result)\n";
    std::cout << "\t@rm -f $(places:%=%.boundedness.result)\n";
    std::cout << "\t@rm -f $(transitions:%=%.quasiliveness.task)\n";
    std::cout << "\t@rm -f $(transitions:%=%.findpath.result)\n";
    std::cout << "\t@rm -f $(transitions:%=%.findpath.task)\n";
    std::cout << "\t@rm -f $(places:%=%.boundedness.task)\n";
    std::cout << "\n";

    return 0;
}
