#include <Runtime.h>
#include <unistd.h>

Runtime rt;

int main() {
    rt.status("I like %d turtles", 3);
    rt.status("Welt");

    char *foo;
    rt.status("%s", foo);

    return 0;
}
