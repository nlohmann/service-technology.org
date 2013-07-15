#include <Runtime.h>
#include <unistd.h>

Runtime rt;

int main(int argc, char** argv) {
    rt.arguments(argc, argv);
    
    rt.status("I like %d turtles", 3);
    rt.status("Welt");

    return 0;
}
