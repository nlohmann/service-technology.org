#include <Runtime.h>
#include <cstdio>

Runtime rt;

int main(int argc, char** argv) {
    rt.arguments(argc, argv);

    rt.status("Los gehts!");
    rt["mood"] = "good";

    FILE *o = fopen("bar", "r");
    if (o == NULL) {
        rt.error(1, "IO error");
    }

    return 0;
}
