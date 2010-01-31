#include <fstream>
#include <libconfig.h++>
#include "output_syntax.h"

using namespace libconfig;

extern void output_parse();
extern FILE *output_in;

Config cfg;

int main(int argc, char **argv) {
    //Setting &deadlock = cfg.getRoot().add("deadlock", Setting::TypeGroup);

    output_parse();
    cfg.writeFile("foo.cfg");

    return 0;
}
