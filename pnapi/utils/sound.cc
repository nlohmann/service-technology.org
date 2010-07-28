#include <config.h>
#include <cstdlib>
#include <fstream>
#include "pnapi.h"

using namespace pnapi;
using pnapi::io::util::operator<<; // to output final conditions

int main(int argc, char** argv) {
    PetriNet net;

    if (argc < 2) {
        fprintf(stderr, "no input given\n");
        exit(EXIT_FAILURE);
    }

    fprintf(stderr, "transforming net\n");
    {
        std::ifstream i;
        i.open(argv[1], std::ios_base::in);
        i >> pnapi::io::owfn >> net;
        std::ofstream o;
        o.open((std::string(argv[1]) + ".lola").c_str(), std::ios_base::trunc);
        o << pnapi::io::lola << net;
    }

    fprintf(stderr, "creating nets for relaxed soundness\n");
    PNAPI_FOREACH(t, net.getTransitions()) {
        PetriNet relaxedNet(net);
        Place& p1 = relaxedNet.createPlace();
        Place& p2 = relaxedNet.createPlace();
        Transition* told = relaxedNet.findTransition((*t)->getName());
        Transition& tnew = relaxedNet.createTransition();
        assert(told);

        PNAPI_FOREACH(p, told->getPreset()) {
            relaxedNet.createArc(**p, tnew);
        }
        PNAPI_FOREACH(p, told->getPostset()) {
            relaxedNet.createArc(tnew, **p);
        }

        p1.setTokenCount(1);
        relaxedNet.createArc(p1, *told);
        relaxedNet.createArc(*told, p2);
        relaxedNet.createArc(p2, tnew);
        relaxedNet.createArc(tnew, p2);

        std::ofstream o;
        o.open((std::string(argv[1]) + "." + (*t)->getName() + ".lola").c_str(), std::ios_base::trunc);
        o << pnapi::io::lola << relaxedNet;
        o << "\n\nFORMULA (" << pnapi::io::lola << net.getFinalCondition() << " AND " << p2.getName() << " = 1 )\n";
    }



    // write the Makefile
    fprintf(stderr, "creating Makefile\n");

    std::cout << ".PHONY : clean veryclean\n\n";
    std::cout << "SED = sed\n\n";
    std::cout << "net = " << argv[1] << ".lola\n";
    std::cout << "transitions = ";
    PNAPI_FOREACH(t, net.getTransitions()) {
        std::cout << (*t)->getName() << " ";
    }
    std::cout << "\nplaces = ";
    PNAPI_FOREACH(p, net.getPlaces()) {
        std::cout << (*p)->getName() << " ";
    }
    std::cout << "\nfinal = \"" << pnapi::io::lola << net.getFinalCondition() << "\"\n\n";

    // display help
    std::cout << "all:\n\t@echo \"Valid targets are quasiliveness, boundedness, and clean.\"\n\n";


    // quasi-liveness using deadtransition
    std::cout << "\n##############################################################################\n";
    std::cout << "# check for quasi-liveness by checking whether any transition is dead\n";
    std::cout << "##############################################################################\n";
    std::cout << "quasiliveness: $(transitions:%=%.quasiliveness.result)\n";
//    std::cout << "\t-@grep -L \"RESULT: 0\" *.quasiliveness.result | tr '\\n' ',' | $(SED) 's/.$$//' | $(SED) 's/,/, /g'\n\n";
    std::cout << "%.quasiliveness.result:\n";
    std::cout << "\t@echo \"ANALYSE TRANSITION $(@:%.quasiliveness.result=%)\" > $(@:%.result=%.task)\n";
    std::cout << "\t-@lola-deadtransition $(net) -a$(@:%.result=%.task) > $@ ; echo \"RESULT: $$?\" >> $@\n\n";


    // quasi-liveness using findpath
    std::cout << "\n##############################################################################\n";
    std::cout << "# check for quasi-liveness by finding a path to enable every transition\n";
    std::cout << "##############################################################################\n";
    std::cout << "quasilivenessfindpath: $(transitions:%=%.findpath.result)\n";
//    std::cout << "\t-@grep -L \"RESULT: 0\" *.findpath.result | $(SED) -e 's/.findpath.result//' | xargs\n\n";

    PNAPI_FOREACH(t, net.getTransitions()) {
        std::cout << (*t)->getName() << ".findpath.result:\n\t@echo \"FORMULA (";
        bool first = true;
        PNAPI_FOREACH(n, (*t)->getPreset()) {
            if (not first) {
                std::cout << " AND";
            } else {
                first = false;
            }
            Arc* a = net.findArc(**n, **t);
            assert(a);
            std::cout << " " << (*n)->getName() << " >= " << a->getWeight();
        }
        std::cout << " )\" > $(@:%.result=%.task)\n";
        std::cout << "\t-@lola-findpath $(net) -a$(@:%.result=%.task) > $@ ; echo \"RESULT: $$?\" >> $@\n";
    }
    std::cout << "\n";


    // liveness using liveprop
    std::cout << "\n##############################################################################\n";
    std::cout << "# check for liveness of the final marking\n";
    std::cout << "##############################################################################\n";
    std::cout << "liveness: liveness.result\n";
//    std::cout << "\t-@grep -q \"RESULT: 0\" liveness.result\n\n";
    std::cout << "liveness.result:\n";
    std::cout << "\t@echo \"FORMULA \"$(final) > $(@:%.result=%.task)\n";
    std::cout << "\t-@lola-liveprop $(net) -a$(@:%.result=%.task) -S > $@ ; echo \"RESULT: $$?\" >> $@\n\n";

    // boundedness using boundedplace
    std::cout << "\n##############################################################################\n";
    std::cout << "# check for boundedness by checking each place\n";
    std::cout << "##############################################################################\n";
    std::cout << "boundedness: $(places:%=%.boundedness.result)\n";
//    std::cout << "\t-@grep -L \"RESULT: 1\" *.boundedness.result | tr '\\n' ',' | $(SED) 's/.$$//' | $(SED) 's/,/, /g'\n\n";
    std::cout << "%.boundedness.result:\n";
    std::cout << "\t@echo \"ANALYSE PLACE $(@:%.boundedness.result=%)\" > $(@:%.result=%.task)\n";
    std::cout << "\t@-lola-boundedplace $(net) -a$(@:%.result=%.task) > $@ ; echo \"RESULT: $$?\" >> $@\n\n";

    // boundedness using boundednet
    std::cout << "\n##############################################################################\n";
    std::cout << "# check for boundedness by checking the whole net\n";
    std::cout << "##############################################################################\n";
    std::cout << "boundednessnet: boundednessnet.result\n\n";
    std::cout << "boundednessnet.result:\n";
    std::cout << "\t-@lola-boundednet $(net) > $@ ; echo \"RESULT: $$?\" >> $@\n\n";

    // relaxed soundness
    std::cout << "\n##############################################################################\n";
    std::cout << "# check relaxed soundness by checking special reachability problem\n";
    std::cout << "##############################################################################\n";
    std::cout << "relaxed: $(transitions:%=%.relaxed.result)\n";
//    std::cout << "\t-@grep -L \"RESULT: 0\" *.relaxed.result | tr '\\n' ',' | $(SED) 's/.$$//' | $(SED) 's/,/, /g'\n\n";
    std::cout << "%.relaxed.result:\n";
    std::cout << "\t-@lola-statepredicate $(net:.lola=).$(@:%.relaxed.result=%).lola > $@ ; echo \"RESULT: $$?\" >> $@\n\n";

    // clean up
    std::cout << "\n##############################################################################\n";
    std::cout << "# clean up\n";
    std::cout << "##############################################################################\n";
    std::cout << "clean:\n";
    std::cout << "\t@$(RM) boundednessnet.result liveness.result liveness.task\n";
    std::cout << "\t@$(RM) $(transitions:%=%.quasiliveness.result)\n";
    std::cout << "\t@$(RM) $(places:%=%.boundedness.result)\n";
    std::cout << "\t@$(RM) $(transitions:%=%.quasiliveness.task)\n";
    std::cout << "\t@$(RM) $(transitions:%=%.findpath.result)\n";
    std::cout << "\t@$(RM) $(transitions:%=%.findpath.task)\n";
    std::cout << "\t@$(RM) $(transitions:%=%.relaxed.result)\n";
    std::cout << "\t@$(RM) $(places:%=%.boundedness.task)\n";
    std::cout << "\nveryclean: clean\n";
    std::cout << "\t@$(RM) $(net) $(MAKEFILE_LIST)\n";
    std::cout << "\t@$(RM) $(net:.lola=).*.lola\n";

    return EXIT_SUCCESS;
}
