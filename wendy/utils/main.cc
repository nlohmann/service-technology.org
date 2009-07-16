#include <cstdlib>
#include "Graph.h"

// lexer and parser
extern int og_yyparse();
extern Graph G;

int main() {    
    og_yyparse();

    G.minimize();
    G.ogOut();

    return EXIT_SUCCESS;
}
