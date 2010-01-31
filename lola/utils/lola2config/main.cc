#include <fstream>
#include <libconfig.h++>
#include "output_syntax.h"
#include "config.h"

using namespace libconfig;

extern void output_parse();
extern FILE *output_in;

Config cfg;

int main(int argc, char **argv) {
    //Setting &deadlock = cfg.getRoot().add("deadlock", Setting::TypeGroup);

    if (argc == 2 && !strcmp(argv[1], "--version")) {
        printf("LoLA Result File Generator\n");
        printf("  part of %s\n\n", PACKAGE_STRING);
        return EXIT_SUCCESS;
    }

    if (argc == 2 && !strcmp(argv[1], "--help")) {
        printf("call %s < FILE\n", argv[0]);
        return EXIT_SUCCESS;
    }

    output_parse();
    cfg.writeFile("foo.cfg");

    return 0;
}
