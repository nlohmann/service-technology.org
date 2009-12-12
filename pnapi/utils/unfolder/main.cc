#include <cstdarg>
#include <cstdio>
#include <cstdlib>

extern int hlowfn_parse();
extern FILE *hlowfn_out;

extern int llowfn_parse();
extern FILE *llowfn_in;
extern FILE *llowfn_out;

FILE *pipe_out;

int main() {

    // parse high-level net and pipe it to LoLA
    pipe_out = popen("lola-none -nfoo", "w");
    hlowfn_out = pipe_out;
    hlowfn_parse();
    pclose(pipe_out);

    // parse LoLA's output and pipe it to standard output
    llowfn_out = stdout;
    llowfn_in = fopen("foo", "r");
    llowfn_parse();
    fclose(llowfn_in);

    return EXIT_SUCCESS;
}
