#include <ctime>
#include <fstream>
#include <libconfig.h++>
#include "output_syntax.h"
#include "config.h"

using namespace libconfig;

extern void output_parse();
extern FILE *output_in;

Config *cfg;

int main(int argc, char **argv) {
    time_t t1, t2;
    cfg = new Config();

    if (argc == 2 && !strcmp(argv[1], "--version")) {
        printf("LoLA Result File Generator\n");
        printf("  part of %s\n\n", PACKAGE_STRING);
        return EXIT_SUCCESS;
    }

    if (argc == 2 && !strcmp(argv[1], "--help")) {
        printf("call %s < FILE\n", argv[0]);
        return EXIT_SUCCESS;
    }

    time(&t1);
    output_parse();
    time(&t2);
    printf("parsing input:  %.2f\n", difftime(t2, t1));

    time(&t1);
    cfg->writeFile("foo.cfg");
    time(&t2);
    printf("writing output: %.2f\n", difftime(t2, t1));

    return 0;
}
