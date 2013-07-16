#include <Runtime.h>

int main(int argc, char** argv) {
    rt.arguments(argc, argv);

    rt.message("Los gehts!");
    rt["mood"] = "good";

    return 0;
}
