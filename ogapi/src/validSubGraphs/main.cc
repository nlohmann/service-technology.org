#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <ctime>
#include "cmdline.h"
#include "Graph.h"
#include "verbose.h"
#include "Output.h"
#include "helpers.h"

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
  evaluateParameters(argc, argv);
  Output::setTempfileTemplate(args_info.tmpfile_arg);
  Output::setKeepTempfiles(args_info.noClean_flag);


  if (args_info.inputs_num > 0)
    og_yyin = fopen(args_info.inputs[0], "r");
  else
    og_yyin = stdin;

  status("parsing input in progress");
  og_yyparse();
  if (args_info.inputs_num > 0)
    fclose(og_yyin);
  status("parsing input done");

//  status("counting valid subgraphs...");
//  int count = G.countValidSubgraphs();
//  message("overapproximation: %u valid subgraphs.", count);

//  G.printFormulas();
//  G.printFormulasCNF();
//  G.printFormulasDNF();

  G.graphformula.settime(args_info.timebound_arg);

  /// a variable holding the time of the call
  clock_t start_clock = clock();

  G.graphformula.cnfClauses = G.calculateGraphFormula(args_info.noCycles_flag);

  double duration = (double)(clock()-start_clock) / CLOCKS_PER_SEC;

//  G.graphformula.printClauseList(G.graphformula.cnfClauses);
//  G.graphformula.printClauseListExpr(G.graphformula.cnfClauses);

  // CNF output
  if (args_info.formulaoutput_given) {
	  std::string filename = (args_info.inputs_num) ? args_info.inputs[0] : "validateSubGraph_formulaoutput";

	  std::string bool_filename = args_info.output_arg ? args_info.output_arg : filename + ".bool";
	  Output output(bool_filename, "boolean formula");
	  G.graphformula.booleanOut(output, args_info.inputs[0], G.graphformula.cnfClauses, &(G.cycles));
  }

  // CNF output
  if (args_info.output_given) {
    std::string filename = (args_info.inputs_num) ? args_info.inputs[0] : "validateSubGraph_output";

    std::string cnf_filename = args_info.output_arg ? args_info.output_arg : filename + ".cnf";
    Output output(cnf_filename, "conjunctive normal form");
    G.graphformula.cnfOut(output, args_info.inputs[0], duration, args_info.noCycles_flag, &(G.cycles));
  }

  return EXIT_SUCCESS;
}
