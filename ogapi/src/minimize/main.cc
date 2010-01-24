#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "cmdline.h"
#include "Graph.h"
#include "verbose.h"

// lexer and parser
extern int og_yyparse();
extern FILE *og_yyin;
extern Graph G;

// gengetopt cmdline parser
gengetopt_args_info args_info;

void evaluateParameters(int argc, char** argv) {
    // set default values
    cmdline_parser_init(&args_info);

    // initialize the parameters structure
    struct cmdline_parser_params *params = cmdline_parser_params_create();

    // call the cmdline parser
    cmdline_parser(argc, argv, &args_info);

    // debug option
    /*if (args_info.bug_flag) {
        FILE *debug_output = fopen("bug.log", "w");
        fprintf(debug_output, "%s\n", CONFIG_LOG);
        fclose(debug_output);
        fprintf(stderr, "Please send file 'bug.log' to %s.\n", PACKAGE_BUGREPORT);
        exit(EXIT_SUCCESS);
    }*/

    free(params);
}

int main(int argc, char **argv)
{
  status("evaluating parameters");
  evaluateParameters(argc, argv);

  status("deciding input option (stdin|file)");
  if (args_info.inputs_num > 0)
    og_yyin = fopen(args_info.inputs[0], "r");
  else
    og_yyin = stdin;

  status("parsing input in progress");
  og_yyparse();
  if (args_info.inputs_num > 0)
    fclose(og_yyin);
  status("parsing input done");

  status("minimizing graph in progress");
  G.minimize();
  status("minimizing graph done");
  if (args_info.output_given)
  {
    char *filename;
    if (strcmp(args_info.inputs[0], ""))
      filename = strcat(strdup(args_info.inputs[0]), ".minimized.og");
    else
      filename = strdup(args_info.inputs[0]);
    FILE *output = fopen(filename, "w");
    G.ogOut(output);
    fclose(output);
    delete filename;
  }
  else
    G.ogOut();

  return EXIT_SUCCESS;
}
